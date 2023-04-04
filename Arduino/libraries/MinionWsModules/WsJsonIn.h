#ifndef WSJSONIN_H
#define WSJSONIN_H

namespace WsJsonIn {
    typedef char* (*callBack)(unsigned char*);
    bool isConnected();
    void open(callBack);
    void refresh();
} //namespace WsJsonIn

#endif