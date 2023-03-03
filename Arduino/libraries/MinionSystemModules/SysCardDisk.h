#ifndef SYSCARDDISK_H
#define SYSCARDDISK_H

#include "FS.h"
#include "FFat.h"

namespace SysCardDisk {
    bool open(void (*pCallback)(bool status));
    void readRAW(uint8_t* pBuffer, uint32_t pSector, uint8_t pIndex);
    void writeRAW(uint8_t* pBuffer, uint32_t pSector, uint8_t pIndex);
    size_t getSectorSize();
    size_t getSectorCount();
    
    /*
    void testFileIO(const char * path);
    void listDir(const char * dirname, uint8_t levels);
    void deleteFile(const char * path);
    void renameFile(const char * path1, const char * path2);
    void appendFile(const char * path, const char * message);
    void writeFile(const char * path, const char * message);
    void readFile(const char * path);
    void open(void (*pCallback)(bool status));
    bool isConnected();
    */
}

#endif