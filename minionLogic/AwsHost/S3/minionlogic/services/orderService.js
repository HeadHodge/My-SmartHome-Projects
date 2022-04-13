////////////////////////////////////////////
//            orderService.js
////////////////////////////////////////////
console.log(`***Load orderService Methods...`);

var order = {};

///////////////////////////////////////////////////////////////////
var deliverProduct = async function(orderUpdate) {
console.log(`deliverProduct, progress: ${orderUpdate.UPDATE.progress}`);
var timeStamp = `${Date.now()}`

//get order
	var order = await services.systemService.loadObject(`orders/active/${orderUpdate.TICKET.orderReference}.json`);
	var clientName = order.CONTRACT.clientName;
	var providerName = order.CONTRACT.providerName;
	var clientInfo = await services.systemService.loadObject(`clients/${clientName}/${clientName}.json`);
	var clientConnection = clientInfo.connection;

//calculate bill
	var currentBalance = parseFloat(clientInfo.runtimeBalance);
	var start = parseFloat(order.CONTRACT.startTime);
	var stop = parseFloat(timeStamp);
	var runtimeTotal = (stop - start); //minutes
	var usageFee = 0; //minutes
	var billingMode = order.CONTRACT.billingMode;
	if(billingMode == 'billable') usageFee = runtimeTotal/60000;
	var newBalance = currentBalance - usageFee;

// create bill
	var bill = {
		'billType'               : 'minion usage fee',
		'billingMode'            : billingMode,
		'currentBalance(minutes)': currentBalance,
		'startStamp(millisecs)'  : start,
		'stopStamp(millisecs)'   : stop,
		'runtimeTotal(millisecs)': runtimeTotal,
		'usageFee(minutes)'      : usageFee,
		'newBalance(minutes)'    : newBalance,
	};
		
	orderUpdate.BILL = bill;
	await global.services.bootService.postNotice(orderUpdate, clientConnection);
	
//update order
	order.CONTRACT.stopTime = timeStamp;
	order.BILL = bill;
	await services.systemService.saveObject(`orders/complete/${order.TICKET.orderReference}.json`, order);
	await services.systemService.deleteObject(`orders/active/${order.TICKET.orderReference}.json`, order);

//save receipts
	var clientReceipt = {
		TICKET: orderUpdate.TICKET,
		BILL: bill,
	}
	
	var chargeType;
	
	if(usageFee == 0)
		chargeType = 'free';
	else
		chargeType = 'fees';
	
	await services.systemService.saveObject(`clients/${clientName}/bills/${chargeType}/${orderUpdate.TICKET.orderReference}.json`, clientReceipt);
	await services.systemService.saveObject(`clients/${providerName}/receipts/${chargeType}/${orderUpdate.TICKET.orderReference}.json`, clientReceipt);

//save bill copy to provider

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
	
	var clientInfo = await services.systemService.loadObject(`clients/${connectInfo.client}/${connectInfo.client}.json`);
	console.log(`***clientInfo: `, clientInfo);
	
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
		minionLocation    : minionInfo.location,
		runtimeBalance	  : clientInfo.runtimeBalance,
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
	
//check balance
	console.log(`billingMode: ${order.CONTRACT.billingMode}, runTimeBalance: ${order.CONTRACT.runtimeBalance}`);
	
	if(order.CONTRACT.billingMode == 'billable' && parseFloat(order.CONTRACT.runtimeBalance) >= 0) {
		throw new Error('*** ABORT ORDER, Client has insufficient runtimeBalance. Increase runtimeBalance and try again.');
	}

//save order
	await services.systemService.saveObject(`orders/active/${order.TICKET.orderReference}.json`, order);

//update client
	var update = {
		SUBJECT: 'ORDER-UPDATE',
		TICKET: order.TICKET,
		UPDATE: order.UPDATES[0],
	};
	
	await global.services.bootService.postNotice(update);

//load minion
	workOrder.TICKET = order.TICKET;
	var minionService = await global.services.bootService.loadModule(`minions/${order.TICKET.minionName}/`, 'minion.js');

//update client
	var update = {
		SUBJECT: 'ORDER-UPDATE',
		TICKET: workOrder.TICKET,
		OUTPUT: await minionService(workOrder),
		UPDATE: {
			progress:"FULFILLED",
			note: "workOrder fulfilled by minionLogic. Thank You for using our minions.",
		}
	};

	await deliverProduct(update);
};
	
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name       : 'orderService',
	orderMinion: orderMinion,
};