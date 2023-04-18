//System Libraries
#include <stdio.h>
#include <dirent.h>
#include <esp_vfs_fat.h>
#include <Update.h>

//My Required Libraries
#include <SysTools.h>
#include <SysVfsCardDisk.h>
#include <SysVfsFlashDisk.h>
#include <SysVfsRamDisk.h>
#include <SysVfsFatFs.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

/////////////////////////////////////////////////////////////////////////////////////////////////
namespace SysFatFs {
/////////////////////////////////////////////////////////////////////////////////////////////////

vfsDiskOptions_t* _vfsDiskOptions[FF_VOLUMES];
FATFS* fatFS[FF_VOLUMES] = {nullptr};
const char* _firmwareFile = "/cardDisk/firmware/flashfirmware.bin";
uint8_t* _formatBuff = nullptr;
uint8_t _disksEnabled = 0;
bool    _isFormatted = false;
bool    _isMounted = false;

/////////////////////////////////////////////////////////////////////////////////////////////////
bool testDisk(char* pDirectory, char* pFile)
{
  DIR *dp;
  struct dirent *ep;     
  SysTools::addLog("SysVfsFatFs::testDisk, opendir: %s, testFile: %s", pDirectory, pFile);

    //LIST DIR
    SysTools::addLog("SysVfsFatFs::testDisk, List Directory");
    dp = opendir(pDirectory);

    if(dp == NULL) {
        SysTools::addLog("SysVfsFatFs::testDisk, Couldn't open the directory");
        return -1;
    }
    
    while ((ep = readdir (dp)) != NULL) SysTools::addLog("SysVfsFatFs::testDisk, fileName: %s", ep->d_name);

    (void) closedir (dp);

    //ADD TEST FILE
    SysTools::addLog("SysVfsFatFs::testDisk, Create Test File: '%s'", pFile);
    
    FILE* fp = fopen(pFile, "w"); // "w" defines "writing mode"
        
    if(fp == NULL){
        SysTools::addLog("SysVfsFatFs::testDisk, Could not create file '%s' \n", pFile);
        return false;
    }
    
    fputs("Hello World!", fp);
    fclose(fp);
    
    //DUMP TEST FILE
    SysTools::addLog("SysVfsFatFs::testDisk, Dump Test File: '%s'", pFile);
    fp = fopen(pFile,"r");
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
    SysTools::addLog("SysVfsFatFs::testDisk, List Directory");
    dp = opendir(pDirectory);
    
    //list directory
    while ((ep = readdir (dp)) != NULL) SysTools::addLog("SysVfsFatFs::testDisk, fileName: %s", ep->d_name);
          
    (void) closedir (dp);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool flashProgram() {
  _firmwareFile = "/cardDisk/firmware.bin";
  SysTools::addLog("SysVfsFatFs::flashProgram: '%s'", _firmwareFile);

    FILE* fp = fopen(_firmwareFile, "rb");
    if(fp == NULL)
    {
        SysTools::addLog("SysVfsFatFs::flashProgram, ABORT: _firmwareFile '%s' missing \n", _firmwareFile);
        return false;
    }
    
    /////////////////////////////
    //Start Firmware Upgrade
    /////////////////////////////
    SysTools::addLog("SysVfsFatFs::flashProgram, *** Program Board ***");

    fseek(fp, 0, SEEK_END); // seek to end of file
    uint32_t fsize = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file
 
    uint8_t buff[4096];
    memset(buff, 0, sizeof(buff));
  
    if(!Update.begin(fsize)) {
        SysTools::addLog("SysVfsFatFs::flashProgram, ABORT: Update.begin() Failed with fsize: %lu\n", fsize);
        return false;
    }

    uint32_t bytesRead = 0;
    uint32_t bytesTotal = 0;
    
    while(true) {
        if(bytesTotal >= fsize) break;
        
        if(fsize - bytesTotal >= sizeof(buff))
            bytesRead = fread(buff, 1, sizeof(buff), fp);
        else
            bytesRead = fread(buff, 1, fsize - bytesTotal, fp);

        bytesTotal += bytesRead;
        
        Update.write(buff, bytesRead);
    }
    
    //Remove firmwareFile
    
    fclose(fp);    
    int8_t ret = remove(_firmwareFile);
    
    if(ret == 0)
        SysTools::addLog("SysVfsFatFs::flashProgram, _firmwareFile deleted successfully");
    else
        SysTools::addLog("SysVfsFatFs::flashProgram, ABORT: Unable to remove _firmwareFile: '%s'", _firmwareFile);
    
    //Complete Firmware Upgrade
      
    if(Update.isFinished() != false)
        SysTools::addLog("SysVfsFatFs::flashProgram, Update successfully completed. Reset Board.");
    else
        SysTools::addLog("SysVfsFatFs::flashProgram, Update not finished? Something went wrong!");
    
    Update.end();

    SysTools::addLog("SysVfsFatFs::flashProgram, Completed!\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool formatDisk(uint8_t pDiskNum, const char* pDisk = "0:") {
    SysTools::addLog("SysVfsFatFs::formatDisk, diskNum: %u, diskSectorSize: %lu, diskSectorCount: %lu", pDiskNum, _vfsDiskOptions[pDiskNum]->sectorSize(), _vfsDiskOptions[pDiskNum]->sectorCount());
    
    //Format Disk
    uint8_t* workBuff = nullptr;
    
    workBuff = (uint8_t*)malloc(FF_MAX_SS);	      
    if(!workBuff) {
        SysTools::addLog("SysVfsFatFs::formatDisk, ABORT: malloc 'workBuff' failed");
        return false;
    }
    
    uint8_t hasFailed = f_mkfs(pDisk, FM_ANY, 0, workBuff, FF_MAX_SS); //drive, format, au size, workbuff, workbuffsize 

    free(workBuff);    
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysVfsFatFs::formatDisk, ABORT: f_mkfs, errCode: %u", hasFailed);
        return false;
    }

    //Formatted
    SysTools::addLog("SysVfsFatFs::formatDisk, DISK FORMATTED\n");   
    return true;
}    

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enableDisk(const char* pFileSystem, vfsDiskOptions_t** pDiskOptions) {
  SysTools::addLog("SysVfsFatFs::enable \n");
  uint8_t diskNum = -1;
  uint8_t hasFailed;

    if(pDiskOptions != nullptr) pDiskOptions[0] = nullptr;

    ///////////////////
    //Enable Disk
    ///////////////////
    if(_disksEnabled >= FF_VOLUMES) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: Exceeded Open Disks Allowed, maxDiskNum: '%u'", _disksEnabled);
        return false;
    }
    
    /////////////////////////
    //  REGISTER DISKIO
    /////////////////////////
    vfsDiskOptions_t* diskOptions = nullptr;

    if(pFileSystem == "/flashDisk") {
        diskNum = SysFlashDisk::enable(&diskOptions);       
        //SysTools::addLog("SysVfsFatFs::enable, FF_MIN_SS: %u, FF_MAX_SS: %u, FF_VOLUMES: %u, FF_STR_VOLUME_ID: %u", FF_MIN_SS, FF_MAX_SS, FF_VOLUMES, FF_STR_VOLUME_ID);
    } else if(pFileSystem == "/cardDisk") {
        diskNum = SysCardDisk::enable(&diskOptions);       
        //SysTools::addLog("SysVfsFatFs::enable, Enabled fs: %s, diskNum: %u ", pFileSystem, diskNum);
    } else if(pFileSystem == "/ramDisk") {
        diskNum = SysRamDisk::enable(&diskOptions);       
        //SysTools::addLog("SysVfsFatFs::enable, Enabled fs: %s, diskNum: %u ", pFileSystem, diskNum);
    } else {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: Invalid Disk Type '%s'", pFileSystem);
        return false;
    }
    
    if(diskNum == -1) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: Invalid diskNum: %u", diskNum);
        return false;
    }
    
    _vfsDiskOptions[diskNum] = diskOptions;
    if(pDiskOptions != nullptr) pDiskOptions[0] = diskOptions;
    
    SysTools::addLog("SysVfsFatFs::enableDisk, Disk I/O Registered, diskNum: %u, fs: %s\n", diskNum, _vfsDiskOptions[diskNum]->fileSystem);
    
    /////////////////////////
    // REGISTER VFS
    /////////////////////////
    SysTools::addLog("SysVfsFatFs::enable, esp_vfs_fat_register, diskNum: %lu, fs: '%s', path: '%s'", diskNum, _vfsDiskOptions[diskNum]->fileSystem, _vfsDiskOptions[diskNum]->diskPath);

    hasFailed = esp_vfs_fat_register(_vfsDiskOptions[diskNum]->fileSystem, _vfsDiskOptions[diskNum]->diskPath, 5, &fatFS[diskNum]);
    if(hasFailed) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: esp_vfs_fat_register failed, errCode: %u", hasFailed);
        return 0;
    }
    
    //esp_vfs_fat Registered
    SysTools::addLog("SysVfsFatFs::enable, REGISTERED esp_vfs_fat_register, diskNum: %lu\n", diskNum);

    /////////////////////////
    //   Mount Disk
    /////////////////////////
    SysTools::addLog("SysVfsFatFs::enable, f_mount disk, diskNum: %u", diskNum);
    fatFS[diskNum]->pdrv = diskNum;
    hasFailed = f_mount(fatFS[diskNum], _vfsDiskOptions[diskNum]->diskPath, 1); //0-delay, 1-immediate
    if(hasFailed != FR_OK) {
        SysTools::addLog("SysVfsFatFs::enable, ABORT: f_mount disk, errCode: %u", hasFailed);

        //FORMAT DISK
        SysTools::addLog("SysVfsFatFs::enable, Trying to Format Disk: %lu", diskNum);
        if(formatDisk(diskNum, _vfsDiskOptions[diskNum]->diskPath) != true) {
            SysTools::addLog("SysVfsFatFs::enableDisk, ABORT: formatDisk failed");
            return false;
        }
        SysTools::addLog("SysVfsFatFs::enableDisk, Format Done");
    
        //RETRY MOUNT DISK
        SysTools::addLog("SysVfsFatFs::enable, f_mount disk, diskNum: %u", diskNum);
        fatFS[diskNum]->pdrv = diskNum;
        hasFailed = f_mount(fatFS[diskNum], _vfsDiskOptions[diskNum]->diskPath, 1); //0-delay, 1-immediate
        if(hasFailed != FR_OK) {
            SysTools::addLog("SysVfsFatFs::enable, ABORT: f_mount disk, errCode: %u", hasFailed);
            return false;
        }
    }

    //Disk Mounted
    _isMounted = true;
    SysTools::addLog("SysVfsFatFs::enable, f_mount: DISK MOUNTED, diskType: %s, sectorSize: %lu, sectorCont %lu \n", _vfsDiskOptions[diskNum]->diskType(), _vfsDiskOptions[diskNum]->sectorSize(), _vfsDiskOptions[diskNum]->sectorCount());

    ///////////////////
    //  Flash Program
    ///////////////////  
    flashProgram();
    
    ///////////////////
    //  Test Disk
    ///////////////////  
    SysTools::addLog("SysVfsFatFs::enable, Test Disk, diskNum: %u", diskNum);

    //RUN TEST
    if(!testDisk(_vfsDiskOptions[diskNum]->fileSystem, _vfsDiskOptions[diskNum]->testFile)) {
        SysTools::addLog("SysVfsFatFs::enableDisk, ABORT: Test Mounted Disk Failed, Formatting Disk");

        //FORMAT DISK
        if(formatDisk(diskNum, _vfsDiskOptions[diskNum]->diskPath) != true) {
            SysTools::addLog("SysVfsFatFs::enableDisk, ABORT: formatFat16FlashDisk failed");
            return false;
        }
        SysTools::addLog("SysVfsFatFs::enableDisk, Format Done");
    
        //RUN TEST AGAIN
        if(!testDisk(_vfsDiskOptions[diskNum]->fileSystem, _vfsDiskOptions[diskNum]->testFile)) {
            SysTools::addLog("SysVfsFatFs::enableDisk, ABORT: Test Mounted Disk Failed");
            return false;
        };
        SysTools::addLog("SysVfsFatFs::enableDisk, Disk Test Done");
    } else {
        SysTools::addLog("SysVfsFatFs::enableDisk, Disk Test Done");
    }

    //All Done
    if(pDiskOptions != nullptr) pDiskOptions[0] = _vfsDiskOptions[diskNum];
    SysTools::addLog("SysVfsFatFs::enableDisk, Enable Vfs Disk '%s' Completed \n", _vfsDiskOptions[diskNum]->fileSystem);
    return true;
}
} //namespace SysFatFs
