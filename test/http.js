var http = require("http");
var comm = require("./comm");
var config = require("../config/config.json")


options = new Object();
options.host = config.server.ip;
options.port = config.server.port;
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

