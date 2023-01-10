#ifndef WSJSONBRIDGE_H
#define WSJSONBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace WsHubBridge
{
    typedef char* (*receivedKeyCallback)(DynamicJsonDocument&); // type for conciseness
    bool isBridgeConnected();
    void openBridge(receivedKeyCallback);
    void refreshBridge();
}

#endif