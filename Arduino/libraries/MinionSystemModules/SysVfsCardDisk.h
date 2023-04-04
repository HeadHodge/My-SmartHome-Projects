#ifndef SYSCARDDISK_H
#define SYSCARDDISK_H
#include <SysVfsFatFs.h>

namespace SysCardDisk {
    uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint32_t writeRAW(uint32_t pSector, const uint8_t* pBuffer, uint32_t pBufferSize);    
    uint8_t enable(SysFatFs::vfsDiskOptions_t** pDiskOptions);

    const char* diskType();
    uint32_t sectorCount();
    uint16_t sectorSize();
} //SysCardDisk

#endif