wsHub = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

wsHassio = {
    "endPoint": "ws://192.168.0.160:8123/api/websocket",
    "onConnection": "hassioConnected",
    "onReceived": "receivedNotice",
}

wsClient = wsHub

ttyBridge = {
    "onReceived": None,
    "onConnection": None,
    "connection" : {
        'port'      : '/dev/pts/0',
        'speed'     : 115200,
        'timeout'   : None,
        'parity'    : 'N', #None
        'xonxoff'   : 0,
        'rtscts'    : 0,  
    }
}

posixBridge = {
    'osCommand' : '/smartRemote/nodes/ttyNode/runPosix.sh',
}

noteFilter = {
    'zone': 'livingRoom'
}

hassioEvents = {
    "event_type": "notePublished",
    "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"
         
    '''
    Note: send hassio script events with the following format
    
    service: script.publish_note
    data:
        note: {'title': 'notePubished event', 'content': {'controlWord': 'Ok', 'zone': 'livingRoom'}}

    'controlWord' = Specify a control word to send the desired key press
    'zone' = Specify the zone this node is running in.
    ***Review the ./zones directory for supported zones and control words
    '''
}
