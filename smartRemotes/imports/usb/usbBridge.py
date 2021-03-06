#############################################
##              usbBridge
#############################################
print('Load usbBridge')

from evdev import InputDevice, categorize, ecodes
import sys, time, json, threading, traceback, asyncio
import noteTool

_parent = sys.modules["__main__"]
        
############################
def captureInput(zone, channel):
############################
    print(f'captureInput on channel: {channel} in zone: {zone}')
    
    lastCode = 0
    lastTime = time.time()
    device = InputDevice(f'/dev/input/event{channel}')
    device.grab()
    postTime = 0
        
    startTime = 0
    startCode = 0
            
    for event in device.async_read_loop():
        try:
            if(event.type != 1 or event.value != 1): continue
            currentTime = event.sec + event.usec/1000000            
            keyEvent = categorize(event)
            if(keyEvent.scancode == startCode and currentTime - startTime < .50): continue
            
            startCode = keyEvent.scancode
            startTime = currentTime
            
            #print(keyEvent)
            
            note = _parent.noteTool.publishNote('usbBridge', 'captured usbInput', {
                "keyCode" : keyEvent.keycode,
                "scanCode": keyEvent.scancode,
                "channel" : channel,
                "device"  : device.name,
                "zone"    : zone,
                "time"    : int(time.time() * 1000)
            })
            
            asyncio.run(_parent.receivedNote(note))
            #_parent.receivedNote(note)
        except:
            print(f'Abort captureInput: {sys.exc_info()[0]}')
            traceback.print_exc()

###################
#     start
###################
def start(options={}):
    print('Start usbBridge')

    try:
        time.sleep(3)
        print(' \n***Grab USB Devices:')
    
        zone = options.get('zone','home')
        channels = options.get('channels', '0,1,2,3').split(',')
        
        for channel in channels:
            threading.Thread(target=captureInput, args=(zone, channel,)).start()
        
        time.sleep(1)
        print(' \n========================================================')

    except:
        print('Abort usbBridge', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
##Open server to listen for control input
#############################################

if __name__ == "__main__":
# Run this module on main thread to unit test with following code

    start()
