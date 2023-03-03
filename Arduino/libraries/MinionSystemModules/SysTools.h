#ifndef MINIONSYSTEM_H
#define MINIONSYSTEM_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace SysTools
{
    void lookupKeyWord(DynamicJsonDocument&, DynamicJsonDocument&);
    void addLog(const char*, ...);
    void sendCommand(const char*);
    void receivedCommand(const char*);
    void getOptions(const char*, DynamicJsonDocument&);
}

#endif