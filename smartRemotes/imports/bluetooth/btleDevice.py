"""
Bluetooth HID keyboard emulator DBUS Service

Original idea taken from:
http://yetanotherpointlesstechblog.blogspot.com/2016/04/emulating-bluetooth-keyboard-with.html

Moved to Python 3 and tested with BlueZ 5.43
"""
#############################################
##            MODULE VARIABLES
#############################################
print('Load btDevice')

from gi.repository import GLib
from dbus.mainloop.glib import DBusGMainLoop
from xml.etree import ElementTree
import os, sys, time, dbus

#Connect to Bluez5 API on the dbus system bus
DBusGMainLoop(set_as_default=True)

#############################################
# Global Variables
#############################################
_options = {}
    
#############################################
def getPowered():
#############################################
    """
    power state of the device.
    """
    return _options['hciProperties']['interface'].Get(_options['hciProperties']['adapter'], 'Powered')

#############################################
def setPowered(state=True):
#############################################
    value = dbus.Boolean(state)
    _options['hciProperties']['interface'].Set(_options['hciProperties']['adapter'], 'Powered', value)

#############################################
def getAlias():
#############################################
    return _options['hciProperties']['interface'].Get(_options['hciProperties']['adapter'], 'Alias')

#############################################
def setAlias(alias):
#############################################
    _options['hciProperties']['interface'].Set(_options['hciProperties']['adapter'], 'Alias', alias)

#############################################
def getDiscoverableTime():
#############################################
    """Discoverable timeout of the Adapter."""
    return _options['hciProperties']['interface'].Get(_options['hciProperties']['adapter'], 'DiscoverableTimeout')

#############################################
def setDiscoverableTime(timeout=300):
#############################################
    duration = dbus.UInt32(timeout)
    _options['hciProperties']['interface'].Set(_options['hciProperties']['adapter'], 'DiscoverableTimeout', duration)

#############################################
def getDiscoverable():
#############################################
    """Discoverable state of the Adapter."""
    return _options['hciProperties']['interface'].Get(_options['hciProperties']['adapter'], 'Discoverable')

#############################################
def setDiscoverable(new_state):
#############################################
    _options['hciProperties']['interface'].Set(_options['hciProperties']['adapter'], 'Discoverable', new_state)

#############################################
def getAddress():
#############################################
    """Return the hci MAC address."""
    return _options['hciProperties']['interface'].Get(_options['hciProperties']['adapter'], 'Address')

#############################################
def isConnected():
#############################################
    """Return the device connection state."""
    if(_options['deviceProperties']['interface'] == None): return 0
    return _options['deviceProperties']['interface'].Get(_options['deviceProperties']['device'], 'Connected')
    
#############################################
def enableConnectSignal(callBack):
#############################################
    print(f'enableConnectSignal')
    if(_options['devicePath'] == None): print('Abort enableConnectSignal, no device connected'); return
    
    _options['systemBus'].add_signal_receiver(callBack, signal_name = 'PropertiesChanged', path = _options['devicePath'])
        
    # Start btOutput event loop
    #print('start connectSignal eventLoop')
    #eventloop = GLib.MainLoop()
    #eventloop.run()

#############################################
def registerProfile(uuid, options):
#############################################
    print(f'Register bluetooth profile, uuid: {uuid}')
    _options['bluezManager'].RegisterProfile('/org/bluez/hidProfile', uuid, options)
     
#############################################
def start():
#############################################
    print(f'Start btDevice')
    global _options
    interface = {}
    
    systemBus = dbus.SystemBus()

    #get dbus introspect interface
    #busctl tree
    interface['dbusIntrospect'] = dbus.Interface(systemBus.get_object('org.bluez', '/org/bluez/hci0'), 'org.freedesktop.DBus.Introspectable')

    #get 1st paired device name
    devicePath = None
    interface['deviceProperties'] = None
    for child in ElementTree.fromstring(interface['dbusIntrospect'].Introspect()):
        if child.tag != 'node': continue
        deviceName = child.attrib['name']
        devicePath = '/org/bluez/hci0/' + deviceName
        interface['deviceProperties'] = dbus.Interface(systemBus.get_object('org.bluez', devicePath), 'org.freedesktop.DBus.Properties')
        #print('devicePath', devicePath)
        break

    #get bluez interfaces
    interface['bluezManager'] = dbus.Interface(systemBus.get_object('org.bluez', '/org/bluez'), 'org.bluez.ProfileManager1')
    interface['hciProperties'] = dbus.Interface(systemBus.get_object('org.bluez', '/org/bluez/hci0'), 'org.freedesktop.DBus.Properties')
    
    _options['systemBus'] = systemBus
    
    _options['devicePath'] = devicePath
    
    _options['dbusIntrospect'] = {
            'interface': interface['dbusIntrospect'],
            'introspect': 'org.freedesktop.DBus.Introspectable'
        }
    
    _options['bluezManager'] = {
            'interface': interface['bluezManager'],
            'manager': 'org.bluez.ProfileManager1'
        }
        
    _options['hciProperties'] = {
            'interface': interface['hciProperties'],
            'properties': 'org.freedesktop.DBus.Properties',
            'adapter': 'org.bluez.Adapter1'
        }
        
    _options['deviceProperties'] = {
            'interface': interface['deviceProperties'],
            'properties': 'org.freedesktop.DBus.Properties',
            'device': 'org.bluez.Device1'
        }

    _options['hciAddress'] = getAddress()
    
#############################################
##                MAIN
#############################################   
start()

#Run this module on main thread to unit test with following code
if __name__ == '__main__':
    import threading
  
    #############################################
    def onConnectSignal(interface, changed, path):
    #############################################
        print(f'****CONNECTION ALERT****, interface: {interface}, connected: {changed["Connected"]}')
    
    print('hciAddress', _options['hciAddress'])
    print('isConnected', isConnected())
    threading.Thread(target=enableConnectSignal, args=(onConnectSignal,)).start()
