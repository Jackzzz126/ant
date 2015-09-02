var http = require("http");
var net = require("net");

var config = require("./config.js").config;

function httpRequest(options, data, func)
{
	options.host = config.ip;
	options.port = config.port;

	options.headers = new Object( );
	if( data !== null )
	{
		options.headers["Content-Length"] = data.length;
	}
	else
	{
		options.headers["Content-Length"] = 0;
	}
	var url = options.host + ":" + options.port + options.path;

	var req = http.request( options, onResponse );
	if( data !== null )
	{
		req.write( data );
	}
	req.on( 'error', function( e ) {
		if( func )
		{
			func( 'error when connect ' + url + ': ' + e.message, null );
		}
		} );
	req.end( );

	function onResponse( res )
	{
		if( res.statusCode != 200 )
		{
			if( func )
			{
				func( 'error when connect ' + url + ', return code: ' + res.statusCode, null );
			}
		}
		var chunkArray = new Array( );
		res.on('data', function (chunk) {
			chunkArray.push(chunk);
			});
		res.on('end', function ( ) {
			var buff = Buffer.concat(chunkArray);
			if( func )
			{
				func( null, buff );
			}
			});
	}
}

function tcpConnect(connFunc, packFunc)
{
	var socket = net.createConnection(config.port, config.ip);
	socket.on("connect", onConn);
	socket.on("error", onError);
	socket.on("data", onRecvData);

	function onConn()
	{
		connFunc(socket);
	}
	function onError(err)
	{
		console.log("Conn error.");
		console.log(err);
		process.exit(0);
	}

	var dataPacksRecved = new Array( );
	function onRecvData(dataBuff)
	{
		dataPacksRecved.push(dataBuff);

		while(true)
		{
			var totalLength = 0;
			for(var i in dataPacksRecved)
			{
				totalLength += dataPacksRecved[i].length;
			}

			if(totalLength < config.HEAD_LENGTH)
			{
				break;
			}

			if(dataPacksRecved[0].length < config.HEAD_LENGTH)
			{
				var buff = Buffer.concat(dataPacksRecved);
				dataPacksRecved = new Array();
				dataPacksRecved.push(buff);
			}

			var packId = dataPacksRecved[0].readInt32LE(0) ^ config.HEAD_MASK;
			var packLen = dataPacksRecved[0].readInt32LE(4) ^ config.HEAD_MASK;
			if(totalLength < (packLen + config.HEAD_LENGTH))
			{
				break;
			}
			var recvBuff = Buffer.concat(dataPacksRecved);
			packFunc(packId, packLen, recvBuff.slice(config.HEAD_LENGTH, packLen + config.HEAD_LENGTH));

			if(totalLength > (packLen + config.HEAD_LENGTH))
			{
				dataPacksRecved = new Array();
				dataPacksRecved.push(recvBuff.slice(packLen + config.HEAD_LENGTH, totalLength));
			}
			else//==
			{
				dataPacksRecved = new Array();
				break;
			}
		}
	}
}

function udpSend(socket, packId, dataBuff, packFunc)
{
	socket.on("message", onMsg);

	var headBuff = new Buffer(config.HEAD_LENGTH);
	headBuff.writeInt32LE(packId ^ config.HEAD_MASK, 0);
	headBuff.writeInt32LE(dataBuff.length ^ config.HEAD_MASK, 4);

	var buffs = new Array();
	buffs.push(headBuff);
	buffs.push(dataBuff);
	var sendBuff = Buffer.concat(buffs);

	socket.send(sendBuff, 0, sendBuff.length, config.udpPort, config.ip);
	
	function onMsg(msg, addr)
	{
		if(msg.length < config.HEAD_LENGTH)
			return;

		var packId = msg.readInt32LE(0) ^ config.HEAD_MASK;
		var packLen = msg.readInt32LE(4) ^ config.HEAD_MASK;
		if((packLen + config.HEAD_LENGTH) !== msg.length)
		{
			return;
		}
		packFunc(packId, msg.slice(config.HEAD_LENGTH, msg.length));
	}
}

exports.httpRequest = httpRequest;
exports.tcpConnect = tcpConnect;
exports.udpSend = udpSend;

