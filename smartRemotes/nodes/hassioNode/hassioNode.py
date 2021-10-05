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

_transNum = 0
_zones = {}

#############################################
async def receivedNote(note):
#############################################
    try:
        print(f' \n***Received Note: {note}')    
        global _zones, _transNum
        
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
        if(_zones.get(zone, None) == None): _zones[zone] = importlib.import_module(zone)
            
        deviceCommands = _zones[zone].wordMap[controller].get(controlWord, None)
        if(deviceCommands == None):
            return print(f'Abort receivedNote, no deviceCommands found for {controlWord}')

        for index, deviceCommand in enumerate(deviceCommands):
            _transNum += 1
            deviceCommand['id'] = _transNum
            print(f' \n***Deliver deviceCommand: {deviceCommand}')
            
            if(deviceCommand['type'] == 'wait'):
                print(f'Wait: {deviceCommand["waitSecs"]}')
                time.sleep(deviceCommand['waitSecs'])
                continue
                
            payload = deviceCommand
            await wsClient.deliverPayload(hassioOptions.hassioNode['connection'], payload)
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()
 
#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***Hub connected: {hassioOptions.hubNode["endPoint"]}')
        
        if hasattr(hassioOptions, 'noteFilter'):
            filter = hassioOptions.noteFilter
        else:
            filter = {}

        note = noteTool.publishNote('zoneNode', 'subscribe', {
            'title': 'control hassioNode request',
            'filter': filter
       })
        
        print(f' \n***Deliver note: {note}')
        await wsClient.deliverPayload(hassioOptions.hubNode['connection'], note)
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
async def receivedConfirmation(confirmation):
#############################################
    if(confirmation['type'] == "auth_required"):
        payload = {
            "type": "auth",
            "access_token": hassioOptions.accessToken
        }
                      
        print(f' \n***deliver access token: {payload}')
        await wsClient.deliverPayload(hassioOptions.hassioNode['connection'], payload)
    elif(confirmation['type'] == "result" and confirmation['success'] != True):
        print(f' \n***Deliver Payload Failed: {confirmation}')

#############################################
async def hassioConnected():
#############################################
    try:
        print(f' \n***hassioConnected: {hassioOptions.hassioNode["endPoint"]}')
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
