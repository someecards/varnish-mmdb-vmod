#Varnish vmod for mmdb

## Installation
Requires libmaxminddb from https://github.com/maxmind/libmaxminddb

```
git clone git@github.com:someecards/varnish-mmdb-vmod.git
cd somegeo-vmod; ./autogen.sh
./configure VARNISHSRC=[PATH_TO_VARNISH_SRC] VMODDIR=/usr/lib/varnish/vmods
make && make install
```

## Example

```
import geo;
import std;

sub vcl_recv{
 set req.http.X-Forwarded-For = client.ip;
 std.syslog(180, geo.city(req.http.X-Forwarded-For));
 std.syslog(180, geo.country(req.http.X-Forwarded-For));
}
```
