## Process this file with automake to produce Makefile.in

SUBDIRS = src po data

EXTRA_DIST = README.old \
	autogen.sh \
	iok.desktop.in \
	intltool-extract.in  \
        intltool-merge.in    \
        intltool-update.in


Applicationsdir = $(datadir)/applications
Applications_in_files = iok.desktop.in
Applications_DATA = $(Applications_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

DISTCLEANFILES=                                 \
        intltool-extract                        \
        intltool-merge                  \
        intltool-update                 \
        iok.desktop

install-data-local:
	@$(NORMAL_INSTALL)

dist-hook:
	$(MAKE) -C "$(srcdir)/po" "$(GETTEXT_PACKAGE).pot"
	cp "$(srcdir)/po/$(GETTEXT_PACKAGE).pot" "$(distdir)/po/"


# Manpage
  iokmandir = $(mandir)/man1
  dist_iokman_DATA = iok.1

