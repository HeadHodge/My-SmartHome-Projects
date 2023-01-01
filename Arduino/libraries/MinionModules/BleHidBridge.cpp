#define US_KEYBOARD 1
#include <Arduino.h>

#include <MinionTools.h>
#include <BleHub.h>
#include <BleHidBridge.h>

namespace BleHubBridge
{
/*    
#define DEBUG_ON 1
#if DEBUG_ON
#define DBG_begin(...)    Serial.begin(__VA_ARGS__)
#define DBG_print(...)    Serial.print(__VA_ARGS__)
#define DBG_println(...)  Serial.println(__VA_ARGS__)
#define DBG_printf(...)   Serial.printf(__VA_ARGS__)
#else
#define DBG_begin(...)
#define DBG_print(...)
#define DBG_println(...)
#define DBG_printf(...)
#endif
*/

bool isOpen = false;

class deviceOptions {
  public:
    int controlCode; //HID code
    int keyDuration;
    int keyDelay;
    int deviceType; //1-keyboard, 2-consumer, 3-mouse
};

deviceOptions translateControlWord(const char *pControlWord)
{
 MinionTools::addLog("getControlCode: %s\r\n", pControlWord);
 deviceOptions deviceOptions;
    
    deviceOptions.keyDuration = 5;
    deviceOptions.keyDelay    = 5;
    deviceOptions.deviceType  = 2;

    if (strcmp(pControlWord, "Home") == 0) {
      deviceOptions.controlCode = 0x223;
    } else if (strcmp(pControlWord, "Ok") == 0) {
      deviceOptions.controlCode = 0x41;
    } else if (strcmp(pControlWord, "Menu") == 0) {
      deviceOptions.controlCode = 0x40;
    } else if (strcmp(pControlWord, "Back") == 0) {
      deviceOptions.controlCode = 0x46;
    } else if (strcmp(pControlWord, "Up") == 0) {
      deviceOptions.controlCode = 0x42;
    } else if (strcmp(pControlWord, "Down") == 0) {
      deviceOptions.controlCode = 0x43;
    } else if (strcmp(pControlWord, "Left") == 0) {
      deviceOptions.controlCode = 0x44;
    } else if (strcmp(pControlWord, "Right") == 0) {
      deviceOptions.controlCode = 0x45;
    } else if (strcmp(pControlWord, "Forward") == 0) {
      deviceOptions.controlCode = 0xB3;
      deviceOptions.keyDuration = 550;
    } else if (strcmp(pControlWord, "Backward") == 0) {
      deviceOptions.controlCode = 0xB4;
      deviceOptions.keyDuration = 550;
    } else if (strcmp(pControlWord, "PlayToggle") == 0) {
      deviceOptions.controlCode = 0xCD;
    } else if (strcmp(pControlWord, "Softer") == 0) {
      deviceOptions.controlCode = 0xEA;
    } else if (strcmp(pControlWord, "Louder") == 0) {
      deviceOptions.controlCode = 0xE9;
    } else if (strcmp(pControlWord, "OnToggle") == 0) {
      deviceOptions.controlCode = 0x30;
    } else {
      deviceOptions.deviceType = 0;
    }
    
    return deviceOptions;
};

bool isBridgeConnected() {
    return BleHub::isHubConnected();
}

void controlDevice(const char *pControlWord) {
    MinionTools::addLog("BleHidBridge::controlDevice with controlCode %s", pControlWord);

    if (BleHub::isHubConnected() == false) {
        MinionTools::addLog("%s", "BleHidBridge::controlDevice Abrted: BleHub not Connected");
        return;
    }
    
    deviceOptions deviceOptions = translateControlWord(pControlWord);
    
    if(deviceOptions.deviceType == 0)
    {
        MinionTools::addLog("BleHidBridge::controlDevice Abort: Invalid contolWord '%s'", pControlWord);
        return;
    }

    MinionTools::addLog("BleHidBridge::Send Control Code: 0x%X to attached device", deviceOptions.controlCode);
    BleHub::controlDevice(deviceOptions.controlCode, deviceOptions.keyDuration, deviceOptions.keyDelay, deviceOptions.deviceType);
}

void openBridge(const char *pDeviceName) {
    BleHub::openHub(pDeviceName);
    isOpen = true;
    MinionTools::addLog("%s", "BleHidBridge::openBridge, Bridge is Open");
}
}