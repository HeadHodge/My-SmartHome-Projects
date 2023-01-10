#ifndef WSHUB_H
#define WSHUB_H

namespace WsHub
{
    typedef char* (*callBack)(unsigned char*); // type for conciseness
    bool isHubConnected();
    void openHub(callBack);
    void refreshHub();
}

#endif