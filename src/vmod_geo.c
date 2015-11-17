#include <stdlib.h>
#include <maxminddb.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "include/vct.h"
#include "vcc_if.h"
#include "vmod_geo.h"

// open the maxmind db once, during initialization.
int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf) 
{
    MMDB_s *mmdb_handle = get_handle();
    int mmdb_baddb = open_mmdb(mmdb_handle);
    if (!mmdb_baddb) {
        priv->priv = (void *)mmdb_handle;
        priv->free = close_mmdb;
    }
    return mmdb_baddb;
}


// Lookup a field
const char *
vmod_lookup(struct sess *sp, const char *ipstr, const char **lookup_path)
{
    char *data = NULL;
    char *cp   = NULL;

    data = geo_lookup(ipstr,lookup_path);

    if (data != NULL) {
        cp = WS_Dup(sp->wrk->ws, data);
        free(data);
    }
        
    return cp;
}

// Lookup up a weather code
const char *
vmod_lookup_weathercode(struct sess *sp, const char *ipstr)
{
    char *data = NULL;
    char *cp   = NULL;

    data = geo_lookup_weather(ipstr);
    
    if (data != NULL) {
        cp = WS_Dup(sp->wrk->ws, data);
        free(data);
    }

    return cp;
}


const char*
vmod_city(struct sess *sp, const char *ipstr)
{
    const char *lookup_path[] = {"city", "names", "en", NULL};
    return vmod_lookup(sp, ipstr, lookup_path);
}

const char*
vmod_country(struct sess *sp, const char *ipstr)
{
    const char *lookup_path[] = {"country", "names", "en", NULL};
    return vmod_lookup(sp, ipstr, lookup_path);
}

const char*
vmod_metro_code(struct sess *sp, const char *ipstr)
{
    const char *lookup_path[] = {"location", "metro_code", NULL};
    return vmod_lookup(sp, ipstr, lookup_path);
}

const char*
vmod_region(struct sess *sp, const char *ipstr)
{
    const char *lookup_path[] = {"subdivisions", "0", "iso_code", NULL};
    return vmod_lookup(sp, ipstr, lookup_path);
}

const char*
vmod_country_code(struct sess *sp, const char *ipstr)
{
    const char *lookup_path[] = {"country", "iso_code", NULL};
    return vmod_lookup(sp, ipstr, lookup_path);
}

const char*
vmod_weather_code(struct sess *sp, const char *ipstr)
{
    return vmod_lookup_weathercode(sp, ipstr);
}
