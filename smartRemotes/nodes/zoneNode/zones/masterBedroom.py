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
    'Task': {
    },
    
    'Climate': {
    },

    'Cover': {
    },

    'Light': {
    },

    'Security': {
    },

    'Media': {
        "Home": [
            {'device': 'ttyClient', 'controlWord': 'Home', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Menu": [
            {'device': 'ttyClient', 'controlWord': 'Menu', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
            
        "Back": [
            {'device': 'ttyClient', 'controlWord': 'Back', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Up": [
            {'device': 'ttyClient', 'controlWord': 'Up', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Down": [
            {'device': 'ttyClient', 'controlWord': 'Down', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Left": [
            {'device': 'ttyClient', 'controlWord': 'Left', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Right": [
            {'device': 'ttyClient', 'controlWord': 'Right', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Ok": [
            {'device': 'ttyClient', 'controlWord': 'Ok', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Forward": [
            {'device': 'ttyClient', 'controlWord': 'Forward', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Backward": [
            {'device': 'ttyClient', 'controlWord': 'Backward', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "PlayToggle": [
            {'device': 'ttyClient', 'controlWord': 'PlayToggle', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],                
        
        "Louder": [
            {'device': 'hassioNode', 'controlWord': 'Louder', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
        
        "Softer": [
            {'device': 'hassioNode', 'controlWord': 'Softer', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
               
        "Silence": [
            {'device': 'hassioNode', 'controlWord': 'Silence', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "Sound": [
            {'device': 'hassioNode', 'controlWord': 'Sound', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
               
        "Dream": [
            {'device': 'hassioNode', 'controlWord': 'Dream', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
               
        "Sleep": [
            {'device': 'hassioNode', 'controlWord': 'Sleep', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "Wake": [
            {'device': 'hassioNode', 'controlWord': 'Wake', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "SoundToggle": [
            {'device': 'hassioNode', 'controlWord': 'SoundToggle', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "On": [
            {'device': 'hassioNode', 'controlWord': 'On', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                
        "Off": [
            {'device': 'hassioNode', 'controlWord': 'Off', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
                 
        "PowerToggle": [
            {'device': 'hassioNode', 'controlWord': 'PowerToggle', 'zone': 'masterBedroom', 'focus': 'Media'}
        ],
    },

    'Fireplace': {
        "Left": [
            {"input_select/select_option": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "30 Minutes"}}
        ],
        
        "Up": [
            {"input_select/select_option": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "60 Minutes"}}
        ],
        
        "Right": [
            {"input_select/select_option": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "90 Minutes"}}
        ],
        
        "Down": [
            {"input_select/select_option": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "120 Minutes"}}
        ],
        
        "Ok": [
            {"switch/turn_off": {"entity_id": "switch.31485653bcddc23a2807"}}
        ]
    },

    'Task': {    
        #Wake Up	
        "Left"  : [
            #Turn Music On
            {"sonos/clear_sleep_timer": {"entity_id": "media_player.master_bedroom"}},
            {"media_player/select_source": {"entity_id": "media_player.master_bedroom", "source": "Blues"}},
            {"media_player/volume_set": {"entity_id": "media_player.master_bedroom", "volume_level": 0.25}},

            #Turn Fireplace On
            {"input_select/select_option": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "90 Minutes"}},

            #Open Shade
            {"cover/open_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"cover/open_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"sleep": 10},

            {"cover/close_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"sleep": 3.2},

            {"cover/stop_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"cover/stop_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}}
        ],

        #Morning	
        "Up"  : [
            #Turn TV On	
            {"remote/send_command": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
            {"script/publish_post": {"post": {"keyCode": "Home", "hidCode": 40, "hidMod": 12}}},
            {"media_player/select_source": {"entity_id": "media_player.firetv_masterbedroom", "source": "com.att.tv"}},

            #Turn Sond On and link bathroom speaker	
            {"sonos/clear_sleep_timer": {"entity_id": "media_player.master_bedroom"}},
            {"sonos/join": {"entity_id": "media_player.bathroom", "master": "media_player.master_bedroom"}},
            {"media_player/select_source": {"entity_id": "media_player.master_bedroom", "source": "TV"}},
            {"media_player/volume_set": {"entity_id": "media_player.bathroom", "volume_level": 0.45}},
            {"media_player/volume_set": {"entity_id": "media_player.master_bedroom", "volume_level": 0.3}}
        ],

        #Daytime	
        "Right"  : [
            #Turn TV Off
            {"remote/send_command" : {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
            {"script/publish_post": {"post": {"keyCode": "OnToggle", "hidCode": 248, "hidMod": 0}}},

            #Turn Music On
            {"sonos/unjoin": {"entity_id": "media_player.bathroom"}},
            {"media_player/select_source": {"entity_id": "media_player.master_bedroom", "source": "Blues"}},
            {"media_player/volume_set": {"entity_id": "media_player.master_bedroom", "volume_level": 0.3}}
        ],

        #Night	
        "Down"  : [
            #Turn Fireplace On
            {"input_select/select_option": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "90 Minutes"}},

            #Turn TV On
            {"remote/send_command": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
            {"script/publish_post": {"post": {"keyCode": "Home", "hidCode": 40, "hidMod": 12}}},
            {"media_player/select_source": {"entity_id": "media_player.firetv_masterbedroom", "source": "com.att.tv"}},
            #{"script/publish_post": {"post": {"keyCode": "OnToggle", "hidCode": 102, "hidMod": 0}}},

            #Turn Sound On
            {"sonos/unjoin": {"entity_id": "media_player.bathroom"}},
            {"sonos/clear_sleep_timer": {"entity_id": "media_player.master_bedroom"}},
            {"media_player/select_source": {"entity_id": "media_player.master_bedroom", "source": "TV"}},
            {"media_player/volume_set": {"entity_id": "media_player.master_bedroom", "volume_level": 0.45}},

            #Close Shades
            {"cover/close_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            #{"cover/close_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"sleep": 10},
            
            {"cover/open_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            #{"cover/open_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"sleep": 1},
            
            #{"cover/stop_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
            {"cover/stop_cover": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}},
        ]
    },
}
    