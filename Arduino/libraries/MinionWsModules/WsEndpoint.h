#ifndef WSENPOINT_H
#define WSENPOINT_H

namespace WsEndpoint {
    //typedef char* (*callBack)(unsigned char*);
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
  
    void replyEndpoint(const char *pMessage);
    void sendEndpoint(const char *pMessage);    
    char** getConnectInfo();
    void refresh();
    bool isWifiConnected();
    
    bool awaitEndpoint(
        char* pConnectInfo[] = _defaultConnectInfo,
        void (*pOnEndpointObject)(DynamicJsonDocument *pObject) = nullptr
    );
    
    bool connectEndpoint(
        char* pConnectInfo[] = _defaultConnectInfo,
        char* pEndpointInfo[] = _defaultConnectInfo,
        void (*pOnEndpointObject)(DynamicJsonDocument *pObject) = nullptr
    );
} //namespace WsEndpoint

#endif