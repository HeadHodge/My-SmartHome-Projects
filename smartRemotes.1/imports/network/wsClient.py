#############################################
##            MODULES VARIABLES
#############################################
print('Load wsClient')
import asyncio, threading, sys, time, traceback
import websockets, json, noteTool

_parent = sys.modules["__main__"]
_options = {}
#_connection = None

##########################
async def deliverNote(note, connection):
##########################
    try:
        print(f' \n***deliverNote note: {note}')
        await connection.send(noteTool.object2serial(note))
    except:
        print('Abort deliverNote', sys.exc_info()[0])
        traceback.print_exc()
    
##########################
async def receiveNotes():
##########################
    print(f'receiveNotes...')
    #global _connection
    
    while True:
        try:            
            async with websockets.connect(_options['hubNode']) as connection:
                print(f' \n***HUB CONNECTED, endpoint: {_options["hubNode"]}')
                #_connection = connection
    
                await _parent.hubConnected(connection)
                    
                async for note in connection:
                    try:            
                        #print(f' \n*** note: {note}')
                        #thread = threading.Thread(target=_parent.receivedNote, args=(noteTool.serial2object(note), connection))
                        #thread.start()
                        await _parent.receivedNote(noteTool.serial2object(note), connection)
                    except:
                        print('Abort receiveNote', sys.exc_info()[0])
                        traceback.print_exc()
                    
                print(' \n***HUB DISCONNECTED')
        except:
            print('Abort receiveNotes', sys.exc_info()[0])
            traceback.print_exc()
       
##########################
def start(options={}):
##########################
    print('Start wsClient')
    global _options

    try:
        _options = options
        asyncio.set_event_loop(asyncio.new_event_loop())
        asyncio.get_event_loop().run_until_complete(receiveNotes())
    except:
        print('Abort: wsClient', sys.exc_info()[0])
        traceback.print_exc()

##########################
#         MAIN
##########################
# Run this module on main thread to unit test with following code
if __name__ == '__main__':
    pass