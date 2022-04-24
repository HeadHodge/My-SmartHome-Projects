////////////////////////////////////////////
//            orders.js
////////////////////////////////////////////
console.log(`***Load orders service...`);

///////////////////////////////////////////////////////////////////
var completeOrder = async function(orderUpdate) {
console.log(`completeOrder, progress: ${orderUpdate.CONSOLE.progress}`);

//get order
	global.activeOrder = orderUpdate.TICKET.orderReference;
	var order = await services.systemService.loadObject(`orders/active/${orderUpdate.TICKET.orderReference}.json`);

//get client connection
	var clientInfo = await services.systemService.loadObject(`members/${order.CONTRACT.clientName}/${order.CONTRACT.clientName}.json`);
	var clientConnection = clientInfo.connection;
	var clientName = clientInfo.member;
	
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
	await services.systemService.saveObject(`members/${order.CONTRACT.clientName}/orders/${order.CONTRACT.billingMode}/${orderUpdate.TICKET.orderReference}/`);
	await services.systemService.saveObject(`members/${order.CONTRACT.providerName}/orders/${order.CONTRACT.billingMode}/${orderUpdate.TICKET.orderReference}/`);


//debit billed orders
billList = await services.systemService.listObjects(`members/${clientName}/orders/billable/`);

console.log(`***BILL LIST*** `, billList);

var totalDebit = 0, order = {};

	for (var bill in billList.CommonPrefixes) {
		var orderPrefix = billList.CommonPrefixes[bill].Prefix
		var orderRef = billList.CommonPrefixes[bill].Prefix.split('/')[4];

		order = await services.systemService.loadObject(`orders/closed/${orderRef}.json`);
		await services.systemService.saveObject(`members/${clientName}/orders/debited/${orderRef}/`);
		await services.systemService.deleteObject(`members/${clientName}/orders/billable/${orderRef}/`);

		totalDebit += order.BILL.minutesBilled;
	};
	
	clientInfo.timeBalance -= totalDebit;
	console.log(`timeBalance`, clientInfo.timeBalance);
	await services.systemService.saveObject(`members/${clientName}/${clientName}.json`, clientInfo);
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
	global.activeOrder = order.TICKET.orderReference;
};	
		
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name         : 'orderServices',
	createOrder  : createOrder,
	completeOrder: completeOrder,
};