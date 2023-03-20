#include "USB.h"
#include "USBMSC.h"

#include <SysTools.h>
#include <SysVfsFlashDisk.h>
#include <UsbFlashBridge.h>

namespace UsbFlashBridge {
USBMSC _MSC;

/////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t onWrite(uint32_t pSector, uint32_t pOffset, uint8_t* pBuffer, uint32_t pBuffSize){
  //SysTools::addLog("usbCom::onWrite: Write Sector: %lu, Bytes: %u", pSector, pBuffSize);
  if(pOffset != 0) SysTools::addLog("usbCom::onWrite: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);
    
    uint16_t writeBytes;
    for(int index=0;index<(pBuffSize/512);++index){
        writeBytes = pBuffSize-(index*512);
        if(writeBytes >= 512)writeBytes = 512;
       
        //SysTools::addLog("usbCom::onWrite: Write Sector: %lu, Bytes: %lu", pSector+index, writeBytes);
        if(SysVfsFlashDisk::writeRAW(pSector+index, (uint8_t*)pBuffer+index*512, writeBytes) != writeBytes) {
            SysTools::addLog("usbCom::onWrite, ABORT: writeRAW failed");
            return 0;
        }   
    }
    
    return pBuffSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t onRead(uint32_t pSector, uint32_t pOffset, void* pBuffer, uint32_t pBuffSize){
  //SysTools::addLog("usbCom::onRead, pSector: %lu, pBuffSize: %u", pSector & 0x00ff, pBuffSize);
  if(pOffset != 0) SysTools::addLog("usbCom::onRead: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    uint16_t bytesRead = SysVfsFlashDisk::readRAW(pSector & 0x00ff, (uint8_t*)pBuffer, pBuffSize);

    if(bytesRead != pBuffSize) SysTools::addLog("usbCom::onRead, ABORT: byteSizes don't match, pSector: %lu, pBuffSize: %lu, bytesRead: %lu", pSector, pBuffSize, bytesRead);
    return bytesRead;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  SysTools::addLog("usbCom::onStartStop: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);

  if(start != 0 || load_eject != 1) return false;
  
  SysTools::addLog("%s", "usbCom::onStartStop: EJECT SD Card");

  _MSC.end(); 
  delay(1000);
  return true;

  //SD.end();
  //delay(1000);
  //SysTools::addLog("%s", "usbCom::onStartStop: EJECTED SD Card");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void onUsbEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  //Serial.printf("event_id: %i", event_id);

  if(event_base == ARDUINO_USB_EVENTS){
    //arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        SysTools::addLog("%s", "USB PLUGGED");

        //SysVfsFlashDisk::dumpBoot();
        break;
        
      case ARDUINO_USB_STOPPED_EVENT:
        SysTools::addLog("%s", "USB UNPLUGGED");
        break;
        
      case ARDUINO_USB_SUSPEND_EVENT:
        SysTools::addLog("%s", "USB SUSPENDED: remote_wakeup");
        break;
        
      case ARDUINO_USB_RESUME_EVENT:
        SysTools::addLog("%s", "USB RESUMED");
        break;
      
      default:
        break;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool enable() {
  SysTools::addLog("%s", "UsbFlashBridge::begin");
    
    //Open Flash Disk
    SysTools::addLog("%s", "UsbFlashBridge::begin, Open Flash Disk");
 
    if(!SysVfsFlashDisk::enable()) {
        SysTools::addLog("%s", "SysVfsFlashDisk::begin, ABORT: Open Flash Disk Failed");
        return false;       
    };
    
    //return true;       
    
    //Open USB-MSC Interface
    SysTools::addLog("SysVfsFlashDisk::open, Open Usb-MSC interface with sectorCount: %i, sectorSize: %i", SysVfsFlashDisk::getSectorCount(), SysVfsFlashDisk::getSectorSize());        
    _MSC.vendorID("ESP32-S3");//max 8 chars
    _MSC.productID("USB_FLASHDRIVE");//max 16 chars
    _MSC.productRevision("1.0");//max 4 chars
    _MSC.onStartStop(onStartStop);
    _MSC.onRead(onRead);
    _MSC.onWrite(onWrite);
    _MSC.mediaPresent(true);
    //_MSC.begin(SysVfsFlashDisk::getSectorCount() - 16, SysVfsFlashDisk::getSectorSize());
    //_MSC.begin(2688, 512);
    _MSC.begin(2815, 512);
    
    SysTools::addLog("%s", "UsbFlashBridge::begin, Enable Usb Stack");
    USB.onEvent(onUsbEvent);
    USB.begin();
    return true;
}
}