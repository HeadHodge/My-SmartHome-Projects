#include <SysTools.h>
#include <SysPartitionDisk.h>

// This file should be compiled with 'Partition Scheme' (in Tools menu)
// set to 'Default with ffat' if you have a 4MB ESP32 dev module or
// set to '16M Fat' if you have a 16MB ESP32 dev module.

void setup(){
    Serial.begin(115200);
    delay(4000);
    SysTools::addLog("%s", "Setup flashDisk");
    
    SysPartitionDisk::enable(nullptr);   
    //SysFlashDisk::testFileIO("test.txt");
    
    //SysTools::addLog( "%s \n", "Test complete" );
}

void loop(){

}
