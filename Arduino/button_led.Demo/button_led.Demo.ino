/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

#include <SysTools.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 14;     // the number of the pushbutton pin
const int ledPin =  15;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
    Serial.begin(115200);
    delay(4000);
    
    SysTools::addLog("%s", "lcdDisplayTest::Setup");
    
    pinMode(BOOST_EN, OUTPUT);
    pinMode(DEV_VBUS_EN, OUTPUT);
    digitalWrite(BOOST_EN, HIGH);
    digitalWrite(DEV_VBUS_EN, HIGH);

    // initialize the LED pin as an output:
    pinMode(GPIO_NUM_15, OUTPUT);
    digitalWrite(GPIO_NUM_15, HIGH);
    pinMode(GPIO_NUM_16, OUTPUT);
    digitalWrite(GPIO_NUM_16, HIGH);
    
    //gpio_set_direction(GPIO_NUM_10, GPIO_MODE_OUTPUT);
    //gpio_set_level(GPIO_NUM_10, 1);

    pinMode(14, INPUT_PULLUP);
}

void loop() {
  // read the state of the pushbutton value:
  SysTools::addLog("buttonState: %u", digitalRead(14));
  
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    SysTools::addLog("%s", "button high");
    // turn LED on:
    //digitalWrite(ledPin, HIGH);
  } else {
    //SysTools::addLog("%s", "button low");
    // turn LED off:
    //digitalWrite(ledPin, LOW);
  }

  //delay(2000);
}
