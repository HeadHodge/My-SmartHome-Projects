console.log(`***Load minionLogic.helloWorld.example/minion.js`);

///////////////////////////////////////////////////////////////////
/////////////////////// MAIN //////////////////////////////////////
///////////////////////////////////////////////////////////////////
module.exports = async function(receivedOrder) {
	console.log(`***Start minionLogic.helloWorld.example/minion.js, receivedOrder: ${receivedOrder}`);
   
	var orderResult = {
		
        RESULT: {
			VIEW   : null,
			DATA   : null,
			CONSOLE: null,
		    SCRIPT : `alert('Hello Fellow Minion Lovers!');`,
	    },
	};
   
   console.log('orderResult', orderResult);
   return orderResult;
};