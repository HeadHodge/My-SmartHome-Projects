/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define US_KEYBOARD 1
#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h> // Install WebSockets by Markus Sattler from IDE Library manager
#include <ArduinoJson.h>  // Install from IDE Library manager

#include <SysTools.h>
#include <WsJsonIn.h>

namespace WsJsonIn {

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";

callBack messageReceived;
bool isDeviceConnected = false;
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *message, size_t length)
{
    SysTools::addLog("webSocketEvent(%d, %d, ...)", num, type);
    
    switch(type) {
    case WStype_DISCONNECTED:
        SysTools::addLog("[%u] Disconnected!", type);
        isDeviceConnected = false;
        break;
    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
      SysTools::addLog("[%u] Connected from %d.%d.%d.%d url: %s", type, ip[0], ip[1], ip[2], ip[3], message);
      isDeviceConnected = true;
    }
      break;
    case WStype_TEXT:
    {
      SysTools::addLog("[%u] Received Message: [%d] %s", type, length, message);
      SysTools::addLog("[%u] Reply Message: %s", type, messageReceived(message));
    }
      break;
    case WStype_BIN:
      SysTools::addLog("[%u] Binary!", type);
      break;
    case WStype_FRAGMENT_TEXT_START:
      SysTools::addLog("[%u] Fragment Text Start!", type);
      break;
    case WStype_FRAGMENT_BIN_START:
      SysTools::addLog("[%u] Fragment Binary Start!", type);
      break;
    case WStype_FRAGMENT:
      SysTools::addLog("[%u] Fragment!", type);
      break;
    case WStype_FRAGMENT_FIN:
      SysTools::addLog("[%u] Fragment Finish!", type);
      break;
    case WStype_PING:
      SysTools::addLog("[%u] Ping!", type);
      break;
    case WStype_PONG:
      SysTools::addLog("[%u] Pong!", type);
      break;
    case WStype_ERROR:
      SysTools::addLog("[%u] Error!", type);
      break;
    default:
      SysTools::addLog("Unknown WStype [%d]", type);
      break;
  }
  
  SysTools::addLog("%s", "");
}

bool isConnected() {
    return isDeviceConnected;
}

void refresh() {
    webSocket.loop();
}

void open(callBack pCallBack) {
    //messageReceived("Hello the Boat");
    messageReceived = pCallBack;
    
    // We start by connecting to a WiFi network
    SysTools::addLog("WsJsonIn::openHub Connect to Wifi AP: '%s'", ssid);

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    SysTools::addLog("%s", "");
    SysTools::addLog("WsJsonIn::open Connected to wifi AP: '%s', IP address: %s", ssid, WiFi.localIP().toString());

    //Listen for Connections
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    SysTools::addLog("%s", "WsJsonIn::openHub Waiting for Device Connection...");
    SysTools::addLog("%s", "WsJsonIn::openHub, Hub is Open");
}
} //namespace WsJsonIn