//Include libraries
#include <OneWire.h>
#include <DS18B20.h>

// init probe objects
DS18B20 ds(5); //nb this uses the 'Arduino name' of the pin (D5)

// initialise functions to use later
float_t get_ds_temp();   //get a digital temperature reading, return it as a float

// initialise variables
unsigned long myTime = 0;
float_t ds_temp; // reading from the DS probe

void setup() {
  Serial.begin(9600); // sets baud rate (serial communication at 9600 bits of data per second)
}

void loop() {
  ds_temp = get_ds_temp(); // Temperature from the function below

  // Time:
  Serial.print("Time: "); // Print text
  myTime = millis();      // FUnction returns the number of milliseconds since the Arduino board began running the current program.
  Serial.println(myTime); // prints time since program started

  // wait 5 seconds
  delay(5000);
}
 
 // get temperature value from DS probe as a function
float_t get_ds_temp(){
  while (ds.selectNext()) {
    Serial.print("Temperature:");
    ds_temp = ds.getTempC();
    Serial.print(ds_temp);
    Serial.println(" deg C");
  }
  return ds_temp;
}