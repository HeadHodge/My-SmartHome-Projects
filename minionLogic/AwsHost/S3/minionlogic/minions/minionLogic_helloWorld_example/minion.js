console.log(`***Load minionLogic.helloWorld.example/minion.js`);

///////////////////////////////////////////////////////////////////
/////////////////////// MAIN //////////////////////////////////////
///////////////////////////////////////////////////////////////////
module.exports = async function(workOrder) {
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
   
   console.log('product', product);
   return product;
};