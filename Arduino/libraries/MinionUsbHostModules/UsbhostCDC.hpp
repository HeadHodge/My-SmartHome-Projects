#ifndef USBHOSTCDC_H
#define USBHOSTCDC_H

namespace UsbhostCDC
{
    void open();
    void refresh();
    bool isConnected();
    void sendData(uint8_t* pDataBuffer, uint8_t pSize);
    void transferDataOut(uint8_t* pDataBuffer, uint8_t pSize);
    void transferData(uint8_t pTranferType, uint8_t* pData, uint8_t pSize);
    void transferDataIn();
    void transferCtlIn();
} //namespace UsbhostCDC

#endif