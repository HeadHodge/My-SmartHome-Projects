///////////////////////////////////////////////////////////
// 		hostMinion.js
///////////////////////////////////////////////////////////
console.log(`**** Load hostMinion.js ****`);

//// module globals ////
var _minionLogic = {};

///////////////////Create nameSpace////////////////////////
(function(_minionLogic){
///////////////////////////////////////////////////////////

//// nameSpace globals ////
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
	
	////// new WebSocket //////member
	_session = `${Date.now()}`;
	_endpoint = `wss://providers.minionLogic.com?member=${_minionLogic.identity.member}&nickname=${_minionLogic.identity.nickname}&namespace=${_minionLogic.identity.namespace}&pin=${_minionLogic.identity.pin}&session=${_session}`;
	_socket = new WebSocket(_endpoint);
	
	////// onopen //////
	_socket.onopen = function(event) {
		console.log(`\n**** Endpoint: ${_endpoint}, connected, waiting for orders ****`);
		_isConnected = true;
		_socket.send(JSON.stringify(_minionLogic.register));
	};
	
	////// onmessage //////
	_socket.onmessage = function(event) {
		console.log(`\n****Received Notice****`);
		
		//filter received data
		var notice = JSON.parse(event.data);
		
        console.log(notice);
        
        if(notice.SUBJECT != 'MINION-ORDER') return;
        
        var filledOrder = {};
        filledOrder.SUBJECT = notice.SUBJECT;
        filledOrder.TICKET = notice.TICKET;
        filledOrder.PRODUCT = _minionLogic.fill;
        filledOrder.REPORT = {
            progress: "FILLED",
			note    : "Minion Order Complete. Thank You for using minionLogic!",
        };

       _socket.send(JSON.stringify(filledOrder)); 
	};
	
	////// onerror //////
	_socket.onerror = function(error) {
		console.log(`\n****Endpoint Error: `, error);
	};
	
	////// onclose //////
	_socket.onclose = function(event) {
		console.log(`\n****Endpoint Closed****`);
		_socket = null;
		_isConnected = null;
		//location.reload();
	};
}

/////////////////////
//       MAIN
/////////////////////
	window.addEventListener("load", function(event) {
		console.log(`**** Start hostMinion ****`);
		
		//alert(`start hostMinion`);
		openConnection();
	});
	
})(_minionLogic);
/////////////////////////// END PRIVATE NAMESPACE //////////////////////	
