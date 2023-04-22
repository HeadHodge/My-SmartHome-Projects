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

char** _connectInfo = nullptr;
char** _endpointInfo = nullptr;

WebSocketsServer* _webSocketServer;
WebSocketsClient* _webSocketClient;
bool _isAPConnected = false;
bool _isServerEnabled = false;
bool _isClientEnabled = false;
bool _isServerConnected = false;
bool _isClientConnected = false;
void (*_onServerPkg)(DynamicJsonDocument& pPkg) = nullptr;
void (*_onClientPkg)(DynamicJsonDocument& pPkg) = nullptr;

/////////////////////////////////////////////////////////////
char** getConnectInfo() {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::getConnectInfo");

    return _connectInfo;
};

/////////////////////////////////////////////////////////////
bool sendReplyPkg(const char *pPkg) {
/////////////////////////////////////////////////////////////
 SysTools::addLog("wsEndpoint::sendJasonReplyStr: %s", pPkg);
     return _webSocketServer->sendTXT(0, pPkg);
}

/////////////////////////////////////////////////////////////
bool sendControlPkg(const char *pPkg) {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::sendJasonControlStr: %s", pPkg);
    return _webSocketClient->sendTXT(pPkg);
}

/////////////////////////////////////////////////////////////
bool sendControlPkg(DynamicJsonDocument& pPkg) {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::sendControlPkg");
  char pkg[1024];
  
    if(serializeJson(pPkg, pkg) == 0){
        SysTools::addLog("wsEndpoint::sendControlPkg, ABORT: Serialize Pkg Failed");
        return false;
    };
        
    return sendControlPkg(pkg);
}

//////////////////////////////////////////////////////////////
void refresh() {
//////////////////////////////////////////////////////////////
    if(_isServerEnabled == true) _webSocketServer->loop();
    if(_isClientEnabled == true) _webSocketClient->loop();
}

/////////////////////////////////////////////////////////////
bool onPkgInput(uint8_t* pPkgStr, size_t pLength, void (*pOnPkgInput)(DynamicJsonDocument& pPkgObj) = nullptr) {
/////////////////////////////////////////////////////////////
  //SysTools::addLog("wsEndpoint::onPkgInput, pLength: %lu, pPkgStr: %s", pLength, pPkgStr);
  if(pOnPkgInput == nullptr) return false;
  DynamicJsonDocument pkgObj(pLength*2);

    DeserializationError err = deserializeJson(pkgObj, (const char*)pPkgStr);
    
    if (err) {
        SysTools::addLog("wsEndpoint::onPkgInput, ABORT: Invalid Pkg, Reason: %s", err.c_str());
        return false;
    };
    
    //SysTools::addLog("wsEndpoint::onPkgInput, Before pkgObj Shrink, memoryUsage: '%lu', capacity: '%lu'", pkgObj.memoryUsage(), pkgObj.capacity());
    pkgObj.shrinkToFit();
    SysTools::addLog("wsEndpoint::onPkgInput, pPkgStr: %s, \nmemoryUsage: '%lu', capacity: '%lu'", pPkgStr, pkgObj.memoryUsage(), pkgObj.capacity());

    pOnPkgInput(pkgObj);
    return true;
}

/////////////////////////////////////////////////////////////
void webClientEvent(WStype_t type, uint8_t* input, size_t length) {
/////////////////////////////////////////////////////////////

	switch(type) {
		case WStype_DISCONNECTED:
			SysTools::addLog("%s", "wsEndpoint::webClientEvent Disconnected!");
            _isClientConnected = false;
			break;
		case WStype_CONNECTED:
			SysTools::addLog("wsEndpoint::webClientEvent Connected to url: %s", input);
            _isClientConnected = true;
			break;
		case WStype_TEXT:
            onPkgInput(input, length, _onClientPkg);
			break;
		case WStype_BIN:
			SysTools::addLog("wsEndpoint::webClientEvent get binary length: %u", length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
            SysTools::addLog("wsEndpoint::webClientEvent, Received Unknown Event Type(type: %d)", type);
			break;
	}
}

//////////////////////////////////////////////////////////////
void webServerEvent(uint8_t num, WStype_t type, uint8_t *input, size_t length) {
//////////////////////////////////////////////////////////////
    
    switch(type) {
        case WStype_DISCONNECTED:
            SysTools::addLog("wsEndpoint::webServerEvent[%u] Disconnected!", type);
            _isServerConnected = false;
            break;
        case WStype_CONNECTED:
            SysTools::addLog("wsEndpoint::webServerEvent[%u] Connected from %s url: %s", type, _connectInfo[CONNECT_ADDRESS], input);
            _isServerConnected = true;
            break;
        case WStype_TEXT:
            //SysTools::addLog("wsEndpoint::webServerEvent[%u] Received Input: [%d] %s", type, length, input);
            onPkgInput(input, length, _onServerPkg);
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
  
}

//////////////////////////////////////////////////////////////
bool isWifiConnected() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::isWifiConnected");

    return _isAPConnected;
}

//////////////////////////////////////////////////////////////
bool connectAP(char* pConnectInfo[]) {
//////////////////////////////////////////////////////////////
  if(_isAPConnected) return true;
  
  SysTools::addLog("wsEndpoint::connectWifi");
  uint32_t startTime = CURRENT_TIME;
  _connectInfo = pConnectInfo;
    
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
    _isAPConnected = true;
    return true;
}
  
//////////////////////////////////////////////////////////////
bool connectEndpoint(char* pConnectInfo[], char* pEndpointInfo[], void (*pOnClientPkg)(DynamicJsonDocument& pPkg)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::connectEndpoint");

    _endpointInfo = pEndpointInfo;
    connectAP(pConnectInfo);

    //Connection to Endpoint
    _onClientPkg = pOnClientPkg;
    _webSocketClient = new WebSocketsClient();

    // try every 5000 again if connection has failed
    _webSocketClient->setReconnectInterval(5000);

    // use HTTP Basic Authorization this is optional remove if not needed
    //_webSocketClient->setAuthorization("user", "Password");

    // event handler
    _webSocketClient->onEvent(webClientEvent);
    
    _webSocketClient->begin(pEndpointInfo[CONNECT_ADDRESS], atoi(pEndpointInfo[CONNECT_PORT]), pEndpointInfo[CONNECT_PATH]);

    SysTools::addLog("wsEndpoint::connectEndpoint %s", "wsClientDevice is Open");

    _isClientEnabled = true;
    return true;
}

//////////////////////////////////////////////////////////////
bool awaitEndpoint(char* pConnectInfo[], void (*pOnPkgInput)(DynamicJsonDocument& pObject)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoint::awaitEndpoint");
   
    _onServerPkg = pOnPkgInput;
    _connectInfo = pConnectInfo;
    connectAP(pConnectInfo);
    
    //Listen for Connections
    _webSocketServer = new WebSocketsServer(atoi(pConnectInfo[CONNECT_PORT]));
    _webSocketServer->onEvent(webServerEvent);
    _webSocketServer->begin();

    SysTools::addLog("wsEndpoint::awaitEndpoint, Waiting for Device Connection on Port: %s...", pConnectInfo[CONNECT_PORT]);

    _isServerEnabled = true;
    return true;
}

} //namespace WsEndpoint