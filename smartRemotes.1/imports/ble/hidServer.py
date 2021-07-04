#!/usr/bin/env python3

####################################################################################################################
#
#                                    *****smartRemotes created by HeadHodge*****
#
#   HeadHodge/smartRemotes is licensed under the MIT License
#
#   A short and simple permissive license with conditions only requiring preservation of copyright and license notices.
#   Licensed works, modifications, and larger works may be distributed under different terms and without source code.
#
####################################################################################################################
print("Load gattServer")

from gi.repository import GLib
from dbus.mainloop.glib import DBusGMainLoop
import os, sys, time, traceback, json, threading
import dbus, dbus.exceptions, dbus.service
#import btleDevice

_mainloop = None
_keyboard = None

BLUEZ_SERVICE_NAME = 'org.bluez'
GATT_MANAGER_IFACE = 'org.bluez.GattManager1'
DBUS_OM_IFACE =      'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE =    'org.freedesktop.DBus.Properties'

GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE =    'org.bluez.GattCharacteristic1'
GATT_DESC_IFACE =    'org.bluez.GattDescriptor1'

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


class Application(dbus.service.Object):
    """
    org.bluez.GattApplication1 interface implementation
    """
    def __init__(self, bus):
 
        self.path = '/'
        self.services = []
        dbus.service.Object.__init__(self, bus, self.path)
        
        self.hidService = HIDService(bus, 0)
        #self.deviceService = DeviceInfoService(bus, 1)
        #self.batteryService = BatteryService(bus, 2)
        #self.genericAttributeService = GenericAttributeService(bus, 3)
        
        #self.add_service(self.deviceService)
        #self.add_service(self.batteryService)
        self.add_service(self.hidService)
        #self.add_service(self.genericAttributeService)

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_service(self, service):
        self.services.append(service)

    @dbus.service.method(DBUS_OM_IFACE, out_signature='a{oa{sa{sv}}}')
    def GetManagedObjects(self):
        response = {}
        #print('GetManagedObjects')

        for service in self.services:
            response[service.get_path()] = service.get_properties()
            chrcs = service.get_characteristics()
            for chrc in chrcs:
                response[chrc.get_path()] = chrc.get_properties()
                descs = chrc.get_descriptors()
                for desc in descs:
                    response[desc.get_path()] = desc.get_properties()

        return response


class Service(dbus.service.Object):
    """
    org.bluez.GattService1 interface implementation
    """
    PATH_BASE = '/org/bluez/example/service'

    def __init__(self, bus, index, uuid, primary):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.uuid = uuid
        self.primary = primary
        self.characteristics = []
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
                GATT_SERVICE_IFACE: {
                        'UUID': self.uuid,
                        'Primary': self.primary,
                        'Characteristics': dbus.Array(
                                self.get_characteristic_paths(),
                                signature='o')
                }
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_characteristic(self, characteristic):
        self.characteristics.append(characteristic)

    def get_characteristic_paths(self):
        result = []
        for chrc in self.characteristics:
            result.append(chrc.get_path())
        return result

    def get_characteristics(self):
        return self.characteristics

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_SERVICE_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_SERVICE_IFACE]


class Characteristic(dbus.service.Object):
    """
    org.bluez.GattCharacteristic1 interface implementation
    """
    def __init__(self, bus, index, uuid, flags, service):
        self.path = service.path + '/char' + str(index)
        self.bus = bus
        self.uuid = uuid
        self.service = service
        self.flags = flags
        self.descriptors = []
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
                GATT_CHRC_IFACE: {
                        'Service': self.service.get_path(),
                        'UUID': self.uuid,
                        'Flags': self.flags,
                        'Descriptors': dbus.Array(
                                self.get_descriptor_paths(),
                                signature='o')
                }
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_descriptor(self, descriptor):
        self.descriptors.append(descriptor)

    def get_descriptor_paths(self):
        result = []
        for desc in self.descriptors:
            result.append(desc.get_path())
        return result

    def get_descriptors(self):
        return self.descriptors

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_CHRC_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_CHRC_IFACE]

    @dbus.service.method(GATT_CHRC_IFACE, in_signature='a{sv}', out_signature='ay')
    def ReadValue(self, options):
        print('Default ReadValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE, in_signature='aya{sv}')
    def WriteValue(self, value, options):
        print('Default WriteValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE)
    def StartNotify(self):
        print('Default StartNotify called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE)
    def StopNotify(self):
        print('Default StopNotify called, returning error')
        raise NotSupportedException()

    @dbus.service.signal(DBUS_PROP_IFACE, signature='sa{sv}as')
    def PropertiesChanged(self, interface, changed, invalidated):
        pass


class Descriptor(dbus.service.Object):
    """
    org.bluez.GattDescriptor1 interface implementation
    """
    def __init__(self, bus, index, uuid, flags, characteristic):
        self.path = characteristic.path + '/desc' + str(index)
        self.bus = bus
        self.uuid = uuid
        self.flags = flags
        self.chrc = characteristic
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
                GATT_DESC_IFACE: {
                        'Characteristic': self.chrc.get_path(),
                        'UUID': self.uuid,
                        'Flags': self.flags,
                }
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_DESC_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_DESC_IFACE]

    @dbus.service.method(GATT_DESC_IFACE,
                        in_signature='a{sv}',
                        out_signature='ay')
    def ReadValue(self, options):
        print ('Default ReadValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_DESC_IFACE, in_signature='aya{sv}')
    def WriteValue(self, value, options):
        print('Default WriteValue called, returning error')
        raise NotSupportedException()


#######################################################
#                HID KEYBOARD
#######################################################
'''
#sourceId="org.bluetooth.service.battery_service" type="primary" uuid="180F"
class BatteryService(Service):
    """
    Fake Battery service that emulates a draining battery.

    """
    SERVICE_UUID = '180f'

    def __init__(self, bus, index):
        Service.__init__(self, bus, index, self.SERVICE_UUID, True)
        self.add_characteristic(BatteryLevelCharacteristic(bus, 0, self))

#name="Battery Level" sourceId="org.bluetooth.characteristic.battery_level" uuid="2A19"
class BatteryLevelCharacteristic(Characteristic):
    """
    Fake Battery Level characteristic. The battery level is drained by 2 points
    every 5 seconds.

    """
    BATTERY_LVL_UUID = '2a19'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.BATTERY_LVL_UUID,
                ['read', 'notify'],
                service)
        self.notifying = False
        self.notifyCnt = 0
        self.battery_lvl = 100

    def notify_battery_level(self):
        self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': [dbus.Byte(self.battery_lvl)] }, [])
        self.notifyCnt += 1
        
    def drain_battery(self):
        if not self.notifying: return True
        
        if self.battery_lvl > 0:
            self.battery_lvl -= 2
            if self.battery_lvl < 5:
                #self.battery_lvl = 0
                GObject.source_remove(self.timer)
                
        print('Battery Level drained: ' + repr(self.battery_lvl))
        self.notify_battery_level()
        return False #Stop after 1st Notify
 
    def ReadValue(self, options):
        print('Battery Level read: ' + repr(self.battery_lvl))
        return [dbus.Byte(self.battery_lvl)]

    def StartNotify(self):
        print(' \n***Start Battery Notify')
        
        if self.notifying:
            print('Already notifying, nothing to do')
            return

        self.notifying = True
        self.timer = GObject.timeout_add(60000, self.drain_battery)

    def StopNotify(self):
        print('Stop Battery Notify')
        
        if not self.notifying:
            print('Not notifying, nothing to do')
            return

        self.notifying = False


#name="Generic Attribute" last-modified="2012-12-04" type="org.bluetooth.service.generic_attribute" uuid="1801"
class GenericAttributeService(Service):

    SERVICE_UUID = '1801'

    def __init__(self, bus, index):
        print(f'INIT: GenericAttributeService')

        Service.__init__(self, bus, index, self.SERVICE_UUID, True)
        #self.add_characteristic(ServiceChangedCharacteristic(bus, 0, self))


#name="Service Changed" type="org.bluetooth.characteristic.gatt.service_changed" last-modified="2013-05-29" uuid="2A05"       
class ServiceChangedCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A05'

    def __init__(self, bus, index, service):
        print(f'INIT: ServiceChangedCharacteristic')
        
        Characteristic.__init__(self, bus, index, self.CHARACTERISTIC_UUID, ['notify'], service)
'''                
'''
        <Value>
        <Field name="Start of Affected Attribute Handle Range">
            <Requirement>Mandatory</Requirement>
            <Format>uint16</Format>
            <Minimum>1</Minimum>
            <Maximum>65535</Maximum>
        </Field>
        <Field name="End of Affected Attribute Handle Range">
            <Requirement>Mandatory</Requirement>
            <Format>uint16</Format>
            <Minimum>1</Minimum>
            <Maximum>65535</Maximum>
        </Field>
        </Value>
        
        example(handle 1-65535): [dbus.Byte(0x01),dbus.Byte(0x00),dbus.Byte(0xFF),dbus.Byte(0xFF)]
'''
'''        
        self.isConnected = False
        self.handles = [dbus.Byte(0x01),dbus.Byte(0x00),dbus.Byte(0xFF),dbus.Byte(0xFF)]
       
    def notifyServiceChanged(self):
        #send keyCode
        print(f' \n***notifyServiceChanged: {[hex(x) for x in keyBytes]}, keyHold: {keyHold}***');
        self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': self.handles }, [])

    def StartNotify(self):
        print(f' \n***CONNECTED: ServiceChanged Characteristic')
        self.isConnected = True
        #GLib.timeout_add(10000, self.send)

    def StopNotify(self):
        print(f' \n***DISCONNECTED: ServiceChanged Characteristic')
        self.isConnected = False


#sourceId="org.bluetooth.service.device_information" type="primary" uuid="180A"
class DeviceInfoService(Service):

    SERVICE_UUID = '180A'

    def __init__(self, bus, index):
        Service.__init__(self, bus, index, self.SERVICE_UUID, True)
        self.add_characteristic(VendorCharacteristic(bus, 0, self))
        self.add_characteristic(ProductCharacteristic(bus, 1, self))
        self.add_characteristic(VersionCharacteristic(bus, 2, self))

#name="Manufacturer Name String" sourceId="org.bluetooth.characteristic.manufacturer_name_string" uuid="2A29"
class VendorCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A29'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ["read"],
                service)
        
        self.value = dbus.Array('HodgeCode'.encode(), signature=dbus.Signature('y'))
        #print(f'***VendorCharacteristic value***: {self.value}')

    def ReadValue(self, options):
        print(f'Read VendorCharacteristic: {self.value}')
        return self.value

#sourceId="org.bluetooth.characteristic.model_number_string" uuid="2A24"
class ProductCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A24'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ["read"],
                service)
        
        self.value = dbus.Array('smartRemotes'.encode(), signature=dbus.Signature('y'))
        #print(f'***ProductCharacteristic value***: {self.value}')

    def ReadValue(self, options):
        print(f'Read ProductCharacteristic: {self.value}')
        return self.value

#sourceId="org.bluetooth.characteristic.software_revision_string" uuid="2A28"
class VersionCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A28'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ["read"],
                service)
        
        self.value = dbus.Array('version 1.0.0'.encode(), signature=dbus.Signature('y'))
        #print(f'***VersionCharacteristic value***: {self.value}')

    def ReadValue(self, options):
        print(f'Read VersionCharacteristic: {self.value}')
        return self.value
'''
#name="Human Interface Device" sourceId="org.bluetooth.service.human_interface_device" type="primary" uuid="1812"
class HIDService(Service):
    SERVICE_UUID = '1812'
   
    def __init__(self, bus, index):
        Service.__init__(self, bus, index, self.SERVICE_UUID, True)
        
        self.protocolMode = ProtocolModeCharacteristic(bus, 0, self)
        self.hidInfo = HIDInfoCharacteristic(bus, 1, self)
        self.controlPoint = ControlPointCharacteristic(bus, 2, self)
        self.reportMap = ReportMapCharacteristic(bus, 3, self)
        self.report1 = Report1Characteristic(bus, 4, self)
        self.report2 = Report2Characteristic(bus, 5, self)
        
        self.add_characteristic(self.protocolMode)
        self.add_characteristic(self.hidInfo)
        self.add_characteristic(self.controlPoint)
        self.add_characteristic(self.reportMap)
        self.add_characteristic(self.report1)
        self.add_characteristic(self.report2)
        
#name="Protocol Mode" sourceId="org.bluetooth.characteristic.protocol_mode" uuid="2A4E"
class ProtocolModeCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A4E'

    def __init__(self, bus, index, service):
        
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ["read", "write-without-response"],
                service)
                
        '''        
        <Field name="Protocol Mode Value">
        <Requirement>Mandatory</Requirement>
        <Format>uint8</Format>
        <Enumerations>
        <Enumeration key="0" value="Boot Protocol Mode"/>
        <Enumeration key="1" value="Report Protocol Mode"/>
        <ReservedForFutureUse start="2" end="255"/>
        </Enumerations>
        '''
        
        #self.value = dbus.Array([1], signature=dbus.Signature('y'))
        self.parent = service
        self.value = dbus.Array(bytearray.fromhex('01'), signature=dbus.Signature('y'))
        #print(f'***ProtocolMode value***: {self.value}')

    def ReadValue(self, options):
        print(f'Read ProtocolMode: {self.value}')
        return self.value

    def WriteValue(self, value, options):
        print(f'Write ProtocolMode {value}')
        self.value = value


#id="hid_information" name="HID Information" sourceId="org.bluetooth.characteristic.hid_information" uuid="2A4A"
class HIDInfoCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A4A'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ['read'],
                service)

        '''
        <Field name="bcdHID">
            <InformativeText>16-bit unsigned integer representing version number of base USB HID Specification implemented by HID Device</InformativeText>
            <Requirement>Mandatory</Requirement>
            <Format>uint16</Format>
        </Field>
        
        <Field name="bCountryCode">
            <InformativeText>Identifies which country the hardware is localized for. Most hardware is not localized and thus this value would be zero (0).</InformativeText>
            <Requirement>Mandatory</Requirement>
            <Format>8bit</Format>
        </Field>
        
        <Field name="Flags">
            <Requirement>Mandatory</Requirement>
            <Format>8bit</Format>
            <BitField>
                <Bit index="0" size="1" name="RemoteWake">
                <Enumerations>
                    <Enumeration key="0" value="The device is not designed to be capable of providing wake-up signal to a HID host"/>
                    <Enumeration key="1" value="The device is designed to be capable of providing wake-up signal to a HID host"/>
                </Enumerations>
                </Bit>
            
                <Bit index="1" size="1" name="NormallyConnectable">
                <Enumerations>
                    <Enumeration key="0" value="The device is not normally connectable"/>
                    <Enumeration key="1" value="The device is normally connectable"/>
                </Enumerations>
                </Bit>
            
                <ReservedForFutureUse index="2" size="6"/>
            </BitField>
        </Field>
        '''
        
        self.value = dbus.Array(bytearray.fromhex('01110003'), signature=dbus.Signature('y'))
        #print(f'***HIDInformation value***: {self.value}')

    def ReadValue(self, options):
        print(f'Read HIDInformation: {self.value}')
        return self.value

#sourceId="org.bluetooth.characteristic.hid_control_point" uuid="2A4C"
class ControlPointCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A4C'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ["write-without-response"],
                service)
        
        self.value = dbus.Array(bytearray.fromhex('00'), signature=dbus.Signature('y'))
        #print(f'***ControlPoint value***: {self.value}')

    def WriteValue(self, value, options):
        print(f'Write ControlPoint {value}')
        self.value = value


#sourceId="org.bluetooth.characteristic.report_map" uuid="2A4B"
class ReportMapCharacteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A4B'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ['read'],
                service)
                
        '''
        <Field name="Report Map Value">
            <Requirement>Mandatory</Requirement>
            <Format>uint8</Format>
            <Repeated>true</Repeated>
        </Field>
        
        HID Report Descriptors https://www.usb.org/sites/default/files/documents/hid1_11.pdf
        HID Report Parser https://eleccelerator.com/usbdescreqparser/
        '''
        
        ##############################################################################################
        # This Report Descriptor defines 2 Input Reports
        # ReportMap designed by HeadHodge
        #
        # <Report Layouts>
        #   <Report>
        #       <ReportId>1</ReportId>
        #       <Description>HID Keyboard Input</Description>
        #       <Example>KeyCode capital 'M' = [dbus.Byte(0x02), dbus.Byte(0x10)]</Example>
        #       <Field>
        #           <Name>Keyboard Modifier</Name>
        #           <Size>uint8</Size>
        #           <Format>
        #               <Bit0>Left CTRL Key Pressed</Bit0>
        #               <Bit1>Left SHIFT Key Pressed</Bit1>
        #               <Bit2>Left ALT Key Pressed</Bit2>
        #               <Bit3>Left CMD(Window) Key Pressed</Bit3>
        #               <Bit4>Right CTRL Key Pressed</Bit4>
        #               <Bit5>Right SHIFT Key Pressed</Bit5>
        #               <Bit6>Right ALT Key Pressed</Bit6>
        #               <Bit7>Right CMD(Window) Key Pressed</Bit7>
        #           </Format>
        #       </Field>
        #       <Field>
        #           <Name>Keyboard Input KeyCode</Name>
        #           <Size>uint8</Size>
        #       </Field>
        #   </Report>
        #   <Report>
        #       <ReportId>2</ReportId>
        #       <Description>HID Consumer Input</Description>
        #       <Example>KeyCode 'VolumeUp' = [dbus.Byte(0xe9), dbus.Byte(0x00)]</Example>
        #       <Field>
        #           <Name>Consumer Input KeyCode</Name>
        #           <Size>uint16</Size>
        #       </Field>
        #   </Report>
        # </Report Layouts>
        ##############################################################################################
  
        #USB HID Report Descriptor
        self.value = dbus.Array(bytearray.fromhex('05010906a1018501050719e029e71500250175019508810295017508150025650507190029658100c0050C0901A101850275109501150126ff0719012Aff078100C0'))
        self.isMapLoaded = False
        #print(f'***ReportMap value***: {self.value}')

    def ReadValue(self, options):
        print(f'Read ReportMap: {self.value}')
        self.isMapLoaded = True
        return self.value


#id="report" name="Report" sourceId="org.bluetooth.characteristic.report" uuid="2A4D"        
class Report1Characteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A4D'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ['read', 'notify'],
                service)
                
        '''
        <Field name="Report Value">
        <Requirement>Mandatory</Requirement>
        <Format>uint8</Format>
        <Repeated>true</Repeated>
        </Field>
        
        Use standard key codes: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
        '''
        
        self.add_descriptor(Report1ReferenceDescriptor(bus, 1, self))
        
        self.isConnected = False
        self.value = [dbus.Byte(0x00),dbus.Byte(0x00)]
        #print(f'***Report value***: {self.value}')
       
    def sendKey(self, keyBytes, keyHold):
        #send keyCode
        print(f' \n***Send report1 keyCode: {[hex(x) for x in keyBytes]}, keyHold: {keyHold}***');
        #if(self.isConnected == False): print('Abort Report1: Not connected to client'); return
        #self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': [dbus.Byte(0x02),dbus.Byte(0x10)] }, [])
        self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': [dbus.Byte(keyBytes[0]),dbus.Byte(keyBytes[1])] }, [])
        GLib.timeout_add(keyHold, self.sendNull)
        
    def sendNull(self):
        self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': [dbus.Byte(0x00),dbus.Byte(0x00)] }, [])
        return False
                
    def ReadValue(self, options):
        print(f'Read Report: {self.value}')
        return self.value

    def StartNotify(self):
        print(f' \n***CONNECTED: Report1 Client')
        self.isConnected = True
        #GLib.timeout_add(10000, self.send)

    def StopNotify(self):
        print(f' \n***DISCONNECTED: Report1 Client')
        self.isConnected = False


#type="org.bluetooth.descriptor.report_reference" uuid="2908"
class Report1ReferenceDescriptor(Descriptor):

    DESCRIPTOR_UUID = '2908'

    def __init__(self, bus, index, characteristic):
        Descriptor.__init__(
                self, bus, index,
                self.DESCRIPTOR_UUID,
                ['read'],
                characteristic)
                
        '''
        <Field name="Report ID">
            <Requirement>Mandatory</Requirement>
            <Format>uint8</Format>
            <Minimum>0</Minimum>
            <Maximum>255</Maximum>
        </Field>
        
        <Field name="Report Type">
            <Requirement>Mandatory</Requirement>
            <Format>uint8</Format>
            <Minimum>1</Minimum>
            <Maximum>3</Maximum>
            <Enumerations>
                <Enumeration value="Input Report" key="1"/>
                <Enumeration value="Output report" key="2"/>
                <Enumeration value="Feature Report" key="3"/>
                <ReservedForFutureUse start="4" end="255"/>
                <ReservedForFutureUse1 start1="0" end1="0"/>
            </Enumerations>
        </Field>
        '''
       
        # This report uses ReportId 1 as defined in the ReportMap characteristic
        self.value = dbus.Array(bytearray.fromhex('0101'), signature=dbus.Signature('y'))
        #print(f'***ReportReference***: {self.value}')

    def ReadValue(self, options):
        print(f'Read ReportReference: {self.value}')
        return self.value


#id="report" name="Report" sourceId="org.bluetooth.characteristic.report" uuid="2A4D"        
class Report2Characteristic(Characteristic):

    CHARACTERISTIC_UUID = '2A4D'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
                self, bus, index,
                self.CHARACTERISTIC_UUID,
                ['read', 'notify'],
                service)
                
        '''
        <Field name="Report Value">
        <Requirement>Mandatory</Requirement>
        <Format>uint8</Format>
        <Repeated>true</Repeated>
        </Field>
        
        Use standard key codes: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
        '''
        
        self.add_descriptor(Report2ReferenceDescriptor(bus, 1, self))
        
        self.isConnected = False
        self.value = [dbus.Byte(0x00),dbus.Byte(0x00)]
        #print(f'***Report value***: {self.value}')
              
    def sendKey(self, keyBytes, keyHold):
        #send keyCode
        print(f' \n***Send report2 keyCode: {[hex(x) for x in keyBytes]}, keyHold: {keyHold}***');
        #if(self.isConnected == False): print('Abort Report2: Not connected to client'); return
        self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': [dbus.Byte(keyBytes[0]),dbus.Byte(keyBytes[1])] }, [])
        GLib.timeout_add(keyHold, self.sendNull)
        
    def sendNull(self):
        self.PropertiesChanged(GATT_CHRC_IFACE, { 'Value': [dbus.Byte(0x00),dbus.Byte(0x00)] }, [])
        return False
                
    def ReadValue(self, options):
        print(f'Read Report: {self.value}')
        return self.value

    def StartNotify(self):
        print(f' \n***CONNECTED: Report2 Client')
        self.isConnected = True
        #GLib.timeout_add(15000, self.send)

    def StopNotify(self):
        print(f' \n***DISCONNECTED: Report2 Client')
        self.isConnected = False
 

#type="org.bluetooth.descriptor.report_reference" uuid="2908"
class Report2ReferenceDescriptor(Descriptor):

    DESCRIPTOR_UUID = '2908'

    def __init__(self, bus, index, characteristic):
        Descriptor.__init__(
                self, bus, index,
                self.DESCRIPTOR_UUID,
                ['read'],
                characteristic)
                
        '''
        <Field name="Report ID">
            <Requirement>Mandatory</Requirement>
            <Format>uint8</Format>
            <Minimum>0</Minimum>
            <Maximum>255</Maximum>
        </Field>
        
        <Field name="Report Type">
            <Requirement>Mandatory</Requirement>
            <Format>uint8</Format>
            <Minimum>1</Minimum>
            <Maximum>3</Maximum>
            <Enumerations>
                <Enumeration value="Input Report" key="1"/>
                <Enumeration value="Output report" key="2"/>
                <Enumeration value="Feature Report" key="3"/>
                <ReservedForFutureUse start="4" end="255"/>
                <ReservedForFutureUse1 start1="0" end1="0"/>
            </Enumerations>
        </Field>
        '''
        
        # This report uses ReportId 2 as defined in the ReportMap characteristic
        self.value = dbus.Array(bytearray.fromhex('0201'), signature=dbus.Signature('y'))
        #print(f'***ReportReference***: {self.value}')

    def ReadValue(self, options):
        print(f'Read ReportReference: {self.value}')
        return self.value

####################################
def register_app_cb():
####################################
    print('GATT application registered')


####################################
def register_app_error_cb(error):
####################################
    print('Failed to register application: ' + str(error))
    _mainloop.quit()


####################################
def find_adapter(bus):
####################################
    remote_om = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
    objects = remote_om.GetManagedObjects()

    for adapter, props in objects.items():
        if GATT_MANAGER_IFACE in props.keys(): return adapter

    return None
   
#############################################
def onConnectSignal(interface, changed, data=[]):
#############################################
    if(changed.get("Connected", None) == None): return    
    #print(f'onConnectSignal, interface: {interface}, changed: {changed}')
    print(f'****CONNECTION ALERT****, interface: {interface}, connected: {changed.get("Connected", 1)}')
        
    #start advertising if not connected to host device
    isConnected = btDevice.isConnected()
    print(f'Host Device, isConnected: {isConnected}')
    if(changed["Connected"] == True): return
    return
    
    #start advertising
    print(' \n***ADVERTISE: Host device not connected');
    os.system("hcitool -i hci0 cmd 0x08 0x0006 50 00 50 00 00 00 00 00 00 00 00 00 00 07 00")
    os.system("hcitool -i hci0 cmd 0x08 0x000a 01")

####################################
async def receivedCommand(controlCommand):
####################################
    try:
    
        '''
        #loop = asyncio.get_event_loop()
        key = post['service_data']['post']
        if(key.get('keyCode', None) == None): print('Abort transfer, "keyCode" missing'); return
        if(key.get('hidCode', None) == None): print('Abort transfer, "hidCode" missing'); return

        #start advertising if not connected to host device
        isConnected = btDevice.isConnected()
        print(f'Host Device, isConnected: {isConnected}')
        if(isConnected != True):
            #start advertising
            os.system("hcitool -i hci0 cmd 0x08 0x0006 50 00 50 00 00 00 00 00 00 00 00 00 00 07 00")
            os.system("hcitool -i hci0 cmd 0x08 0x000a 01")
            print('Abort transfer, host device not connected');
            return;          
    
        reportNum = key.get('hidReport', 1)
        hidCode = key.get('hidCode', 0)
        hidMod = key.get('hidMod', 0)
        hold = key.get('hidWait', 0) * 1000
        repeat = key.get('hidRepeat', 0)    
        '''
        #controlCommand = {"controlWord": "Menu", "hidCode": 0x40, "hidReport": 2}               
        hidReport = controlCommand.get('hidReport', 1)
        hidCode = controlCommand.get('hidCode', 0)
        hidMod = controlCommand.get('hidMod', 0)
        hidWait = controlCommand.get('hidWait', 0) * 1000
        hidRepeat = controlCommand.get('hidRepeat', 0)    
        
        if(hidReport == 1):
            #Transfer Keyboard Input
            keyBytes = [hidMod, hidCode]
            _keyboard.hidService.report1.sendKey(keyBytes, hidWait)
            return        
        elif(hidReport == 2):    
            #Transfer Consumer Input
            keyBytes = hidCode.to_bytes(2, byteorder='little')
            _keyboard.hidService.report2.sendKey(keyBytes, hidWait)
            return
        else:
            print(f'Abort transfer, Invalid hidReport: {hidReport}')
    except:
        print('Abort transfer: ', sys.exc_info()[0])
        traceback.print_exc()
 
####################################
def start(options={}):
####################################
    try:
        print("Start gattServer")
        global _mainloop, _keyboard
    
        DBusGMainLoop(set_as_default=True)
        bus = dbus.SystemBus()
        adapter = find_adapter(bus)

        if not adapter:
            print('GattManager1 interface not found')
            return

        service_manager = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, adapter), GATT_MANAGER_IFACE)
        _keyboard = Application(bus)
        options['app'] = _keyboard
        options['report1'] = _keyboard.hidService.report1
        options['report2'] = _keyboard.hidService.report2
        #options['transfer'] = transfer

        print('Registering GATT application...')
        service_manager.RegisterApplication(options['app'].get_path(), {}, reply_handler=register_app_cb, error_handler=register_app_error_cb)

        # Enable ConnectSignal
        #threading.Thread(target=btDevice.enableConnectSignal, args=(onConnectSignal,)).start()
        #btleDevice.enableConnectSignal(onConnectSignal)
        #print(f'Connected: {btleDevice.isConnected()}')
        #onConnectSignal('org.bluez.Device1', {'Connected': btDevice.isConnected()})
        
        # Start btOutput event loop
        #print('start gattServer mainLoop')
        _mainloop = GLib.MainLoop()
        _mainloop.run()
    except:
        print('Abort gattServer: ', sys.exc_info()[0])
        traceback.print_exc()

##########################
#        MAIN
##########################
if __name__ == '__main__':
    start()
