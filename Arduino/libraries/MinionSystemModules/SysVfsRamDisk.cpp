//Sytem Required Libraries
#include <stdio.h>
//#include <dirent.h>
#include <diskio_impl.h>

//My Required Libraries
#include <SysTools.h>
#include <SysVfsFatFs.h>
#include <SysVfsRamDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysRamDisk {
/////////////////////////////////////////////////////////////////////////////////////////////////
SysFatFs::vfsDiskOptions_t _vfsDiskOptions;
uint8_t* _ramDisk;
const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");

/////////////////////////////////////////////////////////////////////////////////////////////////
const char* diskType() {
 
    return "RamDsk";
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t sectorCount() {

    return 200;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t sectorSize() {
    return 512;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpSector(uint16_t pSector, uint16_t pDumpBytes) {
    SysTools::addLog("SysRamDisk::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        
    uint8_t buffer[512];
    memset(&buffer, 0, sizeof(buffer));
        
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pSector*512, buffer, sizeof(buffer));    
    if(hasFailed) {
        SysTools::addLog("SysRamDisk::dumpSector, ABORT: esp_partition_read_raw failed");
        SysTools::addLog("SysRamDisk::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        return;
    }

    SysTools::dumpBuffer(buffer, 512, pDumpBytes);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv) {
    SysTools::addLog("***SysRamDisk::diskInitialize, pdrv: %u, diskSectorSize: %lu, diskSectorCount: %lu", pDrv, _vfsDiskOptions.sectorSize(), _vfsDiskOptions.sectorCount());
    return RES_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus(uint8_t pdrv) {
    //SysTools::addLog("***SysRamDisk::diskStatus, pdrv: %u", pdrv);
    return RES_OK;
    
    //STA_NOINIT    * Drive not initialized  *
    //STA_NODISK    * No medium in the drive *
    //STA_PROTECT	* Write protected        *
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl(uint8_t pdrv, uint8_t cmd, void* buff) {
    //SysTools::addLog("SysRamDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("***SysRamDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
    } else if(cmd == 1) {
        SysTools::addLog("***SysRamDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", _vfsDiskOptions.sectorCount());
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.sectorCount();
    } else if(cmd == 2) {
        SysTools::addLog("***SysRamDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i", _vfsDiskOptions.sectorSize());
        ((uint16_t*)buff)[0] = (uint16_t)_vfsDiskOptions.sectorSize();
    } else if(cmd == 3) {
        SysTools::addLog("***SysRamDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", _vfsDiskOptions.diskTableSectors);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskTableSectors;
    } else if(cmd == 4) {
        SysTools::addLog("***SysRamDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("***SysRamDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite(uint8_t pDiskNum, const uint8_t* pBuff, DWORD pSector, UINT pCount) {
  //SysTools::addLog("***SysVfsFlashDisk::diskWrite, pDiskNum: %u, pSector: %lu, pCount: %u", pDiskNum, pSector, pCount);
  //dumpBuffer(pBuff, 512, 32);
    memcpy(_ramDisk+(pSector*512), pBuff, pCount*512);
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskRead(BYTE pDiskNum, BYTE* pBuff, DWORD pSector, UINT pCount) {
  //SysTools::addLog("***SysVfsFlashDisk::diskRead, pDiskNum: %u, pSector: %lu, pCount: %u", pDiskNum, pSector, pCount);
    memcpy(pBuff, _ramDisk+(pSector*512), pCount*512);

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
    _ramDisk = (uint8_t*)malloc(sectorCount() * 512);
    memset(_ramDisk, 0, sectorCount() * 512);
    memset(&_vfsDiskOptions, 0, sizeof(_vfsDiskOptions));

    _vfsDiskOptions.diskNum = diskNum;
    _vfsDiskOptions.diskPath = "x:";
    _vfsDiskOptions.fileSystem = "/ramDisk";
    _vfsDiskOptions.testFile = "/ramDisk/README.TXT";
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
} //namespace SysRamDisk

