#ifndef SYSFLASHDISK_H
#define SYSFLASHDISK_H

#include "FS.h"
#include "FFat.h"

namespace SysFlashDisk {
    void testFileIO(const char * path);
    void listDir(const char * dirname, uint8_t levels);
    void deleteFile(const char * path);
    void renameFile(const char * path1, const char * path2);
    void appendFile(const char * path, const char * message);
    void writeFile(const char * path, const char * message);
    void readFile(const char * path);
    void open(void (*pCallback)(bool status));
    bool isConnected();
    
    //bool isReady();
    //bool controlDevice(uint8_t* pDataBuffer, uint8_t pSize);
}

#endif