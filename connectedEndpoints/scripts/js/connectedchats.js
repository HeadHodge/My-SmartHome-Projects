//##################################################
// 					Connected Chats
//##################################################
connectedChats = {}; //js namespace
connectedChats.options = {}; //coniguration file

//////////////// BEGIN PRIVATE NAMESPACE //////////////////////
connectedChats.wsObj = (function(){ //invoke anonymous self executing function

//####################
//### Globals
//####################
var _options = undefined;
var _socket = undefined;
var _isConnected = undefined;

//####################
//### openConnection
//####################
function openConnection(callBack) {
console.log("Enter openConnection");

	if(_isConnected) return;
	
	//### new WebSocket ###
	_socket = new WebSocket(_options.endpoint);
	
	//### onopen ###
	_socket.onopen = function(event) {
		console.log(`**** Endpoint Connected to: ${_options.endpoint} ****`);
		_isConnected = true;
		if(callBack) callBack();
	};
	
	//### onmessage ###
	_socket.onmessage = function(event) {
		console.log(`Message from Endpoint: ${event.data}`);
	};
	
	//### onclose ###
	_socket.onclose = function(event) {
		console.log("****Endpoint Closed****");
		_socket = null;
		_isConnected = null;
		//openConnection();
	};

	//### onerror ###
	_socket.onerror = function(error) {
		console.log(`****Endpoint Error: ${error.message}****`);
	};
}

//################
//### listObjects
//################
function listObjects() {
console.log(`Enter listObjects`);

	if(!_isConnected) return console.log(`Abort: Not Connected`);

	post = JSON.stringify({
		'action': 'listObjects',
		'bucket': 'www.connectedchats.com',
	});

	console.log(`Post: ${post}`);
	_socket.send(post);
}

//################
//### publishNote
//################
function publishNote(author, title, content) {
console.log(`Enter publishNote, author: ${author}, title: ${title}, content: ${content}`);

	if(!_isConnected) return console.log(`Abort: smartRemotes not connected`);

	note = JSON.stringify({
		'author': author,
		'title': title,
		'host': location.hostname,
		'time': new Date().getTime(),
		'content': content
	});

	console.log(`Note: ${note}`);
	note = JSON.stringify({"action": "ping"})
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
		};

		break;
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

	if(!keyCode) return;
	
	//Simulate Button Press
	if(button) {
		button.setAttribute("pressed", "");
		setTimeout(function(){button.removeAttribute("pressed");}, 200);
	}
	
	//Check for Input Key
	if(keyCode == '[') return openMenu();

	//Check for Open Menu
	if(document.querySelector('Overlay[open]')) return closeMenu(keyCode);

	publishNote('htmlNode', 'control device request', {'controlWord': keyCode, 'zone': _zone.getAttribute('value')});
};

//################
//### startClient
//################
function startClient() {
console.log(`startClient`);

	_options = connectedChats.options;
	
	openConnection(listObjects);
}

//################
//###   MAIN   ###
//################

	console.log(`**** Start connectedChat.js ****`);
	
	window.addEventListener("load", startClient);

/////////////////////////// END PRIVATE NAMESPACE //////////////////////	
})();
