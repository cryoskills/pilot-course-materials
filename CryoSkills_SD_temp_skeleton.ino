// Add libraries (SD card and temperature sensor)

// let's declare some functions for stuff we need to do in the script
void setup_sd();   //set up the SD card to log data to a file
void write_to_sd();  // write data to file
// add temperature function here

#define cardSelect 4  // set the pin used by the SD card
File logfile;  // file for logging data to

// initialise variables that we'll use later (temperature)


// now we have our setup loop. We need open the serial connection and set up the SD card.
void setup() {
  Serial.begin(9600); //open serial connection
  setup_sd();
}


// this is our main loop, which will run repeatedly. 
void loop() {
  write_to_sd();
  // call temperature function here
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


  logfile.flush();
  digitalWrite(8, LOW);

// delay to make sure everything goes smoothly
  delay(1000);
}

    
