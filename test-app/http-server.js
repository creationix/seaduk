/*global nucleus,require,p*/
nucleus.dofile("bootstrap.js");

var httpCodec = require('deps/http-codec');
var createServer = require("deps/net").createServer;
var uv = nucleus.uv;

var server = createServer({
  port: 8080, // comment out this line to listen on ephemeral high port
  encode: httpCodec.encoder(),
  decode: httpCodec.decoder(),
}, function (client) {
  "use strict";
  p("client", client, client.socket.getpeername());
  client.read(onRead);
  function onRead(err, data) {
    if (err) throw err;
    p("onRead", data);
    if (data) return client.read(onRead);
    if (data === "") {
      client.write({
        code: 200,
        headers: [
          "Server", "seaduk",
          "Date", new Date().toUTCString(),
          "Connection", "close",
          "Content-Length", "12",
          "Content-Type", "text/plain",
        ]
      });
      client.write("Hello World\n");
      client.write(null, function (err) {
        if (err) throw err;
        print("Closing server");
        server.close();
      });
    }
  }
});

p("New server created", server, server.getsockname());

// Start the libuv event loop
uv.run();

print("Event loop exiting...");
