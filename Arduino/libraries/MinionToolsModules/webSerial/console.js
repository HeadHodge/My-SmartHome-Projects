(function() {
    
/*
std::map<std::string, hidKey>keyMap = {
    {"a",  {"keyboard", 0, 0x0004, "a", "{}", 5, 5, 0x001d}}, 
    {"b",  {"keyboard", 0, 0x0005, "b", "{}", 5, 5, 0x001e}}, 
    {"c",  {"keyboard", 0, 0x0006, "c", "{}", 5, 5, 0x001f}}, 
    {"d",  {"keyboard", 0, 0x0007, "d", "{}", 5, 5, 0x0020}}, 
    {"e",  {"keyboard", 0, 0x0008, "e", "{}", 5, 5, 0x0021}}, 
    {"f",  {"keyboard", 0, 0x0009, "f", "{}", 5, 5, 0x0022}}, 
    {"g",  {"keyboard", 0, 0x000a, "g", "{}", 5, 5, 0x0023}}, 
    {"h",  {"keyboard", 0, 0x000b, "h", "{}", 5, 5, 0x0024}}, 
    {"i",  {"keyboard", 0, 0x000c, "i", "{}", 5, 5, 0x0025}}, 
    {"j",  {"keyboard", 0, 0x000d, "j", "{}", 5, 5, 0x0026}}, 
    {"k",  {"keyboard", 0, 0x000e, "k", "{}", 5, 5, 0x0027}}, 
    {"l",  {"keyboard", 0, 0x000f, "l", "{}", 5, 5, 0x0028}}, 
    {"m",  {"keyboard", 0, 0x0010, "m", "{}", 5, 5, 0x0029}}, 
    {"n",  {"keyboard", 0, 0x0011, "n", "{}", 5, 5, 0x002a}}, 
    {"o",  {"keyboard", 0, 0x0012, "o", "{}", 5, 5, 0x002b}}, 
    {"p",  {"keyboard", 0, 0x0013, "p", "{}", 5, 5, 0x002c}}, 
    {"q",  {"keyboard", 0, 0x0014, "q", "{}", 5, 5, 0x002d}}, 
    {"r",  {"keyboard", 0, 0x0015, "r", "{}", 5, 5, 0x002e}}, 
    {"s",  {"keyboard", 0, 0x0016, "s", "{}", 5, 5, 0x002f}}, 
    {"t",  {"keyboard", 0, 0x0017, "t", "{}", 5, 5, 0x0030}}, 
    {"u",  {"keyboard", 0, 0x0018, "u", "{}", 5, 5, 0x0031}}, 
    {"v",  {"keyboard", 0, 0x0019, "v", "{}", 5, 5, 0x0032}}, 
    {"w",  {"keyboard", 0, 0x001a, "w", "{}", 5, 5, 0x0033}}, 
    {"x",  {"keyboard", 0, 0x001b, "x", "{}", 5, 5, 0x0034}}, 
    {"y",  {"keyboard", 0, 0x001c, "y", "{}", 5, 5, 0x0035}}, 
    {"z",  {"keyboard", 0, 0x001d, "z", "{}", 5, 5, 0x0036}},

    
    {"A",  {"keyboard", 2, 0x0004, "A", "{}", 5, 5, 0x001d}}, 
    {"B",  {"keyboard", 2, 0x0005, "B", "{}", 5, 5, 0x001e}}, 
    {"C",  {"keyboard", 2, 0x0006, "C", "{}", 5, 5, 0x001f}}, 
    {"D",  {"keyboard", 2, 0x0007, "D", "{}", 5, 5, 0x0020}}, 
    {"E",  {"keyboard", 2, 0x0008, "E", "{}", 5, 5, 0x0021}}, 
    {"F",  {"keyboard", 2, 0x0009, "F", "{}", 5, 5, 0x0022}}, 
    {"G",  {"keyboard", 2, 0x000a, "G", "{}", 5, 5, 0x0023}}, 
    {"H",  {"keyboard", 2, 0x000b, "H", "{}", 5, 5, 0x0024}}, 
    {"I",  {"keyboard", 2, 0x000c, "I", "{}", 5, 5, 0x0025}}, 
    {"J",  {"keyboard", 2, 0x000d, "J", "{}", 5, 5, 0x0026}}, 
    {"K",  {"keyboard", 2, 0x000e, "K", "{}", 5, 5, 0x0027}}, 
    {"L",  {"keyboard", 2, 0x000f, "L", "{}", 5, 5, 0x0028}}, 
    {"M",  {"keyboard", 2, 0x0010, "M", "{}", 5, 5, 0x0029}}, 
    {"N",  {"keyboard", 2, 0x0011, "N", "{}", 5, 5, 0x002a}}, 
    {"O",  {"keyboard", 2, 0x0012, "O", "{}", 5, 5, 0x002b}}, 
    {"P",  {"keyboard", 2, 0x0013, "P", "{}", 5, 5, 0x002c}}, 
    {"Q",  {"keyboard", 2, 0x0014, "Q", "{}", 5, 5, 0x002d}}, 
    {"R",  {"keyboard", 2, 0x0015, "R", "{}", 5, 5, 0x002e}}, 
    {"S",  {"keyboard", 2, 0x0016, "S", "{}", 5, 5, 0x002f}}, 
    {"T",  {"keyboard", 2, 0x0017, "T", "{}", 5, 5, 0x0030}}, 
    {"U",  {"keyboard", 2, 0x0018, "U", "{}", 5, 5, 0x0031}}, 
    {"V",  {"keyboard", 2, 0x0019, "V", "{}", 5, 5, 0x0032}}, 
    {"W",  {"keyboard", 2, 0x001a, "W", "{}", 5, 5, 0x0033}}, 
    {"X",  {"keyboard", 2, 0x001b, "X", "{}", 5, 5, 0x0034}}, 
    {"Y",  {"keyboard", 2, 0x001c, "Y", "{}", 5, 5, 0x0035}}, 
    {"Z",  {"keyboard", 2, 0x001d, "Z", "{}", 5, 5, 0x0036}},

    
    {"1",  {"keyboard", 0, 0x001e, "1", "{}", 5, 5, 0x0008}}, 
    {"2",  {"keyboard", 0, 0x001f, "2", "{}", 5, 5, 0x0009}}, 
    {"3",  {"keyboard", 0, 0x0020, "3", "{}", 5, 5, 0x000a}}, 
    {"4",  {"keyboard", 0, 0x0021, "4", "{}", 5, 5, 0x000b}}, 
    {"5",  {"keyboard", 0, 0x0022, "5", "{}", 5, 5, 0x000c}}, 
    {"6",  {"keyboard", 0, 0x0023, "6", "{}", 5, 5, 0x000d}}, 
    {"7",  {"keyboard", 0, 0x0024, "7", "{}", 5, 5, 0x000e}}, 
    {"8",  {"keyboard", 0, 0x0025, "8", "{}", 5, 5, 0x000f}}, 
    {"9",  {"keyboard", 0, 0x0026, "9", "{}", 5, 5, 0x0010}}, 
    {"0",  {"keyboard", 0, 0x0027, "0", "{}", 5, 5, 0x0007}},
}; 
*/    
    
/*  
  
`SETOPTIONS:{memKey: "", keyWord: "", keyModifier: 0, keyCode: , keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`

`SETOPTIONS:{memKey: "NAV_BACK", keyWord: "NAV_BACK", keyModifier: 0, keyCode: 0x224, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "NAV_MENU", keyWord: "NAV_MENU", keyModifier: 0, keyCode: 0x40, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "NAV_SELECT", keyWord: "NAV_SELECT", keyModifier: 0, keyCode: 0x41, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "NAV_UP", keyWord: "NAV_UP", keyModifier: 0, keyCode: 0x42, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "NAV_DOWN", keyWord: "NAV_DOWN", keyModifier: 0, keyCode: 0x43, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`

`SETOPTIONS:{memKey: "NAV_LEFT", keyWord: "NAV_LEFT", keyModifier: 0, keyCode: 0x44, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "NAV_RIGHT", keyWord: "NAV_RIGHT", keyModifier: 0, keyCode: 0x45, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "VOLUME_MUTE", keyWord: "VOLUME_MUTE", keyModifier: 0, keyCode: 0xe2, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "VOLUME_UP", keyWord: "VOLUME_UP", keyModifier: 0, keyCode: 0xe9, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "VOLUME_DOWN", keyWord: "VOLUME_DOWN", keyModifier: 0, keyCode: 0xea, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`

`SETOPTIONS:{memKey: "MEDIA_PLAYTOGGLE", keyWord: "MEDIA_PLAYTOGGLE", keyModifier: 0, keyCode: 0xcd, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "MEDIA_REVERSE", keyWord: "MEDIA_REVERSE", keyModifier: 0, keyCode: 0xb4, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "MEDIA_FORWARD", keyWord: "MEDIA_FORWARD", keyModifier: 0, keyCode: 0xb3, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "VOICE_QUERY", keyWord: "VOICE_QUERY", keyModifier: 0, keyCode: 0x21, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`
`SETOPTIONS:{memKey: "DEVICE_POWERTOGGLE", keyWord: "DEVICE_POWERTOGGLE", keyModifier: 0, keyCode: 0x30, keyUsage: "consumer", keyDuration: 5, keyDelay: 5}`

    {"NAV_HOME",  {"consumer", 0, 0x223, "NAV_HOME", "{}", 5, 5, 0x0000}},    
    {"NAV_BACK",  {"consumer", 0, 0x224, "NAV_BACK", "{}", 5, 5, 0x0000}}, 
    {"NAV_MENU",  {"consumer", 0, 0x40, "NAV_MENU", "{}", 5, 5, 0x0000}}, 
    {"NAV_SELECT",  {"consumer", 0, 0x41, "NAV_SELECT", "{}", 5, 5, 0x0000}}, 
    {"NAV_UP",  {"consumer", 0, 0x42, "NAV_UP", "{}", 5, 5, 0x0000}},
    {"NAV_DOWN",  {"consumer", 0, 0x43, "NAV_DOWN", "{}", 5, 5, 0x0000}}, 

    {"NAV_LEFT",  {"consumer", 0, 0x44, "NAV_LEFT", "{}", 5, 5, 0x0000}}, 
    {"NAV_RIGHT",  {"consumer", 0, 0x45, "NAV_RIGHT", "{}", 5, 5, 0x0000}}, 
    {"VOLUME_MUTE",  {"consumer", 0, 0xe2, "VOLUME_MUTE", "{}", 5, 5, 0x0000}}, 
    {"VOLUME_UP",  {"consumer", 0, 0xe9, "VOLUME_UP", "{}", 5, 5, 0x0000}}, 
    {"VOLUME_DOWN",  {"consumer", 0, 0xea, "VOLUME_DOWN", "{}", 5, 5, 0x0000}}, 

    {"MEDIA_PLAYTOGGLE",  {"consumer", 0, 0xcd, "MEDIA_PLAYTOGGLE", "{}", 5, 5, 0x0000}}, 
    {"MEDIA_REVERSE",  {"consumer", 0, 0xb4, "MEDIA_REVERSE", "{}", 5, 5, 0x0000}}, 
    {"MEDIA_FORWARD",  {"consumer", 0, 0xb3, "MEDIA_FORWARD", "{}", 5, 5, 0x0000}}, 
    {"VOICE_QUERY",  {"keyboard", 0, 0x21, "VOICE_QUERY", "{}", 5, 5, 0x0000}}, 
    {"DEVICE_POWERTOGGLE",  {"consumer", 0, 0x30, "DEVICE_POWERTOGGLE", "{}", 5, 5, 0x0000}}, 
*/
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
var writer;
var connect, vid, pid, send, message;
var enc = new TextEncoder(); // always utf-8
var selectedOption = 0;
var maxOption = options.length;

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

var sendMessage = async function() {
    let message = document.querySelector('#message').value;
    if(message.length == 0) return alert(`Abort: Message value missing`);
    message += '\0';
    
    //console.log(await enc.encode(message));                
    await writer.write(enc.encode(message));      
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

        await device.open({ baudRate: 115200 })
               
        alert('device opened:')
        vid.disabled = true;
        pid.disabled = true;
        connect.disabled = true;
        message.disabled = false;
        send.disabled = false;
        set.disabled = false;
      
    //write
        writer = device.writable.getWriter();

    //read
        const textDecoder = new TextDecoderStream();
        const readableStreamClosed = device.readable.pipeTo(textDecoder.writable);
        const reader = textDecoder.readable.getReader();
        const log = document.getElementById('log');

    // Listen to data coming from the serial device.
        var index = 0, offset = 0, line = '';
        
        while (true) {
          const { value, done } = await reader.read();
            
            log.value += value;
            
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
        
    } catch (error) {
      alert(error)
    }
	  
    connect.disabled = false;
    send.disabled = true;
    message.disabled = true;
    await device.close();
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
    
    vid.value = '0x303A'  //unexpected manufacturing 
    pid.value = '0x80D6' //featherS3
    message.disabled = true;
    send.disabled = true;
    set.disabled = true;
    
    connectButton.addEventListener('click', connectDevice);
    sendButton.addEventListener('click', sendMessage);
    setButton.addEventListener('click', sendOption);
});
})();