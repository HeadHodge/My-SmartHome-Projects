/*
    This sketch sends data via HTTP GET requests to data.sparkfun.com service.

    You need to get streamId and privateKey at data.sparkfun.com and paste them
    below. Or just customize this script to talk to other HTTP servers.

*/

#define US_KEYBOARD 1
#include <Arduino.h>
#include <ArduinoJson.h>  // Install from IDE Library manager

#include <BleHidBridge.h>
#include <UsbHidBridge.h>
#include <WsJsonBridge.h>
#include <WsHassioBridge.h>
#include <UsbSerialBridge.h>
#include <MinionTools.h>

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

char* receivedKey(DynamicJsonDocument& pKeyObj) {
  MinionTools::addLog("receivedKey, keyWord: %s, keyModifier: %i, source: %s, location: %s", (const char *)pKeyObj["required"]["keyWord"], (int)pKeyObj["optional"]["keyModifier"], (const char *)pKeyObj["source"], (const char *)pKeyObj["location"]);
  const char *keyWord = pKeyObj["required"]["keyWord"];
  int targetDevices = 0;

  if(pKeyObj["optional"]["targetDevices"] != nullptr) targetDevices = pKeyObj["optional"]["targetDevices"];
  MinionTools::addLog("receivedKey, targetDevices: %i, useBleHub: %i, useUsbHub: %i, useWsHub: %i, WsDevice: %i", targetDevices, targetDevices & 1, targetDevices & 2, targetDevices & 4, targetDevices & 8);

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
  UsbSerialBridge::open(115200);
  
  MinionTools::addLog("%s", "Open BleHubBridge");
  BleHubBridge::openBridge("wsKeyboard-S3");

  MinionTools::addLog("%s", "Open WsHubBridge");
  WsHubBridge::openBridge(receivedKey);

  MinionTools::addLog("%s", "Open UsbHubBridge");
  UsbHubBridge::openBridge();

  MinionTools::addLog("%s", "Open WsDeviceBridge");
  WsDeviceBridge::openBridge(receivedKey);

} //setup

void loop()
{
  WsHubBridge::refreshBridge();
  WsDeviceBridge::refreshBridge();
  UsbSerialBridge::refresh();
}
