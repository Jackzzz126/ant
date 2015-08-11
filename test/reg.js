var comm = require("./comm");

var ProtoBuf = require("protobufjs");
var BenchMark = ProtoBuf.loadProtoFile("../proto/benchMark.proto").build("BenchMark");

//reg "regNum" users
var regNum = 1;
var sucNum = 0;
var failNum = 0;

comm.connect(onConnect, onPack);
function onConnect(socket)
{
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
	if((sucNum + failNum) === regNum)
	{
		console.log("suc: %d, fail: %d", sucNum, failNum);
		process.exit(0);
	}
}


