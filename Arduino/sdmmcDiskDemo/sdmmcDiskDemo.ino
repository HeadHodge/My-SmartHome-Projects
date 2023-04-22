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

//#include "SD_MMC.h"
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>
#include <SysTools.h>
#include <SysSpiDisplay.h>

void setup(){
    Serial.begin(115200);
    delay(4000);
/*    
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

    SysTools::addLog("%s", "Enable SysSpiDisplay");
    SysSpiDisplay::enable();
*/
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.slot = SDMMC_HOST_SLOT_0;
    config.max_freq_khz = 40000; // 40000=High Speed, 20000=Normal Speed
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    sdmmc_host_init();
    sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    sdmmc_card_t* card = (sdmmc_card_t*)malloc(sizeof(sdmmc_card_t));
    //TEST_ASSERT_NOT_NULL(card);
    sdmmc_card_init(&config, card);
    sdmmc_card_print_info(stdout, card);
    
    //uint8_t* buffer = (uint8_t*)heap_caps_malloc(512, MALLOC_CAP_DMA);
    //sdmmc_read_sectors(card, buffer, 0, 1);
    //free(buffer);
    sdmmc_host_deinit();
    free(card);
}

void loop(){
    //SysSpiDisplay::refresh();
}
