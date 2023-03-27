#ifndef USBFLASHDISKBRIDGE_H
#define USBFLASHDISKBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbFlashDiskBridge {
    bool isOpen();
    bool enable();
    //void controlDevice(DynamicJsonDocument&);
}

#endif