(function() {
  'use strict';

  hterm.defaultStorage = new lib.Storage.Local();

  var port;

  let textEncoder = new TextEncoder();

  let t = new hterm.Terminal();
  t.onTerminalReady = () => {
    console.log('Terminal ready.');
    let io = t.io.push();

    io.onVTKeystroke = str => {
      if (port !== undefined) {
        port.send(textEncoder.encode(str)).catch(error => {
          t.io.println('Send error: ' + error);
        });
      }
    };

    io.sendString = str => {
      if (port !== undefined) {
        port.send(textEncoder.encode(str)).catch(error => {
          t.io.println('Send error: ' + error);
        });
      }
    };
  };

  document.addEventListener('DOMContentLoaded', event => {
    let connectButton = document.querySelector('#connect');

    t.decorate(document.querySelector('#terminal'));
    t.setWidth(80);
    t.setHeight(24);
    t.installKeyboard();

    function connect() {
      t.io.println('Connecting to ' + port.device_.productName + '...');
      port.connect().then(() => {
        console.log(port);
        t.io.println('Connected.');
        connectButton.textContent = 'Disconnect';
        port.onReceive = data => {
          let textDecoder = new TextDecoder();
          t.io.print(textDecoder.decode(data));
        }
        port.onReceiveError = error => {
          t.io.println('Receive error: ' + error);
        };
      }, error => {
        t.io.println('Connection error: ' + error);
      });
    };

connectButton.addEventListener('click', async function() {
    let device
    const VENDOR_ID = 0x1A61
    
    try {
      alert('connect');

      //if ("serial" in navigator) alert('serial in navigator available');

      //const device = await navigator.serial.requestPort({
      //  filters: [{vendorId: 0x303A}]
      //})
      
      device = await navigator.usb.requestDevice({
        filters: [{vendorId: 0x303A}]
      })

      alert('open device')
      await device.open({ baudRate: 115200 })
      alert('device opened:')
      
      const reader = await device.readable.getReader();

      // Listen to data coming from the serial device.
        while (true) {
            const { value, done } = await reader.read();
            if (done) {
                // Allow the serial port to be closed later.
                await reader.releaseLock();
                break;
            }
        
            // value is a Uint8Array.
            console.log(value);
            }        
    } catch (error) {
      alert(error)
    }
	  
      await device.close()
  })
  
/*
    connectButton.addEventListener('click', function() {
      
        alert('connect');

        if ("serial" in navigator) alert('serial in navigator available');

        
      if (port) {
        port.disconnect();
        connectButton.textContent = 'Connect';
        port = null;
      } else {
        serial.requestPort().then(selectedPort => {
          port = selectedPort;
          connect();
        }).catch(error => {
          t.io.println('Connection error: ' + error);
        });
      }
    });
*/

    serial.getPorts().then(ports => {
      if (ports.length == 0) {
        t.io.println('No devices found.');
      } else {
        port = ports[0];
        connect();
      }
    });
  });
})();
