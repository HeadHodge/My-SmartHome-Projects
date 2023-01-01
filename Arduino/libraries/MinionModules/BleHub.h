#ifndef BLEHUB_H
#define BLEHUB_H

namespace BleHub
{
    bool isHubConnected();
    void openHub(const char *);
    void controlDevice(int pControlCode, int pDuration, int pDelay, int pReportId);
}

#endif