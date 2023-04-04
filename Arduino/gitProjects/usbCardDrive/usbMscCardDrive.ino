//#include "USB.h"
//#include "USBMSC.h"

#include <ComSerialBridge.h>
#include <SysTools.h>
#include <UsbMscCardBridge.h>

void setup() {
    ComSerialBridge::open(115200);
    delay(2000);
    SysTools::addLog("%s", "Setup usbMscCardDrive");
  
//Open USB Drive
    UsbMscCardBridge::open();  
}

void loop() {
  // put your main code here, to run repeatedly:
}
