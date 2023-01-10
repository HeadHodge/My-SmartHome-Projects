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
MinionTools::addLog("UsbHidBridge::controlDevice with keyWord %s", (const char *)pKeyObj["settings"]["keyWord"]);
MinionTools::hidKey deviceOptions;

    if (UsbHub::isHubConnected() == false) {
        MinionTools::addLog("%s", "UsbHidBridge::controlDevice Abrted: UsbHub not Connected");
        return;
    }
    
    deviceOptions = MinionTools::lookupKeyWord(pKeyObj);
    MinionTools::addLog("UsbHidBridge::controlDevice keyWord: %s keyCode: 0x%X, duration: %i, delay: %i, usage: %s", deviceOptions.keyWord, deviceOptions.keyCode, deviceOptions.keyDuration, deviceOptions.keyDelay, deviceOptions.keyUsage);
    
    if(deviceOptions.keyCode == 0)
    {
        MinionTools::addLog("UsbHidBridge::controlDevice Abort: Invalid keyWord '%s'", (const char *)pKeyObj["settings"]["keyWord"]);
        return;
    }

    UsbHub::controlDevice(&deviceOptions);
}
void openBridge() {
    UsbHub::openHub();
    isOpen = true;
    MinionTools::addLog("%s", "UsbHidBridge::openBridge, Bridge now Open");
}
}