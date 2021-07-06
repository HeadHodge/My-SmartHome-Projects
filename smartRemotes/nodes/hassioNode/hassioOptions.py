import sys, time, traceback

parent = sys.modules["__main__"]

hubNode = {
    "endPoint": "ws://192.168.0.162:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

hassioNode = {
    "endPoint": "ws://192.168.0.160:8123/api/websocket",
    "onConnection": "hassioConnected",
    "onReceived": "receivedConfirmation",
}
