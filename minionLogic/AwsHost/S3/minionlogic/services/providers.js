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

    var confirmation = {
        SUBJECT: 'TASK-UPDATE',
        
        REPORT: {           
            activity  : 'registerMinion',
            minionName: 'minionsGlobal_helloWorld_example_v1',
            reference : 'job210.step27',
            progress  : 'REGISTERED',
            console   : 'Minion sucessfully registered. Ready for orders.',
        },
    }
    
    //deliver order	to client
	await global.services.bootService.postNotice(confirmation);
};

///////////////////////////////////////////////
var receivedNotice = async function(notice) {
///////////////////////////////////////////////
console.log(`***receivedNotice: `, notice.SUBJECT);
	if(notice.SUBJECT != 'PROVIDER-SERVICES') throw new Error(`*** ABORT receivedNotice, wrong service for subject: ${notice.SUBJECT}`);

    await registerMinion(notice);
};

//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name          : 'providerServices',
	pingServer    : pingServer,
	receivedNotice: receivedNotice,
};