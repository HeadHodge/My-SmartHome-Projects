#ifndef SYSPARTITIONDISK_H
#define SYSPARTITIONDISK_H

namespace SysPartitionDisk {
    void deleteFile(const char * path);
    void renameFile(const char * path1, const char * path2);
    void appendFile(const char * path, const char * message);
    void writeFile(const char * path, const char * message);
    void readFile(const char * path);
    uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint32_t writeRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint16_t getSectorSize();
    uint16_t getSectorCount();
    
    bool open(void (*pCallback)(bool status));
    bool isConnected();
    
    //bool isReady();
    //bool controlDevice(uint8_t* pDataBuffer, uint8_t pSize);
}

#endif