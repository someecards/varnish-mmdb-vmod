AM_CPPFLAGS = -I$(VARNISHSRC)/include -I$(VARNISHSRC)

vmoddir = $(VMODDIR)
vmod_LTLIBRARIES = libvmod_geo.la

libvmod_geo_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

libvmod_geo_la_SOURCES = \
	vcc_if.c \
	vcc_if.h \
	vmod_geo.c

vcc_if.c vcc_if.h: $(VARNISHSRC)/lib/libvmod_std/vmod.py $(top_srcdir)/src/vmod_geo.vcc
	@PYTHON@ $(VARNISHSRC)/lib/libvmod_std/vmod.py $(top_srcdir)/src/vmod_geo.vcc

EXTRA_DIST = \
	vmod_geo.vcc \
	$(VMOD_TESTS)

CLEANFILES = $(builddir)/vcc_if.c $(builddir)/vcc_if.h
