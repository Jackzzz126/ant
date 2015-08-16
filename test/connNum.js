var comm = require("./comm");

var batchSize = 1000;
var sucessNum = 0;

nextBatch();
function nextBatch()
{
	var finishNum = 0;
	for(var i = 0; i < batchSize; i++)
	{
		comm.tcpConnect(onConnect, onPack);
	}

	function onConnect(socket)
	{
		sucessNum++;
		finishNum++;
		if(finishNum >= batchSize)
		{
			console.log("%d success.", sucessNum);
			nextBatch();
		}
	}

	function onPack(packId, packLen, buff)
	{
	}
}


