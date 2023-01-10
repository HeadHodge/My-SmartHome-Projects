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

#include <MinionTools.h>
#include <WsDevice.h>
#include <WsHassioBridge.h>

namespace WsDeviceBridge
{
callBack receivedKey;
bool isOpen = false;
const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            
int refNum = 1;

char *receivedMessage(unsigned char *pMessage) {    
const char* messageType;
DynamicJsonDocument doc(1024);

      deserializeJson(doc, (const char *)pMessage);  
      messageType = doc["type"];
      if(messageType == NULL) messageType = ""; 
      MinionTools::addLog("WsHassioBridge::receivedMessage messageType: %s", messageType);
 
      if(strcmp(messageType, "auth_required") == 0) {
        MinionTools::addLog("WsHassioBridge::receivedMessage Send accessToken: %s", accessToken);
        WsDevice::controlDevice(accessToken);
        return "";
      }     

      if(strcmp(messageType, "auth_ok") == 0) {
        isOpen = false;
        MinionTools::addLog("%s", "WsHassioBridge::receivedMessage Bridge is Open");  
        return "";
      }

      if(strcmp(messageType, "result") == 0) {
        MinionTools::addLog("WsHassioBridge::receivedMessage Command Result: %s", pMessage);
        return "";
      }
      
      return "";
}

bool isBridgeOpen() {
    return isOpen;
}

void refreshBridge() {
    WsDevice::refreshDevice();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {
MinionTools::addLog("WsHassioBridge::controlDevice with keyWord: %s", (const char *)pKeyObj["settings"]["keyWord"]);
char controlString[512];
char keyString[512];

    memset (controlString, 0, 512);
    memset (keyString, 0, 512);
    
    serializeJson(pKeyObj, keyString);
    sprintf(controlString, "{\"id\": %i, \"type\": \"fire_event\", \"event_type\": \"keyPressed\", \"event_data\": %s}", refNum, keyString);
    
    refNum = refNum + 1;
    WsDevice::controlDevice(controlString);
    
    MinionTools::addLog("WsHassioBridge::controlDevice sent controlString %s", controlString);
}

void openBridge(callBack pCallBack)
{
    receivedKey = pCallBack;
    WsDevice::openDevice(receivedMessage);
    return;
}
}