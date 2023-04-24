#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>

#include <SysTools.h>
#include <SysSpiDisplay.h>

/////////////////////////////////////////////////////////////////////////////
void setup(){
/////////////////////////////////////////////////////////////////////////////
    Serial.begin(115200);
    delay(4000);
    
    SysTools::addLog("mmcDiskDemo::setup");    
    
    //Turn Display On
    SysSpiDisplay::enable();

    //Set Slot Options
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

    //Set Host Options
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
}

void loop(){
    SysSpiDisplay::refresh();
}
