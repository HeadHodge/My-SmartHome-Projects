#ifndef SYSVFSBRIDGE_H
#define SYSVFSBRIDGE_H

namespace SysVfsBridge {
    
    typedef struct {
        uint8_t diskNum;
        char*    diskPath;
        char*    fileSystem;
        char*    mountDirectory;
        char*    testFile;
        char*    testDirectory;
        uint16_t diskTableSectors; //each table sector can fit 170KB (340 sectors)

        const char* (*diskType)();
        uint32_t    (*sectorCount)();
        uint16_t    (*sectorSize)();
        bool        (*readRaw)(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
        bool        (*writeRaw)(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
        void        (*onInsert)();
        void        (*onRemove)();
        bool        (*onFormatted)();
    } vfsDiskOptions_t;

    void dumpSector(uint16_t pSector, uint16_t pDumpBytes=32);
    bool enableDisk(const char* pDiskType="flashDisk", bool pFormatDisk = false, vfsDiskOptions_t** pDiskOptions = nullptr);
    bool isConnected();
} //namespace SysVfsBridge

#endif