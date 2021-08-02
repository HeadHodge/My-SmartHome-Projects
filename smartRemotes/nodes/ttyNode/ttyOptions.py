wsClient = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

ttyBridge = {
    'port'      : '/dev/ttyACM0',
    'speed'     : 115200,
    'timeout'   : None,
    'parity'    : 'N', #None
    'xonxoff'   : 0,
    'rtscts'    : 1,  
}

noteFilter = {
    'zone': 'masterBedroom'
}
