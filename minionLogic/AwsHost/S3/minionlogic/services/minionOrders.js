////////////////////////////////////////////
//            orders.js
////////////////////////////////////////////
console.log(`***Load orders service...`);

///////////////////////////////////////////////////////////////////
var closeOrder = async function(ticket) {
console.log(`closeOrder, progress: ${ticket.REPORT.progress}`);

//get order
	global.activeOrder = ticket.TASK.Options.orderTag;
	var order = await services.systemService.loadObject(`orders/active/${ticket.TASK.Options.orderTag}.json`);

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
	
	ticket.BILL = order.BILL;
	
//deliver order	to client
	await global.services.bootService.postNotice(ticket, clientConnection);
	
//delete order	
	await services.systemService.deleteObject(`orders/active/${ticket.TASK.Options.orderTag}.json`);

//save order
	await services.systemService.saveObject(`orders/closed/${ticket.TASK.Options.orderTag}.json`, order);
	await services.systemService.saveObject(`members/${order.CONTRACT.clientName}/orders/${order.CONTRACT.billingMode}/${ticket.TASK.Options.orderTag}/`);
	await services.systemService.saveObject(`members/${order.CONTRACT.providerName}/orders/${order.CONTRACT.billingMode}/${ticket.TASK.Options.orderTag}/`);

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
var fillOrder = async function(ticket, providerConnection) {
///////////////////////////////////////////////////////////////////
console.log(`fillOrder: `, ticket.TASK);

//load minion
	try {
		var minion = await global.services.bootService.loadModule(`minions/${ticket.DETAILS.sku}/`, 'minion.js');
        console.log(`fillOrder locally`);
	} catch {
        console.log(`fillOrder from provider`);
        await global.services.bootService.postNotice(ticket, providerConnection);

        //confirm order
        var confirmation = {
            SUBJECT: 'TASK-UPDATE',
        
            REPORT: {           
                progress  : 'FORWARDED',
                console   : 'Order opened and forwarded to minion provider to fill.',
            },
        };
   
		return await global.services.bootService.postNotice(confirmation);
	}
    
//startMinion
    console.log(`fillOrder: `, ticket);
	var product = await minion(ticket);
	var filledOrder = {};
	
	filledOrder.TASK = ticket.TASK;
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
console.log(`createOrder: `, ticket.TASK);
var order = {};

    ticket.TASK.Options.orderTag =`${Date.now()}`;
    
//get member names	
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	var connectionInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	var clientInfo = await services.systemService.loadObject(`members/${connectionInfo.member}/${connectionInfo.member}.json`);
	var clientName = clientInfo.member;
	
	var minionSku = ticket.DETAILS.sku;
	var minionInfo = await services.systemService.loadObject(`minions/${ticket.DETAILS.sku}/minion.json`);
	var providerInfo = await services.systemService.loadObject(`members/${minionInfo.member}/${minionInfo.member}.json`);
	var providerName = providerInfo.member;
	var providerConnection = providerInfo.connection;

//check balance
	if(minionInfo.mode == 'billable' && parseFloat(clientInfo.timeBalance) <= 0) {
		throw new Error('*** ABORT openOrder, Insufficient timeBalance to open order. Increase your timeBalance and try again.');
	}

//add TASK
	//order.TASK = ticket.TASK;
	
//create contract	
	order.CONTRACT = {
		clientTag   : ticket.TASK.Options.clientTag,       
		orderTag    : ticket.TASK.Options.orderTag,
		clientName  : clientName,
		providerName: providerName,
		minionSku   : minionSku,
		billingMode : minionInfo.mode,
		startStamp  : `${Date.now()}`,
	};

//save order
	await services.systemService.saveObject(`orders/active/${order.CONTRACT.orderTag}.json`, order);
    global.services.activeOrder = order.CONTRACT.orderAcctCode;
	
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