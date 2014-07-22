#include "unity.h"
#include <maxminddb.h>
#include "vmod_geo.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_OpenMMDB(void)
{
	MMDB_s *mmdb_handle = get_handle();
	int mmdb_baddb = open_mmdb(mmdb_handle);
	TEST_ASSERT_EQUAL_INT(0,mmdb_baddb);
}

void test_BadIP(void)
{

	char * ip = "127.0.0.1";
	char * value = geo_lookup_weather(ip);
	char * expected = "New YorkNYUS";
	TEST_ASSERT_EQUAL_STRING(expected,value);
}

void test_CaliforniaIP(void)
{

	char * ip = "199.254.0.98";
	char * value = geo_lookup_weather(ip);
	char * expected = "Beverly HillsCAUS";
	TEST_ASSERT_EQUAL_STRING(expected,value);
}

void test_ParisFranceIP(void)
{
	char * ip = "88.190.229.170";
	char * value = geo_lookup_weather(ip);
	char * expected = "Paris--FR";
	TEST_ASSERT_EQUAL_STRING(expected,value);
}

void test_LookupCity(void)
{
	const char *lookup_path[] = {"city", "names", "en", NULL};
	char *ip = "199.254.0.98";
	const char *actual = geo_lookup(ip, lookup_path);
	char *expected = "Beverly Hills";
	TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_LookupState(void)
{
	const char *lookup_path[] = {"subdivisions", "0", "iso_code", NULL};
	char *ip = "199.254.0.98";
	const char *actual = geo_lookup(ip, lookup_path);
	char *expected = "CA";
	TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_LookupCountry(void)
{
	const char *lookup_path[] = {"country", "iso_code", NULL};
	char *ip = "199.254.0.98";
	const char *actual = geo_lookup(ip, lookup_path);
	char *expected = "US";
	TEST_ASSERT_EQUAL_STRING(expected, actual);
}
