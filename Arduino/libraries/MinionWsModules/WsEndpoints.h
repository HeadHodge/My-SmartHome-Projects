#ifndef WSENPOINTS_H
#define WSENPOINTS_H
#include <ArduinoJson.h>  // Install from IDE Library manager

namespace WsEndpoints {
    //typedef void (*sendEndpointPkg)(DynamicJsonDocument* pEndpointPkg);
    typedef void (*onEndpointPkg)(char* pSource, DynamicJsonDocument* pPkgIn, void (*sendEndpointPkg)(DynamicJsonDocument* pEndpointPkg));
    #define CURRENT_TIME millis()
    
    enum connectOptions {
        CONNECT_SSID, CONNECT_PASSWORD, CONNECT_ADDRESS, CONNECT_PORT, CONNECT_PATH
    };

    static char* _defaultConnectInfo[] = {
        "NOSSID",
        "NOPASSWORD",
        "NOADDRESS",
        "NOPORT",
        "NOPATH"
    };
  
    bool enable(onEndpointPkg pOnPkgInput);
    bool createWifiAP();
    void sendEndpointPkg(DynamicJsonDocument* pEndpointPkg);
    bool sendReplyPkg(const char *pPkg);
    bool sendControlPkg(const char *pPkg);    
    bool sendControlPkg(DynamicJsonDocument& pPkg);
    void refresh();
    bool isNetworkConnected();
    
    bool awaitEndpoint(
        char* pConnectInfo[] = _defaultConnectInfo,
        void (*pOnPkgInput)(DynamicJsonDocument& pPkgObj) = nullptr
    );
    
    bool connectEndpoint(
        char* pEndpointInfo[] = _defaultConnectInfo,
        void (*pOnPkgInput)(DynamicJsonDocument& pPkgObj) = nullptr
    );
} //namespace WsEndpoints

#endif