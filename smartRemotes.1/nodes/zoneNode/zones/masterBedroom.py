controller = 'Media'
isFocusSet = None
isTaskSet  = None
isSilent   = None
isOn       = None

focus = {
    "Home" 	: "Media",
    "Up" 	: "Fireplace",
    "Left"  : "Audio",
    "Right"	: "Video",
    "Down"  : "Covers",
}

commands = {
    'Audio': {
    },

    'Video': {
    },

    'Covers': {
    },

    'Media': {
        "Home": [
            {'device': 'harmonyHub', 'controlWord': 'Home', 'zone': 'masterBedroom'}
        ],
        
        "Menu": [
            {'device': 'harmonyHub', 'controlWord': 'Menu', 'zone': 'masterBedroom'}
        ],
            
        "Back": [
            {'device': 'harmonyHub', 'controlWord': 'Back', 'zone': 'masterBedroom'}
        ],
        
        "Up": [
            {'device': 'harmonyHub', 'controlWord': 'Up', 'zone': 'masterBedroom'}
        ],
        
        "Down": [
            {'device': 'harmonyHub', 'controlWord': 'Down', 'zone': 'masterBedroom'}
        ],
        
        "Left": [
            {'device': 'harmonyHub', 'controlWord': 'Left', 'zone': 'masterBedroom'}
        ],
        
        "Right": [
            {'device': 'harmonyHub', 'controlWord': 'Right', 'zone': 'masterBedroom'}
        ],
        
        "Ok": [
            {'device': 'harmonyHub', 'controlWord': 'Ok', 'zone': 'masterBedroom'}
        ],
        
        "Louder": [
            {'device': 'harmonyHub', 'controlWord': 'Louder', 'zone': 'masterBedroom'}
        ],
        
        "Softer": [
            {'device': 'harmonyHub', 'controlWord': 'Softer', 'zone': 'masterBedroom'}
        ],
        
        "Forward": [
            {'device': 'harmonyHub', 'controlWord': 'Forward', 'zone': 'masterBedroom'}
        ],
        
        "Backward": [
            {'device': 'harmonyHub', 'controlWord': 'Backward', 'zone': 'masterBedroom'}
        ],
                
        "On": [
            {'device': 'harmonyHub', 'controlWord': 'On', 'zone': 'masterBedroom'}
        ],
                
        "Off": [
            {'device': 'harmonyHub', 'controlWord': 'Off', 'zone': 'masterBedroom'}
        ],
                
        "Silence": [
            {'device': 'hidClient', 'controlWord': 'Silence', 'zone': 'masterBedroom'}
        ],
                
        "Sound": [
            {'device': 'hidClient', 'controlWord': 'Sound', 'zone': 'masterBedroom'}
        ],
        
        "PlayToggle": [
            {'device': 'harmonyHub', 'controlWord': 'PlayToggle', 'zone': 'masterBedroom'}
        ],                
                
        "SoundToggle": [
            {'device': 'harmonyHub', 'controlWord': 'SoundToggle', 'zone': 'masterBedroom'}
        ],
                
        "PowerToggle": [
            {'device': 'harmonyHub', 'controlWord': 'PowerToggle', 'zone': 'masterBedroom'}
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
    