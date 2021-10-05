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
        
        "Sleep": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.living_room", "source": "Blues"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.living_room", "volume_level": 0.14}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": False}},
            {"type": "call_service", "domain": "sonos", "service": "set_sleep_timer", "service_data": {"entity_id": "media_player.living_room", "sleep_time": 3600}},
        ],
        
        "Relax": [
            {"type": "call_service", "domain": "cover", "service": "set_cover_position", "service_data": {"entity_id": "cover.downstairs_slider", "position": 30}}, #Close DiningRoom Shade
            {"type": "call_service", "domain": "cover", "service": "set_cover_position", "service_data": {"entity_id": "cover.springs_window_fashions_graber_csz1_cellular_shade_level", "position": 0}}, #Close LivingRoom Shade
            {"type": "call_service", "domain": "cover", "service": "close_cover", "service_data": {"entity_id": "cover.somfy_unknown_type_5a52_id_5401_level"}}, #Close BedRoom Screen
        ],
                
        "Wake": [
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.living_room", "source": "Blues"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": False}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.living_room", "volume_level": 0.2}},
        #   {"type": "call_service", "domain": "cover", "service": "set_cover_position", "service_data": {"entity_id": "cover.downstairs_slider", "position": 65}}, #Open DiningRoom Shade
            {"type": "call_service", "domain": "cover", "service": "set_cover_position", "service_data": {"entity_id": "cover.springs_window_fashions_graber_csz1_cellular_shade_level", "position": 50}}, #Open LivingRoom Shade
        ],
        
        "Off": [
        #Turn off TV
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.living_room", "is_volume_muted": True}},
        ],
         
        "On": [
        #Turn on TV
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_downstairs_remote", "device": "Insignia FireTV", "command": "On/Off"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.living_room", "source": "TV"}},
        ],
    }
}
