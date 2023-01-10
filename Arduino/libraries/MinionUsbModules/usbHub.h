#ifndef USBHUB_H
#define USBHUB_H
#include <MinionTools.h>

namespace UsbHub
{
    bool isHubConnected();
    void openHub();
    void controlDevice(MinionTools::hidKey*);
}

#endif