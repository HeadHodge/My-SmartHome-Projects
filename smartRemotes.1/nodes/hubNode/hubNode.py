#############################################
##            MODULE VARIABLES
#############################################
print('Load routeNote')

from multiprocessing import Process
from dbus.mainloop.glib import DBusGMainLoop
import os, sys

path = os.path.join(os.path.dirname(__file__), '../../imports/network')
sys.path.append(path)

#import usbServer, gattServer, httpServer, wsioServer, wsClient, btDevice, btProfile, keyMaps, map2hassio
import time, json, asyncio, traceback, queue, threading, importlib
import httpServer, wsioServer, noteTool, hubOptions

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
async def receivedNote(note, connection=None):
#############################################
    try:
        print(f' \n***receiveNote: {note}')
        title = note.get("title", 'untitled')
        
        #send receipt to client
        if(note.get('deliverReceipt', False) == True or (hasattr('hubOptions', 'deliverReceipts') == True and hubOptions.deliverReceipts == True)):
            receipt = noteTool.publishNote('hubNode', 'note receipt', {'state': 'received', 'title': note['title'], 'time': note['time']})
            await wsioServer.deliverNote(receipt, connection)
        
        #subscribe note
        if(note['title'].casefold() == 'subscribe'):
            return subscribeNote(note, connection)
        
        #deliver note to subscribers
        subscribers = _titles.get(title, {})
        unsubscribeList = []
        print(f'***deliver note title: \'{title}\' to {len(subscribers)} subscriber(s)')
                
        for index, subscriber in enumerate(subscribers):
            try:
                #if(subscriber == None): continue
                filter = subscribers[subscriber]["noteFilter"]
                connection = subscribers[subscriber]["connection"]
                #receiveNote = subscribers[subscriber]["receiveNote"]
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
                await wsioServer.deliverNote(note, connection)                
                print(f'***note delivered: \'{title}\'')
            except:
                #_titles[title].pop(subscriber)
                traceback.print_exc()
                unsubscribeList.append(subscriber)
                print(f'***note undelivered: ', sys.exc_info()[0])
                
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
        '''
        #start local nodes
        try:
            for node in options.localNodes:
                print(f'subscribe to: {node["subscribe"]}')
                path = os.path.join(os.path.dirname(__file__), '../' + node['name'])
                sys.path.append(path)
                module = importlib.import_module(node['name'])
                class clientInfo: pass
                setattr(clientInfo, 'send_obj', receiveNote)
                note = noteTool.publishNote('hubNode', 'subscribe', {
                    'title': node["subscribe"],
                    'filter': node["filter"]
                })
                
                subscribeNote(note, module.receiveNote, clientInfo)
                time.sleep(3)
        except:
            print('Abort add local subscribers: ', sys.exc_info()[0])
            traceback.print_exc()
        '''
     
        #Start wsioServer Module
        time.sleep(3)
  
        try:
            threading.Thread(target=wsioServer.start, args=(hubOptions.wsioServer,)).start()
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
