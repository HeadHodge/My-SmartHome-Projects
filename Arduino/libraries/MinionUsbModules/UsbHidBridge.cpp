#define US_KEYBOARD 1
#include <Arduino.h>

#include <MinionTools.h>
#include <UsbHub.h>
#include <UsbHidBridge.h>

namespace UsbHubBridge
{
bool isOpen = false;

bool isBridgeConnected() {
    return UsbHub::isHubConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
MinionTools::addLog("UsbHidBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);

    if (UsbHub::isHubConnected() == false) {
        MinionTools::addLog("%s", "UsbHidBridge::controlDevice Abrted: UsbHub not Connected");
        return;
    }
    
    MinionTools::lookupKeyWord(pKeyObj, optionsObj);    
    MinionTools::addLog("UsbHidBridge::controlDevice: %s %i %i, %i, %i", (const char*)optionsObj["keyWord"], (int)optionsObj["keyCode"], (int)optionsObj["keyModifier"], (int)optionsObj["keyDuration"], (int)optionsObj["keyDelay"]);
    
    if((int)optionsObj["keyCode"] == 0)
    {
        MinionTools::addLog("UsbHidBridge::controlDevice Abort: Invalid keyCode '0x%X'", (int)optionsObj["keyCode"]);
        return;
    }

    UsbHub::controlDevice(optionsObj);
}
void openBridge() {
    UsbHub::openHub();
    isOpen = true;
    MinionTools::addLog("%s", "UsbHidBridge::openBridge, Bridge now Open");
}
}