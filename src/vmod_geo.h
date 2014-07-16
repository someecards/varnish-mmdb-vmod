// function to give to vcl
void 
close_mmdb(void *);

// function to open the maxmind db once
int 
open_mmdb(MMDB_s *);

// function to get a value from the returned mmdb lookup
char *
get_value(MMDB_lookup_result_s *, const char **);
