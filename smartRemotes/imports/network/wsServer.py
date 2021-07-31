#############################################
##            MODULE VARIABLES
#############################################
print('Load wsServer')
import sys, time, json, asyncio, traceback
import websockets, noteTool

_parent = sys.modules["__main__"]
_options = None

#######################################
async def deliverPayload(connection, payload):
#######################################
    #try:    
    #print('deliverPayload: ', payload)
    await connection.send(noteTool.object2serial(payload))
    #except:
    #    print('Abort deliverPayload', sys.exc_info()[0])
    #    traceback.print_exc()
       
#######################################
async def onConnect(connection, path):
#######################################
    print(f'*** Connected client on wsServer')

    #await connection.send('{"format": "greeting", "greeting": "Hello?", "from": ""}')

    async for data in connection:
        try:        
            payload = noteTool.serial2object(data)
            if(hasattr(_parent, 'receivedPayload')): await _parent.receivedPayload(connection, payload)
        except:
            print('Abort onConnect', sys.exc_info()[0])
            traceback.print_exc()
 
#######################################
def start(options):
#######################################
    print('Start wsServer')
    
    while(True):
        try:
            print(f'*** Start wsServer on address: {options["address"]}, port: {options["port"]}')
            asyncio.set_event_loop(asyncio.new_event_loop())
            server = websockets.serve(onConnect, options["address"], options["port"])
            asyncio.get_event_loop().run_until_complete(server)

            print(f'*** Wait for client connections')
            asyncio.get_event_loop().run_forever()

        except:
            print('Abort start', sys.exc_info()[0])
            traceback.print_exc()

#######################################
#              MAIN
#######################################
# Run this module on main thread to unit test with following code

if __name__ == "__main__":
    options = {
        "endpoint": "ws://192.168.0.164:8080",
        "address": "192.168.0.164",
        "port": "8080",
        "path": "/",
        "queue": None,
        "onEvent": None
        }
        
    start(options)