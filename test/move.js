var comm = require("./comm");

var ProtoBuf = require("protobufjs");
var BenchMark = ProtoBuf.loadProtoFile("../proto/benchMark.proto").build("BenchMark");

var userNum = 1;
var loginSucNum = 0;
var loginFailNum = 0;

var moveSendNum = 0;
var moveRecvNum = 0;

var sendFreqency = 1;

var timeOrigin = (new Date()).getTime();

comm.connect(onConnect, onPack);
function onConnect(socket)
{
	var req = new BenchMark.ReqLogin();
	for(var i = 0; i < userNum; i++)
	{
		req.charId = i + 1;
		req.pwd = "pwd-" + req.charId;

		var dataBuff = req.encode().toBuffer();

		var headBuff = new Buffer(8);
		headBuff.writeInt32LE(-11 ^ 0x79669966, 0);
		headBuff.writeInt32LE(dataBuff.length ^ 0x79669966, 4);

		socket.write(headBuff);
		socket.write(dataBuff);
	}
}

function onPack(packId, packLen, buff)
{
	debugger;
	if(packId === -1011)//res login
	{
		var res = BenchMark.ResReg.decode(buff);
		if(res.status === 0)
		{
			loginSucNum++;
		}
		else
		{
			console.log(res.status);
			loginFailNum++;
		}
		if((loginSucNum + loginFailNum) % 1000 === 0)
		{
			debugger;
			var now = (new Date()).getTime();
			console.log("	suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);
		}
		if((loginSucNum + loginFailNum) === userNum)
		{
			debugger;
			var now = (new Date()).getTime();
			console.log("suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);
			setInterval(sendMove, 1000 / sendFreqency);
			setInterval(statusOut, 1000);
			//process.exit();
		}
	}
	else//res move
	{
		moveRecvNum++;
	}
}

function statusOut()
{
	debugger;
	console.log("%d send, %d recv.", moveSendNum, moveRecvNum);
	moveSendNum = 0;
	moveRecvNum = 0;
}

function sendMove()
{
	debugger;
	var req = new BenchMark.ReqMove();
	for(var i = 0; i < userNum; i++)
	{
		req.charId = i + 1;
		req.x = 1;
		req.y = 1;

		var dataBuff = req.encode().toBuffer();

		var headBuff = new Buffer(8);
		headBuff.writeInt32LE(-12 ^ 0x79669966, 0);
		headBuff.writeInt32LE(dataBuff.length ^ 0x79669966, 4);

		socket.write(headBuff);
		socket.write(dataBuff);
		moveSendNum++;
	}
}

