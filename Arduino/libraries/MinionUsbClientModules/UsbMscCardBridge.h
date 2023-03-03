#ifndef USBHIDBRIDGE_H
#define USBHIDBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbMscCardBridge {
    bool isOpen();
    void open();
    //void controlDevice(DynamicJsonDocument&);
}

#endif