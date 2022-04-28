console.log(`***Load minionLogic.helloWorld.example/minion.js`);

/////////////////////// MAIN //////////////////////////////////////
var query = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example/minion.js, workOrder: ${workOrder}`);

	var product = {
			Result : 'ANSWER',
			
			Console: 'minion query completed succesfully',
			
 			View   : ``,
					  
			Data   : ``,
			
		    Script : `
				var event = new CustomEvent("name-of-event", { "detail": "Example of an event" });
				document.dispatchEvent(event);
				alert('**HELLO WORLD**');
			`,
					  
 			Style  : `div {
						display : none;
					 }`,				 
	};
   
   return product;
};
 
/////////////////////// MAIN //////////////////////////////////////
var fill = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example/minion.js, workOrder: ${workOrder}`);
   
	var product = {
			Result : 'FILLED',
			Console: 'minion completed task succesfully',
 			View   : `<div>Minion's Shadow DOM View Area</div>`,
			Data   : ``,
		    Script : `alert('${workOrder.OPTIONS.required.message}');`,
 			Style  : `div {
						background-color: lightgrey;
						text-align: center;
						height: 250px;
						margin-top: 100px;
						padding-top: 100px;
					 }`,				 
	};
   
   return product;
};
 
/////////////////////// MAIN //////////////////////////////////////
module.exports = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example/minion.js, workOrder: ${workOrder}`);
var product;

	if(workOrder.OPTIONS.minionCommand == 'query') {
		product = fill(workOrder);
	} else {
		product = fill(workOrder);
	}
   
   console.log('product', product);
   return product;
};
