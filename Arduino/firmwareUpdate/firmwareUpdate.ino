/* 
 Name:      SD_Update.ino
 Created:   12.09.2017 15:07:17
 Author:    Frederik Merz <frederik.merz@novalight.de>
 Purpose:   Update firmware from SD card

 Steps:
   1. Flash this image to the ESP32 an run it
   2. Copy update.bin to a SD-Card, you can basically
      compile this or any other example
      then copy and rename the app binary to the sd card root
   3. Connect SD-Card as shown in SD example, 
      this can also be adapted for SPI
   3. After successfull update and reboot, ESP32 shall start the new app
*/
//#include <FS.h>
//#include <FFat.h>
#include <Update.h>

#include <SysTools.h>
#include <UsbFlashBridge.h>

const char _firmwareFile[] = {"/fatdisk/firmware/avatar.bin"};

// perform the actual update from a given stream
void performUpdate(Stream &updateSource, size_t updateSize) {
   if (Update.begin(updateSize)) {      
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize) {
         Serial.println("Written : " + String(written) + " successfully");
      }
      else {
         Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end()) {
         Serial.println("OTA done!");
         if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting.");
         } else {
            Serial.println("Update not finished? Something went wrong!");
         }
      } else {
         Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }

   } else {
      Serial.println("Not enough space to begin OTA");
   }
}

// check given FS for valid update.bin and perform update if available
void updateFromFS() {
    SysTools::addLog("%s", "firmwareUpdate::updateFromFS");

    FILE* fp = fopen(_firmwareFile, "rb");
    if(fp == NULL)
    {
        SysTools::addLog("FUCK firmwareUpdate::updateFromFS, Add File: '%s' to Update Firmware \n", _firmwareFile);
        return;
    }

    fseek(fp, 0, SEEK_END); // seek to end of file
    uint32_t fsize = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file
 
    uint8_t buff[4096];
    memset(buff, 0, sizeof(buff));
  
    SysTools::addLog("firmwareUpdate::updateFromFS, Program board with %lu binary bytes", fsize);
    if(!Update.begin(fsize)) {
        SysTools::addLog("firmwareUpdate::updateFromFS ABORT: Update.begin() failed");
        return;
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
        
        SysTools::addLog("firmwareUpdate::updateFromFS, %d binary bytes read", bytesRead);
        SysTools::addLog("firmwareUpdate::updateFromFS, %d binary bytes written", Update.write(buff, bytesRead));
    }
               
    //SysTools::addLog("firmwareUpdate::updateFromFS, Abort Update");
    //Update.abort();

    SysTools::addLog("firmwareUpdate::updateFromFS, OTA done!");
    fclose(fp);
    int8_t ret = remove(_firmwareFile);
    if(ret == 0)
        printf("File deleted successfully");
    else
        printf("Error: unable to delete the file");
    
    Update.end();
      
    if(Update.isFinished()) {
        SysTools::addLog("firmwareUpdate::updateFromFS, Update successfully completed. Rebooting.");
    } else {
        SysTools::addLog("firmwareUpdate::updateFromFS, Update not finished? Something went wrong!");
    }
    
 return;
/*
    File fp = FFat.open("/fatdisk/README.TXT");
    
    if(!fp) {
      SysTools::addLog("firmwareUpdate::updateFromFS, ABORT: Unable to open: '%s'", "/README.TXT");
      return;
    }
        
    size_t updateSize = fp.size();

    SysTools::addLog("firmwareUpdate::updateFromFS, updateSize: %lu", updateSize);
    return;
    
    if (updateSize > 0) {
      SysTools::addLog("Try to start update");
      performUpdate(fp, updateSize);
    } else {
        SysTools::addLog("Error, file is empty");
    }
    
    // when finished remove the binary from sd card to indicate end of the process
    fp.close();
    //fp.remove("/update.bin");
*/  
}

void setup() {
    Serial.begin(115200);
    delay(4000);
    
    SysTools::addLog("%s", "firmwareUpdate::Setup");
    UsbFlashBridge::enable(); //Start FatFs   
    
    //if(FFat.begin(false, "", 5, "ffat")) {
    //  Serial.println("FFat Mount Failed");
    //  return;
    //}
    
    updateFromFS();
}

void loop() {
  
}
