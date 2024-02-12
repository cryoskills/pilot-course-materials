#include <cryo_sleep.h>
#include <cryo_adc.h> // see documentation for cryo_sleep and cryo_adc here: https://github.com/cryoskills/sensor-kit-libraries/tree/main/src
#include <DS18B20.h>  // open-source library for using the DS temperature sensor: https://github.com/matmunk/DS18B20
#include <SPI.h> 
#include <SD.h>  // for logging data to the SD card (SPI and SD are both included in the standard Arduino setup)

/*
Code for running the cryoskills sensor kit: taking temperature measurements using a DS18B20 digital probe, and PS1000 analog probe.
Special fill-in-the-blanks edition: write your own code in the gaps.
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

// initialise some variables

//--------- Your code here! -----------

// define some parameters we already know (like resistances and voltages)

//--------- Your code here! -----------

//create probe object. Specify the pins we are connected to, and think about which gain and averaging values will give us the best result
// see the cryo_adc docs: https://github.com/cryoskills/sensor-kit-libraries/tree/main/src

//--------- Your code here! -----------

// now we have our setup loop. We need to configure the clock, set up our alarms, open the serial connection, set an output pin, and set up the SD card.
void setup() {
  cryo_configure_clock();
  cryo_add_alarm_every(5, take_measurements);  //set alarm to take measurements every x seconds
  //open serial connection

  //--------- Your code here! -----------
  
  //use the cryo_adc library to initialise our probe

  //--------- Your code here! -----------
 
 //set pin 6 to output current to our circuit

 //--------- Your code here! -----------

 // set up the sd card, using our function defined below
  setup_sd();
}

// this is our main loop, which will run repeatedly. We need to wake our cryo-child up, check for alarms, then send him back to sleep.
void loop() {
  cryo_wakeup_debug();  // use cryo_wakeup_debug() for testing, and cryo_wakeup() for deployment.
  cryo_raise_alarms();  // check if it's time to take measurements, and do that
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

//--------- Your code here! -----------

  return ds_temp;
}

float_t get_adc_temp(){
  //switch pin 6 on, so we have the voltage from the controller across our circuit 

  //--------- Your code here! -----------

  // enable adc to take measurement

  //--------- Your code here! -----------
  
  // take reading

  //--------- Your code here! -----------

  // disable adc

  //--------- Your code here! -----------

  //switch pin 6 off again

  //--------- Your code here! -----------

  // convert reading to voltage
  // (the range of readings is from 0 to 32768, and the range of voltages we can measure is 0 to 1)

  //--------- Your code here! -----------

  //convert voltage to temperature. Cast your mind back to high school maths class...
  // n.b., remember the gain!

  //--------- Your code here! -----------

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
 char filename[12];       
 strcpy(filename, "logfile.txt");  //change this to whatever you like!

  // open the file for writing

  //--------- Your code here! -----------

  //write a header

  //--------- Your code here! -----------

  //set the output pins
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
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

//--------- Your code here! -----------
  
// delay to make sure everything goes smoothly
  delay(100);
}

