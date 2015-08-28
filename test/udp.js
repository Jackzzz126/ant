var dgram = require('dgram');

var config = require("./config.json")

var socket = dgram.createSocket("udp4");
var buff = new Buffer("Hello world.");
console.log("send: \"%s\".", buff);
socket.send(buff, 0, buff.length, config.udpPort, config.ip);

socket.on("message", onMsg);

function onMsg(msg, addr)
{
	console.log("recv: \"%s\" from %s:%d.", msg, addr.address, addr.port);
}


