#ifndef WSDEVICE_H
#define WSDEVICE_H

namespace WsDevice
{
    typedef char* (*callBack)(unsigned char*); // type for conciseness
    bool isDeviceConnected();
    void openDevice(callBack);
    void refreshDevice();
    void controlDevice(const char *);
}

#endif