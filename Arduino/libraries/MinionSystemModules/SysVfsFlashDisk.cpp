#include <stdio.h>
#include <dirent.h>


//My Required Co-Libraries
#include <SysTools.h>
#include <SysVfsFatFs.h>
#include <SysVfsFlashDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysFlashDisk {
/////////////////////////////////////////////////////////////////////////////////////////////////
SysVfsFatFs::vfsDiskOptions_t _vfsDiskOptions;
const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");
uint8_t* _flashBuff = (uint8_t*)malloc(4096);

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpSector(uint16_t pSector, uint16_t pDumpBytes) {
    SysTools::addLog("SysVfsFatFs::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        
    uint8_t buffer[512];
    memset(&buffer, 0, sizeof(buffer));
        
    uint32_t hasFailed = esp_partition_read_raw(SysFlashDisk::_ffatPartition, pSector*512, buffer, sizeof(buffer));    
    if(hasFailed) {
        SysTools::addLog("SysVfsFatFs::dumpSector, ABORT: esp_partition_read_raw failed");
        SysTools::addLog("SysVfsFatFs::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        return;
    }

    SysTools::dumpBuffer(buffer, 512, pDumpBytes);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv) {
    SysTools::addLog("***SysFlashDisk::diskInitialize, pdrv: %u, diskSectorSize: %lu, diskSectorCount: %lu", pDrv, _vfsDiskOptions.diskSectorSize, _vfsDiskOptions.diskSectorCount);
        
    return RES_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus(uint8_t pdrv) {
    //SysTools::addLog("***SysFlashDisk::diskStatus, pdrv: %u", pdrv);
    return RES_OK;
    
    //STA_NOINIT    * Drive not initialized  *
    //STA_NODISK    * No medium in the drive *
    //STA_PROTECT	* Write protected        *
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl(uint8_t pdrv, uint8_t cmd, void* buff) {
    //SysTools::addLog("SysFlashDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("***SysFlashDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
    } else if(cmd == 1) {
        SysTools::addLog("***SysFlashDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", _vfsDiskOptions.diskSectorCount);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskSectorCount;
    } else if(cmd == 2) {
        SysTools::addLog("***SysFlashDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i", _vfsDiskOptions.diskSectorSize);
        ((uint16_t*)buff)[0] = (uint16_t)_vfsDiskOptions.diskSectorSize;
    } else if(cmd == 3) {
        SysTools::addLog("***SysFlashDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", _vfsDiskOptions.diskTableSectors);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskTableSectors;
    } else if(cmd == 4) {
        SysTools::addLog("***SysFlashDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("***SysFlashDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeRAW(uint32_t pStartSector, const uint8_t* pBuffer, uint32_t pBufferSize) {
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
  
    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, _flashBuff, 4096);       
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: esp_partition_read_raw failed, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    memcpy(_flashBuff+byteOffset, pBuffer, 512);

    hasFailed = esp_partition_erase_range(_ffatPartition, startBlock*4096, 4096);   
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: esp_partition_erase_range failed: %lu", hasFailed);
        SysTools::addLog("SysFlashDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    hasFailed = esp_partition_write_raw(_ffatPartition, startBlock*4096, _flashBuff, 4096);    
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::writeRAW, ABORT: esp_partition_read failed");
        SysTools::addLog("SysFlashDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t readRAW(uint32_t pStartSector, uint8_t* pBuffer, uint32_t pBufferSize) {
    //SysTools::addLog("SysFlashDisk::readRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
    
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pStartSector*512, pBuffer, pBufferSize);    
    if(hasFailed) {
        SysTools::addLog("SysFlashDisk::readRAW, ABORT: esp_partition_read failed");
        SysTools::addLog("SysFlashDisk::readRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite(uint8_t pdrv, const uint8_t* buff, DWORD sector, UINT count) {
    //SysTools::addLog("***SysFlashDisk::disk_write, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);
    //dumpBuffer(buff, 512, 32);

    for(int i=0; i < count; ++i) {
        if(writeRAW(sector+i, buff+(i*512), 512) != 512) {
            SysTools::addLog("***SysFlashDisk::disk_write, ABORT: writeRAW failed");
            return RES_ERROR;
        };       
    }
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskRead (BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    //SysTools::addLog("***SysFlashDisk::disk_read, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);

    if(readRAW(sector, buff, count*_vfsDiskOptions.diskSectorSize) != count*_vfsDiskOptions.diskSectorSize) {
        SysTools::addLog("***SysFlashDisk::disk_read, ABORT: Bytes Read != '%lu'", count*_vfsDiskOptions.diskSectorSize);
        return RES_ERROR;
    }

    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enable(SysVfsFatFs::vfsDiskOptions_t** pDiskOptions) {
  SysTools::addLog("SysFlashDisk::enable Open Flash Disk");
    
    memset(&_vfsDiskOptions, 0, sizeof(_vfsDiskOptions));
    //_vfsDiskOptions.diskNum = diskNum;
    _vfsDiskOptions.diskPath = "x:";
    _vfsDiskOptions.fileSystem = "/flashDisk";
    _vfsDiskOptions.testFile = "/flashDisk/README.TXT";
    _vfsDiskOptions.testDirectory = "x:/firmware";

    ((uint8_t*)_vfsDiskOptions.diskPath)[0]      = '0';
    ((uint8_t*)_vfsDiskOptions.testDirectory)[0] = '0';
    
    _vfsDiskOptions.diskTableSectors = 1;
    _vfsDiskOptions.diskSectorSize = 512;
    _vfsDiskOptions.diskSectorCount = (_ffatPartition->size / 512) - 1;

    if(pDiskOptions == nullptr) return false;
    //memcpy(pDiskOptions, &_vfsDiskOptions, sizeof(_vfsDiskOptions));
    pDiskOptions[0] = &_vfsDiskOptions;
    
    SysTools::addLog("SysFlashDisk::enable, Flash Disk Opened, address: 0x%04X, size: 0x%04X, label: %s \n", _ffatPartition->address, _ffatPartition->size, _ffatPartition->label);
    return true;
};
} //namespace SysFlashDisk

