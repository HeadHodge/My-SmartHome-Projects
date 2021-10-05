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
    from gi.repository import GObject  # python3
except ImportError:
    import gobject as GObject  # python2

mainloop = None

BLUEZ_SERVICE_NAME = 'org.bluez'
LE_ADVERTISING_MANAGER_IFACE = 'org.bluez.LEAdvertisingManager1'
DBUS_OM_IFACE = 'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'

LE_ADVERTISEMENT_IFACE = 'org.bluez.LEAdvertisement1'


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

try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

BUS_NAME = 'org.bluez'
AGENT_INTERFACE = 'org.bluez.Agent1'
AGENT_PATH = "/org/bluez/justWorks/agent"
AGENT_CAPABILITY = "NoInputNoOutput"

bus = None
device_obj = None
dev_path = None

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


def find_adapter(bus):
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

class Advertisement(dbus.service.Object):
    PATH_BASE = '/org/bluez/example/advertisement'

    def __init__(self, bus, index, advertising_type):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.ad_type = advertising_type
        self.service_uuids = None
        self.manufacturer_data = None
        self.solicit_uuids = None
        self.service_data = None
        self.local_name = None
        self.include_tx_power = False
        self.data = None
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        properties = dict()
        properties['Type'] = self.ad_type
        if self.service_uuids is not None:
            properties['ServiceUUIDs'] = dbus.Array(self.service_uuids,
                                                    signature='s')
        if self.solicit_uuids is not None:
            properties['SolicitUUIDs'] = dbus.Array(self.solicit_uuids,
                                                    signature='s')
        if self.manufacturer_data is not None:
            properties['ManufacturerData'] = dbus.Dictionary(
                self.manufacturer_data, signature='qv')
        
        if self.service_data is not None:
            properties['ServiceData'] = dbus.Dictionary(self.service_data,
                                                        signature='sv')
        if self.local_name is not None:
            properties['LocalName'] = dbus.String(self.local_name)
        
        properties['Appearance'] = dbus.UInt16(961)
        
        properties['Discoverable'] = dbus.Boolean(True)
        
        properties['DiscoverableTimeout'] = dbus.UInt16(0)

        if self.include_tx_power:
            properties['Includes'] = dbus.Array(["tx-power"], signature='s')

        if self.data is not None:
            properties['Data'] = dbus.Dictionary(
                self.data, signature='yv')
        return {LE_ADVERTISEMENT_IFACE: properties}

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_service_uuid(self, uuid):
        if not self.service_uuids:
            self.service_uuids = []
        self.service_uuids.append(uuid)

    def add_solicit_uuid(self, uuid):
        if not self.solicit_uuids:
            self.solicit_uuids = []
        self.solicit_uuids.append(uuid)

    def add_manufacturer_data(self, manuf_code, data):
        if not self.manufacturer_data:
            self.manufacturer_data = dbus.Dictionary({}, signature='qv')
        self.manufacturer_data[manuf_code] = dbus.Array(data, signature='y')

    def add_service_data(self, uuid, data):
        if not self.service_data:
            self.service_data = dbus.Dictionary({}, signature='sv')
        self.service_data[uuid] = dbus.Array(data, signature='y')

    def add_local_name(self, name):
        if not self.local_name:
            self.local_name = ""
        self.local_name = dbus.String(name)

    def add_data(self, ad_type, data):
        if not self.data:
            self.data = dbus.Dictionary({}, signature='yv')
        self.data[ad_type] = dbus.Array(data, signature='y')

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        print('GetAll')
        if interface != LE_ADVERTISEMENT_IFACE:
            raise InvalidArgsException()
        print('returning props')
        return self.get_properties()[LE_ADVERTISEMENT_IFACE]

    @dbus.service.method(LE_ADVERTISEMENT_IFACE,
                         in_signature='',
                         out_signature='')
    def Release(self):
        print('%s: Released!' % self.path)


class TestAdvertisement(Advertisement):

    def __init__(self, bus, index):
        Advertisement.__init__(self, bus, index, 'peripheral')
        #self.add_service_uuid('180D')
        #self.add_service_uuid('180F')
        #self.add_manufacturer_data(0xffff, [0x00, 0x01, 0x02, 0x03])
        #self.add_service_data('9999', [0x00, 0x01, 0x02, 0x03, 0x04])
        self.add_local_name('TestAdvertisement')
        self.include_tx_power = True
        #self.add_data(0x26, [0x01, 0x01, 0x00])


def main(timeout=0):
    global mainloop

    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    bus = dbus.SystemBus()

    adapter = find_adapter(bus)
    if not adapter:
        print('LEAdvertisingManager1 interface not found')
        return


    path = AGENT_PATH
    capability = AGENT_CAPABILITY
    agent = Agent(bus, path)

    obj = bus.get_object(BUS_NAME, "/org/bluez");
    manager = dbus.Interface(obj, "org.bluez.AgentManager1")
    manager.RegisterAgent(path, capability)
    manager.RequestDefaultAgent(path)

    print("Agent registered")




    adapter_props = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),
                                   "org.freedesktop.DBus.Properties")

    adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(1))

    ad_manager = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter),
                                LE_ADVERTISING_MANAGER_IFACE)

    test_advertisement = TestAdvertisement(bus, 0)

    mainloop = GObject.MainLoop()

    ad_manager.RegisterAdvertisement(test_advertisement.get_path(), {},
                                     reply_handler=register_ad_cb,
                                     error_handler=register_ad_error_cb)

    if timeout > 0:
        threading.Thread(target=shutdown, args=(timeout,)).start()
    else:
        print('Advertising forever...')

    mainloop.run()  # blocks until mainloop.quit() is called

    ad_manager.UnregisterAdvertisement(test_advertisement)
    print('Advertisement unregistered')
    dbus.service.Object.remove_from_connection(test_advertisement)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--timeout', default=0, type=int, help="advertise " +
                        "for this many seconds then stop, 0=run forever " +
                        "(default: 0)")
    args = parser.parse_args()

    main(args.timeout)
