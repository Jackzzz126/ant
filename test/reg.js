var comm = require("./comm");

var ProtoBuf = require("protobufjs");
var BenchMark = ProtoBuf.loadProtoFile("../proto/benchMark.proto").build("BenchMark");

//reg "regNum" users
var regNum = 10000;
var sucNum = 0;
var failNum = 0;

var timeOrigin = (new Date()).getTime();

comm.tcpConnect(onConnect, onPack);
function onConnect(socket)
{
	var req = new BenchMark.ReqReg();
	for(var i = 0; i < regNum; i++)
	{
		req.charId = i + 1;
		req.name = "user-" + req.charId;
		req.pwd = "pwd-" + req.charId;

		var dataBuff = req.encode().toBuffer();

		var headBuff = new Buffer(config.HEAD_LENGTH);
		headBuff.writeInt32LE(-10 ^ config.HEAD_MASK, 0);
		headBuff.writeInt32LE(dataBuff.length ^ config.HEAD_MASK, 4);

		socket.write(headBuff);
		socket.write(dataBuff);
	}
}

function onPack(packId, packLen, buff)
{
	var res = BenchMark.ResReg.decode(buff);
	if(res.status === 0)
	{
		sucNum++;
	}
	else
	{
		failNum++;
	}
	if((sucNum + failNum) % 1000 === 0)
	{
		var now = (new Date()).getTime();
		console.log("	suc: %d, fail: %d, in %d ms.", sucNum, failNum, now - timeOrigin);
	}
	if((sucNum + failNum) === regNum)
	{
		var now = (new Date()).getTime();
		console.log("suc: %d, fail: %d, in %d ms.", sucNum, failNum, now - timeOrigin);
		process.exit(0);
	}
}


