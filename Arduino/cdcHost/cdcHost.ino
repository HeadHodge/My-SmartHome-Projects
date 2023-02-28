#include <Arduino.h>
#include "usb/usb_host.h"

//#include "UsbhostHost.hpp"
//#include "UsbhostAcm.hpp"

USBhost host;           // host is required to detect any device, before USB class is initialized
USBacmDevice *device;   // when USB class is detected from descriptor
bool _isOut = true, _isIn = true;

void acm_events(int event, void *data, size_t len)
{
    switch (event)
    {
    case CDC_CTRL_SET_CONTROL_LINE_STATE:
        Serial.println("cdcHost::acm_events, CDC_CTRL_SET_CONTROL_LINE_STATE");
        break;

     case CDC_DATA_OUT: {
        Serial.printf("cdcHost::acm_events: CDC_DATA_OUT, dataLen: %i \n", (int)len);
        char* sout = (char *)data;

        for(int i=0; i < (int)len; ++i) {
          Serial.printf("0%X ", sout[i]);
        }

        Serial.println("");
        
        _isOut = true;
 /*       
        while(!device || !device->isConnected()) {
            Serial.printf("%c", '.');
            delay(1);
        };
        
        uint8_t irCmd[] = {1,2,3,0};
        device->OUTDATA((uint8_t *)&irCmd[0], 3);

        return;
 */       
        //Serial.printf("cdcHost::acm_events, CDC_DATA_OUT Event: %X, %X, %X, %X, %X, %X, %X, %X \n", sout[0], sout[1], sout[2], sout[3], sout[4], sout[5], sout[6], sout[7]);
        //while(!device || !device->isConnected()) {
        //  Serial.printf("%c", '.');
        //  delay(10);
        //};

        //device->INDATA();
        break;
    }
    
    case CDC_DATA_IN: {
        //device->INDATA();
        char* sin = (char *)data;
        sin[len] = 0;
        Serial.printf("cdcHost::acm_events, CDC_DATA_IN, dataLen: %i, dataIn: %s \n", (int)len, sin);
                        
        _isIn = true;
        device->INDATA();
        return;
 /*        
        if(memcmp(sin, "S01", (int)len) == 0) {
          //device->OUTDATA((uint8_t *)"\x03\x00", 1);
          //delay(100);
          
          uint8_t irCmd1[] = {3,1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74};
          device->OUTDATA((uint8_t *)&irCmd1[0], sizeof(irCmd1));

          for(int i=0; i < 1; ++i) {
              while(!device || !device->isConnected()) {
                  Serial.printf("%c", '.');
                  delay(1);
              };

              device->OUTDATA((uint8_t *)&irCmd1[i], 1);
              //delay(500);     
           }
        while(!device || !device->isConnected()) {
          Serial.printf("%c", '.');
          delay(10);
        };
          uint8_t irCmd2[] = {0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26};
          device->OUTDATA((uint8_t *)&irCmd2[0], sizeof(irCmd2));
         
        while(!device || !device->isConnected()) {
          Serial.printf("%c", '.');
          delay(10);
        };
          uint8_t irCmd3[] = {0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74};
          device->OUTDATA((uint8_t *)&irCmd3[0], sizeof(irCmd3));
        
        while(!device || !device->isConnected()) {
          Serial.printf("%c", '.');
          delay(10);
        };
          uint8_t irCmd4[] = {0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,0,26,17,122,1,165,0,102,0,26,17,122,1,165,0,102,0,26,255,255};
          device->OUTDATA((uint8_t *)&irCmd4[0], sizeof(irCmd4));
        
          //Serial.printf("cdcHost::acm_events, Send irCmd, cmdLen: %i \n", sizeof(irCmd));
          //device->OUTDATA((uint8_t *)&irCmd[0], sizeof(irCmd));
        };
 */         
        break;
    }
    case CDC_CTRL_SET_LINE_CODING:
        Serial.println("cdcHost::acm_events, CDC_CTRL_SET_LINE_CODING");
        log_i("CDC_CTRL_SET_LINE_CODING");
        break;
        
    default: {
        Serial.printf("cdcHost::acm_events, ABORT: Received Unsupported Event '%i'", event);
        break;
    }};
}

void client_event_callback(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    Serial.println("cdcHost::client_event_callback");
    
    if (event_msg->event == USB_HOST_CLIENT_EVENT_NEW_DEV)
    {
        Serial.println("cdcHost::client_event_callback: USB_HOST_CLIENT_EVENT_NEW_DEV");
              
        int offset = 0;
        host.open(event_msg);
        usb_device_info_t info = host.getDeviceInfo();
        const usb_device_desc_t *dev_desc = host.getDeviceDescriptor();
        const usb_config_desc_t *config_desc = host.getConfigurationDescriptor();
        Serial.printf("cdcHost::acm_events, device speed: %s, configNum: %i, interfaceNum: %i, deviceAddress: %d, max ep_ctrl size: %d, config: %d \n", info.speed ? "USB_SPEED_FULL" : "USB_SPEED_LOW", dev_desc->bNumConfigurations, config_desc->bNumInterfaces, info.dev_addr, info.bMaxPacketSize0, info.bConfigurationValue);
        
        for (int i = 0; i < dev_desc->bNumConfigurations; i++) {
            Serial.printf("cdcHost::client_event_callback, device interfaceNum: %i \n", config_desc->bNumInterfaces);
           
            for (int n = 0; n < config_desc->bNumInterfaces; n++) {
                const usb_intf_desc_t *intf = usb_parse_interface_descriptor(config_desc, n, 0, &offset);
                Serial.printf("cdcHost::acm_events, deviceInterfaces: %i, deviceClass: 0X%X \n", config_desc->bNumInterfaces, intf->bInterfaceClass);
                
                if (intf->bInterfaceClass == 0x0a) { // CDC ACM
                    Serial.println("cdcHost::client_event_callback, CDC/ACM device connected. Adding driver.");
                    device = new USBacmDevice(config_desc, &host);
                    n = config_desc->bNumInterfaces;
                    
                    if (device) {
                        device->init();
                        device->onEvent(acm_events);
                        device->setControlLine(1, 1);
                        //device->INDATA();
                        Serial.println("cdcHost::client_event_callback, DC/ACM device added.");
                    }
                }
            }
        }
    }
    else
    {
        Serial.printf("cdcHost::client_event_callback, ABORT: Unsupported Event '%i' \n", event_msg->event);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    delay(5000);
    Serial.println("cdcHost::setup Start");

    //Enable and Power Esp32-s3-otg-usb board
    gpio_set_direction((gpio_num_t)12, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)13, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)17, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)18, GPIO_MODE_OUTPUT);

    gpio_set_level((gpio_num_t)12, 1); // DEV_VBUS_EN: High level to enable DEV_VBUS power supply.
    gpio_set_level((gpio_num_t)13, 0); // BOOST_EN: High level to enable Boost boost circuit, when using battery.
    gpio_set_level((gpio_num_t)17, 1); // IDEV_LIMIT_EN: Enable current limiting IC, high level enable.
    gpio_set_level((gpio_num_t)18, 1); // USB_SEL: Used to switch the USB interface. When high level, the USB_DEV interface is used. When low level, the USB_HOST interface is used.
    
    host.registerClientCb(client_event_callback);
    Serial.printf("cdcHosts::setup, host.init: %i \n", host.init(true));

//uint8_t reset [] = {0, 0, 0, 0, 0, 115, 38, 37};
    //device->OUTDATA((uint8_t*)&reset[0], 8);
    //device->OUTDATA((uint8_t *)"\x00\x00\x00\x00\x00\x73\x26\x25\x00", 8);
    Serial.println("cdcHost::setup Complete");
    
    while(!device || !device->isConnected()) {
      Serial.printf("%c", '.');
      delay(1000);
    };
    
    Serial.println("cdcHosts::setup, Usb Connected");
    
    uint8_t initCmd[] = {0,0,0,0,0,115,38,37};
    uint8_t xmitMode[] = {3};
    uint8_t powerCmd[] = {1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,0,26,17,122,1,165,0,102,0,26,17,122,1,165,0,102,0,26,255,255};
    uint8_t powerCmd[] = {0x01,0xA4,0x00,0xCE,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x17,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00,0x1A,0x00,0x4A,0x00 1A,0x00,0x4A,0x00,0x1A,0x07,0x65,0x01,0xA5,0x00,0x66,0x00,0x1A,0x11,0x7A,255,255}; 
    device->INDATA();
    delay(1000);

    _isIn = false;
    _isOut = false;
    device->OUTDATA((uint8_t *)&initCmd[0],sizeof(initCmd));
    while(!_isIn || !_isOut) delay(1);
    
    _isOut = false;
    device->OUTDATA((uint8_t *)&xmitMode[0],sizeof(xmitMode));
    while(!_isOut) delay(1);
    
    _isOut = false;
    device->OUTDATA((uint8_t *)&powerCmd[0], sizeof(powerCmd));
    while(!_isOut) delay(1);

    delay(2000);
    
    _isOut = false;
    device->OUTDATA((uint8_t *)&xmitMode[0], sizeof(xmitMode));
    while(!_isOut) delay(1);
    
    _isOut = false;
    device->OUTDATA((uint8_t *)&powerCmd[0], sizeof(powerCmd));
    while(!_isOut) delay(1);
}

void loop()
{
    //Serial.println("cdcHost::loop");

    if (device && device->isConnected())
    {
        //Serial.println("cdcHost::loop Device Connected, send test message.");
        //device->OUTDATA((uint8_t *)"test\n", 5);
    }
    delay(5000);
}
