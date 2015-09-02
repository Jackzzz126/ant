var comm = require("./comm");

var totalConnNum = 20000;
var packSize = 128;
var sendFreqency = 1;
var packId = -1;//-1: echo, -2: double echo, -3:rw db

var timeOrigin = (new Date()).getTime();

var sendCount = 0;
var recvCount = 0;
var totalTimeDelta = 0;

var successConnNum = 0;
var allConns = new Array();

for(var i = 0; i < totalConnNum; i++)
{
	comm.tcpConnect(onConnect, onPack);
}
function onConnect(socket)
{
	successConnNum++;
	allConns.push(socket);
	if(successConnNum >= totalConnNum)
	{
		console.log("%d conn connect successfully.", successConnNum);
		setInterval(sendMsg, 1000 / sendFreqency);
		setInterval(statusOut, 1000);
	}
}

function statusOut()
{
	console.log("%d send, %d recv, avg delay: %d.", sendCount, recvCount, totalTimeDelta / recvCount / 2);
	sendCount = 0;
	recvCount = 0;
	totalTimeDelta = 0;
}

function sendMsg()
{
	for(var i in allConns)
	{
		var now = (new Date()).getTime();

		var dataBuff = new Buffer(packSize);
		dataBuff.writeInt32LE(now - timeOrigin , 0);

		comm.send(allConns[i], packId, dataBuff);
		sendCount++;
	}
}

function onPack(packId, packLen, buff)
{
	recvCount++;

	var timeStamp = buff.readInt32LE(0);
	var now = (new Date()).getTime();
	var timeDelta = now - timeOrigin - timeStamp;
	totalTimeDelta += timeDelta;
}


