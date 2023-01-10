#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

#include <MinionTools.h>
#include <WsDevice.h>

namespace WsDevice
{ 
WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            
const char* silenceCommand = "{\"id\": 1, \"type\": \"call_service\", \"domain\": \"media_player\",\"service\": \"volume_mute\", \"target\": {\"entity_id\": \"media_player.master_bedroom\"}, \"service_data\":{\"is_volume_muted\": true}}";
bool isConnected = false;
callBack receivedMessage;

/*
void receivedMessage(unsigned char *pMessage) {    
const char* messageType;
DynamicJsonDocument doc(1024);

      deserializeJson(doc, (const char *)pMessage);  
      messageType = doc["type"];
      if(messageType == NULL) messageType = ""; 
      MinionTools::addLog("WsDevice::receivedMessage messageType: %s", messageType);
 
      if(strcmp(messageType, "auth_required") == 0) {
        MinionTools::addLog("WsDevice::receivedMessage Send accessToken: %s", accessToken);
        webSocket.sendTXT(accessToken);
        return;
      }     

      if(strcmp(messageType, "auth_ok") == 0) {
        MinionTools::addLog("WsDevice::receivedMessage Send silence command: %s", silenceCommand);
        webSocket.sendTXT(silenceCommand);
        return;
      }

      if(strcmp(messageType, "result") == 0) {
        MinionTools::addLog("WsDevice::receivedMessage Command Result: %s", pMessage);
        return;
      }
}
*/

void webSocketEvent(WStype_t type, uint8_t * message, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			MinionTools::addLog("%s", "WsDevice::webSocketEvent Disconnected!");
            isConnected = false;
			break;
		case WStype_CONNECTED:
			MinionTools::addLog("WsDevice::webSocketEvent Connected to url: %s", message);
            isConnected = true;
			break;
		case WStype_TEXT:
            receivedMessage(message);
			break;
		case WStype_BIN:
			MinionTools::addLog("WsDevice::webSocketEvent get binary length: %u", length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}

bool isDeviceConnected() {
    return isConnected;
}

void refreshDevice() {
    webSocket.loop();
}

void controlDevice(const char *pControlMessage) {
    webSocket.sendTXT(pControlMessage);
}

void openDevice(callBack pCallBack) {

    MinionTools::addLog("WsDevice::openDevice Connect to Wifi AP: '%s'", ssid);

    receivedMessage = pCallBack;
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

  MinionTools::addLog("%s", "WsDevice::openDevice Device is Open");
}
}