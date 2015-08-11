var comm = require("./comm");

options = new Object();
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

