var http = require("http");

function httpRequest(options, data, func)
{
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

exports.httpRequest = httpRequest;

