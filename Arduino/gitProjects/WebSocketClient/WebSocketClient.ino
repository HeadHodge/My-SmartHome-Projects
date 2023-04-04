/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <ArduinoJson.h>

//#include <WiFi.h>
//#include <WiFiMulti.h>
//#include <WiFiClientSecure.h>
//#include <WebSocketsClient.h>

#include <MinionTools.h>
#include <WsHassioBridge.h>

//const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            
//const char* silenceCommand = "{\"id\": 1, \"type\": \"call_service\", \"domain\": \"media_player\",\"service\": \"volume_mute\", \"target\": {\"entity_id\": \"media_player.master_bedroom\"}, \"service_data\":{\"is_volume_muted\": true}}";

/*
WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            
const char* silenceCommand = "{\"id\": 1, \"type\": \"call_service\", \"domain\": \"media_player\",\"service\": \"volume_mute\", \"target\": {\"entity_id\": \"media_player.master_bedroom\"}, \"service_data\":{\"is_volume_muted\": true}}";


//#define USE_SERIAL Serial1

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	MinionTools::addLog("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			MinionTools::addLog("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		MinionTools::addLog("%02X ", *src);
		src++;
	}
	MinionTools::addLog("%s", "\n");
}
*/

char *receivedWord(const char *pControlWord, const char *pDeviceBridge) {

  return "";
/*  
const char* messageType;
DynamicJsonDocument doc(1024);
//StaticJsonDocument<2048> doc;

      deserializeJson(doc, (const char *)pMessage);  
      messageType = doc["type"];
      if(messageType == NULL) messageType = ""; 
      MinionTools::addLog("WsDevice::receivedMessage messageType: %s", messageType);
 
      if(strcmp(messageType, "auth_required") == 0) {
        MinionTools::addLog("WsDevice::receivedMessage Send accessToken: %s", accessToken);
        WsDevice::controlDevice(accessToken);
        return "";
      }     

      if(strcmp(messageType, "auth_ok") == 0) {
        MinionTools::addLog("WsDevice::receivedMessage Send silence command: %s", silenceCommand);
        WsDevice::controlDevice(silenceCommand);
        return "";
      }

      if(strcmp(messageType, "result") == 0) {
        MinionTools::addLog("WsDevice::receivedMessage Command Result: %s", pMessage);
        return "";
      }
 */     
      return "";
}
/*
void webSocketEvent(WStype_t type, uint8_t * message, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			MinionTools::addLog("%s", "WsDevice::webSocketEvent Disconnected!");
			break;
		case WStype_CONNECTED:
			MinionTools::addLog("WsDevice::webSocketEvent Connected to url: %s", message);
			break;
		case WStype_TEXT:
      receivedMessage(message);
			break;
		case WStype_BIN:
			MinionTools::addLog("WsDevice::webSocketEvent get binary length: %u", length);
			hexdump(message, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}

void openDevice() {

    // We start by connecting to a WiFi network
    MinionTools::addLog("WsDevice::openDevice Connect to Wifi AP: '%s'", ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    MinionTools::addLog("%s", "");
    MinionTools::addLog("WsDevice::openDevice, Connected to wifi AP: '%s', IP address: %s", ssid, WiFi.localIP().toString());

  // server address, port and URL
  webSocket.begin("192.168.0.224", 8123, "/api/websocket");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
}
*/
void setup() {
	  MinionTools::openLog(115200);
    MinionTools::addLog("%s", "Open WebSocketClient");

    WsHassioBridge::openBridge(receivedWord);
}

void loop() {
	WsHassioBridge::refreshBridge();
}
