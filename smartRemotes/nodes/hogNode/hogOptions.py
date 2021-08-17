wsClient = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

pairAgent = {
    "pairSecurity": "justWorks",
}

pairAdvertise = {
    "btmgmt": {
        "timeout"    : "0",
        "rm-adv"     : "1",
        "connectable": "on",
        "linksec"    : "off",
        "bredr"      : "off",
        "le"         : "on",
        "advertising": "off",
        "name"       : "bluezHoG",
        "appearance" : "961",
        "discov"     : "limited",
        "add-adv"    : "-t 0 -c -g -n -a -p 1",
        "power"      : "on"
    }
}


hidKeyboard = {
}

noteFilter = {
    'zone': 'livingRoom'
}