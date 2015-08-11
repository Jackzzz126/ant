var http = require("http");
var net = require("net");

var config = require("../config/config.json")

function httpRequest(options, data, func)
{
	options.host = config.server.ip;
	options.port = config.server.port;

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

function connect(connFunc, packFunc)
{
	var socket = net.createConnection(config.server.port, config.server.ip);
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
	}

	var dataPacksRecved = new Array( );
	function onRecvData(dataBuff)
	{
		var headLen = 8;
		dataPacksRecved.push(dataBuff);

		var totalLength = 0;
		for(var i in dataPacksRecved)
		{
			totalLength += dataPacksRecved[i].length;
		}
		if(totalLength < headLen)
		{
			return;
		}

		if(dataPacksRecved[0].length < headLen)
		{
			var buff = Buffer.concat(dataPacksRecved);
			dataPacksRecved = new Array();
			dataPacksRecved.push(buff);
		}

		var packId = dataPacksRecved[0].readInt32LE(0) ^ 0x79669966;
		var packLength = dataPacksRecved[0].readInt32LE(4) ^ 0x79669966;
		if(totalLength >= (packLength + headLen))
		{
			var recvBuff = Buffer.concat(dataPacksRecved);
			packFunc(packId, packLength, recvBuff.slice(headLen, packLength + headLen));
			if(totalLength > (packLength + headLen))
			{
				dataPacksRecved = new Array();
				dataPacksRecved.push(recvBuff.slice(packLength + headLen, totalLength));
			}
		}
	}
}

exports.httpRequest = httpRequest;
exports.connect = connect;

