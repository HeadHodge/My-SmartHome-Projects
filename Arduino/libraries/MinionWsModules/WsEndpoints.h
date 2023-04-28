#ifndef WSENPOINTS_H
#define WSENPOINTS_H
#include <ArduinoJson.h>  // Install from IDE Library manager

namespace WsEndpoints {
    typedef void (*onEndpointPkg)(char* pSource, DynamicJsonDocument* pPkgIn, bool (*sendEndpointPkg)(DynamicJsonDocument* pEndpointPkg));
    #define CURRENT_TIME millis()
  
    bool enable(onEndpointPkg pOnPkgInput);
    void refresh();
    bool isNetworkConnected();
    bool sendHttpServerPkg(DynamicJsonDocument* pPkg);
    bool sendWsClientPkg(DynamicJsonDocument* pPkg);
    bool sendWsServerPkg(DynamicJsonDocument* pPkg);
   
    bool connectWsClientEndpoint(
        char* pIpAddress = "192.168.0.102",
        uint16_t pPort = 8123,
        char* pPath = "/api/websocket",
        char* pLoginId = "admin",
        char* pLoginPsw = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU"      
    );
} //namespace WsEndpoints

#endif