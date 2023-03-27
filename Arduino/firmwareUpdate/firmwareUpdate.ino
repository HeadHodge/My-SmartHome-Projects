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
#include <Update.h>

#include <SysTools.h>
#include <UsbFlashBridge.h>

const char _firmwareFile[] = {"/fatdisk/firmware/avatar.bin"};

/////////////////////////////////////////////////////////////////////////////////////////////////
void updateFromFS() {
  // check given FS for valid update.bin and perform update if available
  SysTools::addLog("%s", "firmwareUpdate::updateFromFS");

    FILE* fp = fopen(_firmwareFile, "rb");
    if(fp == NULL)
    {
        SysTools::addLog("firmwareUpdate::updateFromFS, Add File: '%s' to Update Firmware \n", _firmwareFile);
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
        
        //SysTools::addLog("firmwareUpdate::updateFromFS, %d binary bytes read", bytesRead);
        //SysTools::addLog("firmwareUpdate::updateFromFS, %d binary bytes written", Update.write(buff, bytesRead));
        Update.write(buff, bytesRead);
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
      
    if(Update.isFinished() != false) {
        SysTools::addLog("firmwareUpdate::updateFromFS, Update successfully completed. Rebooting.");
    } else {
        SysTools::addLog("firmwareUpdate::updateFromFS, Update not finished? Something went wrong!");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
}
