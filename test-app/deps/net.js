/*jshint node:true*/
"use strict";

var uv = require('uv');
var wrapSocket = require('./wrap-socket');

exports.createServer = createServer;

// options.host -- tcp addr to bind to
// options.port -- tcp port to bind to
// options.backlog -- tcp backlog
// options.encode -- encode function
// options.decode -- decode function
// onClient(client)
//   client.read(callback) - read data from stream
//     client.read.update(newDecode) - update decoder
//   client.write(data, callback) - write data to stream
//     client.write.update(newEncode) - update encoder
//   client.socket - uv_tcp_t instance
function createServer(options, onClient) {
  var server = new uv.Tcp();
  server.bind(options.host || "127.0.0.1",
              options.port || 0);
  server.listen(options.backlog || 128, onConnection);

  function onConnection(err) {
    if (err) throw err;
    var client = new uv.Tcp();
    server.accept(client);
    var stream = wrapSocket(client, options.decode, options.encode);
    onClient(stream);
  }
  return server;
}
