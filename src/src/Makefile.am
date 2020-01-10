## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@GTK3_CFLAGS@ @LIBXML2_CFLAGS@ @UNIQUE3_CFLAGS@

bin_PROGRAMS = iok

iok_SOURCES = \
	main.c keyevent.c keys.h\
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

iok_LDADD = @GTK3_LIBS@ @LIBXML2_LIBS@ @UNIQUE3_LIBS@ $(INTLLIBS)

