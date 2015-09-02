var dgram = require('dgram');

var comm = require("./comm.js");

var ProtoBuf = require("protobufjs");
var BenchMark = ProtoBuf.loadProtoFile("../proto/benchMark.proto").build("BenchMark");

var userNum = 3000;
var loginSucNum = 0;
var loginFailNum = 0;

var timeOrigin = (new Date()).getTime();

var allSockets = new Array();

for(var i = 0; i < userNum; i++)
{
	var socket = dgram.createSocket("udp4");
	allSockets.push(socket);

	var req = new BenchMark.ReqUdpLogin();
	req.charId = i + 1;
	var dataBuff = req.encode().toBuffer();

	comm.udpSend(socket, -21, dataBuff, onPack);
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
	}
	else if(packId === -1022)
	{
	}

	if((loginSucNum + loginFailNum) === userNum)
	{
		var now = (new Date()).getTime();
		console.log("suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);
	}
}


