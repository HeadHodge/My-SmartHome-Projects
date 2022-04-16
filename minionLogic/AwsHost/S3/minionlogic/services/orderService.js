////////////////////////////////////////////
//            orderService.js
////////////////////////////////////////////
console.log(`***Load orderService Methods...`);

var order = {};

///////////////////////////////////////////////////////////////////
var wrapProduct = async function(product, namespace) {
console.log(`wrapProduct: `, namespace);

var template = await services.systemService.loadFile(`services/productWrapper.txt`);
template = template.replace(/ML.namespace/g, namespace);
template = template.replace(/ML.script/g, product.SCRIPT);
template = template.replace(/ML.view/g, product.VIEW);
template = template.replace(/ML.data/g, product.DATA);
template = template.replace(/ML.console/g, product.CONSOLE);

	await global.services.bootService.postNotice(template);
	return template;


/*
window.onload = function() {
    var s = document.createElement('script');
    s.type = 'text/javascript';
    var code = 'alert("hello world!");';
    try {
      s.appendChild(document.createTextNode(code));
      document.body.appendChild(s);
    } catch (e) {
      s.text = code;
      document.body.appendChild(s);
    }
  }
  
//wrap namespace around product
window.onload = function() {
    var s = document.createElement('script');
    s.type = 'text/javascript';
    var code = 'alert("hello world!");';
    try {
      s.appendChild(document.createTextNode(code));
      document.body.appendChild(s);
    } catch (e) {
      s.text = code;
      document.body.appendChild(s);
    }
  }
var myApp = {};
(function(context) { 
    var id = 0;
 
    context.next = function() {
        return id++;    
    };
 
    context.reset = function() {
        id = 0;     
    }
})(myApp);  
*/
  
};

///////////////////////////////////////////////////////////////////
var completeOrder = async function(orderUpdate) {
console.log(`completeOrder, progress: ${orderUpdate.REPORT.progress}`);
//wrapProduct
	orderUpdate.PRODUCT = await wrapProduct(orderUpdate.OUTPUT, orderUpdate.TICKET.minionName);
	
//get order
	var order = await services.systemService.loadObject(`orders/active/${orderUpdate.TICKET.orderReference}.json`);
	order.CONTRACT.stopStamp = `${Date.now()}`;

//get billing info
	var clientName = order.CONTRACT.clientName;
	var providerName = order.CONTRACT.providerName;
	var clientInfo = await services.systemService.loadObject(`clients/${clientName}/${clientName}.json`);
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
	await services.systemService.saveObject(`clients/${clientName}/${clientName}.json`, clientInfo);

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
	
	await services.systemService.saveObject(`clients/${clientName}/bills/${chargeType}/${orderUpdate.TICKET.orderReference}/`);
	await services.systemService.saveObject(`clients/${providerName}/receipts/${chargeType}/${orderUpdate.TICKET.orderReference}/`);
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
	};
	
//create updates
	order.REPORTS = [];
			
	console.log(`***Order Created: `, order);
	return order;
};

///////////////////////////////////////////////////////////////////
var fillOrder = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`fillOrder: `, workOrder);

//load minion
	var minionService = await global.services.bootService.loadModule(`minions/${workOrder.TICKET.minionName}/`, 'minion.js');

//update client
	var orderUpdate = {
		SUBJECT: 'ORDER-UPDATE',
		TICKET: workOrder.TICKET,
		OUTPUT: await minionService(workOrder),
		REPORT: {
			progress:"FILLED",
			note: "Order filled by minionLogic. Thank You for using our minions!",
		}
	};

//complete order
	await completeOrder(orderUpdate);
};

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

//fill local order
	workOrder.TICKET = order.TICKET;
	workOrder.REPORT = order.REPORT;
	await fillOrder(workOrder);
};
	
//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name       : 'orderService',
	orderMinion: orderMinion,
};