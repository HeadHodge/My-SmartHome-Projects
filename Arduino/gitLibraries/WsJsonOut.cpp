#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

#include <SysTools.h>
#include <WsJsonOut.h>

namespace WsJsonOut
{ 
WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            
const char* silenceCommand = "{\"id\": 1, \"type\": \"call_service\", \"domain\": \"media_player\",\"service\": \"volume_mute\", \"target\": {\"entity_id\": \"media_player.master_bedroom\"}, \"service_data\":{\"is_volume_muted\": true}}";
bool isDeviceConnected = false;
callBack receivedMessage;

/////////////////////////////////////////////////////////////
void webSocketEvent(WStype_t type, uint8_t * message, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			SysTools::addLog("%s", "WsJsonOut::webSocketEvent Disconnected!");
            isDeviceConnected = false;
			break;
		case WStype_CONNECTED:
			SysTools::addLog("WsJsonOut::webSocketEvent Connected to url: %s", message);
            isDeviceConnected = true;
			break;
		case WStype_TEXT:
            receivedMessage(message);
			break;
		case WStype_BIN:
			SysTools::addLog("WsJsonOut::webSocketEvent get binary length: %u", length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}

/////////////////////////////////////////////////////////////
bool isConnected() {
    return isDeviceConnected;
}

/////////////////////////////////////////////////////////////
void refresh() {
    webSocket.loop();
}

/////////////////////////////////////////////////////////////
void controlDevice(const char *pControlMessage) {
    webSocket.sendTXT(pControlMessage);
}

/////////////////////////////////////////////////////////////
void open(callBack pCallBack) {

    SysTools::addLog("WsJsonOut::openDevice Connect to Wifi AP: '%s'", ssid);

    receivedMessage = pCallBack;
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    SysTools::addLog("%s", "");
    SysTools::addLog("WsJsonOut::openDevice, Connected to wifi AP: '%s', IP address: %s", ssid, WiFi.localIP().toString());

  // server address, port and URL
  webSocket.begin("192.168.0.224", 8123, "/api/websocket");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  SysTools::addLog("%s", "WsJsonOut::openDevice Device is Open");
}
} //namespace WsJsonOut