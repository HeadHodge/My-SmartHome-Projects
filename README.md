# **my smartHome projects**

I have ongoing projects with a goal to automate my home with the latest and best technologies. It makes our lives a little more convenient and a little more secure. It's also a fun learning experience for me too.

I've put a lot of work into my projects and get excited when I figure out how to get something working, that up until then was a mystery to me. So I've provided the results of my work here, with the hope it may help others and that people may look at it and offer ways to improve on it.

I currently use a Hassio Hub to control my heat, blinds, lights, locks, and entertainment devices. I also use Abode to secure and protect my home. My code is mostly python, c, and javascript.

My current projects mostly focus on creating bridges to convert data from one format to another and ways to communicate this data over a variety of medium.

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
  - ip based interaces to control various ip based devices, i.e. Sonos, Huntington shades, Wimo outlets
  - low power (ble) bluetooth peripheral servers to emulate HID keyboards, mice, and consumer control devices via 'HoG' (HID over Gatt) to control bluetooth based devices, i.e. FireTV stick




