var net = require("net");
var ProtoBuf = require("protobufjs");
var config = require("../config/config.json")

var BenchMark = ProtoBuf.loadProtoFile("../proto/benchMark.proto").build("BenchMark");

var socket = net.createConnection(config.server.port, config.server.ip);
socket.on("connect", onConnect);
socket.on("error", onError);
socket.on("data", onRecvData);

//reg "regNum" users
var regNum = 1;

function onConnect()
{
	debugger;
	var req = new BenchMark.ReqReg();
	for(var i = 0; i < regNum; i++)
	{
		req.name = "user-" + (i + 1);
		req.pwd = "pwd-" + (i + 1);

		var dataBuff = req.encode().toBuffer();

		var headBuff = new Buffer(8);
		headBuff.writeInt32LE(-10 ^ 0x79669966, 0);
		headBuff.writeInt32LE(dataBuff.length ^ 0x79669966, 4);

		socket.write(headBuff);
		socket.write(dataBuff);
	}
}

function onError(err)
{
	debugger;
	console.log("Conn error.");
}

var dataPacksRecved = new Array( );
function onRecvData(dataBuff)
{
	debugger;
	dataPacksRecved.push(dataBuff);

	var totalLength = 0;
	for(var i in dataPacksRecved)
	{
		totalLength += dataPacksRecved[i].length;
	}
	if(totalLength < 8)
	{
		return;
	}

	if(dataPacksRecved[0].length < 8)
	{
		var buff = Buffer.concat(dataPacksRecved);
		dataPacksRecved = new Array();
		dataPacksRecved.push(buff);
	}

	debugger;
	var packId = dataPacksRecved[0].readInt32LE(0) ^ 0x79669966;
	var packLength = dataPacksRecved[0].readInt32LE(4) ^ 0x79669966;
	if(totalLength >= (packLength + 8))
	{
		var recvBuff = Buffer.concat(dataPacksRecved);
		res = BenchMark.ResReg.decode(recvBuff.slice(8, packLength + 8));
		var x = 1;
	}
}

