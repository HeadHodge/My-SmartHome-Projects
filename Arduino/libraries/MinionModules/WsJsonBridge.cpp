/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define US_KEYBOARD 1
#include <Arduino.h>

#include <MinionTools.h>
#include <WsHub.h>
#include <WsJsonBridge.h>

namespace WsHubBridge
{
const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

receivedKeyCallback receivedKey;
bool isConnected = false;
DynamicJsonDocument messageObj(512);
DynamicJsonDocument keyObj(512);
bool isMuted = false;

char *messageReceived(unsigned char *pMessage){
    MinionTools::addLog("%s", "WsJsonBridge::messageReceived");
       
    deserializeJson(messageObj, (const char *)pMessage);
    
    keyObj["key"] = messageObj["settings"]["key"];
    if(keyObj["key"] == NULL) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'key' missing in message");
        return "";
    }
    
    keyObj["from"] = messageObj["settings"]["from"];
    if(keyObj["key"] == NULL) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'from' missing in message");
        return "";
    }
       
    if (strcmp(keyObj["key"], "Louder") == 0) {
        keyObj["bridge"] = "WSDEVICEBRIDGE";
    } else if (strcmp(keyObj["key"], "Softer") == 0) {
        keyObj["bridge"] = "WSDEVICEBRIDGE";
    } else if (strcmp(keyObj["key"], "Silence") == 0) {
        keyObj["bridge"] = "WSDEVICEBRIDGE";
    } else if (strcmp(keyObj["key"], "Sound") == 0) {
        keyObj["bridge"] = "WSDEVICEBRIDGE";
    } else if (strcmp(keyObj["key"], "SoundToggle") == 0) {
        keyObj["bridge"] = "WSDEVICEBRIDGE";

        if(isMuted == false) {
            isMuted = true;
            keyObj["key"] = "Silence";
        } else {
            isMuted = false;
            keyObj["key"] = "Sound";
        };       
    } else {
        keyObj["bridge"] = "BLEHUBBRIDGE";        
    };
 
    /*
        "Louder": [
            {"id": 0, "type": "call_service", "domain": "media_player", "service": "volume_up", "service_data": {"entity_id": "media_player.master_bedroom"}},
        ],
        
        "Softer": [
            {"type": "call_service", "domain": "media_player", "service": "volume_down", "service_data": {"entity_id": "media_player.master_bedroom"}},
        ],
                
        "Silence": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": True}},
        ],
        
        "Sound": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": False}},
        ],
    */
    
    //MinionTools::addLog("WsJsonBridge::messageReceived controlWord: '%s', deviceBridge: '%s'\r\n", controlWord, deviceBridge);
    receivedKey(keyObj);
    return "";
}

bool isBridgeConnected() {
    return isConnected;
}

void refreshBridge() {
    WsHub::refreshHub();
}

void openBridge(receivedKeyCallback pReceivedKey)
{
    receivedKey = pReceivedKey;
    WsHub::openHub(messageReceived);
    MinionTools::addLog("%s", "WsJsonBridge::openBridge Bridge is Open");  
    return;
}
}