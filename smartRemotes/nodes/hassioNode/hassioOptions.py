import sys, time, traceback

parent = sys.modules["__main__"]
accessToken-3911 = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"
accessToken-3121 = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiIzZTNmZGRiY2Y2NGE0YjczOGI1NzQxOTExZDIwN2VmNyIsImlhdCI6MTYzNDE1MTU4NSwiZXhwIjoxOTQ5NTExNTg1fQ.PAo52K5_mAtoVsjU_RBvJmwq3OZXu5UUmzsan1HNQ-s"
accessToken = accessToken-3121

hubNode = {
    "endPoint": "ws://192.168.0.106:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

hassioNode = {
    "endPoint": "ws://192.168.0.102:8123/api/websocket",
    "onConnection": "hassioConnected",
    "onReceived": "receivedConfirmation",
}
