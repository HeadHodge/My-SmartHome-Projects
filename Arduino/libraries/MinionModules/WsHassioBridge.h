#ifndef WSHASSIOBRIDGE_H
#define WSHASSIOBRIDGE_H

namespace WsDeviceBridge
{
    typedef char* (*callBack)(const char*, const char*); // type for conciseness
    bool isBridgeOpen();
    void openBridge(callBack);
    void refreshBridge();
    void controlDevice(const char *);
}

#endif