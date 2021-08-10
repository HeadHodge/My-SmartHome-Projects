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

#############################################
async def receivedNote(note):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        global _zones
        
        # validate zone
        zone = note['content'].get('zone', None)
        if(zone == None):
            return print(f'Abort receivedNote, invalid zone: {zone}')
        
        # validate controlWord
        controlWord = note['content'].get('controlWord', None)
        if(controlWord == None):
            return print(f'Abort receivedNote, invalid controlWord: {ontrolWord}')
        
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

        await ttyBridge.deliverCommand(
            ttyOptions.ttyBridge['connection'],
            bytearray(keyDownCommand)
        )
        
        time.sleep(keyPressSecs)
        
        await ttyBridge.deliverCommand(
            ttyOptions.ttyBridge['connection'],
            bytearray(keyUpCommand)
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
            'title' : 'control ttyClient request',
            'filter': filter
        })
        
        await wsClient.deliverPayload(ttyOptions.wsClient['connection'], note)
        
        print(f' \n***Wait for \'{note["content"]["title"]}\' notes...')
        print(f'*********************************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
   
#############################################
def start():
#############################################
    print('Start ttyNode')

    try:
        time.sleep(3)
         
        # Start wsClient module
        try:            
            threading.Thread(target=wsClient.start, args=(ttyOptions.wsClient,)).start()
        except:
            print('Abort wsClient: ', sys.exc_info()[0])
            traceback.print_exc()
        
        time.sleep(1)
       
        # Start ttyBridge module
        try:            
            threading.Thread(target=ttyBridge.start, args=(ttyOptions.ttyBridge,)).start()
        except:
            print('Abort ttyBridge: ', sys.exc_info()[0])
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