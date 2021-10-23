[Home](https://github.com/HeadHodge/smartHome-Projects-Main/blob/main/README.md)

## Using smartRemotes:

Using smartRemotes should hopefully be relatively painless... :)

- Copy smartRemotes to your linux box (I'm always logged in as 'root' to avoid security issues)

- From the /smartRemotes/services directory, pick the nodes you want to use and copy its service file to the /etc/systemctld/service directory.

- use systemctl control command to start the node.

- From the /smartRemotes/scripts directory execute the .sh file that will display the status of the running node or use systemctl status command to do the same

- Each node has a configuration file in the /smartRemotes/nodes/xxxNode directory. Adjust settings and install missing python support modules as needed until the node is running properly

- **Note:** the hubNode is the only required Node, so make that your first Node to use

## Node Summary:

- **hub Node:** a router node that listens for published controlWord messages and routes them to all nodes that have subscribed to that published message

- **usb Node:** an input node that captures input from any connected usb HID device, i.e. wired/wireless keyboards and remote controls

- **http Node:** an input node that serves web pages to capture user control input

- **hog Node:** an output node that controls bluetooth devices via a HID over Gatt ble keyboard/media control/mouse peripheral 

- **tty Node:** an output node that does same thing as the hogNode except requires a Nordic nRF52840 usb dongle

- **ir Node:** an output node that controls Ir devices by blasting Ir codes to them

- **harmony Node:** an output node that controls Harmony Hubs via ip

- **hasssio Node:** an output node that  controls Hassio Hubs via ip

- **zone Node:** an output node that interprets published controlWords and re-publishes one or more new controlWords to effectively control multiple devices in a coordinated fashion
