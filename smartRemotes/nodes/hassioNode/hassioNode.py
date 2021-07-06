#############################################
##            hassioNode
#############################################
print('Load hassioNode')
    
from gi.repository import GLib
from multiprocessing import Process
import os, sys, time, json, traceback, queue, threading, asyncio

path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/maps')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/maps/map2hassio.py')
sys.path.append(path)

import wsioClient, hassioOptions, noteTool

_sessionId = 0

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
async def receivedNote(note, connection):
#############################################
    print(f' \n***receivedNote: {note}')
    global _ioQueue, _sessionId
    return
    
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
        
#############################################
async def receivedConfirmation(confirmation, connection):
#############################################
    print(f' \n***receivedConfirmation: {confirmation}')

    content = json.loads(confirmation)
    
    if(content['type'] == "auth_required"):
        payload = {
            "type": "auth",
            "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"
        }
                      
        print(f' \n***deliverPayload: {payload}')
        await wsioClient.deliverPayload(payload, connection)
    
    print(f' \n***Wait for hassio control confirmations...')
    print(f'**********************************************')
 
#############################################
async def hubConnected(connection):
#############################################
    try:
        print(f' \n***hubConnected')
        
        note = noteTool.publishNote('zoneNode', 'subscribe', {
            'title': 'control device request',
        })
        
        await wsioClient.deliverPayload(note, connection)
        
        print(f' \n***Wait for control device requests...')
        print(f'**************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hassioConnected(connection):
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
