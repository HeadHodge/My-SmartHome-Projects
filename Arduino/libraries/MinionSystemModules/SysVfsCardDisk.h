#ifndef SYSCARDDISK_H
#define SYSCARDDISK_H

namespace SysCardDisk {
    uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint32_t writeRAW(uint32_t pSector, const uint8_t* pBuffer, uint32_t pBufferSize);    
    uint8_t enable(SysVfsFatFs::vfsDiskOptions_t** pDiskOptions);

    char* cardType(uint8_t pdrv);
    uint32_t sectorCount(uint8_t pdrv);
    uint32_t sectorSize(uint8_t pdrv);
} //SysCardDisk

#endif