#ifndef MINIONTOOLS_H
#define MINIONTOOLS_H

#include <ArduinoJson.h>  // Install from IDE Library manager

namespace MinionTools
{   
    struct hidKey {
        const char* keyUsage; // 1-keyboard, 2-consuner, 3-mouse
        int         keyModifier;
        int         keyCode;
        const char* keyWord;
        const char* keyData;
        int         keyDuration;
        int         keyDelay;
        int         androidCode;
    };
  
    hidKey lookupKeyWord(DynamicJsonDocument&);
    void openLog(int);
    void addLog(const char*, ...);
}

#endif