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

char** _connectInfo = nullptr;
char** _endpointInfo = nullptr;

bool _isServerEnabled = false;
bool _isClientEnabled = false;
bool _isApEnabled = false;
bool _isServerConnected = false;
bool _isClientConnected = false;
bool _isNetworkConnected = false;

void (*_onServerPkg)(DynamicJsonDocument& pPkg) = nullptr;
void (*_onClientPkg)(DynamicJsonDocument& pPkg) = nullptr;

/////////////////////////////////////////////////////////////
char** getConnectInfo() {
/////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::getConnectInfo");

    return _connectInfo;
};

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

/////////////////////////////////////////////////////////////
void webClientEvent(WStype_t type, uint8_t* input, size_t length) {
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
void webServerEvent(uint8_t num, WStype_t type, uint8_t *input, size_t length) {
//////////////////////////////////////////////////////////////
    
    switch(type) {
        case WStype_DISCONNECTED:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Disconnected!", type);
            _isServerConnected = false;
            break;
        case WStype_CONNECTED:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Connected from %s url: %s", type, _connectInfo[CONNECT_ADDRESS], input);
            _isServerConnected = true;
            break;
        case WStype_TEXT:
            //SysTools::addLog("wsEndpoints::webServerEvent[%u] Received Input: [%d] %s", type, length, input);
            onPkgInput(input, length, _onServerPkg);
            break;
        case WStype_BIN:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Binary!", type);
            break;
        case WStype_FRAGMENT_TEXT_START:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Fragment Text Start!", type);
            break;
        case WStype_FRAGMENT_BIN_START:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Fragment Binary Start!", type);
            break;
        case WStype_FRAGMENT:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Fragment!", type);
            break;
        case WStype_FRAGMENT_FIN:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Fragment Finish!", type);
            break;
        case WStype_PING:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Ping!", type);
            break;
        case WStype_PONG:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Pong!", type);
            break;
        case WStype_ERROR:
            SysTools::addLog("wsEndpoints::webServerEvent[%u] Error!", type);
            break;
        default:
            SysTools::addLog("wsEndpoints::webServerEvent Unknown WStype [%d]", type);
            break;
    }
  
}

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = _wsServer.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:

      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", client_num, payload);
      _wsServer.sendTXT(client_num, "{Thank You}");

      // Toggle LED
/*
      if ( strcmp((char *)payload, "toggleLED") == 0 ) {
        led_state = led_state ? 0 : 1;
        Serial.printf("Toggling LED to %u\n", led_state);
        digitalWrite(led_pin, led_state);

      // Report the state of the LED
      } else if ( strcmp((char *)payload, "getLEDState") == 0 ) {
        sprintf(msg_buf, "%d", led_state);
        Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
        _wsServer.sendTXT(client_num, msg_buf);

      // Message not recognized
      } else {
        Serial.println("[%u] Message not recognized");
      }
*/
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

//////////////////////////////////////////////////////////////
bool isNetworkConnected() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::isNetworkConnected");

    return _isNetworkConnected;
}

//////////////////////////////////////////////////////////////
void acceptHttpClient() {
//////////////////////////////////////////////////////////////
    if(!_isApEnabled) return;
/*
    WiFiClient client = _httpServer.available();   // listen for incoming clients
    
    
    if(client) {                             // if you get a client,
        Serial.println("New Client.");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
    
        while(client.connected()) {            // loop while the client's connected
            if(client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
                        client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    } else {    // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /H")) {
                    digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
                }
                if (currentLine.endsWith("GET /L")) {
                    digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
                }
            }
        }
        
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
*/
}

//////////////////////////////////////////////////////////////
void refresh() {
//////////////////////////////////////////////////////////////
    //if(_isServerEnabled == true) _wsServerServer->loop();
    //if(_isClientEnabled == true) _wsServerClient->loop();
  //SysTools::addLog("wsEndpoints::createAP");
    _wsServer.loop();
    //acceptHttpClient();
}
/*  
//////////////////////////////////////////////////////////////
bool connectEndpoint(char* pConnectInfo[], char* pEndpointInfo[], void (*pOnClientPkg)(DynamicJsonDocument& pPkg)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::connectEndpoint");

    _endpointInfo = pEndpointInfo;
    //connectAP(pConnectInfo);

    //Connection to Endpoint
    _onClientPkg = pOnClientPkg;
    _wsServerClient = new WebSocketsClient();

    // try every 5000 again if connection has failed
    _wsServerClient->setReconnectInterval(5000);

    // use HTTP Basic Authorization this is optional remove if not needed
    //_wsServerClient->setAuthorization("user", "Password");

    // event handler
    _wsServerClient->onEvent(webClientEvent);
    
    _wsServerClient->begin(pEndpointInfo[CONNECT_ADDRESS], atoi(pEndpointInfo[CONNECT_PORT]), pEndpointInfo[CONNECT_PATH]);

    SysTools::addLog("wsEndpoints::connectEndpoint %s", "wsClientDevice is Open");

    _isClientEnabled = true;
    return true;
}

//////////////////////////////////////////////////////////////
bool awaitEndpoints(char* pConnectInfo[], void (*pOnPkgInput)(DynamicJsonDocument& pObject)) {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::awaitEndpoint");
   
    _onServerPkg = pOnPkgInput;
    _connectInfo = pConnectInfo;
    //connectAP(pConnectInfo);
    
    //Listen for Connections
    _wsServerServer = new WebSocketsServer(80);
    _wsServerServer->onEvent(webServerEvent);
    _wsServerServer->begin();
    SysTools::addLog("wsEndpoints::awaitEndpoint, Waiting for Device Connection on Port: %s...", pConnectInfo[CONNECT_PORT]);

    _isServerEnabled = true;
    return true;
}
*/

void getHomePage(AsyncWebServerRequest *request) {
  SysTools::addLog("wsEndpoints::getHomePage");

    request->send(200, "text/html", "<b>Hello, world</b><script>alert('Hello World!');</script>");
}

//////////////////////////////////////////////////////////////
bool startServers() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::startServers");
  
    _httpServer.on("/", HTTP_GET, getHomePage);   
    _httpServer.begin();
    
    _wsServer.begin();
    _wsServer.onEvent(onWebSocketEvent);
  
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

  char* ssid = "WAP2Gx-MASTERBEDROOM";
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
bool enable() {
//////////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpoints::enable");

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