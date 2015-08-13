var comm = require("./comm");

var reqNum = 10000;
var sucNum = 0;
var failNum = 0;

var timeOrigin = (new Date()).getTime();

options = new Object();
options.path = "/hello";
options.method = "GET";

for(var i = 0; i < reqNum; i++)
{
	comm.httpRequest(options, null, onReturn);
}
function onReturn(errStr, reply)
{
	if(errStr === null)
	{
		sucNum++;
	}
	else
	{
		console.log(errStr);
		failNum++;
	}
	if((sucNum + failNum) % 1000 === 0)
	{
		var now = (new Date()).getTime();
		console.log("suc: %d, fail: %d, in %d ms.", sucNum, failNum, now - timeOrigin);
	}
	if((sucNum + failNum) === reqNum)
	{
		process.exit(0);
	}
}

