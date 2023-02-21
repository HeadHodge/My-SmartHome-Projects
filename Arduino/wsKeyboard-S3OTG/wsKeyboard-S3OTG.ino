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
#include "USB.h"

#include <UsbSD.h>
#include <BleHidBridge.h>
#include <UsbHidBridge.h>
#include <WsJsonBridge.h>
#include <WsHassioBridge.h>
#include <ComSerialBridge.h>
#include <UsbhostVcom.hpp>
#include <MinionTools.h>

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const int maxMessage = 512;
//USBCDC USBSerial;

char* receivedKey(DynamicJsonDocument& pKeyObj){
  MinionTools::addLog("receivedKey, keyWord: %s, keyModifier: %i, source: %s, location: %s", (const char *)pKeyObj["required"]["keyWord"], (int)pKeyObj["options"]["keyModifier"], (const char *)pKeyObj["source"], (const char *)pKeyObj["location"]);
  const char *keyWord = pKeyObj["required"]["keyWord"];
  int targetDevices = 0;
 
    if(pKeyObj["optional"]["targetDevices"] != nullptr) targetDevices = (int)pKeyObj["optional"]["targetDevices"];
    MinionTools::addLog("receivedKey, targets: %i, useBleHub: %i, useUsbHub: %i, useWsHub: %i, WsDevice: %i", targetDevices, targetDevices & 1, targetDevices & 2, targetDevices & 4, targetDevices & 8);

//Bit Flags keyModifier High Word selects Bridge//
//bit0 0 Default
//bit1 1 BleHubBridge
//bit2 2 UsbHubBridge
//bit4 4 WsHubBridge
//bit8 8 WsDeviceBridge
  
    if(targetDevices == 0) BleHubBridge::controlDevice(pKeyObj); //default bridge
    if((targetDevices & 1) != 0) BleHubBridge::controlDevice(pKeyObj);
    if((targetDevices & 2) != 0) UsbHubBridge::controlDevice(pKeyObj);
    //if((targetDevices & 4) != 0) WsHubBridge::controlDevice(pKeyObj);
    if((targetDevices & 8) != 0) WsDeviceBridge::controlDevice(pKeyObj);
    
    return "";
}

void setup()
{
    ComSerialBridge::open(115200);
    //USBSerial.begin(115200);
    
    //UsbSD::open();
     
    MinionTools::addLog("%s", "Open BleHubBridge");
    BleHubBridge::openBridge("wsKeyboard-S3OTG");

    MinionTools::addLog("%s", "Open WsHubBridge");
    WsHubBridge::openBridge(receivedKey);

    MinionTools::addLog("%s", "Open WsDeviceBridge");
    WsDeviceBridge::openBridge(receivedKey);

    //MinionTools::addLog("%s", "Open UsbHubBridge");
    //UsbHubBridge::openBridge();

    MinionTools::addLog("%s", "Open UsbhostIrBridge");
    UsbhostInterface::open();

    //MinionTools::addLog("%s", "Open UsbdeviceDrive");
    //UsbSD::open();

} //setup

void loop()
{
    WsHubBridge::refreshBridge();
    WsDeviceBridge::refreshBridge();
    ComSerialBridge::refresh();
}
