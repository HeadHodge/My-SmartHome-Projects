//var mysql = require('mysql');
//help

var fs = require('fs');

exports.handler = async (event) => {
    console.log("connectionId: \n" + JSON.stringify(event.requestContext.connectionId, null, 2))

    const directory = '/mnt/root';
    //const directory = __dirname;
    var filenames = fs.readdirSync(directory);
  
    console.log("\nCurrent directory filenames in: " + directory + ', count: ' + filenames.length);
   
    filenames.forEach(file => {
        console.log(file);
    });

    const response = {
        statusCode: 200,
        body: JSON.stringify('Hello from Lambda!'),
    };
    return response;
};
