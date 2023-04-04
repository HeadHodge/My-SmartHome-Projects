#ifndef SYSVFSDISKS_H
#define SYSVFSDISKS_H

namespace SysFatFs {
    
    typedef struct {
        uint8_t diskNum;
        char*   diskPath;
        char*   fileSystem;
        char*   testFile;
        char*   testDirectory;
        uint16_t diskTableSectors; //each table sector can fit 170KB (340 sectors)

        const char* (*diskType)();
        uint32_t    (*sectorCount)();
        uint16_t    (*sectorSize)();
        bool        (*readRaw)(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
        bool        (*writeRaw)(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
    } vfsDiskOptions_t;

    void dumpSector(uint16_t pSector, uint16_t pDumpBytes=32);
    bool enableDisk(const char* pDiskType="flashDisk", vfsDiskOptions_t** pDiskOptions = nullptr);
    bool isConnected();
} //namespace SysFatFs

#endif