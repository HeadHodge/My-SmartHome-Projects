#ifndef SYSFLASHDISK_H
#define SYSFLASHDISK_H
#include <FF.h>
#include <esp_vfs_fat.h>
#include <diskio_impl.h>

namespace SysFlashDisk {
    uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint32_t writeRAW(uint32_t pSector, const uint8_t* pBuffer, uint32_t pBufferSize);    
    void setOptions(uint8_t pDiskNum, SysVfsFatFs::vfsDiskOptions_t* pDiskOptions);
}

#endif