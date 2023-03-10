#include "FS.h"
#include "FFat.h"
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
#define FFAT_SECTORSIZE 512
#define FFAT_MSCFILE "/mscDrive.bin"

const uint32_t DISK_SECTOR_COUNT = 2 * 8; // 8KB is the smallest size that windows allow to mount
const uint16_t DISK_SECTOR_SIZE = 512;    // Should be 512
const uint16_t DISC_SECTORS_PER_TABLE = 1; //each table sector can fit 170KB (340 sectors)

File _file;
const esp_partition_t* _ffatPartition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "ffat");
bool _isReadMode = true;
uint16_t _sectorCount = 0, _sectorSize = FFAT_SECTORSIZE;
uint8_t _tempBuff[4096];


  uint8_t bootSectors[4][512] = {
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

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorSize() {
    return _sectorSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorCount() {
    return _sectorCount;
}
 
/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpBuffer(uint8_t* pBuffer, uint32_t pBufferSize, uint32_t pByteCount) {

/////////////////////////////////////////////////////////////////////////////////////////////////  
    SysTools::addLog("SysPartitionDisk::dumpBuffer, First %i Bytes\n", pByteCount);
    
    uint8_t lineCount = 0;
    for(int i=0; i<pByteCount; ++i) {
        ++lineCount;
        if(lineCount > 16) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", (pBuffer+i)[0], (pBuffer+i)[0]);
        if(i < (pByteCount - 1)) Serial.printf(",");
    }
     
    Serial.println("\n");
    
/////////////////////////////////////////////////////////////////////////////////////////////////    
    SysTools::addLog("SysPartitionDisk::dumpBuffer, Last %i Bytes\n", pByteCount);
    
    lineCount = 0;
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
uint32_t formatDisk(uint32_t pStartSector, uint8_t* pBuffer, uint32_t pBufferSize) {
  SysTools::addLog("SysPartitionDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
    
  uint32_t hasFailed;
  const uint16_t startBlock = (pStartSector*512)/4096;
  const uint16_t byteOffset = (pStartSector*512) - (startBlock*4096);
  const uint16_t maxBufferSize = 4096 - byteOffset;

   SysTools::addLog("SysPartitionDisk::writeRAW, replace memory, startBlock: %lu, byteOffset: %lu, bufferSize: %lu, maxBufferSize: %lu", startBlock, byteOffset, pBufferSize, maxBufferSize);

   if(pBufferSize > maxBufferSize) {
        SysTools::addLog("syspartitiondisk::writeRAW, ABORT: pBufferSize %lu greater than maxBufferSize: %lu", pBufferSize, maxBufferSize);
        return 0;       
    }
    
    
    dumpBuffer(pBuffer, pBufferSize, 32);

uint8_t tempBuff[4096];
//uint8_t tempBuff1[4096];

    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, &tempBuff[0], 4096);    
    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read_raw failed");
        return 0;
    }
    
    memcpy(&tempBuff[byteOffset], &pBuffer[0], pBufferSize);
  
    hasFailed = esp_partition_erase_range(_ffatPartition, startBlock*4096, sizeof(tempBuff));   
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_erase_range failed: %lu", hasFailed);
        return 0;
    }

    hasFailed = esp_partition_write_raw(_ffatPartition, startBlock*4096, &tempBuff[0], sizeof(tempBuff));    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read failed");
        return 0;
    }

    SysTools::addLog("SysPartitionDisk::writeRAW, totalBytes written: %lu, starting at byte: %lu \n", pBufferSize, pStartSector);
    dumpBuffer(tempBuff, sizeof(tempBuff), 32);

    Serial.println("");
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeRAW(uint32_t pStartSector, uint8_t* pBuffer, uint32_t pBufferSize) {
  SysTools::addLog("SysPartitionDisk::writeRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);
    
  uint32_t hasFailed;
  const uint16_t startBlock = (pStartSector*512)/4096;
  const uint16_t byteOffset = (pStartSector*512) - (startBlock*4096);
  const uint16_t maxBufferSize = 4096 - byteOffset;

   SysTools::addLog("SysPartitionDisk::writeRAW, replace memory, startBlock: %lu, byteOffset: %lu, bufferSize: %lu, maxBufferSize: %lu", startBlock, byteOffset, pBufferSize, maxBufferSize);

   if(pBufferSize > maxBufferSize) {
        SysTools::addLog("syspartitiondisk::writeRAW, ABORT: pBufferSize %lu greater than maxBufferSize: %lu", pBufferSize, maxBufferSize);
        return 0;       
    }
    
    
    dumpBuffer(pBuffer, pBufferSize, 32);
    

//uint8_t tempBuff1[4096];

    hasFailed = esp_partition_read_raw(_ffatPartition, startBlock*4096, &_tempBuff[0], 4096);    
    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read_raw failed");
        return 0;
    }
    
    memcpy(&_tempBuff[byteOffset], &pBuffer[0], pBufferSize);
  
    hasFailed = esp_partition_erase_range(_ffatPartition, startBlock*4096, sizeof(_tempBuff));   
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_erase_range failed: %lu", hasFailed);
        return 0;
    }

    hasFailed = esp_partition_write_raw(_ffatPartition, startBlock*4096, &_tempBuff[0], sizeof(_tempBuff));    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::writeRAW, ABORT: esp_partition_read failed");
        return 0;
    }

    SysTools::addLog("SysPartitionDisk::writeRAW, totalBytes written: %lu, starting at byte: %lu \n", pBufferSize, pStartSector);
    dumpBuffer(_tempBuff, sizeof(_tempBuff), 32);

    Serial.println("");
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t readRAW(uint32_t pStartSector, uint8_t* pBuffer, uint32_t pBufferSize) {
    //SysTools::addLog("SysPartitionDisk::readRAW, pStartSector: %lu, pBufferSize: %lu", pStartSector, pBufferSize);

    uint32_t hasFailed = esp_partition_read_raw(_ffatPartition, pStartSector*512, pBuffer, pBufferSize);    
    if(hasFailed) {
        SysTools::addLog("SysPartitionDisk::readRAW, ABORT: esp_partition_read failed");
        return 0;
    }

    //SysTools::addLog("SysPartitionDisk::readRAW, totalBytes read: %lu, starting at sector: %lu \n", pBufferSize, pStartSector);
    //dumpBuffer(pBuffer, pBufferSize, 32);
    //Serial.println("");
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool open(void (*pCallback)(bool)) {
  SysTools::addLog("%s", "SysPartitionDisk::open");

    if(_ffatPartition == nullptr) {
        SysTools::addLog("SysPartitionDisk::open, ABORT: Flash Partition 'ffat' Not Found \n");
        return false;
    }

    SysTools::addLog("SysPartitionDisk::open, esp_partition found, address: 0x%04X, size: 0x%04X, label: %s \n", _ffatPartition->address, _ffatPartition->size, _ffatPartition->label);
    if(FORMAT_FFAT) FFat.format();

    if(!FFat.begin()){
        SysTools::addLog("SysPartitionDisk::open, ABORT: FFat.begin Failed \n");
        return false;
    }
    
    uint8_t sector[512];
    formatDisk(0, &bootSectors[0][0], sizeof(bootSectors));
    
    /*
    readRAW(0, &sector[0], sizeof(sector));
    readRAW(1, &sector[0], sizeof(sector));
    readRAW(2, &sector[0], sizeof(sector));
    readRAW(3, &sector[0], sizeof(sector));
    */
    
    //memset(&sector, 'x', sizeof(sector));
    //writeRAW(0, &bootSectors[0], sizeof(bootSectors));
    
    //readRAW(0, &sector[0], sizeof(sector));
   
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
}