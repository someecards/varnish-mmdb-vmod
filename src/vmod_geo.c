#include <stdlib.h>
#include <maxminddb.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "include/vct.h"
#include "vcc_if.h"

static char* MMDB_CITY_PATH = "/mnt/mmdb/GeoLite2-City.mmdb";
static char* MMDB_COUNTRY_PATH = "/mnt/mmdb/GeoLite2-Country.mmdb";


int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
        return 1;
}

const char *
vmod_lookup(struct sess *sp, const char *ipstr, const char **lookup_path)
{
        MMDB_s mmdb;
        char *data = NULL;

        // Create DB connection
        int status = MMDB_open(MMDB_CITY_PATH, MMDB_MODE_MMAP, &mmdb);
        if (MMDB_SUCCESS != status) {
                #ifdef DEBUG
                fprintf(stderr, "\n  Can't open %s - %s\n",
                        MMDB_CITY_PATH, MMDB_strerror(status));

                if (MMDB_IO_ERROR == status) {
                    fprintf(stderr, "    IO error: %s\n", strerror(errno));
                }
                #endif
                exit(1);
        }

        // Lookup IP in the DB
        int gai_error, mmdb_error;
        MMDB_lookup_result_s result =
            MMDB_lookup_string(&mmdb, ipstr, &gai_error, &mmdb_error);

        if (0 != gai_error) {
            #ifdef DEBUG
            fprintf(stderr,
                    "\n  Error from getaddrinfo for %s - %s\n\n",
                    ipstr, gai_strerror(gai_error));
            #endif
            exit(2);
        }

        if (MMDB_SUCCESS != mmdb_error) {
            #ifdef DEBUG
            fprintf(stderr,
                    "\n  Got an error from libmaxminddb: %s\n\n",
                    MMDB_strerror(mmdb_error));
            #endif
            exit(3);
        }

        // Parse results
        MMDB_entry_data_s entry_data;
        int exit_code = 0;
        char* str = NULL;
        if (result.found_entry) {
            int status = MMDB_aget_value(&result.entry, &entry_data, lookup_path);

            if (MMDB_SUCCESS != status) {
                #ifdef DEBUG
                fprintf(
                    stderr,
                    "Got an error looking up the entry data - %s\n",
                    MMDB_strerror(status));
                #endif
                exit_code = 4;
            }

            if (entry_data.has_data) {
                switch(entry_data.type){
                    case MMDB_DATA_TYPE_UTF8_STRING:
                        data = strndup(entry_data.utf8_string, entry_data.data_size);
                        break;
                    case MMDB_DATA_TYPE_UINT16:
                        str = malloc(entry_data.data_size);
                        sprintf(str, "%u", entry_data.uint16);
                        data = strndup(str, entry_data.data_size);
                        free(str);
                        break;
                    default:
                        #ifdef DEBUG
                        fprintf(
                            stderr,
                            "\n  No handler for entry data type (%d) was found\n\n",
                            entry_data.type);
                        #endif
                        exit_code = 6;
                        break;
                }
            }
    } else {
        #ifdef DEBUG
        fprintf(
            stderr,
            "\n  No entry for this IP address (%s) was found\n\n",
            ipstr);
        #endif
        exit_code = 5;
    }
    if (exit_code != 0) {
        data = malloc(sizeof(char)*2);
        sprintf(data, "");
    }

    char *cp;
    cp = WS_Dup(sp->wrk->ws, data);
    free(data);
    MMDB_close(&mmdb);
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
