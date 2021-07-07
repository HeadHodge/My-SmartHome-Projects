#############################################
##            MODULES VARIABLES
#############################################
print('Load wsioClient')
import asyncio, threading, sys, time, traceback
import websockets, json

_parent = sys.modules["__main__"]

##########################
async def deliverPayload(payload, connection):
##########################
    try:
        #print(f' \n***deliverPayload: {payload}', connection)
        await connection.send(json.dumps(payload))
    except:
        print('Abort deliverPayload', sys.exc_info()[0])
        traceback.print_exc()
    
##########################
async def receivePayloads(options):
##########################
    print(f'receivePayloads...')
    #global _connection
    
    while True:
        try:            
            async with websockets.connect(options['endPoint']) as connection:
                print(f' \n***ENDPOINT CONNECTED, endpoint: {options["endPoint"]}')
                #_connection = connection
    
                options['connection'] = connection
                onConnect = getattr(_parent, options['onConnection'], None)
                onReceived = getattr(_parent, options['onReceived'], None)
                
                if(onConnect != None): await onConnect()
                
                async for payload in connection:
                    try:
                        #print(f' \n*** payload: {payload}')
                        if(onReceived != None): await onReceived(payload)
                    except:
                        print('Abort receiveNote', sys.exc_info()[0])
                        traceback.print_exc()
                    
                print(' \n***ENDPOINT DISCONNECTED')
        except:
            print('Abort receivePayloads', sys.exc_info()[0])
            traceback.print_exc()
       
##########################
def start(options={}):
##########################
    print('Start wsioClient')

    try:
        asyncio.set_event_loop(asyncio.new_event_loop())
        asyncio.get_event_loop().run_until_complete(receivePayloads(options))
    except:
        print('Abort: wsioClient', sys.exc_info()[0])
        traceback.print_exc()

##########################
#         MAIN
##########################
# Run this module on main thread to unit test with following code
if __name__ == '__main__':
    pass