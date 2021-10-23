[Home](https://github.com/HeadHodge/smartHome-Projects-Main/blob/main/README.md)

# **My Virtual Bluetooth Keyboard Project**

My 1st endeavor into programming embedded mcu devices. I bought a Nordic semiconductors nRF52840 usb Dongle for $10 and use it to create a low power (ble) bluetooth peripheral on a stick to emulate HID keyboards, mice, and consumer control devices via 'HoG' (HID over Gatt)

<img src="https://www.rutronik.com/fileadmin//Rutronik/News/Knowledge/Produktmeldungen/09_SEP18-Nordic_Dongle_SOURCE_NORDIC.jpg" width="350">

## Overview:

I've found 3 ways of using the Zephyr RTOS for my virtual bluetooth keyboard peripheral that all work well...

1. Use the Zephyr support for a native_posix_x86 board to create a linux executable that runs on any linux x86 box and uses the linux kernel distributed hci bluetooth controller to use the onboard bluetooth soc.

2. Same as option 1 except provide your own usb hci controller and soc for any linux x86 box that doesnt have a supported onboard soc. <img src="./images/1.2.jpg" width="400px" height="auto"> https://www.amazon.com/dp/B08M1VJHVD?psc=1&ref=ppx_yo2_dt_b_product_details

3. Program the compiled image onto any Zephyr supported soc board, like the nRF52840 dongle. SupportedBoards:https://docs.zephyrproject.org/latest/boards/index.html#boards  
<img src="./images/1.1.jpg" width="400px" height="auto"> https://www.mouser.com/ProductDetail/Nordic-Semiconductor/nRF52840-Dongle?qs=gTYE2QTfZfTbdrOaMHWEZg%3D%3D&mgh=1&gclid=CjwKCAjw_L6LBhBbEiwA4c46uj8hwq3PdIPZfNVH8l0bPHUx-mP9aKUwK6NtwtzDSJV9iLa59FsNzRoCfj0QAvD_BwE 

I have a working nRF52840 dongle, but I'm personally using option 1 in my home using an Ubuntu server 20.04 on a BRIX mini <img src="./images/1.3.jpg" width="400px" height="auto"> https://www.amazon.com/dp/B07DMM7Z7N?psc=1&ref=ppx_yo2_dt_b_product_detail 

The BRIX comes with an Intel bluetooth/wifi soc that works really well with Ubuntu, as opposed to the junk broadcom/realtec socs that come with many cheapo chinese mini boxes.

More information: [Using the virtual bluetoothKeyboard](https://github.com/HeadHodge/My-SmartHome-Projects/tree/main/documents/Using-bluetoothKeyboard)
