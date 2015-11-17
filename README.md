#Varnish vmod for GeoMaxMind DB
https://www.maxmind.com/en/home

## Installation
Requires:
 * Varnish 3.X
 * libmaxminddb from https://github.com/maxmind/libmaxminddb
 * varnish source the version you're installing into https://github.com/varnish/Varnish-Cache
 * GeoLite2 IP db http://geolite.maxmind.com/download/geoip/database/GeoLite2-City.mmdb.gz *note: must be installed to /mnt/mmdb/GeoLite2-City.mmdb*


```
cd /usr/local/src
git clone https://github.com/varnish/Varnish-Cache.git
cd Varnish-Cache
git branch 3.0 -t origin/3.0
git checkout 3.0
# make sure i'm matching release versions - 3.0.5 in my case.
git checkout 1a89b1f75895bbf874e83cfc6f6123737a3fd76f
./autogen.sh
./configure --prefix=/usr/local
make
make install
cd ..
git clone --recursive https://github.com/maxmind/libmaxminddb.git
cd libmaxminddb
./bootstrap
./configure --prefix=/usr/local
make 
make install
cd ..
git clone git@github.com:russellsimpkins/varnish-mmdb-vmod.git
cd varnish-mmdb-vmod
./autogen.sh
./configure --prefix=/usr --with-maxminddbfile=/mnt/mmdb/GeoIP2-City.mmdb VARNISHSRC=/usr/local/src/Varnish-Cache VMODDIR=/usr/lib64/varnish/vmods
make
make install
```

I added --with-maxminddbfile to autoconf so that you can decide, when you build the module, where you're data file will live. If you don't specify a value the default will be used.

### Install GeoLite2 database
```
mkdir /mnt/mmdb/
cd /mnt/mmdb/
wget http://geolite.maxmind.com/download/geoip/database/GeoLite2-City.mmdb.gz
gunzip GeoLite2-City.mmdb.gz
```

If you choose to use a different location, `#define MAX_CITY_DB "/path/to/GeoLite2-City.mmdb"`

### Install vmod
```
git clone git@github.com:someecards/varnish-mmdb-vmod.git
cd varnish-mmdb-vmod
./autogen.sh
./configure VARNISHSRC=/path/to/varnish/source VMODDIR=/usr/lib/varnish/vmods
make
make install
```

This will work with the free data or the licensed data. 

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
