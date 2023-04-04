#ifndef WSJSONOUT_H
#define WSJSONOUT_H

namespace WsJsonOut
{
    typedef char* (*callBack)(unsigned char*); // type for conciseness
    bool isConnected();
    void open(callBack);
    void refresh();
    void controlDevice(const char *);
} //namespace WsJsonOut

#endif