## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@GTK2_CFLAGS@ @LIBXML2_CFLAGS@ @UNIQUE_CFLAGS@

bin_PROGRAMS = iok

iok_SOURCES = \
	main.c keyevent.c keys.h\
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

iok_LDADD = @GTK2_LIBS@ @LIBXML2_LIBS@ @UNIQUE_LIBS@ $(INTLLIBS)

