#include <maxminddb.h>
// can come by way of configure --with-maxminddbfile 
#ifndef MAX_CITY_DB
#define MAX_CITY_DB "/mnt/mmdb/GeoLite2-City.mmdb"
#endif

#ifndef VMOD_GEO_H
#define VMOD_GEO_H

MMDB_s *
get_handle(void);

// function to give to vcl
void 
close_mmdb(void *);

// function to open the maxmind db once
int 
open_mmdb(MMDB_s *);

// function to get a value from the returned mmdb lookup
char *
get_value(MMDB_lookup_result_s *, const char **);

const char *
geo_lookup(const char *ipstr, const char **lookup_path);

char *
geo_lookup_weather(const char *ipstr);

#endif
