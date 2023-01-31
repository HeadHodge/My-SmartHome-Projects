#define US_KEYBOARD 1
#include <Arduino.h>

#include <MinionTools.h>
#include <UsbCom.h>
#include <UsbSerialBridge.h>

namespace UsbSerialBridge
{
bool openFlg = false;

bool isConnected() {
    return UsbCom::isConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
MinionTools::addLog("UsbSerialBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);
}

void refresh() {
    UsbCom::refresh();
}

void logString(const char* pString) {
    UsbCom::controlDevice("LOG:");
    UsbCom::controlDevice(pString);
    UsbCom::controlDevice(" \r\n");
}

void open(int pBaud = 115200) {
    UsbCom::open(pBaud);
    openFlg = true;
    MinionTools::addLog("%s", "UsbSerialBridge::open, Bridge now Open");
}
}