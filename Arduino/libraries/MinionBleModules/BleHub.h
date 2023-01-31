#ifndef BLEHUB_H
#define BLEHUB_H

#include <ArduinoJson.h>
#include <MinionTools.h>

namespace BleHub
{
    bool isHubConnected();
    void openHub(const char *);
    void controlDevice(DynamicJsonDocument&);
}

#endif