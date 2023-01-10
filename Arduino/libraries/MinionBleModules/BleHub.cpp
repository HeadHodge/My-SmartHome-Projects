#define US_KEYBOARD 1
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>

#include <BleHub.h>

namespace BleHub
{
// Change the below values if desired
#define BUTTON_PIN 2
#define MESSAGE "h"
#define DEVICE_NAME "ESP32-C3-MINI Keyboard"

NimBLEHIDDevice* hid;
NimBLECharacteristic* reportList[3];
//NimBLECharacteristic* output;

// Forward declarations
void bluetoothTask(void*);
bool isConnected = false;
const char *deviceName = "ESP32 Keyboard";

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
    reportList[0] = hid->inputReport(1); // keyboard reportID
    reportList[1] = hid->inputReport(2); // consumer reportID
    reportList[2] = hid->inputReport(3); // mouse reportID
    
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
// Message (report) sent when a key is pressed or released

void controlDevice(MinionTools::hidKey* pOptions) {
    
MinionTools::addLog("%s", "debug 100");
delay(10);

MinionTools::addLog("BleHub::controlDevice controlCode: 0x%X, duration: %i, delay: %i, usage: %s", pOptions->keyCode, pOptions->keyDuration, pOptions->keyDelay, pOptions->keyUsage);
int reportId = -1;
uint8_t* deviceReportAddress = NULL;
int deviceReportSize = 0;

    if(isConnected == false) {
        MinionTools::addLog("%s", "BleHub::controlDevice ABORT: Gateway not connected.");
        return;
    };
    
    if(pOptions->keyUsage == "keyboard") {
       //KEYBOARD REPORT
       reportId = 0;
       struct {
            uint8_t keyModifier; // bitmask: CTRL = 1, SHIFT = 2, ALT = 4
            uint8_t keyCode;
        } report = {pOptions->keyModifier, pOptions->keyCode};
        
        deviceReportAddress = (uint8_t*)&report;
        deviceReportSize = sizeof(report);
        MinionTools::addLog("BleHub::controlDevice Send Keyboard Report, keyModifier: %i, keyCode: %i", report.keyModifier, report.keyCode);

    } else if(pOptions->keyUsage == "consumer") {
        //CONSUMER REPORT
        reportId = 1;
        struct {
            uint16_t keyCode; // two byte keycode
        } report = {pOptions->keyCode};
        
        deviceReportAddress = (uint8_t*)&report;
        deviceReportSize = sizeof(report);
        MinionTools::addLog("BleHub::controlDevice Send Consumer Report, keyCode: %i", report.keyCode);
         
    } else if(pOptions->keyUsage == "mouse") {       
        //MOUSE REPORT
        reportId = 2;
        struct mouseReport {
            uint8_t pressedButtons; // one byte buttoncode
            uint8_t xOffset;        // move mouse relative on X-sxis
            uint8_t yOffset;        // move mouse relative on Y-sxis
        } report = { 0, 100, 100 }; //{pOptions->keyModifiers, pOptions->xOffset, pOptions->yOffset}; TODO
        
        deviceReportAddress = (uint8_t*)&report;
        deviceReportSize = sizeof(report);
        MinionTools::addLog("BleHub::controlDevice Send Mouse Report, clickModifier: %i, xOffset: %i, yOffset: %i", 0, 100, 100);
        
    } else {
        MinionTools::addLog("BleHub::controlDevice ABORT: Invalid keyUsage specified '%s'", pOptions->keyUsage);
        return;
    };
        
    // send the input report
    reportList[reportId]->setValue(deviceReportAddress, deviceReportSize);
    reportList[reportId]->notify();

    delay(pOptions->keyDuration);

    // release all keys between two characters; otherwise two identical
    // consecutive characters are treated as just one key press
    memset(deviceReportAddress, 0, deviceReportSize);

    reportList[reportId]->setValue(deviceReportAddress, deviceReportSize);
    reportList[reportId]->notify();

    delay(pOptions->keyDelay);
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