orderMinion = {
	TICKET: {
		client   : 'HeadHodge',
		action	 : 'activateMinion',
		dueDate  : 'now',
		reportTo : 'job210.step27',
		signature: 'HeadHodge',
	},
			
	MINION: {
		name: 'minionLogic.helloWorld.example',
		task: 'fillOrder',
	},
		
	DATA: {
		message   : 'Hello from minionLogic',
	},
};
