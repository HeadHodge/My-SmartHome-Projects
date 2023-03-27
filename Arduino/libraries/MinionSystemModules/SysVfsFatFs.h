#ifndef SYSVFSDISKS_H
#define SYSVFSDISKS_H
#include <FF.h>
#include <esp_vfs_fat.h>
#include <diskio_impl.h>

namespace SysVfsFatFs {
    
    typedef struct {
        uint8_t diskNum;
        char*   diskPath;
        char*   fileSystem;
        char*   testFile;
        char*   testDirectory;
        FATFS*  fatFS;
        uint16_t diskSectorCount;  // 8KB is the smallest size that windows allow to mount
        uint16_t diskSectorSize;   // Should be 512
        uint16_t diskTableSectors; //each table sector can fit 170KB (340 sectors)

        ff_diskio_impl_t diskioEvents;    
    } vfsDiskOptions_t;

    void dumpSector(uint16_t pSector, uint16_t pDumpBytes=32);
    bool enable(const char* pDiskType="flashDisk", vfsDiskOptions_t** pDiskOptions = nullptr);
    bool isConnected();
} //SysVfsFatFs

#endif