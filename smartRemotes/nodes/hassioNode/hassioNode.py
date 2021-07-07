#############################################
##            hassioNode
#############################################
print('Load hassioNode')
    
import os, sys, time, json, traceback, queue, threading, asyncio, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import wsioClient, hassioOptions, noteTool

_transactionNum = 0

_zones = {}

# keyCode Input
_inOptions = {
    "port": "8080",
    "firstPost": "User",
    "userEvent" : None,
    "agentEvent" : None
   }

# hassio service events Output
_outOptions = {
    "endpoint": "ws://192.168.0.160:8123/api/websocket",
    "address": "192.168.0.160",
    "port": "8123",
    "path": "/api/websocket",
    "firstPost": "User",
    "userEvent" : None,
    "agentEvent" : None
}
 
#############################################
async def receivedNote(payload):
#############################################
    try:
        print(f' \n***Received Note: {payload}')    
        global _zones
        
        # validate zone
        note = noteTool.serial2object(payload)
        zone = note['content'].get('zone', None)
        focus = note['content'].get('focus', 'Media')
        if(zone == None):
            return print(f'Abort receivedNote, invalid zone: {zone}')
        
        # validate controlWord
        controlWord = note['content'].get('controlWord', None)
        if(controlWord == None):
            return print(f'Abort receivedNote, invalid controlWord: {ontrolWord}')
        
        #validate hub
        _zones[zone] = importlib.import_module(zone)
        
        deviceCommands = _zones[zone].commands[focus].get(controlWord, None)
        if(deviceCommands == None):
            return print(f'Abort receivedNote, no deviceCommands found for {controlWord}')

        for index, deviceCommand in enumerate(deviceCommands):
            #if(deviceCommand.get('device', None) == None): continue
            print(f' \n***Deliver deviceCommand: {deviceCommand}')
            _zones[zone].transactionNum += 1
            deviceCommand['id'] = _zones[zone].transactionNum
            payload = deviceCommand
            await wsioClient.deliverPayload(payload, hassioOptions.hassioNode['connection'])

    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()
    
    
    
    
    
    
    
    return
    '''
    keyCode = note.get('keyCode', None)
    zone = note.get('zone', 'home')
    if(keyCode == None): print(f'Abort inPosts, invalid keyCode: {keyCode}'); return
    
    hassioSequence = map2hassio.keyCode2hassio(keyCode, zone)
    if(hassioSequence == None): print(f'Abort inUserEvent, invalid keyCode: "{keyCode}"'); return
    
    for task in hassioSequence:
        key = list(task.keys())[0]
        data = task[key]
        command = key.split('/')
        if(command[0] == 'sleep'): time.sleep(int(data)); continue
        
        _sessionId += 1
        
        payload = {
            "id": _sessionId, 
            "type": "call_service",	
            "domain": command[0],
            "service": command[1],
            "service_data": data
        }

        print(f' \n***outTRANSFER: {payload}')
        await _outOptions['transfer'](payload, _outOptions)
    '''
 
#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***hubConnected')
        
        note = noteTool.publishNote('zoneNode', 'subscribe', {
            'title': 'control hassioNode request',
        })
        
        print(f' \n***Deliver Note: {note}')
        await wsioClient.deliverPayload(note, hassioOptions.hubNode['connection'])
        
        print(f' \n***Wait for control device requests...')
        print(f'**************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
async def receivedConfirmation(confirmation):
#############################################
    print(f' \n***receivedConfirmation: {confirmation}')

    content = json.loads(confirmation)
    
    if(content['type'] == "auth_required"):
        payload = {
            "type": "auth",
            "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"
        }
                      
        print(f' \n***deliverPayload: {payload}')
        await wsioClient.deliverPayload(payload, hassioOptions.hassioNode['connection'])
    
    print(f' \n***Wait for hassio control confirmations...')
    print(f'**********************************************')

#############################################
async def hassioConnected():
#############################################
    try:
        print(f' \n***hassioConnected')
        
        print(f' \n***Wait for hassio control confirmations...')
        print(f'**********************************************')
    except:
        print('Abort hassioConnected: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
##                MAIN
#############################################
try:
    time.sleep(3)
    
    # Start connect hassioNode
    try:
        threading.Thread(target=wsioClient.start, args=(hassioOptions.hassioNode,)).start()
    except:
        print('Abort connect hassioNode: ', sys.exc_info()[0])
        traceback.print_exc()
        
    time.sleep(1)
    
    # Start connect hubNode
    try:
        threading.Thread(target=wsioClient.start, args=(hassioOptions.hubNode,)).start()
    except:
        print('Abort connect hubNode: ', sys.exc_info()[0])
        traceback.print_exc()
except:
    print('Abort hassioNode', sys.exc_info()[0])
    traceback.print_exc()
