                    **** smartRemotes ****

<b>Overview:</b>

smartRemotes is a collection of modules to capture remote key codes and scan codes and interface the input to applications that need control input. The 1st interface written to date, captures key codes from usb hid devices (i.e. wireless usb keyboards) and interfaces the input to the Home-Assistant service bus to control smart home devices.

The main modules are loosely coupled to each other via network communications using websockets. This is a stateful communications protocol that is efficient for continuous data flow and is available on practically any platform you can think of. The ubiquitos nature of websockets along with the JSON standard for object serializaton, makes for an easy convient way to communicate between diverse systems.

<b>Module Descriptions:</b>
    
<b>_usb2keyCode:</b>

   Captures usb serial hid device (i.e. usb wired/wireless keyboards) scan code input, converts data to a standard key code format and offloads the results to the keyCode2hassio module. 
    
<b>myCode:</b>

Captures key code input from html browser pages, then converts to a standard key format and offloads the result to the keyCode2hassio hub.
    
<b>_keyCode2hassio:</b>

Standard keyCodes are captured from other modules via websockets. The keyCodes are transformed into the proper format needed to control the Hassio hub's attached smart devices using their websockets API.

<b>_keyCode2bt:</b>

This module is basically a virtual/emulated bluetooth keyboard. It captures keyCodes from the Hassio Hub using their websockets API and sends the keyCodes to any device connected via bluetooth thats accepts HID bluetooh keyboard input. There are several other sub-modules to facilitate device bluetooth pairing.
 
