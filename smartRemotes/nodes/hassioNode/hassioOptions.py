import sys, time, traceback

parent = sys.modules["__main__"]
accessToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"

hubNode = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

hassioNode = {
    "endPoint": "ws://192.168.0.160:8123/api/websocket",
    "onConnection": "hassioConnected",
    "onReceived": "receivedConfirmation",
}
