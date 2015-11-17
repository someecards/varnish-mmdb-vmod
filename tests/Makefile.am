AM_CFLAGS = -Wall -D_GNU_SOURCE -DMAX_CITY_DB=\"$(MAX_CITY_DB)\"
AM_CPPFLAGS = -I$(VARNISHSRC)/include -I$(VARNISHSRC) -I$(top_srcdir)/src

tests.c: .FORCE
	ruby $(top_srcdir)/auto/generate_test_runner.rb tests.c test_Runner.c

.FORCE:


bin_PROGRAMS = testrunner
testrunner_LDFLAGS = -lmaxminddb
testrunner_SOURCES = \
	tests.c \
	unity.c \
	../src/geo_functions.c \
	test_Runner.c
