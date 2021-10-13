#############################################
##               irBridge
#############################################
print('Load irBridge')

import os, sys, time, json, threading, traceback, asyncio
import broadlink

_parent = sys.modules["__main__"]
        
##########################
async def deliverCommand(connection, command):
##########################
    try:
        #print(f'***deliverCommand: {command}')
        connection.send_data(command)
    except:
        print('Abort deliverCommand', sys.exc_info()[0])
        traceback.print_exc()
        
###################
#     start
###################
def start(options={}):
    try:    
        print(f'Start irBridge')

        deviceDetails = options.get('deviceDetails', None)
        
        if(deviceDetails == None):
            print('Abort irBridge: no deviceDetails provided')
            return
            
        #devices = broadlink.discover(timeout=5)
        device = broadlink.gendevice(*deviceDetails)
        device.auth()    
        print(f"device available: {device}")
        
        onConnect = options.get('onConnect', None)
        if(onConnect != None): asyncio.run(onConnect(device))
    except:
        print('Abort irBridge', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
##Open server to listen for control input
#############################################

if __name__ == "__main__":
# Run this module on main thread to unit test with following code

    start()
