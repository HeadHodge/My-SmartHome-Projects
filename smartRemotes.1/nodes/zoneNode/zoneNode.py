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
#_zone = 'livingRoom'
#if(len(sys.argv) > 0): _zone = sys.argv[1]
'''
_zones = {}
_focus = 'Up'
_primaryController = {}
_primaryModule = None
_isFocusSet = None
_isTaskSet = None
_isSilent = None
_eventNum = 4
'''

##########################################
def getTask(keyCode, zone):
##########################################
    #print(f'Enter onSelectTask with {keyCode} in zone: {zone}')
    task = None

    _zones[zone].isTaskSet = None
    return _zones[zone].tasks.get(keyCode, None)
    
##########################################
def setFocus(keyCode, zone):
##########################################
    #print(f'Enter onSelectFocus with {keyCode} in zone {zone}')

    _zones[zone].isFocusSet = None
    
    if(keyCode == 'Menu'):
        _zones[zone].isTaskSet = True
        print(f'select task for {zone}')
        return
    
    if(keyCode == 'Ok'):
        print(f'reload selected for {zone}')
        return 'RELOAD'

    def Home()        : return _zones[zone].controllers.get('Home', None)
    def Louder()      : return _zones[zone].controllers.get('Louder', None)
    def Softer()      : return _zones[zone].controllers.get('Softer', None)
    def SoundToggle() : return _zones[zone].controllers.get('SoundToggle', None)
    def Backward()    : return _zones[zone].controllers.get('Backward', None)
    def PlayToggle()  : return _zones[zone].controllers.get('PlayToggle', None)
    def Forward()     : return _zones[zone].controllers.get('Forward', None)

    case = {
        'Home'       : Home,
        'Louder'     : Louder,
        'Softer'     : Softer,
        'SoundToggle': SoundToggle,
        'Backward'   : Backward,
        'PlayToggle' : PlayToggle,
        'Forward'    : Forward
    }
    
    selection = case.get(keyCode, None)
    if(selection == None): return
    _zones[zone].primaryModule = selection()
    print(f'new controller selected: {_zones[zone].primaryModule}')
    
##########################################
def transformWord(controlWord, options):
##########################################
    try:
        if(controlWord == 'PowerToggle'):
            if(options.isOn == True):
                controlWord = 'Off'
                options.isOn = False
            else:
                controlWord = 'On'
                options.isOn = True
                
            return controlWord

        if(controlWord == 'SoundToggle'):
            if(options.isSilent == True):
                controlWord = 'Sound'
                options.isSilent = False
            else:
                controlWord = 'Silence'
                options.isSilent = True
                
            return controlWord
            
        return controlWord
    except:
        print('Abort translateNote: ', sys.exc_info()[0])
        traceback.print_exc()
        return transformWord

#############################################
def translateNote(note):
#############################################
    try:
        controlWord = note['content'].get('controlWord', None)
        zone = note['content'].get('zone', 'home')
        print(f'translateNote controlWord: {controlWord}, zone: {zone}')

        #validate zone
        if(_zoneOptions.get(zone, None) == None): _zoneOptions[zone] = importlib.import_module(zone)
        if(_zoneOptions.get(zone, None) == None): return print(f'Abort translateNote, invalid zone: {zone}')
        options = _zoneOptions[zone]
        controlWord = transformWord(controlWord, options)
        
        if(options.isFocusSet): return setFocus(controlWord, zone)
        if(options.isTaskSet): return getTask(controlWord, zone)
      
        if(controlWord == 'Focus'): options.isFocusSet = True; return print('Set Focus Flag')
    
        return options.commands[options.controller].get(controlWord, [])
    except:
        print('Abort translateNote: ', sys.exc_info()[0])
        traceback.print_exc()

#############################################
async def receivedNote(note, connection):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        
        deviceCommands = translateNote(note)
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

