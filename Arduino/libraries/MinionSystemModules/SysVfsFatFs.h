#ifndef SYSVFSDISKS_H
#define SYSVFSDISKS_H

namespace SysVfsFatFs {
    
    typedef struct {
        uint8_t diskNum;
        char*   diskType;
        char*   diskPath;
        char*   fileSystem;
        char*   testFile;
        char*   testDirectory;
        uint32_t diskSectorCount;  // 8KB is the smallest size that windows allow to mount
        uint16_t diskSectorSize;   // Should be 512
        uint16_t diskTableSectors; //each table sector can fit 170KB (340 sectors)
        bool (*readRaw)(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
        bool (*writeRaw)(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
    } vfsDiskOptions_t;

    //int32_t onRead(uint32_t pSector, uint32_t pOffset, void* pBuffer, uint32_t pBuffSize);
    //int32_t onWrite(uint32_t pSector, uint32_t pOffset, uint8_t* pBuffer, uint32_t pBuffSize);
    void dumpSector(uint16_t pSector, uint16_t pDumpBytes=32);
    bool enableDisk(const char* pDiskType="flashDisk", vfsDiskOptions_t** pDiskOptions = nullptr);
    bool isConnected();
} //SysVfsFatFs

#endif