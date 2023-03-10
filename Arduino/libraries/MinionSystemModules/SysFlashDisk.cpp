#include <Preferences.h>

#include "SysFlashTools.h"
#include "SysTools.h"
#include "SysFlashDisk.h"

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

namespace SysFlashDisk {
// You only need to format FFat the first time you run a test
#define FFAT_MSCFILE "/mscDrive.bin"

Preferences _disk;
bool _isReadMode = true;
uint16_t _sectorCount = 0, _sectorSize = 512;

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
uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize) {
    SysTools::addLog("SysFlashDisk:readRAW, pSector: %lu, pBufferSize: %lu", pSector, pBufferSize);
/*
    if(_isReadMode == false) {
        _file.flush();
        _file.close();
        _file = FFat.open(FFAT_MSCFILE, FILE_READ);   
        _isReadMode = true;
        SysTools::addLog("SysFlashDisk:readRAW, File Switched to *READ* mode\n");
    }
    
    _file.seek(pSector * 512, fs::SeekSet);
    SysTools::addLog("SysFlashDisk:readRAW, Seek: %lu, Position: %lu", pSector, _file.position()/512);
    uint32_t bytes = _file.read(pBuffer, pBufferSize);    
    SysTools::addLog("SysFlashDisk:readRAW, Position: %lu, Raw Bytes Read: %lu", _file.position(), bytes);
*/

    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t writeRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize) {
    SysTools::addLog("SysFlashDisk:writeRAW, pSector: %lu, pBufferSize: %lu", pSector, pBufferSize);
/*
    if(_isReadMode == true) {
        _file.flush();
        _file.close();
        _file = FFat.open(FFAT_MSCFILE, FILE_WRITE);   
        _isReadMode = false;
        SysTools::addLog("SysFlashDisk:writeRAW, File Switched to *WRITE* mode\n");
    }
      
    _file.seek(pSector * 512, fs::SeekSet);
    SysTools::addLog("SysFlashDisk:writeRAW, Seek: %lu, Position: %lu", pSector, _file.position()/512);
    uint32_t bytes = _file.write(pBuffer, pBufferSize);
    SysTools::addLog("SysFlashDisk:writeRAW, Position: %lu, Raw Bytes Written: %lu, usedBytes: %lu", _file.position(), bytes, FFat.usedBytes());
*/
    return pBufferSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorSize() {
    return _sectorSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t getSectorCount() {
    return _sectorCount;
;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool open(void (*pCallback)(bool)) {
    SysTools::addLog("%s", "SysFlashDisk:open");
    
    if(!_disk.begin("mscDrive", false, "ffat")) {
        SysTools::addLog("%s", "SysFlashDisk::open, ABORT: Open Disk Failed");
        return false;
    }

    SysTools::addLog("SysFlashDisk:open, Available Sectors: %lu", _disk.freeEntries()/3);
    if(_disk.getUInt("sectorCount") == 0) SysFlashTools::formatDisk(&_disk, 32);
    SysFlashTools::printSector(&_disk, "3");
    SysFlashTools::dumpSector(&_disk, "0");
    //SysTools::addLog("SysFlashDisk:open, sectorCount: %i", _disk.getUInt("sectorCount"));
    return false;


    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
}