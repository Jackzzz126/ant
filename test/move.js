var comm = require("./comm");

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
	comm.tcpConnect(onConnect, onPack);
}
function onConnect(socket)
{
	allSockets.push(socket);
	var req = new BenchMark.ReqLogin();
	req.charId = allSockets.length;
	req.pwd = "pwd-" + req.charId;

	var dataBuff = req.encode().toBuffer();

	var headBuff = new Buffer(config.HEAD_LENGTH);
	headBuff.writeInt32LE(-11 ^ config.HEAD_MASK, 0);
	headBuff.writeInt32LE(dataBuff.length ^ config.HEAD_MASK, 4);

	socket.write(headBuff);
	socket.write(dataBuff);
}

function onPack(packId, packLen, buff)
{
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
			var now = (new Date()).getTime();
			console.log("	suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);
		}
		if((loginSucNum + loginFailNum) === userNum)
		{
			var now = (new Date()).getTime();
			console.log("suc: %d, fail: %d, in %d ms.", loginSucNum, loginFailNum, now - timeOrigin);
			setInterval(sendMove, 1000 / sendFreqency);
			setInterval(statusOut, 1000);
			//process.exit();
		}
	}
	else if(packId === -1012)//res move
	{
		var res = BenchMark.ResMove.decode(buff);

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
		var req = new BenchMark.ReqMove();
		req.charId = i + 1;
		req.x = 1;
		req.y = 1;
		req.time = now - timeOrigin;

		var dataBuff = req.encode().toBuffer();

		var headBuff = new Buffer(config.HEAD_LENGTH);
		headBuff.writeInt32LE(-12 ^ config.HEAD_MASK, 0);
		headBuff.writeInt32LE(dataBuff.length ^ config.HEAD_MASK, 4);

		allSockets[i].write(headBuff);
		allSockets[i].write(dataBuff);
		moveSendNum++;
	}
}

