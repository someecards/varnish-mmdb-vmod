#Varnish vmod for mmdb

**NOTE**
This is for Varnish 3

## Installation
I'm going to assume you have nothing installed. 

Requires Varnish cache from https://github.com/varnish/Varnish-Cache

Requires libmaxminddb from https://github.com/maxmind/libmaxminddb

```
cd /usr/local/src
git clone https://github.com/varnish/Varnish-Cache.git
cd Varnish-Cache
git branch 3.0 -t origin/3.0
git checkout 3.0
./autogen.sh
./configure --prefix=/usr/local
make
make install
cd ..
git clone https://github.com/maxmind/libmaxminddb.git
cd libmaxminddb
./bootstrap
./configure --prefix=/usr/local
make 
make install
cd ..
git clone git@github.com:russellsimpkins/varnish-mmdb-vmod.git
cd varnish-mmdb-vmod
./autogen.sh
./configure --prefix=/usr/local VARNISHSRC=[PATH_TO_VARNISH_SRC] VMODDIR=/usr/local/lib/varnish/vmods
make
make install
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
