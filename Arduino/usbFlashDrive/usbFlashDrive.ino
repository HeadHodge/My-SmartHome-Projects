//#include <ComSerialBridge.h>
#include <SysTools.h>
#include <UsbFlashBridge.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

void setup(){
    //ComSerialBridge::open(115200);   
    Serial.begin(115200);
    delay(4000);
    SysTools::addLog("%s", "usbFlashDrive::Setup");

    UsbFlashBridge::enable();   
    //SysFlashDisk::testFileIO("/test.txt");   
    //SysTools::addLog( "%s \n", "Test complete" );
}

void loop(){

}
