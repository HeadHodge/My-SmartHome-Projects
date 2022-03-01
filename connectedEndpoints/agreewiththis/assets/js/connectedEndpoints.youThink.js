//##################################################
// 					Connected Chats
//                     youThink
//##################################################
console.log(`**** Load connectedChats.youThink.js ****`);

connectedChats.youThink = {

//################
//    globals
//################
_endpoint: undefined,
_options : undefined,

//################
//     start
//################
start: function() {
	console.log(`start youThink`);

	_options = connectedChats.options;
	_endpoint = connectedChats.endpoint;	

	_endpoint.start();

	document.querySelector('#addPost-button').addEventListener("click", connectedChats.endpoint.addPost);
}}

//################
//###   MAIN   ###
//################
	console.log(`**** Start connectedChat.js ****`);
	
	window.addEventListener("load", connectedChats.youThink.start);
