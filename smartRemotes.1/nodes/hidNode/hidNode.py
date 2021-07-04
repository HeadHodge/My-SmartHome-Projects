#############################################
##            gattNode
#############################################
print('Load gattNode')

from multiprocessing import Process
from dbus.mainloop.glib import DBusGMainLoop
import os, sys, time, json, asyncio, traceback, queue, threading, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/ble')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import wsClient, noteTool, hidServer, hidOptions

_zones = {}

#############################################
async def receivedNote(note, connection):
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
        
        #validate hub
        if(_zones.get(zone, None) == None): _zones[zone] = importlib.import_module(zone)
        if(_zones.get(zone, None) == None): return print(f'Abort receivedNote, invalid zone: {zone}')

        # validate controlCommand
        controlCommand = _zones[zone].commands.get(controlWord, None)
        print(f'controlWord: {controlCommand}')
       
        #controlCommand = options.controlWordMap.get(controlWord, None)
        if(controlCommand == None):
            return print(f'Abort receivedNote, invalid controlCommand for {controlWord}')
         
        
        
        
        
        
        controlCommand = {"controlWord": "Menu", "hidCode": 0x40, "hidReport": 2}               
        await hidServer.receivedCommand(controlCommand)
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hubConnected(connection):
#############################################
    print(f' \n***hubConnected')
    note = noteTool.publishNote('gattNode', 'subscribe', {
        'title': 'control hidClient request'
    })
    
    await wsClient.deliverNote(note, connection)
    
    print(f' \n***Wait for \'{note["content"]["title"]}\' notes...')
    print(f'*********************************************************')
   
#############################################
def start():
#############################################
    print('Start gattNode')

    try:
        '''
        # Validate process args
        if len(sys.argv) < 3: 
            print('Terminate usb2hassio, missing required zone name and/or event list arguments')
            print('Example: python3 usb2hassio.py masterBedroom 3,4,5,6')
            sys.exit()
        '''
        
        time.sleep(3)
        
        # Start gattServer module
        try:            
            #_btControlOptions['userEvent'] = btControlEvent
            #_btControlOptions['channel'] = options["controlPort"]
            #threading.Thread(target=btServer.start, args=(_btControlOptions,)).start()

            #_btTransferOptions['userEvent'] = btTransferEvent
            #_btTransferOptions['channel'] = options["interruptPort"]
            #threading.Thread(target=btServer.start, args=(_btTransferOptions,)).start()
            
            threading.Thread(target=hidServer.start, args=(hidOptions.hidServer,)).start()
        except:
            print('Abort btServer: ', sys.exc_info()[0])
            traceback.print_exc()

        time.sleep(1)
        
        # Start wsClient module
        try:            
            threading.Thread(target=wsClient.start, args=(hidOptions.wsClient,)).start()

            time.sleep(1)
        except:
            print('Abort btServer: ', sys.exc_info()[0])
            traceback.print_exc()           
    except:
        print('Abort gattNode: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()