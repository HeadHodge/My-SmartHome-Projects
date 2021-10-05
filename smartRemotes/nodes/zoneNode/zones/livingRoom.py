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
            {'device': 'ttyNode', 'controlWord': 'Home', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Menu": [
            {'device': 'ttyNode', 'controlWord': 'Menu', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
            
        "Back": [
            {'device': 'ttyNode', 'controlWord': 'Back', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Up": [
            {'device': 'ttyNode', 'controlWord': 'Up', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Down": [
            {'device': 'ttyNode', 'controlWord': 'Down', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Left": [
            {'device': 'ttyNode', 'controlWord': 'Left', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Right": [
            {'device': 'ttyNode', 'controlWord': 'Right', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Ok": [
            {'device': 'ttyNode', 'controlWord': 'Ok', 'zone': 'livingRoom', 'focus': 'Media'}
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
            {'device': 'ttyNode', 'controlWord': 'Backward', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "PlayToggle": [
            {'device': 'ttyNode', 'controlWord': 'PlayToggle', 'zone': 'livingRoom', 'focus': 'Media'}
        ],                
        
        "Forward": [
            {'device': 'ttyNode', 'controlWord': 'Forward', 'zone': 'livingRoom', 'focus': 'Media'}
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
        
        "Louder": [
            {'device': 'hassioNode', 'controlWord': 'Louder', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Softer": [
            {'device': 'hassioNode', 'controlWord': 'Softer', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
               
        "Silence": [
            {'device': 'hassioNode', 'controlWord': 'Silence', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "Sound": [
            {'device': 'hassioNode', 'controlWord': 'Sound', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
               
        "Sleep": [
            {'device': 'hassioNode', 'controlWord': 'Sleep', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
               
        "Relax": [
            {'device': 'hassioNode', 'controlWord': 'Relax', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "Wake": [
            {'device': 'hassioNode', 'controlWord': 'Wake', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "SoundToggle": [
            {'device': 'hassioNode', 'controlWord': 'SoundToggle', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "On": [
            {'device': 'hassioNode', 'controlWord': 'On', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "Off": [
            {'device': 'hassioNode', 'controlWord': 'Off', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
    },
}
    