#ifndef USBHOSTCDC_H
#define USBHOSTCDC_H

namespace UsbhostCDC
{
    void open(void (*pCallback)(bool status));
    void refresh();
    bool isConnected();
    bool isReady();
    bool controlDevice(uint8_t* pDataBuffer, uint8_t pSize);
} //namespace UsbhostCDC

#endif