var p = nucleus.dofile('deps/utils.js').prettyPrint;
var uv = nucleus.uv;

var host = "luvit.io";

uv.getaddrinfo({
      node: host,
      socktype: "stream", // Only show TCP results
      //family: "inet",  // Only show IPv4 results
}, function (err, results) {
      if(err) {
        print("Lookup failed : ",err);
        return;
      }
      p("Dns results for luvit.io",results);
});

try {
    var dns = new uv.Dns();
    p('Sync single lookup/resolve : ', dns.lookup_sync(host), dns.resolve_sync(host), dns.resolve4_sync(host), dns.resolve6_sync(host));
} catch(e){
    print('Failed : ',e);
}

uv.run();
