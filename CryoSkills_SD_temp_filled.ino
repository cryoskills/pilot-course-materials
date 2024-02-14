// Add libraries (SD card and temperature sensor)
#include <SD.h>
#include <SPI.h> 
#include <DS18B20.h>  // open-source library for using the DS temperature sensor: https://github.com/matmunk/DS18B20
#include <OneWire.h>

// let's declare some functions for stuff we need to do in the script
void setup_sd();   //set up the SD card to log data to a file
void write_to_sd();  // write data to file
float_t get_ds_temp();   //get a digital temperature reading, return it as a float

#define cardSelect 4  // set the pin used by the SD card
File logfile;  // file for logging data to

// initialise probe object
DS18B20 ds(5); //nb this uses the 'Arduino name' of the pin (D5)

// initialise variables that we'll use later (temperature)
float_t ds_temp; // reading from the DS probe

// now we have our setup loop. We need open the serial connection and set up the SD card.
void setup() {
  Serial.begin(9600); //open serial connection
  setup_sd();
}


// this is our main loop, which will run repeatedly. 
void loop() {
  ds_temp = get_ds_temp();
  write_to_sd();
}

// set up the SD card for writing
void setup_sd() {
  Serial.println("setting up SD!");
  if (!SD.begin(cardSelect)) {    //check that SD card is working ok
     Serial.println("Card init. failed!");
  }

// give the log file a name
 char filename[10];       
 strcpy(filename, "Log00.txt");
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
  logfile.println("Digital probe");

  //set the output pins
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

// write data to the sd card
void write_to_sd() {
  digitalWrite(8, HIGH);   // set pin 8 to write
  Serial.println("writing!");  // announce that we are writing to the SD card

// print temperature measurements to the log file
  logfile.println(ds_temp);
  logfile.flush();
  digitalWrite(8, LOW);

// delay to make sure everything goes smoothly
  delay(1000);
}


//get temperature value from the DS probe, and return it as a float. See the documentation here: https://github.com/matmunk/DS18B20
float_t get_ds_temp(){
  while (ds.selectNext()) {
    Serial.print("Temperature:");
    ds_temp = ds.getTempC();
    Serial.print(ds_temp);
    Serial.println(" deg C");
  }
  return ds_temp;
}
    
