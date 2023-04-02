#include <stdio.h>
#include <dirent.h>
#include <sd_diskio.h>

//My Required Libraries
#include <SysTools.h>
#include <SysVfsFatFs.h>
#include <SysVfsCardDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysCardDisk {
/////////////////////////////////////////////////////////////////////////////////////////////////
SysVfsFatFs::vfsDiskOptions_t _vfsDiskOptions;
uint8_t* _diskBuff = (uint8_t*)malloc(4096);

/////////////////////////////////////////////////////////////////////////////////////////////////
const char* mediaType() {
    sdcard_type_t cardType = sdcard_type(_vfsDiskOptions.diskNum);

    if(cardType == CARD_MMC){
       return "MMC";
    } else if(cardType == CARD_SD){
        return "SDSC";
    } else if(cardType == CARD_SDHC){
        return "SDHC";
    } else if(cardType == CARD_NONE){
        return "NONE";
    };

    return "UNKNOWN";
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t sectorCount() {
    return sdcard_num_sectors(_vfsDiskOptions.diskNum);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t sectorSize() {
    return sdcard_sector_size(_vfsDiskOptions.diskNum);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpSector(uint16_t pSector, uint16_t pDumpBytes) {
    SysTools::addLog("SysVfsFatFs::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        
    uint8_t buffer[512];
    memset(&buffer, 0, sizeof(buffer));
    
    /*
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pSector*512, buffer, sizeof(buffer));    
    if(hasFailed) {
        SysTools::addLog("SysVfsFatFs::dumpSector, ABORT: esp_partition_read_raw failed");
        SysTools::addLog("SysVfsFatFs::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        return;
    }
    */
    
    SysTools::dumpBuffer(buffer, 512, pDumpBytes);
};
/*
/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv) {
    SysTools::addLog("***SysCardDisk::diskInitialize, pdrv: %u, diskSectorSize: %lu, diskSectorCount: %lu", pDrv, _vfsDiskOptions.diskSectorSize, _vfsDiskOptions.diskSectorCount);
        
    return RES_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus(uint8_t pdrv) {
    //SysTools::addLog("***SysCardDisk::diskStatus, pdrv: %u", pdrv);
    return RES_OK;
    
    //STA_NOINIT    * Drive not initialized  *
    //STA_NODISK    * No medium in the drive *
    //STA_PROTECT	* Write protected        *
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl(uint8_t pdrv, uint8_t cmd, void* buff) {
    //SysTools::addLog("SysCardDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("***SysCardDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
    } else if(cmd == 1) {
        SysTools::addLog("***SysCardDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", _vfsDiskOptions.diskSectorCount);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskSectorCount;
    } else if(cmd == 2) {
        SysTools::addLog("***SysCardDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i", _vfsDiskOptions.diskSectorSize);
        ((uint16_t*)buff)[0] = (uint16_t)_vfsDiskOptions.diskSectorSize;
    } else if(cmd == 3) {
        SysTools::addLog("***SysCardDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", _vfsDiskOptions.diskTableSectors);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskTableSectors;
    } else if(cmd == 4) {
        SysTools::addLog("***SysCardDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("***SysCardDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    return RES_OK;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite(uint8_t pdrv, const uint8_t* buff, DWORD sector, UINT count) {
    //SysTools::addLog("***SysCardDisk::disk_write, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);
    //dumpBuffer(buff, 512, 32);

    for(int i=0; i < count; ++i) {
        if(writeRAW(sector+i, buff+(i*512), 512) != 512) {
            SysTools::addLog("***SysCardDisk::disk_write, ABORT: writeRAW failed");
            return RES_ERROR;
        };       
    }
    
    return RES_OK;
};

DRESULT diskRead (BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    //SysTools::addLog("***SysCardDisk::disk_read, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);

    if(readRAW(sector, buff, count*_vfsDiskOptions.diskSectorSize) != count*_vfsDiskOptions.diskSectorSize) {
        SysTools::addLog("***SysCardDisk::disk_read, ABORT: Bytes Read != '%lu'", count*_vfsDiskOptions.diskSectorSize);
        return RES_ERROR;
    }

    return RES_OK;
};

*/

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeRAW(uint32_t pStartSector, const uint8_t* pBuffer, uint32_t pBufferSize) {
  SysTools::addLog("SysCardDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
  uint32_t hasFailed;

/*

  const uint16_t startBlock = (pStartSector*512)/4096;
  const uint16_t byteOffset = (pStartSector*512) - (startBlock*4096);
  const uint16_t maxBufferSize = 4096 - byteOffset;

   if(pBufferSize > maxBufferSize) {
        SysTools::addLog("SysCardDisk::writeRAW, ABORT: pBufferSize %lu greater than maxBufferSize: %lu", pBufferSize, maxBufferSize);
        return 0;       
    }
  
    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, _diskBuff, 4096);       
    if(hasFailed) {
        SysTools::addLog("SysCardDisk::writeRAW, ABORT: esp_partition_read_raw failed, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    memcpy(_diskBuff+byteOffset, pBuffer, 512);

    hasFailed = esp_partition_erase_range(_ffatPartition, startBlock*4096, 4096);   
    if(hasFailed) {
        SysTools::addLog("SysCardDisk::writeRAW, ABORT: esp_partition_erase_range failed: %lu", hasFailed);
        SysTools::addLog("SysCardDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    hasFailed = esp_partition_write_raw(_ffatPartition, startBlock*4096, _diskBuff, 4096);    
    if(hasFailed) {
        SysTools::addLog("SysCardDisk::writeRAW, ABORT: esp_partition_read failed");
        SysTools::addLog("SysCardDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
*/
    return pBufferSize;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t readRAW(uint32_t pStartSector, uint8_t* pBuffer, uint32_t pBufferSize) {
    SysTools::addLog("SysCardDisk::readRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);

    for(int i=0; i < pBufferSize/512; ++i) {
        SysTools::addLog("SysCardDisk::readRAW, Read Sector: %lu", pStartSector + i);

        //if(!SD.readRAW(pBuffer + (i * 512), pStartSector + i)) {
            SysTools::addLog("SysCardDisk::readRAW, ABORT: SD.readRAW failed");
            return 0;
        //}
    }
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t enable(SysVfsFatFs::vfsDiskOptions_t** pDiskOptions) {
  SysTools::addLog("SysCardDisk::enable, Open Card Disk");
  if(pDiskOptions != nullptr) pDiskOptions[0] = nullptr;

    ///////////////////////////////
    //Register Disk I/O Functions
    ///////////////////////////////
    SPI.begin();
    uint8_t diskNum = sdcard_init(SS, &SPI, 4000000);

    if(diskNum == -1) {
        SysTools::addLog("SysCardDisk::enable, ABORT: Open Disk Failed, diskNum: %u", diskNum);
        return diskNum;    
    }

    SysTools::addLog("SysCardDisk::enable, Disk I/O Functions Registered, diskNum: %u", diskNum);

    ///////////////////////////////
    //Set VFS Disk Options 
    ///////////////////////////////           
    memset(&_vfsDiskOptions, 0, sizeof(_vfsDiskOptions));

    _vfsDiskOptions.diskNum = diskNum;
    _vfsDiskOptions.diskPath = "x:";
    _vfsDiskOptions.fileSystem = "/cardDisk";
    _vfsDiskOptions.testFile = "/cardDisk/README.TXT";
    _vfsDiskOptions.testDirectory = "x:/firmware";
    _vfsDiskOptions.diskTableSectors = 1;
    _vfsDiskOptions.mediaType   = &mediaType;
    _vfsDiskOptions.sectorSize  = &sectorSize;
    _vfsDiskOptions.sectorCount = &sectorCount;
    _vfsDiskOptions.readRaw     = &sd_read_raw;
    _vfsDiskOptions.writeRaw    = &sd_write_raw;

    ((uint8_t*)_vfsDiskOptions.diskPath)[0]      = diskNum+'0';
    ((uint8_t*)_vfsDiskOptions.testDirectory)[0] = diskNum+'0';
        
    SysTools::addLog("SysCardDisk::open, Set _vfsDiskOptions Complete\n");
    if(pDiskOptions != nullptr) pDiskOptions[0] = &_vfsDiskOptions;    
    return diskNum;    
 };
} //namespace SysCardDisk

