#include <ComSerialBridge.h>
#include <SysTools.h>
#include <SysFlashTools.h>

void setup() {
    ComSerialBridge::open(115200);
    delay(5000);
    SysTools::addLog("%s \n", "Start flashTools::setup");
   
    SysFlashTools::formatFatDisk();
}

void loop() {
  // put your main code here, to run repeatedly:
}
