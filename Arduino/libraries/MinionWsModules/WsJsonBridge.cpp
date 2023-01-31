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
    deserializeJson(messageObj, pMessageJson);
      
    if(messageObj["event"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'event' value missing in message");
        return "";
    }
 
    if(messageObj["event"] != "keyPressed") {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: invalid 'event' value in message");
        return "";
    }
   
    if(messageObj["required"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'required' values missing in message");
        return "";
    }
   
    if(messageObj["required"]["keyWord"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'required.keyWord' value missing in message");
        return "";
    }
    
    if(messageObj["optional"] == nullptr) {
        MinionTools::addLog("%s", "WsJsonBridge::messageReceived Abort: 'optional' values missing in message");
        return "";
    }
    
    if(messageObj["source"] == nullptr) messageObj["source"] = "";
    
    if(messageObj["location"] == nullptr) messageObj["location"] = "";
    
    //if(messageObj["targets"] == nullptr) messageObj["targets"] = 0;
    
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