####################################
##            MODULE VARIABLES
####################################
print('Load wsServer')

import sys, time, json, traceback, asyncio
import noteTool
from aiohttp import web

_parent = sys.modules["__main__"]
_options = {}

#############################################
async def deliverNote(note, connection):
#############################################
    #print('receivedNote')
    await connection.send_str(noteTool.object2serial(note))

#############################################
async def receiveNotes(request):
#############################################
    global _options

    connection = web.WebSocketResponse()
    #connection.deliverNote = deliverNote
    await connection.prepare(request)
    print(f' \n***CONNECTED: wsServer on port: {_options["port"]}')

    async for message in connection:
        try:        
            note = noteTool.serial2object(message.data)
            await _parent.receivedNote(note, connection)
        except:
            print('Abort receiveNote', sys.exc_info()[0])
            traceback.print_exc()

    print(' \n***CLOSED: wsServer')
    #return connection
    
#############################################
def start(options={"port": 8080}):
#############################################
    print('Start wsServer')
    global _options
    
    _options = options
    
    asyncio.set_event_loop(asyncio.new_event_loop())
    while True:
        try:        
            app = web.Application()
            app.add_routes([web.get('/', receiveNotes)])
            web.run_app(app, port=options['port'], handle_signals=False)
        except:
            print('Abort start wsServer', sys.exc_info()[0])
            traceback.print_exc()
   
#######################################
#              MAIN
#######################################

if __name__ == '__main__':
    start()