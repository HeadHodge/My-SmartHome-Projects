#ifndef SYSVFSRAMDISK_H
#define SYSVFSRAMDISK_H
#include <SysVfsBridge.h>

namespace SysRamDisk {
    bool readRAW(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
    bool writeRAW(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
    uint8_t enable(SysVfsBridge::vfsDiskOptions_t** pDiskOptions);
    
    const char* diskType();
    uint32_t sectorCount();
    uint16_t sectorSize();
} //namespace SysRamDisk

#endif