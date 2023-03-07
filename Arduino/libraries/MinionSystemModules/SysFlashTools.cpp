#include "SysTools.h"
#include "SysFlashTools.h"

namespace SysFlashTools {

#define FAT_U8(v) ((v) & 0xFF)
#define FAT_U16(v) FAT_U8(v), FAT_U8((v) >> 8)
#define FAT_U32(v) FAT_U8(v), FAT_U8((v) >> 8), FAT_U8((v) >> 16), FAT_U8((v) >> 24)
#define FAT_MS2B(s,ms)    FAT_U8(((((s) & 0x1) * 1000) + (ms)) / 10)
#define FAT_HMS2B(h,m,s)  FAT_U8(((s) >> 1)|(((m) & 0x7) << 5)),      FAT_U8((((m) >> 3) & 0x7)|((h) << 3))
#define FAT_YMD2B(y,m,d)  FAT_U8(((d) & 0x1F)|(((m) & 0x7) << 5)),    FAT_U8((((m) >> 3) & 0x1)|((((y) - 1980) & 0x7F) << 1))
#define FAT_TBL2B(l,h)    FAT_U8(l), FAT_U8(((l >> 8) & 0xF) | ((h << 4) & 0xF0)), FAT_U8(h >> 4)
#define FAT_FILECONTENTS "This is tinyusb's MassStorage Class demo.\r\n\r\nIf you find any bugs or get any questions, feel free to file an\r\nissue at github.com/hathach/tinyusb"

#define MSC_DRIVENAME "/mscDrive.bin"

/*
/////////////////////////////////////////////////////////////////////////////////////////////////
void listDir(const char * dirname, uint8_t levels){
    SysTools::addLog("Listing directory: %s", dirname);

    File root = FFat.open(dirname);
    if(!root){
        SysTools::addLog("%s", "- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        SysTools::addLog("%s", " - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            SysTools::addLog("  DIR : %s", file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            SysTools::addLog("  FILE: %s, SIZE: %lu", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void testFileIO(const char* path){
    SysTools::addLog("Testing file I/O with %s", MSC_DRIVENAME);
    
    static uint8_t buf[512];
    size_t len = 0;
    File file = FFat.open(MSC_DRIVENAME, FILE_WRITE);
    if(!file){
        SysTools::addLog("%s", "- failed to open file for writing");
        return;
    }

    size_t i;
    SysTools::addLog("%s", "- writing..." );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          //SysTools::addLog("%s", ".");
        }
        file.write(buf, 512);
    }
    uint32_t end = millis() - start;
    SysTools::addLog(" - %u bytes written in %u ms", 2048 * 512, end);
    file.close();

    file = FFat.open(MSC_DRIVENAME);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        SysTools::addLog("%s", "- reading..." );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              //SysTools::addLog("%s", ".");
            }
            len -= toRead;
        }
        end = millis() - start;
        SysTools::addLog("- %u bytes read in %u ms", flen, end);
        file.close();
                
        SysTools::addLog("**Test Complete** \n", flen, end);
        listDir("/", 0);
    } else {
        SysTools::addLog("%s", "- failed to open file for reading");
    }
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////
bool formatDisk(Preferences* pDisk, uint32_t pSectorCount) {
  SysTools::addLog("SysFlashTools::formatDisk, pSectorCount: %lu", pSectorCount);
  uint8_t _sectorTables = 1;

  SysTools::addLog("SysFlashTools::formatDisk, create 4 bootSectors");  
  uint8_t bootSectors[4][512] = {
  //------------- Block0: Boot Sector -------------//
  {
    // Header (62 bytes)
    0xEB, 0x3C, 0x90, //jump_instruction
    'M' , 'S' , 'D' , 'O' , 'S' , '5' , '.' , '0' , //oem_name
    FAT_U16(512), //bytes_per_sector
    FAT_U8(1),    //sectors_per_cluster
    FAT_U16(1),   //reserved_sectors_count
    FAT_U8(1),    //file_alloc_tables_num
    FAT_U16(16),  //max_root_dir_entries
    FAT_U16(pSectorCount), //fat12_sector_num
    0xF8,         //media_descriptor
    FAT_U16(_sectorTables),   //sectors_per_alloc_table;//FAT12 and FAT16
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
 
 
    //Format disk file
    SysTools::addLog("%s", "SysFlashTools::formatDisk, Format Disk");
    uint32_t start = millis();
    uint8_t nullSector[512] = {0};
    
    //Add boot sectors to disk file
    SysTools::addLog("SysFlashTools::formatDisk, Write %i Boot Sectors...", 4);
    
    SysTools::addLog("SysFlashTools::formatDisk, bytesPut: %i", pDisk->putBytes("0", (uint8_t*)&bootSectors[0], sizeof(bootSectors[0])));
    SysTools::addLog("SysFlashTools::formatDisk, bytesPut: %i", pDisk->putBytes("1", (uint8_t*)&bootSectors[1], sizeof(bootSectors[1])));
    SysTools::addLog("SysFlashTools::formatDisk, bytesPut: %i", pDisk->putBytes("2", (uint8_t*)&bootSectors[2], sizeof(bootSectors[2])));
    SysTools::addLog("SysFlashTools::formatDisk, bytesPut: %i", pDisk->putBytes("3", (uint8_t*)&bootSectors[3], sizeof(bootSectors[3])));
     
    //All Done
    SysTools::addLog("%s", "SysFlashTools::formatDisk, Create Flash Disk Complete \n");
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void dumpSector(Preferences* pDisk, char* pKey, uint32_t pByteCount) {
  uint16_t sectorSize = pDisk->getBytesLength(pKey);
  uint8_t _sector[sectorSize] = {0};

    if(pByteCount > sectorSize) pByteCount = sectorSize;

    if(pDisk->getBytes(pKey, (uint8_t*)&_sector, sectorSize) != sectorSize) {
        SysTools::addLog("SysFlashTools::dumpSector, ABORT: Unable to read sector: %s", pKey);
        return;
    }
    
    Serial.println("");
    Serial.printf("Sector Dump: First %i Bytes\n", pByteCount);
    uint8_t lineCount = 0;
    for(int i=0; i<pByteCount; ++i) {
        ++lineCount;
        if(lineCount > 16) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", _sector[i], _sector[i]);
        if(i < (pByteCount - 1)) Serial.printf(",");
    }
    
    Serial.println("");
    Serial.printf("Sector Dump: Last %i Bytes\n", pByteCount);
    lineCount = 0;
    for(int i=sectorSize-pByteCount; i<sectorSize; ++i) {
        ++lineCount;
        if(lineCount > 16) {
            lineCount = 0;
            Serial.println("");
        }
        
        Serial.printf("0x%02X(%1c)", _sector[i], _sector[i]);
        if(i < (sectorSize - 1)) Serial.printf(",");
    }
    
    Serial.println("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void printSector(Preferences* pDisk, char* pKey) {
  uint16_t sectorSize = pDisk->getBytesLength(pKey);
  uint8_t _sector[sectorSize] = {0};

    if(pDisk->getBytes(pKey, (uint8_t*)&_sector, sectorSize) != sectorSize) {
        SysTools::addLog("SysFlashTools::printSector, ABORT: Unable to read sector: %s", pKey);
        return;
    }
    
    SysTools::addLog("SysFlashTools::printSector, Print Sector, sector: %s, val:\n\n'%s'", pKey, _sector);
}
} //namespace