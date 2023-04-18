#ifndef WSJSONIN_H
#define WSJSONIN_H

namespace WsJsonIn {
    typedef char* (*callBack)(unsigned char*);
    #define CONNECTSSID 0
    #define CONNECTPASSWORD 1
    #define CONNECTADDRESS 2
    
    char** getConnectInfo();
    bool isConnected();
    void open(callBack);
    void refresh();
} //namespace WsJsonIn

#endif