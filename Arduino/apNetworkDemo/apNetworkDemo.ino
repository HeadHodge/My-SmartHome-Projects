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

void onEndpointPkg(char* pSource, DynamicJsonDocument* pPkgIn, void (*sendEndpointPkg)(DynamicJsonDocument* pEndpointPkg)) {
  Serial.printf("onEndpointPkg Called from: %s\n", pSource);
  char* htmlReply = "{\"homePage\": \"<b>Hello, world</b><script>alert('Hello World!');</script>\"}";
  DynamicJsonDocument* optionsObj; // = new DynamicJsonDocument(128);

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
  //SysTools::displayConnection();
  return;
}

void loop() {
  WsEndpoints::refresh();
  return;
}
