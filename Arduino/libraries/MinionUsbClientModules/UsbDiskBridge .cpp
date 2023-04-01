#include <USB.h>
#include <USBMSC.h>

#include <SysTools.h>
#include <SysVfsFatFs.h>
//#include <SysVfsFlashDisk.h>
//#include <SysVfsCardDisk.h>
#include <UsbDiskBridge.h>

namespace UsbDiskBridge {
USBMSC _MSC;
SysVfsFatFs::vfsDiskOptions_t* _diskOptions = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t onWrite(uint32_t pSector, uint32_t pOffset, uint8_t* pBuffer, uint32_t pBuffSize){
  //SysTools::addLog("UsbDiskBridge::onWrite: Write Sector: %lu, Bytes: %u", pSector, pBuffSize);
  if(pOffset != 0) SysTools::addLog("UsbDiskBridge::onWrite: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);
    
    uint16_t bytesWritten = 0;   
    for(int index=0;index<(pBuffSize/512);++index) {
       
        //SysTools::addLog("UsbDiskBridge::onWrite: Write Sector: %lu, Bytes: %lu", pSector+index, writeBytes);
        if(!_diskOptions->writeRaw(_diskOptions->diskNum, (uint8_t*)(pBuffer+index*512), pSector+index)) {
            SysTools::addLog("UsbDiskBridge::onWrite, ABORT: writeRAW failed");
            return 0;
        }   

        bytesWritten += 512;
    }
    
    if(bytesWritten != pBuffSize) 
        SysTools::addLog("UsbDiskBridge::onWrite, WARNING: bytesWritten: %lu <> pBuffSize: %lu", bytesWritten, pBuffSize);

    return pBuffSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t onRead(uint32_t pSector, uint32_t pOffset, void* pBuffer, uint32_t pBuffSize){
  //SysTools::addLog("UsbDiskBridge::onRead, pSector: %lu, SectorCount: %lu", pSector, pBuffSize/512);
  //SysTools::addLog("UsbDiskBridge::onRead, pSector: %lu, SectorCount: %lu", pSector & 0x00ff, pBuffSize/512);
   if(pOffset != 0) SysTools::addLog("UsbDiskBridge::onRead: ***WARNING*** POffset is non-ze, _vfsDisks[diskNum]->fileSystemro, pOffset: %u\n", pOffset);

    uint16_t bytesRead = 0;
    for(int index=0;index<(pBuffSize/512);++index) {
        
        if(!_diskOptions->readRaw(_diskOptions->diskNum, (uint8_t*)(pBuffer+index*512), pSector + index)) {
            SysTools::addLog("UsbDiskBridge::onRead, ABORT: readRaw failed");
            //SysTools::addLog("UsbDiskBridge::onRead, ABORT: readRaw failed");
            return 0;
        }
        
        bytesRead += 512;
    }

    if(bytesRead != pBuffSize) 
        SysTools::addLog("UsbDiskBridge::onRead, WARNING: bytesRead: %lu <> pBuffSize: %lu", bytesRead, pBuffSize);
    
    return pBuffSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  SysTools::addLog("UsbDiskBridge::onStartStop: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);

  if(start != 0 || load_eject != 1) return false;
  
  SysTools::addLog("%s", "UsbDiskBridge::onStartStop: EJECT SD Card");

  _MSC.end(); 
  delay(1000);
  return true;

  //SD.end();
  //delay(1000);
  //SysTools::addLog("%s", "UsbDiskBridge::onStartStop: EJECTED SD Card");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void onUsbEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  //Serial.printf("UsbDiskBridge::event_id: %i", event_id);

  if(event_base == ARDUINO_USB_EVENTS){
    //arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        SysTools::addLog("%s", "UsbDiskBridge::USB PLUGGED");

        //SysVfsDisks::dumpBoot();
        break;
        
      case ARDUINO_USB_STOPPED_EVENT:
        SysTools::addLog("%s", "UsbDiskBridge::USB UNPLUGGED");
        break;
        
      case ARDUINO_USB_SUSPEND_EVENT:
        SysTools::addLog("%s", "UsbDiskBridge::USB SUSPENDED: remote_wakeup");
        break;
        
      case ARDUINO_USB_RESUME_EVENT:
        SysTools::addLog("%s", "UsbDiskBridge::USB RESUMED");
        break;
      
      default:
        break;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enable() {
  SysTools::addLog("%s", "UsbDiskBridge::begin");
  
    //Open Flash Disk
    SysTools::addLog("%s", "UsbDiskBridge::begin, Open Flash Disk");
 
    //if(!SysVfsFatFs::enableDisk("/flashDisk", &diskOptions)) {
    //    SysTools::addLog("%s", "UsbDiskBridge::enable, ABORT: Open /flashDisk Disk Failed");
    //    return false;       
    //};
 
    if(!SysVfsFatFs::enableDisk("/cardDisk", &_diskOptions)) {
        SysTools::addLog("%s", "UsbDiskBridge::enable, ABORT: Open /cardDisk Disk Failed");
        return false;       
    };
    
    //return true;       
    delay(5000);
    //Open USB-MSC Interface
    SysTools::addLog("UsbDiskBridge::open, Open Usb-MSC interface with sectorCount: %i, sectorSize: %i", _diskOptions->diskSectorCount, _diskOptions->diskSectorSize);        
    _MSC.vendorID("ESP32-S3");//max 8 chars
    _MSC.productID("USB_FLASHDRIVE");//max 16 chars
    _MSC.productRevision("1.0");//max 4 chars
    _MSC.onStartStop(onStartStop);
    _MSC.onRead(onRead);
    _MSC.onWrite(onWrite);
    _MSC.mediaPresent(true);
    _MSC.begin(_diskOptions->diskSectorCount, _diskOptions->diskSectorSize);
    //_MSC.begin(7614, _diskOptions->diskSectorSize);
    
    SysTools::addLog("%s", "UsbDiskBridge::enable, Enable Usb Stack");
    USB.onEvent(onUsbEvent);
    USB.begin();
    return true;
}
} //namespace UsbDiskBridge