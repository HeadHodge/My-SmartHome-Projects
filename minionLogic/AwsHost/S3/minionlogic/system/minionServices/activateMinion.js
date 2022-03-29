////////////////////////////////////////////
//          activateMinion
////////////////////////////////////////////
console.log(`***Load activateMinion...`);

///////////////////////////////////////////////////////////////////
activateLocalMinion = async function(receivedOrder) {
console.log(`activateLocalMinion: ${receivedOrder.ORDER.minionName}`);

try {

//create orderTransferred
	
	var orderTransferred = {
		TICKET: {
			client     : 'client',
			provider   : 'provider',
			clientRef  : 'job210.step27',
			contractRef: '12345678',
			createdOn  : '1243657790',
		},
		
		UPDATE: {
				timestamp: '1234566790',
				progress : 'TRANSFERED',
				note     : 'order was transferred to provider for processing',
			},	
	};

	console.log(`***Send orderTransferred update`);	
	await global.sendMemo(orderTransferred);
	
} catch(err) {

// Send orderFailed
	orderUpdate = {
		TICKET: {
			client   : 'HeadHodge',
			action	 : 'activateMinion',
			dueDate  : 'immediate',
			reportTo : 'job210.step27',
			signature: 'minionServices',
		},
			
		UPDATE: {
			progress : 'FAILED',
			note     : `***UNKNOWN ERROR: ${err.message}`,
			trackWith: 'HeadHodge',
		},
			
		TIMESTAMP: {
			time       : '12346578890',
			transaction: '12346578890:HeadHodge&microLogic',
		},
	};
	
	console.log(`***Send orderFailed update`);	
	await global.sendMemo(orderUpdate);
}

	console.log(`***All Done`);	

//var minion = await global.loadRoutine(`minions/${receivedOrder.MINION.name}/`, `minion.js`);

	//var returnOrder = minion(receivedOrder);
	//console.log(`returnOrder: `, returnOrder);


};

///////////////////////////////////////////////////////////////////
/////////////////////// MAIN //////////////////////////////////////
///////////////////////////////////////////////////////////////////
module.exports = async function(connection, receivedOrder) {

try {
console.log(`***Start activateMinion, receivedOrder: `, receivedOrder);


//temp form	
	var tmpOrder = JSON.stringify({
		ORDER: {
			action	  : 'activateMinion',
			minionName: 'minionLogic.helloWorld.example',
			minionTask: 'fillOrder',
			startOn   : 'now',
			clientRef : 'job210.step27',
		},
		
		UPDATE: {
			timestamp: '1234566790',
			progress : 'RECEIVED',
			note     : 'order was sucessfully received for processing',
		},
		
		TICKET: {
			client     : 'client',
			provider   : 'provider',
			clientRef  : 'job210.step27',
			contractRef: '12345678',
			createdOn  : '1243657790',
		},		
	});

//get contract client and provider
	var client = await global.loadObject(`connections/connectionList/${connection}`);
	console.log(`***Client: `, client.name);
	
	var provider = await global.loadObject(`minions/${receivedOrder.ORDER.minionName}/provider`);
	console.log(`***Provider: `, provider.name);

//create contract
	
	var orderContract = {
		TICKET: {
			client     : 'client',
			provider   : 'provider',
			clientRef  : 'job210.step27',
			contractRef: '12345678',
			createdOn  : '1243657790',
		},
		
		ORDER: {
			action	  : 'activateMinion',
			minionName: 'minionLogic.helloWorld.example',
			minionTask: 'fillOrder',
			startOn   : 'now',
			clientRef : 'job210.step27',
		},
		
		UPDATES: [
			{
				timestamp: '1234566790',
				progress : 'RECEIVED',
				note     : 'order was sucessfully received for processing',
			},	
		],
		
		BILLING: [
		
		],	
	};

// Save contract
	var contractKey = orderContract.TICKET.createdOn + ':' + orderContract.TICKET.client + ':' + orderContract.TICKET.provider; 
	await global.saveContract(contractKey, orderContract);

//create orderUpdate
	
	var orderUpdate = {
		TICKET: {
			client     : 'client',
			provider   : 'provider',
			clientRef  : 'job210.step27',
			contractRef: '12345678',
			createdOn  : '1243657790',
		},
		
		UPDATE: {
				timestamp: '1234566790',
				progress : 'RECEIVED',
				note     : 'order was sucessfully received for processing',
			},	
	};

	console.log(`***Send orderReceived update`);	
	await global.sendMemo(orderUpdate);

//call activateMinion method

	receivedOrder.TICKET = orderContract.TICKET 
	console.log(`activate method: `, provider.activateWith);
	await eval(provider.activateWith + '(receivedOrder)');
	
} catch(err) {
	console.log(`***ABORT: `, err);
	
}};
