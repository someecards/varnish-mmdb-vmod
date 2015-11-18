#include <stdlib.h>
#include <string.h>
#include <maxminddb.h>
#include "vmod_geo.h"

#ifndef DEBUG
#define DEBUG 0
#endif

static MMDB_s mmdb_handle;
static int    mmdb_baddb;

static char* MMDB_CITY_PATH = MAX_CITY_DB;
static char* DEFAULT_WEATHER_CODE = "New YorkNYUS";

MMDB_s *
get_handle() 
{
    return &mmdb_handle;
}
// close gets called by varnish when then the treads destroyed
void close_mmdb(void *db) 
{
    // don't do anything if the db didn't open correctly.
    if (mmdb_baddb)
        return;
    MMDB_s *handle = (MMDB_s *)db;
    MMDB_close(handle);
}

// Open the maxmind db file
int
open_mmdb(MMDB_s *mmdb) {
    mmdb_baddb = MMDB_open(MMDB_CITY_PATH, MMDB_MODE_MMAP, mmdb);
    if (mmdb_baddb != MMDB_SUCCESS) {
        #ifdef DEBUG
        fprintf(stderr, "[ERROR] open_mmdb: Can't open %s - %s\n",
                MMDB_CITY_PATH, MMDB_strerror(mmdb_baddb));
        if (MMDB_IO_ERROR == mmdb_baddb) {
            fprintf(stderr, "[ERROR] open_mmdb: IO error: %s\n", strerror(mmdb_baddb));
        }
        #endif
        return 1;
    }
    return 0;
}

// lookup an ip address using the maxmind db and return the value
// lookup_path described in this doc: http://maxmind.github.io/MaxMind-DB/
const char *
geo_lookup(const char *ipstr, const char **lookup_path) 
{
    char *data = NULL;

    if (mmdb_baddb)
        return NULL;

    // Lookup IP in the DB
    int gai_error, mmdb_error;
    MMDB_lookup_result_s result =
        MMDB_lookup_string(&mmdb_handle, ipstr, &gai_error, &mmdb_error);
    
    if (0 != gai_error) {
        #ifdef DEBUG
        fprintf(stderr,
                "[INFO] Error from getaddrinfo for %s - %s\n\n",
                ipstr, gai_strerror(gai_error));
        #endif
        return NULL;
    }


    if (MMDB_SUCCESS != mmdb_error) {
        #ifdef DEBUG
        fprintf(stderr,
                "[ERROR] Got an error from libmaxminddb: %s\n\n",
                MMDB_strerror(mmdb_error));
        #endif
        return NULL;
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
                    "[WARN] Got an error looking up the entry data. Make sure the lookup_path is correct. %s\n",
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
            case MMDB_DATA_TYPE_DOUBLE:
                asprintf(&data, "%f", entry_data.double_value);
                break;
            default:
                #ifdef DEBUG
                fprintf(
                        stderr,
                        "[WARN] No handler for entry data type (%d) was found\n",
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
            "[INFO] No entry for this IP address (%s) was found\n",
            ipstr);
        #endif
        exit_code = 5;
    }

    if (exit_code != 0) {
        data = calloc(1, sizeof(char));
    }
    return data;
}

 
// Given a valid result and some entry data, lookup a value
// NOTE: You must free() the return value if != NULL
//  
// @result - pointer to a result after calling MMDB_lookup_string
// @path - lookup value for MMDB_aget_value
// @return - NULL on failure

char *
get_value(MMDB_lookup_result_s *result, const char **path) {

    MMDB_entry_data_s entry_data;
    int status  = MMDB_aget_value( &(*result).entry, &entry_data, path);
    char *value = NULL;

    if (MMDB_SUCCESS != status) {
        #ifdef DEBUG
        fprintf(
                stderr,
                "[WARN] get_value got an error looking up the entry data. Make sure you use the correct path - %s\n",
                MMDB_strerror(status));
        #endif
        return NULL;
    }
    
    if (entry_data.has_data) {
        switch(entry_data.type) {           
        case MMDB_DATA_TYPE_UTF8_STRING:
            value = strndup(entry_data.utf8_string, entry_data.data_size);
            break;
        case MMDB_DATA_TYPE_UINT16:
            value = malloc(entry_data.data_size);
            sprintf(value, "%u", entry_data.uint16);
            break;
        case MMDB_DATA_TYPE_DOUBLE:
            asprintf(&value, "%f", entry_data.double_value);
            break;
        default:
            #ifdef DEBUG
            fprintf(
                    stderr,
                    "[WARN] get_value: No handler for entry data type (%d) was found. \n",
                    entry_data.type);
            #endif
            break;
        }
    }
    return value;
}


// This function builds up a code we need to lookup weather
// using Accuweather data.
// country code                     e.g. US
// city                             e.g. Beverly Hills
// if country code == US, get region e.g. CA
// And then return "Beverly HillsCAUS" if a US address or
//                    "Paris--FR" if non US
char *
geo_lookup_weather(const char *ipstr) 
{
    
    if (mmdb_baddb) {
        return strdup(DEFAULT_WEATHER_CODE);
    }

    char *data;

    // Lookup IP in the DB
    int ip_lookup_failed, db_status;
    MMDB_lookup_result_s result =
        MMDB_lookup_string(&mmdb_handle, ipstr, &ip_lookup_failed, &db_status);

    if (ip_lookup_failed) {
        #ifdef DEBUG
        fprintf(stderr,
                "[WARN] vmod_lookup_weathercode: Error from getaddrinfo for IP: %s Error Message: %s\n",
                ipstr, gai_strerror(ip_lookup_failed));
        #endif
        return strdup(DEFAULT_WEATHER_CODE);
    }

    if (db_status != MMDB_SUCCESS) {
        #ifdef DEBUG
        fprintf(stderr,
                "[ERROR] vmod_lookup_weathercode: libmaxminddb failure. \
Maybe there is something wrong with the file: %s libmaxmind error: %s\n",
                MMDB_CITY_PATH,
                MMDB_strerror(db_status));
        #endif
        return strdup(DEFAULT_WEATHER_CODE);
    }

    // these varaibles will hold our results
    char *country = NULL;
    char *city    = NULL;
    char *state   = NULL;

    // these are used to extract values from the mmdb
    const char *country_lookup[] = {"country", "iso_code", NULL};
    const char *city_lookup[]    = {"city", "names", "en", NULL};
    const char *state_lookup[]   = {"subdivisions", "0", "iso_code", NULL};

    if (result.found_entry) {
        
        country = get_value(&result, country_lookup);
        city    = get_value(&result, city_lookup);

        if (country != NULL && strcmp(country,"US") == 0) {
            state = get_value(&result, state_lookup);
        } else {
            state = strdup("--");
        }

        // we should always return new york
        if (country == NULL || city == NULL || state == NULL) {
            data = strdup(DEFAULT_WEATHER_CODE);
        } else {
            size_t chars = (sizeof(char)* ( strlen(country) + strlen(city) + strlen(state)) ) + 1;
            data = malloc(chars);
            sprintf(data, "%s%s%s", city, state, country);
        }

    } else {
        #ifdef DEBUG
        fprintf(
                stderr,
                "[INFO] No entry for this IP address (%s) was found\n",
                ipstr);
        #endif
        data = strdup(DEFAULT_WEATHER_CODE);
    }
    if (country != NULL)
        free(country);

    if (city != NULL)
        free(city);

    if (state != NULL)
        free(state);

    return data;
}
