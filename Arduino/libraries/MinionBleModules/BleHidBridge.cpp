#define US_KEYBOARD 1
#include <Arduino.h>

#include <SysTools.h>
#include <BleHub.h>
#include <BleHidBridge.h>

namespace BleHubBridge
{
bool isOpen = false;

bool isBridgeConnected() {
    return BleHub::isHubConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
SysTools::addLog("BleHidBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);

    if (BleHub::isHubConnected() == false) {
        SysTools::addLog("%s", "BleHidBridge::controlDevice Aborted: BleHub not Connected");
        return;
    }
    
    SysTools::lookupKeyWord(pKeyObj, optionsObj);
    SysTools::addLog("BleHidBridge::controlDevice: %s %i %i, %i, %i", (const char*)optionsObj["keyWord"], (int)optionsObj["keyCode"], (int)optionsObj["keyModifier"], (int)optionsObj["keyDuration"], (int)optionsObj["keyDelay"]);
    
    if((int)optionsObj["keyCode"] == 0)
    {
        SysTools::addLog("BleHidBridge::controlDevice Abort: Invalid keyWord '%s'", (const char *)pKeyObj["required"]["keyWord"]);
        return;
    }

    BleHub::controlDevice(optionsObj);
}

void openBridge(const char *pDeviceName) {
    BleHub::openHub(pDeviceName);
    isOpen = true;
    SysTools::addLog("%s", "BleHidBridge::openBridge, Bridge is Open");
}
}