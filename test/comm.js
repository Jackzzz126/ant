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

function tcpConnect(connFunc, packFunc)
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
		console.log(err);
		process.exit(0);
	}

	var dataPacksRecved = new Array( );
	function onRecvData(dataBuff)
	{
		var HEADLEN = 8;
		dataPacksRecved.push(dataBuff);

		while(true)
		{
			var totalLength = 0;
			for(var i in dataPacksRecved)
			{
				totalLength += dataPacksRecved[i].length;
			}

			if(totalLength < HEADLEN)
			{
				break;
			}

			if(dataPacksRecved[0].length < HEADLEN)
			{
				var buff = Buffer.concat(dataPacksRecved);
				dataPacksRecved = new Array();
				dataPacksRecved.push(buff);
			}

			var packId = dataPacksRecved[0].readInt32LE(0) ^ 0x79669966;
			var packLen = dataPacksRecved[0].readInt32LE(4) ^ 0x79669966;
			if(totalLength < (packLen + HEADLEN))
			{
				break;
			}
			var recvBuff = Buffer.concat(dataPacksRecved);
			packFunc(packId, packLen, recvBuff.slice(HEADLEN, packLen + HEADLEN));

			if(totalLength > (packLen + HEADLEN))
			{
				dataPacksRecved = new Array();
				dataPacksRecved.push(recvBuff.slice(packLen + HEADLEN, totalLength));
			}
			else//==
			{
				dataPacksRecved = new Array();
				break;
			}
		}
	}
}

exports.httpRequest = httpRequest;
exports.tcpConnect = tcpConnect;

