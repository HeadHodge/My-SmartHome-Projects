#ifndef WSHASSIOBRIDGE_H
#define WSHASSIOBRIDGE_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace WsHassioBridge
{
    typedef char* (*callBack)(DynamicJsonDocument&); // type for conciseness
    bool isOpen();
    void open(callBack);
    void refresh();
    void controlDevice(DynamicJsonDocument&);
} //namespace WsHassioBridge

#endif