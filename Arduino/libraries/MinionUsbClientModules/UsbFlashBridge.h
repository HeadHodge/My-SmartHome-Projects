#ifndef USBFLASHBRIDGE_H
#define USBFLASHBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbFlashBridge {
    bool isOpen();
    bool enable();
    //void controlDevice(DynamicJsonDocument&);
}

#endif