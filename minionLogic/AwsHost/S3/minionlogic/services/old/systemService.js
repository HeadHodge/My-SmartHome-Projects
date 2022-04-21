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
console.log(`***renameObject, currentKey: ${currentKey}, newKey: ${newKey}`);
var params;

//renameObject

//copy new
	console.log(`copy current object: ${currentKey}`);
	
	params = {
		Bucket    : `minionlogic`, 
		CopySource: `/minionlogic/${currentKey}`, 
		Key       : `${newKey}`,
 };
	
	await s3Client.copyObject(params).promise();

//delete old
	console.log(`delete current object: ${currentKey}`);
	params = {
        Bucket     : 'minionlogic',
        Key        : currentKey,		
	};
	
    await s3Client.deleteObject(params).promise();
};

/////////////////////////////////////////////////////////
var deleteObject = async function(objectKey) {
//////////////////////////////////////////////////////////
console.log(`***deleteObject: ${objectKey}`);

//deleteObject
	params = {
        Bucket     : 'minionlogic',
        Key        : objectKey,
    };

    console.log(`deleteObject, params: `, params);
    object = await s3Client.deleteObject(params).promise();
};

/////////////////////////////////////////////////////////
var saveObject = async function(objectKey, object) {
//////////////////////////////////////////////////////////
console.log(`***saveObject: ${objectKey}`);

//saveObject
	params = {
        Bucket     : 'minionlogic',
        Key        : objectKey,
        Body       : JSON.stringify(object),
        ContentType: 'text/plain',
    };

    console.log(`saveObject, params: `, params.bucket, params.key);
    object = await s3Client.putObject(params).promise();
};

/////////////////////////////////////////////////////////
var loadFile = async function(objectKey) {
//////////////////////////////////////////////////////////
console.log(`***loadFile: ${objectKey}`);

//getObject
    var params = {
        Bucket: 'minionlogic',
        Key   : objectKey,
    };
  
  console.log(`loadFile, params: : `, params);
  var object = await s3Client.getObject(params).promise();
  
  return object.Body.toString('utf-8'); 
};

/////////////////////////////////////////////////////////
var loadObject = async function(objectKey) {
//////////////////////////////////////////////////////////
console.log(`***loadObject: ${objectKey}`);

//getObject
    var params = {
        Bucket: 'minionlogic',
        Key   : objectKey,
    };
  
  console.log(`loadObject, params: : `, params);
  var object = await s3Client.getObject(params).promise();
  
  return JSON.parse(object.Body.toString('utf-8')); 
};

//////////////////////////////////////////////////////////
module.exports = {
//////////////////////////////////////////////////////////
    name        : 'systemServices',
    aws         : aws,
    fs          : fs,
    s3Client    : s3Client,
    apiGateway  : apiGateway,
	loadFile    : loadFile,
    loadObject  : loadObject,
	saveObject  : saveObject,
	deleteObject: deleteObject,
	renameObject: renameObject,
};