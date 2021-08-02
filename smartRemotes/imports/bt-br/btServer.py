"""
Bluetooth HID keyboard emulator DBUS Service

Original idea taken from:
http://yetanotherpointlesstechblog.blogspot.com/2016/04/emulating-bluetooth-keyboard-with.html

Moved to Python 3 and tested with BlueZ 5.43
"""
"""
create a bluetooth device to emulate a HID keyboard
"""
#############################################
##            MODULE VARIABLES
#############################################
print("Load btServer")

import os, sys, time, traceback, json, socket, threading, asyncio
import btDevice

#myDeviceAddress = 'DC:A6:32:65:8A:AB'

##########################
async def transfer(post, options):
##########################
    global myCode
    
    try:
        loop = asyncio.get_event_loop()
        key = post['service_data']['post']
    
        if(options.get('isConnected', None) == None): print('Abort transfer, no active connecton available'); return
        if(key.get('keyCode', None) == None): print('Abort transfer, "keyCode" missing'); return
        if(key.get('hidCode', None) == None): print('Abort transfer, "hidCode" missing'); return
    
        repeat = key.get('hidRepeat', 0)    
        hold = key.get('hidWait', 0)
        reportNum = key.get('hidReport', 1)
        hidCode = key.get('hidCode', 0)
        hidMod = key.get('hidMod', 0)

        if(reportNum == 1):
            #Send Report #1
            await loop.sock_sendall(options['connection'], bytes([ 0xA1, reportNum, hidMod, 0, hidCode, 0, 0, 0, 0, 0 ]))
            await asyncio.sleep(hold)  
            await loop.sock_sendall(options['connection'], bytes([ 0xA1, reportNum, 0, 0, 0, 0, 0, 0, 0, 0 ]))
            return        
        elif(reportNum == 2):    
            #Send Report #2
            keyBytes = hidCode.to_bytes(2, byteorder='little')
            #print(bytes([ 0xA1, reportNum, keyBytes[0], keyBytes[1] ]))
            await loop.sock_sendall(options['connection'], bytes([ 0xA1, reportNum, keyBytes[0], keyBytes[1] ]))
            await asyncio.sleep(hold)
            await loop.sock_sendall(options['connection'], bytes([ 0xA1, reportNum, 0, 0 ]))
            return
        elif(reportNum == 3):    
            #Send Report #3
            print('SEND Report 3')
            #Send Report #3
            await loop.sock_sendall(options['connection'], bytes([ 0xA1, reportNum, hidMod, hidCode ]))
            await asyncio.sleep(hold)  
            await loop.sock_sendall(options['connection'], bytes([ 0xA1, reportNum, 0, 0 ]))
            return        
        else:
            print(f'Abort transfer, Invalid reportNum: {reportNum}')
    except:
        print('Abort transfer: ', sys.exc_info()[0])
        traceback.print_exc()
             
#############################################
async def connect(server, loop, options):
#############################################
    while True:
        try:
            print(" \n==============================================================")
            print("***CONNECT")
            connection, device = await loop.sock_accept(server)
            options['server'] = server
            options['connection'] = connection
            options['isConnected'] = True
            print(f' \n***CONNECTED to device: {device}\n')
            
            while True:
                print(f'***WAIT btPOST')
                post = await loop.sock_recv(connection, 2048)
                if(len(post) == 0): raise Exception("****DISCONNECTED****")
                if(options.get('userEvent', None) != None): await options['userEvent'](post, options); continue
                print(f' \n***RECEIVED POST: {post}')
        except:
            print(' \n***ABORT CONNECTION: ', sys.exc_info()[1])
            options['isConnected'] = None
            
####################################################################################################
def start(options={}): # Note: standard hid channels > "controlPort": 17, "interruptPort": 19
####################################################################################################
    print("Start btServer")

    try:
        if(options.get('channel', None) == None): print('Abort btServer: "channel" option missing'); return
        if(options.get('userEvent', None) == None): print('Abort btServer: "userEvent" option missing'); return
            
        options['address'] = btDevice.getAddress()
        options['transfer'] = transfer
        options['connection'] = None
        
        if(options.get('address', None) == None): print('Abort btServer: "address" option missing'); return
       
        print(f'create server at btAddress: {options["address"]} on hidChannel: {options["channel"]}')
        
        server = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_SEQPACKET, socket.BTPROTO_L2CAP)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((options['address'], options['channel']))
        server.setblocking(False)
        server.listen(1)

        asyncio.set_event_loop(asyncio.new_event_loop())
        loop = asyncio.get_event_loop()
        loop.run_until_complete(connect(server, loop, options))
    except:
        print('Abort btServer: ', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
#############################################   

# Run this module on main thread to unit test with following code
if __name__ == '__main__':
    P_CTRL = 17 # Service port - must match port configured in SDP record
    P_INTR = 19 # Service port - must match port configured in SDP record#Interrrupt port

    start(P_CTRL)
    start(P_INTR)
