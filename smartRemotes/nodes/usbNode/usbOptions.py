# Run "python3 -m evdev.evtest" to list valid device channels

# Example:
# 0   /dev/input/event0    Anycon AirMouse Remote              usb-0000:01:00.0-1.4/input0
# 1   /dev/input/event1    Anycon AirMouse Remote Mouse        usb-0000:01:00.0-1.4/input1
# 2   /dev/input/event2    Anycon AirMouse Remote System Control usb-0000:01:00.0-1.4/input1
# 3   /dev/input/event3    Anycon AirMouse Remote Consumer Control usb-0000:01:00.0-1.4/input1

usbBridge = {
    "zone"    : "masterBedroom",
    "channels": "0,1,2,3",
}

wsClient = {
    "endPoint": "ws://192.168.0.102:8080/",
    "onConnection": "hubConnected",
    "onReceived": "receivedNote",
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
