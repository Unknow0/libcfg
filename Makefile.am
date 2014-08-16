EXTRA_DIST=README.md

lib_LTLIBRARIES=libcfg.la
libcfg_la_SOURCES=src/cfg.c

include_HEADERS=src/cfg.h

EXTRA_PROGRAMS=test
test_SOURCES=src/main.c
test_LDADD=libcfg.la

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libcfg.pc

distclean-local:
	-rm -rf Makefile.in aclocal.m4 autom4te.cache/ config.guess config.sub depcomp install-sh ltmain.sh missing
