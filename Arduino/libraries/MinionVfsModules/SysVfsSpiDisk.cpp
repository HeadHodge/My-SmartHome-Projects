#include <stdio.h>
#include <dirent.h>
#include <sd_diskio.h>

//My Required Libraries
#include <SysTools.h>
//#include <SysVfsBridge.h>
#include <SysVfsSpiDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysSpiDisk {
/////////////////////////////////////////////////////////////////////////////////////////////////
SysVfsBridge::vfsDiskOptions_t _vfsDiskOptions;
uint8_t* _diskBuff = (uint8_t*)malloc(4096);

/////////////////////////////////////////////////////////////////////////////////////////////////
const char* diskType() {
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

/////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t enable(SysVfsBridge::vfsDiskOptions_t** pDiskOptions) {
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
    _vfsDiskOptions.diskType    = &diskType;
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
} //namespace SysSpiDisk

