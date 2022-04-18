console.log(`***Load minionLogic.helloWorld.example/minion.js`);

/////////////////////// MAIN //////////////////////////////////////
var query = async function(workOrder) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example/minion.js, workOrder: ${workOrder}`);

	var product = {
			Result : 'ANSWER',
			
			Console: 'minion query completed succesfully',
			
 			View   : `<div id='Topics'>\r\n
					     <div id='Description'>Query result: Description</div>\r\n
					     <div id='Usage'>Query result: Usage</div>\r\n
					     <div id='Example'>Query result: Example</div>\r\n
					     <div id='Availability'>Query result: Availability</div>\r\n
					     <div id='About'>Query result: About</div>\r\n
					     <div id='Support'>Query result: Support</div>\r\n
					  </div>\r\n`,
					  
			Data   : ``,
			
		    Script : `
				var event = new CustomEvent("name-of-event", { "detail": "Example of an event" });
				document.dispatchEvent(event);
				alert('?Query Minion?');
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
		product = query(workOrder);
	} else {
		product = fill(workOrder);
	}
   
   console.log('product', product);
   return product;
};
