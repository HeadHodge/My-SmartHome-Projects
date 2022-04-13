console.log(`***Load minionLogic.helloWorld.example/minion.js`);

///////////////////////////////////////////////////////////////////
/////////////////////// MAIN //////////////////////////////////////
///////////////////////////////////////////////////////////////////
module.exports = async function(workOrder) {
	console.log(`***Start minionLogic.helloWorld.example/minion.js, workOrder: ${workOrder}`);
   
	var minionOutput = {
			RESULT : 'SUCCESS',
 			VIEW   : null,
			DATA   : null,
			CONSOLE: 'minion completed task succesfully',
		    SCRIPT : `alert('Hello Fellow Minion Lovers!');`,
	};
   
   console.log('minionOutput', minionOutput);
   return minionOutput;
};