#############################################
##               hogNode
#############################################
print('Load hogNode')

from multiprocessing import Process
from dbus.mainloop.glib import DBusGMainLoop
import os, sys, time, json, asyncio, traceback, queue, threading, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/bt-ble')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import wsClient, hogKeyboard, pairAgent, pairAdvertise, noteTool, hogOptions

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
        
        #validate hub
        if(_zones.get(zone, None) == None): _zones[zone] = importlib.import_module(zone)
        if(_zones.get(zone, None) == None): return print(f'Abort receivedNote, invalid zone: {zone}')

        # validate controlCommand
        controlCommand = _zones[zone].commands.get(controlWord, None)
      
        #controlCommand = options.controlWordMap.get(controlWord, None)
        if(controlCommand == None):
            return print(f'Abort receivedNote, invalid controlCommand for {controlWord}')
         
        options = controlCommand[0].get('options', None)
        if(options == 'reload'):
            print(f'reload options')        
            importlib.reload(_zones[zone])
              
        #controlCommand = [{"controlWord": "Menu", "hidCode": 0x40, "hidReport": 2}]              
        await hogKeyboard.receivedCommand(controlCommand[0])
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***hubConnected')
        
        filter = getattr(hogOptions, 'noteFilter', {'zone': 'livingRoom'})
       
        note = noteTool.publishNote('irNode', 'subscribe', {
            'title' : 'control hidNode request',
            'filter': filter
        })
        
        await wsClient.deliverPayload(hogOptions.wsClient['connection'], note)
        
        print(f' \n***Wait for \'{note["content"]["title"]}\' notes...')
        print(f'*********************************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
   
#############################################
def start():
#############################################
    print('Start hogNode')

    try:
        time.sleep(3)
        
        # Start hogKeyboard module
        try:            
            threading.Thread(target=hogKeyboard.start, args=(hogOptions.hidKeyboard,)).start()
        except:
            print('Abort hogKeyboard: ', sys.exc_info()[0])
            traceback.print_exc()

        time.sleep(1)
      
        # Start pairAgent module
        try:            
            threading.Thread(target=pairAgent.start, args=(hogOptions.pairAgent,)).start()
        except:
            print('Abort pairAgent: ', sys.exc_info()[0])
            traceback.print_exc()

        time.sleep(1)
        
        # Start pairAdvertise module
        try:            
            threading.Thread(target=pairAdvertise.start, args=(hogOptions.pairAdvertise,)).start()
        except:
            print('Abort pairAdvertise: ', sys.exc_info()[0])
            traceback.print_exc()

        time.sleep(1)
            
        # Start wsClient module
        try:            
            threading.Thread(target=wsClient.start, args=(hogOptions.wsClient,)).start()

            time.sleep(1)
        except:
            print('Abort btServer: ', sys.exc_info()[0])
            traceback.print_exc()           
    except:
        print('Abort hogNode: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()