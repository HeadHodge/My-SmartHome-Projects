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
console.log(`***createProduct`, ticket);
   
    var product = {
            View   : `
                    <div class='container'>
                        <div class='hello'>${ticket.MINION.Details.message}</div>
                    </div>`,

            Data   : ``,

            Script : `alert('Incoming Minion Message');`,

            Style  : `
                    div.container {
                        color: ${ticket.MINION.Options.textColor};
                        background-color: ${ticket.MINION.Options.screenColor};
                        position: absolute;
                        left: 50%;
                        top: 50%;
                        width: ${ticket.MINION.Options.bannerSize};
                        height: ${ticket.MINION.Options.bannerSize};
                        margin-left: ${ticket.MINION.Options.bannerLocation};
                        margin-top: ${ticket.MINION.Options.bannerLocation};
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

    if(ticket.MINION.Details.command == 'createProduct') {
        product = await createProduct(ticket);
    } else {
        product = await query(ticket);
    }
   
   console.log('product', product);
   return product;
};
