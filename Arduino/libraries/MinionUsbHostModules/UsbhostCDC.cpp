#define US_KEYBOARD 1
#include <Arduino.h>
#include <usb/usb_host.h>

#include <SysTools.h>
#include <UsbhostCDC.hpp>

namespace UsbhostCDC {
#define ACTION_NONE             0x00
#define ACTION_CONNECTDEV       0x01
#define ACTION_RESETDEV         0x02
#define ACTION_DISCONNECTDEV    0x03
#define ACTION_ENABLESEND       0x04
#define ACTION_SENDCMD          0x05

#define EVENT_NONE              0
#define EVENT_INSERTED          1
#define EVENT_REMOVED           2
#define EVENT_CONNECTED         3
#define EVENT_DISCONNECTED      4
#define EVENT_ENABLED           5
#define EVENT_STARTED           6
#define EVENT_STOPPED           7
#define EVENT_OPENING           8
#define EVENT_OPENED            9
#define EVENT_SENDING           10
#define EVENT_SENT              11
#define EVENT_RESENT            12
#define EVENT_RECEIVING         13
#define EVENT_RECEIVED          14
#define EVENT_CONFIRMED         15

#define TRANSFERTYPE_CTLIN      0x01
#define TRANSFERTYPE_CTLOUT     0x02
#define TRANSFERTYPE_DATAIN     0x03
#define TRANSFERTYPE_DATAOUT    0x04
//#define USB_EP_DESC_GET_MPS(desc_ptr) ((desc_ptr)->wMaxPacketSize & 0x7FF)

void transferData(uint8_t pTranferType, uint8_t* pData, uint8_t pSize);

struct usbClientInfo {
    uint8_t  action;    
    uint32_t event;
    uint32_t step;    
    uint32_t flags;
    
    uint8_t ctl_class;
    uint8_t ctl_addrIn;
    uint8_t ctl_addrOut;
    uint8_t data_class;
    uint8_t data_addrIn;
    uint8_t data_addrOut;
    
    usb_host_client_handle_t client_hdl;
    usb_device_handle_t dev_hdl;
    usb_device_info_t dev_info;
    
    uint8_t dev_addr;
    const usb_device_desc_t *dev_desc;
    const usb_config_desc_t *dev_config;
    
    uint16_t ctl_maxIn; 
    usb_transfer_t* ctl_transferIn;
    uint16_t data_maxIn; 
    usb_transfer_t* data_transferIn;
    uint16_t data_maxOut;
    usb_transfer_t* data_transferOut;
};

struct usbClientInfo _clientInfo = {0};
void (*_statusCallback)(bool) = nullptr;
esp_err_t _libErr;
bool _isConnected = false;
bool _isReady = false;

/////////////////////////////////////////////////////////////////////////////////////////////////
void getStringDescriptorData(const usb_str_desc_t *pStringDesc, uint8_t* pDataBuff, uint8_t pMaxSize) {
    
    if (pStringDesc == NULL || pDataBuff == nullptr) return;
    uint8_t dataSize = pStringDesc->bLength/2;
    if(dataSize > pMaxSize) dataSize = pMaxSize;
    memset(pDataBuff, 0, pMaxSize);

    uint8_t index = 0;
    
    for (int i = 0; i < dataSize; i++) {
        /*
        USB String descriptors of UTF-16.
        Right now We just skip any character larger than 0xFF to stay in BMP Basic Latin and Latin-1 Supplement range.
        */
        
        if (pStringDesc->wData[i] > 0xFF) continue; //skip non utf8 chars
  
        pDataBuff[index] = pStringDesc->wData[i];
        ++index;
        
        //printf("%c", (char)pStringDesc->wData[i]);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool eventWatch(int pEvent, long pMaxMillisecs = 1000) {
  SysTools::addLog("%s, watchEvent: %i", "UsbhostCDC::eventWatch", pEvent);
  long startMillisecs = millis();
  
    while(_clientInfo.event != pEvent) {
        
        if((millis() - startMillisecs) > pMaxMillisecs) {
            SysTools::addLog("UsbhostCDC::eventWatch ***ABORT***, eventWatch exceeded maxMillisecs: %l", pMaxMillisecs);
            return false;
        }
        
        usb_host_client_handle_events(_clientInfo.client_hdl, 1);
        
        if (usb_host_lib_handle_events(1, &_clientInfo.flags) != ESP_OK) continue;
        SysTools::addLog("UsbHostTest usb_host_lib_handle_events, flags: %i", _clientInfo.flags);
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void transferCtlIn_cb(usb_transfer_t *pTransfer)
{
    SysTools::addLog("***clientTask: %s, bytes: %i, data: %s", "transferCtlIn_cb::EVENT_RECEIVED ", pTransfer->actual_num_bytes, pTransfer->data_buffer);
    transferData(TRANSFERTYPE_CTLIN, nullptr, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void transferDataIn_cb(usb_transfer_t *pTransfer)
{
    SysTools::addLog("***clientTask: %s, bytes: %i, data: %s", "transferDataIn_cb::EVENT_RECEIVED ", pTransfer->actual_num_bytes, pTransfer->data_buffer);

    transferData(TRANSFERTYPE_DATAIN, nullptr, 0); //Listen for input

    _clientInfo.event = EVENT_RECEIVED;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void transferDataOut_cb(usb_transfer_t *pTransfer)
{
    SysTools::addLog("***transferDataOut_cb::EVENT_SENT, sentBytes: %i", pTransfer->actual_num_bytes);
    
    _clientInfo.event = EVENT_SENT;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void allocBuffers() {
    SysTools::addLog("%s", "UsbHostCDC::allocBuffers"); 

    // allocate dataIn buffer
    _clientInfo.data_maxIn = 64;
    _libErr = usb_host_transfer_alloc(_clientInfo.data_maxIn, 0, &_clientInfo.data_transferIn);
    SysTools::addLog("UsbHostCDC::allocBuffers, dataIn usb_host_transfer_alloc: %i", _libErr);  

    _clientInfo.data_transferIn->num_bytes = _clientInfo.data_maxIn;
    _clientInfo.data_transferIn->bEndpointAddress = _clientInfo.data_addrIn;
    _clientInfo.data_transferIn->callback = transferDataIn_cb;
    _clientInfo.data_transferIn->device_handle = _clientInfo.dev_hdl;
    _clientInfo.data_transferIn->context = (void *)&_clientInfo;       

    // allocate dataOut buffer
    _clientInfo.data_maxOut = 200;            
    _libErr = usb_host_transfer_alloc(_clientInfo.data_maxOut, 0, &_clientInfo.data_transferOut);
    SysTools::addLog("UsbHostCDC::allocBuffers, dataOut usb_host_transfer_alloc: %i", _libErr);  
        
    _clientInfo.data_transferOut->num_bytes = _clientInfo.data_maxOut;
    _clientInfo.data_transferOut->device_handle = _clientInfo.dev_hdl;
    _clientInfo.data_transferOut->bEndpointAddress = _clientInfo.data_addrOut;
    _clientInfo.data_transferOut->callback = transferDataOut_cb;
    _clientInfo.data_transferOut->context = (void *)&_clientInfo;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool connectDevice(void* pClientInfo) {
  struct usbClientInfo *clientInfo = (struct usbClientInfo *)pClientInfo;
  SysTools::addLog("UsbhostCDC::connectDevice, devAddr: %i", clientInfo->dev_addr);
    /*
    open device
        claim interface0 class 0x02
            inaddr = 0x81
        claim interface1 class 0x0a
            inaddr = 0x82
            outaddr= 0x02
    */
    
    //Open the device and claim interfaces
    _libErr = usb_host_device_open(clientInfo->client_hdl, clientInfo->dev_addr, &clientInfo->dev_hdl);
    SysTools::addLog("UsbhostCDC::connectDevice, usb_host_device_open: %i", _libErr);  

    //Get device descriptors
    _libErr = usb_host_get_device_descriptor(clientInfo->dev_hdl, &clientInfo->dev_desc);
    SysTools::addLog("connectDevice, usb_host_get_device_descriptor: %i", _libErr);  
            
    _libErr = usb_host_device_info(clientInfo->dev_hdl, &clientInfo->dev_info);
    SysTools::addLog("UsbhostCDC::connectDevice, usb_host_device_info: %i", _libErr);

    _libErr = usb_host_get_active_config_descriptor(clientInfo->dev_hdl, &clientInfo->dev_config);
    SysTools::addLog("connectDevice, usb_host_get_active_config_descriptor: %i \n", _libErr);  

    uint8_t strBuff[32] = {0};
    
    SysTools::addLog("UsbhostCDC::connectDevice deviceInfo, bDeviceClass %i, bDeviceSubClass %i, idVendor %lu, idProduct %lu", clientInfo->dev_desc->bDeviceClass, clientInfo->dev_desc->bDeviceSubClass, clientInfo->dev_desc->idVendor, clientInfo->dev_desc->idProduct);
    //SysTools::addLog("UsbhostCDC::connectDevice deviceInfo, bDeviceClass %i, bDeviceSubClass %i, idVendor %l, idProduct %l, iManufacturer %s, iProduct %s, iSerialNumber %s", clientInfo->dev_desc->bDeviceClass, clientInfo->dev_desc->bDeviceSubClass, clientInfo->dev_desc->idVendor, clientInfo->dev_desc->idProduct, clientInfo->dev_desc->iManufacturer, clientInfo->dev_desc->iProduct, clientInfo->dev_desc->iSerialNumber);

    getStringDescriptorData(clientInfo->dev_info.str_desc_manufacturer, &strBuff[0], sizeof(strBuff));
    SysTools::addLog("UsbhostCDC::connectDevice,str_desc_manufacturer: %s", strBuff);
    
    getStringDescriptorData(clientInfo->dev_info.str_desc_product, &strBuff[0], sizeof(strBuff));
    SysTools::addLog("UsbhostCDC::connectDevice,str_desc_product: %s", strBuff);
    
    getStringDescriptorData(clientInfo->dev_info.str_desc_serial_num, &strBuff[0], sizeof(strBuff));
    SysTools::addLog("UsbhostCDC::connectDevice,str_desc_serial_num: %s \n", strBuff); 
            
    //get endpoint addresses
    const usb_intf_desc_t *dev_intf;
    const usb_ep_desc_t *dev_ep;
    
    //default ep addresses
    clientInfo->ctl_class = -1;
    clientInfo->ctl_addrIn = -1;
    clientInfo->ctl_addrOut = -1;
    clientInfo->data_class = -1;
    clientInfo->data_addrIn = -1;
    clientInfo->data_addrOut = -1;
    
    for(int interface = 0; interface < _clientInfo.dev_config->bNumInterfaces; interface++) {
        dev_intf = usb_parse_interface_descriptor(_clientInfo.dev_config, interface, 0, nullptr);
        SysTools::addLog("connectDevice getAddresses, interfaceNum: %i, interfaceClass: 0x%X, interfaceSubclass: 0x%X", interface, dev_intf->bInterfaceClass, dev_intf->bInterfaceSubClass);
        
        if(interface == 0) clientInfo->ctl_class = dev_intf->bInterfaceClass;
        if(interface == 1) clientInfo->data_class = dev_intf->bInterfaceClass;
        
        for(int ep = 0; ep < dev_intf->bNumEndpoints; ep++) {
            int offset = 0;
            dev_ep = usb_parse_endpoint_descriptor_by_index(dev_intf, ep, _clientInfo.dev_config->wTotalLength, &offset);                
            SysTools::addLog("connectDevice getAddresses, interface: %i, endpoint: %i, MPS: %i, address: 0x%X - %s", interface, ep, dev_ep->bEndpointAddress, dev_ep->wMaxPacketSize & 0x7FF, (dev_ep->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) ? "IN" : "OUT");               

            if(dev_intf->bInterfaceClass == 0x02 && (dev_ep->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) != 0)
                clientInfo->ctl_addrIn = dev_ep->bEndpointAddress;           
            if(dev_intf->bInterfaceClass == 0x02 && (dev_ep->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) == 0)
                clientInfo->ctl_addrOut = dev_ep->bEndpointAddress;           
            if(dev_intf->bInterfaceClass == 0x0A && (dev_ep->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) != 0)
                clientInfo->data_addrIn = dev_ep->bEndpointAddress;           
            if(dev_intf->bInterfaceClass == 0x0A && (dev_ep->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) == 0)
                clientInfo->data_addrOut = dev_ep->bEndpointAddress;           
        }
    }

    SysTools::addLog("connectDevice, ctlAddresses: 0x%X, in: 0x%X, out: 0x%X, dataAddresses: 0x%X, in: 0x%X, out: 0x%X", clientInfo->ctl_class, clientInfo->ctl_addrIn, clientInfo->ctl_addrOut, clientInfo->data_class, clientInfo->data_addrIn, clientInfo->data_addrOut); 
    
    _libErr = usb_host_interface_claim(clientInfo->client_hdl, clientInfo->dev_hdl, 0, 0);
    SysTools::addLog("connectDevice, usb_host_interface_claim 0: %i", _libErr);  
        
    _libErr = usb_host_interface_claim(clientInfo->client_hdl, clientInfo->dev_hdl, 1, 0);
    SysTools::addLog("connectDevice, usb_host_interface_claim 1: %i", _libErr);  
    
    allocBuffers();

    // Start listening for dainput
    transferData(TRANSFERTYPE_DATAIN, nullptr, 0);
    SysTools::addLog("connectDevice, %s \n", "listening for dataIn");  
    
    clientInfo->event = EVENT_CONNECTED;
    
    uint8_t resetCmd[] = {0,0,0,0,0,115};
    uint8_t openCmd[] = {37,3};
    
    transferData(TRANSFERTYPE_DATAOUT, &resetCmd[0], sizeof(resetCmd)); 
    if(eventWatch(EVENT_RECEIVED) == false) return false;
     
    transferData(TRANSFERTYPE_DATAOUT, &openCmd[0], sizeof(openCmd));  
    if(eventWatch(EVENT_SENT) == false) return false;
       
    SysTools::addLog("connectDevice, Status: %s \n", "OPENED");      
    _isReady = true;
    if(_statusCallback != nullptr) _statusCallback(_isReady);
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void transferData(uint8_t pTranferType, uint8_t* pData, uint8_t pSize) {
   
    if(pTranferType == TRANSFERTYPE_DATAIN) {
        SysTools::addLog("%s, packetSize: %i", "UsbHostCDC::transferData, Type: dataIn", _clientInfo.data_maxIn);
        memset(_clientInfo.data_transferIn->data_buffer, 0, _clientInfo.data_maxIn);

        usb_host_transfer_submit(_clientInfo.data_transferIn);
        SysTools::addLog("%s", "UsbHostCDC::transferData EVENT_SENDING dataIn");
        
    } else if(pTranferType == TRANSFERTYPE_DATAOUT) {    
        SysTools::addLog("%s, packetSize: %i", "UsbHostCDC::transferData, Type: dataOut", pSize);
        
        if(pSize > _clientInfo.data_maxOut) pSize = _clientInfo.data_maxOut;
        _clientInfo.data_transferOut->num_bytes = pSize;
        memset(_clientInfo.data_transferOut->data_buffer, 0, _clientInfo.data_maxOut);
        memcpy(_clientInfo.data_transferOut->data_buffer, pData, pSize);
    
        usb_host_transfer_submit(_clientInfo.data_transferOut);
        _clientInfo.event = EVENT_SENDING;
        SysTools::addLog("%s", "UsbHostCDC::transferData EVENT_SENDING dataOut");
        
    } else if(pTranferType == TRANSFERTYPE_CTLIN) {
        SysTools::addLog("%s, packetSize: %i", "UsbHostCDC::transferData, Type: ctlIn", _clientInfo.ctl_maxIn);
        memset(_clientInfo.ctl_transferIn->data_buffer, 0, _clientInfo.ctl_maxIn);

        usb_host_transfer_submit(_clientInfo.ctl_transferIn);
        SysTools::addLog("%s", "UsbHostCDC::transferData Transferring ctlIn");
    }    
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *pClientInfo) {
  SysTools::addLog("%s, event: %i \n", "clientTask client_event_cb", event_msg->event);
  struct usbClientInfo* clientInfo = (struct usbClientInfo *)pClientInfo;
   
    //This is function is called from within usb_host_client_handle_events(). Don't block and try to keep it short
   
    switch (event_msg->event) {
        case USB_HOST_CLIENT_EVENT_NEW_DEV: {
            SysTools::addLog("client_event_cb: %s", "USB_HOST_CLIENT_EVENT_NEW_DEV");

            clientInfo->dev_addr = event_msg->new_dev.address; //Store the address of the new device
    
            clientInfo->event = EVENT_INSERTED;
            connectDevice(clientInfo);
            break;
        }
        
        case USB_HOST_CLIENT_EVENT_DEV_GONE: {
            SysTools::addLog("client_event_cb: %s", "USB_HOST_CLIENT_EVENT_DEV_GONE");
            
            _libErr = usb_host_interface_release(clientInfo->client_hdl, clientInfo->dev_hdl, 0);
            SysTools::addLog("client_event_cb:disconnect, usb_host_interface_release 0: %i", _libErr);  
        
            _libErr = usb_host_interface_release(clientInfo->client_hdl, clientInfo->dev_hdl, 1);
            SysTools::addLog("client_event_cb:disconnect, usb_host_interface_release 1: %i", _libErr);  

            _libErr = usb_host_transfer_free(_clientInfo.data_transferIn);
            SysTools::addLog("client_event_cb:disconnect, dataOut usb_host_transfer_release: %i", _libErr);  

            _libErr = usb_host_transfer_free(_clientInfo.data_transferOut);
            SysTools::addLog("client_event_cb:disconnect, dataOut usb_host_transfer_release: %i", _libErr);  
    
            _libErr = usb_host_device_close(clientInfo->client_hdl, clientInfo->dev_hdl);
            SysTools::addLog("client_event_cb:disconnect, usb_host_device_close: %i", _libErr);  
            
            SysTools::addLog("%s, DEVICE_REMOVED \n", "client_event_cb:disconnect");  
            clientInfo->event = EVENT_REMOVED;
            break;
        }
        
        default:
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void refresh() {
    usb_host_client_handle_events(_clientInfo.client_hdl, 1);
    
    if (usb_host_lib_handle_events(1, &_clientInfo.flags) != ESP_OK) return;

    SysTools::addLog("UsbHostTest usb_host_lib_handle_events, flags: %i", _clientInfo.flags);
    
    if(_clientInfo.flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)
        SysTools::addLog("%s", "USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS");
    
    if(_clientInfo.flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
        SysTools::addLog("%s", "USB_HOST_LIB_EVENT_FLAGS_ALL_FREE");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool isReady() {
    return _isReady;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool isConnected() {
    return _isConnected;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
bool controlDevice(uint8_t* pDataBuffer, uint8_t pSize) {
  SysTools::addLog("%s, packetSize: %i", "UsbhostCDC::controlDevice", pSize);
    
    if(_isReady == false) {
        SysTools::addLog("%s", "UsbhostCDC::controlDevice ABORT!, Device Not Ready");
        return false;
    }
    
    uint8_t statusCmd[] = {37};
    uint8_t openCmd[] = {3};
  
    transferData(TRANSFERTYPE_DATAOUT, pDataBuffer, pSize);   
    if(eventWatch(EVENT_SENT) == false) return false;
     
    transferData(TRANSFERTYPE_DATAOUT, &statusCmd[0], sizeof(statusCmd));  
    if(eventWatch(EVENT_RECEIVED) == false) return false;
     
    transferData(TRANSFERTYPE_DATAOUT, &openCmd[0], sizeof(openCmd));  
    if(eventWatch(EVENT_SENT) == false) return false;
    
    SysTools::addLog("%s", "ALL DONE");
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void open(void (*pCallback)(bool)) {
    SysTools::addLog("%s", "UsbHostTest: open");

    _statusCallback = pCallback;
    
    //Enable and PowerUp Esp32-s3-otg-usb boardclientTask
    gpio_set_direction((gpio_num_t)12, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)13, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)17, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)18, GPIO_MODE_OUTPUT);

    gpio_set_level((gpio_num_t)12, 1); // DEV_VBUS_EN: High level to enable DEV_VBUS power supply.
    gpio_set_level((gpio_num_t)13, 0); // BOOST_EN: High level to enable Boost boost circuit, when using battery.
    gpio_set_level((gpio_num_t)17, 1); // IDEV_LIMIT_EN: Enable current limiting IC, high level enable.
    gpio_set_level((gpio_num_t)18, 1); // USB_SEL: Used to switch the USB interface. When high level, the USB_DEV interface is used. When low level, the USB_HOST interface is used.
    
    //Install Usb-Host Library
    const usb_host_config_t hostConfig = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };

    _libErr = usb_host_install(&hostConfig);
    SysTools::addLog("UsbHostTest, usb_host_install status: %d", _libErr);
    
    //Register New Client
    const usb_host_client_config_t clientConfig = {
        .is_synchronous = false,
        .max_num_event_msg = 5,
        .async = {
            .client_event_callback = client_event_cb,
            .callback_arg = &_clientInfo,
        }
    };

    _libErr = usb_host_client_register(&clientConfig, &_clientInfo.client_hdl);
    SysTools::addLog("UsbHostTest, usb_host_client_register: %d", _libErr);
    
    SysTools::addLog("%s \n", "UsbHostTest status: Opened \n"); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////
} //namespace UsbhostCDC
