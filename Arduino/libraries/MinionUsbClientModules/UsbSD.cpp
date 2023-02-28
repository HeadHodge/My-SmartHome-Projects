#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "USB.h"
#include "USBMSC.h"

#include "USBSD.h"

namespace UsbSD
{
USBMSC MSC;
bool enableDrive();
    
static int32_t onWrite(uint32_t pSector, uint32_t pOffset, uint8_t* pBuffer, uint32_t pBuffSize){
  //MinionTools::addLog("usbCom::onWrite: Write Sector: 0x%X, Bytes: %u, Offset: %u \n", pSector, pBuffSize, pOffset);
  if(pOffset != 0) MinionTools::addLog("usbCom::onWrite: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    for(int i=0;i<(pBuffSize/512);++i){
      SD.writeRAW((uint8_t*)pBuffer+(i*512), pSector + i);
    }
    
  return pBuffSize;
}

static int32_t onRead(uint32_t pSector, uint32_t pOffset, void* pBuffer, uint32_t pBuffSize){
  //MinionTools::addLog("usbCom::onRead: pSector: %u, pOffset: %u, pBuffSize: %u\n", pSector, pOffset, pBuffSize);
  if(pOffset != 0) MinionTools::addLog("usbCom::onRead: ***WARNING*** POffset is non-zero, pOffset: %u\n", pOffset);

    for(int i=0;i<(pBuffSize/512);++i){
      SD.readRAW((uint8_t*)pBuffer+(i*512), pSector + i);
    }
  
    return pBuffSize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  MinionTools::addLog("usbCom::onStartStop: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);

  if(start != 0 || load_eject != 1) return false;
  
  MinionTools::addLog("%s", "usbCom::onStartStop: EJECT SD Card");

  MSC.end(); 
  delay(1000);

  SD.end();
  delay(1000);
  MinionTools::addLog("%s", "usbCom::onStartStop: EJECTED SD Card");

  enableDrive();
  return true;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    MinionTools::addLog("UsbSD::listDir: %s", dirname);

    File root = fs.open(dirname);
    if(!root){
        MinionTools::addLog("UsbSD::listDir %s", "Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        MinionTools::addLog("UsbSD::listDir %s", "Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            MinionTools::addLog("UsbSD::listDir DIR: %s", file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            MinionTools::addLog("UsbSD::listDir FILE: %s, SIZE: %u", file.name(), file.size());
        }
        
        file = root.openNextFile();
    }
}

bool enableDrive() {
  MinionTools::addLog("%s", "usbDrive::enableDrive");
  uint8_t cardType;
  uint64_t cardSize;
  size_t sectorSize;
  size_t numSectors;

    if(!SD.begin()){
        MinionTools::addLog("%s", "UsbSD::enableDrive SD Card Mount Failed");
        return false;
    }
    
    cardType = SD.cardType();

    if(cardType == CARD_NONE){
        MinionTools::addLog("%s", "UsbSD::enableDrive No SD Card Attached");
        return false;
    }

    MinionTools::addLog("%s", "UsbSD::enableDrive SD Card Type: ");
    if(cardType == CARD_MMC){
        MinionTools::addLog("%s", "MMC");
    } else if(cardType == CARD_SD){
        MinionTools::addLog("%s", "SDSC");
    } else if(cardType == CARD_SDHC){
        MinionTools::addLog("%s", "SDHC");
    } else {
        MinionTools::addLog("%s", "UNKNOWN");
    }

    cardSize = SD.cardSize() / (1024 * 1024);
    sectorSize = SD.sectorSize();
    numSectors = SD.numSectors();
    MinionTools::addLog("UsbSD::enableDrive SD Card Size: %lluMB, sectorSize: %zu, sectorCount: %zu\n", cardSize, sectorSize, numSectors);

    //formatSD();
    listDir(SD, "/", 0);

    MSC.vendorID("ESP32");//max 8 chars
    MSC.productID("USB_MSC");//max 16 chars
    MSC.productRevision("1.0");//max 4 chars
    MSC.onStartStop(onStartStop);
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);
    MSC.mediaPresent(true);
    MSC.begin(numSectors, sectorSize);
    
    return true;
};

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base != ARDUINO_USB_EVENTS)
  
  {
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        MinionTools::addLog("UsbSD::usbEventCallback: %s", "USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        MinionTools::addLog("UsbSD::usbEventCallback: %s", "USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        MinionTools::addLog("UsbSD::usbEventCallback: %s", "USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        MinionTools::addLog("UsbSD::usbEventCallback: %s", "USB RESUMED");
        break;
      
      default:
        break;
    }
  }
}

void open() {
  MinionTools::addLog("%s", "UsbSD::open");


    enableDrive();
   
//Open USB Devices
    USB.onEvent(usbEventCallback);
    USB.begin();
}
}