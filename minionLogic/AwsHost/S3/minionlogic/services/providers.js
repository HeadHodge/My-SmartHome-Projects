////////////////////////////////////////////
//            providers.js
////////////////////////////////////////////
console.log(`***Load providers service...`);

///////////////////////////////////////////////
var pingServer = async function(serviceOrder) {
///////////////////////////////////////////////
console.log(`***pingServer: `, serviceOrder);

};

///////////////////////////////////////////////
var registerMinion = async function(notice) {
///////////////////////////////////////////////
console.log(`***registerMinion: `, notice.TASK.activity);

//get member names	
	var clientConnection = global.services.bootService.event.requestContext.connectionId;
	var connectionInfo = await services.systemService.loadObject(`connections/${clientConnection}.json`);
	var clientInfo = await services.systemService.loadObject(`members/${connectionInfo.member}/${connectionInfo.member}.json`);
	var clientName = clientInfo.member;

 //build registration
    registration = {
        member     : `${clientInfo.member}`,
        minionName : `${clientInfo.namespace}_${notice.OPTIONS.minionName}_${notice.OPTIONS.minionType}_${notice.OPTIONS.minionVersion}`,
        mode       : `Test`,
        royalty    : `${notice.OPTIONS.royalty}`,
        description: `${notice.OPTIONS.description}`,           
    };

//save registration
	await services.systemService.saveObject(`minions/${registration.minionName}/minion.json`, registration);

//confirm registration
    var confirmation = {
        SUBJECT: 'TASK-UPDATE',
        
        REPORT: {           
            activity  : 'registerMinion',
            minionName: `${registration.minionName}`,
            reference : 'job210.step27',
            progress  : 'REGISTERED',
            console   : 'Minion registered and waiting for orders.',
        },
        
        REGISTRATION  : JSON.stringify(registration),   
    }
    
	await global.services.bootService.postNotice(confirmation);
};

///////////////////////////////////////////////
var receivedNotice = async function(notice) {
///////////////////////////////////////////////
console.log(`***receivedNotice: `, notice.SUBJECT);

    if(notice.SUBJECT == 'MINION-ORDER') return await global.services.orderService.closeOrder(notice);

	if(notice.SUBJECT == 'PROVIDER-SERVICES') return await registerMinion(notice);
    
    throw new Error(`*** ABORT receivedNotice, wrong service for subject: ${notice.SUBJECT}`);
};

//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name          : 'providerServices',
	pingServer    : pingServer,
	receivedNotice: receivedNotice,
};