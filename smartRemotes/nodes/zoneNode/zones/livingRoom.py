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
            {'device': 'ttyClient', 'controlWord': 'Home', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Menu": [
            {'device': 'ttyClient', 'controlWord': 'Menu', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
            
        "Back": [
            {'device': 'ttyClient', 'controlWord': 'Back', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Up": [
            {'device': 'ttyClient', 'controlWord': 'Up', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Down": [
            {'device': 'ttyClient', 'controlWord': 'Down', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Left": [
            {'device': 'ttyClient', 'controlWord': 'Left', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Right": [
            {'device': 'ttyClient', 'controlWord': 'Right', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Ok": [
            {'device': 'ttyClient', 'controlWord': 'Ok', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Forward": [
            {'device': 'ttyClient', 'controlWord': 'Forward', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Backward": [
            {'device': 'ttyClient', 'controlWord': 'Backward', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "PlayToggle": [
            {'device': 'ttyClient', 'controlWord': 'PlayToggle', 'zone': 'livingRoom', 'focus': 'Media'}
        ],                
        
        "Louder": [
            {'device': 'irNode', 'controlWord': 'Louder', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
        
        "Softer": [
            {'device': 'irNode', 'controlWord': 'Softer', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
               
        "Silence": [
            {'device': 'irNode', 'controlWord': 'Silence', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "Sound": [
            {'device': 'irNode', 'controlWord': 'Sound', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
               
        "Dream": [
            {'device': 'hassioNode', 'controlWord': 'Dream', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
               
        "Sleep": [
            {'device': 'hassioNode', 'controlWord': 'Sleep', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "Wake": [
            {'device': 'hassioNode', 'controlWord': 'Wake', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "SoundToggle": [
            {'device': 'hassioNode', 'controlWord': 'SoundToggle', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "On": [
            {'device': 'irNode', 'controlWord': 'On', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                
        "Off": [
            {'device': 'irNode', 'controlWord': 'Off', 'zone': 'livingRoom', 'focus': 'Media'}
        ],
                 
        "PowerToggle": [
            {'device': 'irNode', 'controlWord': 'PowerToggle', 'zone': 'livingRoom', 'focus': 'Media'}
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
    