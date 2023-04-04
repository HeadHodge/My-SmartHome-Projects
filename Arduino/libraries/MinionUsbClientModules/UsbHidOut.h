#ifndef USBHIDOUT_H
#define USBHIDOUT_H

#include <ArduinoJson.h>

namespace UsbHidOut {
    bool isConnected();
    void open();
    void controlDevice(DynamicJsonDocument&);
} //namespace UsbHidOut

#endif