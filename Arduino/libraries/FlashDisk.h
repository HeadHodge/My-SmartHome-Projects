#ifndef FLASHDISK_H
#define FLASHDISK_H

namespace FlashDisk {
    //void deleteFile(const char * path);
    //void renameFile(const char * path1, const char * path2);
    //void appendFile(const char * path, const char * message);
    //void writeFile(const char * path, const char * message);
    //void readFile(const char * path);
    void dumpSector(uint16_t pSector, uint16_t pDumpBytes=32);
    void dumpBoot();
    uint32_t readRAW(uint32_t pSector, uint8_t* pBuffer, uint32_t pBufferSize);
    uint32_t writeRAW(uint32_t pSector, const uint8_t* pBuffer, uint32_t pBufferSize);
    uint16_t getSectorCount();
    uint16_t getSectorSize();
    
    bool enable();
    bool isConnected();
    
    //bool isReady();
    //bool controlDevice(uint8_t* pDataBuffer, uint8_t pSize);
}

#endif