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

char *messageReceived(unsigned char *pMessageJson){
    MinionTools::addLog("%s", "WsJsonBridge::messageReceived");
       
    deserializeJson(messageObj, pMessageJson);
      
    if(messageObj["event"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'event' value missing in message");
        return "";
    }
 
    if(messageObj["event"] != "keyPressed") {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: invalid 'event' value in message");
        return "";
    }
   
    if(messageObj["settings"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'settings' values missing in message");
        return "";
    }
   
    if(messageObj["settings"]["keyWord"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'settings.keyWord' value missing in message");
        return "";
    }
    
    if(messageObj["options"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'options' values missing in message");
        return "";
    }
    
    if(messageObj["source"] == nullptr) messageObj["source"] = "";
    
    if(messageObj["location"] == nullptr) messageObj["location"] = "";
    
    if(messageObj["targets"] == nullptr) messageObj["targets"] = 0;
    
    if(messageObj["options"]["keyModifier"] == nullptr) messageObj["options"]["keyModifier"] = 0;
    
    receivedKey(messageObj);
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