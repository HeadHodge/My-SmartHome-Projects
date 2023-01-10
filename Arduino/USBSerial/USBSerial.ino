#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "USB.h"

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial2
#define USBSerial Serial
#else
USBCDC Serial0;
USBCDC USBSerial;
#endif

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        Serial0.println("USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        Serial0.println("USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        Serial0.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        Serial0.println("USB RESUMED");
        break;
      
      default:
        break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS){
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT:
        Serial0.println("CDC CONNECTED");
        break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
        Serial0.println("CDC DISCONNECTED");
        break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT:
        Serial0.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
        break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT:
        Serial0.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
        break;
      case ARDUINO_USB_CDC_RX_EVENT:
        Serial0.printf("CDC RX [%u]:", data->rx.len);
        {
            uint8_t buf[data->rx.len];
            size_t len = USBSerial.read(buf, data->rx.len);
            Serial0.write(buf, len);
        }
        Serial0.println();
        break;
       case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        Serial0.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  }
}

void setup() {
  Serial0.begin(115200);
  USBSerial.begin(115200);

  USB.onEvent(usbEventCallback);
  //USBSerial.onEvent(usbEventCallback);
  
  USB.begin();
}

void loop() {
  delay(3000);
  Serial0.println("USB UNPLUGGED,hw");
  USBSerial.println("USB UNPLUGGED,usb");

  while(Serial0.available()){
    delay(3000);
    Serial0.println("USB WHILE,hw");
    USBSerial.println("USB WHILE,usb");
    
    size_t l = Serial0.available();
    uint8_t b[l];
    l = Serial0.read(b, l);
    USBSerial.write(b, l);
  }
}
#endif /* ARDUINO_USB_MODE */
