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
#include <WebSocketsClient.h>
#include <ArduinoJson.h>  // Install from IDE Library manager

#include <SysTools.h>
#include <WsEndpoint.h>

namespace WsEndpoint {

char* _connectInfo[] = {
    "WAP2G-MASTERBEDROOM",
    "Pin#95833",
    "000.000.000.000",
    "80"
};

WebSocketsServer* _webSocketServer;
WebSocketsClient* _webSocketClient;
bool _isWifiConnected = false;
bool _isServerEnabled = false;
bool _isClientEnabled = false;
bool _isServerConnected = false;
bool _isClientConnected = false;
void (*_onServerObject)(DynamicJsonDocument *pObject) = nullptr;
void (*_onClientObject)(DynamicJsonDocument *pObject) = nullptr;

/////////////////////////////////////////////////////////////
char** getConnectInfo() {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::getConnectInfo");

    return _connectInfo;
};

/////////////////////////////////////////////////////////////
bool onEndpointInput(uint8_t* pInput, size_t pLength, void (*pOnEndpointObject)(DynamicJsonDocument *pObject) = nullptr) {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::onEndpointInput, pLength: %lu, pInput: %s", pLength, pInput);
  if(pOnEndpointObject == nullptr) return false;
  DynamicJsonDocument doc(pLength*2);

    DeserializationError err = deserializeJson(doc, (const char*)pInput);
    
    if (err) {
        SysTools::addLog("wsEndpoint::onEndpointInput, ABORT: Invalid Jason, Reason: %s", err.c_str());
        return false;
    };
    
    SysTools::addLog("wsEndpoint::onEndpointInput, Before Doc Shrink, memoryUsage: '%lu', capacity: '%lu'", doc.memoryUsage(), doc.capacity());
    doc.shrinkToFit();
    SysTools::addLog("wsEndpoint::onEndpointInput, After Doc Shrink,  memoryUsage: '%lu', capacity: '%lu'", doc.memoryUsage(), doc.capacity());

    pOnEndpointObject(&doc);
    return true;
}

/////////////////////////////////////////////////////////////
void webClientEvent(WStype_t type, uint8_t* input, size_t length) {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::webClientEvent(type: %d)", type);

	switch(type) {
		case WStype_DISCONNECTED:
			SysTools::addLog("%s", "wsEndpoint::webClientEvent Disconnected!");
            _isServerConnected = false;
			break;
		case WStype_CONNECTED:
			SysTools::addLog("wsEndpoint::webClientEvent Connected to url: %s", input);
            _isServerConnected = true;
			break;
		case WStype_TEXT:
            onEndpointInput(input, length, _onClientObject);
			break;
		case WStype_BIN:
			SysTools::addLog("wsEndpoint::webClientEvent get binary length: %u", length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}

//////////////////////////////////////////////////////////////
void webServerEvent(uint8_t num, WStype_t type, uint8_t *input, size_t length) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::webServerEvent(num: %d, type: %d)", num, type);
    
    switch(type) {
    case WStype_DISCONNECTED:
        SysTools::addLog("wsEndpoint::webServerEvent[%u] Disconnected!", type);
        _isServerConnected = false;
        break;
    case WStype_CONNECTED:
    {
      //IPAddress ip = webSocket.remoteIP(num);
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Connected from %s url: %s", type, _connectInfo[CONNECT_ADDRESS], input);
      _isServerConnected = true;
    }
      break;
    case WStype_TEXT:
    {
        SysTools::addLog("wsEndpoint::webServerEvent[%u] Received Input: [%d] %s", type, length, input);
    }
        break;
    case WStype_BIN:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Binary!", type);
      break;
    case WStype_FRAGMENT_TEXT_START:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Fragment Text Start!", type);
      break;
    case WStype_FRAGMENT_BIN_START:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Fragment Binary Start!", type);
      break;
    case WStype_FRAGMENT:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Fragment!", type);
      break;
    case WStype_FRAGMENT_FIN:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Fragment Finish!", type);
      break;
    case WStype_PING:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Ping!", type);
      break;
    case WStype_PONG:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Pong!", type);
      break;
    case WStype_ERROR:
      SysTools::addLog("wsEndpoint::webServerEvent[%u] Error!", type);
      break;
    default:
      SysTools::addLog("wsEndpoint::webServerEvent Unknown WStype [%d]", type);
      break;
  }
  
  SysTools::addLog("wsEndpoint::webServerEvent, %s", "");
}

//////////////////////////////////////////////////////////////
bool isWifiConnected() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::isWifiConnected");

    return _isWifiConnected;
}

/////////////////////////////////////////////////////////////
void replyEndpoint(const char *pJson) {
/////////////////////////////////////////////////////////////
    _webSocketServer->sendTXT(0, pJson);
}

/////////////////////////////////////////////////////////////
void sendEndpoint(const char *pJson) {
/////////////////////////////////////////////////////////////
    _webSocketClient->sendTXT(pJson);
}

//////////////////////////////////////////////////////////////
void refresh() {
//////////////////////////////////////////////////////////////
    if(_isServerEnabled == true) _webSocketServer->loop();
    if(_isClientEnabled == true) _webSocketClient->loop();
}

//////////////////////////////////////////////////////////////
bool connectWifi(char* pConnectInfo[]) {
//////////////////////////////////////////////////////////////
  if(_isWifiConnected) return true;
  
  SysTools::addLog("wsEndpoint::connectWifi");
  uint32_t startTime = CURRENT_TIME;
    
    // We start by connecting to a WiFi network
    SysTools::addLog("wsEndpoint::connectWifi Connect to Wifi AP: '%s', Wifi PSW: '%s'", pConnectInfo[CONNECT_SSID], pConnectInfo[CONNECT_PASSWORD]);

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(pConnectInfo[CONNECT_SSID], pConnectInfo[CONNECT_PASSWORD]);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
        if(CURRENT_TIME - startTime >= 60000) return false;
    }

    sprintf(pConnectInfo[CONNECT_ADDRESS], "%s", WiFi.localIP().toString());

    SysTools::addLog("wsEndpoint::connectWifi Connected to SSID: '%s', Password: %s, Address: %s", pConnectInfo[CONNECT_SSID], pConnectInfo[CONNECT_PASSWORD], pConnectInfo[CONNECT_ADDRESS]);
    _isWifiConnected = true;
    return true;
}
  
//////////////////////////////////////////////////////////////
bool connectEndpoint(char* pConnectInfo[], char* pEndpointInfo[], void (*pOnEndpointObject)(DynamicJsonDocument *pObject)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::connectEndpoint");

    _onClientObject = pOnEndpointObject;
    connectWifi(pConnectInfo);
    _webSocketClient = new WebSocketsClient();

    // try every 5000 again if connection has failed
    _webSocketClient->setReconnectInterval(5000);

    // use HTTP Basic Authorization this is optional remove if not needed
    //_webSocketClient->setAuthorization("user", "Password");

    // event handler
    _webSocketClient->onEvent(webClientEvent);
    
    // server address, port and URL
    //pEndpointInfo[CONNECT_ADDRESS] = "192.168.0.224";
    //pEndpointInfo[CONNECT_PORT] = "8123";
    _webSocketClient->begin(pEndpointInfo[CONNECT_ADDRESS], atoi(pEndpointInfo[CONNECT_PORT]), pEndpointInfo[CONNECT_PATH]);

    SysTools::addLog("wsEndpoint::connectEndpoint %s", "wsClientDevice is Open");

    _isClientEnabled = true;
    return true;
}

//////////////////////////////////////////////////////////////
bool awaitEndpoint(char* pConnectInfo[], void (*pOnEndpointObject)(DynamicJsonDocument *pObject)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::awaitEndpoint");
   
    _onServerObject = pOnEndpointObject;
    connectWifi(pConnectInfo);
    
    //Listen for Connections
    _webSocketServer = new WebSocketsServer(atoi(pConnectInfo[CONNECT_PORT]));
    _webSocketServer->onEvent(webServerEvent);
    _webSocketServer->begin();

    SysTools::addLog("wsEndpoint::awaitEndpoint, Waiting for Device Connection on Port: %u...", pConnectInfo[CONNECT_PORT]);

    _isServerEnabled = true;
    return true;
}

} //namespace WsEndpoint