#include "USB.h"
#include "USBMSC.h"

#include <ComSerialBridge.h>
#include <SysTools.h>
#include <SysCardDisk.h>
#include <UsbMscCardBridge.h>

#define HWSerial Serial

namespace UsbMscCardBridge {
USBMSC _MSC;

static int32_t onWrite(uint32_t pSector, uint32_t pOffset, uint8_t* pBuffer, uint32_t pBuffSize){
  //HWSerial.printf("usbCom::onWrite: Write Sector: 0x%X, Bytes: %u, Offset: %u \n", pSector, pBuffSize, pOffset);
  if(pOffset != 0) HWSerial.printf("usbCom::onWrite: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    for(int index=0;index<(pBuffSize/512);++index){
      //SD.writeRAW((uint8_t*)pBuffer+(index*512), pSector + index);
      SysCardDisk::writeRAW((uint8_t*)pBuffer, pSector, index);
    }
    
  return pBuffSize;
}

static int32_t onRead(uint32_t pSector, uint32_t pOffset, void* pBuffer, uint32_t pBuffSize){
  //HWSerial.printf("usbCom::onRead: pSector: %u, pOffset: %u, pBuffSize: %u\n", pSector, pOffset, pBuffSize);
  if(pOffset != 0) HWSerial.printf("usbCom::onRead: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    for(int index=0;index<(pBuffSize/512);++index){
      //SD.readRAW((uint8_t*)pBuffer+(index*512), pSector + index);
      SysCardDisk::readRAW((uint8_t*)pBuffer, pSector, index);
    }
  
    return pBuffSize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  HWSerial.printf("usbCom::onStartStop: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);

  if(start != 0 || load_eject != 1) return false;
  
  HWSerial.println("usbCom::onStartStop: EJECT SD Card");

  _MSC.end(); 
  delay(1000);

  //SD.end();
  //delay(1000);
  //Serial.println("usbCom::onStartStop: EJECTED SD Card");

  //enableDrive();
  return true;
}

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        HWSerial.println("USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        HWSerial.println("USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        HWSerial.println("USB RESUMED");
        break;
      
      default:
        break;
    }
  }
}

bool enableDrive() {
        SysCardDisk::open(nullptr);

        _MSC.vendorID("ESP32");//max 8 chars
        _MSC.productID("USB_MSC");//max 16 chars
        _MSC.productRevision("1.0");//max 4 chars
        _MSC.onStartStop(onStartStop);
        _MSC.onRead(onRead);
        _MSC.onWrite(onWrite);
        _MSC.mediaPresent(true);
        _MSC.begin(SysCardDisk::getSectorCount(), SysCardDisk::getSectorSize());
    
        return true;
};

void open() {
    enableDrive();
   
//Open USB Devices
    USB.onEvent(usbEventCallback);
    USB.begin();
}
}