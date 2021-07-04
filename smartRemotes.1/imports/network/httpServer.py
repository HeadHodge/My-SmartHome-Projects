####################################
##            MODULE VARIABLES
#############################################
print('Load httpServer')
import sys, time, json, traceback, asyncio
from aiohttp import web

def start(port=80):
    print('Start httpServer')
    
    try:
        asyncio.set_event_loop(asyncio.new_event_loop())
        app = web.Application()
        app.add_routes([web.static('/', '/smartRemotes/nodes/htmlNode')])
        web.run_app(app, port=port, handle_signals=False)
    except:
        print('Abort httpServer: ', sys.exc_info()[0])
        traceback.print_exc()
        
#######################################
#              MAIN
#######################################

if __name__ == '__main__':
    start()