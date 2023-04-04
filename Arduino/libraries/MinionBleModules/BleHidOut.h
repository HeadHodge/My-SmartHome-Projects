#ifndef BLEHIDOUT_H
#define BLEHIDOUT_H

#include <ArduinoJson.h>

namespace BleHidOut
{
    bool isConnected();
    void open(const char *);
    void controlDevice(DynamicJsonDocument&);
} //namespace BleHidOut

#endif