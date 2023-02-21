#define US_KEYBOARD 1
#include <Arduino.h>
#include <usb/usb_host.h>

#include <MinionTools.h>
#include <UsbhostCDC.hpp>

namespace UsbhostCDC
{
#define ACTION_NONE             0x00
#define ACTION_CONNECTDEV       0x01
#define ACTION_RESETDEV         0x02
#define ACTION_DISCONNECTDEV    0x03
#define ACTION_ENABLESEND       0x04
#define ACTION_SENDCMD          0x05

#define EVENT_NONE              0x00
#define EVENT_INSERTED          0x01
#define EVENT_REMOVED           0x02
#define EVENT_CONNECTED         0x03
#define EVENT_DISCONNECTED      0x04
#define EVENT_ENABLED           0x05
#define EVENT_STARTED           0x06
#define EVENT_STOPPED           0x07
#define EVENT_OPENING           0x08
#define EVENT_OPENED            0x09
#define EVENT_SENDING           0x0a
#define EVENT_SENT              0x0b
#define EVENT_RESENT            0x0c
#define EVENT_RECEIVING         0x0d
#define EVENT_RECEIVED          0x0e
#define EVENT_CONFIRMED         0x0f

#define TRANSFERTYPE_CTLIN      0x01
#define TRANSFERTYPE_CTLOUT     0x02
#define TRANSFERTYPE_DATAIN     0x03
#define TRANSFERTYPE_DATAOUT    0x04
//#define USB_EP_DESC_GET_MPS(desc_ptr) ((desc_ptr)->wMaxPacketSize & 0x7FF)

struct usbClientInfo {
    uint8_t  action;    
    uint32_t event;
    uint32_t state;    
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
esp_err_t _libErr;
bool _isConnected = false;

void connectDevice(void* pClientInfo) {
  MinionTools::addLog("%s", "clientTask connectDevice");
  struct usbClientInfo *clientInfo = (struct usbClientInfo *)pClientInfo;
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
    MinionTools::addLog("connectDevice, usb_host_device_open: %i", _libErr);  

    //Get device descriptors
    _libErr = usb_host_get_device_descriptor(clientInfo->dev_hdl, &clientInfo->dev_desc);
    MinionTools::addLog("connectDevice, usb_host_get_device_descriptor: %i", _libErr);  

    _libErr = usb_host_get_active_config_descriptor(clientInfo->dev_hdl, &clientInfo->dev_config);
    MinionTools::addLog("connectDevice, usb_host_get_active_config_descriptor: %i", _libErr);  
            
    _libErr = usb_host_device_info(clientInfo->dev_hdl, &clientInfo->dev_info);
    MinionTools::addLog("connectDevice, usb_host_device_info: %i", _libErr);  
            
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
        MinionTools::addLog("connectDevice getAddresses, interfaceNum: %i, interfaceClass: 0x%X, interfaceSubclass: 0x%X", interface, dev_intf->bInterfaceClass, dev_intf->bInterfaceSubClass);
        
        if(interface == 0) clientInfo->ctl_class = dev_intf->bInterfaceClass;
        if(interface == 1) clientInfo->data_class = dev_intf->bInterfaceClass;
        
        for(int ep = 0; ep < dev_intf->bNumEndpoints; ep++) {
            int offset = 0;
            dev_ep = usb_parse_endpoint_descriptor_by_index(dev_intf, ep, _clientInfo.dev_config->wTotalLength, &offset);                
            MinionTools::addLog("connectDevice getAddresses, interface: %i, endpoint: %i, MPS: %i, address: 0x%X - %s", interface, ep, dev_ep->bEndpointAddress, dev_ep->wMaxPacketSize & 0x7FF, (dev_ep->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) ? "IN" : "OUT");               

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

    MinionTools::addLog("connectDevice, ctlAddresses: 0x%X, in: 0x%X, out: 0x%X, dataAddresses: 0x%X, in: 0x%X, out: 0x%X", clientInfo->ctl_class, clientInfo->ctl_addrIn, clientInfo->ctl_addrOut, clientInfo->data_class, clientInfo->data_addrIn, clientInfo->data_addrOut); 
    
    _libErr = usb_host_interface_claim(clientInfo->client_hdl, clientInfo->dev_hdl, 0, 0);
    MinionTools::addLog("connectDevice, usb_host_interface_claim 0: %i", _libErr);  
        
    _libErr = usb_host_interface_claim(clientInfo->client_hdl, clientInfo->dev_hdl, 1, 0);
    MinionTools::addLog("connectDevice, usb_host_interface_claim 1: %i", _libErr);  

    transferData(TRANSFERTYPE_DATAIN, nullptr, 0);
    MinionTools::addLog("connectDevice, %s \n", "listening for dataIn");  

    //uint8_t ctlCmd[] = {80,6,0,1,0,0,12,0};
    //transferData(TRANSFERTYPE_CTLIN, &ctlCmd[0], sizeof(ctlCmd));
    //MinionTools::addLog("connectDevice, %s \n", "listening for ctlIn");

    
    clientInfo->event = EVENT_CONNECTED;      
    //MinionTools::addLog("connectDevice, Status: %s \n", "OPENED");      
}

void transferCtlIn_cb(usb_transfer_t *pTransfer)
{
    //MinionTools::addLog("%s", "***clientTask transferCtlIn_cb");
    MinionTools::addLog("***clientTask: %s, bytes: %i, data: %s", "transferCtlIn_cb", pTransfer->actual_num_bytes, pTransfer->data_buffer);
    //_clientInfo.event = EVENT_RECEIVED;
    transferData(TRANSFERTYPE_CTLIN, nullptr, 0);
}

void transferDataIn_cb(usb_transfer_t *pTransfer)
{
    //MinionTools::addLog("%s", "***clientTask transferDataIn_cb");
    MinionTools::addLog("***clientTask: %s, bytes: %i, data: %s", "transferDataIn_cb", pTransfer->actual_num_bytes, pTransfer->data_buffer);
    transferData(TRANSFERTYPE_DATAIN, nullptr, 0); //Listen for input
    _clientInfo.event = EVENT_RECEIVED;
}

void transferDataOut_cb(usb_transfer_t *pTransfer)
{
    MinionTools::addLog("***clientTask::transferDataOut_cb, sentBytes: %i", pTransfer->actual_num_bytes);
    _clientInfo.event = EVENT_SENT;
    //transferDataIn();
}

void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *pClientInfo)
{
    MinionTools::addLog("%s, event: %i \n", "clientTask client_event_cb", event_msg->event);
    struct usbClientInfo *clientInfo = (struct usbClientInfo *)pClientInfo;
   
    //This is function is called from within usb_host_client_handle_events(). Don't block and try to keep it short
    
    switch (event_msg->event) {
        case USB_HOST_CLIENT_EVENT_NEW_DEV: {
            MinionTools::addLog("client_event_cb: %s", "USB_HOST_CLIENT_EVENT_NEW_DEV");

            clientInfo->dev_addr = event_msg->new_dev.address; //Store the address of the new device
    
            clientInfo->event = EVENT_INSERTED;
            break;
        }
        case USB_HOST_CLIENT_EVENT_DEV_GONE: {
            MinionTools::addLog("client_event_cb: %s", "USB_HOST_CLIENT_EVENT_DEV_GONE");
            clientInfo->event = EVENT_REMOVED;
            break;
        }
        default:
            break;
    }
}

void transferData(uint8_t pTranferType, uint8_t* pData, uint8_t pSize) {
     
    if(pTranferType == TRANSFERTYPE_DATAIN) {
        MinionTools::addLog("%s, packetSize: %i", "clientTask::transferData, Type: dataIn", _clientInfo.data_maxIn);
        memset(_clientInfo.data_transferIn->data_buffer, 0, _clientInfo.data_maxIn);
        _clientInfo.data_transferIn->num_bytes = _clientInfo.data_maxIn;
        _clientInfo.data_transferIn->bEndpointAddress = _clientInfo.data_addrIn;
        _clientInfo.data_transferIn->callback = transferDataIn_cb;
        _clientInfo.data_transferIn->device_handle = _clientInfo.dev_hdl;
        _clientInfo.data_transferIn->context = (void *)&_clientInfo;

        usb_host_transfer_submit(_clientInfo.data_transferIn);
        MinionTools::addLog("%s", "clientTask Transferring dataIn");
        
    } else if(pTranferType == TRANSFERTYPE_DATAOUT) {    
        MinionTools::addLog("%s, packetSize: %i", "clientTask::transferData, Type: dataOut", pSize);
        if(pSize > _clientInfo.data_maxOut) pSize = _clientInfo.data_maxOut;
        memset(_clientInfo.data_transferOut->data_buffer, 0, _clientInfo.data_maxOut);
        memcpy(_clientInfo.data_transferOut->data_buffer, pData, pSize);
    
        _clientInfo.data_transferOut->num_bytes = pSize;
        _clientInfo.data_transferOut->device_handle = _clientInfo.dev_hdl;
        _clientInfo.data_transferOut->bEndpointAddress = _clientInfo.data_addrOut;
        _clientInfo.data_transferOut->callback = transferDataOut_cb;
        _clientInfo.data_transferOut->context = (void *)&_clientInfo;
    
        usb_host_transfer_submit(_clientInfo.data_transferOut);
        _clientInfo.event = EVENT_SENDING;
        MinionTools::addLog("%s", "clientTask Transferring dataOut");
    } else if(pTranferType == TRANSFERTYPE_CTLIN) {
        MinionTools::addLog("%s, packetSize: %i", "clientTask::transferData, Type: ctlIn", _clientInfo.ctl_maxIn);
        memset(_clientInfo.ctl_transferIn->data_buffer, 0, _clientInfo.ctl_maxIn);
        _clientInfo.ctl_transferIn->num_bytes = _clientInfo.ctl_maxIn;
        _clientInfo.ctl_transferIn->bEndpointAddress = _clientInfo.ctl_addrIn;
        _clientInfo.ctl_transferIn->callback = transferCtlIn_cb;
        _clientInfo.ctl_transferIn->device_handle = _clientInfo.dev_hdl;
        _clientInfo.ctl_transferIn->context = (void *)&_clientInfo;

        usb_host_transfer_submit(_clientInfo.ctl_transferIn);
        MinionTools::addLog("%s", "clientTask Transferring ctlIn");
    }    
}

void usb_host_state_handle_events() {   
  if(_clientInfo.event == EVENT_NONE) return;
  uint8_t action = _clientInfo.action;
  uint8_t event = _clientInfo.event;
  
    _clientInfo.event = EVENT_NONE; //Consume current event
    
    if(action == ACTION_CONNECTDEV) {
        if(event == EVENT_INSERTED) {
            MinionTools::addLog("ACTION_CONNECTDEV handleEvents: %s", "EVENT_INSERTED");
            connectDevice(&_clientInfo);            
        } else if(event == EVENT_CONNECTED) {
            MinionTools::addLog("ACTION_CONNECTDEV handleEvents: %s", "EVENT_CONNECTED");
            _isConnected = true;
            
            MinionTools::addLog("ACTION_CONNECTDEV handleEvents: %s", "send reset cmd");
            uint8_t resetCmd[] = {0,0,0,0,0,115,38,37};
            transferData(TRANSFERTYPE_DATAOUT, &resetCmd[0], sizeof(resetCmd));
            
        } else if(event ==  EVENT_SENDING) {
            MinionTools::addLog("ACTION_CONNECTDEV handleEvents: %s", "EVENT_SENDING");
            
        } else if(event == EVENT_SENT) {
            MinionTools::addLog("ACTION_RESETDEV handleEvents: %s", "EVENT_SENT");
            
            //transferDataIn(); //listen for dataIn           
        } else if(event == EVENT_RECEIVED) {
            MinionTools::addLog("ACTION_RESETDEV handleEvents: %s \n", "EVENT_RECEIVED");
            _clientInfo.action = ACTION_SENDCMD;
            _clientInfo.event = EVENT_ENABLED;
        }
        
    } else if(action == ACTION_ENABLESEND) {
        if(event == EVENT_ENABLED) {
            MinionTools::addLog("ACTION_ENABLESEND handleEvents: %s", "EVENT_ENABLED");
            uint8_t enableCmd[] = {3};
            transferData(TRANSFERTYPE_DATAOUT, &enableCmd[0], sizeof(enableCmd));            
        } else if(event == EVENT_SENT) {
            MinionTools::addLog("ACTION_ENABLESEND handleEvents: %s \n", "EVENT_SENT");
            //uint8_t sendCmd[] = {1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,0,26,17,122,1,165,0,102,0,26,17,122,1,165,0,102,0,26,255,255};
            //transferData(TRANSFERTYPE_DATAOUT, &sendCmd[0], sizeof(sendCmd));
            _clientInfo.action = ACTION_SENDCMD;
            _clientInfo.event = EVENT_ENABLED;            
        }
        
    } else if(action == ACTION_SENDCMD) {
        if(event == EVENT_ENABLED) {
            MinionTools::addLog("ACTION_SENDCMD handleEvents: %s", "EVENT_ENABLED");
            _clientInfo.state = 0;
            _clientInfo.event = EVENT_STARTED;
            
        } else if(event == EVENT_STARTED) {
            ++_clientInfo.state;
            
            uint8_t enableCmd[] = {3};
            transferData(TRANSFERTYPE_DATAOUT, &enableCmd[0], sizeof(enableCmd));
            
        } else if(event == EVENT_SENT) {
            MinionTools::addLog("ACTION_SENDCMD handleEvents: %s \n", "EVENT_SENT");
            
            if(_clientInfo.state == 1 || _clientInfo.state == 3) {
                MinionTools::addLog("ACTION_SENDCMD handleEvents: %s", "Send Command");
                ++_clientInfo.state;
                uint8_t sendCmd[] = {1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,0,26,17,122,1,165,0,102,0,26,17,122,1,165,0,102,0,26,255,255};
                transferData(TRANSFERTYPE_DATAOUT, &sendCmd[0], sizeof(sendCmd));
                
            } else if(_clientInfo.state == 2) {
                MinionTools::addLog("ACTION_SENDCMD handleEvents: %s \n", "Restart ACTION_SENDCMD");
                delay(1000);
                _clientInfo.event = EVENT_STARTED;
                
            } else {
                MinionTools::addLog("ACTION_SENDCMD handleEvents: %s \n", "ACTION_SENDCMD DONE");
                _clientInfo.action = ACTION_NONE;
            }
        }
    } else if(action == ACTION_NONE) {
        if(event == EVENT_INSERTED) {
            MinionTools::addLog("ACTION_NONE handleEvents: %s", "DEVICE CONNECTED");
            _clientInfo.action = ACTION_CONNECTDEV;
            _clientInfo.event = EVENT_INSERTED;
            MinionTools::addLog("ACTION_NONE handleEvents: %s", "Start: ACTION_CONNECTDEV");
        } else if(event == EVENT_REMOVED) {
            MinionTools::addLog("ACTION_NONE handleEvents: %s", "DEVICE DISCONNETED");
            MinionTools::addLog("ACTION_NONE handleEvents: %s", "DEVICE CLOSED");
        }        
    }

    if(_clientInfo.event != EVENT_NONE) usb_host_state_handle_events();
}

void refresh() {
    usb_host_client_handle_events(_clientInfo.client_hdl, 1);
    
    usb_host_state_handle_events();
    
    if (usb_host_lib_handle_events(1, &_clientInfo.flags) != ESP_OK) return;

    MinionTools::addLog("UsbHostTest usb_host_lib_handle_events, flags: %i", _clientInfo.flags);
    
    if(_clientInfo.flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)
        MinionTools::addLog("%s", "USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS");
    
    if(_clientInfo.flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
        MinionTools::addLog("%s", "USB_HOST_LIB_EVENT_FLAGS_ALL_FREE");
}

void open() {
    MinionTools::addLog("%s", "UsbHostTest: open");
    
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
    MinionTools::addLog("UsbHostTest, usb_host_install status: %d", _libErr);
    
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
    MinionTools::addLog("UsbHostTest, usb_host_client_register: %d", _libErr);

    //_clientInfo.ctl_maxIn = 64;
    //_libErr = usb_host_transfer_alloc(_clientInfo.ctl_maxIn, 0, &_clientInfo.ctl_transferIn);
    //MinionTools::addLog("UsbHostTest, ctlIn usb_host_transfer_alloc: %i", _libErr);  

    _clientInfo.data_maxIn = 64;
    _libErr = usb_host_transfer_alloc(_clientInfo.data_maxIn, 0, &_clientInfo.data_transferIn);
    MinionTools::addLog("UsbHostTest, dataIn usb_host_transfer_alloc: %i", _libErr);  

    _clientInfo.data_maxOut = 200;            
    _libErr = usb_host_transfer_alloc(_clientInfo.data_maxOut, 0, &_clientInfo.data_transferOut);
    MinionTools::addLog("UsbHostTest, dataOut usb_host_transfer_alloc: %i", _libErr);  
    
    MinionTools::addLog("%s \n", "UsbHostTest status: Opened \n"); 
}
} //namespace UsbhostCDC
