console.log(`***Load minionLogic.helloWorld.example/minion.js`);

/////////////////////// MAIN //////////////////////////////////////
var query = async function(ticket) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example/minion.js`);

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
var createProduct = async function(ticket) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example.v1/minion.js, workOrder`);
   
    var product = {
            Result : 'FILLED',

            Console: 'minion completed task succesfully',

            View   : `
                    <div class='container'>
                        <div class='hello'>${workOrder.OPTIONS.helloMessage}</div>
                    </div>`,

            Data   : ``,

            Script : `alert('Incoming Minion Message');`,

            Style  : `
                    div.container {
                        color: ${workOrder.OPTIONS.textColor};
                        background-color: ${workOrder.OPTIONS.screenColor};
                        position: absolute;
                        left: 50%;
                        top: 50%;
                        width: ${workOrder.OPTIONS.bannerSize};
                        height: ${workOrder.OPTIONS.bannerSize};
                        margin-left: ${workOrder.OPTIONS.bannerLocation};
                        margin-top: ${workOrder.OPTIONS.bannerLocation};
                     }
                     
                     div.hello {
                        position: absolute;
                        text-align: center;
                        width: 100%;
                        top: 40%;                    
                     }
                     
                     :host {
                        width: 100%;
                        height: 100%;
                        background-color: red;
                     }`,                 
    };
   
   return product;
};
 
/////////////////////// MAIN //////////////////////////////////////
module.exports = async function(ticket) {
///////////////////////////////////////////////////////////////////
console.log(`***Start minionLogic.helloWorld.example/minion.js`);
var product;

    if(ticket.DETAILS.command == 'createProduct') {
        product = query(ticket);
    } else {
        product = createProduct(ticket);
    }
   
   console.log('product', product);
   return product;
};
