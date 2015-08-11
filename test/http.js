var http = require("http");
var comm = require("./comm");
var config = require("./config.json");


options = new Object();
options.host = config.ip;
options.port = config.port;
options.path = "/hello";
options.method = "GET";

comm.httpRequest(options, null, onReturn);
function onReturn(errStr, reply)
{
	if(errStr !== null)
	{
		console.log(errStr);
	}
	else
	{
		console.log(reply.toString());
	}
}

