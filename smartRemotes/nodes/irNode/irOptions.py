wsClient = {
    "endPoint": "ws://192.168.0.106:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

irBridge = {
    'onConnect': None,
    'deviceDetails': (25276, ('192.168.0.10', 80), b'\xa0C\xb0\x089\xc3', 'masterBedroom', False),
    #scan20 = (25741, ('192.168.0.20', 80), b'\xa0C\xb0pCI', 'livingRoom', False)
}

noteFilter = {
    'zone': 'masterBedroom'
}
