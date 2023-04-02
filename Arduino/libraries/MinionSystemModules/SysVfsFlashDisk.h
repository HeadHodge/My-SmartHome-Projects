#ifndef SYSVFSFLASHDISK_H
#define SYSVFSFLASHDISK_H

namespace SysFlashDisk {
    bool readRAW(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
    bool writeRAW(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
    uint8_t enable(SysVfsFatFs::vfsDiskOptions_t** pDiskOptions);
    
    const char* mediaType();
    uint32_t sectorCount();
    uint16_t sectorSize();
} //namespace SysFlashDisk

#endif