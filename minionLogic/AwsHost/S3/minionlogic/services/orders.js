////////////////////////////////////////////
//            orders.js
////////////////////////////////////////////
console.log(`***Load orders service...`);

///////////////////////////////////////////////////////////////////
var completeOrder = async function(orderUpdate) {
console.log(`completeOrder, progress: ${orderUpdate.REPORT.progress}`);
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
		'stopT(millisecs)'       : stopTime,
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

// Get ticket info
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	console.log(`***clientConnection: `, clientConnection);
	
	var connectInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	console.log(`***connectInfo: `, connectInfo);
	
	var clientInfo = await services.systemService.loadObject(`members/${connectInfo.member}/${connectInfo.member}.json`);
	console.log(`***clientInfo: `, clientInfo);
	
	var minionInfo = await services.systemService.loadObject(`minions/${workOrder.OPTIONS.minionName}/minion.json`);
	console.log(`***minionInfo: `, minionInfo);
	
	var providerInfo = await services.systemService.loadObject(`members/${minionInfo.member}/${minionInfo.member}.json`);
	console.log(`***providerInfo: `, providerInfo);
//add workOrder
	order.TASK = workOrder.TASK;
	order.OPTIONS = workOrder.OPTIONS;
	
//create ticket
	order.TICKET = {
		taskName	      : workOrder.TASK.name,
		minionName	      : workOrder.OPTIONS.minionName,
		clientReference   : workOrder.TASK.reference,
		orderReference    : `${Date.now()}:${connectInfo.member}:${minionInfo.member}`,
	};

//create contract
	order.CONTRACT = {
		clientName        : connectInfo.member,
		clientConnection  : clientConnection,
		providerName      : minionInfo.member,
		providerConnection: providerInfo.connection,
		minionLocation    : minionInfo.location,
		timeBalance	  	  : clientInfo.timeBalance,
		billingMode	      : minionInfo.mode,
	};
	
//create updates
	order.REPORTS = [];
			
	console.log(`***Order Created: `, order);
	return order;
};

//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name         : 'orderServices',
	createOrder  : createOrder,
	completeOrder: completeOrder,
};