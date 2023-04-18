//Required System Libraries
#include <ArduinoJson.h>  // Install from IDE Library manager

//My Required Libraries
#include <SysTools.h>
#include <WsEndpoint.h>

//wsJson Hub Endpoint
char* _localEndpointInfo[] = {
    "WAP2G-MASTERBEDROOM",
    "Pin#95833",
    "000.000.000.000",
    "80"
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
void onServerEndpointObject(DynamicJsonDocument *pObject) {
///////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpointDemo::onServerEndpointObject");
}

///////////////////////////////////////////////////////////
void onClientEndpointObject(DynamicJsonDocument *pObject) {
///////////////////////////////////////////////////////////
  SysTools::addLog("wsEndpointDemo::onClientEndpointObject");
}

///////////////////////////////////////////////////////////
void setup(void) {
///////////////////////////////////////////////////////////
    delay(5000);
    WsEndpoint::awaitEndpoint(_localEndpointInfo, onServerEndpointObject);
    WsEndpoint::connectEndpoint(_localEndpointInfo, _remoteEndpointInfo, onClientEndpointObject);
}

///////////////////////////////////////////////////////////
void loop() {
///////////////////////////////////////////////////////////
    WsEndpoint::refresh();
}
