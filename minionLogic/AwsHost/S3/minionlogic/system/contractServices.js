////////////////////////////////////////////
//            contractServices.js
////////////////////////////////////////////
console.log(`***Load contractServices...`);
var gwo;

///////////////////////////////////////////////////////////////////
var createContract = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`createContract: `, workOrder, gwo);
return;
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

///////////////////////////////////////////////////////////////////
/////////////////////// MAIN //////////////////////////////////////
///////////////////////////////////////////////////////////////////

module.exports = async function(workOrder) {
console.log(`***Start contractServices, workOrder: `, workOrder);
	gwo = workOrder;
	
	return {
		createContract: createContract,
	};
};
