//##################################################
// 					microServiceMinions
//##################################################
console.log(`**** Load microServiceMinions.options.js ****`);
if(typeof microServiceMinions == 'undefined') microServiceMinions = {}; //js namespace

//################
//    Options
//################
microServiceMinions.options = {
//"endpoint"   : "wss://5dl6cjqlqa.execute-api.us-west-2.amazonaws.com/test?name=HeadHodge&pin=6853",
"endpoint"   : "wss://providers.minionLogic.com?nickname=provider&member=provider@hodgemail.us&pin=7377&trialMode=on",
"authToken"  : 'eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiI1YmM0ZGYxNGY4ZGE0MTdkYTNhZjdkNjkwYzg0NDQ2ZSIsImlhdCI6MTYxMzAxMDQ4MiwiZXhwIjoxOTI4MzcwNDgyfQ.MffxNYX4VssITLgdZBPilKTq3p4R9RuoQP2yeeoyyPw',
"socket"     : undefined,
"isConnected": undefined,
}
