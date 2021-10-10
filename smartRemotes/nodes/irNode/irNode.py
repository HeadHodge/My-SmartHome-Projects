#############################################
##               irNode
#############################################
print('Load irNode')

from multiprocessing import Process
from dbus.mainloop.glib import DBusGMainLoop
import os, sys, time, json, asyncio, traceback, queue, threading, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/usb')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import wsClient, irBridge, irOptions, noteTool

_zones = {}

#############################################
async def receivedNote(note):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        global _zones
        
        # validate zone
        zone = note['content'].get('zone', None)
        if(zone == None): return print(f'Abort receivedNote, invalid zone: {zone}')
        
        # validate controlWord
        controlWord = note['content'].get('controlWord', None)
        if(controlWord == None): return print(f'Abort receivedNote, invalid controlWord: {controlWord}')
        
        # validate controlMap
        if(_zones.get(zone, None) == None): _zones[zone] = importlib.import_module(zone)
        if(_zones.get(zone, None) == None): return print(f'Abort receivedNote, invalid zone: {zone}')

        # validate commands
        commands = _zones[zone].wordMap.get(controlWord, None)
        if(commands == None): return print(f'Abort commands, invalid commands for {controlWord}')

        print(f'send irCommand: {controlWord}')
        await irBridge.deliverCommand(irOptions.irBridge['connection'], commands[0]["irCommand"])
        
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***hubConnected')
        
        filter = getattr(irOptions, 'noteFilter', {})
       
        note = noteTool.publishNote('irNode', 'subscribe', {
            'title' : 'control irNode request',
            'filter': filter
        })
        
        await wsClient.deliverPayload(irOptions.wsClient['connection'], note)
        
        print(f' \n***Wait for \'{note["content"]["title"]}\' notes...')
        print(f'*********************************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def bridgeConnected(connection):
#############################################
    try:
        print(f' \n***bridgeConneced')
        
        irOptions.irBridge['connection'] = connection
    except:
        print('Abort bridgeConneced: ', sys.exc_info()[0])
        traceback.print_exc()
   
#############################################
def start():
#############################################
    print('Start irNode')

    try:
        time.sleep(3)
         
        # Start wsClient module
        try:            
            threading.Thread(target=wsClient.start, args=(irOptions.wsClient,)).start()
        except:
            print('Abort wsClient: ', sys.exc_info()[0])
            traceback.print_exc()
        
        time.sleep(3)
       
        # Start irBridge module
        try:            
            irOptions.irBridge['onConnect'] = bridgeConnected
            threading.Thread(target=irBridge.start, args=(irOptions.irBridge,)).start()
        except:
            print('Abort irBridge: ', sys.exc_info()[0])
            traceback.print_exc()
        
        time.sleep(1)
    except:
        print('Abort irNode: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()