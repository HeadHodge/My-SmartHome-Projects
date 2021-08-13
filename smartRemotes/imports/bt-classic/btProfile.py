"""
Bluetooth HID keyboard emulator DBUS Service

Original idea taken from:
http://yetanotherpointlesstechblog.blogspot.com/2016/04/emulating-bluetooth-keyboard-with.html

Moved to Python 3 and tested with BlueZ 5.43

create a bluetooth device to emulate a HID keyboard
"""
#############################################
##            MODULE VARIABLES
#############################################
print("Load btPairbt")
import os, sys, time, traceback, threading, asyncio

path = os.path.join(os.path.dirname(__file__), '../../imports/bluetooth')
sys.path.append(path)

import btleDevice

#############################################
def loadServiceRecord():
#############################################
    """
    Read and return SDP record from a file
    :return: (string) SDP record
    """
    try:
        currentDirectory  = os.path.dirname(os.path.realpath(__file__))
        filePath = os.path.join(currentDirectory, 'serviceRecord.xml')
        print(f'load serviceRecord: {filePath}')
        fh = open(filePath, 'r')
        return fh.read()
        fh.close()
    except OSError:
        sys.exit('Could not open the serviceRecord. Exiting...')

#############################################
def loadProfile(deviceName='smartRemotes Keypad'):
#############################################
    #Setup and register HID Profile
    print('loadProfile')

    profileUUID = '00001124-0000-1000-8000-00805f9b34fb'
    
    profileOptions = {
        'Name': 'smartRemotes Profile',
        'Role': 'server',
        'RequireAuthentication': False,
        'RequireAuthorization': False,
        'AutoConnect': True,
        'ServiceRecord': loadServiceRecord()
    }
    
    btDevice.registerProfile(profileUUID, profileOptions)
    
    btDevice.setPowered(True)
    btDevice.setAlias(deviceName)
    btDevice.setDiscoverable(True)
    btDevice.setDiscoverableTime(900)
    
    print(f'Device Name: "{btDevice.getAlias()}"')
    print(f'isDiscoverable: {btDevice.getDiscoverable()}')
    print(f'Discoverable Duration: {btDevice.getDiscoverableTime()}')
    print(f'Devce Powered: {btDevice.getPowered()}')

#############################################
def start(options={"controlPort": 17, "interruptPort": 19}):
#############################################
    print(f"Start btProfile")

    try:
        #print('register profile')
        loadProfile()
        
        # Start btOutput event loop
        print('start btPairbt eventLoop')
        asyncio.set_event_loop(asyncio.new_event_loop())
        loop = asyncio.get_event_loop()
        loop.run_forever()
    except:
        print('Abort btPairbt: ', sys.exc_info()[0])
        traceback.print_exc()
   
#############################################
##                MAIN
#############################################

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()