AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2011 Varnish Software AS])
AC_INIT([libvmod-geo], [trunk])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/vmod_geo.vcc)
AC_CONFIG_HEADERS(config.h)

AC_CANONICAL_SYSTEM
AC_LANG(C)

AM_INIT_AUTOMAKE([foreign])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_STDC
if test "x$ac_cv_prog_cc_c99" = xno; then
	AC_MSG_ERROR([Could not find a C99 compatible compiler])
fi
AC_PROG_CPP

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Check for rst utilities
AC_CHECK_PROGS(RST2MAN, [rst2man rst2man.py], "no")
if test "x$RST2MAN" = "xno"; then
	AC_MSG_WARN([rst2man not found - not building man pages])
fi
AM_CONDITIONAL(HAVE_RST2MAN, [test "x$RST2MAN" != "xno"])

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/stdlib.h])

# Check for python
AC_CHECK_PROGS(PYTHON, [python3 python3.1 python3.2 python2.7 python2.6 python2.5 python2 python], [AC_MSG_ERROR([Python is needed to build this vmod, please install python.])])

# Varnish source tree
AC_ARG_VAR([VARNISHSRC], [path to Varnish source tree (mandatory)])
if test "x$VARNISHSRC" = x; then
	AC_MSG_ERROR([No Varnish source tree specified])
fi
VARNISHSRC=`cd $VARNISHSRC && pwd`
AC_CHECK_FILE([$VARNISHSRC/include/varnishapi.h],
	[],
	[AC_MSG_FAILURE(["$VARNISHSRC" is not a Varnish source directory])]
)

# Check for libmaxminddb
AC_ARG_WITH([maxminddb],
    [AS_HELP_STRING([--with-maxminddb],
	[the directory where the maxminddb SDK is installed in])],
    [
        CPPFLAGS="$CPPFLAGS -I${withval}/include"
        LDFLAGS="$LDFLAGS -L${withval}/lib"
    ],
    [:]
)
AC_ARG_WITH([maxminddbfile],
    [AS_HELP_STRING([--with-maxminddbfile],
	[the maximind city db file e.g. /mnt/mmdb/GeoLite2-City.mmdb])],
    [
        AC_SUBST(MAX_CITY_DB,[${withval}])
    ],
    [:]
)
AC_CHECK_LIB([maxminddb], [MMDB_get_entry_data_list],
    [AC_CHECK_HEADER([maxminddb.h],
        [],
        [AC_MSG_ERROR([Failed to find the maxminddb SDK (1). Specify --with-maxminddb=PATH])]
    )],
    [AC_MSG_ERROR([Failed to find the maxminddb SDK (2). Specify --with-maxminddb=PATH])]
)
LIBS="$LIBS -lmaxminddb"

# vmod installation dir
AC_ARG_VAR([VMODDIR], [vmod installation directory @<:@LIBDIR/varnish/vmods@:>@])
if test "x$VMODDIR" = x; then
	VMODDIR=`pkg-config --variable=vmoddir varnishapi`
	if test "x$VMODDIR" = x; then
		AC_MSG_FAILURE([Can't determine vmod installation directory through pkgconfig])
	fi
fi

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	tests/Makefile
])
AC_OUTPUT
