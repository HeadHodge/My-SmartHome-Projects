#############################################
##       hassioNode commandWord map
#############################################
print('Load hassioNode masterBedroom commandWord map')

wordMap = {
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
        
        "Sleep": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"device": "Vizio", "command": "PowerOff", "num_repeats": 2}, "target": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device_id": "dc9da11c2d596030c830962ca5d4c0cb"}},
            {"type": "call_service", "domain": "sonos", "service": "unjoin", "service_data": {"entity_id": "media_player.bathroom"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.master_bedroom", "source": "Blues"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.master_bedroom", "volume_level": 0.14}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": False}},
            {"type": "call_service", "domain": "sonos", "service": "set_sleep_timer", "service_data": {"entity_id": "media_player.master_bedroom", "sleep_time": 7200}},
            {"type": "call_service", "domain": "cover", "service": "close_cover", "service_data": {"entity_id": "cover.z_wave_to_rts_interface_virtual_node_2"}},
        ],
        
        "Relax": [
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": True}},
        ],
                
        "Wake": [
            {"type": "call_service", "domain": "sonos", "service": "join", "service_data": {"master": "media_player.master_bedroom", "entity_id": "media_player.bathroom"}},
            {"type": "call_service", "domain": "media_player", "service": "select_source", "service_data": {"entity_id": "media_player.master_bedroom", "source": "Blues"}},
            {"type": "call_service", "domain": "media_player", "service": "volume_mute", "service_data": {"entity_id": "media_player.master_bedroom", "is_volume_muted": False}},
            {"type": "call_service", "domain": "media_player", "service": "volume_set", "service_data": {"entity_id": "media_player.master_bedroom", "volume_level": 0.2}},
            {"type": "call_service", "domain": "cover", "service": "close_cover", "service_data": {"entity_id": "cover.z_wave_to_rts_interface_virtual_node_2"}},
            {"type": "wait", "waitSecs": 12},
            {"type": "call_service", "domain": "cover", "service": "open_cover", "service_data": {"entity_id": "cover.z_wave_to_rts_interface_virtual_node_2"}},
            {"type": "wait", "waitSecs": 7.5},
            {"type": "call_service", "domain": "cover", "service": "stop_cover", "service_data": {"entity_id": "cover.z_wave_to_rts_interface_virtual_node_2"}},
        ],       
              
        "Reset": [
            {"type": "call_service", "domain": "switch", "service": "turn_off", "service_data": {"entity_id": "switch.31485653bcddc23a2807"}},
        ],       
               
        "Preset1": [
            {"type": "call_service", "domain": "input_select", "service": "select_option", "service_data": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "30 Minutes"}},
        ],       
               
        "Preset2": [
            {"type": "call_service", "domain": "input_select", "service": "select_option", "service_data": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "60 Minutes"}},
        ],       
               
        "Preset3": [
            {"type": "call_service", "domain": "input_select", "service": "select_option", "service_data": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "90 Minutes"}},
        ],       
               
        "Preset4": [
            {"type": "call_service", "domain": "input_select", "service": "select_option", "service_data": {"entity_id": "input_select.masterbedroom_fireplace_duration", "option": "120 Minutes"}},
        ],       
        
        "PowerOff": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"device": "Vizio", "command": "PowerOff", "num_repeats": 2}, "target": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device_id": "dc9da11c2d596030c830962ca5d4c0cb"}},
        ],
     
        "PowerOn": [
            {"type": "call_service", "domain": "remote", "service": "send_command", "service_data": {"entity_id": "remote.broadlink_ir_hub_upstairs_remote", "device": "Vizio", "command": "On/Off"}},
        ],
    }
}
