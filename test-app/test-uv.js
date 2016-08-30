var p = nucleus.dofile("deps/utils.js").prettyPrint;
var uv = nucleus.uv;

function assert(condition, message) {
    if (!condition) {
        throw new Error(message || "Assertion failed")
    }
}

print("\nTimer.prototype");
p(uv.Timer.prototype);
print("Handle.prototype (via Timer.prototype)");
p(Object.getPrototypeOf(uv.Timer.prototype));

print("\nPrepare.prototype");
p(uv.Prepare.prototype);
print("Handle.prototype (via Prepare.prototype)");
p(Object.getPrototypeOf(uv.Prepare.prototype));

print("\nCheck.prototype");
p(uv.Check.prototype);
print("Handle.prototype (via Check.prototype)");
p(Object.getPrototypeOf(uv.Check.prototype));

print("\nIdle.prototype");
p(uv.Idle.prototype);
print("Handle.prototype (via Idle.prototype)");
p(Object.getPrototypeOf(uv.Idle.prototype));

print("\nAsync.prototype");
p(uv.Async.prototype);
print("Handle.prototype (via Async.prototype)");
p(Object.getPrototypeOf(uv.Async.prototype));

print("\nTcp.prototype");
p(uv.Tcp.prototype);
print("Stream.prototype (via Tcp.prototype)");
var streamProto = Object.getPrototypeOf(uv.Tcp.prototype);
p(streamProto);
print("Handle.prototype (via Stream.prototype)");
p(Object.getPrototypeOf(streamProto));

print("\nPipe.prototype");
p(uv.Pipe.prototype);
print("Stream.prototype (via Pipe.prototype)");
p(Object.getPrototypeOf(uv.Pipe.prototype));

print("\nTty.prototype");
p(uv.Tty.prototype);
print("Stream.prototype (via Tty.prototype)");
p(Object.getPrototypeOf(uv.Tty.prototype));

var prepare = new uv.Prepare();
prepare.start(function () {
  print("prepare...");
});
prepare.unref();

var check = new uv.Check();
check.start(function () {
  print("check...");
});

var idle = new uv.Idle();
idle.start(function () {
  print("idle...");
  idle.stop();
});

var async = new uv.Async(function () {
  print("async...");
});
p("async", async);
async.send();

print("\nTesting uv.walk");
var timer = new uv.Timer();
var tcp = new uv.Tcp();
uv.walk(p);
timer.close();
tcp.close();
check.unref();
idle.unref();
async.unref();

print("\nTesting simple timeout");
var timer = new uv.Timer();
timer.start(100, 0, function () {
  p("timeout", timer);
  timer.close(function () {
    p("closed", timer);
  });
});
uv.run();

print("\nTesting simple interval");
timer = new uv.Timer();
var count = 3;
timer.start(50, 50, function () {
  p("interval", timer);
  if (--count) return;
  timer.close(function () {
    p("close", timer);
  });
});
uv.run();


print("\nTest two concurrent timers");
var timer1 = new uv.Timer();
var timer2 = new uv.Timer();
timer1.start(100, 0, function () {
  p("timeout", timer1);
  timer1.close();
  timer2.close();
});
timer2.start(10, 20, function () {
  p("interval", timer2);
});
uv.run();

print("\nTest shrinking interval");
timer = new uv.Timer();
timer.start(10, 0, function () {
  var repeat = timer.getRepeat();
  p("interval", timer, repeat);
  if (repeat === 0) {
    timer.setRepeat(8);
    timer.again();
  }
  else if (repeat == 2) {
    timer.stop();
    timer.close();
  }
  else {
    timer.setRepeat(repeat / 2);
  }
});
uv.run();

assert(typeof uv.version === 'function','misc not compiled in seaduk');

var version = uv.version();
var version_string = uv.version_string();
p({
    version: version,
    version_string: version_string,
});
assert(typeof version === "number");
assert(typeof version_string === "string");

var rss = uv.resident_set_memory();
var total = uv.get_total_memory();
p({
  rss: rss,
  total: total
});
assert(rss < total);

var uptime = uv.uptime();
p({uptime: uptime});

var rusage = uv.getrusage();
p(rusage);

var info = uv.cpu_info();
p(info);

var addresses = uv.interface_addresses();
p(addresses);

var avg = uv.loadavg();
p({loadavg:avg});
assert(avg.length === 3);

var path = uv.exepath();
p({exepath: path});

var old = uv.cwd();
uv.chdir("/");
var cwd = uv.cwd();
p({
    original: old,
    changed: cwd
});
assert(cwd !== old);
uv.chdir(old);

var old = uv.get_process_title();
uv.set_process_title("Magic");
var changed = uv.get_process_title();
p({
    original: old,
    changed: changed,
});
assert(old !== changed);
uv.set_process_title(old);

var time = uv.hrtime();
p({"hrtime": time});

print("\nTesting TCP Server");
var server = new uv.Tcp();
server.bind("127.0.0.1", 8080);
server.listen(128, function (err) {
  if (err) throw err;
  var client = new uv.Tcp();
  server.accept(client);
  p("client", client, client.getpeername());
  client.readStart(function (err, chunk) {
    if (err) throw err;
    p("read", chunk);
    if (chunk) {
      client.write(chunk);
    }
    else {
      client.shutdown(function () {
        client.close();
        server.close();
      });
    }
  });
});
p("server", server, server.getsockname());

var client = new uv.Tcp();
client.connect("127.0.0.1", 8080, function (err) {
  if (err) throw err;
  p("client connected", client, client.getpeername(), client.getsockname());
  client.readStart(function (err, chunk) {
    if (err) throw err;
    p("client onread", chunk);
    if (!chunk) {
      client.close();
    }
  });
  client.write("Hello", function (err) {
    if (err) throw err;
    client.write("World");
    client.shutdown();
  });
});
uv.run();


// Use uv.Async to implement nextTick
var ticks = [];
var ticker = new uv.Async(function () {

  if (!ticks.length) return;
  var list = ticks;
  ticks = [];
  ticker.unref();
  list.forEach(function (cb) {
    cb();
  });
});

function nextTick(callback) {
  ticks.push(callback);
  ticker.ref();
  ticker.send();
}

print("before");
nextTick(function () {
  print("tick");
});
print("after");
uv.run();


print("Testing stdio with default mode");
var stdin = new uv.Tty(0, true);
var stdout = new uv.Tty(1, false);
p(stdin, stdout, stdin.getWinsize());
print("Type test, hit enter to send");
print("Press Control-D to stop test");
stdin.readStart(function (err, chunk) {
  if (err) throw err;
  p("stdin", chunk);
  if (chunk) {
    stdout.write(chunk);
  }
  else {
    stdin.readStop();
  }
});
uv.run();
stdin.close();
stdout.close();

ticker.close();
prepare.close();
check.close();
idle.close();
uv.run();
