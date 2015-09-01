var dgram = require('dgram');

var config = require("./config.json")

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

	var headBuff = new Buffer(8);
	headBuff.writeInt32LE(-21 ^ 0x79669966, 0);
	headBuff.writeInt32LE(dataBuff.length ^ 0x79669966, 4);

	var buffs = new Array();
	buffs.push(headBuff);
	buffs.push(dataBuff);
	var sendBuff = Buffer.concat(buffs);

	socket.send(sendBuff, 0, sendBuff.length, config.udpPort, config.ip);
}

function onMsg(msg, addr)
{
	var packId = msg.readInt32LE(0) ^ 0x79669966;
	var packLen = msg.readInt32LE(4) ^ 0x79669966;

	if(packLen + 8 == msg.length && packId === -1022)
	{
		var res = BenchMark.ResUdpLogin.decode(msg.slice(8, msg.length));
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


