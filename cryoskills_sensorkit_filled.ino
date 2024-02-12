#include <cryo_sleep.h>
#include <cryo_adc.h> // see documentation for cryo_sleep and cryo_adc here: https://github.com/cryoskills/sensor-kit-libraries/tree/main/src
#include <DS18B20.h>  // open-source library for using the DS temperature sensor: https://github.com/matmunk/DS18B20
#include <SPI.h> 
#include <SD.h>  // for logging data to the SD card (SPI and SD are both included in the standard Arduino setup)

/*
Code for running the cryoskills sensor kit: taking temperature measurements using a DS18B20 digital probe, and PS1000 analog probe.
Full version, no gap-filling needed!
*/ 

// let's declare some functions for stuff we need to do in the script
float_t get_ds_temp();   //get a digital temperature reading, return it as a float
float_t get_adc_temp();  // get an analog temperature reading, return it as a float
void setup_sd();   //set up the SD card to log data to a file
void write_to_sd();  // write data to file
void take_measurements();  //this will wrap up all of the measurements and logging in one function, for neatness

#define cardSelect 4  // set the pin used by the SD card

File logfile;  // file for logging data to

// initialise probe object
DS18B20 ds(5); //nb this uses the 'Arduino name' of the pin (D5)

// initialise variables that we'll use later
float_t ds_temp; // reading from the DS probe
int16_t adc_reading; //raw reading from the analog probe   
float_t adc_voltage; //reading converted to voltage  
float_t adc_resistance; //voltage converted to resistanace 
float_t adc_temp;    //resistance converted to temp!
float_t V_1; //voltage on one side of the wheatstone bridge
float_t V_2; //voltage on the other side of the wheatstone bridge

// define some parameters we already know
float_t R_1 = 2970;  //measured R1 resistance
float_t R_2 = 2960;  //measured R2 resistance
float_t R_3 = 995;  //measured R3 resistance
float_t V_in = 3.3;  //input voltage

//create probe object. Specify the pins we are connected to, and think about which gain and averaging values will give us the best result
ADCDifferential adc(ADCDifferential::INPUT_PIN_POS::A1_PIN, ADCDifferential::INPUT_PIN_NEG::A2_PIN, ADCDifferential::GAIN::GAIN_8X, ADCDifferential::AVERAGES::AVG_X1024);

// now we have our setup loop. We need to configure the clock, set up our alarms, open the serial connection, set an output pin, and set up the SD card.
void setup() {
  cryo_configure_clock();
  cryo_add_alarm_every(5, take_measurements);
  Serial.begin(115200); //open serial connection
  //while(!Serial); // this forces the program to wait until the serial monitor is ready, so we don't miss anything
  adc.begin(); //use the cryo_adc library to initialise our probe
  pinMode(6, OUTPUT); //set pin 6 to output current to our circuit
  setup_sd();
}

// this is our main loop, which will run repeatedly. We need to wake our cryo-child up, check for alarms, then send him back to sleep.
void loop() {
  cryo_wakeup_debug();  // use cryo_wakeup_debug() for testing, and cryo_wakeup() for deployment.
  cryo_raise_alarms();  // 
  cryo_sleep_debug();  //ditto, use debug mode for testing
}

// this is where the business happens: we need to take measurements, and then write them to the SD card by calling our other functions.
void take_measurements(){
  ds_temp = get_ds_temp();
  adc_temp = get_adc_temp();
  write_to_sd();
}

//get temperature value from the DS probe, and return it as a float. See the documentation here: https://github.com/matmunk/DS18B20
float_t get_ds_temp(){
  while (ds.selectNext()) {
    Serial.print("Digital probe:");
    ds_temp = ds.getTempC();
    Serial.println(ds_temp);
    Serial.println();
  }
  return ds_temp;
}

float_t get_adc_temp(){
   digitalWrite(6, HIGH); //switch pin 6 on, so we have the voltage from the controller across our circuit 
  // enable adc to take measurement
  adc.enable();
  delay(50); // leave some time for things to happen
  // take reading
  adc_reading = adc.read();
  // disable adc
  adc.disable();
  digitalWrite(6, LOW); //switch pin 6 off again
  // convert reading to voltage
  // the range of readings is from 0 to 32768, and the range of voltages we can measure is 0 to 1
  adc_voltage = (adc_reading/32768.*1);

  // print out our voltage to serial, for even more excitement
  // n.b, remember the gain!
  // n.b.b, think about units, what will give you the most accuracy? Are we working with floats or integers?
  Serial.println("Voltage (mV):");
  Serial.println(adc_voltage*1000/8);

  //convert voltage to temperature. Cast your mind back to high school maths class...
  V_2 = V_in*(R_3/(R_3+R_1));  // find V_2 using the voltage division rule
  V_1 = V_2+adc_voltage/8; // find V_1 using our adc voltage (which is the difference between V_1 and V_2)
  adc_resistance = (V_1*R_1)/(V_in-V_1); //also using voltage division rule, and rearranging for the resistance we want
  adc_temp = (adc_resistance-1000)/3.86;  //convert from resistance to temperature using the calibration curve. Think about how best to approximate this relationship (there are a few ways!)

  // finally, print out our temperature value! 
  Serial.print("Analog probe:");
  Serial.println(adc_temp);
  Serial.println();

  // return an integer
  return adc_temp;  
}

// set up the SD card for writing
void setup_sd() {
  Serial.println("setting up SD!");
  if (!SD.begin(cardSelect)) {    //check that SD card is working ok
     Serial.println("Card init. failed!");
   }

// give the log file a name
 char filename[10];       
 strcpy(filename, "log00.txt");
 for (uint8_t i = 0; i < 100; i++) {  // give it the next available filename
   filename[3] = '0' + i/10;
   filename[4] = '0' + i%10;
   if (!SD.exists(filename)) {
     break;
   }
 }

  // open the file for writing
  logfile = SD.open(filename, FILE_WRITE);
  Serial.print("Writing to "); 
  Serial.println(filename);

  //write a header
  logfile.println("Temperature data from Cryoskills sensor kit, degrees C");
  logfile.println("Digital probe\tAnalog probe");

  //set the output pins
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

// write data to the sd card
void write_to_sd() {
  digitalWrite(8, HIGH);   // set pin 8 to write
  Serial.println("writing!");  // announce that we are writing to the SD card
  PseudoRTC* my_rtc = cryo_get_rtc(); // get time from the RTC, and print that to the logfile
  logfile.printf(
        "%04d-%02d-%02d %02d:%02d:%02d\t",
        my_rtc->year,
        my_rtc->month+1,
        my_rtc->day+1,       
        my_rtc->hour,
        my_rtc->minute,
        my_rtc->second
    );

// print temperature measurements to the log file
  logfile.print(ds_temp);
  logfile.print("\t");
  logfile.print(adc_temp);
  logfile.print("\t");
  logfile.println();
  logfile.flush();
  digitalWrite(8, LOW);
  
// delay to make sure everything goes smoothly
  delay(100);
}

