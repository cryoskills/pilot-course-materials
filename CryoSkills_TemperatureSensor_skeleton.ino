// Include libraries

// init probe objects
DS18B20 ds(5); // nb this uses the 'Arduino name' of the pin (D5)

void setup() {
  Serial.begin(9600); // sets baud rate (serial communication at 9600 bits of data per second)
}

void loop() {
 
 //get temperature value from DS probe, print to serial  
  while (ds.selectNext()) {
    Serial.print("Temperature: ");
    Serial.print(ds.getTempC());
    Serial.println(" deg C");
  }

  // wait 1 second
  delay(1000);

}