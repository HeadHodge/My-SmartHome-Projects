#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "USB.h"
#include "USBHIDConsumerControl.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

#define USBSerial Serial

USBHIDConsumerControl ConsumerControl;
USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;

const int buttonPin = 0;
int previousButtonState = HIGH;

void setup() {
  //pinMode(buttonPin, INPUT_PULLUP);
  USBSerial.begin(115200);

  Keyboard.begin();
  ConsumerControl.begin();
  Mouse.begin();
  USB.begin();
}

void loop() {
  delay(5000);
  USBSerial.println("Loop usb");

  Keyboard.pressRaw('h');
  Keyboard.releaseRaw('h');
  ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
  ConsumerControl.release();
  Mouse.move(200,200);
}
#endif /* ARDUINO_USB_MODE */
