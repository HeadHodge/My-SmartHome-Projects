var smartRemotes = {
/////////////////////////// BEGIN PRIVATE NAMESPACE //////////////////////

//################
//### Global Variables
//################
_server: "ws://192.168.0.162:8080",
_socket: undefined,
_isConnected: undefined,

//################
//### openConnection
//################
openConnection: function(callBack) {
console.log("Enter openConnection");

	if(smartRemotes._isConnected) return;
	
	//### new WebSocket ###
	smartRemotes._socket = new WebSocket(smartRemotes._server);
	
	//### onopen ###
	smartRemotes._socket.onopen = function(event) {
		console.log("**** smartRemotes Server Connected ****");
		smartRemotes._isConnected = true;
		if(callBack) callBack();
	};
	
	//### onmessage ###
	smartRemotes._socket.onmessage = function(event) {
		console.log(`Message from smartRemotes: ${event.data}`);
	};
	
	//### onclose ###
	smartRemotes._socket.onclose = function(event) {
		console.log("****Server Closed****");
		smartRemotes._socket = null;
		smartRemotes._isConnected = null;
		smartRemotes.openConnection();
	};

	//### onerror ###
	smartRemotes._socket.onerror = function(error) {
		console.log(`****Server Error: ${error.message}****`);
		//smartRemotes._socket = null;
		//smartRemotes._isConnected = null;
		//smartRemotes.openConnection();
	};
},

//################
//### publishNote
//################
publishNote: function(author, title, content) {
console.log(`Enter publishNote, author: ${author}, title: ${title}, content: ${content}`);

	if(!smartRemotes._isConnected) return console.log(`Abort: smartRemotes not connected`);

	note = JSON.stringify({
		'author': author,
		'title': title,
		'host': location.hostname,
		'time': new Date().getTime(),
		'content': content
	});

	console.log(`Note: ${note}`);
	
	smartRemotes._socket.send(note);
}};

//################
//### Main
//################
console.log(`**** smartRemotes Object Loaded ****`);
