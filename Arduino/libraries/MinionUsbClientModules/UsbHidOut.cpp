// System Required Libraries
#define US_KEYBOARD 1
#include <Arduino.h>
#include <USB.h>

// My Required Libraries
#include <USBHIDConsumerControl.h>
#include <USBHIDMouse.h>
#include <USBHIDKeyboard.h>
#include <SysTools.h>
#include <UsbHidOut.h>

namespace UsbHidOut {
USBHIDKeyboard Keyboard;
USBHIDConsumerControl ConsumerControl;
USBHIDMouse Mouse;
bool isDeviceConnected = false;

bool isConnected() {
    return isDeviceConnected;
}

void controlDevice(DynamicJsonDocument& pOptionsObj) {
  SysTools::addLog("UsbHidOut::controlDevice keyWord: %s, keyCode: 0x%X, duration: %i, delay: %i, usage: %s", (const char*)pOptionsObj["keyWord"], (int)pOptionsObj["keyCode"], (int)pOptionsObj["keyDuration"], (int)pOptionsObj["keyDelay"], (const char*)pOptionsObj["keyMap"]);
  const char* keyMap = (const char*)pOptionsObj["keyMap"];
  //const char* usage[] = {"keyboard", "consumer", "mouse"};

    if(isDeviceConnected == false) {
        SysTools::addLog("%s", "UsbHidOut::controlDevice ABORTED: Device not connected.");
        return;
    };
    
    //SysTools::addLog("UsbHidOut::controlDevice: keyMap: '%s', Usage: '%s', strcmp: %i, isEqual: %i", keyMap, "consumer", strcmp(keyMap,"consumer"), keyMap== "consumer");
   
    if(strcmp(keyMap, "keyboard") == 0) {
       //KEYBOARD REPORT
        KeyReport report = {(int)pOptionsObj["keyModifier"], 0, (int)pOptionsObj["keyCode"], 0, 0, 0, 0, 0};
               
        Keyboard.sendReport(&report);
        delay((int)pOptionsObj["keyDuration"]);
        
        memset(&report, 0, sizeof(report)); //clear report
        Keyboard.sendReport(&report); //send empty report
        delay((int)pOptionsObj["keyDelay"]);

        SysTools::addLog("UsbHidOut::controlDevice Send Keyboard Report, keyModifier: %i, keyCode: 0x%X", (int)pOptionsObj["keyModifier"], (int)pOptionsObj["keyCode"]);

    } else if(strcmp(keyMap, "consumer") == 0) {
        //CONSUMER REPORT      
        ConsumerControl.press((int)pOptionsObj["keyCode"]);
        delay((int)pOptionsObj["keyDuration"]);
        
        ConsumerControl.release();
        delay((int)pOptionsObj["keyDelay"]);
        
        SysTools::addLog("UsbHidOut::controlDevice Send Consumer Report, keyCode: 0x%X", (int)pOptionsObj["keyCode"]);
         
    } else if(strcmp(keyMap, "mouse") == 0) {       
        //MOUSE REPORT
        Mouse.move(200,200);
        
        SysTools::addLog("UsbHidOut::controlDevice Send Mouse Report, clickModifier: %i, xOffset: %i, yOffset: %i", 0, 200, 200);
        
    } else {
        SysTools::addLog("UsbHidOut::controlDevice ABORT: Invalid keyMap specified '%s'", keyMap);
        SysTools::addLog("UsbHidOut::controlDevice ABORT: '%i', '%i', '%i', '%i', '%i', '%i', '%i', '%i', '%i'", keyMap[0], keyMap[1], keyMap[2], keyMap[3], keyMap[4], keyMap[5], keyMap[6], keyMap[7], keyMap[8], keyMap[9]);
        return;
    };
}    
 
void open() {
SysTools::addLog("%s", "UsbHidOut::open");

    Keyboard.begin();
    ConsumerControl.begin();
    Mouse.begin();
    USB.begin();
    
    isDeviceConnected = true;
}
} //namespace UsbHidOut