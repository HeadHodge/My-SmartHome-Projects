//Sytem Required Libraries
#include <stdio.h>
#include <dirent.h>
#include <diskio_impl.h>
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>

//My Required Libraries
#include <SysTools.h>
//#include <SysVfsBridge.h>
#include <SysVfsMmcDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysMmcDisk {
/////////////////////////////////////////////////////////////////////////////////////////////////
SysVfsBridge::vfsDiskOptions_t _vfsDiskOptions;
sdmmc_card_t* _card = nullptr;
esp_err_t _espRC = 0;
uint16_t  _sectorSize = 0;
uint32_t  _diskCapacity = 0;
uint8_t*  _readBuffer = (uint8_t*)heap_caps_malloc(512, MALLOC_CAP_DMA);

const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");

/////////////////////////////////////////////////////////////////////////////////////////////////
const char* diskType() {
/////////////////////////////////////////////////////////////////////////////////////////////////
 
    return "mmcDsk";
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t sectorCount() {
/////////////////////////////////////////////////////////////////////////////////////////////////
    //return 5120;
    //return (uint32_t)(_diskCapacity/_sectorSize);
    return _diskCapacity;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t sectorSize() {
/////////////////////////////////////////////////////////////////////////////////////////////////
    //return 512;
    return _sectorSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv) {
/////////////////////////////////////////////////////////////////////////////////////////////////
    SysTools::addLog("***SysMmcDisk::diskInitialize, pdrv: %u, diskSectorSize: %lu, diskSectorCount: %lu", pDrv, _vfsDiskOptions.sectorSize(), _vfsDiskOptions.sectorCount());
    return RES_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus(uint8_t pdrv) {
/////////////////////////////////////////////////////////////////////////////////////////////////
    //SysTools::addLog("***SysMmcDisk::diskStatus, pdrv: %u", pdrv);
    return RES_OK;
    
    //STA_NOINIT    * Drive not initialized  *
    //STA_NODISK    * No medium in the drive *
    //STA_PROTECT	* Write protected        *
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl(uint8_t pdrv, uint8_t cmd, void* buff) {
/////////////////////////////////////////////////////////////////////////////////////////////////
    //SysTools::addLog("SysMmcDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("***SysMmcDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
    } else if(cmd == 1) {
        SysTools::addLog("***SysMmcDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", _vfsDiskOptions.sectorCount());
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.sectorCount();
    } else if(cmd == 2) {
        SysTools::addLog("***SysMmcDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i", _vfsDiskOptions.sectorSize());
        ((uint16_t*)buff)[0] = (uint16_t)_vfsDiskOptions.sectorSize();
    } else if(cmd == 3) {
        SysTools::addLog("***SysMmcDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", _vfsDiskOptions.diskTableSectors);
        ((uint32_t*)buff)[0] = (uint32_t)_vfsDiskOptions.diskTableSectors;
    } else if(cmd == 4) {
        SysTools::addLog("***SysMmcDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("***SysMmcDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite(uint8_t pDiskNum, const uint8_t* pBuff, DWORD pSector, UINT pCount) {
/////////////////////////////////////////////////////////////////////////////////////////////////
  //SysTools::addLog("***SysVfsMmcDisk::diskWrite, pDiskNum: %u, pSector: %lu, pCount: %u", pDiskNum, pSector, pCount);
  
    if(sdmmc_write_sectors(_card, pBuff, pSector, pCount)!= ESP_OK) {
        SysTools::addLog("***SysVfsMmcDisk::diskWrite, FAILED");
        return RES_ERROR;
    }
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskRead(BYTE pDiskNum, BYTE* pBuff, DWORD pSector, UINT pCount) {
/////////////////////////////////////////////////////////////////////////////////////////////////
  //SysTools::addLog("***SysVfsMmcDisk::diskRead, pDiskNum: %u, pSector: %lu, pCount: %u", pDiskNum, pSector, pCount);
  
    if(sdmmc_read_sectors(_card, pBuff, pSector, pCount)!= ESP_OK) {
        SysTools::addLog("***SysVfsMmcDisk::diskRead, FAILED");
        return RES_ERROR;
    }
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
bool writeRaw(uint8_t pdrv, uint8_t* buffer, uint32_t sector) {
/////////////////////////////////////////////////////////////////////////////////////////////////
    return (diskWrite(pdrv, buffer, sector, 1) == RES_OK);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool readRaw(uint8_t pdrv, uint8_t* buffer, uint32_t sector) {
/////////////////////////////////////////////////////////////////////////////////////////////////
    return (diskRead(pdrv, buffer, sector, 1) == RES_OK);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void onRemove() {
/////////////////////////////////////////////////////////////////////////////////////////////////
  SysTools::addLog("SysVfsMmcDisk::onRemove");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void onInsert() {
/////////////////////////////////////////////////////////////////////////////////////////////////
  SysTools::addLog("SysVfsMmcDisk::onInsert");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool onFormatted() {
/////////////////////////////////////////////////////////////////////////////////////////////////
  SysTools::addLog("SysVfsMmcDisk::onFormatted");
  DIR *dp;
  struct dirent *ep;     

    //ADD DEFAULT DIRECTORIES
    mkdir("/firmware", 0777);
    mkdir("/system", 0777);

    //ADD FILE
    SysTools::addLog("SysVfsMmcDisk::onFormatted, Create Test File: '%s'", "/ReadMe.txt");
    
    FILE* fp = fopen("/ReadMe.txt", "w"); // "w" defines "writing mode"
        
    if(fp == NULL){
        SysTools::addLog("SysVfsMmcDisk::onFormatted, Could not create file '%s' \n", "/ReadMe.txt");
        return false;
    }
    
    fputs("Welcome!\nThanks for Using 'smartRemotes'\nBy: http://minionLogic.com", fp);
    fclose(fp);
    
    //DUMP FILE
    SysTools::addLog("SysVfsMmcDisk::onFormatted, Dump Test File: '%s'", "/ReadMe.txt");
    fp = fopen("/ReadMe.txt","r");
    int c;

    Serial.printf("%c", '\n');
    while(1) {
      c = fgetc(fp);
      if(feof(fp)) break ;
      Serial.printf("%c", c);
    }
    Serial.printf("%c%c", '\n', '\n');
   
    fclose(fp);
        
    //LIST DIR
    SysTools::addLog("SysVfsMmcDisk::onFormatted, List Directory");
    dp = opendir("/");
    
    //list directory
    while ((ep = readdir (dp)) != NULL) SysTools::addLog("SysVfsMmcDisk::onFormatted, fileName: %s", ep->d_name);          
    (void) closedir (dp);

    Serial.println("");
    return true;
}
   
/////////////////////////////////////////////////////////////////////////////////////////////////
bool enableMmcDrive() {
/////////////////////////////////////////////////////////////////////////////////////////////////
  SysTools::addLog("SysVfsMmcDisk::enableMmcDrive");
   
    //Set Slot Options
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = (gpio_num_t) 36;
    slot_config.cmd = (gpio_num_t) 35;
    slot_config.d0  = (gpio_num_t) 37;
    slot_config.d1  = (gpio_num_t) 38;
    slot_config.d2  = (gpio_num_t) 33;
    slot_config.d3  = (gpio_num_t) 34;
    slot_config.width = 4;

    //sdmmc_host_init
    _espRC = sdmmc_host_init();    
    SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, sdmmc_host_init: %i", _espRC);
    if(_espRC != ESP_OK) {
        SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, ABORT: sdmmc_host_init");
        return false;
    }

    //sdmmc_host_init_slot
    _espRC = sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, sdmmc_host_init_slot: %i", _espRC); 
    if(_espRC != ESP_OK) {
        SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, ABORT: sdmmc_host_init_slot failed");
        return false;
    }

    //Set Host Options
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.flags = SDMMC_HOST_FLAG_4BIT;
    host.slot = SDMMC_HOST_SLOT_1;
    host.max_freq_khz = BOARD_MAX_SDMMC_FREQ;

    _card = (sdmmc_card_t*)malloc(sizeof(sdmmc_card_t));    
    if(_card == nullptr) {
        SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, ABORT: Allocate card failed");
        return false;
    }

    //sdmmc_card_init
    _espRC = sdmmc_card_init(&host, _card);
    if(_espRC != ESP_OK) {
        SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, ABORT: sdmmc_card_init failed");
        return false;
    }
    SysTools::addLog("SysVfsMmcDisk::enableMmcDrive, sdmmc_card_init: %i", _espRC); 
    
    //Success
    _sectorSize = _card->csd.sector_size;
    _diskCapacity = _card->csd.capacity;
    sdmmc_card_print_info(stdout, _card);
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t enable(SysVfsBridge::vfsDiskOptions_t** pDiskOptions) {
/////////////////////////////////////////////////////////////////////////////////////////////////
  SysTools::addLog("SysVfsMmcDisk::enable Open MMC Disk");
  if(pDiskOptions != nullptr) pDiskOptions[0] = nullptr;
  uint8_t diskNum = 0xFF;
    
    if(!enableMmcDrive()) {
        SysTools::addLog("SysVfsMmcDisk::enable ABORT: enableMmcDrive Failed");
        return diskNum;
    }
        
    if(ff_diskio_get_drive(&diskNum) != ESP_OK || diskNum == 0xFF) return diskNum;

    ff_diskio_impl_t diskioOptions = {
        diskInitialize,
        diskStatus,
        diskRead,
        diskWrite,
        diskIoctl
    };
    
    ff_diskio_register(diskNum, &diskioOptions);
    SysTools::addLog("SysVfsMmcDisk::enable, Disk I/O Functions Registered, diskNum: %u", diskNum);

    ///////////////////////////////
    //Set VFS Disk Options 
    ///////////////////////////////           
    memset(&_vfsDiskOptions, 0, sizeof(_vfsDiskOptions));

    _vfsDiskOptions.diskNum = diskNum;
    _vfsDiskOptions.diskPath = "x:";
    _vfsDiskOptions.fileSystem = "/mmcDisk";
    _vfsDiskOptions.mountDirectory = "";
    _vfsDiskOptions.testFile = "/mmcDisk/ReadMe.txt";
    _vfsDiskOptions.testDirectory = "x:/firmware";
    _vfsDiskOptions.diskTableSectors = 1;
    _vfsDiskOptions.diskType    = &diskType;
    _vfsDiskOptions.sectorSize  = &sectorSize;
    _vfsDiskOptions.sectorCount = &sectorCount;
    _vfsDiskOptions.readRaw     = &readRaw;
    _vfsDiskOptions.writeRaw    = &writeRaw;
    _vfsDiskOptions.onInsert    = &onInsert;
    _vfsDiskOptions.onRemove    = &onRemove;
    _vfsDiskOptions.onFormatted = &onFormatted;

    ((uint8_t*)_vfsDiskOptions.diskPath)[0]      = diskNum+'0';
    ((uint8_t*)_vfsDiskOptions.testDirectory)[0] = diskNum+'0';
        
    SysTools::addLog("SysCardDisk::open, Set _vfsDiskOptions Complete\n");
    if(pDiskOptions != nullptr) pDiskOptions[0] = &_vfsDiskOptions;    
    return diskNum;    
};
} //namespace SysMmcDisk
