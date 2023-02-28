#ifndef USBHIDBRIDGE_H
#define USBHIDBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbHubBridge
{
    bool isBridgeOpen();
    void openBridge();
    void controlDevice(DynamicJsonDocument&);
}

#endif