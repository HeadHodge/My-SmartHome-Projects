#ifndef BLEHUB_H
#define BLEHUB_H
#include <MinionTools.h>

namespace BleHub
{
    bool isHubConnected();
    void openHub(const char *);
    void controlDevice(MinionTools::hidKey*);
}

#endif