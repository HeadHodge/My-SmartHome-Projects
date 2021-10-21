# Using My Bluetooth Keyboard
## Overview:

I've found 3 ways of using the Zephyr rtos for my virtual bluetooth keyboard peripheral that all work well...

1. Use the Zephyr support for a native_posix_x86 board to create a linux executable that runs on any linux x86 box and uses the linux kernel distributed hci bluetooth controller to use the onboard bluetooth soc.

2. Same as option 1 except provide your own usb hci controller and soc for any linux x86 box that doesnt have a supported onboard soc. <img src="./images/1.2.jpg" width="480px" height="auto"> https://www.amazon.com/dp/B08M1VJHVD?psc=1&ref=ppx_yo2_dt_b_product_details

3. Program the compiled image onto any Zephyr supported soc board, like the nRF52840 dongle.. <img src="./images/1.1.jpg" width="480px" height="auto"> https://docs.zephyrproject.org/latest/boards/index.html#boards  

I have a working nRF52840 dongle, but I'm personally using option 1 in my home using Ubuntu server 20.04 on a BRIX mini <img src="./images/1.3.jpg" width="480px" height="auto"> https://www.amazon.com/dp/B07DMM7Z7N?psc=1&ref=ppx_yo2_dt_b_product_detail The BRIX comes with an Intel bluetooth/wifi soc that works really well with Ubuntu, as opposed to the junk broadcom/realtec socs that come with many cheapo chinese mini boxes.
