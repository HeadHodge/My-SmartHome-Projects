#ifndef SYSSERIALPORT_H
#define SYSSERIALPORT_H

namespace SysSerialPort {
    bool isConnected();
    void open(int baudRate);
    void refresh();
    void controlDevice(const char* pString);
} //namespace

#endif