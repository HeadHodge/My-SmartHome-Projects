console.log(`***Loading 'example.minionLogic.helloWorld.bjs/minion.js'`);

module.exports = function(receivedOrder) {
	console.log(`***Start 'example.minionLogic.helloWorld.bjs/minion.js', receivedOrder: ${receivedOrder}`);
   
	var returnOrder = {
		TICKET: {
			action     : 'completeOrder',
			submittedby: 'minionLogic',
			deliverTo  : receivedOrder.TICKET.deliverTo,
		},
		
	    STATUS: {
	        state  : 'FILLED FAILED ANSWERED',
	        notes  : 'Minion: "example.minionLogic.helloWorld.bjs", Task: "displayMessage", completed successfully',
	    },
 
        RESULT: {
		    SCRIPT: `alert('Hello Fellow Minion Lovers!');`,
	    },
		
		SETTLEMENT: {
			billTo   : 'HeadHodge',
			royaltyTo: 'minionMakers',
			startTime: '123456',
			stopTime : '123478',
			unitTotal: '22',
			unitRate : '.02',
			units    : 'milliseconds',
			ratePlan : '30 Day Trial',
		},
	};
   
   console.log('returnOrder', returnOrder);
   return returnOrder;
};