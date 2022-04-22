////////////////////////////////////////////
//            clients.js
////////////////////////////////////////////
console.log(`***Load clients service...`);

///////////////////////////////////////////////
var receivedNotice = async function(notice) {
///////////////////////////////////////////////
console.log(`***receivedNotice: `, notice);
var workOrder = notice;

//create order
	var order = await global.services.orderService.createOrder(workOrder);
return;

//check balance
	console.log(`billingMode: ${order.CONTRACT.billingMode}, runBalance: ${order.CONTRACT.runBalance}`);
	
	if(order.CONTRACT.billingMode == 'billable' && parseFloat(order.CONTRACT.runBalance) >= 0) {
		throw new Error('*** ABORT ORDER, Client has insufficient runBalance. Increase runBalance and try again.');
	}

//create order report
	orderReport = {
		progress	   : 'OPEN',
		note  		   : 'workOrder received by minionLogic and waiting for provider fulfillment',
		timestamp      : `${Date.now()}`,
	};

//save order
	order.REPORTS.push(orderReport);
	order.CONTRACT.startStamp = `${Date.now()}`;
	await services.systemService.saveObject(`orders/active/${order.TICKET.orderReference}.json`, order);

//process local order
	workOrder.TICKET = order.TICKET;
	
//await fillOrder(workOrder);
	await global.services.bootService.postNotice(workOrder, order.CONTRACT.providerConnection);
};

//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name          : 'clientServices',
	receivedNotice: receivedNotice,
};