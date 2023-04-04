//Sytem Required Libraries
#include <stdio.h>
#include <dirent.h>
//#include <FF.h>
//#include <esp_vfs_fat.h>
#include <diskio_impl.h>


//My Required Libraries
#include <SysTools.h>
#include <SysVfsFatFs.h>
#include <SysVfsFlashDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysFlashDisk {
/////////////////////////////////////////////////////////////////////////////////////////////////
SysFatFs::vfsDiskOptions_t _vfsDiskOptions;
uint8_t* _diskBuff = (uint8_t*)malloc(4096);
const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");
//_ffatPartition->address, _ffatPartition->size, _ffatPartition->label

/////////////////////////////////////////////////////////////////////////////////////////////////
const char* diskType() {
 
    return _ffatPartition->label;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t sectorCount() {

    return (_ffatPartition->size / 512) - 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t sectorSize() {
    return 512;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpSector(uint16_t pSector, uint16_t pDumpBytes) {
    SysTools::addLog("SysVfsFatFs::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        
    uint8_t buffer[512];
    memset(&buffer, 0, sizeof(buffer));
        
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pSector*512, buffer, sizeof(buffer));    
    if(hasFailed) {
        SysTools::addLog("SysVfsFatFs::dumpSector, ABORT: esp_partition_read_raw failed");
        SysTools::addLog("SysVfsFatFs::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        return;
    }

    SysTools::dumpBuffer(buffer, 512, pDumpBytes);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv) {
    SysTools::addLog("***SysVfsFlashDisk::diskInitialize, pdrv: %u, diskSectorSize: %lu, diskSectorCount: %lu", pDrv, _vfsDiskOptions.sectorSize(), _vfsDiskOptions.sectorCount());
        
    return RES_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus(uint8_t pdrv) {
    //SysTools::addLog("***SysVfsFlashDisk::diskStatus, pdrv: %u", pdrv);
    return RES_OK;
    
    //STA_NOINIT    * Drive not initialized  *
    //STA_NODISK    * No medium in the drive *
    //STA_PROTECT	* Write protected        *
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl(uint8_t pdrv, uint8_t cmd, void* buff) {
    //SysTools::addLog("SysVfsFlashDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("***SysVfsFlashDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
    } else if(cmd == 1) {
        SysTools::addLog("***SysVfsFlashDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", _vfsDiskOptions.sectorCount());
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.sectorCount();
    } else if(cmd == 2) {
        SysTools::addLog("***SysVfsFlashDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i", _vfsDiskOptions.sectorSize());
        ((uint16_t*)buff)[0] = (uint16_t)_vfsDiskOptions.sectorSize();
    } else if(cmd == 3) {
        SysTools::addLog("***SysVfsFlashDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", _vfsDiskOptions.diskTableSectors);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskTableSectors;
    } else if(cmd == 4) {
        SysTools::addLog("***SysVfsFlashDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("***SysVfsFlashDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeSector(uint32_t pStartSector, const uint8_t* pBuffer, uint32_t pBufferSize) {
  //SysTools::addLog("SysFlashDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
  uint32_t hasFailed;
  const uint16_t startBlock = (pStartSector*512)/4096;
  const uint16_t byteOffset = (pStartSector*512) - (startBlock*4096);
  const uint16_t maxBufferSize = 4096 - byteOffset;

   //SysTools::addLog("SysFlashDisk::writeRAW, replace flash memory, pStartSector: %lu, startBlock: %lu, byteOffset: %lu, bufferSize: %lu, maxBufferSize: %lu", pStartSector, startBlock, byteOffset, pBufferSize, maxBufferSize);
   //dumpBuffer(pBuffer, 512, 32);

   if(pBufferSize > maxBufferSize) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: pBufferSize %lu greater than maxBufferSize: %lu", pBufferSize, maxBufferSize);
        return 0;       
    }
  
    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, _diskBuff, 4096);       
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: esp_partition_read_raw failed, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    memcpy(_diskBuff+byteOffset, pBuffer, 512);

    hasFailed = esp_partition_erase_range(_ffatPartition, startBlock*4096, 4096);   
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: esp_partition_erase_range failed: %lu", hasFailed);
        SysTools::addLog("SysFlashDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    hasFailed = esp_partition_write_raw(_ffatPartition, startBlock*4096, _diskBuff, 4096);    
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: esp_partition_read failed");
        SysTools::addLog("SysFlashDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite(uint8_t pDiskNum, const uint8_t* pBuff, DWORD pSector, UINT pCount) {
  //SysTools::addLog("***SysVfsFlashDisk::diskWrite, pDiskNum: %u, pSector: %lu, pCount: %u", pDiskNum, pSector, pCount);
  //dumpBuffer(pBuff, 512, 32);

    for(int index=0; index < pCount; ++index) {
        if(writeSector(pSector+index, pBuff+(index*512), 512) != 512) {
            SysTools::addLog("***SysFlashDisk::disk_write, ABORT: writeSector failed, sector: %lu", pSector+index);
            return RES_ERROR;
        };       
    }
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskRead(BYTE pDiskNum, BYTE* pBuff, DWORD pSector, UINT pCount) {
  //SysTools::addLog("***SysVfsFlashDisk::diskRead, pDiskNum: %u, pSector: %lu, pCount: %u", pDiskNum, pSector, pCount);
   
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pSector*512, pBuff, pCount*512);    
    if(hasFailed) {
        SysTools::addLog("SysVfsFlashDisk::readRAW, ABORT: esp_partition_read failed, pSector: %lu, pCount: %lu, pBufferSize: %lu", pSector, pCount, pCount*512);
        return RES_ERROR;
    }

    //SysTools::dumpBuffer(pBuff, 512, 32);
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
bool writeRaw(uint8_t pdrv, uint8_t* buffer, uint32_t sector) {
    return diskWrite(pdrv, buffer, sector, 1) == ESP_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool readRaw(uint8_t pdrv, uint8_t* buffer, uint32_t sector) {
    return diskRead(pdrv, buffer, sector, 1) == ESP_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t enable(SysFatFs::vfsDiskOptions_t** pDiskOptions) {
  SysTools::addLog("SysVfsFlashDisk::enable Open Flash Disk");
  if(pDiskOptions != nullptr) pDiskOptions[0] = nullptr;
  uint8_t diskNum = 0xFF;
    
    if(ff_diskio_get_drive(&diskNum) != ESP_OK || diskNum == 0xFF) return diskNum;

    ff_diskio_impl_t diskioOptions = {
        diskInitialize,
        diskStatus,
        diskRead,
        diskWrite,
        diskIoctl
    };
    
    ff_diskio_register(diskNum, &diskioOptions);
    SysTools::addLog("SysVfsFlashDisk::enable, Disk I/O Functions Registered, diskNum: %u", diskNum);

    ///////////////////////////////
    //Set VFS Disk Options 
    ///////////////////////////////           
    memset(&_vfsDiskOptions, 0, sizeof(_vfsDiskOptions));

    _vfsDiskOptions.diskNum = diskNum;
    _vfsDiskOptions.diskPath = "x:";
    _vfsDiskOptions.fileSystem = "/flashDisk";
    _vfsDiskOptions.testFile = "/flashDisk/README.TXT";
    _vfsDiskOptions.testDirectory = "x:/firmware";
    _vfsDiskOptions.diskTableSectors = 1;
    _vfsDiskOptions.diskType    = &diskType;
    _vfsDiskOptions.sectorSize  = &sectorSize;
    _vfsDiskOptions.sectorCount = &sectorCount;
    _vfsDiskOptions.readRaw     = &readRaw;
    _vfsDiskOptions.writeRaw    = &writeRaw;

    ((uint8_t*)_vfsDiskOptions.diskPath)[0]      = diskNum+'0';
    ((uint8_t*)_vfsDiskOptions.testDirectory)[0] = diskNum+'0';
        
    SysTools::addLog("SysCardDisk::open, Set _vfsDiskOptions Complete\n");
    if(pDiskOptions != nullptr) pDiskOptions[0] = &_vfsDiskOptions;    
    return diskNum;    
};
} //namespace SysFlashDisk

