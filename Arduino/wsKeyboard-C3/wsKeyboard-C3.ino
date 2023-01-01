/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define US_KEYBOARD 1
#include <Arduino.h>
#include <ArduinoJson.h>  // Install from IDE Library manager
#include <BleHidBridge.h>
#include <WsJsonBridge.h>
#include <MinionTools.h>

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

char *controlWordReceived(const char *pControlWord, const char *pDeviceBridge){
    MinionTools::addLog("Event controlWordReceived Called, ControlWord: '%s' to DeviceBridge: '%s' \r\n", pControlWord, pDeviceBridge);
           
    if(strcmp(pDeviceBridge, "BLEBRIDGE") == 0) {
        BleHidBridge::controlDevice(pControlWord);
    } else if (strcmp(pDeviceBridge, "USBBRIDGE") == 0) {
        //UsbHidBridge::controlDevice(pControlWord);
    } else if (strcmp(pDeviceBridge, "WSBRIDGE") == 0) {
        //WsHidBridge::controlDevice(pControlWord);
    } else {
        BleHidBridge::controlDevice(pControlWord);
    }

    return "";
}

void setup()
{
    MinionTools::openLog(115200);
    
    MinionTools::addLog("%s", "Open BleHidBridge");
    BleHidBridge::openBridge("wsKeyboard-C3");

    MinionTools::addLog("%s", "Open WsJsonBridge");
    WsJsonBridge::openBridge(controlWordReceived);
}

void loop()
{
    WsJsonBridge::refreshBridge();
    delay(100);
}
