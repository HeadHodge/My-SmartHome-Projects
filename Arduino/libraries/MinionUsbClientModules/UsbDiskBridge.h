#ifndef USBDISKBRIDGE_H
#define USBDISKBRIDGE_H

//#include <ArduinoJson.h>  // Install from IDE Library manager

namespace UsbDiskBridge {
    bool isOpen();
    bool enable();
    //void controlDevice(DynamicJsonDocument&);
} //namespace UsbDiskBridge

#endif