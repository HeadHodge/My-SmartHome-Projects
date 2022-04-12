////////////////////////////////////////////
//            orderService.js
////////////////////////////////////////////
console.log(`***Load orderService Methods...`);

var order = {};

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
var shipOrder = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`shipOrder: `, workOrder);
var ticket = {};
};

///////////////////////////////////////////////////////////////////
var createOrder = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`createOrder: `, workOrder);

// Get ticket info
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	console.log(`***clientConnection: `, clientConnection);
	
	var connectInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	console.log(`***connectInfo: `, connectInfo);
	
	var minionInfo = await services.systemService.loadObject(`minions/${workOrder.OPTIONS.required.minion}/minion.json`);
	console.log(`***minionInfo: `, minionInfo);
	
	var providerInfo = await services.systemService.loadObject(`clients/${minionInfo.client}/${minionInfo.client}.json`);
	console.log(`***providerInfo: `, providerInfo);

//create order	
	order = workOrder;

//create ticket
	order.TICKET = {
		taskName	      : workOrder.TASK.name,
		minionName	      : workOrder.OPTIONS.required.minion,
		clientReference   : workOrder.TASK.reference,
		orderReference    : `${Date.now()}:${connectInfo.client}:${minionInfo.client}`,
	};

//create contract
	order.CONTRACT = {
		clientName        : connectInfo.client,
		clientConnection  : clientConnection,
		providerName      : minionInfo.client,
		providerConnection: providerInfo.connection,
		billingMode	      : minionInfo.mode,
		startTime		  : `${Date.now()}`,
	};
	
//create updates
	order.UPDATES = [{
		progress	   : 'OPENED',
		note  		   : 'workOrder received by minionLogic and shipping to provider for fulfillment',
		timestamp      : `${Date.now()}`,
	}];
			
	console.log(`***Order Created: `, order);
	return order;
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
//            orderMinion 
///////////////////////////////////////////////
var orderMinion = async function(workOrder) {
console.log(`***orderMinion: `, workOrder);

//create order
	var order = await createOrder(workOrder);
	await services.systemService.saveObject(`orders/active/${order.TICKET.orderReference}.json`, order);
	
//update client 
	await global.services.bootService.postNotice(order);
};
	
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name       : 'orderService',
	orderMinion: orderMinion,
};