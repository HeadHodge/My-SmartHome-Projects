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
callBack controlWordReceived;
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
        //MinionTools::addLog("WsHassioBridge::receivedMessage Send silence command: %s", silenceCommand);
        //WsDevice::controlDevice(silenceCommand);
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

void controlDevice(const char *pControlWord) {
MinionTools::addLog("WsHassioBridge::controlDevice with controlCode %s", pControlWord);
char buffer[512];

    memset (buffer, 0, 512);
     
    if (strcmp(pControlWord, "Louder") == 0) {
        sprintf(buffer, "{\"id\": %i, \"type\": \"call_service\", \"domain\": \"media_player\", \"service\": \"volume_up\", \"service_data\": {\"entity_id\": \"media_player.master_bedroom\"}}", refNum);
    } else if (strcmp(pControlWord, "Softer") == 0) {
        sprintf(buffer, "{\"id\": %i, \"type\": \"call_service\", \"domain\": \"media_player\", \"service\": \"volume_down\", \"service_data\": {\"entity_id\": \"media_player.master_bedroom\"}}", refNum);
    } else if (strcmp(pControlWord, "Silence") == 0) {
        sprintf(buffer, "{\"id\": %i, \"type\": \"call_service\", \"domain\": \"media_player\",\"service\": \"volume_mute\", \"target\": {\"entity_id\": \"media_player.master_bedroom\"}, \"service_data\":{\"is_volume_muted\": true}}", refNum);
    } else if (strcmp(pControlWord, "Sound") == 0) {
        sprintf(buffer, "{\"id\": %i, \"type\": \"call_service\", \"domain\": \"media_player\",\"service\": \"volume_mute\", \"target\": {\"entity_id\": \"media_player.master_bedroom\"}, \"service_data\":{\"is_volume_muted\": false}}", refNum);
    } else {
        return;        
    };
    
    refNum = refNum + 1;
    MinionTools::addLog("WsHassioBridge::controlDevice send controlCommand %s", buffer);
    WsDevice::controlDevice(buffer);

/*
    if (BleHub::isHubConnected() == false) {
        MinionTools::addLog("%s", "BleHidBridge::controlDevice Abrted: BleHub not Connected");
        return;
    }
    
    deviceOptions deviceOptions = translateControlWord(pControlWord);
    
    if(deviceOptions.deviceType == 0)
    {
        MinionTools::addLog("BleHidBridge::controlDevice Abort: Invalid contolWord '%s'", pControlWord);
        return;
    }

    MinionTools::addLog("BleHidBridge::Send Control Code: 0x%X to attached device", deviceOptions.controlCode);
    BleHub::controlDevice(deviceOptions.controlCode, deviceOptions.keyDuration, deviceOptions.keyDelay, deviceOptions.deviceType);
*/
}

void openBridge(callBack pCallBack)
{
    controlWordReceived = pCallBack;
    WsDevice::openDevice(receivedMessage);
    return;
}
}