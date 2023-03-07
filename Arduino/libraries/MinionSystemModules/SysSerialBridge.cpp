#define US_KEYBOARD 1
#include <Arduino.h>

#include <SysTools.h>
#include <SysSerialPort.h>
#include <SysSerialBridge.h>

namespace SysSerialBridge {
bool openFlg = false;

bool isConnected() {
    return SysSerialPort::isConnected();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {    
SysTools::addLog("ComSerialBridge::controlDevice with keyWord %s", (const char *)pKeyObj["required"]["keyWord"]);
DynamicJsonDocument optionsObj(1024);
}

void refresh() {
    SysSerialPort::refresh();
}

void logString(const char* pString) {
    SysSerialPort::controlDevice("LOG:");
    SysSerialPort::controlDevice(pString);
    SysSerialPort::controlDevice(" \r\n");
}

void open(int pBaud = 115200) {
    SysSerialPort::open(pBaud);
    openFlg = true;
    SysTools::addLog("%s", "ComSerialBridge::open, Bridge now Open");
}
} //namespace