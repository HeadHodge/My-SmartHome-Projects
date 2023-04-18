#ifndef SYSSPIDISPLAY_H
#define SYSSPIDISPLAY_H

namespace SysSpiDisplay {

    static char* _defaultConnectInfo[] = {
        "??SSID??",
        "??PASSWORD??",
        "??ADDRESS??",
        "??PORT??"
    };
    
    void enable(char* pConnectInfo[] = _defaultConnectInfo);
    void refresh();

    //toggleDisplay On/Off
    //pSetState 1=Display On, 0=Display Off
    //pDelay wait x millisecs before changing state
    void toggleDisplay(uint8_t pSetState=0, uint8_t pDelay=30000); 
    
} //SysSpiDisplay

#endif