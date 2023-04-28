/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <SysTools.h>
#include <WsEndpoints.h>
#include <ArduinoJson.h>

//Hassio Endpoint
char* _remoteEndpointInfo[] = {
    "WAP2G-MASTERBEDROOM",
    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU",
    "192.168.0.102",
    "8123",
    "/api/websocket"
};

void onEndpointPkg(char* pSource, DynamicJsonDocument* pPkgIn, bool (*sendEndpointPkg)(DynamicJsonDocument* pEndpointPkg)) {
  Serial.printf("onEndpointPkg Called from: %s\n", pSource);
  char* htmlReply = "{\"homePage\": \"<b>Hello, world</b><script>alert('Hello World!');</script>\"}";
  DynamicJsonDocument* optionsObj;

    optionsObj = SysTools::createPkg(htmlReply);
    
    if(optionsObj == nullptr) {
        SysTools::addLog("ABORT: createPkg failed");
        return;
    };
 
     sendEndpointPkg(optionsObj);
     free(optionsObj);
};

void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println();
  Serial.println("Configuring access point...");

  SysTools::displayHome();
  WsEndpoints::enable(&onEndpointPkg);
  WsEndpoints::connectWsClientEndpoint(
      "192.168.0.102",  //IpAddress
      8123,             //PortNumber
      "/api/websocket", //Path
      "admin",          //LoginId
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU"      
  );
 
  //SysTools::displayConnection();
  return;
}

void loop() {
  WsEndpoints::refresh();
  return;
}
