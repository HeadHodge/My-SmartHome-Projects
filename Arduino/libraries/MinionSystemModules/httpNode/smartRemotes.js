//##################################################
// 					smartRemote
//##################################################

//////////////// BEGIN PRIVATE NAMESPACE //////////////////////
webRemote = (function(){ //invoke anonymous self executing function

//################
//### Global Variables
//################
//var _server = "ws://192.168.0.102:8080";
var _server = "ws://192.168.0.235:81";
var _masterBedroomBridge = "ws://192.168.0.219:81";
var _livingRoomBridge = "ws://192.168.0.235:81";

var _authToken = 'eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1YmM0ZGYxNGY4ZGE0MTdkYTNhZjdkNjkwYzg0NDQ2ZSIsImlhdCI6MTYxMzAxMDQ4MiwiZXhwIjoxOTI4MzcwNDgyfQ.MffxNYX4VssITLgdZBPilKTq3p4R9RuoQP2yeeoyyPw';
var _isKeyDown, _wasMapRecalled;
var _mouseDownTime=0, _mouseUpTime=0;
var	_case, _filter, _map, _shortcuts, _zone, _category, _skin, _commands, _overlay, _menu;
var _socket, _isConnected;
var _socket = undefined;
var _isConnected = undefined;

//################
//### openConnection
//################
function openConnection(callBack) {
console.log("Enter openConnection");

	if(_isConnected) return;
	
	//### new WebSocket ###
	_socket = new WebSocket(_server);
	
	//### onopen ###
	_socket.onopen = function(event) {
		console.log(`**** smartRemotes Server Connected to: ${_server} ****`);
		_isConnected = true;
		if(callBack) callBack();
	};
	
	//### onmessage ###
	_socket.onmessage = function(event) {
		console.log(`Message from smartRemotes: ${event.data}`);
	};
	
	//### onclose ###
	_socket.onclose = function(event) {
		console.log("****Server Closed****");
		_socket = null;
		_isConnected = null;
		openConnection();
	};

	//### onerror ###
	_socket.onerror = function(error) {
		console.log(`****Server Error: ${error.message}****`);
	};
}

//################
//### publishNote
//################
function publishNote(pNote) {
console.log(`Enter publishNote`);

	if(!_isConnected) return console.log(`Abort: smartRemotes not connected`);
/*
	note = JSON.stringify({
		'author': author,
		'title': title,        
  		'host': location.hostname,
		'time': new Date().getTime(),
        'settings': {'from': "webRemote", 'to': content.zone, 'keyWord': content.controlWord, 'keyModifier': parseInt(content.controlModifier)},
		'content': content
	});
 */
	note = JSON.stringify(pNote);
	console.log(`Note: ${note}`);
	_socket.send(note);
}

//################
//### getFile
//################
function getFile(fileName, callBack) {
var xhr = new XMLHttpRequest();

	xhr.open('GET', fileName);
	xhr.send();
	
	xhr.onload = function() {
		if (xhr.status != 200) return console.log(`Error ${xhr.status}: ${xhr.statusText}`); // e.g. 404: Not Found
		if(typeof callBack != 'undefined') callBack(xhr.response, fileName);
	};

	xhr.onerror = function() {
		notify("getFile Request failed");
	};
};

//################
//### loadSkin
//################
function loadSkin(content, filePath) {
console.log(`Enter loadSkin, filePath: ${filePath}`);
var Skin = document.querySelector('Skin.container');

	switch(filePath) {
	case '/skin/' + _skin.getAttribute('value')+'.htm':
		var keys;

		if(content) {
			Skin.setAttribute('htmfile', filePath);
			Skin.innerHTML=content;
		};

		keys = document.querySelectorAll('[key]');

		for(var button = 0; button < keys.length; button++) {
			keys[button].addEventListener("click", keyClicked);
        
            keys[button].addEventListener("mousedown", function(event) {
                _mouseDownTime = new Date().getTime();
            });
        
            keys[button].addEventListener("mouseup", function(event) {
                _mouseUpTime = new Date().getTime();
            });
		};
    };
};

//################
//### closeMenu
//################
function closeMenu(charCode) {
var option = document.querySelector('Overlay[open] Option[key="'+charCode+'"]');
var name = option.getAttribute('name'), value = option.getAttribute('value'), layout = option.getAttribute('layout'), text = option.getAttribute('value'); //, text = option.innerHTML;

	_overlay.removeAttribute("open");
	_zone.innerHTML = "";
	_skin.innerHTML = "";
	document.querySelector('Skin.container').innerHTML = '';

	switch(name) {
	case 'zone':
		if(value == 'exit') break;
		var skin = option.getAttribute('skin');
        
        if(value == 'livingRoom')
            _server = _livingRoomBridge
        else
            _server = _masterBedroomBridge
        
        console.log(`Server Address set to: ${_server}`);
		openConnection();

		_zone.setAttribute('value', value);
		_zone.setAttribute('text', value);
		_zone.innerHTML = value;
		_skin.setAttribute('value', skin);
		_skin.setAttribute('text', skin);
		_skin.innerHTML = skin;

		var filePath = '/skin/' + _skin.getAttribute('value')+'.htm';
		getFile(filePath, loadSkin);
		break;
	};

};

//################
//### openMenu
//################
function openMenu(optionName='zone', optionCategory='Zones') {
console.log(`Enter openMenu,  optionName: ${optionName}, optionCategory: ${optionCategory}`);
var options=document.querySelector(optionCategory);
var buttons;

	//Reset Selections
	if(optionName == 'zone') {
		_menu.removeAttribute('zoneValue');
		_menu.removeAttribute('zoneOption');
		_menu.removeAttribute('skinValue');
		_menu.removeAttribute('skinOption');
		_menu.removeAttribute('taskValue');
		_menu.removeAttribute('taskOption');
	};
	
	//Add Options
	if(!options) {
		options=document.querySelector('Notes[nooptions]');
		options.querySelector('option.text').innerHTML = '*** ERROR: '+optionName.toUpperCase()+' OPTIONS MISSING ***';
		options.querySelector('option.button').setAttribute('name', optionName);
		options.querySelector('option.button').setAttribute('value', 'none');
	};
	
	_menu.innerHTML = options.innerHTML;
	_overlay.setAttribute("open", "");

	buttons = _menu.querySelectorAll('Option.button');	
	for(var button = 0; button < buttons.length; button++) {
		buttons[button].addEventListener("click", keyClicked);
	};
};
  
//################
//### keyClicked
//################
function keyClicked(event) {
var button = event.currentTarget;
var keyCode = button.attributes['key'].nodeValue;
var options = {};
var keyModifer = 0;
var deviceTargets = 0;

    //alert(`${_mouseUpTime}, ${_mouseDownTime}, == ${_mouseUpTime - _mouseDownTime}`);

	if(!keyCode) return;
	options.keyDuration = _mouseUpTime - _mouseDownTime;
	//if(button.attributes['duration']) options.keyDuration = button.attributes['duration'].nodeValue;
	if(button.attributes['targets']) deviceTargets = button.attributes['targets'].nodeValue;
	if(button.attributes['modifier']) options.keyModifer = button.attributes['modifier'].nodeValue;
	if(button.attributes['delay']) options.keyDelay = button.attributes['delay'].nodeValue;
	if(button.attributes['code']) options.keyCode = button.attributes['code'].nodeValue;
	if(button.attributes['usage']) options.keyUsage = button.attributes['usage'].nodeValue;

	//Simulate Button Press
	if(button) {
		button.setAttribute("pressed", "");
		setTimeout(function(){button.removeAttribute("pressed");}, 200);
	}
	
	//Check for Input Key
	if(keyCode == '[') return openMenu();

	//Check for Open Menu
	if(document.querySelector('Overlay[open]')) return closeMenu(keyCode);

	publishNote({
        'time'          : new Date().getTime(),
        'source'        : 'webRemote',
        'location'      : 'masterBedroom',
        'targets'       : deviceTargets, //0-default, 1-blehubbridge, 2-usbhubbridge, 3-wshubbridge, 4-wsdevicebridge
        'event'         : 'keyPressed',       
        'settings'      : {'keyWord': keyCode},
        'options'       : options,
        'variables'     : {},
    });
};

//################
//### loadCase
//################
function loadCase(content, filePath=null) {
console.log(`Enter loadCase, filePath: ${filePath}`);

	if(content) document.querySelector('body').innerHTML += content;		
	
	switch(filePath) {
	case '/skin/case.htm':
		getFile("/menus/zones.htm", loadCase);
		break;
	case '/menus/zones.htm':
		var params = new URLSearchParams(window.location.search);

		_case = document.querySelector('Case');
		_commands = document.querySelector('Commands');
		_filter = document.querySelector('Filter');
		_map = document.querySelector('Map');
		_shortcuts = document.querySelector('Shortcuts');
		_overlay = document.querySelector('Overlay');
		_menu = document.querySelector('Menu');
		_zone = document.querySelector('#Zone');
		_category = document.querySelector('#Category');
		_skin = document.querySelector('#Skin');

		document.querySelector('commands > [key="["]').addEventListener("click", keyClicked);
		
		openMenu();
		break;
	default:
		//openConnection();
		if(!filePath) getFile("keypads/fireTvControl.htm", loadCase);
		break;
	};
};

//################
//###   MAIN   ###
//################

	console.log(`**** webRemote Object Loaded ****`);
	
	window.addEventListener("load", function(){
		loadCase();
	});

/////////////////////////// END PRIVATE NAMESPACE //////////////////////	
})();
