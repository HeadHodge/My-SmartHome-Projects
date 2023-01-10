#define US_KEYBOARD 1
#include <Arduino.h>
#include "USB.h"
#include "USBHIDConsumerControl.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

#include <UsbHub.h>

namespace UsbHub
{
USBHIDKeyboard Keyboard;
USBHIDConsumerControl ConsumerControl;
USBHIDMouse Mouse;
bool isConnected = false;

bool isHubConnected() {
    return isConnected;
}

void controlDevice(MinionTools::hidKey* pOptions) {
MinionTools::addLog("UsbHub::controlDevice keyWord: %s, keyCode: 0x%X, duration: %i, delay: %i, usage: %s", pOptions->keyWord, pOptions->keyCode, pOptions->keyDuration, pOptions->keyDelay, pOptions->keyUsage);
//delay(50);

    if(isConnected == false) {
        MinionTools::addLog("%s", "UsbHub::controlDevice ABORTED: Gateway not connected.");
        return;
    };
   
    if(pOptions->keyUsage == "keyboard") {
       //KEYBOARD REPORT
        KeyReport report = {pOptions->keyModifier, 0, pOptions->keyCode, 0, 0, 0, 0, 0};
               
        Keyboard.sendReport(&report);
        delay(pOptions->keyDuration);
        
        memset(&report, 0, sizeof(report)); //clear report
        Keyboard.sendReport(&report); //send empty report
        delay(pOptions->keyDelay);

        MinionTools::addLog("UsbHub::controlDevice Send Keyboard Report, keyModifier: %i, keyCode: 0x%X", pOptions->keyModifier, pOptions->keyCode);

    } else if(pOptions->keyUsage == "consumer") {
        //CONSUMER REPORT      
        ConsumerControl.press(pOptions->keyCode);
        delay(pOptions->keyDuration);
        
        ConsumerControl.release();
        delay(pOptions->keyDelay);
        
        MinionTools::addLog("UsbHub::controlDevice Send Consumer Report, keyCode: 0x%X", pOptions->keyCode);
         
    } else if(pOptions->keyUsage == "mouse") {       
        //MOUSE REPORT
        Mouse.move(200,200);
        
        MinionTools::addLog("UsbHub::controlDevice Send Mouse Report, clickModifier: %i, xOffset: %i, yOffset: %i", 0, 200, 200);
        
    } else {
        MinionTools::addLog("UsbHub::controlDevice ABORT: Invalid keyUsage specified '%s'", pOptions->keyUsage);
        return;
    };
}    
 
void openHub() {
MinionTools::addLog("%s", "UsbHub::openHub");

    Keyboard.begin();
    ConsumerControl.begin();
    Mouse.begin();
    USB.begin();
    
    isConnected = true;
}
}