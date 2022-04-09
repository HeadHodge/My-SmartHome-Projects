////////////////////////////////////////////
//            orderService.js
////////////////////////////////////////////
console.log(`***Load orderService Methods...`);

//global.services.bootService.loadModule('services/', 'systemService.js').then(result => global.services.systemService = result);

///////////////////////////////////////////////////////////////////
activateLocalMinion = async function(receivedOrder) {
console.log(`activateLocalMinion: ${receivedOrder.ORDER.minionName}`);

//create transferredOrder
	var transferredOrder = {
		SUBJECT: 'ORDERUPDATE',
		
		TICKET  : receivedOrder.TICKET,
		
		UPDATE : {
			timestamp: `${Date.now()}`,
			progress : 'TRANSFERRED',
			note     : 'order was transferred to provider for processing',
		},	
	};

	console.log(`***Send orderTransferred update`);	
	await global.updateContract(transferredOrder);
	
//Load minion
	var minion = await global.loadModule(`minions/${receivedOrder.ORDER.minionName}/`, `minion.js`);
	console.log(`minion: `, minion);

//invokeMinion
	var orderResult = await minion(receivedOrder);
	
	orderResult.SUBJECT = 'COMPLETED',
	orderResult.TICKET  = receivedOrder.TICKET,
	orderResult.UPDATE  = {
		timestamp: `${Date.now()}`,
		progress : 'CLOSED',
		note     : 'Order was filled and transferred to client.',
	},	
	
	console.log(`orderResult: `, orderResult);
	await global.updateContract(orderResult);

};

///////////////////////////////////////////////////////////////////
var createTicket = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`createTicket: `, workOrder);
var ticket = {};

var clientConnection = global.services.bootService.event.requestContext.connectionId;
	console.log(`***clientConnection: `, clientConnection);

var client = await services.systemService.loadObject(`connections/connectedList/${clientConnection}`);
	console.log(`***Client: `, client);

var provider = await services.systemService.loadObject(`minions/${workOrder.OPTIONS.required.minion}/provider`);
	console.log(`***Provider: `, provider);

	ticket.reference = `${Date.now()}:${client.name}:${provider.name}`

	await global.services.bootService.postNotice(ticket);




return ticket;

//var key = `123456789createdOn + ':' + orderContract.TICKET.client + ':' + orderContract.TICKET.provider; 
//get contract client and provider
  var contract = {};
  
  var client = await global.loadObject(`connections/connectedList/${global.memo.requestContext.connectionId}`);
	console.log(`***Client: `, client.name);
	
	var provider = await global.loadObject(`minions/${workOrder.ORDER.minionName}/provider`);
	console.log(`***Provider: `, provider.name);
	
  contract.CLIENT = client;
	
  contract.PROVIDER = provider;
  
	contract.TICKET = {
			client     : client.name,
			provider   : provider.name,
			clientRef  : workOrder.ORDER.clientRef,
			contractRef: `${Date.now()}:${client.name}:${provider.name}`,
			createdOn  : `${Date.now()}`,
	};

  contract.ORDER = workOrder.ORDER;
  
  contract.UPDATES = [{
			timestamp: `${Date.now()}`,
			progress : 'RECEIVED',
			note     : 'Order was received and ticket opened for processing',
  }];
  
  contract.BILLING = {};
  
//createContract
  var params = {
      Bucket: 'minionlogic',
      Key   : 'contracts/openedList/' + contract.TICKET.contractRef,
      Body  : JSON.stringify(contract),
      ContentType: 'text/plain',
  };
        
	console.log(`createContract, params: `, params);
  await S3.putObject(params).promise();
  
//sendUpdate
  var receivedOrder = {
		  SUBJECT: 'ORDERUPDATE',
      TICKET: contract.TICKET,
      UPDATE: contract.UPDATES[0],
  };
  
	console.log(`***Send receivedOrder update`);	
	await global.sendMemo('client', receivedOrder);
  
  return contract;
};

/*
ORDER
-TASK
-OPTIONS
-CONTENT
-TICKET
-UPDATE
-CONTRACT
*/

///////////////////////////////////////////////
//            activateMinion 
///////////////////////////////////////////////
var activateMinion = async function(workOrder) {

console.log(`***activateMinion: `, workOrder);

//await global.services.bootService.postNotice({status: 'received workOrder'}, global.services.bootService.event.requestContext.connectionId);

//var contractServices = await loadModule('system/', 'contractServices.js');
	
	var ticket = await createTicket(workOrder);
	//console.log(`event: `, global.services.bootService);
    
	return;

	
//call activateMinion method
	console.log(`activate method: `, contract.PROVIDER.activateWith, contract);
	receivedOrder.TICKET = contract.TICKET 
	await eval(contract.PROVIDER.activateWith + '(receivedOrder)');
	//await activateLocalMinion(receivedOrder);
};
	
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name          : 'orderService',
	activateMinion: activateMinion,
};