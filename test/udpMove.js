var dgram = require('dgram');

var comm = require("./comm.js");

var ProtoBuf = require("protobufjs");
var BenchMark = ProtoBuf.loadProtoFile("../proto/benchMark.proto").build("BenchMark");

var userNum = 3000;
var loginSucNum = 0;
var loginFailNum = 0;

var moveSendNum = 0;
var moveRecvNum = 0;
var totalTimeDelta = 0;

var sendFreqency = 1;

var timeOrigin = (new Date()).getTime();

var allSockets = new Array();

for(var i = 0; i < userNum; i++)
{
	var socket = dgram.createSocket("udp4");
	allSockets.push(socket);
	comm.udpSetCallBack(socket, onPack);

	var req = new BenchMark.ReqUdpLogin();
	req.charId = i + 1;
	var dataBuff = req.encode().toBuffer();

	comm.udpSend(socket, -21, dataBuff);
}

function onPack(packId, buff)
{
	if(packId === -1021)
	{
		var res = BenchMark.ResUdpLogin.decode(buff);
		if(res.status === 0)
		{
			loginSucNum++;
		}
		else
		{
			loginFailNum++;
		}

		if((loginSucNum + loginFailNum) === userNum)
		{
			var now = (new Date()).getTime();
			console.log("suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);

			setInterval(sendMove, 1000 / sendFreqency);
			setInterval(statusOut, 1000);
		}
	}
	else if(packId === -1022)
	{
		var res = BenchMark.ResUdpMove.decode(buff);

		var now = (new Date()).getTime();
		var timeDelta = now - timeOrigin - res.time;
		totalTimeDelta += timeDelta;

		moveRecvNum++;
	}
	else
	{
		console.log("Unknown pack recevied.");
		process.exit(1);
	}
}

function statusOut()
{
	console.log("%d send, %d recv, avg delay: %d.", moveSendNum, moveRecvNum,
		totalTimeDelta / moveRecvNum / 2);
	moveSendNum = 0;
	moveRecvNum = 0;
	totalTimeDelta = 0;
}

function sendMove()
{
	for(var i = 0; i < userNum; i++)
	{
		var now = (new Date()).getTime();
		var req = new BenchMark.ReqUdpMove();
		req.charId = i + 1;
		req.x = 1;
		req.y = 1;
		req.time = now - timeOrigin;

		var dataBuff = req.encode().toBuffer();
		comm.udpSend(allSockets[i], -22, dataBuff);

		moveSendNum++;
	}
}


