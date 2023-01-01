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
#include <WsHassioBridge.h>
#include <MinionTools.h>

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

char*receivedKey(DynamicJsonDocument pKeyObj){
MinionTools::addLog("receivedKey Event key: %s, bridge: %s, from: %s", (const char *)pKeyObj["key"], (const char *)pKeyObj["bridge"], (const char *)pKeyObj["from"]);
const char *key = pKeyObj["key"];
  
    if(strcmp(pKeyObj["bridge"], "BLEHUBBRIDGE") == 0) {
        BleHubBridge::controlDevice(key);
    } else if (strcmp(pKeyObj["bridge"], "WSDEVICEBRIDGE") == 0) {
        WsDeviceBridge::controlDevice(key);
    } else if (strcmp(pKeyObj["bridge"], "USBHUBBRIDGE") == 0) {
        //UsbHubBridge::controlDevice(key);
    } else {
        BleHubBridge::controlDevice(key); //Default Bridge
    }

    return "";
}

char *receivedControlWord(const char *pControlWord, const char *pDeviceBridge){
    MinionTools::addLog("Event controlWordReceived Called, ControlWord: '%s' to DeviceBridge: '%s' \r\n", pControlWord, pDeviceBridge);
           
    if(strcmp(pDeviceBridge, "BLEBRIDGE") == 0) {
        BleHubBridge::controlDevice(pControlWord);
    } else if (strcmp(pDeviceBridge, "USBBRIDGE") == 0) {
        //UsbHidBridge::controlDevice(pControlWord);
    } else if (strcmp(pDeviceBridge, "WSBRIDGE") == 0) {
        //WsHidBridge::controlDevice(pControlWord);
    } else {
        BleHubBridge::controlDevice(pControlWord);
    }

    return "";
}

void setup()
{
    MinionTools::openLog(115200);
    
    MinionTools::addLog("%s", "Open BleHubBridge");
    BleHubBridge::openBridge("wsKeyboard-S3");

    MinionTools::addLog("%s", "Open WsHubBridge");
    WsHubBridge::openBridge(receivedKey);

    MinionTools::addLog("%s", "Open WsDeviceBridge");
    WsDeviceBridge::openBridge(receivedControlWord);
}

void loop()
{
    WsHubBridge::refreshBridge();
    WsDeviceBridge::refreshBridge();
}
