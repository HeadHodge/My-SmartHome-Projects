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

#include <SysTools.h>
#include <WsJsonOut.h>
#include <WsHassioBridge.h>

/////////////////////////////////////////////////////////////
namespace WsHassioBridge {
callBack receivedKey;
bool isBridgeOpen = false;
const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            
int refNum = 1;

/////////////////////////////////////////////////////////////
char *receivedMessage(unsigned char *pMessage) {    
const char* messageType;
DynamicJsonDocument doc(1024);

      deserializeJson(doc, (const char *)pMessage);  
      messageType = doc["type"];
      if(messageType == NULL) messageType = ""; 
      SysTools::addLog("WsHassioBridge::receivedMessage messageType: %s", messageType);
 
      if(strcmp(messageType, "auth_required") == 0) {
        SysTools::addLog("WsHassioBridge::receivedMessage Send accessToken: %s", accessToken);
        WsJsonOut::controlDevice(accessToken);
        return "";
      }     

      if(strcmp(messageType, "auth_ok") == 0) {
        isBridgeOpen = false;
        SysTools::addLog("%s", "WsHassioBridge::receivedMessage Bridge is Open");  
        return "";
      }

      if(strcmp(messageType, "result") == 0) {
        SysTools::addLog("WsHassioBridge::receivedMessage Command Result: %s", pMessage);
        return "";
      }
      
      return "";
}

/////////////////////////////////////////////////////////////
bool isOpen() {
    return isBridgeOpen;
}

/////////////////////////////////////////////////////////////
void refresh() {
    WsJsonOut::refresh();
}

void controlDevice(DynamicJsonDocument& pKeyObj) {
  SysTools::addLog("WsHassioBridge::controlDevice with keyWord: %s", (const char *)pKeyObj["required"]["keyWord"]);
  char controlString[512];
  char keyString[512];

    memset (controlString, 0, 512);
    memset (keyString, 0, 512);
    
    serializeJson(pKeyObj, keyString);
    sprintf(controlString, "{\"id\": %i, \"type\": \"fire_event\", \"event_type\": \"keyPressed\", \"event_data\": %s}", refNum, keyString);
    
    refNum = refNum + 1;
    WsJsonOut::controlDevice(controlString);
    
    SysTools::addLog("WsHassioBridge::controlDevice sent controlString %s", controlString);
}

/////////////////////////////////////////////////////////////
void open(callBack pCallBack)
{
    receivedKey = pCallBack;
    WsJsonOut::open(receivedMessage);
    return;
}
} //namespace WsHassioBridge