#ifndef WSKEYHUB_H
#define WSKEYHUB_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace WsKeyHub {
    typedef char* (*receivedKeyCallback)(DynamicJsonDocument&);bool isConnected();
    void open(receivedKeyCallback);
    void refresh();
} //namespace WsKeyHub

#endif