#############################################
##              harmonyNode
#############################################
print('Load harmonyNode.py')

import sys, os, io, time, traceback, threading, asyncio, json, importlib
path = os.path.join('/aioharmony', os.path.dirname(__file__))
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './hubs')
sys.path.append(path)

from aioharmony.harmonyapi import HarmonyAPI, SendCommandDevice
from aioharmony.const import ClientCallbackType, WEBSOCKETS, XMPP
import wsClient, aioharmony.__main__ as harmonyHub, harmonyOptions, noteTool

_hubOptions = {}

#############################################
async def setDevice(note):
#############################################
    try:
        zone = note['content'].get('zone', None)
            
        if(harmonyOptions.hubs.get(zone, None) == None):
            return print(f'Abort receivedNote, invalid zone: {zone}')
            
        if(harmonyOptions.hubs[zone].get('config', None) == None):
            client = await harmonyHub.get_client(harmonyOptions.hubs[zone]['ip'], WEBSOCKETS, False)
            print(f"\t {json.dumps(client.json_config, sort_keys=True, indent=4)}")
           
            class commandArgs:
                device_id = 'Amazon Fire TV'
                command = 'OK'
                repeat_num = 1
                hold_secs = 0
                delay_secs = 0
                
            await harmonyHub.send_command(client, commandArgs)
         
    except:
        print('Abort setDevice: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def receivedNote(note, connection):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        
        # validate zone
        zone = note['content'].get('zone', None)
        if(zone == None):
            return print(f'Abort receivedNote, invalid zone: {zone}')
        
        # validate ontrolWord
        ontrolWord = note['content'].get('controlWord', None)
        if(ontrolWord == None):
            return print(f'Abort receivedNote, invalid ontrolWord: {ontrolWord}')
        
        #validate hub
        if(_hubOptions.get(zone, None) == None): _hubOptions[zone] = importlib.import_module(zone)
        if(_hubOptions.get(zone, None) == None): return print(f'Abort receivedNote, invalid zone: {zone}')
        options = _hubOptions[zone]

        # validate controlCommand
        controlCommand = options.controlWordMap.get(ontrolWord, None)
        if(controlCommand == None):
            return print(f'Abort receivedNote, invalid controlCommand for {ontrolWord}')
        
        # validate connection
        connection = getattr(options, 'connection', None)
        if(connection == None):
            hubIp = options.hubIp
            connection = await harmonyHub.get_client(hubIp, WEBSOCKETS, False)
            options.connection = connection
            print(f"***Hub Config:\n{json.dumps(connection.json_config, sort_keys=True, indent=4)}")
            print(json.dumps(connection.json_config, sort_keys=True, indent=4), file=open(f'/smartRemotes/{zone}.hubConfig', 'w'))

        # send device control command
        print(f' \n***Send controlCommand: {controlCommand}')

        class properties:
            if(controlCommand['deviceID'] == -1):
                activity = controlCommand['command']
            else:
                device_id = controlCommand['deviceID']
                command = controlCommand['command']
                repeat_num = controlCommand['repeatNum']
                hold_secs = controlCommand['holdSecs']
                delay_secs = controlCommand['delaySecs']
                        
        if(controlCommand['deviceID'] == -1):
            await harmonyHub.start_activity(connection, properties)
        else:
            await harmonyHub.send_command(connection, properties)
        
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hubConnected(connection):
#############################################
    try:
        print(f' \n***hubConnected')
            
        if hasattr(harmonyOptions, 'noteFilter'):
            filter = harmonyOptions.noteFilter
        else:
            filter = {}

        note = noteTool.publishNote('harmonyNode', 'subscribe', {
            'title': 'control harmonyHub request',
            'filter': filter
        })
        
        await wsClient.deliverNote(note, connection)
        
        print(f' \n***Wait for \'{note["content"]["title"]}\' notes...')
        print(f'*********************************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
#############################################
try:       
    time.sleep(3)
    
    #Start wsClient Module
    try:
        thread = threading.Thread(target=wsClient.start, args=(harmonyOptions.wsClient,))
        thread.start()
        time.sleep(1)
    except:
        print('Abort run wsClient: ', sys.exc_info()[0])
        traceback.print_exc()
except:
    print('Abort zoneNode', sys.exc_info()[0])
    traceback.print_exc()

