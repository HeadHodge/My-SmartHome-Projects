#define US_KEYBOARD 1
#include <Arduino.h>

#include <SysTools.h>
#include <UsbHidOut.h>
#include <UsbHidBridge.h>

namespace UsbHidBridge {
bool isOpen = false;

bool isBridgeConnected() {
    return UsbHidOut::isConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
SysTools::addLog("UsbHidBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);

    if (UsbHidOut::isConnected() == false) {
        SysTools::addLog("%s", "UsbHidBridge::controlDevice Abrted: UsbHub not Connected");
        return;
    }
    
    //SysTools::lookupKeyWord(pKeyObj, optionsObj);    
    SysTools::addLog("UsbHidBridge::controlDevice: %s %i %i, %i, %i", (const char*)optionsObj["keyWord"], (int)optionsObj["keyCode"], (int)optionsObj["keyModifier"], (int)optionsObj["keyDuration"], (int)optionsObj["keyDelay"]);
    
    if((int)optionsObj["keyCode"] == 0)
    {
        SysTools::addLog("UsbHidBridge::controlDevice Abort: Invalid keyCode '0x%X'", (int)optionsObj["keyCode"]);
        return;
    }

    UsbHidOut::controlDevice(optionsObj);
}
void openBridge() {
    UsbHidOut::open();
    isOpen = true;
    SysTools::addLog("%s", "UsbHidBridge::openBridge, Bridge now Open");
}
} //namespace UsbHidBridge