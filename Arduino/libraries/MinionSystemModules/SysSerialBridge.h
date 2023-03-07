#ifndef SYSSERIALBRIDGE_H
#define SYSSERIALBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace SysSerialBridge {
    bool isOpen();
    void open(int baudRate);
    void refresh();
    void logString(const char* pString);
    void controlDevice(DynamicJsonDocument&);
}

#endif