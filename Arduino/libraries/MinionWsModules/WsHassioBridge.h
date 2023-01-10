#ifndef WSHASSIOBRIDGE_H
#define WSHASSIOBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace WsDeviceBridge
{
    typedef char* (*callBack)(DynamicJsonDocument&); // type for conciseness
    bool isBridgeOpen();
    void openBridge(callBack);
    void refreshBridge();
    void controlDevice(DynamicJsonDocument&);
}

#endif