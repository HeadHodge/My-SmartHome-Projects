#############################################
##       hassioNode commandWord map
#############################################
print('Load hassioNode commandWord map')

transactionNum = 0

commands = {
    "Media": {
        "Louder": [
            {"id": 0, "type": "call_service", "domain": "media_player", "service": "volume_up", "service_data": {"entity_id": "media_player.master_bedroom"}},
        ],
        
        "Softer": [
            {"type": "call_service", "domain": "media_player", "service": "volume_down", "service_data": {"entity_id": "media_player.master_bedroom"}},
        ],
                
        "Silence": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": True}},
        ],
        
        "Sound": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": False}},
        ],
        
        "Dream": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": True}},
        ],
        
        "Sleep": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
            {"type": "call_service", "domain": "sonos", "service": "unjoin", "service_data": {"entity_id": "media_player.bathroom"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.master_bedroom", "source": "Blues"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.master_bedroom", "volume_level": 0.14}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": False}},
            {"type": "call_service", "domain": "sonos", "service": "set_sleep_timer", "service_data": {"entity_id": "media_player.master_bedroom", "sleep_time": 3600}},
        ],
                
        "Wake": [
            {"type": "call_service", "domain": "sonos", "service": "unjoin", "service_data": {"entity_id": "media_player.bathroom"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.master_bedroom", "source": "Rock"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": False}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.master_bedroom", "volume_level": 0.4}},
        ],
         
        "On": [
        #Turn on TV
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.master_bedroom", "source": "TV"}},
        ],
        
        "Off": [
        #Turn off TV
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
        ],
    }
}
