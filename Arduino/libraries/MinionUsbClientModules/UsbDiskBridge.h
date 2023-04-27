#ifndef USBDISKBRIDGE_H
#define USBDISKBRIDGE_H
#include <SysVfsBridge.h>

namespace UsbDiskBridge {
    bool isOpen();
    bool enable(SysVfsBridge::vfsDiskOptions_t* pDiskOptions = nullptr);
} //namespace UsbDiskBridge

#endif