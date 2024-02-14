/*
  Turns on and off a light emitting diode(LED) connected to digital pin 10,
  when pressing a pushbutton attached to pin 9.

  The circuit:
  - LED attached from pin 10 to ground through 220 ohm resistor
  - pushbutton attached to pin 9 from +3.3V
  - 24K resistor attached to pin 9 from ground

*/

// Define constants and variables here:
// Constants won't change. They're used here to set pin numbers:
const int buttonPin = 9;  // the number of the pushbutton pin
const int LEDpin = 10;        // the number of the LED pin

// Variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
    // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output:
  pinMode(LEDpin, OUTPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(LEDpin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(LEDpin, LOW);
  }
}
