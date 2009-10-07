SUBDIRS=gtkspell examples docs po

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=gtkspell-2.0.pc
DISTCLEANFILES=gtkspell-2.0.pc

dist-hook:
	$(MAKE) -C "$(srcdir)/po" "$(GETTEXT_PACKAGE).pot"
	cp "$(srcdir)/po/$(GETTEXT_PACKAGE).pot" "$(distdir)/po/"

