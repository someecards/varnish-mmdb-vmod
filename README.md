#Varnish vmod for mmdb

## Installation
Requires:
 * libmaxminddb from https://github.com/maxmind/libmaxminddb
 * varnish source the version you're installing into https://github.com/varnish/Varnish-Cache
 * GeoLite2 IP db http://geolite.maxmind.com/download/geoip/database/GeoLite2-City.mmdb.gz *note: must be installed to /mnt/mmdb/GeoLite2-City.mmdb*

### Install GeoLite2 database
```
mkdir /mnt/mmdb/
cd /mnt/mmdb/
wget http://geolite.maxmind.com/download/geoip/database/GeoLite2-City.mmdb.gz
gunzip GeoLite2-City.mmdb.gz
```

### Install vmod
```
git clone git@github.com:someecards/varnish-mmdb-vmod.git
cd varnish-mmdb-vmod
./autogen.sh
./configure VARNISHSRC=/path/to/varnish/source VMODDIR=/usr/lib/varnish/vmods
make
make install
```

## Example testing VCL

```
import geo;
import std;

sub vcl_recv{
 // Do our best to be sure the original IP is passed on (from various proxies)
 if (!req.http.X-Forwarded-For || req.http.X-Forwarded-For == "") {
     set req.http.X-Forwarded-For = client.ip;
 } else {
     set req.http.X-Forwarded-For = regsub(req.http.X-Forwarded-For,",.*", "");
 }

 std.syslog(180, geo.city(req.http.X-Forwarded-For));
 std.syslog(180, geo.country(req.http.X-Forwarded-For));
}
```
