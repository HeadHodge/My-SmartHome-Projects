#############################################
##            GLOBAL VARIABLES
#############################################
print('Load ttyServer')

import sys, time, json, threading, traceback, asyncio
import serial, noteTool

_parent = sys.modules["__main__"]
        
##########################
async def deliverCommand(connection, command):
##########################
    try:
        #print(f'***deliverCommand: {command}')
        connection.write(command)
        #await receiveReports(connection)
    except:
        print('Abort deliverCommand', sys.exc_info()[0])
        traceback.print_exc()
        
##########################
def deliverReports(connection):
##########################
    try:
        print(f' \n***receiveReports')
        
        while True:
            report = connection.readline()
            if(len(report) == 0): break
            print(f'***{report}')        
    except:
        print('Abort receiveReports', sys.exc_info()[0])
        traceback.print_exc()

###################
#     start
###################
def start(options={}):
    print(f'Start ttyServer: {options["port"]}')

    try:    
        options['connection'] = serial.Serial(
            options['port'],
            options['speed'], 
            timeout=options['timeout'],
            parity=options['parity'], 
            xonxoff=options['xonxoff'], 
            rtscts=options['rtscts']
        )

        if(options['connection'].is_open == False): print('Abort: open serial port failed'); return;
        
        deliverReports(options['connection'])
    except:
        print('Abort ttyServer', sys.exc_info()[0])
        traceback.print_exc()
    
#############################################
##                MAIN
##Open server to listen for control input
#############################################

if __name__ == "__main__":
# Run this module on main thread to unit test with following code

    start()
