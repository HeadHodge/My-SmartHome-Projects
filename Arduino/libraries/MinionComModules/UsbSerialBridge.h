#ifndef USBSERIALBRIDGE_H
#define USBSERIALBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbSerialBridge
{
    bool isOpen();
    void open(int baudRate);
    void refresh();
    void logString(const char* pString);
    void controlDevice(DynamicJsonDocument&);
}

#endif