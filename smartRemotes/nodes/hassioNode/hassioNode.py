#############################################
##            hassioNode
#############################################
print('Load hassioNode')
    
import os, sys, time, json, traceback, queue, threading, asyncio, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import wsClient, hassioOptions, noteTool

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
async def receivedNote(note):
#############################################
    try:
        print(f' \n***Received Note: {note}')    
        global _zones
        
        # validate zone
        zone = note['content'].get('zone', None)
        controller = note['content'].get('focus', 'Media')
        if(zone == None):
            return print(f'Abort receivedNote, invalid zone: {zone}')
        
        # validate controlWord
        controlWord = note['content'].get('controlWord', None)
        if(controlWord == None):
            return print(f'Abort receivedNote, invalid controlWord: {ontrolWord}')
        
        #validate map
        if(_zones.get(zone, None) == None):
            _zones[zone] = importlib.import_module(zone)
            _zones[zone].transNum = 0
            
        deviceCommands = _zones[zone].wordMap[controller].get(controlWord, None)
        if(deviceCommands == None):
            return print(f'Abort receivedNote, no deviceCommands found for {controlWord}')

        for index, deviceCommand in enumerate(deviceCommands):
            _zones[zone].transNum += 1
            deviceCommand['id'] = _zones[zone].transNum
            print(f' \n***Deliver deviceCommand: {deviceCommand}')
 
            payload = deviceCommand
            await wsClient.deliverPayload(hassioOptions.hassioNode['connection'], payload)
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()
 
#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***hubConnected')
        
        if hasattr(hassioOptions, 'noteFilter'):
            filter = hassioOptions.noteFilter
        else:
            filter = {}

        note = noteTool.publishNote('zoneNode', 'subscribe', {
            'title': 'control hassioNode request',
            'filter': filter
       })
        
        print(f' \n***Deliver Note: {note}')
        await wsClient.deliverPayload(hassioOptions.hubNode['connection'], note)
        
        print(f' \n***Wait for control device requests...')
        print(f'**************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
async def receivedConfirmation(confirmation):
#############################################
    print(f' \n***receivedConfirmation: {confirmation}')

    #content = json.loads(confirmation)
    
    if(confirmation['type'] == "auth_required"):
        payload = {
            "type": "auth",
            "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"
        }
                      
        print(f' \n***deliverPayload: {payload}')
        await wsClient.deliverPayload(hassioOptions.hassioNode['connection'], payload)
    
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
        threading.Thread(target=wsClient.start, args=(hassioOptions.hassioNode,)).start()
    except:
        print('Abort connect hassioNode: ', sys.exc_info()[0])
        traceback.print_exc()
        
    time.sleep(1)
    
    # Start connect hubNode
    try:
        threading.Thread(target=wsClient.start, args=(hassioOptions.hubNode,)).start()
    except:
        print('Abort connect hubNode: ', sys.exc_info()[0])
        traceback.print_exc()
except:
    print('Abort hassioNode', sys.exc_info()[0])
    traceback.print_exc()