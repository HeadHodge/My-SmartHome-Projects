#ifndef MINIONSYSTEM_H
#define MINIONSYSTEM_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace SysTools
{
    //void lookupKeyWord(DynamicJsonDocument&, DynamicJsonDocument&);
    void addLog(const char*, ...);
    void displayHome();
    void displayAPConnection();
    void displayStationConnection();
    //void sendCommand(const char*);
    //void receivedCommand(const char*);
    //void getOptions(const char*, DynamicJsonDocument&);
    void dumpBuffer(const uint8_t* pBuffer, uint32_t pBufferSize, uint32_t pByteCount);
    bool saveOptionsObj(DynamicJsonDocument* pOptionsObject, char* pFileName="system.cfg", char* pDirectory="/cardDisk/system");
    char* createPkgString(DynamicJsonDocument* pPkg);
    DynamicJsonDocument* createPkg(char* pJsonString);
    DynamicJsonDocument* getOptionsObj(char* pFileName="system.cfg", char* pDirectory="/cardDisk/system");
}

#endif