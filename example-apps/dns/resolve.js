var uv = nucleus.uv;

// TODO : DNS
var host = 'ct.de';

try {
    var dns = new uv.Dns();
    print('Lookup : '+dns.lookup_sync(host));
    print('Resolve : '+dns.resolve_sync(host));
    print('Resolve IPv4 : '+dns.resolve_v4_sync(host));
    print('Resolve IPv6 : '+dns.resolve_v6_sync(host));
} catch(e){
    print('Failed : ',e);
}

uv.run();
