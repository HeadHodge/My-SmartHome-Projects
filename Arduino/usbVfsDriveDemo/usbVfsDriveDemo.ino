//located in my github repository
#include <SysTools.h>
#include <UsbDiskBridge.h>
#include <SysVfsBridge.h>
#include <SysSpiDisplay.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.
SysVfsBridge::vfsDiskOptions_t* _flashDiskOptions = nullptr;
SysVfsBridge::vfsDiskOptions_t* _mmcDiskOptions = nullptr;

void setup(){
    Serial.begin(115200);
    delay(4000);
    
    //Turn Display On
    SysSpiDisplay::enable();

    if(!SysVfsBridge::enableDisk("/flashDisk", false, &_flashDiskOptions)) {
        SysTools::addLog("usbVfsDriveDemo::enable, ABORT: Open '%s' Failed", "/flashDisk");
        return;       
    };
    SysTools::addLog("usbVfsDriveDemo::enable, /flashDisk: %u Enabled", _flashDiskOptions->diskNum);

    if(!SysVfsBridge::enableDisk("/mmcDisk", false, &_mmcDiskOptions)) {
        SysTools::addLog("usbVfsDriveDemo::enable, ABORT: Open '%s' Failed", "/mmcDisk");
        return;       
    };    
    SysTools::addLog("usbVfsDriveDemo::enable, /mmcDisk: %u Enabled", _mmcDiskOptions->diskNum);
    
    SysTools::addLog("%s", "usbVfsDriveDemo::Setup Enable usbDiskBridge");
    UsbDiskBridge::enable(_mmcDiskOptions);   
}

void loop(){
    SysSpiDisplay::refresh();

}
