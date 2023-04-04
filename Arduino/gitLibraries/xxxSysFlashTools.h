#ifndef SYSFLASHTOOLS_H
#define SYSFLASHTOOLS_H

#include <Preferences.h>

namespace SysFlashTools {
    //void listDir(const char * dirname, uint8_t levels);
    //void testFileIO(const char * path);
    void printSector(Preferences* pDisk, char* pKey);
    void dumpSector(Preferences* pDisk, char* pKey, uint32_t pByteCount = 16);
    bool formatDisk(Preferences* pDisk, uint32_t pSectorCount);
}

#endif