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
//#include <UsbHidBridge.h>
#include <WsJsonBridge.h>
#include <WsHassioBridge.h>
#include <MinionTools.h>

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

char*receivedKey(DynamicJsonDocument& pKeyObj){

MinionTools::addLog("receivedKey Event, key: %s, modifier: %i, from: %s, to: %s", (const char *)pKeyObj["keyWord"], (int)pKeyObj["keyModifier"], (const char *)pKeyObj["from"], (const char *)pKeyObj["to"]);
MinionTools::addLog("receivedKey Event, useAny: %i, useBle: %i, useWs: %i", (int)pKeyObj["keyModifier"] & 56, (int)pKeyObj["keyModifier"] & 8,(int)pKeyObj["keyModifier"] & 32);
const char *keyWord = pKeyObj["keyWord"];
  
    if(((int)pKeyObj["keyModifier"] & 8) != 0) BleHubBridge::controlDevice(pKeyObj);
    //if(((int)pKeyObj["keyModifier"] & 16) != 0) UsbHubBridge::controlDevice(pKeyObj);
    if(((int)pKeyObj["keyModifier"] & 32) != 0) WsDeviceBridge::controlDevice(pKeyObj);
    if(((int)pKeyObj["keyModifier"] & 56) == 0) BleHubBridge::controlDevice(pKeyObj); //default bridge

    return "";
}

void setup()
{
    MinionTools::openLog(115200);
    
    MinionTools::addLog("%s", "Open BleHubBridge");
    BleHubBridge::openBridge("wsKeyboard-C3");

    MinionTools::addLog("%s", "Open WsHubBridge");
    WsHubBridge::openBridge(receivedKey);

    MinionTools::addLog("%s", "Open WsDeviceBridge");
    WsDeviceBridge::openBridge(receivedKey);

    //MinionTools::addLog("%s", "Open UsbHubBridge");
    //UsbHubBridge::openBridge();
}

void loop()
{
    //MinionTools::addLog("%s", "wsKeyboard-S3::Loop");
    WsHubBridge::refreshBridge();
    WsDeviceBridge::refreshBridge();
}
