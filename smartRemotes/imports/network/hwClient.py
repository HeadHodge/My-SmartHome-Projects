#############################################
##            MODULES VARIABLES
#############################################
print('Load wsClient')
import asyncio, threading, sys, time, traceback
import websockets, json, noteTool

_server = None

##########################
async def deliverNote(note):
##########################
    try:
        print(f' \n***deliverNote note: {note}')
        await _server.send(noteTool.object2serial(note))
    except:
        print('Abort deliverNote', sys.exc_info()[0])
        traceback.print_exc()
    
##########################
async def connectServer(options):
##########################
    print(f'connectServer...')
    global _server
    
    while True:
        try:            
            async with websockets.connect(options['hubNode']) as server:
                try:            
                    print(f' \n***SERVER CONNECTED, endpoint: {options["hubNode"]}')
                    _server = server
    
                    if(options.get('serverConnected', None) != None):
                        await options['serverConnected'](server, deliverNote)
                    
                    async for receivedNote in server:
                        print(f' \n*** receivedNote: {receivedNote}')
                    
                        if(options.get('receivedNote', None) != None): 
                            await options['receivedNote'](noteTool.serial2object(receivedNote), deliverNote)
                except:
                    print('Abort connectedServer', sys.exc_info()[0])
                    traceback.print_exc()
                    
            print(' \n***SERVER DISCONNECTED')
        except:
            print('Abort connectServer', sys.exc_info()[0])
            traceback.print_exc()
       
##########################
def start(options={"hubNode": "ws://192.168.0.222:8080/"}):
##########################
    print('Start wsClient')

    try:
        asyncio.set_event_loop(asyncio.new_event_loop())
        asyncio.get_event_loop().run_until_complete(connectServer(options))
    except:
        print('Abort: wsClient', sys.exc_info()[0])
        traceback.print_exc()

##########################
#         MAIN
##########################
# Run this module on main thread to unit test with following code
if __name__ == '__main__':
    pass