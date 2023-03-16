
//#include "FS.h"
//#include "FFat.h"
# include <stdio.h>
# include <string.h>
#include "FF.h"

//#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "esp_partition.h"

#include "SysFlashTools.h"
#include "SysTools.h"
#include "SysPartitionDisk.h"

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.
namespace SysPartitionDisk {

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

const uint16_t DISK_SECTOR_COUNT = 16; // 8KB is the smallest size that windows allow to mount
const uint16_t DISK_SECTOR_SIZE = 512;    // Should be 512
const uint16_t DISC_SECTORS_PER_TABLE = 1; //each table sector can fit 170KB (340 sectors)
const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");

FATFS* _fatFS = (FATFS*)malloc(sizeof (FATFS));
//File   _file;
bool   _isFormatted = false;
bool   _isReadMode = true;
uint16_t _sectorCount = DISK_SECTOR_COUNT;
uint16_t _sectorSize = DISK_SECTOR_SIZE;
//uint8_t _flashBuff[8192];
uint8_t* _formatBuff;
uint8_t* _flashBuff = (uint8_t*)malloc(4096);

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
    FAT_U16(DISC_SECTORS_PER_TABLE),   //sectors_per_alloc_table;//FAT12 and FAT16
    FAT_U16(1),   //sectors_per_track;//A value of 0 may indicate LBA-only access
    FAT_U16(1),   //num_heads
    FAT_U32(0),   //hidden_sectors_count
    FAT_U32(0),   //total_sectors_32
    0x00,         //physical_drive_number;0x00 for (first) removable media, 0x80 for (first) fixed disk
    0x00,         //reserved
    0x29,         //extended_boot_signature;//should be 0x29
    FAT_U32(0x1234), //serial_number: 0x1234 => 1234
    'T' , 'i' , 'n' , 'y' , 'U' , 'S' , 'B' , ' ' , 'M' , 'S' , 'C' , //volume_label padded with spaces (0x20)
    'F' , 'A' , 'T' , '1' , '2' , ' ' , ' ' , ' ' ,  //file_system_type padded with spaces (0x20)

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
    'T' , 'i' , 'n' , 'y' , 'U' , 'S' , 'B' , ' ' ,
    'M' , 'S' , 'C' , 
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
/* 
void readFile(const char * path){
    SysTools::addLog("Reading file: %s\r\n", path);

    File file = FFat.open(path);
    if(!file || file.isDirectory()){
        SysTools::addLog("%s", "- failed to open file for reading");
        return;
    }

    SysTools::addLog("%s", "- read from file:");
    while(file.available()){
        SysTools::addLog("%s", file.read());
    }
    file.close();
}

void writeFile(const char * path, const char * message){
    SysTools::addLog("Writing file: %s\r\n", path);

    File file = FFat.open(path, FILE_WRITE);
    if(!file){
        SysTools::addLog("%s", "- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        SysTools::addLog("%s", "- file written");
    } else {
        SysTools::addLog("%s", "- write failed");
    }
    file.close();
}

void appendFile(const char * path, const char * message){
    SysTools::addLog("Appending to file: %s\r\n", path);

    File file = FFat.open(path, FILE_APPEND);
    if(!file){
        SysTools::addLog("%s", "- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        SysTools::addLog("%s", "- message appended");
    } else {
        SysTools::addLog("%s", "- append failed");
    }
    file.close();
}

void renameFile(const char * path1, const char * path2){
    SysTools::addLog("Renaming file %s to %s\r\n", path1, path2);

    if (FFat.rename(path1, path2)) {
        SysTools::addLog("%s", "- file renamed");
    } else {
        SysTools::addLog("%s", "- rename failed");
    }
}

void deleteFile(const char * path){
    SysTools::addLog("Deleting file: %s\r\n", path);

    if(FFat.remove(path)){
        SysTools::addLog("%s", "- file deleted");
    } else {
        SysTools::addLog("%s", "- delete failed");
    }
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorSize() {
    return _sectorSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorCount() {
    return _sectorCount;
}
 
/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpBuffer(const uint8_t* pBuffer, uint32_t pBufferSize, uint32_t pByteCount) {
  SysTools::addLog("\nSysPartitionDisk::dumpBuffer, totalBytes: %u", pBufferSize);
    
    //Print First Few Buffer Bytes
    uint8_t lineCount = 0;
    
    Serial.printf("%s", "First: ");
    for(int i=0; i<pByteCount; ++i) {
        ++lineCount;
        if(lineCount > 16) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", (pBuffer+i)[0], (pBuffer+i)[0]);
        if(i < (pByteCount - 1)) Serial.printf(",");
    }
     
    Serial.println("\n===========================================================================================================================================");
    
    //Print Last Few Buffer Bytes
    //SysTools::addLog("SysPartitionDisk::dumpBuffer, Last %i Bytes of %u Total", pByteCount, pBufferSize);
    
    lineCount = 0;
    
    Serial.printf("%s", " Last: ");
    for(int i=(pBufferSize - pByteCount); i<pBufferSize; ++i) {
        ++lineCount;
        if(lineCount > 16) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", (pBuffer+i)[0], (pBuffer+i)[0]);
        if(i < (pBufferSize - 1)) Serial.printf(",");
    }
        
    Serial.println("\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize (BYTE pdrv) {
    SysTools::addLog("SysPartitionDisk::diskInitialize, pdrv: %u", pdrv);
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskStatus (BYTE pdrv) {
    SysTools::addLog("SysPartitionDisk::diskStatus, pdrv: %u", pdrv);
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeRAW(uint32_t pStartSector, const uint8_t* pBuffer, uint32_t pBufferSize) {
  //SysTools::addLog("SysPartitionDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
  uint32_t hasFailed;
  const uint16_t startBlock = (pStartSector*512)/4096;
  const uint16_t byteOffset = (pStartSector*512) - (startBlock*4096);
  const uint16_t maxBufferSize = 4096 - byteOffset;

   SysTools::addLog("SysPartitionDisk::writeRAW, replace flash memory, pStartSector: %lu, startBlock: %lu, byteOffset: %lu, bufferSize: %lu, maxBufferSize: %lu", pStartSector, startBlock, byteOffset, pBufferSize, maxBufferSize);
   //dumpBuffer(pBuffer, 512, 32);

   if(pBufferSize > maxBufferSize) {
        SysTools::addLog("syspartitiondisk::writeRAW, ABORT: pBufferSize %lu greater than maxBufferSize: %lu", pBufferSize, maxBufferSize);
        return 0;       
    }
    //dumpBuffer(pBuffer, pBufferSize, 32);
  
    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, _flashBuff, 4096);       
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read_raw failed, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
        return 0;
    }
    
    //if(pStartSector == 0 || pStartSector == 0) dumpBuffer(pBuffer, 512, 16);
    //if(pStartSector == 0 || pStartSector == 0) dumpBuffer(_flashBuff, 512, 16);
    memcpy(_flashBuff+byteOffset, pBuffer, 512);
    //if(pStartSector == 0 || pStartSector == 0) dumpBuffer(_flashBuff+byteOffset, 512, 16);

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
/*    
    if(pStartSector == 82 || pStartSector == 97) {
        esp_partition_read_raw(_ffatPartition, 63*512, _flashBuff, 1024);       
        dumpBuffer(_flashBuff, 512, 16);
        dumpBuffer(_flashBuff+512, 512, 16);
    }
*/
     //SysTools::addLog("SysPartitionDisk::writeRAW, totalBytes written: %lu, starting at byte: %lu \n", pBufferSize, pStartSector);
    //dumpBuffer(_flashBuff, 512, 32);
    //Serial.println("");
    
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskWrite (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
    SysTools::addLog("SysPartitionDisk::disk_write, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);
    //dumpBuffer(buff, 512, 32);
    
    if(!_flashBuff) {
        SysTools::addLog("SysPartitionDisk::diskWrite, ABORT: calloc '_flashBuff' failed");
        return RES_ERROR;
    }
    
    for(int i=0; i < count; ++i) {
        if(writeRAW(sector+i, buff+(i*512), 512) != 512) {
            SysTools::addLog("SysPartitionDisk::disk_write, ABORT: writeRAW failed");
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

    //SysTools::addLog("SysPartitionDisk::readRAW, totalBytes read: %lu, starting at sector: %lu \n", pBufferSize, pStartSector);
    //dumpBuffer(pBuffer, pBufferSize, 32);
    //Serial.println("");
    
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskRead (BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    SysTools::addLog("SysPartitionDisk::disk_read, pdrv: %u, sector: %lu, count: %u", pdrv, sector, count);

    readRAW(sector, buff, count*DISK_SECTOR_SIZE);
    
    for(int i=0; i < count; ++i) {
        SysTools::addLog("SysPartitionDisk::disk_read, dump sector: %u", sector+i);
        dumpBuffer(buff+(i*512), DISK_SECTOR_SIZE, 16);
    }

    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
DRESULT diskIoctl (BYTE pdrv, BYTE cmd, void* buff) {
    //SysTools::addLog("SysPartitionDisk::disk_ioctl, pdrv: %u, cmd: %u", pdrv, cmd);
    
    if(cmd == 0) {
        SysTools::addLog("SysPartitionDisk::disk_ioctl, cmd: 0, CTRL_SYNC");
        if(_isFormatted == true)return RES_OK;
        
        uint8_t sector[512];
        
        for(int i=0; i < 36; ++i) {
            if(readRAW(63+i, &sector[0], 512) != 512) continue;
            if(writeRAW(0+i, (const uint8_t*)&sector, 512) != 512) continue;

            SysTools::addLog("SysPartitionDisk::disk_ioctl, Sector: %lu copied to Sector: %lu", 63+i, 0+i);
        }
        
        _isFormatted = true;
    } else if(cmd == 1) {
        SysTools::addLog("SysPartitionDisk::disk_ioctl, cmd: 1, GET_SECTOR_COUNT: %i", 512);
        ((uint32_t*)buff)[0] = (uint32_t)512;
    } else if(cmd == 2) {
        SysTools::addLog("SysPartitionDisk::disk_ioctl, cmd: 2, GET_SECTOR_SIZE: %i",512);
        ((uint16_t*)buff)[0] = (uint16_t)512;
    } else if(cmd == 3) {
        SysTools::addLog("SysPartitionDisk::disk_ioctl, cmd: 3, GET_BLOCK_SIZE: %i", 1);
        ((uint32_t*)buff)[0] = (uint32_t)1;
    } else if(cmd == 4) {
        SysTools::addLog("SysPartitionDisk::disk_ioctl, cmd: 4, CTRL_TRIM");
    } else {
        SysTools::addLog("SysPartitionDisk::disk_ioctl, ABORT: Unsupported cmd: %u", cmd);
        return RES_PARERR;
    };
    
    Serial.println("");
    return RES_OK;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enable(void (*pCallback)(bool)) {
  //SysTools::addLog("%s", "SysPartitionDisk::open");
  SysTools::addLog("SysPartitionDisk::open, esp_partition found, address: 0x%04X, size: 0x%04X, label: %s \n", _ffatPartition->address, _ffatPartition->size, _ffatPartition->label);
    
/*    
    //Format Disk
    SysTools::addLog("SysPartitionDisk::open, Format Disk If Unformatted");

    writeRAW(0, (const uint8_t*)&bootSectors[0][0], 512);
    writeRAW(1, (const uint8_t*)&bootSectors[1][0], 512);
    writeRAW(2, (const uint8_t*)&bootSectors[2][0], 512);
    writeRAW(3, (const uint8_t*)&bootSectors[3][0], 512);

    //Disk Formatted
    SysTools::addLog("SysPartitionDisk::open, DISK FORMATTED\n");
    _isFormatted = true;
*/

    //esp_vfs_fat_register
    SysTools::addLog("SysPartitionDisk::open, esp_vfs_fat_register, FF_MIN_SS: %u, FF_MAX_SS: %u", FF_MIN_SS, FF_MAX_SS);
    uint8_t hasFailed = esp_vfs_fat_register("/myfat", "", 5, &_fatFS);
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: esp_vfs_fat_register failed, errCode: %u", hasFailed);
        return 0;
    }

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
    
    //Registered
    SysTools::addLog("SysPartitionDisk::open, REGISTERED ff_diskio_register\n");

    //Format Disk
    _formatBuff = (uint8_t*)malloc(4096);	      
    if(!_formatBuff) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: malloc '_formatBuff' failed");
        return 0;
    }
    
    hasFailed = f_mkfs("", 1, 512, _formatBuff, 4096); //drive, format, au size, workbuff, workbuffsize 
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_mount disk, errCode: %u", hasFailed);
        return 0;
    }

    //Disk Formatted
    SysTools::addLog("SysPartitionDisk::open, DISK FORMATTED\n");
    
    //f_mount disk
    SysTools::addLog("SysPartitionDisk::open, f_mount disk, pdrv: %u\n", _fatFS->pdrv);
    
    hasFailed = f_mount(_fatFS, "", 0); //0-delay, 1-immediate
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_mount disk, errCode: %u", hasFailed);
        return 0;
    }

    //Disk Mounted
    SysTools::addLog("SysPartitionDisk::open, f_mount: DISK MOUNTED\n");
    uint8_t sector[512];
    
    Serial.println("dump sector 2");
    readRAW(2, &sector[0], 512);
    dumpBuffer(&sector[0], 512, 256);
    
    Serial.println("dump sector 3");
    readRAW(3, &sector[0], 512);
    dumpBuffer(&sector[0], 512, 16);
    
    //Create File
    SysTools::addLog("SysPartitionDisk::open,  Create File: /myfat/README.TXT\n");
    
    FILE* fp = fopen("/myfat/README.TXT", "w"); // "w" defines "writing mode"
    if ( fp == NULL )
    {
        Serial.printf( "Could not create file /myfat/README.TXT \n" ) ;
        return 0;
    }
    
    fputs("Hello World", fp);
    fclose(fp);
        
    //Open File
    SysTools::addLog("SysPartitionDisk::open, Open File: /myfat/README.TXT\n");
    
    fp = fopen("/myfat/README.TXT", "r") ;
    if ( fp == NULL )
    {
        Serial.printf( "Could not open file /myfat/README.TXT \n" ) ;
        return 0;
    }
    
    //Read File
    char str[128];
    if(fgets(str, sizeof(str), fp) != NULL) Serial.printf("/myfat/README.TXT: %s \n", str) ;
    fclose(fp);
    
    return 0;

/*   
    FF_DIR dj;         // Directory object /
    FILINFO fno;    // File information /
    FIL file;      // File objects /
    uint size;

    hasFailed = f_open(&file, "readme.txt", FA_READ);
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_open, errCode: %u", hasFailed);
        return 0;
    }

    hasFailed = f_read (&file, &content, sizeof(content), &size);
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_read, errCode: %u", hasFailed);
        return 0;
    }
    
    f_close (&file);
    SysTools::addLog("SysPartitionDisk::open, READ: %lu, content: %s", size, content);
    

return 0;
*/
   //Write File
    
    FIL fdisk;      /* File objects */
    FRESULT fr;          /* FatFs function common result code */

    /* Create destination file on the drive 0 */
    hasFailed = f_open(&fdisk, "/world.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_open, errCode: %u", hasFailed);
        return 0;
    }
  
    const uint8_t text[] = "Hello World";
    uint sent, read;
    
    hasFailed = f_write (&fdisk, (uint8_t*)&text[0], sizeof(text), &sent);
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_write, errCode: %u", hasFailed);
        return 0;
    }
    
    f_close (&fdisk);

    //File Written
    SysTools::addLog("SysPartitionDisk::open, FILE WRITTEN\n");

    //Read File
    uint8_t content[128];
    
    hasFailed = f_open(&fdisk, "/world.txt", FA_READ);
    hasFailed = f_read (&fdisk, &content, sizeof(content), &read);
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: f_read, errCode: %u", hasFailed);
        return 0;
    }
    
    f_close (&fdisk);
    SysTools::addLog("SysPartitionDisk::open, READ: %lu, content: %s", read, content);

    //File Read
    SysTools::addLog("SysPartitionDisk::open, FILE READ\n");
 
    //Open Complete
    SysTools::addLog("SysPartitionDisk::open, COMPLETED\n");
     
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
}