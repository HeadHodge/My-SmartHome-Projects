#include <Arduino.h>

#include <MinionTools.h>
#include <ComSerialBridge.h>
#include <UsbhostCDC.hpp>

void setup()
{
    ComSerialBridge::open(115200);
    delay(5000);
    MinionTools::addLog("%s \n", "irDroid::setup Start");

    UsbhostCDC::open();
}

void loop()
{
    //delay(10);
    //MinionTools::addLog("%s \n", "UsbhostInterface::refresh");
    UsbhostCDC::refresh();
}
