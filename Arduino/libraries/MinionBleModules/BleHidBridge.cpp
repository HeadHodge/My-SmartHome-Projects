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
MinionTools::addLog("BleHidBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);

    if (BleHub::isHubConnected() == false) {
        MinionTools::addLog("%s", "BleHidBridge::controlDevice Aborted: BleHub not Connected");
        return;
    }
    
    MinionTools::lookupKeyWord(pKeyObj, optionsObj);
    MinionTools::addLog("BleHidBridge::controlDevice: %s %i %i, %i, %i", (const char*)optionsObj["keyWord"], (int)optionsObj["keyCode"], (int)optionsObj["keyModifier"], (int)optionsObj["keyDuration"], (int)optionsObj["keyDelay"]);
    
    if((int)optionsObj["keyCode"] == 0)
    {
        MinionTools::addLog("BleHidBridge::controlDevice Abort: Invalid keyWord '%s'", (const char *)pKeyObj["required"]["keyWord"]);
        return;
    }

    BleHub::controlDevice(optionsObj);
}

void openBridge(const char *pDeviceName) {
    BleHub::openHub(pDeviceName);
    isOpen = true;
    MinionTools::addLog("%s", "BleHidBridge::openBridge, Bridge is Open");
}
}