////////////////////////////////////////////
//            GLOBAL VARIABLES
////////////////////////////////////////////
const debug = require('../requires/debugLog.js').debug;
const fs = require('fs');
const http = require('http');

var basePath = __dirname + '/../_html2keyCode';

//##########################################
const staticServe = function(req, res) {
//##########################################
debug.log(`Enter staticServe, basePath: ${basePath}, req.url: ${req.url}`);
try {
	fs.readFile(basePath+req.url, function(err,data) {
		//res.setHeader('Content-Type', 'text/html');

		if (err) {
			res.writeHead(404);
			res.end(JSON.stringify(err));
			return;
		};
		
		res.writeHead(200);
		res.end(data);
	});
}
catch (error) {
	console.log(`Invalid Messag: ${error}`);
	connection.send(`Invalid Messag: ${error}`);
}};

////////////////////////////////////////////
//                MAIN
//Open server to listen for control clients
////////////////////////////////////////////
console.log(`Loaded htmlServer.js`);
var httpServer = http.createServer(staticServe);
	
	httpServer.listen(80);
