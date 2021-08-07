#############################################
##               irBridge
#############################################
print('Load irBridge')

import os, sys, time, json, threading, traceback, asyncio

_parent = sys.modules["__main__"]
        
##########################
async def deliverCommand(command):
##########################
    try:
        print(f'***deliverCommand: ir-ctl --send={command}')
        reply = os.system(f'ir-ctl --send={command}')
        print(f'reply: {reply}')
    except:
        print('Abort deliverCommand', sys.exc_info()[0])
        traceback.print_exc()
        
###################
#     start
###################
def start(options={}):
    try:    
        print(f'Start irBridge: {options}')
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
