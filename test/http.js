var http = require("http");
var comm = require("./comm");


options = new Object();
options.host = "192.168.1.21";
options.port = 8003;
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

