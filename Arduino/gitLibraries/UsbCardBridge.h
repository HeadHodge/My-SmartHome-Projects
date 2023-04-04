#ifndef USBCARDBRIDGE_H
#define USBCARDBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbCardBridge {
    bool isOpen();
    void open();
    //void controlDevice(DynamicJsonDocument&);
}

#endif