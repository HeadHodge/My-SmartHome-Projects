////////////////////////////////////////////
//            GLOBAL VARIABLES
////////////////////////////////////////////
const debug = require('../requires/debugLog.js').debug;
const mqtt = require('/root/node_modules/mqtt')
const brokerUrl = 'mqtt://192.168.0.160:1883'; //test broker: 'mqtt://test.mosquitto.org'
const brokerOptions = {
	username:"admin", 
	password:"pepper"
};

var broker = null, onInput = null;

//##########################################
const connectBroker = function() {
//##########################################
console.log(`Enter connectBroker`);

	if(broker) broker.end(); //Close existing broker
	broker = mqtt.connect(brokerUrl, brokerOptions);

	broker.on('connect', function() {
		console.log(`Enter MQTT broker connected`);
		broker.subscribe('remoteInput', 0);
	});
 
	broker.on('message', function(topic, message) {
		console.log(` \n \n========================================================================================`);
		console.log(`Enter MQTT broker message received, topic: ${topic}, message: ${message}`);
		global.onInput(JSON.parse(message));
		//onInput(message);
		//onInput(JSON.parse(message));
	});

	broker.on("error", function(error) {
		console.log(`Enter broker failed: ${error.toString()}`);
		if(broker) broker.end();
		broker = null;
	});	

	broker.on("close", function() {
		console.log(`Enter broker closed`);
		broker = null;
	});	
};

//##########################################
const sendTask = function(task) {
//##########################################
console.log(`Enter sendTask`);

	if(!broker) connectBroker(task);
	broker.publish('controlInput', task);
};

//##########################################
const captureInput = function(callBack=null) {
//##########################################
console.log(`Enter captureInput`);

	onInput = callBack;
	if(!broker) connectBroker();
	broker.subscribe('remoteInput', 0);
};

////////////////////////////////////////////
//                MAIN
//Open connection with Hub and send request
////////////////////////////////////////////
console.log('Loaded mqttClient.js');

	exports.captureInput = captureInput;
	exports.sendTask = sendTask;
	connectBroker();
