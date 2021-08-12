# **my smartHome projects**

I have ongoing projects with a goal to automate my home with the latest and best technologies. It makes our lives a little more convenient and a little more secure. It's also a fun learning experience for me too.

I've put a lot of work into my projects and get excited when I figure out how to get something working, that up until then was a mystery to me. So I've provided the results of my work here, with the hope it may help others and that people may look at it and offer ways to improve on it.

I currently use a Hassio Hub to control my heat, blinds, lights, locks, and entertainment devices. I also use Abode to secure and protect my home. My code is mostly python, c, and javascript.

My current projects mostly focus on creating bridges to convert data from one format another and ways to communicate this data over a variety of medium.

## **my 'smartRemotes' project:**

The objective is to capture input from a variety of sources and use the input to control a variety of my home devices (mainly, but not totally, entertainment equipment).

**Main Features:**

Currently I capture input from:
  - web pages
  - wired and wireless mice, keyboards, and remote controls
  - any HID compliant input device
  - wireless and other ip based data input.

I use web sockets to transport this input around the home to the devices the input is intended to control.

I use 'bridges' to convert captured input from a variety of input devices to ip packets which are routed to other 'bridges' that convert the packets to formats required to control the intended device. 




