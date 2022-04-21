var aws = require('aws-sdk'); aws.config.update({region: 'us-west-2'});
var ses = new aws.SES({ region: "us-west-2" });
var s3 = new aws.S3({});

var newClient = {
    runtimeBalance: 60, //free minutes
};

//////////////////////////////////////////////////////////
async function sendMail(address, subject, content) {
//////////////////////////////////////////////////////////
const params = {
    Destination: {
        ToAddresses: [address],
    },
        
    Message: {
        Body: {
            Text: { Data: content },
        },

        Subject: { Data: subject },
    },
    
    Source: 'support@hodgemail.us',
};

// Send registration email
    console.log(`send new registration email to: `, address);
    await ses.sendEmail(params).promise();  
}

//////////////////////////////////////////////////////////
async function registerClient(connectOptions) {
//////////////////////////////////////////////////////////
console.log('Enter registerClient: ', connectOptions);
var client, params;

// verify credentials exist
    if(!connectOptions.client || !connectOptions.email || !connectOptions.pin) throw "Invalid Credentials";
        
try {
    
// try to load client
    params = {
        Bucket: 'minionlogic',
        Key: `clients/${connectOptions.client}/${connectOptions.client}.json`,
    };
        
    console.log(`load client, get params: `, params);
    var object = await s3.getObject(params).promise();
    client = JSON.parse(object.Body.toString('utf-8'));
    
} catch(err) {
    
//Client Not Found
    console.log(`***Client Not Found: `, client, err.message);
    
//Register new client
    client = Object.assign({
        email: connectOptions.email,
        pin  : connectOptions.pin,
    }, newClient);

    await sendMail(connectOptions.email, 'New minionLogic Client Registration Notice','Welcome to minionLogic!.');
}
    
// authenticate client
    console.log(`verify connection matches client credentials: `, connectOptions, client);
    if(client.email != connectOptions.email || client.pin != connectOptions.pin) throw "Credential Mismatch, retry or reset.";

// update client
    client.connection = connectOptions.connection;
    
    params = {
        Bucket     : 'minionlogic',
        Key        : `clients/${connectOptions.client}/${connectOptions.client}.json`,
        Body       : JSON.stringify(client),
        ContentType: 'text/plain',
    };
        
    console.log(`***update authorized client, put params: `, params);
    await s3.putObject(params).promise();
}

///////////////////////////////////////////////
/////////////////  MAIN  //////////////////////
///////////////////////////////////////////////
exports.handler = async (event) => {
console.log('**connectEndpoint**, connectionId: \n' + JSON.stringify(event.requestContext.connectionId, null, 2));
var params = {}, connectionInfo = event["queryStringParameters"];

try {
    connectionInfo.connection = event.requestContext.connectionId;

    await registerClient(connectionInfo);

// save connection info by connectionId
    params = {
        Bucket: 'minionlogic',
        Key   : `connections/${connectionInfo.connection}.json`,
        Body  : JSON.stringify(connectionInfo),
        ContentType: 'text/plain',
    };
        
    console.log(`***Save connection by connectionId, params: `, params);
    await s3.putObject(params).promise();

}  catch (err) {
    console.log(`***ABORT: `, err);
    
    return {
        statusCode: 401,
        body: JSON.stringify('***CONNECTION ABORTED'),
    };
}

    console.log(`***CONNECTED: `);
    return {
        statusCode: 200,
        body: JSON.stringify('***CONNECTED'),
    };
};
