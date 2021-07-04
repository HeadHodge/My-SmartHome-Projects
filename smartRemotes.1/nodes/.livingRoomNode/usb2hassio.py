#############################################
##            MODULE VARIABLES
#############################################
print('Load usb2hassio')
    
import os, sys, time, json, traceback, queue, threading, asyncio
if len(sys.argv) < 3: 
    print('Terminate usb2hassio, missing required zone name and/or event list arguments')
    print('Example: python3 usb2hassio.py masterBedroom 3,4,5,6')
    sys.exit()

path = os.path.join(os.path.dirname(__file__), '../../imports/usb')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/maps')
sys.path.append(path)
path = os.path.join(os.path.dirname(__file__), '../../imports/maps/map2hassio.py')
sys.path.append(path)
import wsClient, usbServer, keyMaps, map2hassio

_ioQueue = queue.Queue()
_sessionId = 0

# keyCode formatted Input
_inOptions = {
    "zone": sys.argv[1],
    "channels": sys.argv[2].split(','),
    "firstPost": "User",
    "userEvent": None,
    "agentEvent": None
   }

# hassio service events Output
_outOptions = {
    "endpoint": "ws://192.168.0.160:8123/api/websocket",
    "address": "192.168.0.160",
    "port": "8123",
    "path": "/api/websocket",
    "firstPost": "User",
    "userEvent": None,
    "agentEvent": None
   }
 
#############################################
async def inPosts(post):
#############################################
    print(f' \n***inPosts: {post}')
    global _ioQueue, _sessionId
    
    usbNum = post.get('scanCode', 0)
    zone = post.get('zone', 'home')
    keyCode = keyMaps.usbNum2keyCode(usbNum)
    if(keyCode == None): print(f'Abort inPosts, invalid keyCode: {keyCode}'); return

    print(f' \n***TRANSLATE: {keyCode}')
    hassioSequence = map2hassio.keyCode2hassio(keyCode, zone)
    if(hassioSequence == None): return
    
    for task in hassioSequence:
        key = list(task.keys())[0]
        data = task[key]
        command = key.split('/')
        
        if(command[0] == 'sleep'):
            payload = {
                "id": 0, 
                "type": "sleep",	
                "service_data": data
            }
            
            _ioQueue.put(payload)
            continue
            
        _sessionId += 1
        
        payload = {
            "id": _sessionId, 
            "type": "call_service",	
            "domain": command[0],
            "service": command[1],
            "service_data": data
        }
        
        print(f' \n***outTRANSFER: {payload}')
        await _outOptions['transfer'](payload, _outOptions)
        
#############################################
async def outPosts(post, options):
#############################################
    print(f' \n***outUSER: {post}')
    print(f' \n***********************************************************************')
    print(f'***inUSER WAIT')

    content = json.loads(post)
    if(content['type'] == "auth_required"):
        payload = {
            "type": "auth",
            "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1NmVhNzU3ODkzMDE0MTMzOTJhOTZiYmY3MTZiOWYyOCIsImlhdCI6MTYxNDc1NzQ2OSwiZXhwIjoxOTMwMTE3NDY5fQ.K2WwAh_9OjXZP5ciIcJ4lXYiLcSgLGrC6AgTPeIp8BY"
        }
        
        print(f' \n***outTRANSFER: {payload}')
        #asyncio.run(options['transfer'](payload, options))
        await options['transfer'](payload, options)
    
#############################################
##                MAIN
#############################################
try:
    # Start wsServer Module
    try:
        _inOptions['userEvent'] = inPosts
        usbServer = threading.Thread(target=usbServer.start, args=(_inOptions,))
        usbServer.start()
    except:
        print('Abort run usbServer: ', sys.exc_info()[0])
        traceback.print_exc()

    # Start wsClient Module
    try:
        _outOptions['userEvent'] = outPosts
        #_outOptions['agentEvent'] = outAgentEvent
        wsClient = threading.Thread(target=wsClient.start, args=(_outOptions,))
        wsClient.start()
    except:
        print('Abort run wsClient: ', sys.exc_info()[0])
        traceback.print_exc()
except:
    print('Abort key2hassio', sys.exc_info()[0])
    traceback.print_exc()

