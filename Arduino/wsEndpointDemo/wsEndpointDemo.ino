//Required System Libraries
#include <ArduinoJson.h>  // Install from IDE Library manager

//My Required Libraries
#include <SysTools.h>
#include <WsEndpoint.h>
#include <UsbDiskBridge.h>

//wsJson Hub Endpoint
char* _localEndpointInfo[] = {
    "WAP2G-MASTERBEDROOM",
    "Pin#95833",
    "000.000.000.000",
    "81"
};

//Hassio Endpoint
char* _remoteEndpointInfo[] = {
    "WAP2G-MASTERBEDROOM",
    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU",
    "192.168.0.102",
    "8123",
    "/api/websocket"
};

///////////////////////////////////////////////////////////
void onServerInputObject(DynamicJsonDocument& pObject) {
///////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpointDemo::onServerInputObject");
}

///////////////////////////////////////////////////////////
void onClientInputObject(DynamicJsonDocument& pObject) {
///////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpointDemo::onClientInputObject");
  const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            

    if(WsEndpoint::sendControlPkg(accessToken))
        SysTools::addLog("wsEndpointDemo::onClientInputObject Send accessToken sent");
    else
        SysTools::addLog("wsEndpointDemo::onClientInputObject Send accessToken failed");

    pObject.clear();
}

///////////////////////////////////////////////////////////
void setup(void) {
///////////////////////////////////////////////////////////
  const char* accessToken = "{\"type\": \"auth\", \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MDI4OWI1NjJlMTU0Mzc4YTU3ZDBiOTkwZTc0NWMyOSIsImlhdCI6MTY3MjQyNDE0MCwiZXhwIjoxOTg3Nzg0MTQwfQ.A5CqydtUIjuhr4Yf9KliXqaxMhzmduaG779ICZSZ6eU\"}";            

    delay(5000);

    DynamicJsonDocument *optionsObj = new DynamicJsonDocument(256);
    DeserializationError err = deserializeJson(*optionsObj, accessToken);
    
    if(err) {
        SysTools::addLog("wsEndpointDemo::setup, ABORT: Invalid Pkg, Reason: %s", err.c_str());
        optionsObj->clear();
        return;
    };

    UsbDiskBridge::enable("/cardDisk");
    //SysTools::saveOptionsObj(optionsObj);
    optionsObj->clear();
    
    optionsObj = SysTools::getOptionsObj("wifiStation.cfg", "/cardDisk/system");

    if(optionsObj != nullptr){
        const char* ssid = (const char*)(*optionsObj)["apSSID"];
        SysTools::addLog("wsEndpointDemo::setup, ***optionsObj ssid: %s***", ssid);
        optionsObj->clear();
    }

    WsEndpoint::awaitEndpoint(_localEndpointInfo, onServerInputObject);
    //WsEndpoint::connectEndpoint(_localEndpointInfo, _remoteEndpointInfo, onClientInputObject);
}

///////////////////////////////////////////////////////////
void loop() {
///////////////////////////////////////////////////////////
    WsEndpoint::refresh();
}
