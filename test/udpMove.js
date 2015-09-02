var dgram = require('dgram');

var config = require("./config.js").config;

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
	socket.on("message", onMsg);

	var req = new BenchMark.ReqUdpLogin();
	req.charId = i + 1;
	var dataBuff = req.encode().toBuffer();

	var headBuff = new Buffer(config.HEAD_LENGTH);
	headBuff.writeInt32LE(-21 ^ config.HEAD_MASK, 0);
	headBuff.writeInt32LE(dataBuff.length ^ config.HEAD_MASK, 4);

	var buffs = new Array();
	buffs.push(headBuff);
	buffs.push(dataBuff);
	var sendBuff = Buffer.concat(buffs);

	socket.send(sendBuff, 0, sendBuff.length, config.udpPort, config.ip);
}

function onMsg(msg, addr)
{
	var packId = msg.readInt32LE(0) ^ config.HEAD_MASK;
	var packLen = msg.readInt32LE(4) ^ config.HEAD_MASK;

	if(packLen + config.HEAD_LENGTH == msg.length && packId === -1021)
	{
		var res = BenchMark.ResUdpLogin.decode(msg.slice(config.HEAD_LENGTH, msg.length));
		if(res.status === 0)
		{
			loginSucNum++;
		}
		else
		{
			loginFailNum++;
		}
	}

	if((loginSucNum + loginFailNum) === userNum)
	{
		var now = (new Date()).getTime();
		console.log("suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);
	}
}


