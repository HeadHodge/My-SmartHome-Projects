#ifndef USBHUB_H
#define USBHUB_H

#include <ArduinoJson.h>
#include <SysTools.h>

namespace UsbHub
{
    bool isHubConnected();
    void openHub();
    void controlDevice(DynamicJsonDocument&);
}

#endif