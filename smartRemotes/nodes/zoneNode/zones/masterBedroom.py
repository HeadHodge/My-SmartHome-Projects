controller = 'Media'
isFocusSet = None
isTaskSet  = None
isSilent   = None
isOn       = None

focus = {
    "Home" 	: "Media",
    "Ok" 	: "Task",
    "Left"  : "Climate",
    "Up" 	: "Cover",
    "Right"	: "Light",
    "Down"  : "Security",
}

wordMap = {
    'Media': {
        "Home": [
            {'device': 'ttyNode', 'controlWord': 'Home', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Menu": [
            {'device': 'ttyNode', 'controlWord': 'Menu', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
            
        "Back": [
            {'device': 'ttyNode', 'controlWord': 'Back', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Left": [
            {'device': 'ttyNode', 'controlWord': 'Left', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Up": [
            {'device': 'ttyNode', 'controlWord': 'Up', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Right": [
            {'device': 'ttyNode', 'controlWord': 'Right', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Down": [
            {'device': 'ttyNode', 'controlWord': 'Down', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Ok": [
            {'device': 'ttyNode', 'controlWord': 'Ok', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Preset1": [
            {'device': 'hassioNode', 'controlWord': 'Preset1', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Preset2": [
            {'device': 'hassioNode', 'controlWord': 'Preset2', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Preset3": [
            {'device': 'hassioNode', 'controlWord': 'Preset3', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Preset4": [
            {'device': 'hassioNode', 'controlWord': 'Preset4', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Reset": [
            {'device': 'hassioNode', 'controlWord': 'Reset', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Backward": [
            {'device': 'ttyNode', 'controlWord': 'Backward', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "PlayToggle": [
            {'device': 'ttyNode', 'controlWord': 'PlayToggle', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],                
        
        "Forward": [
            {'device': 'ttyNode', 'controlWord': 'Forward', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Enter": [
            {'device': 'hassioNode', 'controlWord': 'Enter', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Stay": [
            {'device': 'hassioNode', 'controlWord': 'Stay', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],                
        
        "Exit": [
            {'device': 'hassioNode', 'controlWord': 'Exit', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Softer": [
            {'device': 'hassioNode', 'controlWord': 'Softer', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Louder": [
            {'device': 'hassioNode', 'controlWord': 'Louder', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
               
        "Silence": [
            {'device': 'hassioNode', 'controlWord': 'Silence', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "Sound": [
            {'device': 'hassioNode', 'controlWord': 'Sound', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "SoundToggle": [
            {'device': 'hassioNode', 'controlWord': 'SoundToggle', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                 
        "PowerOff": [
            {'device': 'hassioNode', 'controlWord': 'PowerOff', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                 
        "PowerOn": [
            {'device': 'hassioNode', 'controlWord': 'PowerOn', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
               
        "Sleep": [
            {'device': 'hassioNode', 'controlWord': 'Sleep', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
               
        "Relax": [
            {'device': 'hassioNode', 'controlWord': 'Relax', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "Wake": [
            {'device': 'hassioNode', 'controlWord': 'Wake', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
    }
}
    