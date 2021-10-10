wsClient = {
    "endPoint": "ws://192.168.0.163:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

irBridge = {
    'onConnect': None,
    'recordCommand': 'ir-keytable -c -p all -t',
    'sendCommand'  : 'ir-ctl -S [deviceCommand]',
}

noteFilter = {
    'zone': 'livingRoom'
}
