#############################################
##            htmlNode
#############################################
print('Load htmlNode.py')

import os, sys, time, json, asyncio, traceback, queue, threading, importlib

path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)

import httpServer
   
#############################################
def start(options={"controlPort": 17, "interruptPort": 19}):
#############################################
    try:
        # Start httpServer Module
        try:
            threading.Thread(target=httpServer.start).start()
            time.sleep(1)
        except:
            print('Abort httpServer: ', sys.exc_info()[0])
            traceback.print_exc()

    except:
        print('Abort htmlNode: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()
