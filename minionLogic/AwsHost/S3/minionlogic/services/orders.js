////////////////////////////////////////////
//            orders.js
////////////////////////////////////////////
console.log(`***Load orders service...`);

///////////////////////////////////////////////////////////////////
var completeOrder = async function(orderUpdate) {
console.log(`completeOrder, progress: ${orderUpdate.REPORT.progress}`);

//get order
	var order = await services.systemService.loadObject(`orders/active/${orderUpdate.TICKET.orderReference}.json`);

//get client connection
	var clientInfo = await services.systemService.loadObject(`members/${order.CONTRACT.clientName}/${order.CONTRACT.clientName}.json`);
	var clientConnection = clientInfo.connection;
	
//create bill
	order.CONTRACT.stopStamp = `${Date.now()}`;
	
	var startTime = parseFloat(order.CONTRACT.startStamp);
	var stopTime = parseFloat(order.CONTRACT.stopStamp);
	var computeTime = stopTime - startTime;
	var minutesCharged = parseInt((computeTime/60000)*1000000)/1000000, minutesBilled = 0;
	
	if(order.CONTRACT.billingMode == 'billable') minutesBilled = minutesCharged;
	
	order.BILL = {
		minionName    : order.CONTRACT.minionName,
		orderReference: order.CONTRACT.orderReference,
		startTime     : startTime,
		stopTime      : stopTime,
		computeTime   : computeTime,
		billingMode	  : order.CONTRACT.billingMode,
		minutesCharged: minutesCharged,
		minutesBilled : minutesBilled,
	};
	
	orderUpdate.BILL = order.BILL;
	
//deliver order	to client
	await global.services.bootService.postNotice(orderUpdate, clientConnection);
	
//delete order	
	await services.systemService.deleteObject(`orders/active/${order.TICKET.orderReference}.json`);

//save order
	await services.systemService.saveObject(`orders/closed/${order.TICKET.orderReference}.json`, order);
	await services.systemService.saveObject(`members/${order.CONTRACT.clientName}/bills/${order.CONTRACT.billingMode}/${orderUpdate.TICKET.orderReference}/`);
	await services.systemService.saveObject(`members/${order.CONTRACT.providerName}/receipts/${order.CONTRACT.billingMode}/${orderUpdate.TICKET.orderReference}/`);

return;

//get order
	var order = await services.systemService.loadObject(`orders/active/${orderUpdate.TICKET.orderReference}.json`);
	order.CONTRACT.stopStamp = `${Date.now()}`;

//get billing info
	var clientName = order.CONTRACT.clientName;
	var providerName = order.CONTRACT.providerName;
	var clientInfo = await services.systemService.loadObject(`members/${clientName}/${clientName}.json`);
	var clientConnection = clientInfo.connection;

//calculate bill
	var currentBalance = parseFloat(clientInfo.runtimeBalance);
	var startTime = parseFloat(order.CONTRACT.startStamp);
	var stopTime = parseFloat(order.CONTRACT.stopStamp);
	var runtimeTotal = (stopTime - startTime); //minutes
	var usageFee = 0; //minutes
	var billingMode = order.CONTRACT.billingMode;
	if(billingMode == 'billable') usageFee = runtimeTotal/60000;
	var newBalance = currentBalance - usageFee;

// create bill
	var bill = {
		'billType'               : 'minion usage fee',
		'billingMode'            : billingMode,
		'currentBalance(minutes)': currentBalance,
		'startTime(millisecs)'   : startTime,
		'stopTime(millisecs)'       : stopTime,
		'runtimeTotal(millisecs)': runtimeTotal,
		'usageFee(minutes)'      : usageFee,
		'newBalance(minutes)'    : newBalance,
	};
		
	orderUpdate.BILL = bill;
	await global.services.bootService.postNotice(orderUpdate, clientConnection);

//update order
	order.REPORTS.push(orderUpdate.REPORT);
	order.BILL = bill;
	await services.systemService.renameObject(`orders/active/${order.TICKET.orderReference}.json`, `orders/complete/${order.TICKET.orderReference}.json`);

//update client's balance
	clientInfo.runtimeBalance = `${Math.round((newBalance - runtimeTotal/1000) * 10000)/10000}`;
	await services.systemService.saveObject(`members/${clientName}/${clientName}.json`, clientInfo);

//save receipts
	var clientReceipt = {
		TICKET: orderUpdate.TICKET,
		BILL: bill,
	}
	
	var chargeType;
	
	if(usageFee == 0)
		chargeType = 'noFee';
	else
		chargeType = 'usageFee';
	
	await services.systemService.saveObject(`members/${clientName}/bills/${chargeType}/${orderUpdate.TICKET.orderReference}/`);
	await services.systemService.saveObject(`members/${providerName}/receipts/${chargeType}/${orderUpdate.TICKET.orderReference}/`);
};

///////////////////////////////////////////////////////////////////
var localOrder = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`fillOrder: `, workOrder);

//load minion
	var minionService = await global.services.bootService.loadModule(`minions/${workOrder.TICKET.minionName}/`, 'minion.js');

//update client
	var orderUpdate = {
		SUBJECT: 'ORDER-UPDATE',
		TICKET : workOrder.TICKET,
		PRODUCT: await minionService(workOrder),
		REPORT : {
			progress:"FILLED",
			note    : "Order filled by minionLogic. Thank You for using our minions!",
		}
	};

//complete order
	await completeOrder(orderUpdate);
};

///////////////////////////////////////////////////////////////////
var createOrder = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`createOrder: `, workOrder);
var order = {};

//BUILD ORDER

//get member names	
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	var connectionInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	var clientInfo = await services.systemService.loadObject(`members/${connectionInfo.member}/${connectionInfo.member}.json`);
	var clientName = clientInfo.member;
	
	var minionName = workOrder.OPTIONS.minionName;
	var minionInfo = await services.systemService.loadObject(`minions/${workOrder.OPTIONS.minionName}/minion.json`);
	var providerInfo = await services.systemService.loadObject(`members/${minionInfo.member}/${minionInfo.member}.json`);
	var providerName = providerInfo.member;
	var providerConnection = providerInfo.connection;
	
//check balance
	if(minionInfo.mode == 'billable' && parseFloat(clientInfo.timeBalance) <= 0) {
		throw new Error('*** ABORT createOrder, Insufficient timeBalance to open order. Increase your timeBalance and try again.');
	}

//add taskName
	order.TASK = workOrder.TASK;
	
//create ticket
	order.TICKET = {
		minionName    : minionName,
		taskReference : workOrder.TASK.reference,
		orderReference: `${Date.now()}`,
	};

//create contract	
	order.CONTRACT = {
		orderReference: order.TICKET.orderReference,
		clientName    : clientName,
		providerName  : providerName,
		minionName    : minionName,
		minionLocation: minionInfo.location,
		billingMode   : minionInfo.mode,
		startStamp    : `${Date.now()}`,
	};
	
//send workOrder to provider
	workOrder.TICKET = order.TICKET;
	await global.services.bootService.postNotice(workOrder, providerConnection);

//save order
	await services.systemService.saveObject(`orders/active/${order.TICKET.orderReference}.json`, order);
};	
		
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name         : 'orderServices',
	createOrder  : createOrder,
	completeOrder: completeOrder,
};