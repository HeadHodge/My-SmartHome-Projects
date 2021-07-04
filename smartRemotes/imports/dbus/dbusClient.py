#!/usr/bin/python3
import os, sys, time
import dbus

print("Load dbusClient")
iface = None

def start(dbusName='smartKeypads.ip2btBridge', ifaceName='ip2bt.Input'):
    global iface
    print("Start dbusClient")
    methods = dbus.SessionBus().get_object(dbusName, '/ip2btBridge/methods')
    iface = dbus.Interface(methods, ifaceName)
        
def printName(name):
    print(name)
    
def send_key_state():
    """sends a single frame of the current key state to the emulator server"""
    bin_str = ""
    element = self.state[2]
    for bit in element:
       bin_str += str(bit)
    iface.send_keys(int(bin_str, 2), self.state[4:10])
   
def send_string(string=''):
    """sends a single frame of the current key state to the emulator server"""
    iface.send_string(string)

if __name__ == "__main__":
    dc = start()
    """
    string_to_send = 'Hello World'
    print("Sending " + string_to_send)
    dc.send_string(string_to_send)
    print("Done.")
    """
