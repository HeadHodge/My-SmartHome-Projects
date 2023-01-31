#ifndef USBCOM_H
#define USBCOM_H

namespace UsbCom
{
    bool isConnected();
    void open(int baudRate);
    void refresh();
    void controlDevice(const char* pString);
}

#endif