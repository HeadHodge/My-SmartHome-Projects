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

void setup() {
  Serial.begin(115200);
  delay(4000);
  Serial.println();
  Serial.println("Configuring access point...");

  SysTools::displayHome();
  WsEndpoints::enable();
  //SysTools::displayConnection();
  return;
}

void loop() {
  WsEndpoints::refresh();
  return;
}
