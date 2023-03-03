#define US_KEYBOARD 1
#include <Arduino.h>

#include <SysTools.h>
#include <ComSerial.h>
#include <ComSerialBridge.h>

namespace ComSerialBridge
{
bool openFlg = false;

bool isConnected() {
    return ComSerial::isConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
SysTools::addLog("ComSerialBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);
}

void refresh() {
    ComSerial::refresh();
}

void logString(const char* pString) {
    ComSerial::controlDevice("LOG:");
    ComSerial::controlDevice(pString);
    ComSerial::controlDevice(" \r\n");
}

void open(int pBaud = 115200) {
    ComSerial::open(pBaud);
    openFlg = true;
    SysTools::addLog("%s", "ComSerialBridge::open, Bridge now Open");
}
}