[Home](https://github.com/HeadHodge/smartHome-Projects-Main/blob/main/README.md)

# **My Smart Remotes Project:**

### **Overview:**

smartRemotes is a simple framework to control all the devices around my home with a variety of input control devices. It's to control many devices i.e. tv, stereo, shades, fireplace, lights, etc. with a single input control device, like a wireless usb remote control. Conversely it's to control a single device, like a fireplace, with a variety of input control devices, i.e. remote control, keyboard, wall mounted control panel, web page, etc.

My objective is to capture input from a variety of sources and use the input to control a variety of my home devices (mainly, but not totally, entertainment equipment).

**Main Features:**

Currently captures input from:
  - web pages
  - wired and wireless mice, keyboards, and remote controls
  - any HID compliant input device
  - wireless and other ip based data input.
  
<img src="./images/1.1.png" width="400px" height="auto"> 

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

### **How it Works:**

smartRemotes is a collection of python coded modules called 'nodes'. Each node performs a single task and is designed to be easily loaded and run independently of all other nodes.

A node performs only one of these three tasks:

 - Capture input control data from an input device, assign the input a single controlWord that represents the 'intent' of the input control data, then publish the controlWord to the home network via a websockets message.
 - Subscribe to one or more websocket messages and use the controlWord contained in the message to control a specified device.
 - Take published messages created by input nodes and route them to output nodes that have subscribed to the message to control a specific device.

The use of any available node is optional and independent of all other nodes **except** at least one router node must be running for the publish and subscribe of controlWord messages to work properly.

Nodes are loosely coupled to each other via your home network using websockets. This is a stateful communications protocol that is efficient for continuous data flow and is available on practically any platform you can think of. The ubiquitous nature of websockets along with the JSON standard for object serialization, makes for an easy convenient way to communicate between diverse systems.
