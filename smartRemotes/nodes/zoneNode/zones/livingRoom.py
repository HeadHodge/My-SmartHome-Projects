controller ='Media'
isFocusSet = None
isTaskSet  = None
isSilent   = None

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
            {'device': 'harmonyHub', 'controlWord': 'Home', 'zone': 'livingRoom'}
        ],
        
        "Menu": [
            {'device': 'harmonyHub', 'controlWord': 'Menu', 'zone': 'livingRoom'}
        ],
            
        "Back": [
            {'device': 'harmonyHub', 'controlWord': 'Back', 'zone': 'livingRoom'}
        ],
        
        "Up": [
            {'device': 'harmonyHub', 'controlWord': 'Up', 'zone': 'livingRoom'}
        ],
        
        "Down": [
            {'device': 'harmonyHub', 'controlWord': 'Down', 'zone': 'livingRoom'}
        ],
        
        "Left": [
            {'device': 'harmonyHub', 'controlWord': 'Left', 'zone': 'livingRoom'}
        ],
        
        "Right": [
            {'device': 'harmonyHub', 'controlWord': 'Right', 'zone': 'livingRoom'}
        ],
        
        "Ok": [
            {'device': 'harmonyHub', 'controlWord': 'Ok', 'zone': 'livingRoom'}
        ],
        
        "Louder": [
            {'device': 'harmonyHub', 'controlWord': 'Louder', 'zone': 'livingRoom'}
        ],
        
        "Softer": [
            {'device': 'harmonyHub', 'controlWord': 'Softer', 'zone': 'livingRoom'}
        ],
                
        "Silence": [
            {'device': 'hidClient', 'controlWord': 'Silence', 'zone': 'livingRoom'}
        ],
                
        "Sound": [
            {'device': 'hidClient', 'controlWord': 'Sound', 'zone': 'livingRoom'}
        ],
                
        "SoundToggle": [
            {'device': 'hidClient', 'controlWord': 'SoundToggle', 'zone': 'livingRoom'}
        ],
        
        "Forward": [
            {'device': 'harmonyHub', 'controlWord': 'Forward', 'zone': 'livingRoom'}
        ],
        
        "Backward": [
            {'device': 'harmonyHub', 'controlWord': 'Backward', 'zone': 'livingRoom'}
        ],
        
        "PlayToggle": [
            {'device': 'harmonyHub', 'controlWord': 'PlayToggle', 'zone': 'livingRoom'}
        ],
                
        "On": [
            {'device': 'harmonyHub', 'controlWord': 'On', 'zone': 'livingRoom'}
        ],
                
        "Off": [
            {'device': 'harmonyHub', 'controlWord': 'Off', 'zone': 'livingRoom'}
        ],
             
        "PowerToggle": [
            #Turn off TV
            {"script/publish_post": {"post": {"keyCode": "OnToggle", "hidCode": 0x30, "hidReport": 2}}},
            {"remote/send_command" : {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
            {"sleep": 2},

            #Turn on Sleep Timer with Music
            {"sonos/unjoin": {"entity_id": "media_player.bathroom"}},
            {"media_player/select_source": {"entity_id": "media_player.master_bedroom", "source": "Blues"}},
            {"media_player/volume_set": {"entity_id": "media_player.master_bedroom", "volume_level": 0.18}},
            {"sonos/set_sleep_timer": {"entity_id": "media_player.master_bedroom", "sleep_time": 3600}}
        ]
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
