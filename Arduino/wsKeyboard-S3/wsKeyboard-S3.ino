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
#include <UsbHidBridge.h>
#include <WsJsonBridge.h>
#include <WsHassioBridge.h>
#include <MinionTools.h>
#include <map>

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

char* receivedKey(DynamicJsonDocument& pKeyObj){
MinionTools::addLog("receivedKey, keyWord: %s, keyModifier: %i, source: %s, location: %s", (const char *)pKeyObj["settings"]["keyWord"], (int)pKeyObj["options"]["keyModifier"], (const char *)pKeyObj["source"], (const char *)pKeyObj["location"]);
MinionTools::addLog("receivedKey, targets: %i, useBleHub: %i, useUsbHub: %i, useWsHub: %i, WsDevice: %i", (int)pKeyObj["targets"], (int)pKeyObj["targets"] & 1, (int)pKeyObj["targets"] & 2, (int)pKeyObj["targets"] & 4, (int)pKeyObj["targets"] & 8);
const char *keyWord = pKeyObj["settings"]["keyWord"];

//Bit Flags keyModifier High Word selects Bridge//
//bit0 0 Default
//bit1 1 BleHubBridge
//bit2 2 UsbHubBridge
//bit4 4 WsHubBridge
//bit8 8 WsDeviceBridge
  
    if(((int)pKeyObj["targets"]) ==  0) BleHubBridge::controlDevice(pKeyObj); //default bridge
    if(((int)pKeyObj["targets"]   &  1) != 0) BleHubBridge::controlDevice(pKeyObj);
    if(((int)pKeyObj["targets"]   &  2) != 0) UsbHubBridge::controlDevice(pKeyObj);
    //if(((int)pKeyObj["targets"] &  4) != 0) WsHubBridge::controlDevice(pKeyObj);
    if(((int)pKeyObj["targets"]   &  8) != 0) WsDeviceBridge::controlDevice(pKeyObj);

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
    WsDeviceBridge::openBridge(receivedKey);

    MinionTools::addLog("%s", "Open UsbHubBridge");
    UsbHubBridge::openBridge();

} //setup

void loop()
{
    WsHubBridge::refreshBridge();
    WsDeviceBridge::refreshBridge();
}
