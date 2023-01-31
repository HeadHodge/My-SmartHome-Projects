#ifndef MINIONTOOLS_H
#define MINIONTOOLS_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace MinionTools
{
    void lookupKeyWord(DynamicJsonDocument&, DynamicJsonDocument&);
    void addLog(const char*, ...);
    void sendCommand(const char*);
    void receivedCommand(const char*);
    void getOptions(const char*, DynamicJsonDocument&);
}

#endif