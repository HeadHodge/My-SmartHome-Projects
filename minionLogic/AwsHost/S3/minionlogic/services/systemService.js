////////////////////////////////////////////////////////
////////////////// systemServices //////////////////////
////////////////////////////////////////////////////////
console.log('***Load systemServices');

var aws        = require('aws-sdk');
var s3Client   = new aws.S3({});
var fs         = require('fs');
var apiGateway = new aws.ApiGatewayManagementApi({
    endpoint: 'https://85igzlcgab.execute-api.us-west-2.amazonaws.com/production/',
});

/////////////////////////////////////////////////////////
var renameObject = async function(currentKey, newKey) {
//////////////////////////////////////////////////////////
console.log(`***renameObject, currentKey: ${cureentKey}, newKey: ${newKey}`);

//renameObject
	await s3Client.copyObject('minionlogic', currentKey, 'minionlogic', newKey).promise();
};

/////////////////////////////////////////////////////////
var saveObject = async function(objectKey, object) {
//////////////////////////////////////////////////////////
console.log(`***loadObject: ${objectKey}`);
var objectBuffer;

//saveObject
	params = {
        Bucket     : 'minionlogic',
        Key        : objectKey,
        Body       : JSON.stringify(object),
        ContentType: 'text/plain',
    };

    console.log(`saveObject, params: `, params);
    object = await s3Client.putObject(params).promise();
};

/////////////////////////////////////////////////////////
var loadObject = async function(objectKey) {
//////////////////////////////////////////////////////////
console.log(`***loadObject: ${objectKey}`);
var objectBuffer;

//getObject
    var params = {
        Bucket: 'minionlogic',
        Key   : objectKey,
    };
  
  console.log(`getObject, params: : `, params);
  var object = await s3Client.getObject(params).promise();
  
  return JSON.parse(object.Body.toString('utf-8')); 
};

//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name      : 'systemServices',
    aws       : aws,
    fs        : fs,
    s3Client  : s3Client,
    apiGateway: apiGateway,
    loadObject: loadObject,
	saveObject: saveObject,
};