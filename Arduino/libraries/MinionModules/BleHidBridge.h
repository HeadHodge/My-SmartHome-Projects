#ifndef BLEHIDBRIDGE_H
#define BLEHIDBRIDGE_H

namespace BleHubBridge
{
    bool isBridgeConnected();
    void openBridge(const char *);
    void controlDevice(const char *);
}

#endif