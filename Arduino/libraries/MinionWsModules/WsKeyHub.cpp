/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define US_KEYBOARD 1
#include <Arduino.h>

#include <SysTools.h>
#include <WsEndpoints.h>
#include <WsKeyHub.h>

namespace WsKeyHub {
const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

receivedKeyCallback receivedKey;
bool isBridgeConnected = false;
DynamicJsonDocument messageObj(512);

char* _localEndpointInfo[] = {
    "WAP2G-MASTERBEDROOM",
    "Pin#95833",
    "000.000.000.000",
    "80"
};

char *messageReceived(unsigned char *pMessageJson){
    deserializeJson(messageObj, pMessageJson);
      
    if(messageObj["event"] == nullptr) {
        SysTools::addLog("%s", "WsKeyHub::messageReceived Abort: 'event' value missing in message");
        return "";
    }
 
    if(messageObj["event"] != "keyPressed") {
        SysTools::addLog("%s", "WsKeyHub::messageReceived Abort: invalid 'event' value in message");
        return "";
    }
   
    if(messageObj["required"] == nullptr) {
        SysTools::addLog("%s", "WsKeyHub::messageReceived Abort: 'required' values missing in message");
        return "";
    }
   
    if(messageObj["required"]["keyWord"] == nullptr) {
        SysTools::addLog("%s", "WsKeyHub::messageReceived Abort: 'required.keyWord' value missing in message");
        return "";
    }
    
    if(messageObj["optional"] == nullptr) {
        SysTools::addLog("%s", "WsKeyHub::messageReceived Abort: 'optional' values missing in message");
        return "";
    }
    
    if(messageObj["source"] == nullptr) messageObj["source"] = "";
    
    if(messageObj["location"] == nullptr) messageObj["location"] = "";
    
    receivedKey(messageObj);
    return "";
}

bool isConnected() {
    return isBridgeConnected;
}

void refresh() {
    WsEndpoints::refresh();
}

void open(receivedKeyCallback pReceivedKey)
{
    receivedKey = pReceivedKey;
    //WsEndpoints::awaitEndpoint(_localEndpointInfo);
    //(messageReceived);
    
    SysTools::addLog("%s", "WsKeyHub::open Hub is Open");  
    return;
}
} //namespace WsKeyHub
