/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define US_KEYBOARD 1
//#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>

#include <SysTools.h>
#include <WsEndpoints.h>

namespace WsEndpoints {
AsyncWebServer   _httpServer(80);
WebSocketsServer _wsServer = WebSocketsServer(8000);
WebSocketsClient _wsClient = WebSocketsClient();
AsyncWebServerRequest* _httpRequest = nullptr;
IPAddress _wsClientAddress;

//char** _connectInfo = nullptr;
//char** _endpointInfo = nullptr;

bool _isServerEnabled = false;
bool _isClientEnabled = false;
bool _isApEnabled = false;
bool _isServerConnected = false;
bool _isClientConnected = false;
bool _isNetworkConnected = false;

void (*_onServerPkg)(DynamicJsonDocument& pPkg) = nullptr;
void (*_onClientPkg)(DynamicJsonDocument& pPkg) = nullptr;
onEndpointPkg _onPkgInput = nullptr;

/////////////////////////////////////////////////////////////
bool sendReplyPkg(const char *pPkg) {
/////////////////////////////////////////////////////////////
 SysTools::addLog("wsEndpoints::sendJasonReplyStr: %s", pPkg);
     //return _wsServerServer->sendTXT(0, pPkg);
}

/////////////////////////////////////////////////////////////
bool sendControlPkg(const char *pPkg) {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::sendJasonControlStr: %s", pPkg);
    //return _wsServerClient->sendTXT(pPkg);
}

/////////////////////////////////////////////////////////////
bool sendControlPkg(DynamicJsonDocument& pPkg) {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::sendControlPkg");
  char pkg[1024];
  
    if(serializeJson(pPkg, pkg) == 0){
        SysTools::addLog("wsEndpoints::sendControlPkg, ABORT: Serialize Pkg Failed");
        return false;
    };
        
    return sendControlPkg(pkg);
}

/////////////////////////////////////////////////////////////
bool onPkgInput(uint8_t* pPkgStr, size_t pLength, void (*pOnPkgInput)(DynamicJsonDocument& pPkgObj) = nullptr) {
/////////////////////////////////////////////////////////////
  //SysTools::addLog("wsEndpoints::onPkgInput, pLength: %lu, pPkgStr: %s", pLength, pPkgStr);
  if(pOnPkgInput == nullptr) return false;
  DynamicJsonDocument pkgObj(pLength*2);

    DeserializationError err = deserializeJson(pkgObj, (const char*)pPkgStr);
    
    if (err) {
        SysTools::addLog("wsEndpoints::onPkgInput, ABORT: Invalid Pkg, Reason: %s", err.c_str());
        return false;
    };
    
    //SysTools::addLog("wsEndpoints::onPkgInput, Before pkgObj Shrink, memoryUsage: '%lu', capacity: '%lu'", pkgObj.memoryUsage(), pkgObj.capacity());
    pkgObj.shrinkToFit();
    SysTools::addLog("wsEndpoints::onPkgInput, pPkgStr: %s, \nmemoryUsage: '%lu', capacity: '%lu'", pPkgStr, pkgObj.memoryUsage(), pkgObj.capacity());

    pOnPkgInput(pkgObj);
    return true;
}

//////////////////////////////////////////////////////////////
bool isNetworkConnected() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::isNetworkConnected");

    return _isNetworkConnected;
}

//////////////////////////////////////////////////////////////
void sendHttpEndpointPkg(DynamicJsonDocument* pEndpointPkg) {
//////////////////////////////////////////////////////////////
  if(_httpRequest == nullptr) return;
  SysTools::addLog("wsEndpoints::sendHttpEndpointPkg, HomePage: %s", (const char*)(*pEndpointPkg)["homePage"]);

    _httpRequest->send(200, "text/html", (const char*)(*pEndpointPkg)["homePage"]);
    _httpRequest = nullptr;
}

//////////////////////////////////////////////////////////////
void refresh() {
//////////////////////////////////////////////////////////////
    _wsServer.loop();
}

/////////////////////////////////////////////////////////////
void onWsClientEvent(WStype_t type, uint8_t* input, size_t length) {
/////////////////////////////////////////////////////////////

	switch(type) {
		case WStype_DISCONNECTED:
			SysTools::addLog("%s", "wsEndpoints::webClientEvent Disconnected!");
            _isClientConnected = false;
			break;
		case WStype_CONNECTED:
			SysTools::addLog("wsEndpoints::webClientEvent Connected to url: %s", input);
            _isClientConnected = true;
			break;
		case WStype_TEXT:
            onPkgInput(input, length, _onClientPkg);
			break;
		case WStype_BIN:
			SysTools::addLog("wsEndpoints::webClientEvent get binary length: %u", length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
            SysTools::addLog("wsEndpoints::webClientEvent, Received Unknown Event Type(type: %d)", type);
			break;
	}
}

//////////////////////////////////////////////////////////////
bool connectWsEndpoint(char* pEndpointInfo[], void (*pOnClientPkg)(DynamicJsonDocument& pPkg)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::connectEndpoint");

    //_endpointInfo = pEndpointInfo;
    
    //Set Client Options
    _wsClient.setReconnectInterval(5000); // try every 5000 again if connection has failed    
    _wsClient.onEvent(onWsClientEvent); // Set event handler
    //_wsClient.>setAuthorization("user", "Password"); // use HTTP Basic Authorization this is optional remove if not needed
    _wsClient.begin("192.168.0.102", 8123, "/");

    SysTools::addLog("wsEndpoints::connectEndpoint %s", "wsClient is Connecting to Endpoint");

    _isClientEnabled = true;
    return true;
}

// Callback: receiving any WebSocket message
void onWsServerEvent(
    uint8_t client_num,
    WStype_t type,
    uint8_t *payload,
    size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {
    case WStype_CONNECTED:
        // New client has connected
      _wsClientAddress = _wsServer.remoteIP(client_num);
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], Connected to Endpoint: %s", client_num, _wsClientAddress.toString());
      break;

    case WStype_DISCONNECTED:
      // Client has disconnected
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], Disconnected!\n", client_num);
      break;

    // Handle text messages from client
    case WStype_TEXT:
      // Print out raw message
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], Received text: %s\n", client_num, payload);
      _wsServer.sendTXT(client_num, "{Thank You}");
      break;

    // For everything else: do nothing
    case WStype_BIN:
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], ABORT: Unknown Event WStype_BIN");
    case WStype_ERROR:
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], ABORT: Unknown Event WStype_ERROR");
    case WStype_FRAGMENT_TEXT_START:
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], ABORT: Unknown Event WStype_FRAGMENT_TEXT_START");
    case WStype_FRAGMENT_BIN_START:
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], ABORT: Unknown Event WStype_FRAGMENT_BIN_START");
    case WStype_FRAGMENT:
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], ABORT: Unknown Event WStype_FRAGMENT");
    case WStype_FRAGMENT_FIN:
    default:
      SysTools::addLog("wsEndpoints::onWsServerEvent[%u], ABORT: Unknown Event %u", client_num, type);
      break;
  }
}
 
//////////////////////////////////////////////////////////////
void onHttpServerEvent(AsyncWebServerRequest *pRequest) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::onHttpServerEvent");

    _httpRequest = pRequest;
    _onPkgInput("web", nullptr, &sendHttpEndpointPkg);
    //request->send(200, "text/html", "<b>Hello, world</b><script>alert('Hello World!');</script>");
}

//////////////////////////////////////////////////////////////
bool startServers() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::startServers");
  
    _httpServer.on("/", HTTP_GET, onHttpServerEvent);   
    _httpServer.begin();
    
    _wsServer.begin();
    _wsServer.onEvent(onWsServerEvent);
  
    return true;
}

//////////////////////////////////////////////////////////////
bool createWifiAP() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::createWifiAP");

    // You can remove the password parameter if you want the AP to be open.
    // a valid password must have more than 7 characters
    WiFi.softAP("smartRemotes", "");
    
    //IPAddress myIP = WiFi.softAPIP();
    SysTools::addLog("wsEndpoints::createWifiAP, AP IP address: %s", WiFi.softAPIP().toString());
    
    startServers();
    SysTools::displayAPConnection();

    _isNetworkConnected = true;

    SysTools::addLog("wsEndpoints::createWifiAP, Wifi AP Started");
    return true;
}

//////////////////////////////////////////////////////////////
bool connectWifiAP() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::connectWifiAP");
  if(_isNetworkConnected) return true;

  char* ssid = "WAP2G-MASTERBEDROOM";
  char* password = "Pin#95833";
  uint32_t startTime = CURRENT_TIME;
    
    // We start by connecting to a WiFi network
    SysTools::addLog("wsEndpoints::connectAP Connect to Wifi AP: '%s', Wifi PSW: '%s'", ssid, password);

    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);
        
    Serial.print(".");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        if(CURRENT_TIME - startTime >= 15000) {
            Serial.println("");
            SysTools::addLog("wsEndpoints::connectAP ABORT: Connect AP: '%s' Failed\n", ssid);
            WiFi.disconnect();
            return false;
        }
    }
    Serial.println("");
    
    startServers();
    SysTools::displayStationConnection();

    //DONE
    SysTools::addLog("wsEndpoints::connectAP Connected as Address: %s\n", WiFi.localIP().toString());
    _isNetworkConnected = true;
    SysTools::displayStationConnection();
    return true;
}

//////////////////////////////////////////////////////////////
bool enable(onEndpointPkg pOnPkgInput) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::enable");
_onPkgInput = pOnPkgInput;

    if(_isNetworkConnected) {
        SysTools::addLog("wsEndpoints::enable, Network already connected");       
        return true;
    }
   
    if(connectWifiAP()) {
        SysTools::addLog("wsEndpoints::enable, WifiAP Connected");
        _isNetworkConnected = true;
        return true;
    };

    
    if(createWifiAP()) {
        SysTools::addLog("wsEndpoints::enable, WifiAP Created");
        _isNetworkConnected = true;
        return true;
    };

    SysTools::addLog("wsEndpoints::enable, ABORT: Unable to Connect Network");
    _isNetworkConnected = false;
    return false;
}
} //namespace WsEndpoints