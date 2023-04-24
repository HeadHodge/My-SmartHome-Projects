#ifndef SYSSPIDISK_H
#define SYSSPIDISK_H
#include <SysVfsBridge.h>

namespace SysSpiDisk {
    uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint32_t writeRAW(uint32_t pSector, const uint8_t* pBuffer, uint32_t pBufferSize);    
    uint8_t enable(SysVfsBridge::vfsDiskOptions_t** pDiskOptions);

    const char* diskType();
    uint32_t sectorCount();
    uint16_t sectorSize();
} //SysSpiDisk

#endif