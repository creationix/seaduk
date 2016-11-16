var p = nucleus.dofile('deps/utils.js').prettyPrint;
var uv = nucleus.uv;

// TODO : DNS
var ntpServer = "194.109.22.18";
var ntpPort = 123;
var running = true;

// Taken from https://github.com/annejan/nodemcu-lua-watch/blob/master/ntp.lua
var req = new Uint8Array([227, 0, 6, 236, 0,0,0,0,0,0,0,0, 49, 78, 49, 52,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]);
var data = new Buffer(req);

var server = new uv.Udp();
server.bind('0.0.0.0',0);

server.send(ntpServer, ntpPort , data.toString('binary') , function (err) {
  if (err) {throw err;}
  p('NTP packet sent. Waiting for response', server);
  server.readStart(function (err, resp) {
    if (err) {throw err;}
    p('NTP packet received. ('+resp.length+' bytes)', server);

    server.readStop();

    var highw = resp[40] * 256 + resp[41];
    var loww = resp[42] * 256 + resp[43];
    var ntpstamp=( highw * 65536 + loww );
    var lstamp = ntpstamp - 1104494400 - 1104494400 	
    var newDate = new Date(lstamp*1000);
    p(newDate);
    running = false;
  });
});

var timer1 = new uv.Timer();
timer1.start(1000, 2000, function () {
  if (!running) {
       timer1.stop();
       timer1.close();
       return;
  }
  server.send(ntpServer, ntpPort , data.toString('binary') , function (err) {
    if(err) {throw err;}
    print('Sent another NTP package');
  });
});

uv.run();
