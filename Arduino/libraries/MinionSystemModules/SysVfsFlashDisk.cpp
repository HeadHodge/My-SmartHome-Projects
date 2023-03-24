#include <stdio.h>
#include <dirent.h>

#include <FF.h>
#include <esp_vfs_fat.h>
#include <diskio_impl.h>

//located in my github repository
#include <SysTools.h>
#include <SysVfsFlashDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.
namespace SysVfsFlashDisk {

// You only need to format FFat the first time you run a test
#define FORMAT_FFAT true

#define FAT_U8(v) ((v) & 0xFF)
#define FAT_U16(v) FAT_U8(v), FAT_U8((v) >> 8)
#define FAT_U32(v) FAT_U8(v), FAT_U8((v) >> 8), FAT_U8((v) >> 16), FAT_U8((v) >> 24)
#define FAT_MS2B(s,ms)    FAT_U8(((((s) & 0x1) * 1000) + (ms)) / 10)
#define FAT_HMS2B(h,m,s)  FAT_U8(((s) >> 1)|(((m) & 0x7) << 5)),      FAT_U8((((m) >> 3) & 0x7)|((h) << 3))
#define FAT_YMD2B(y,m,d)  FAT_U8(((d) & 0x1F)|(((m) & 0x7) << 5)),    FAT_U8((((m) >> 3) & 0x1)|((((y) - 1980) & 0x7F) << 1))
#define FAT_TBL2B(l,h)    FAT_U8(l), FAT_U8(((l >> 8) & 0xF) | ((h << 4) & 0xF0)), FAT_U8(h >> 4)
#define FAT_FILECONTENTS "This is tinyusb's MassStorage Class demo.\r\n\r\nIf you find any bugs or get any questions, feel free to file an\r\nissue at github.com/hathach/tinyusb"

#define MSC_DRIVENAME "/mscDrive.bin"

// You only need to format FFat the first time you run a test
#define FFAT_FORMATFLG false
#define FFAT_MSCFILE "/mscDrive.bin"

//const uint16_t DISK_SECTOR_COUNT    = 64;   // 8KB is the smallest size that windows allow to mount
uint16_t DISK_SECTOR_COUNT      = 2815;   // 8KB is the smallest size that windows allow to mount
uint16_t DISK_SECTOR_SIZE       = 512;     // Should be 512
uint16_t DISK_SECTORS_PER_TABLE = 2; //each table sector can fit 170KB (340 sectors)

const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");
FATFS* _fatFS = nullptr;
bool   _isFormatting = false;
bool   _isFormatted = false;
bool   _isMounted = false;
bool   _dumpBootFlg = false;
bool   _isReadMode = true;
uint8_t* _formatBuff = nullptr;
uint8_t* _flashBuff = (uint8_t*)malloc(4096);

/*
const  uint8_t bootSectors[4][512] = {
  //------------- Block0: Boot Sector -------------//
  {
    // Header (62 bytes)
    0xEB, 0x3C, 0x90, //jump_instruction
    'M' , 'S' , 'D' , 'O' , 'S' , '5' , '.' , '0' , //oem_name
    FAT_U16(DISK_SECTOR_SIZE), //bytes_per_sector
    FAT_U8(1),    //sectors_per_cluster
    FAT_U16(1),   //reserved_sectors_count
    FAT_U8(1),    //file_alloc_tables_num
    FAT_U16(16),  //max_root_dir_entries
    FAT_U16(DISK_SECTOR_COUNT), //fat12_sector_num
    0xF8,         //media_descriptor
    FAT_U16(DISK_SECTORS_PER_TABLE),   //sectors_per_alloc_table;//FAT12 and FAT16
    FAT_U16(1),   //sectors_per_track;//A value of 0 may indicate LBA-only access
    FAT_U16(1),   //num_heads
    FAT_U32(0),   //hidden_sectors_count
    FAT_U32(0),   //total_sectors_32
    0x00,         //physical_drive_number;0x00 for (first) removable media, 0x80 for (first) fixed disk
    0x00,         //reserved
    0x29,         //extended_boot_signature;//should be 0x29
    FAT_U32(0x1234), //serial_number: 0x1234 => 1234
    'T' , 'i' , 'n' , 'y' , 'U' , 's' , 'b' , ' ' , 'M' , 'S' , 'C' , //volume_label padded with spaces (0x20)
    'F' , 'A' , 'T' , '1' , '6' , ' ' , ' ' , ' ' ,  //file_system_type padded with spaces (0x20)

    // Zero up to 2 last bytes of FAT magic code (448 bytes)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

     //boot signature (2 bytes)
    0x55, 0xAA
  },

  //------------- Block1: FAT12 Table -------------//
  {
    FAT_TBL2B(0xFF8, 0xFFF), FAT_TBL2B(0xFFF, 0x000) // first 2 entries must be 0xFF8 0xFFF, third entry is cluster end of readme file
  },

  //------------- Block2: Root Directory -------------//
  {
    // first entry is volume label
    'F' , 'l' , 'a' , 's' , 'h' , ' D' , ' i' , ' s' ,
    'k' , ' ' , ' ' , 
    0x08, //FILE_ATTR_VOLUME_LABEL
    0x00, 
    FAT_MS2B(0,0), 
    FAT_HMS2B(0,0,0),
    FAT_YMD2B(0,0,0), 
    FAT_YMD2B(0,0,0), 
    FAT_U16(0), 
    FAT_HMS2B(13,42,30),  //last_modified_hms
    FAT_YMD2B(2018,11,5), //last_modified_ymd
    FAT_U16(0), 
    FAT_U32(0),
  
    // second entry is readme file
    'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' ,//file_name[8]; padded with spaces (0x20)
    'T' , 'X' , 'T' ,     //file_extension[3]; padded with spaces (0x20)
    0x20,                 //file attributes: FILE_ATTR_ARCHIVE
    0x00,                 //ignore
    FAT_MS2B(1,980),      //creation_time_10_ms (max 199x10 = 1s 990ms)
    FAT_HMS2B(13,42,36),  //create_time_hms [5:6:5] => h:m:(s/2)
    FAT_YMD2B(2018,11,5), //create_time_ymd [7:4:5] => (y+1980):m:d
    FAT_YMD2B(2020,11,5), //last_access_ymd
    FAT_U16(0),           //extended_attributes
    FAT_HMS2B(13,44,16),  //last_modified_hms
    FAT_YMD2B(2019,11,5), //last_modified_ymd
    FAT_U16(2),           //start of file in cluster
    FAT_U32(sizeof(FAT_FILECONTENTS) - 1) //file size
  
  },

  //------------- Block3: Readme Content -------------//
  {
    FAT_FILECONTENTS
  }
 };
*/
 
/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorSize() {
    return DISK_SECTOR_SIZE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorCount() {
    return DISK_SECTOR_COUNT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpSector(uint16_t pSector, uint16_t pDumpBytes) {
    SysTools::addLog("SysPartitionDisk::dumpSector, pSector: %lu, pDumpBytes: %lu, volume: %s, _isMounted: %u", pSector, pDumpBytes, _ffatPartition->label, _isMounted);
        
    uint8_t buffer[512];
    memset(&buffer, 0, sizeof(buffer));
        
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pSector*512, buffer, sizeof(buffer));    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::dumpSector, ABORT: esp_partition_read_raw failed");
        SysTools::addLog("SysPartitionDisk::dumpSector, pSector: %lu, pDumpBytes: %lu", pSector, pDumpBytes);
        return;
    }

    SysTools::dumpBuffer(buffer, 512, pDumpBytes);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeRAW(uint32_t pStartSector, const uint8_t* pBuffer, uint32_t pBufferSize) {
  //SysTools::addLog("SysPartitionDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
  uint32_t hasFailed;
  const uint16_t startBlock = (pStartSector*512)/4096;
  const uint16_t byteOffset = (pStartSector*512) - (startBlock*4096);
  const uint16_t maxBufferSize = 4096 - byteOffset;

   //SysTools::addLog("SysPartitionDisk::writeRAW, replace flash memory, pStartSector: %lu, startBlock: %lu, byteOffset: %lu, bufferSize: %lu, maxBufferSize: %lu", pStartSector, startBlock, byteOffset, pBufferSize, maxBufferSize);
   //dumpBuffer(pBuffer, 512, 32);

   if(pBufferSize > maxBufferSize) {
        SysTools::addLog("syspartitiondisk::writeRAW, ABORT: pBufferSize %lu greater than maxBufferSize: %lu", pBufferSize, maxBufferSize);
        return 0;       
    }
  
    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, _flashBuff, 4096);       
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read_raw failed, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    memcpy(_flashBuff+byteOffset, pBuffer, 512);

    hasFailed = esp_partition_erase_range(_ffatPartition, startBlock*4096, 4096);   
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_erase_range failed: %lu", hasFailed);
        SysTools::addLog("SysPartitionDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    hasFailed = esp_partition_write_raw(_ffatPartition, startBlock*4096, _flashBuff, 4096);    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read failed");
        SysTools::addLog("SysPartitionDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }

    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
    //SysTools::addLog("***SysPartitionDisk::disk_write, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);
    //dumpBuffer(buff, 512, 32);
    
    if(!_flashBuff) {
        SysTools::addLog("SysPartitionDisk::diskWrite, ABORT: calloc '_flashBuff' failed");
        return RES_ERROR;
    }
    
    for(int i=0; i < count; ++i) {
        if(writeRAW(sector+i, buff+(i*512), 512) != 512) {
            SysTools::addLog("***SysPartitionDisk::disk_write, ABORT: writeRAW failed");
            return RES_ERROR;
        };       
    }
   
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t readRAW(uint32_t pStartSector, uint8_t* pBuffer, uint32_t pBufferSize) {
    //SysTools::addLog("SysPartitionDisk::readRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
    
    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pStartSector*512, pBuffer, pBufferSize);    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::readRAW, ABORT: esp_partition_read failed");
        SysTools::addLog("SysPartitionDisk::readRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskRead (BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    //SysTools::addLog("***SysPartitionDisk::disk_read, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);

    readRAW(sector, buff, count*DISK_SECTOR_SIZE);
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
bool formatDisk() {
    SysTools::addLog("SysPartitionDisk::formatDisk, DISK_SECTOR_SIZE: %lu, DISK_SECTOR_COUNT: %lu", DISK_SECTOR_SIZE, DISK_SECTOR_COUNT);
    
    //Format Disk
    uint8_t* workBuff = nullptr;
    
    workBuff = (uint8_t*)malloc(FF_MAX_SS);	      
    if(!workBuff) {
        SysTools::addLog("SysPartitionDisk::formatDisk, ABORT: malloc 'workBuff' failed");
        return false;
    }
    
    uint8_t hasFailed = f_mkfs("", FM_ANY, 2048, workBuff, FF_MAX_SS); //drive, format, au size, workbuff, workbuffsize 

    free(workBuff);    
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::formatDisk, ABORT: f_mount disk, errCode: %u", hasFailed);
        return false;
    }

    //Formatted
    SysTools::addLog("SysPartitionDisk::formatDisk, DISK FORMATTED\n");   
    return true;
}    

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv) {
    SysTools::addLog("***SysPartitionDisk::diskInitialize, pdrv: %u, DISK_SECTOR_COUNT: %lu, DISK_SECTOR_COUNT: %lu", pDrv, DISK_SECTOR_SIZE, DISK_SECTOR_COUNT);
        
    return RES_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus (BYTE pdrv) {
    //SysTools::addLog("***SysPartitionDisk::diskStatus, pdrv: %u", pdrv);
    return 0;
    
    //STA_NOINIT    * Drive not initialized  *
    //STA_NODISK    * No medium in the drive *
    //STA_PROTECT	* Write protected        *
};

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpBoot() {
    _dumpBootFlg = true;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl (BYTE pdrv, BYTE cmd, void* buff) {
    //SysTools::addLog("SysPartitionDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("***SysPartitionDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
    } else if(cmd == 1) {
        SysTools::addLog("***SysPartitionDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", DISK_SECTOR_COUNT);
        ((uint32_t*)buff)[0] = (uint32_t)DISK_SECTOR_COUNT;
    } else if(cmd == 2) {
        SysTools::addLog("***SysPartitionDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i",DISK_SECTOR_SIZE);
        ((uint16_t*)buff)[0] = (uint16_t)DISK_SECTOR_SIZE;
    } else if(cmd == 3) {
        SysTools::addLog("***SysPartitionDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", 1);
        ((uint32_t*)buff)[0] = (uint32_t)1;
    } else if(cmd == 4) {
        SysTools::addLog("***SysPartitionDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("***SysPartitionDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enable() {
  SysTools::addLog("SysPartitionDisk::open, esp_partition found, address: 0x%04X, size: 0x%04X, label: %s \n", _ffatPartition->address, _ffatPartition->size, _ffatPartition->label);

    DISK_SECTORS_PER_TABLE = 2;
    DISK_SECTOR_SIZE = 512;
    DISK_SECTOR_COUNT = (_ffatPartition->size / 512) - 1;
    
    //esp_vfs_fat_register
    SysTools::addLog("SysPartitionDisk::open, esp_vfs_fat_register, FF_MIN_SS: %u, FF_MAX_SS: %u", FF_MIN_SS, FF_MAX_SS);
    uint8_t hasFailed = esp_vfs_fat_register("/fatdisk", "", 5, &_fatFS);
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: esp_vfs_fat_register failed, errCode: %u", hasFailed);
        return 0;
    }
    
    //esp_vfs_fat Registered
    SysTools::addLog("SysPartitionDisk::open, REGISTERED esp_vfs_fat_register\n");

    //ff_diskio_register
    SysTools::addLog("SysPartitionDisk::open, ff_diskio_register, pdrv: %u", _fatFS->pdrv);
    
    ff_diskio_impl_t diskioSettings = {
        diskInitialize,
        diskStatus,
        diskRead,
        diskWrite,
        diskIoctl
    };

    ff_diskio_register(_fatFS->pdrv, &diskioSettings);
    
    //ff_diskio Registered
    SysTools::addLog("SysPartitionDisk::open, REGISTERED ff_diskio_register\n");
  
    //Mount Disk
    SysTools::addLog("SysPartitionDisk::open, f_mount disk, pdrv: %u\n", _fatFS->pdrv);
    hasFailed = f_mount(_fatFS, "", 0); //0-delay, 1-immediate
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_mount disk, errCode: %u", hasFailed);
        return 0;
    }

    //Disk Mounted
    _isMounted = true;
    SysTools::addLog("SysPartitionDisk::open, f_mount: DISK MOUNTED: %lu \n", disk_status(_fatFS->pdrv));

    //Open Directory
    if(opendir("/fatdisk") == NULL) {
        SysTools::addLog("opendir failed");
        
        //FORMAT DISK
        formatDisk();
        if(formatDisk() != true) {
            SysTools::addLog("SysPartitionDisk::open, ABORT: formatFat16FlashDisk failed");
            return false;
        }
        
        //Change Volume Label
        //SysTools::addLog("SysPartitionDisk::open, Change Volume Label: %lu", f_setlabel("FLASH DISK"));

        //CREATE FILE
        SysTools::addLog("SysPartitionDisk::open, Create File: '/fatdisk/README.TXT'");
    
        FILE* fp = fopen("/fatdisk/README.TXT", "w"); // "w" defines "writing mode"
        if(fp == NULL)
        {
            SysTools::addLog("SysPartitionDisk::open, Could not create file '/fatdisk/README.TXT' \n");
            return false;
        }
    
        fputs("Hello World", fp);
        fclose(fp);
        
        f_mkdir("firmware");        
    }
    
    _isFormatted = true;
    SysTools::addLog("SysPartitionDisk::open, '/fatdisk' is formatted");
    
    //All Done
    SysTools::addLog("SysPartitionDisk::open, Open Flash Disk Completed \n");
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
}