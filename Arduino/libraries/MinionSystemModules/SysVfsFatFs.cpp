#include <stdio.h>
#include <dirent.h>


//My Required Co-Libraries
#include <SysTools.h>
#include <SysVfsFatFs.h>
#include <SysVfsFlashDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysVfsFatFs {
/////////////////////////////////////////////////////////////////////////////////////////////////
DSTATUS diskInitialize(uint8_t pDrv);
DRESULT diskWrite(uint8_t pdrv, const uint8_t* buff, DWORD sector, UINT count);
DRESULT diskRead (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DSTATUS diskStatus(uint8_t pdrv);
DRESULT diskIoctl(uint8_t pdrv, uint8_t cmd, void* buff);

vfsDiskOptions_t _vfsDisks[FF_VOLUMES];
uint8_t* _formatBuff = nullptr;

uint8_t _maxDiskNum = 1;
bool    _isFormatting = false;
bool    _isFormatted = false;
bool    _isMounted = false;
bool    _dumpBootFlg = false;
bool    _isReadMode = true;

/////////////////////////////////////////////////////////////////////////////////////////////////
bool formatDisk(const char* pDisk = "0:") {
    SysTools::addLog("SysVfsFatFs::formatDisk, DISK_SECTOR_SIZE: %lu, DISK_SECTOR_COUNT: %lu", _vfsDisks[0].diskSectorSize, _vfsDisks[0].diskSectorCount);
    
    //Format Disk
    uint8_t* workBuff = nullptr;
    
    workBuff = (uint8_t*)malloc(FF_MAX_SS);	      
    if(!workBuff) {
        SysTools::addLog("SysVfsFatFs::formatDisk, ABORT: malloc 'workBuff' failed");
        return false;
    }
    
    uint8_t hasFailed = f_mkfs(pDisk, FM_ANY, 2048, workBuff, FF_MAX_SS); //drive, format, au size, workbuff, workbuffsize 

    free(workBuff);    
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysVfsFatFs::formatDisk, ABORT: f_mount disk, errCode: %u", hasFailed);
        return false;
    }

    //Formatted
    SysTools::addLog("SysVfsFatFs::formatDisk, DISK FORMATTED\n");   
    return true;
}    

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enable(const char* pFileSystem, vfsDiskOptions_t** pDiskOptions) {
  SysTools::addLog("SysVfsFatFs::enable \n");
  uint8_t diskNum = 0;
    
    if(pDiskOptions != nullptr) pDiskOptions[00] = nullptr;
    
    if(_maxDiskNum >= FF_VOLUMES) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: Exceeded Open Disks Allowed, maxDiskNum: '%u'", _maxDiskNum);
        return false;
    }
    
    if(pFileSystem == "/flashDisk") {
        diskNum = _maxDiskNum++;// ++_maxDiskNum;       
        SysFlashDisk::setOptions(diskNum, &_vfsDisks[diskNum]);
    } else {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: Invalid Disk Type '%s'", pFileSystem);
        return false;
    }
    
    //esp_vfs_fat_register
    uint8_t hasFailed;

    SysTools::addLog("SysVfsFatFs::enable, esp_vfs_fat_register, FF_MIN_SS: %u, FF_MAX_SS: %u, FF_VOLUMES: %u, FF_STR_VOLUME_ID: %u", FF_MIN_SS, FF_MAX_SS, FF_VOLUMES, FF_STR_VOLUME_ID);
    hasFailed = esp_vfs_fat_register(_vfsDisks[diskNum].fileSystem, _vfsDisks[diskNum].diskPath, 5, &_vfsDisks[diskNum].fatFS);
    if(hasFailed) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: esp_vfs_fat_register failed, errCode: %u", hasFailed);
        return 0;
    }
    
    _vfsDisks[diskNum].fatFS->pdrv = diskNum;
    SysTools::addLog("SysVfsFatFs::enable, REGISTERED esp_vfs_fat_register, diskNum: %lu\n", diskNum);
    
    //esp_vfs_fat Registered
    SysTools::addLog("SysVfsFatFs::enable, REGISTERED esp_vfs_fat_register\n");

    //ff_diskio_register
    SysTools::addLog("SysVfsFatFs::enable, ff_diskio_register, diskNum: %u", diskNum);
    ff_diskio_register(diskNum, &_vfsDisks[diskNum].diskioEvents);
    
    //ff_diskio Registered
    SysTools::addLog("SysVfsFatFs::enable, REGISTERED ff_diskio_register\n");
  
    //Mount Disk
    SysTools::addLog("SysVfsFatFs::enable, f_mount disk, diskNum: %u\n", diskNum);
    hasFailed = f_mount(_vfsDisks[diskNum].fatFS, _vfsDisks[diskNum].diskPath, 0); //0-delay, 1-immediate
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: f_mount disk, errCode: %u", hasFailed);
        return 0;
    }

    //Disk Mounted
    _isMounted = true;
    SysTools::addLog("SysVfsFatFs::enable, f_mount: DISK MOUNTED: %lu \n", disk_status(diskNum));

    //Open Directory
    if(opendir(_vfsDisks[diskNum].fileSystem) == NULL) {
        SysTools::addLog("SysVfsFatFs::enable, opendir failed");
        
        //FORMAT DISK
        if(formatDisk(_vfsDisks[diskNum].diskPath) != true) {
            SysTools::addLog("SysVfsFatFs::enable, ABORT: formatFat16FlashDisk failed");
            return false;
        }
        
        _isFormatted = true;
        SysTools::addLog("SysVfsFatFs::enable, '%s' is formatted", pFileSystem);

        //CREATE FILE
        SysTools::addLog("SysVfsFatFs::enable, Create File: '%s'", _vfsDisks[diskNum].testFile);
    
        FILE* fp = fopen(_vfsDisks[diskNum].testFile, "w"); // "w" defines "writing mode"
        if(fp == NULL)
        {
            SysTools::addLog("SysVfsFatFs::enable, Could not create file '%s' \n", _vfsDisks[diskNum].testFile);
            return false;
        }
    
        fputs("Hello World!", fp);
        fclose(fp);
        
        //CREATE DIRECTORY
        SysTools::addLog("SysVfsFatFs::enable, Create Directory: '%s'", _vfsDisks[diskNum].testDirectory);
        f_mkdir(_vfsDisks[diskNum].testDirectory);        
    }
    
    //All Done
    SysTools::addLog("SysVfsFatFs::enable, Open Disk Completed \n");
    
    if(pDiskOptions != nullptr) pDiskOptions[0] = &_vfsDisks[diskNum];
    return true;
}
} //namespace SysVfsFatFs
