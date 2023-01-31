#define US_KEYBOARD 1
#include <Arduino.h>
#include <USB.h>

#include <minionTools.h>
#include <usbCom.h>

namespace UsbCom
{
bool connectedFlg = false;

bool isConnected() {
    return connectedFlg;
}

void controlDevice(const char* pString) {
    
    if(Serial.availableForWrite() == 0) return;
    Serial.print(pString);
}    
 
void refresh() {
  //MinionTools::addLog("%s", "UsbCom::refresh");
    if (Serial.available() == false) return;
    const int maxMessage = 512;
    char message[maxMessage];
    
    memset (message, 0, maxMessage);
    Serial.readBytesUntil('\0', message, maxMessage - 1); //leave one safety null char in message buffer
    MinionTools::receivedCommand((const char*)&message[0]);
}
 
void open(int pBaud = 115200) {

    Serial.begin(pBaud);
    delay(10000);
    connectedFlg = true;
    
    MinionTools::addLog("%s", "UsbCom::open UsbCom Now Open");
    delay(1000);
}
}