# **my smartHome projects**
I have ongoing projects with a goal to automate my home with the latest and best technologies. It makes our lives a little more convenient and a little more secure. It's also a fun learning experience for me too.

[bluetoothKeyboard](https://github.com/HeadHodge/smartHome-Projects-Main/blob/main/bluetoothKeyboard.md)

[smartRemotes](https://github.com/HeadHodge/smartHome-Projects-Main/blob/main/smartRemotes.md)

## **my 'smartRemotes' project:**

My objective is to capture input from a variety of sources and use the input to control a variety of my home devices (mainly, but not totally, entertainment equipment).

**Main Features:**

Currently I capture input from:
  - web pages
  - wired and wireless mice, keyboards, and remote controls
  - any HID compliant input device
  - wireless and other ip based data input.

I use web sockets to transport this input around the home to the devices the input is intended to control.

I use 'bridges' to convert captured input from a variety of input devices to ip packets which are routed to other 'bridges' that convert the packets to formats required to control the intended device. 

The main 'bridges' I've written and use are:

  - web sockets servers and clients to transfer data around my lan to their intended destinations with durable long lasting connections.
  - usb HID input data capture from any HID compliant device, i.e. wired/wireless keyboards, mice, and remote controls
  - Ird data capture from IR remotes and keypads
  - Ird blasters to transmit Ird to control IR controlled devices
  - ip based interaces to control various ip based devices, i.e. Sonos, Huntington shades, Wimo outlets, Hassio Hub
  - low power (ble) bluetooth peripheral servers to emulate HID keyboards, mice, and consumer control devices via 'HoG' (HID over Gatt) to control bluetooth based devices, i.e. FireTV stick

more details: https://github.com/HeadHodge/smartHome-Projects-Main/tree/main/smartRemotes

## **my 'hogKeyboard' project:**

My 1st endeavor into programming embedded mcu devices. I bought a Nordic semiconductors nRF52840 usb Dongle for $10 and use it to create a low power (ble) bluetooth peripheral on a stick to emulate HID keyboards, mice, and consumer control devices via 'HoG' (HID over Gatt)

<img src="https://www.rutronik.com/fileadmin//Rutronik/News/Knowledge/Produktmeldungen/09_SEP18-Nordic_Dongle_SOURCE_NORDIC.jpg" width="350">

more details: https://github.com/HeadHodge/smartHome-Projects-Main/tree/main/hogKeyboard


