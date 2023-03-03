#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "SysTools.h"
#include "SysCardDisk.h"

namespace SysCardDisk {

uint8_t _cardType;
uint64_t _cardSize;
size_t _sectorSize;
size_t _numSectors;

/////////////////////////////////////////////////////////////////////////////////////////////////
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    SysTools::addLog("Listing directory: %s", dirname);

    File root = fs.open(dirname);
    if(!root){
        SysTools::addLog("%s", "Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        SysTools::addLog("%s", "Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            SysTools::addLog("  DIR : %s", file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            SysTools::addLog("  FILE: %s,  SIZE: %lu", file.name(), file.size());
        }
        
        file = root.openNextFile();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void readRAW(uint8_t* pBuffer, uint32_t pSector, uint8_t pIndex) {
    SD.readRAW(pBuffer+(_sectorSize * pIndex), pSector + pIndex);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void writeRAW(uint8_t* pBuffer, uint32_t pSector, uint8_t pIndex) {
    SD.writeRAW(pBuffer+(_sectorSize * pIndex), pSector + pIndex);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
size_t getSectorSize() {
    return _sectorSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
size_t getSectorCount() {
    return _numSectors;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool open(void (*pCallback)(bool)) {
    SysTools::addLog("%s", "SysCardDisk::open");
        
    if(!SD.begin()){
        SysTools::addLog("%s", "SD Card Mount Failed");
        return false;
    }
    
    _cardType = SD.cardType();

    if(_cardType == CARD_NONE){
        SysTools::addLog("SysCardDisk::open, %s", "No SD Card Attached");
        return false;
    }

    SysTools::addLog("SysCardDisk::open, %s", "SD Card Type: ");
    if(_cardType == CARD_MMC){
        SysTools::addLog("SysCardDisk::open, %s", "MMC");
    } else if(_cardType == CARD_SD){
        SysTools::addLog("SysCardDisk::open, %s", "SDSC");
    } else if(_cardType == CARD_SDHC){
        SysTools::addLog("SysCardDisk::open, %s", "SDHC");
    } else {
        SysTools::addLog("SysCardDisk::open, %s", "UNKNOWN");
    }

    _cardSize = SD.cardSize() / (1024 * 1024);
    _sectorSize = SD.sectorSize();
    _numSectors = SD.numSectors();
    
    SysTools::addLog("SysCardDisk::open, Card Size: %lluMB, sectorSize: %zu, sectorCount: %zu\n", _cardSize, _sectorSize, _numSectors);
    //formatSD();
    listDir(SD, "/", 0);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
}
