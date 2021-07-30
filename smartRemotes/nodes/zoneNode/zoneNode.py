#############################################
##              zoneNode
#############################################
print('Load zoneNode.py')
    
import sys, os
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), './zones')
sys.path.append(path)

import time, traceback, queue, threading, asyncio, importlib
import wsClient, json, noteTool, zoneOptions

_zoneOptions = {}

##########################################
def getTask(keyCode, zone):
##########################################
    #print(f'Enter onSelectTask with {keyCode} in zone: {zone}')
    global _zoneOptions
    task = None

    _zoneOptions[zone].isTaskSet = None
    return _zoneOptions[zone].tasks.get(keyCode, None)
    
##########################################
def setFocus(controlWord, zone):
##########################################
    #print(f'Enter onSelectFocus with {keyCode} in zone {zone}')
    global _zoneOptions
    
    _zoneOptions[zone].isFocusSet = None
    
    controller = _zoneOptions[zone].focus.get(controlWord, None)
    if(controller == None): print(f'Abort setFocus, controller not found for controlWord: {controlWord}'); return

    _zoneOptions[zone].controller = controller
    print(f' \n***Focus Set to: {_zoneOptions[zone].controller}')
    
##########################################
def translateWord(controlWord, options):
##########################################
    try:
        if(controlWord == 'PowerToggle' and options.isFocusSet == True):
            controlWord = 'On'
            options.isOn = True
            options.isFocusSet = False
            print(f'controlWord translated to: {controlWord}')
            return controlWord
            
        if(controlWord == 'PowerToggle' and options.isFocusSet != True):
            controlWord = 'Off'
            options.isOn = False
            options.isFocusSet = False
            print(f'controlWord translated to: {controlWord}')
            return controlWord
            
        if(controlWord == 'Softer' and options.isFocusSet == True):
            controlWord = 'Sleep'
            options.isFocusSet = False
            print(f'controlWord translated to: {controlWord}')
            return controlWord
            
        if(controlWord == 'Louder' and options.isFocusSet == True):
            controlWord = 'Wake'
            options.isFocusSet = False
            print(f'controlWord translated to: {controlWord}')
            return controlWord
            
        if(controlWord == 'SoundToggle' and options.isFocusSet == True):
            controlWord = 'Dream'
            options.isFocusSet = False
            print(f'controlWord translated to: {controlWord}')
            return controlWord
 
        if(controlWord == 'SoundToggle' and options.isFocusSet == False):
            if(options.isSilent == True):
                controlWord = 'Sound'
                options.isSilent = False
            else:
                controlWord = 'Silence'
                options.isSilent = True
                
            print(f'controlWord translated to: {controlWord}')
            return controlWord
            
        return controlWord
    except:
        print('Abort translateNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def receivedNote(note, connection):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        global _zoneOptions
        
        #deviceCommands = translateNote(note)
        
        controlWord = note['content'].get('controlWord', None)
        zone = note['content'].get('zone', 'home')
        _zoneOptions[zone] = importlib.import_module(zone)
        options = _zoneOptions[zone]
        controlWord = translateWord(controlWord, options)
        
        if(options.isFocusSet): return setFocus(controlWord, zone)
        #if(options.isTaskSet): return getTask(controlWord, zone)      
        if(controlWord == 'Focus'): options.isFocusSet = True; print('Set Focus Flag'); return
    
        deviceCommands = options.commands[options.controller].get(controlWord, [])
                
        print(f'publish deviceCommands: {deviceCommands}')
        for index, deviceCommand in enumerate(deviceCommands):
            if(deviceCommand.get('device', None) == None): continue
            note = noteTool.publishNote('zoneNode', 'control ' + deviceCommand['device'] + ' request', deviceCommand)
            print(f'deliver deviceCommand({index}): {note}')
            await wsClient.deliverNote(note, connection)
    except:
        print('Abort receivedNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def hubConnected(connection):
#############################################
    try:
        print(f' \n***hubConnected')
        note = noteTool.publishNote('zoneNode', 'subscribe', {
            'title': 'control device request',
        })
        
        await wsClient.deliverNote(note, connection)
        
        print(f' \n***Wait for \'control device request\' notes...')
        print(f'*********************************************************')
    except:
        print('Abort hubConnected: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
#############################################
try:       
    time.sleep(3)
    
    #Start wsClient Module
    try:
        thread = threading.Thread(target=wsClient.start, args=(zoneOptions.wsClient,)).start()
    except:
        print('Abort run wsClient: ', sys.exc_info()[0])
        traceback.print_exc()
    
except:
    print('Abort zoneNode', sys.exc_info()[0])
    traceback.print_exc()

