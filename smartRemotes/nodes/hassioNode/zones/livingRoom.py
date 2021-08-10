#############################################
##       hassioNode commandWord map
#############################################
print('Load hassioNode livingRoom commandWord map')

wordMap = {
    "Media": {
        "Louder": [
            {"id": 0, "type": "call_service", "domain": "media_player", "service": "volume_up", "service_data": {"entity_id": "media_player.living_room"}},
        ],
        
        "Softer": [
            {"type": "call_service", "domain": "media_player", "service": "volume_down", "service_data": {"entity_id": "media_player.living_room"}},
        ],
                
        "Silence": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": True}},
        ],
        
        "Sound": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": False}},
        ],
        
        "Dream": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": True}},
        ],
        
        "Sleep": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
            {"type": "call_service", "domain": "sonos", "service": "unjoin", "service_data": {"entity_id": "media_player.bathroom"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.living_room", "source": "Blues"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.living_room", "volume_level": 0.14}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": False}},
            {"type": "call_service", "domain": "sonos", "service": "set_sleep_timer", "service_data": {"entity_id": "media_player.living_room", "sleep_time": 3600}},
        ],
                
        "Wake": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
            {"type": "call_service", "domain": "sonos", "service": "unjoin", "service_data": {"entity_id": "media_player.bathroom"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.living_room", "source": "Rock"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": False}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.living_room", "volume_level": 0.4}},
        ],
         
        "On": [
        #Turn on TV
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.living_room", "source": "TV"}},
        ],
        
        "Off": [
        #Turn off TV
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
        ],
    }
}
