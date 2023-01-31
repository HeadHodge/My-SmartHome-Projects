/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#define US_KEYBOARD 1
#include <Arduino.h>
#include <ArduinoJson.h>  // Install from IDE Library manager
#include "USB.h"

#include <BleHidBridge.h>
#include <UsbHidBridge.h> esp32-c3 doesnt support USB
#include <WsJsonBridge.h>
#include <WsHassioBridge.h>
#include <UsbSerialBridge.h>
#include <MinionTools.h>

USBCDC USBSerial;
const char* ssid     = "WAP-IOT";
const char* password = "Pin#92109";
const int maxMessage = 512;

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
/*    
    if(event_base != ARDUINO_USB_CDC_EVENTS) return;
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;

    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT:
        HWSerial.println("CDC CONNECTED");
        break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
        HWSerial.println("CDC DISCONNECTED");
        break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT:
        HWSerial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
        break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT:
        HWSerial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
        break;
      case ARDUINO_USB_CDC_RX_EVENT:
        HWSerial.printf("CDC RX [%u]:", data->rx.len);
        {
            uint8_t buf[data->rx.len];
            size_t len = USBSerial.read(buf, data->rx.len);
            HWSerial.write(buf, len);
        }
        HWSerial.println();
        break;
       case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        HWSerial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  */
}

char* receivedKey(DynamicJsonDocument& pKeyObj){
  MinionTools::addLog("receivedKey, keyWord: %s, keyModifier: %i, source: %s, location: %s", (const char *)pKeyObj["required"]["keyWord"], (int)pKeyObj["options"]["keyModifier"], (const char *)pKeyObj["source"], (const char *)pKeyObj["location"]);
  const char *keyWord = pKeyObj["required"]["keyWord"];
  int targetDevices = 0;
 
//Bit Flags keyModifier High Word selects Bridge//
//bit0 0 Default
//bit1 1 BleHubBridge
//bit2 2 UsbHubBridge
//bit4 4 WsHubBridge
//bit8 8 WsDeviceBridge

    if(pKeyObj["optional"]["targetDevices"] != nullptr) targetDevices = (int)pKeyObj["optional"]["targetDevices"];
    MinionTools::addLog("receivedKey, targets: %i, useBleHub: %i, useUsbHub: %i, useWsHub: %i, WsDevice: %i", targetDevices, targetDevices & 1, targetDevices & 2, targetDevices & 4, targetDevices & 8);
  
    if((targetDevices & 1) != 0) {BleHubBridge::controlDevice(pKeyObj); return "";}
    if((targetDevices & 2) != 0) {UsbHubBridge::controlDevice(pKeyObj); return "";}
    //if((targetDevices & 4) != 0) {WsHubBridge::controlDevice(pKeyObj); return "";}
    if((targetDevices & 8) != 0) {WsDeviceBridge::controlDevice(pKeyObj); return "";}
    
    BleHubBridge::controlDevice(pKeyObj); //default bridge
    return "";
}

void setup()
{
    USBSerial.onEvent(usbEventCallback);
    USBSerial.begin(115200);
    UsbSerialBridge::open(115200);
     
    MinionTools::addLog("%s", "Open BleHubBridge");
    BleHubBridge::openBridge("wsKeyboard-S3OTG");

    MinionTools::addLog("%s", "Open WsHubBridge");
    WsHubBridge::openBridge(receivedKey);

    MinionTools::addLog("%s", "Open WsDeviceBridge");
    WsDeviceBridge::openBridge(receivedKey);

    MinionTools::addLog("%s", "Open UsbHubBridge");
    UsbHubBridge::openBridge();

} //setup

void loop()
{
    WsHubBridge::refreshBridge();
    WsDeviceBridge::refreshBridge();

    if (Serial.available() == false) USBSerial.println("Dat received on serial po");
    UsbSerialBridge::refresh();
}
