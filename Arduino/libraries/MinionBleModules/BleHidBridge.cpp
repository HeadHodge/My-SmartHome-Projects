#define US_KEYBOARD 1
#include <Arduino.h>

#include <MinionTools.h>
#include <BleHub.h>
#include <BleHidBridge.h>

namespace BleHubBridge
{
bool isOpen = false;

bool isBridgeConnected() {
    return BleHub::isHubConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
MinionTools::addLog("BleHidBridge::controlDevice with keyWord %s", (const char *)pKeyObj["settings"]["keyWord"]);
MinionTools::hidKey deviceOptions;

    if (BleHub::isHubConnected() == false) {
        MinionTools::addLog("%s", "BleHidBridge::controlDevice Abrted: BleHub not Connected");
        return;
    }
    
    deviceOptions = MinionTools::lookupKeyWord(pKeyObj);
    MinionTools::addLog("BleHub::controlDevice keyWord: %s keyCode: 0x%X, duration: %i, delay: %i, usage: %s", deviceOptions.keyWord, deviceOptions.keyCode, deviceOptions.keyDuration, deviceOptions.keyDelay, deviceOptions.keyUsage);
    
    if(deviceOptions.keyCode == 0)
    {
        MinionTools::addLog("BleHidBridge::controlDevice Abort: Invalid keyWord '%s'", (const char *)pKeyObj["settings"]["keyWord"]);
        return;
    }

    BleHub::controlDevice(&deviceOptions);
}

void openBridge(const char *pDeviceName) {
    BleHub::openHub(pDeviceName);
    isOpen = true;
    MinionTools::addLog("%s", "BleHidBridge::openBridge, Bridge is Open");
}
}