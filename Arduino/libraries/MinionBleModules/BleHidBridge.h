#ifndef BLEHIDBRIDGE_H
#define BLEHIDBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace BleHidBridge
{
    bool isBridgeConnected();
    void openBridge(const char *);
    void controlDevice(DynamicJsonDocument&);
} //namespace BleHidBridge

#endif