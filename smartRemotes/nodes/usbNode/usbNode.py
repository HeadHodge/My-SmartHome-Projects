#############################################
##            usbNode
#############################################
print('Load usbNode')
    
import os, sys, time, json, traceback, queue, threading, asyncio

path = os.path.join(os.path.dirname(__file__), '../../imports/usb')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)

import wsClient, usbBridge, usbOptions, noteTool

#############################################
async def receivedNote(note):
#############################################
    print(f' \n***receivedNote: {note}')
    
    scanCode = note['content']['scanCode']
    scanCodeMap = usbOptions.scanCodeMap.get(scanCode, None)
    if(scanCodeMap == None): return print(f'Abort receivedNote, invalid scanCode: {scanCode}')

    note = noteTool.publishNote('usbNode', 'control device request', {
        'scanCode': scanCode,
        'controlWord': scanCodeMap["controlWord"],
        'zone': usbOptions.usbBridge['zone'],
    })

    await wsClient.deliverPayload(usbOptions.wsClient['connection'], note)

#############################################
async def hubConnected():
#############################################
    try:
        print(f' \n***hubConnected')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
#############################################
try:
    time.sleep(3)
   
    # Start usbBridge Module
    try:
        threading.Thread(target=usbBridge.start, args=(usbOptions.usbBridge,)).start()
    except:
        print('Abort usbBridge: ', sys.exc_info()[0])
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

