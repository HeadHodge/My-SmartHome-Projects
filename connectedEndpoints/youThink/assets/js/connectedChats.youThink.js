//##################################################
// 					Connected Chats
//##################################################

//################
//# youThink
//################
connectedChats.youThink = {
//################
//# globals
//################
_core   : undefined,
_options: undefined,

//################
//# startClient
//################
startClient: function() {
	console.log(`startClient`);

	_core = connectedChats.core;
	_options = connectedChats.options;
	
	//openConnection(listObjects);
	
	_core.startCore();
}
}

//################
//###   MAIN   ###
//################

	console.log(`**** Start connectedChat.js ****`);
	
	window.addEventListener("load", connectedChats.youThink.startClient);

