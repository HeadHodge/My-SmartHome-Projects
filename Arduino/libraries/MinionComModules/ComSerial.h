#ifndef COMSERIAL_H
#define COMSERIAL_H

namespace ComSerial
{
    bool isConnected();
    void open(int baudRate);
    void refresh();
    void controlDevice(const char* pString);
}

#endif