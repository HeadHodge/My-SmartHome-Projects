#############################################
##            usbNode
#############################################
print('Load usbNode')
    
import os, sys, time, json, traceback, queue, threading, asyncio

path = os.path.join(os.path.dirname(__file__), '../../imports/usb')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)

import wsClient, usbServer, usbOptions, noteTool

# Global Variables
_zone = 'home'
_channels = []
_wsConnection = None
   
#############################################
async def receivedNote(note):
#############################################
    print(f' \n***receivedNote: {note}')
    
    scanCode = note['content']['scanCode']
    scanCodeMap = usbOptions.scanCodeMap.get(scanCode, None)
    if(scanCodeMap == None): return print(f'Abort receivedNote, invalid scanCode: {scanCode}')

    note = noteTool.publishNote('usbNode', 'control device request', {'scanCode': scanCode, 'controlWord': scanCodeMap["controlWord"], 'zone': _zone})
    #print(f'deliver note: {note}')
    await wsClient.deliverNote(note, _wsConnection)

#############################################
async def hubConnected(connection):
#############################################
    try:
        print(f' \n***hubConnected')
        global _wsConnection
        
        _wsConnection = connection
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
#############################################
try:
    time.sleep(3)

    # Initialize globals from cmd line
    if len(sys.argv) < 3: 
        print('Terminate usbNode, missing required zone name and/or event list arguments')
        print('Example: python3 usb2hassio.py masterBedroom 3,4,5,6')
        print('Run "python3 -m evdev.evtest" to list valid devices')
        sys.exit()
        
    _zone = sys.argv[1]
    _channels = sys.argv[2].split(',')
    
    time.sleep(1)
    
    # Start usbServer Module
    try:
        threading.Thread(target=usbServer.start).start()
    except:
        print('Abort usbServer: ', sys.exc_info()[0])
        traceback.print_exc()

    time.sleep(1)
    
    # Start wsClient Module
    try:
        threading.Thread(target=wsClient.start, args=(usbOptions.wsClient,)).start()
    except:
        print('Abort wsClient: ', sys.exc_info()[0])
        traceback.print_exc()

except:
    print('Abort usbNode', sys.exc_info()[0])
    traceback.print_exc()

