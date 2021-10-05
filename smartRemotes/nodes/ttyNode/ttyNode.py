#############################################
##               ttyNode
#############################################
print('Load ttyNode')

from multiprocessing import Process
from dbus.mainloop.glib import DBusGMainLoop
import os, sys, time, json, asyncio, traceback, queue, threading, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/usb')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import wsClient, ttyBridge, ttyOptions, noteTool

_zones = {}
_isIdle = True

#############################################
async def receivedNote(note):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        global _zones, _isIdle
        _isIdle = False
        
        # validate zone
        zone = note['content'].get('zone', None)
        if(zone == None):
            return print(f'Abort receivedNote, invalid zone: {zone}')
        
        # validate controlWord
        controlWord = note['content'].get('controlWord', None)
        if(controlWord == None):
            return print(f'Abort receivedNote, invalid controlWord: {controlWord}')
        
        # validate controlMap
        if(_zones.get(zone, None) == None): _zones[zone] = importlib.import_module(zone)
        if(_zones.get(zone, None) == None): return print(f'Abort receivedNote, invalid zone: {zone}')

        # validate commandOptions
        commandOptions = _zones[zone].wordMap.get(controlWord, None)
        if(commandOptions == None):
            return print(f'Abort commandOptions, invalid commandOptions for {controlWord}')
        
        keyPressSecs = commandOptions[0].get("keyPressSecs", .1)
        keyDownCommand = commandOptions[0].get("deviceCommand", [0xff, 0x00, 0x00, 0x00, 0x00, 0x00])
        keyUpCommand = [0xff, keyDownCommand[1], 0x00, 0x00, 0x00, 0x00]

        #print(f'** Deliver: {keyDownCommand}')
        await ttyBridge.deliverCommand(
            ttyOptions.ttyBridge['connection'],
            bytes(keyDownCommand)
        )
        
        #if(keyDownCommand[1] == 3): return
        return
        
        time.sleep(keyPressSecs)
        
        #print(f'** Deliver: {keyUpCommand}')
        await ttyBridge.deliverCommand(
            ttyOptions.ttyBridge['connection'],
            bytes(keyUpCommand)
        )
        
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***hubConnected')
        
        if hasattr(ttyOptions, 'noteFilter'):
            filter = ttyOptions.noteFilter
        else:
            filter = {}
       
        note = noteTool.publishNote('gattNode', 'subscribe', {
            'title' : 'control ttyNode request',
            'filter': filter
        })
        
        await wsClient.deliverPayload(ttyOptions.wsClient['connection'], note)
        #await ttyBridge.deliverCommand(ttyOptions.ttyBridge['connection'], bytes([0x0d]))
        
        print(f' \n***Wait for \'{note["content"]["title"]}\' notes...')
        print(f'*********************************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
async def receivedNotice(notice):
#############################################
    try:
        global _zones
       
        if(notice['type'] == "auth_required"):
            print(f'*** receivedNotice: "auth_required"')
            
            payload = {
                "type": "auth",
                "access_token": ttyOptions.hassioEvents["access_token"] 
            }
                          
            print(f' \n***deliverPayload: {payload}')
            await wsClient.deliverPayload(ttyOptions.wsClient['connection'], payload)
        
        elif(notice['type'] == "auth_ok"):
            print(f'*** receivedNotice: "auth_ok"')
            
            payload = {
                "id"        : 100,
                "type"      : "subscribe_events",
                "event_type": ttyOptions.hassioEvents["event_type"]    
            }
            
            print(f' \n**deliver: {payload}')
            await wsClient.deliverPayload(ttyOptions.wsClient['connection'], payload)
        
        elif(notice['type'] == "result"):
            print(f'*** receivedNotice: "result" {notice}')
        
        elif(notice['type'] == "event"):
            note = notice["event"]["data"]["note"]
            print(f'*** receivedNotice: "notePublished" {note}')
            await receivedNote(note)

        else:
            print(f'*** receivedNotice: {notice}')
           
        print(f' \n***Wait for hassio notices...')
        print(f'**********************************************')
    except:
        print('Abort receivedNotice: ', sys.exc_info()[0])
        traceback.print_exc()

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
async def ttyConnected():
#############################################
    try:
        print(f' \n***ttyConnected')
        global _isIdle
        keyUpCommand = [0xff, 0x01, 0x00, 0x00, 0x00, 0x00]
        return
        
        while True:
            _isIdle = True
            await asyncio.sleep(5)
            if(_isIdle == False): continue
            
            print('ping')
            await ttyBridge.deliverCommand(
                ttyOptions.ttyBridge['connection'],
                bytes(keyUpCommand)
            )
    except:
        print('Abort ttyConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
def start():
#############################################
    print('Start ttyNode')

    try:
        time.sleep(3)
        '''
        # Start posixBridge module
        try:            
            threading.Thread(target=posixBridge.start, args=(ttyOptions.posixBridge,)).start()
        except:
            print('Abort posixBridge: ', sys.exc_info()[0])
            traceback.print_exc()
        
        time.sleep(1)
        '''
        
        # Start ttyBridge module
        try:   
            ttyOptions.ttyBridge['onConnection'] = ttyConnected
            threading.Thread(target=ttyBridge.start, args=(ttyOptions.ttyBridge,)).start()
        except:
            print('Abort ttyBridge: ', sys.exc_info()[0])
            traceback.print_exc()

        time.sleep(1)

        # Start wsClient module
        try:            
            threading.Thread(target=wsClient.start, args=(ttyOptions.wsClient,)).start()
        except:
            print('Abort wsClient: ', sys.exc_info()[0])
            traceback.print_exc()
        
        time.sleep(1)
        
    except:
        print('Abort ttyNode: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()