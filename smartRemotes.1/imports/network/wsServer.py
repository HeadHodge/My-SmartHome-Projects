#############################################
##            MODULE VARIABLES
#############################################
print('Load wsServer')
import sys, time, json, websockets
import asyncio, traceback

_options = None

#######################################
async def onInput(connection, note):
#######################################
    print('received note: ', note)
    try:    
        #reply = await _options['onEvent']('note', note)
        #print('reply: ', reply)
        #if(reply == None): return
    
        await connection.send('{"format": "reply", "reply": "Got It"}')
    except:
        print('Abort onInput', sys.exc_info()[0])
        traceback.print_exc()
       
#######################################
async def onConnect(connection, path):
#######################################
    print('wsServer Connected')

    #await connection.send('{"format": "greeting", "greeting": "Hello?", "from": ""}')
        
    async for post in connection:
        await onInput(connection, post)
 
#######################################
def start(options):
#######################################
    print('Start wsServer')
    global _options
    
    try:
        _options = options
        
        start_server = websockets.serve(onConnect, options["address"], options["port"])
        asyncio.get_event_loop().run_until_complete(start_server)

        print(f'wait for connections on address: {options["address"]}, port: {options["port"]}')
        asyncio.get_event_loop().run_forever()

    except:
        print('Abort wsServer.py', sys.exc_info()[0])
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