(function() {
    
var options = [
        `SETOPTIONS:{memKey: "NAV_HOME",      memMap: "consumer", memValue: {keyWord: "NAV_HOME", keyModifier: 0, keyCode: ${parseInt("223", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_BACK",      memMap: "consumer", memValue: {keyWord: "NAV_BACK", keyModifier: 0, keyCode: ${parseInt("224", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_MENU",      memMap: "consumer", memValue: {keyWord: "NAV_MENU", keyModifier: 0, keyCode: ${parseInt("40", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_SELECT",    memMap: "consumer", memValue: {keyWord: "NAV_SELECT", keyModifier: 0, keyCode: ${parseInt("41", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_UP",        memMap: "consumer", memValue: {keyWord: "NAV_UP", keyModifier: 0, keyCode: ${parseInt("42", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_DOWN",      memMap: "consumer", memValue: {keyWord: "NAV_DOWN", keyModifier: 0, keyCode: ${parseInt("43", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_LEFT",      memMap: "consumer", memValue: {keyWord: "NAV_LEFT", keyModifier: 0, keyCode: ${parseInt("44", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "NAV_RIGHT",     memMap: "consumer", memValue: {keyWord: "NAV_RIGHT", keyModifier: 0, keyCode: ${parseInt("45", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "VOLUME_MUTE",   memMap: "consumer", memValue: {keyWord: "VOLUME_MUTE", keyModifier: 0, keyCode: ${parseInt("e2", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "VOLUME_UP",     memMap: "consumer", memValue: {keyWord: "VOLUME_UP", keyModifier: 0, keyCode: ${parseInt("e9", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "VOLUME_DOWN",   memMap: "consumer", memValue: {keyWord: "VOLUME_DOWN", keyModifier: 0, keyCode: ${parseInt("ea", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "MEDIA_PLAY",    memMap: "consumer", memValue: {keyWord: "MEDIA_PLAY", keyModifier: 0, keyCode: ${parseInt("cd", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "MEDIA_REVERSE", memMap: "consumer", memValue: {keyWord: "MEDIA_REVERSE", keyModifier: 0, keyCode: ${parseInt("b4", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "MEDIA_FORWARD", memMap: "consumer", memValue: {keyWord: "MEDIA_FORWARD", keyModifier: 0, keyCode: ${parseInt("b3", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "VOICE_QUERY",   memMap: "consumer", memValue: {keyWord: "VOICE_QUERY", keyModifier: 0, keyCode: ${parseInt("21", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
        `SETOPTIONS:{memKey: "DEVICE_POWER",  memMap: "consumer", memValue: {keyWord: "DEVICE_POWER", keyModifier: 0, keyCode: ${parseInt("30", 16)}, keyMap: "consumer", keyDuration: 5, keyDelay: 5}}`,
    ];

'use strict';
var textEncoder;
var writableStreamClosed;
var writer, reader;
var connect, vid, pid, send, message;
var enc = new TextEncoder(); // always utf-8
var selectedOption = 0;
var maxOption = options.length;

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

var sendMessage = async function() {
    let message = document.querySelector('#message').value;
    if(message.length == 0) message = "";
    message += '\0';
    
    //console.log(`write: ${new Int8Array([0x03])}`);   
    //await writer.write(enc.encode(message))             
    //await writer.write(new Int8Array([0xFF, 0xFF]));

    var enc = new TextEncoder(); // always utf-8

    await writer.write(new Int8Array([3]));
        
    //await writer.write(new Int8Array([3,1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,255,255]));
    //await writer.write(new Int8Array([1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26]));
    //await writer.write(new Int8Array([1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,0,26,17,122,1,165,0,102,0,26,17,122,1,165,0,102,0,26]));

    sleep(10);
    await writer.write(new Int8Array([
                 1,164,0,206,0,26,0,23,0,26,0,23,
         0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,
         0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,
         0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,
         0,26
    ]));
    //sleep(100);
    await writer.write(new Int8Array([
             0,74,0,26,0,74,0,26,0,23,0,26,0,23,
        0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,
        0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,
        0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74
    ]));
    await writer.write(new Int8Array([        
        0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,
        0,26,17,122,1,165,0,102,0,26,17,122,1,165,0,102,
        0,26
    ]));
        
    await writer.write(new Int8Array([255,255]));
    //await writer.write(new Int8Array([0, 0, 0, 0, 0, 115, 38, 37]));
    var { value, done } = await reader.read();
    console.log(value);
    console.log(done);
        
    //leep(100);
    //const { value, done } = await reader.read();
    console.log('done');
    /*
  await writer.write(new Int8Array([
        1,164,0,206,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0
    ]));
    sleep(100);
    
    await writer.write(new Int8Array([
        26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0
    ]));
    sleep(100);
    
    await writer.write(new Int8Array([
        74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,23,0
    ]));
    sleep(100);
    
    await writer.write(new Int8Array([
        26,0,23,0,26,0,23,0,26,0,23,0,26,0,74,0,26,0,74,0,26,0,74,0,26,0,23,0,26,0
    ]));
    sleep(100);
    
    await writer.write(new Int8Array([
        74,0,26,0,74,0,26,0,74,0,26,0,74,0,26,7,102,1,165,0,102,0,26,17,122,1,165,0
    ]));
    sleep(100);
    
    await writer.write(new Int8Array([
        102,0,26,17,122,1,165,0,102,0,26
    ]));
    sleep(100);
*/
}

var sendOption = async function() {
  console.log(`sendOption: selectedOption: ${selectedOption}`);

    if(selectedOption == 0) {
        if(confirm(`Option: ` + (selectedOption + 1) + ` of: ` + options.length + `\r\n\r\n` + options[selectedOption]) == false) return;
        log.value = ''; //clear log 
    }
    
    await writer.write(enc.encode(options[selectedOption]));
    
    ++selectedOption;
    if(selectedOption >= maxOption) selectedOption = 0; 
    //selectedOption = 0; 
}

var receivedNotify = async function(pNotifyObj) {
  console.log(`receivedNotify command: ${pNotifyObj.Command}, selectedOption: ${selectedOption}`);
    
    if(selectedOption == 0) return;
    await sendOption();
}
 
var connectDevice = async function() {
    let device
   
    try {
        const filter = { usbVendorId: vid.value, usbProductId: pid.value };
        const device = await navigator.serial.requestPort({ filters: [filter] });

        await device.open({ baudRate: 115200, dataBits: 8, stopBits: 1, parity: "none", flowControl: "none" })
               
        alert('device opened:')
        vid.disabled = true;
        pid.disabled = true;
        connect.disabled = true;
        message.disabled = false;
        send.disabled = false;
        set.disabled = false;

        //const textDecoder = new TextDecoderStream();
        //const readableStreamClosed = device.readable.pipeTo(textDecoder.writable);
        //const reader = textDecoder.readable.getReader();
        const log = document.getElementById('log');

    //read
        reader = device.readable.getReader();
      
    //write
        writer = device.writable.getWriter();
        
        //await writer.write(new Int8Array([0, 0, 0, 0, 0]));
        await writer.write(new Int8Array([0, 0, 0, 0, 0, 115, 38, 37]));
        //await writer.write(new Int8Array([0, 0, 0, 0, 0, 115, 36, 37, 38]));

    // Listen to data coming from the serial device.
        var index = 0, offset = 0, line = '';
/*        
        while (true) {
          const { value, done } = await reader.read();
          log.value += value;
            
            //var enc = new TextEncoder(); // always utf-8
            //console.log(enc.encode("This is a string converted to a Uint8Array"));
            
            //for (var i=0; i<value.length; i++) {
            //    log.value += `0x${value.charCodeAt(i).toString(16)} `;
            //}

            //log.value += enc.encode(value);
            
            if (done) {
                // Allow the serial device to be closed later. 
                await reader.releaseLock();
                break;
            }

            continue;
         
            while (true) {
                offset = log['value'].substr(index).indexOf('\n');
                if(offset == -1) break;
                
                console.log(index + ', ' + offset + ', ' + log['value'].substring(index, index+offset));
                line = log['value'].substring(index, index+offset);
                index += offset + 1;
                offset = 0;
                
                if(line.includes("NOTIFY:"))
                    await receivedNotify(JSON.parse(line.substr("NOTIFY:".length)));              
            }                                   

            if (done) {
                // Allow the serial device to be closed later. 
                await reader.releaseLock();
                break;
            }
        }
 */
        
    } catch (error) {
      alert(error)
    }
/*	  
    connect.disabled = false;
    send.disabled = true;
    message.disabled = true;
    await device.close();
*/
};

document.addEventListener('DOMContentLoaded', event => {
    let connectButton = document.querySelector('#connect');
    let sendButton = document.querySelector('#send');
    let setButton = document.querySelector('#set');
    
    vid = document.getElementById("vid");
    pid = document.getElementById("pid");
    connect = document.getElementById("connect");
    message = document.getElementById("message");
    send    = document.getElementById("send");
    set     = document.getElementById("set");
    
    vid.value = '0x04D8'  //irdroid 
    pid.value = '0xFD08' //usbSerial-IR 
    message.disabled = true;
    send.disabled = true;
    set.disabled = true;
    
    connectButton.addEventListener('click', connectDevice);
    sendButton.addEventListener('click', sendMessage);
    setButton.addEventListener('click', sendOption);
});
})();