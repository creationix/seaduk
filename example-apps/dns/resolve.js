var p = nucleus.dofile('deps/utils.js').prettyPrint;
var uv = nucleus.uv;

// TODO : DNS
var host = "www.amazon.com";

try {
    var dns = new uv.Dns();
    print("Lookup : "+dns.lookup_sync(host));
    print("Resolve : "+dns.resolve_sync(host));
} catch(e){
    print("Failed : ",e);
}

uv.run();
