#ifndef BLEHIDBRIDGE_H
#define BLEHIDBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace BleHubBridge
{
    bool isBridgeConnected();
    void openBridge(const char *);
    void controlDevice(DynamicJsonDocument&);
}

#endif