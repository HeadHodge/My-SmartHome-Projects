///////////////////////////////////////////////////////////
// 		              openTicket
///////////////////////////////////////////////////////////
console.log(`**** Load openTicket.js ****`);

///module globals
var _minionLogic = {};

///////////////////Create nameSpace////////////////////////
(function(_minionLogic){
///////////////////////////////////////////////////////////

///nameSpace globals
var _socket = undefined;
var _isConnected = undefined;
var _session = undefined;
var _endpoint = undefined;

///////////////////////////////////////////////////////////////////
function openConnection() {
///////////////////////////////////////////////////////////////////
console.log("openConnection");
var endpoint;

	if(_isConnected) return;
	
	///new WebSocket
	_session = `${Date.now()}`;
	_endpoint = `wss://clients.minionLogic.com?nickname=${_minionLogic.identity.nickname}&member=${_minionLogic.identity.member}&pin=${_minionLogic.identity.pin}&session=${_session}`;
	_socket = new WebSocket(_endpoint);
	
	///onopen
	_socket.onopen = function(event) {
		console.log(`\n**** Endpoint: ${_endpoint}, connected, send ticket ****`);
		_isConnected = true;
		_socket.send(JSON.stringify(_minionLogic.ticket));
	};
	
	///onmessage
	_socket.onmessage = function(event) {
		console.log(`\n****Received: `, event.data);
		
		//filter received data
		var ticket = JSON.parse(event.data);
		
		if(ticket.MINION.Product){
			installProduct(ticket);
		}
	};
	
	///onerror
	_socket.onerror = function(error) {
		console.log(`\n****Endpoint Error: `, error);
	};
	
	///onclose
	_socket.onclose = function(event) {
		console.log(`\n****Endpoint Closed****`);
		_socket = null;
		_isConnected = null;
	};
}

///////////////////////////////////////////////////////////////////
var installProduct = async function(ticket) {
///////////////////////////////////////////////////////////////////
console.log(`installProduct`);

var nameSpace = ticket.MINION.Sku.split('_')[0];
var content = ``;

//validate session
/*
	if(ticket.TICKET.session != _session){
		console.log(ticket.TICKET.session + ',' + _session);
		console.log(`********************************`);
		console.log(`***ABORT: CONTENT HAS EXPIRED***`);
		console.log(`********************************`);
		return;
	};
*/
	
//create shadow host
	var div = document.createElement('div');
	var root = div.attachShadow({mode: 'open'});
    div.id = nameSpace;
	document.body.appendChild(div);
	
//create style
	var style = document.createElement('style');
	style.textContent = ticket.MINION.Product.Style;
	root.innerHTML = style.outerHTML + ticket.MINION.Product.View;

//create private/public script
	var script = document.createElement('script');
	script.type = 'text/javascript';
	
	var content = ``;
	content += `console.log('${ticket.MINION.Product.Console}');\r\n`;
	content += `var ${nameSpace} = {};\r\n`;
	content += `(function($publicObject) {\r\n`;
	content += `${ticket.MINION.Product.Script}\r\n`;
	content += `})(${nameSpace});\r\n`;
	script.textContent = content;
	
	root.appendChild(script);
};

/////////////////////
//       MAIN
/////////////////////
	window.addEventListener("load", function(event) {
		console.log(`**** Start openTicket ****`);
		
		//alert(`start openTicket`);
		openConnection();
	});
	
})(_minionLogic);
/////////////////////////// END PRIVATE NAMESPACE //////////////////////	
