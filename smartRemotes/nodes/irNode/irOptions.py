wsClient = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

ttyBridge = {
    'recordCommand': 'ir-keytable -c -p all -t',
    'sendCommand'  : 'ir-ctl -S [deviceCommand]',
}

noteFilter = {
    'zone': 'livingRoom'
}
