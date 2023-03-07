#include "USB.h"
#include "USBMSC.h"

#include <UsbFlashBridge.h>
#include <SysFlashDisk.h>
#include <SysTools.h>

namespace UsbFlashBridge {
USBMSC _MSC;

static int32_t onWrite(uint32_t pSector, uint32_t pOffset, uint8_t* pBuffer, uint32_t pBuffSize){
  //SysTools::addLog("usbCom::onWrite: Write Sector: 0x%X, Bytes: %u, Offset: %u \n", pSector, pBuffSize, pOffset);
  if(pOffset != 0) SysTools::addLog("usbCom::onWrite: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    //for(int index=0;index<(pBuffSize/512);++index){
    //SD.writeRAW((uint8_t*)pBuffer+(index*512), pSector + index);
    return SysFlashDisk::writeRAW(pSector, (uint8_t*)pBuffer, pBuffSize);
    //}
}

static int32_t onRead(uint32_t pSector, uint32_t pOffset, void* pBuffer, uint32_t pBuffSize){
  //SysTools::addLog("usbCom::onRead: pSector: %u, pOffset: %u, pBuffSize: %u\n", pSector, pOffset, pBuffSize);
  if(pOffset != 0) SysTools::addLog("usbCom::onRead: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    //for(int index=0;index<(pBuffSize/512);++index){
    //SD.readRAW((uint8_t*)pBuffer+(index*512), pSector + index);
    return SysFlashDisk::readRAW(pSector, (uint8_t*)pBuffer, pBuffSize);
    //}
}

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

void onUsbEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  //Serial.printf("event_id: %i", event_id);
  //delay(1000);
  SysTools::addLog("UsbFlasBridge::onUsbEvent, event_id: %i", event_id);
  //return;

  if(event_base == ARDUINO_USB_EVENTS){
    //arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        //Serial.println("USB PLUGGED");
        SysTools::addLog("%s", "USB PLUGGED");
        break;
        
      case ARDUINO_USB_STOPPED_EVENT:
        //Serial.println("USB UNPLUGGED");
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

bool open() {
  SysTools::addLog("%s", "UsbFlashBridge::open");
    
    //Open Flash Disk
    SysTools::addLog("%s", "UsbFlashBridge::open, Open Flash Disk");
 
    if(!SysFlashDisk::open(nullptr)) {
        SysTools::addLog("%s", "UsbFlashBridge::open, ABORT: Open Flash Disk Failed");
        return false;       
    };
   
    //Open USB-MSC Interface
    SysTools::addLog("UsbFlashBridge::open, Open Usb-MSC interface with sectorCount: %i, sectorSize: %i", SysFlashDisk::getSectorCount(), SysFlashDisk::getSectorSize());        
    _MSC.vendorID("ESP32-S3");//max 8 chars
    _MSC.productID("USB_FLASHDRIVE");//max 16 chars
    _MSC.productRevision("1.0");//max 4 chars
    _MSC.onStartStop(onStartStop);
    _MSC.onRead(onRead);
    _MSC.onWrite(onWrite);
    _MSC.mediaPresent(true);
    _MSC.begin(SysFlashDisk::getSectorCount(), SysFlashDisk::getSectorSize());
    
    SysTools::addLog("%s", "UsbFlashBridge::open, Enable Usb Stack");
    USB.onEvent(onUsbEvent);
    USB.begin();
    return true;
}
}