//##################################################
// 					Connected Chats
//                      endpoint
//##################################################
console.log(`**** Load connectedChat.endpoint.js ****`);
if(typeof connectedChats == 'undefined') connectedChats = {}; //js namespace
connectedChats.endpoint = {};

//////////////// BEGIN PRIVATE FUNCTION //////////////////////
(function(){ //invoke anonymous self executing function

//####################
//### Globals
//####################
var _options = undefined;
var _socket = undefined;
var _isConnected = undefined;
var _routeTable = undefined;
var _form = {};

//####################
//### openConnection
//####################
function openConnection(callBack) {
console.log("Enter openConnection");
endpoint = "wss://5des0hjizg.execute-api.us-west-2.amazonaws.com/production";

	if(_isConnected) return;
	
	//### new WebSocket ###
	_socket = new WebSocket(endpoint);
	
	//### onopen ###
	_socket.onopen = function(event) {
		console.log(`**** Endpoint Connected to: ${endpoint} ****`);
		_isConnected = true;
		if(callBack) callBack();
	};
	
	//### onmessage ###
	_socket.onmessage = function(event) {
		console.log(`Message from Endpoint: ${event.data}`);
		return;
		
		data = JSON.parse(event.data);
		
		if(data.lambda && data.state){
			if(_routeTable[data.lambda][data.state]) _routeTable[data.lambda][data.state]();
		}
	};
	
	//### onclose ###
	_socket.onclose = function(event) {
		console.log("****Endpoint Closed****");
		_socket = null;
		_isConnected = null;
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
//    getUts
//################
function getUts() {
console.log(`getUts`);
var now = new Date(), Id = '';
    
	now = new Date(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), now.getUTCHours(), now.getUTCMinutes(), now.getUTCSeconds()); 

	Id += (now.getYear()-100).toString();
	Id += (now.getMonth() < 9 ? '0' : '') + (now.getMonth()+1).toString(); // JS months are 0-based, so +1 and pad with 0's
	Id += ((now.getDate() < 10) ? '0' : '') + now.getDate().toString(); // pad with a 0
	Id += '.';
	Id += ((now.getHours() < 10) ? '0' : '') + now.getHours().toString(); // pad with a 0
	Id += ((now.getMinutes() < 10) ? '0' : '') + now.getMinutes().toString(); // pad with a 0
	Id += '.';
	Id += Math.floor(Math.random()*10000);
	//... etc... with .getHours(), getMinutes(), getSeconds(), getMilliseconds()
	
	return Id;
}

//################
// actionFailed
//################
function actionFailed() {
console.log(`actionFailed`);
}

//################
// alertEndpoints
//################
function alertEndpoints() {
console.log(`alertEndpoints`);
	
	_form = {
		'action'   : 'alertEndpoints',
		'uts'      : getUts(),
		'bucket'   : 'www.connectedchats.com',
	};
	
	payload = JSON.stringify(_form);

	console.log(`payload: ${payload}`);
	_socket.send(payload);
}

//################
//    addPost
//################
function addPost() {
console.log(`addPost`);

var _form = {
		'action'   : 'addPost',
		'bucket'   : 'www.connectedchats.com',
		'post'     : document.querySelector('#post-field').value,
		'category' : document.querySelector('#category-field').value,
		'name'     : document.querySelector('#name-field').value,
		'pin'      : document.querySelector('#pin-field').value,
		'email'    : document.querySelector('#email-field').value,
		'uts'      : getUts(),
	};
	
	_form = {
		'action'   : 'addPost',
		'uts'      : getUts(),
		'bucket'   : 'www.connectedchats.com',
		'post'     : 'Hello World',
		'category' : 'random thoughts',
		'name'     : 'HeadHodge',
		'pin'      : '92109',
		'email'    : 'bob@hodgemail.us',
	};
	
	payload = JSON.stringify(_form);

	console.log(`payload: ${payload}`);
	_socket.send(payload);
}

//################
//     start
//################
function start() {
console.log(`start endpoint`);

	_options = connectedChats.options;
	
	_routeTable = {
		'addPost' : [actionFailed, alertEndpoints],
	};
	
	openConnection();
}

//################
//     MAIN
//################

	connectedChats.endpoint.start = start;
	connectedChats.endpoint.addPost = addPost;
		
})();
/////////////////////////// END PRIVATE FUNCTION //////////////////////	

