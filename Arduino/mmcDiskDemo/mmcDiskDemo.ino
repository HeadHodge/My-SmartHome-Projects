/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       12
 *    D3       13
 *    CMD      15
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      14
 *    VSS      GND
 *    D0       2  (add 1K pull up after flashing)
 *    D1       4
 */

#include "FS.h"
#include "SD_MMC.h"
#include <SysTools.h>
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>
//#include <SysSpiDisplay.h>

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void setup(){
    Serial.begin(115200);
    delay(4000);

//bool begin(const char * mountpoint="/sdcard", bool mode1bit=false, bool format_if_mount_failed=false, int sdmmc_frequency=BOARD_MAX_SDMMC_FREQ, uint8_t maxOpenFiles = 5);
    
    //if(_card) {
    //    return true;
    //}
    
    //Slot Options
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = (gpio_num_t) 36;
    slot_config.cmd = (gpio_num_t) 35;
    slot_config.d0  = (gpio_num_t) 37;
    slot_config.d1  = (gpio_num_t) 38;
    slot_config.d2  = (gpio_num_t) 33;
    slot_config.d3  = (gpio_num_t) 34;
    slot_config.width = 4;

    //Start Host
    SysTools::addLog("sdmmc_host_init: %i", sdmmc_host_init());    
    SysTools::addLog("sdmmc_host_init_slot: %i", sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    //Host Options
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.flags = SDMMC_HOST_FLAG_4BIT;
    host.slot = SDMMC_HOST_SLOT_1;
    host.max_freq_khz = BOARD_MAX_SDMMC_FREQ;

    sdmmc_card_t* card = (sdmmc_card_t*)malloc(sizeof(sdmmc_card_t));
    
    if(card == nullptr) {
        SysTools::addLog("ABORT: Allocate card failed");
        return;
    }

    //Open Disk
    SysTools::addLog("sdmmc_card_init: %i", sdmmc_card_init(&host, card));
    sdmmc_card_print_info(stdout, card);

    //Read Disk
    uint8_t* buffer = (uint8_t*)heap_caps_malloc(512, MALLOC_CAP_DMA);
    sdmmc_read_sectors(card, buffer, 63, 1);
    SysTools::dumpBuffer(buffer, 512, 32);
    free(buffer);

    //Close Disk
    sdmmc_host_deinit();
    free(card);

return;

/*
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = format_if_mount_failed,
        .max_files = maxOpenFiles,
        .allocation_unit_size = 0
    };
    
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &_card);
    if (ret != ESP_OK) {
      
    }
*/
return;
/////////////////////////////////////////////////////////////////////////
  
    if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed");
        return;
    }

    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
return;

    SysTools::addLog("%s", "Enable SysSpiDisplay");
    //SysSpiDisplay::enable();

    listDir(SD_MMC, "/", 0);
    createDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 0);
    removeDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 2);
    writeFile(SD_MMC, "/hello.txt", "Hello ");
    appendFile(SD_MMC, "/hello.txt", "World!\n");
    readFile(SD_MMC, "/hello.txt");
    deleteFile(SD_MMC, "/foo.txt");
    renameFile(SD_MMC, "/hello.txt", "/foo.txt");
    readFile(SD_MMC, "/foo.txt");
    testFileIO(SD_MMC, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
}

void loop(){
//    SysSpiDisplay::refresh();
}
