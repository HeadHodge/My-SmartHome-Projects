#!/usr/bin/python
# SPDX-License-Identifier: LGPL-2.1-or-later

from __future__ import print_function

import argparse
import dbus
import dbus.exceptions
import dbus.mainloop.glib
import dbus.service
import time
import threading

try:
    from gi.repository import GLib # python3
    from gi.repository import GObject  # python3
except ImportError:
    import gobject as GObject  # python2

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
#mainloop = GObject.MainLoop()
mainloop = GLib.MainLoop()
bus = dbus.SystemBus()
device_obj = None
dev_path = None

#Ad Defines
BLUEZ_SERVICE_NAME = 'org.bluez'
LE_ADVERTISING_MANAGER_IFACE = 'org.bluez.LEAdvertisingManager1'
DBUS_OM_IFACE = 'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'
LE_ADVERTISEMENT_IFACE = 'org.bluez.LEAdvertisement1'

#Agent Defines
BUS_NAME = 'org.bluez'
AGENT_INTERFACE = 'org.bluez.Agent1'
AGENT_PATH = "/org/bluez/justWorks/agent"
AGENT_CAPABILITY = "NoInputNoOutput"

class InvalidArgsException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.freedesktop.DBus.Error.InvalidArgs'


class NotSupportedException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.bluez.Error.NotSupported'


class NotPermittedException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.bluez.Error.NotPermitted'


class InvalidValueLengthException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.bluez.Error.InvalidValueLength'


class FailedException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.bluez.Error.Failed'

def ask(prompt):
	try:
		return raw_input(prompt)
	except:
		return input(prompt)

def set_trusted(path):
	props = dbus.Interface(bus.get_object("org.bluez", path),
					"org.freedesktop.DBus.Properties")
	props.Set("org.bluez.Device1", "Trusted", True)

def dev_connect(path):
	dev = dbus.Interface(bus.get_object("org.bluez", path),
							"org.bluez.Device1")
	dev.Connect()

class Rejected(dbus.DBusException):
	_dbus_error_name = "org.bluez.Error.Rejected"

def pair_reply():
	print("Device paired")
	set_trusted(dev_path)
	dev_connect(dev_path)
	mainloop.quit()

def pair_error(error):
	err_name = error.get_dbus_name()
	if err_name == "org.freedesktop.DBus.Error.NoReply" and device_obj:
		print("Timed out. Cancelling pairing")
		device_obj.CancelPairing()
	else:
		print("Creating device failed: %s" % (error))

def register_ad_cb():
    print('Advertisement registered')


def register_ad_error_cb(error):
    print('Failed to register advertisement: ' + str(error))
    mainloop.quit()


def find_adapter():
    global bus

    remote_om = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, '/'),
                               DBUS_OM_IFACE)
    objects = remote_om.GetManagedObjects()

    for o, props in objects.items():
        if LE_ADVERTISING_MANAGER_IFACE in props:
            return o

    return None


def shutdown(timeout):
    print('Advertising for {} seconds...'.format(timeout))
    time.sleep(timeout)
    mainloop.quit()


class Agent(dbus.service.Object):
	exit_on_release = True

	def set_exit_on_release(self, exit_on_release):
		self.exit_on_release = exit_on_release

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="", out_signature="")
	def Release(self):
		print("Release")
		if self.exit_on_release:
			mainloop.quit()

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="os", out_signature="")
	def AuthorizeService(self, device, uuid):
		print("AuthorizeService (%s, %s)" % (device, uuid))
		authorize = ask("Authorize connection (yes/no): ")
		if (authorize == "yes"):
			return
		raise Rejected("Connection rejected by user")

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="o", out_signature="s")
	def RequestPinCode(self, device):
		print("RequestPinCode (%s)" % (device))
		set_trusted(device)
		return ask("Enter PIN Code: ")

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="o", out_signature="u")
	def RequestPasskey(self, device):
		print("RequestPasskey (%s)" % (device))
		set_trusted(device)
		passkey = ask("Enter passkey: ")
		return dbus.UInt32(passkey)

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="ouq", out_signature="")
	def DisplayPasskey(self, device, passkey, entered):
		print("DisplayPasskey (%s, %06u entered %u)" %
						(device, passkey, entered))

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="os", out_signature="")
	def DisplayPinCode(self, device, pincode):
		print("DisplayPinCode (%s, %s)" % (device, pincode))

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="ou", out_signature="")
	def RequestConfirmation(self, device, passkey):
		print("RequestConfirmation (%s, %06d)" % (device, passkey))
		confirm = ask("Confirm passkey (yes/no): ")
		if (confirm == "yes"):
			set_trusted(device)
			return
		raise Rejected("Passkey doesn't match")

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="o", out_signature="")
	def RequestAuthorization(self, device):
		print("RequestAuthorization (%s)" % (device))
		auth = ask("Authorize? (yes/no): ")
		if (auth == "yes"):
			return
		raise Rejected("Pairing rejected")

	@dbus.service.method(AGENT_INTERFACE,
					in_signature="", out_signature="")
	def Cancel(self):
		print("Cancel")

class Ad(dbus.service.Object):
    PATH_BASE = '/org/bluez/example/advertisement'

    def __init__(self, bus, index, adType):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.ad_type = adType
        self.service_uuids = None
        self.manufacturer_data = None
        self.solicit_uuids = None
        self.service_data = None
        self.local_name = 'TaTa'
        self.include_tx_power = True
        self.data = None
        dbus.service.Object.__init__(self, bus, self.path)

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def get_properties(self):
        properties = dict()
        
        properties['Type'] = self.ad_type
       
        properties['LocalName'] = dbus.String(self.local_name)
        
        properties['Appearance'] = dbus.UInt16(961)
        
        properties['Discoverable'] = dbus.Boolean(True)
        
        properties['DiscoverableTimeout'] = dbus.UInt16(0)

        properties['Includes'] = dbus.Array(["tx-power"], signature='s')

        return {LE_ADVERTISEMENT_IFACE: properties}

    @dbus.service.method(DBUS_PROP_IFACE, in_signature='s', out_signature='a{sv}')
    def GetAll(self, interface):
        print('GetAll')
        if interface != LE_ADVERTISEMENT_IFACE: raise InvalidArgsException()
        print('returning props')
        
        
        properties = dict()
        
        properties['Type'] = self.ad_type
       
        properties['LocalName'] = dbus.String(self.local_name)
        
        properties['Appearance'] = dbus.UInt16(961)
        
        properties['Discoverable'] = dbus.Boolean(True)
        
        properties['DiscoverableTimeout'] = dbus.UInt16(0)

        properties['Includes'] = dbus.Array(["tx-power"], signature='s')
        
        return properties

    @dbus.service.method(LE_ADVERTISEMENT_IFACE, in_signature='', out_signature='')
    def Release(self):
        print(f'{self.path}: Released!')


class TestAdvertisement(Ad):

    def __init__(self, bus, index):
        Ad.__init__(self, bus, index, 'peripheral')
        #self.add_service_uuid('180D')
        #self.add_service_uuid('180F')
        #self.add_manufacturer_data(0xffff, [0x00, 0x01, 0x02, 0x03])
        #self.add_service_data('9999', [0x00, 0x01, 0x02, 0x03, 0x04])
        self.add_local_name('TestAdvertisement')
        self.include_tx_power = True
        #self.add_data(0x26, [0x01, 0x01, 0x00])

def startAgent():
    print(f'startAgent')

    path = AGENT_PATH
    capability = AGENT_CAPABILITY
    agent = Agent(bus, path)

    obj = bus.get_object(BUS_NAME, "/org/bluez");
    manager = dbus.Interface(obj, "org.bluez.AgentManager1")
    manager.RegisterAgent(path, capability)
    manager.RequestDefaultAgent(path)

    print("Agent registered")


def startAd(timeout=0):
    print(f'startAd')
    global adapter, bus
    
    adapter_props = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),
                                   "org.freedesktop.DBus.Properties")

    adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(1))

    ad_manager = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),
                                LE_ADVERTISING_MANAGER_IFACE)

    #test_advertisement = TestAdvertisement(bus, 0)
    test_advertisement =  Ad(bus, 0, 'peripheral')


    ad_manager.RegisterAdvertisement(test_advertisement.get_path(), {},
                                     reply_handler=register_ad_cb,
                                     error_handler=register_ad_error_cb)

def stopAd():
    print(f'stopAd')
    
    ad_manager = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter), LE_ADVERTISING_MANAGER_IFACE)
    ad_manager.UnregisterAdvertisement(test_advertisement)
    print('Ad unregistered')
    dbus.service.Object.remove_from_connection(test_advertisement)

def srart(timeout=0):
    print(f'Start gapBridge')
    global mainloop, adapter

    adapter = find_adapter()
    
    if not adapter:
        print('LEAdvertisingManager1 interface not found')
        return
 
    #startAgent()
    startAd()
 
    mainloop.run()  # blocks until mainloop.quit() is called
    #stopAd()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--timeout', default=0, type=int, help="advertise " +
                        "for this many seconds then stop, 0=run forever " +
                        "(default: 0)")
    args = parser.parse_args()

    srart(args.timeout)
