#############################################
##            MODULE VARIABLES
#############################################
print('Load routeNote')

from multiprocessing import Process
from dbus.mainloop.glib import DBusGMainLoop
import os, sys

path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)

#import usbServer, gattServer, httpServer, wsServer, wsClient, btDevice, btProfile, keyMaps, map2hassio
import time, json, asyncio, traceback, queue, threading, importlib
import httpServer, wsServer, noteTool, hubOptions

_titles = {}
    
##########################
def subscribeNote(note, connection=None):
##########################
    title = note["content"].get("title", None)
    filter = note["content"].get("filter", {})
    time = note.get("time", None)
    
    print(f' \n***subscribeNote title: \'{title}\', time: \'{time}\', filter: {filter}')
    
    if(_titles.get(title, None) == None): _titles[title] = {}
    _titles[title][str(time)] = {'noteFilter': filter, 'connection': connection}
    print(f'\'{title}\': subscribed')      
    
#############################################
async def receivedPayload(connection, note):
#############################################
    try:
        print(f' \n***receivedNote: {note}')
        title = note.get("title", 'untitled')
        
        #send receipt to client
        if(note.get('deliverReceipt', False) == True or (hasattr('hubOptions', 'deliverReceipts') == True and hubOptions.deliverReceipts == True)):
            receipt = noteTool.publishNote('hubNode', 'note receipt', {'state': 'received', 'title': note['title'], 'time': note['time']})
            await wsServer.deliverPayload(hubOptions.wsServer['connection'], receipt)
        
        #subscribe note
        if(note['title'].casefold() == 'subscribe'):
            return subscribeNote(note, connection)
        
        #deliver note to subscribers
        subscribers = _titles.get(title, {})
        unsubscribeList = []
        print(f'***deliver note title: \'{title}\' to {len(subscribers)} subscriber(s)')
                
        for index, subscriber in enumerate(subscribers):
            try:
                filter = subscribers[subscriber]["noteFilter"]
                connection = subscribers[subscriber]["connection"]
                print(f'deliver to subscriber{index}: {subscriber}')
                print(f'apply note filter: {filter}')
                isValid = True
                
                for key in filter:
                    filterVal = filter[key]
                    noteVal = note["content"].get(key, None)
                    
                    if(noteVal.casefold()!= filterVal.casefold()):
                        print(f'Test failed \'{key}\': \'{noteVal}\' invalid')
                        isValid = False
                        break

                if(isValid == False): continue
                await wsServer.deliverPayload(connection, note)                
                print(f'***note delivered: \'{title}\'')
            except:
                #_titles[title].pop(subscriber)
                #traceback.print_exc()
                print(f'***note undelivered: ', sys.exc_info()[0])
                unsubscribeList.append(subscriber)
                
        #unsubscribe disconnected subscribers
        for subscriber in unsubscribeList:
            print(f'***Unsubscribe disconnected subscriber: {subscriber}')
            _titles[title].pop(subscriber)
        
        print(f'===============================================\n')
    except:
        print('Abort receiveNote: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
def start():
#############################################
    try:     
        #Start wsServer Module
        time.sleep(3)
  
        try:
            threading.Thread(target=wsServer.start, args=(hubOptions.wsServer,)).start()
        except:
            print('Abort wsServer: ', sys.exc_info()[0])
            traceback.print_exc()
    except:
        print('Abort hubNode: ', sys.exc_info()[0])
        traceback.print_exc()
  
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':

    start()
