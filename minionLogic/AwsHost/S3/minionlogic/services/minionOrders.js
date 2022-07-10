////////////////////////////////////////////
//            orders.js
////////////////////////////////////////////
console.log(`***Load orders service...`);

///////////////////////////////////////////////////////////////////
var completeOrder = async function(ticket) {
console.log(`completeOrder, progress: `);

//get order
	global.activeOrder = ticket.sellersKey;
	var order = await services.systemService.loadObject(`orders/active/${ticket.sellersKey}.json`);

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
	await services.systemService.deleteObject(`orders/active/${ticket.sellersKey}.json`);

//save order
	await services.systemService.saveObject(`orders/completed/${ticket.sellersKey}.json`, order);
	await services.systemService.saveObject(`members/${order.CONTRACT.clientName}/orders/${order.CONTRACT.billingMode}/${ticket.sellersKey}/`);
	await services.systemService.saveObject(`members/${order.CONTRACT.providerName}/orders/${order.CONTRACT.billingMode}/${ticket.sellersKey}/`);

//debit billed orders
billList = await services.systemService.listObjects(`members/${clientName}/orders/billable/`);

console.log(`***BILL LIST*** `, billList);

var totalDebit = 0, order = {};

	for (var bill in billList.CommonPrefixes) {
		var orderPrefix = billList.CommonPrefixes[bill].Prefix
		var orderRef = billList.CommonPrefixes[bill].Prefix.split('/')[4];

		order = await services.systemService.loadObject(`orders/completed/${orderRef}.json`);
		await services.systemService.saveObject(`members/${clientName}/orders/debited/${orderRef}/`);
		await services.systemService.deleteObject(`members/${clientName}/orders/billable/${orderRef}/`);

		totalDebit += order.BILL.minutesBilled;
	};
	
	clientInfo.timeBalance -= totalDebit;
	console.log(`timeBalance`, clientInfo.timeBalance);
	await services.systemService.saveObject(`members/${clientName}/${clientName}.json`, clientInfo);
};

///////////////////////////////////////////////////////////////////
var buildOrder = async function(ticket, providerConnection) {
///////////////////////////////////////////////////////////////////
console.log(`makeOrder: `);

//load minion
	try {
		var minion = await global.services.bootService.loadModule(`minions/${ticket.MINION.Sku}/`, 'minion.js');
        console.log(`makeOrder locally`);
	} catch {
        console.log(`makeOrder from provider`);
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

    var productTicket = {
        deliverTo : 'client',
        subject   : 'minionCompleted',
        buyersKey : ticket.buyersKey,
        sellersKey: ticket.sellersKey,
        
        MINION: {
           Name   : ticket.MINION.Name,
           Sku    : ticket.MINION.Sku,          
           Product: product,
           Status : {
                progress: 'BUILT',
                note    : 'Minion built product succesfully.',
            },
        },
	};

//complete order
	await completeOrder(productTicket);	
};
  
///////////////////////////////////////////////////////////////////
var createOrder = async function(ticket) {
///////////////////////////////////////////////////////////////////
console.log(`createOrder: `, ticket);
    var order = {};
    
//get order information
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	var connectionInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	var clientInfo = await services.systemService.loadObject(`members/${connectionInfo.member}/${connectionInfo.member}.json`);
	var clientName = clientInfo.member;
	
	var minionSku = ticket.MINION.Sku;
	var minionInfo = await services.systemService.loadObject(`minions/${ticket.MINION.Sku}/minion.json`);
	var providerInfo = await services.systemService.loadObject(`members/${minionInfo.member}/${minionInfo.member}.json`);
	var providerName = providerInfo.member;
	var providerConnection = providerInfo.connection;

//check balance
	if(minionInfo.mode == 'billable' && parseFloat(clientInfo.timeBalance) <= 0) {
		throw new Error('*** ABORT openOrder, Insufficient timeBalance to open order. Increase your timeBalance and try again.');
	}
	
//create contract	
    ticket.sellersKey =`${Date.now()}`;

	order.CONTRACT = {
		buyersKey   : ticket.buyersKey,       
		sellersKey  : ticket.sellersKey,
		clientName  : clientName,
		providerName: providerName,
		minionSku   : minionSku,
		billingMode : minionInfo.mode,
		startStamp  : `${Date.now()}`,
	};

//save order
	await services.systemService.saveObject(`orders/active/${order.CONTRACT.sellersKey}.json`, order);
    global.services.activeOrder = order.CONTRACT.orderAcctCode;
	
//send ticket to provider
	await buildOrder(ticket, providerConnection);
};	
		
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name         : 'orderServices',
	createOrder  : createOrder,
	completeOrder: completeOrder,
};