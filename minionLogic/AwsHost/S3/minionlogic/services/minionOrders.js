////////////////////////////////////////////
//            orders.js
////////////////////////////////////////////
console.log(`***Load orders service...`);

///////////////////////////////////////////////////////////////////
var closeOrder = async function(filledOrder) {
console.log(`closeOrder, progress: ${filledOrder.REPORT.progress}`);

//get order
	global.activeOrder = filledOrder.TICKET.orderReference;
	var order = await services.systemService.loadObject(`orders/active/${filledOrder.TICKET.orderReference}.json`);

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
	
	filledOrder.BILL = order.BILL;
	
//deliver order	to client
	await global.services.bootService.postNotice(filledOrder, clientConnection);
	
//delete order	
	await services.systemService.deleteObject(`orders/active/${order.TICKET.orderReference}.json`);

//save order
	await services.systemService.saveObject(`orders/closed/${order.TICKET.orderReference}.json`, order);
	await services.systemService.saveObject(`members/${order.CONTRACT.clientName}/orders/${order.CONTRACT.billingMode}/${filledOrder.TICKET.orderReference}/`);
	await services.systemService.saveObject(`members/${order.CONTRACT.providerName}/orders/${order.CONTRACT.billingMode}/${filledOrder.TICKET.orderReference}/`);

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
var fillOrder = async function(openedOrder, providerConnection) {
///////////////////////////////////////////////////////////////////
console.log(`postProxyNotice: `, openedOrder);

//load minion
	try {
		var minion = await global.services.bootService.loadModule(`minions/${openedOrder.TICKET.minionName}/`, 'minion.js');
	} catch {
		await global.services.bootService.postNotice(openedOrder, providerConnection);

    //confirm order
        var confirmation = {
            SUBJECT: 'TASK-UPDATE',
        
            TICKET: openedOrder.TICKET,
        
            REPORT: {           
                progress  : 'FORWARDED',
                console   : 'Order opened and forwarded to minion provider to fill.',
            },
        };
   
		return await global.services.bootService.postNotice(confirmation);
	}

	var product = await minion(openedOrder);
	var filledOrder = {};
	
	filledOrder.SUBJECT = openedOrder.SUBJECT;
	filledOrder.TICKET = openedOrder.TICKET;
	filledOrder.PRODUCT = product;
	filledOrder.REPORT = {
		progress: "FILLED",
			note: "Minion Order Complete. Thank You for using minionLogic!",
	};

//complete order
	await closeOrder(filledOrder);	
};
  
///////////////////////////////////////////////////////////////////
var createOrder = async function(ticket) {
///////////////////////////////////////////////////////////////////
console.log(`openOrder: `, ticket);
var order = {};

//BUILD ORDER

//get member names	
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	var connectionInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	var clientInfo = await services.systemService.loadObject(`members/${connectionInfo.member}/${connectionInfo.member}.json`);
	var clientName = clientInfo.member;
	
	var minionName = ticket.OPTIONS.minionName;
	var minionInfo = await services.systemService.loadObject(`minions/${ticket.TASK.Details.Sku}/minion.json`);
	var providerInfo = await services.systemService.loadObject(`members/${minionInfo.member}/${minionInfo.member}.json`);
	var providerName = providerInfo.member;
	var providerConnection = providerInfo.connection;

//check balance
	if(minionInfo.mode == 'billable' && parseFloat(clientInfo.timeBalance) <= 0) {
		throw new Error('*** ABORT openOrder, Insufficient timeBalance to open order. Increase your timeBalance and try again.');
	}

//add taskName
	order.TASK = ticket.TASK;
	
//create ticket
	order.TICKET = {
		minionName: minionName,
		clientCode: ticket.TASK.From.acctCode,
		orderCode :`${Date.now()}`,
		session   : connectionInfo.session,
	};

//create contract	
	order.CONTRACT = {
		orderReference: order.TICKET.orderCode,
		clientName    : clientName,
		providerName  : providerName,
		minionName    : minionName,
		minionLocation: minionInfo.location,
		billingMode   : minionInfo.mode,
		startStamp    : `${Date.now()}`,
	};

//save order
	await services.systemService.saveObject(`orders/active/${order.TICKET.orderReference}.json`, order);
	 global.services.activeOrder = order.TICKET.orderReference;
	
//send ticket to provider
	//ticket.TICKET = order.TICKET;
	//await global.services.bootService.postNotice(ticket, providerConnection);
	await fillOrder(ticket, providerConnection);
};	
		
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name       : 'orderServices',
	createOrder: createOrder,
	closeOrder : closeOrder,
};