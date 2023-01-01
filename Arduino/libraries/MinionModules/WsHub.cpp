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

#include <MinionTools.h>
#include <WsHub.h>

namespace WsHub
{

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
/*
#define DEBUG_ON 1
#if DEBUG_ON
#define DBG_begin(...)    Serial.begin(__VA_ARGS__)
#define DBG_print(...)    Serial.print(__VA_ARGS__)
#define DBG_println(...)  Serial.println(__VA_ARGS__)
#define DBG_printf(...)   Serial.printf(__VA_ARGS__)
#else
#define DBG_begin(...)
#define DBG_print(...)
#define DBG_println(...)
#define DBG_printf(...)
#endif
*/
callBack messageReceived;
bool isConnected = false;
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *message, size_t length)
{
    MinionTools::addLog("webSocketEvent(%d, %d, ...)", num, type);
    
    switch(type) {
    case WStype_DISCONNECTED:
      MinionTools::addLog("[%u] Disconnected!", type);
      isConnected = false;
      break;
    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
      MinionTools::addLog("[%u] Connected from %d.%d.%d.%d url: %s", type, ip[0], ip[1], ip[2], ip[3], message);
      isConnected = true;
    }
      break;
    case WStype_TEXT:
    {
      MinionTools::addLog("[%u] Received Message: [%d] %s", type, length, message);
      MinionTools::addLog("[%u] Reply Message: %s", type, messageReceived(message));
    }
      break;
    case WStype_BIN:
      MinionTools::addLog("[%u] Binary!", type);
      break;
    case WStype_FRAGMENT_TEXT_START:
      MinionTools::addLog("[%u] Fragment Text Start!", type);
      break;
    case WStype_FRAGMENT_BIN_START:
      MinionTools::addLog("[%u] Fragment Binary Start!", type);
      break;
    case WStype_FRAGMENT:
      MinionTools::addLog("[%u] Fragment!", type);
      break;
    case WStype_FRAGMENT_FIN:
      MinionTools::addLog("[%u] Fragment Finish!", type);
      break;
    case WStype_PING:
      MinionTools::addLog("[%u] Ping!", type);
      break;
    case WStype_PONG:
      MinionTools::addLog("[%u] Pong!", type);
      break;
    case WStype_ERROR:
      MinionTools::addLog("[%u] Error!", type);
      break;
    default:
      MinionTools::addLog("Unknown WStype [%d]", type);
      break;
  }
  
  MinionTools::addLog("%s", "");
}

bool isHubConnected() {
    return isConnected;
}

void refreshHub() {
    webSocket.loop();
}

void openHub(callBack pCallBack)
{
    //messageReceived("Hello the Boat");
    messageReceived = pCallBack;
    
    // We start by connecting to a WiFi network
    MinionTools::addLog("WsHub::openHub Connect to Wifi AP: '%s'", ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    MinionTools::addLog("%s", "");
    MinionTools::addLog("WsHub::openHub Connected to wifi AP: '%s', IP address: %s", ssid, WiFi.localIP().toString());

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    MinionTools::addLog("%s", "WsHub::openHub Waiting for Device Connection...");
    MinionTools::addLog("%s", "WsHub::openHub, Hub is Open");
}
}