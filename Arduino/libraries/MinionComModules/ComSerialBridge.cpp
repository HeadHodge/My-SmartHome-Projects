#define US_KEYBOARD 1
#include <Arduino.h>

#include <MinionTools.h>
#include <ComSerial.h>
#include <ComSerialBridge.h>

namespace ComSerialBridge
{
bool openFlg = false;

bool isConnected() {
    return ComSerial::isConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
MinionTools::addLog("ComSerialBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
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
    MinionTools::addLog("%s", "ComSerialBridge::open, Bridge now Open");
}
}