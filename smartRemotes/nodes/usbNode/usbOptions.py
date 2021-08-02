wsClient = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
}

usbBridge = {
    "zone"    : "masterBedroom",
    "channels": "0,1,2,3",
}

scanCodeMap = {
    1  : {"controlWord": "Back"},
    28 : {"controlWord": "Ok"},
    59 : {"controlWord": "SoundToggle"},
    103: {"controlWord": "Up"},
    104: {"controlWord": "Less"},
    105: {"controlWord": "Left"},
    106: {"controlWord": "Right"},
    108: {"controlWord": "Down"},
    109: {"controlWord": "More"},
    113: {"controlWord": "SoundToggle"},
    114: {"controlWord": "Softer"},
    115: {"controlWord": "Louder"},
    116: {"controlWord": "PowerToggle"},
    127: {"controlWord": "Menu"},
    158: {"controlWord": "Exit"},
    163: {"controlWord": "Forward"},
    164: {"controlWord": "PlayToggle"},
    165: {"controlWord": "Backward"},
    172: {"controlWord": "Home"},
    191: {"controlWord": "PowerToggle"},
    240: {"controlWord": "Focus"},
    272: {"controlWord": "Ok"},
    273: {"controlWord": "Back"},
}
