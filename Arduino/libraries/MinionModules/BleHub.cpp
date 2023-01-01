#define US_KEYBOARD 1
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>

#include <MinionTools.h>
#include <BleHub.h>

namespace BleHub
{
// Change the below values if desired
#define BUTTON_PIN 2
#define MESSAGE "h"
#define DEVICE_NAME "ESP32-C3-MINI Keyboard"
/*
#define DEBUG_ON 1
#if DEBUG_ON
#define DBG_begin(...)    Serial.begin(__VA_ARGS__)
#define DBG_print(...)    Serial.print(__VA_ARGS__)
#define DBG_println(...)  Serial.println(__VA_ARGS__)
#define DBG_printf(...)   Serial.printf(__VA_ARGS__)
#else
#define DBG_begin(...)
#define DBG_print(...)
#define DBG_println(...)
#define DBG_printf(...)
#endif
*/

NimBLEHIDDevice* hid;
NimBLECharacteristic* reportList[3];
//NimBLECharacteristic* output;

// Forward declarations
void bluetoothTask(void*);
bool isConnected = false;
const char *deviceName = "ESP32 Keyboard";

// Message (report) sent when a key is pressed or released
struct InputReport {
    uint8_t modifiers;       // bitmask: CTRL = 1, SHIFT = 2, ALT = 4
    uint8_t reserved;        // must be 0
    uint8_t pressedKeys[6];  // up to six concurrenlty pressed keys
};

struct ConsumerReport {
    uint16_t pressedKey;     // two byte keycode
};

struct MouseReport {
    uint8_t pressedButton;     // one byte buttoncode
    uint8_t offsetX;           // move mouse on X-sxis
    uint8_t offsetY;           // move mouse on Y-sxis
};

// The report map describes the HID device (a keyboard in this case) and
// the messages (reports in HID terms) sent and received.
static const uint8_t REPORT_MAP[] = {
//
//ReportId = 1 (Keyboard Conrol Input)
    USAGE_PAGE(1),      0x01,       // Generic Desktop Controls
    USAGE(1),           0x06,       // Keyboard
    COLLECTION(1),      0x01,       // Application
    REPORT_ID(1),       0x01,       //   Report ID (1)
    USAGE_PAGE(1),      0x07,       //   Keyboard/Keypad
    USAGE_MINIMUM(1),   0xE0,       //   Keyboard Left Control
    USAGE_MAXIMUM(1),   0xE7,       //   Keyboard Right Control
    LOGICAL_MINIMUM(1), 0x00,       //   Each bit is either 0 or 1
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_COUNT(1),    0x08,       //   8 bits for the modifier keys
    REPORT_SIZE(1),     0x01,       
    HIDINPUT(1),        0x02,       //   Data, Var, Abs
    REPORT_COUNT(1),    0x01,       //   1 bytes (for up to 1 concurrently pressed keys)
    REPORT_SIZE(1),     0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0xE7,       //   101 keys
    USAGE_MINIMUM(1),   0x00,
    USAGE_MAXIMUM(1),   0xE7,
    HIDINPUT(1),        0x00,       //   Data, Array, Abs
    END_COLLECTION(0),              // End application collection
//
//Report Reference Id = 2 (Media Control Input)
    USAGE_PAGE(1),      0x0C,       // Usage Page (Consumer)
    USAGE(1),           0x01,       // Usage (Consumer Control)
    COLLECTION(1),      0x01,       // Collection (Application)
    REPORT_ID(1),       0x02,       //   Report ID (2)
    REPORT_SIZE(1),     0x10,       //   Report Size (16)
    REPORT_COUNT(1),    0x01,       //   Report Count (1)
    LOGICAL_MINIMUM(1), 0x01,       //   Logical Minimum (1)
    LOGICAL_MAXIMUM(2), 0xff, 0x07, //   Logical Maximum (2047)
    USAGE_MINIMUM(1),   0x01,       //   Usage Minimum (Consumer Control)
    USAGE_MAXIMUM(2),   0xff, 0x07, //   Usage Maximum (0x07FF)
    HIDINPUT(1),        0x00,       //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    END_COLLECTION(0),              // End Collection
//
//Report Reference Id = 3 (Mouse Control Input)
    USAGE_PAGE(1),      0x01,       // Usage Page (Generic Desktop Ctrls)
    USAGE(1),           0x02,       // Usage (Mouse)
    COLLECTION(1),      0x01,       // Collection (Application)
    REPORT_ID(1),       0x03,       //   Report ID (3)
    USAGE(1),           0x01,       //   Usage (Pointer)
    COLLECTION(1),      0x00,       //   Collection (Physical)
    USAGE_PAGE(1),      0x09,       //     Usage Page (Button)
    USAGE_MINIMUM(1),   0x01,       //     Usage Minimum (0x01)
    USAGE_MAXIMUM(1),   0x03,       //     Usage Maximum (0x03)
    LOGICAL_MINIMUM(1), 0x00,       //     Logical Minimum (0)
    LOGICAL_MAXIMUM(1), 0x01,       //     Logical Maximum (1)
    REPORT_COUNT(1),    0x03,       //     Report Count (3)
    REPORT_SIZE(1),     0x01,       //     Report Size (1)
    HIDINPUT(1),        0x02,       //     Input (Data,Var,Abs,No Wrap,Linear,...)
    REPORT_COUNT(1),    0x01,       //     Report Count (1)
    REPORT_SIZE(1),     0x05,       //     Report Size (5)
    HIDINPUT(1),        0x03,       //     Input (Const,Var,Abs,No Wrap,Linear,...)
    USAGE_PAGE(1),      0x01,       //     Usage Page (Generic Desktop Ctrls)
    USAGE(1),           0x30,       //     Usage (X)
    USAGE(1),           0x31,       //     Usage (Y)
    LOGICAL_MINIMUM(1), 0x81,       //     Logical Minimum (-127)
    LOGICAL_MAXIMUM(1), 0x7F,       //     Logical Maximum (127)
    REPORT_SIZE(1),     0x08,       //     Report Size (8)
    REPORT_COUNT(1),    0x02,       //     Report Count (2)
    HIDINPUT(1),        0x06,       //     Input (Data,Var,Rel,No Wrap,Linear,...)
    END_COLLECTION(0),              //   End Collection
    END_COLLECTION(0)               // End Collection
};

/*
 * Callbacks related to BLE connection
 */
class BleKeyboardCallbacks : public BLEServerCallbacks {

    void onConnect(BLEServer* server) {
        isConnected = true;
        MinionTools::addLog("%s", "BleHub::BleKeyboardCallbacks Connected");
    }

    void onDisconnect(BLEServer* server) {
        isConnected = false;
        MinionTools::addLog("%s", "BleHub::BleKeyboardCallbacks Disconnected");
    }
};

void bluetoothTask(void*) {

    // initialize the device
    NimBLEDevice::init(deviceName);
    NimBLEServer* server = NimBLEDevice::createServer();
    server->setCallbacks(new BleKeyboardCallbacks());

    // create an HID device
    hid = new NimBLEHIDDevice(server);
    reportList[0] = hid->inputReport(1); // reportID
    reportList[1] = hid->inputReport(2); // reportID
    reportList[2] = hid->inputReport(3); // reportID
    
    //output = hid->outputReport(1); // report ID
    //output->setCallbacks(new OutputCallbacks());

    // set manufacturer name
    hid->manufacturer()->setValue("Maker Community");
    // set USB vendor and product ID
    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    // information about HID device: device is not localized, device can be connected
    hid->hidInfo(0x00, 0x02);

    // Security: device requires bonding
    BLESecurity* security = new BLESecurity();
    security->setAuthenticationMode(ESP_LE_AUTH_BOND);

    // set report map
    hid->reportMap((uint8_t*)REPORT_MAP, sizeof(REPORT_MAP));
    hid->startServices();

    // set battery level to 100%
    hid->setBatteryLevel(100);

    // advertise the services
    NimBLEAdvertising* advertising = server->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hid->hidService()->getUUID());
    advertising->addServiceUUID(hid->deviceInfo()->getUUID());
    advertising->addServiceUUID(hid->batteryService()->getUUID());
    advertising->start();

    //Serial.println("BLE ready");
    delay(portMAX_DELAY);
}

bool isHubConnected() {
    return isConnected;
}

void controlDevice(int pControlCode, int pDuration=550, int pDelay=5, int pReportId=1) {
    MinionTools::addLog("BleHub::controlDevice controlCode: 0x%X, duration: %i, delay: %i, reportId: %i", pControlCode, pDuration, pDelay, pReportId);
   
    if(isConnected == false) {
        MinionTools::addLog("%s", "BleHub::controlDevice Aborted: Gateway not connected.");
        return;
    };
    
        // create input report

       /*
       //REPORT 1
        InputReport report = {
            //.modifiers = map.modifier,
            .modifiers = map.modifier,
            .reserved = map.usage
            //.pressedKeys = {
            //   map.usage,
            //    0, 0, 0, 0, 0
            //}
        };
        */
       
        //REPORT 2
        ConsumerReport report = {
            .pressedKey = pControlCode //Volume Up
            //.pressedKey = 0x46 //FireTV Back
            //.pressedKey = 0xB3 //FireTV F.F.           
        };

        /*        
        //REPORT 3
        MouseReport report = {
            .pressedButton = 0,
            .offsetX = 100,
            .offsetY = 100
        };
        */

        // send the input report
        reportList[1]->setValue((uint8_t*)&report, sizeof(report));
        reportList[1]->notify();

        delay(pDuration);
        //delay(500);

        // release all keys between two characters; otherwise two identical
        // consecutive characters are treated as just one key press
        report.pressedKey = 0x00;
        reportList[1]->setValue((uint8_t*)&report, sizeof(report));
        reportList[1]->notify();
        
        MinionTools::addLog("%s", "BLEHIDBRIDGE::controlDevice Key Pressed");

        delay(pDelay);
 }
 
void openHub(const char *pDeviceName) {
    // start Bluetooth task
    deviceName = pDeviceName;
    xTaskCreate(bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);

    MinionTools::addLog("BleHub::openHub, Hub: '%s' is Open", pDeviceName);
    //Serial.print("BleHub '%s' is Open", pDeviceName);
    //Serial.println();
}
}