#include <cryo_sleep.h>  // see documentation for cryo_sleep and cryo_adc here: https://github.com/cryoskills/sensor-kit-libraries/tree/main/src
#include <DS18B20.h>  // open-source library for using the DS temperature sensor: https://github.com/matmunk/DS18B20 
#include <SPI.h> 
#include <SD.h>  // for logging data to the SD card (SPI and SD are both included in the standard Arduino setup)

/*
Code for running the cryoskills sensor kit: taking temperature measurements using a DS18B20 digital probe, and PS1000 analog probe.
Full version, no gap-filling needed!
*/ 

// let's declare some functions for stuff we need to do in the script
float get_ds18b20_temperature();   //get a digital temperature reading, return it as a float
float get_pt1000_temperature();  // get an analog temperature reading, return it as a float
void setup_sd();   //set up the SD card to log data to a file
void write_to_sd();  // write data to file
void take_measurements();  //this will wrap up all of the measurements and logging in one function, for neatness

#define cardSelect 4  // set the pin used by the SD card

File logfile;  // file for logging data to

// initialise probe object
DS18B20 ds(5); //nb this uses the 'Arduino name' of the pin (D5)

// initialise variables that we'll use later
float ds18b20_temperature; // reading from the DS probe
float RTD_ADC; // Raw ADC reading from the analogue probe
float RTD_Voltage; // ADC reading converted to voltage
float RTD_Resistance; // Voltage converted to resistance
float RTD_Temp; // Resistance converted to temperature
float pt1000_temperature;    //resistance converted to temp!

// define some parameters we already know
float V_in = 3.2;  //input voltage
const int R1 = 3270; // R1 is a fixed resistor equal to ~3300 Ohms... but is it exactly 3300?

const int ResistorPin = 11; // R1 is connected to D11
const int PT1000Pin = A1; // PT1000 is connected to A1

// now we have our setup loop. We need to configure the clock, set up our alarms, open the serial connection, set an output pin, and set up the SD card.
void setup() {
  cryo_configure_clock(__DATE__, __TIME__);
  cryo_add_alarm_every(5, take_measurements);
  Serial.begin(9600); //open serial connection
  while(!Serial); // this forces the program to wait until the serial monitor is ready, so we don't miss anything
  pinMode(6, OUTPUT); //set pin 6 to output current to our circuit
  pinMode(ResistorPin, OUTPUT); // Sets the resistor as an output
  pinMode(PT1000Pin, INPUT); // Sets the PT1000 as an input
  setup_sd(); // setup the sd card to log data
}

// this is our main loop, which will run repeatedly. We need to wake our cryo-child up, check for alarms, then send him/her/them back to sleep.
void loop() {
  cryo_wakeup_debug();  // use cryo_wakeup_debug() for testing, and cryo_wakeup() for deployment.
  cryo_raise_alarms();  // 
  cryo_sleep_debug();  //ditto, use debug mode for testing
}

// this is where the business happens: we need to take measurements, and then write them to the SD card by calling our other functions.
void take_measurements(){
  ds18b20_temperature = get_ds18b20_temperature();
  pt1000_temperature = get_pt1000_temperature();
  write_to_sd();
}

//get temperature value from the DS probe, and return it as a float. See the documentation here: https://github.com/matmunk/DS18B20
float get_ds18b20_temperature(){
  while (ds.selectNext()) {
    Serial.print("Digital probe:");
    ds18b20_temperature = ds.getTempC();
    Serial.println(ds18b20_temperature);
    Serial.println();
  }
  return ds18b20_temperature;
}

float get_pt1000_temperature(){

  digitalWrite(ResistorPin, HIGH); // Set the resistor to high
  delay(500); //give it some time to get started

  //RTD_ADC = analogRead(PT1000Pin); // Read the ADC measurement of the PT1000
  RTD_ADC = analogRead(PT1000Pin); // Read the ADC measurement of the PT1000

  Serial.print("ADC: ");
  Serial.println(RTD_ADC); // Prints the ADC measurement to the serial monitor 

  RTD_Voltage = RTD_ADC*(V_in / 1023.0); // convert ADC to voltage
  Serial.print("Voltage: ");
  Serial.println(RTD_Voltage); // Prints the RTD voltage

  RTD_Resistance = (RTD_Voltage/(V_in-RTD_Voltage))*R1; // Calculates the RTD resistance
  Serial.print("RTD resistance: ");
  Serial.println(RTD_Resistance);
  RTD_Temp = (RTD_Resistance - 999.97) / 3.8621; // Calculates the correpsonding RTD temperature

  Serial.print("Analog temperature: ");
  Serial.println(RTD_Temp); // Prints the RTD temperature

  //digitalWrite(ResistorPin, LOW); // Set the resistor to low

  delay(1000); // 1 second delay

  // return an integer
  return pt1000_temperature;  
}

// set up the SD card for writing
void setup_sd() {
//  Serial.println("setting up SD!");
//  if (!SD.begin(cardSelect)) {    //check that SD card is working ok
//     Serial.println("Card init. failed!");
//   }

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
  logfile.println("Time\tDigital probe\tAnalog probe");

  //set the output pins
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
  logfile.print(ds18b20_temperature);
  logfile.print("\t");
  logfile.print(pt1000_temperature);
  logfile.print("\t");
  logfile.println();
  logfile.flush();
  digitalWrite(8, LOW);
  
// delay to make sure everything goes smoothly
  delay(100);
}

