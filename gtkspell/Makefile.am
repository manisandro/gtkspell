## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES =
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =

if ENABLE_GTK3
lib_LTLIBRARIES += libgtkspell3-3.la
libgtkspell3_3_la_CPPFLAGS = -DPACKAGE_LOCALE_DIR=\""$(localedir)"\" @GTK_SPELL3_CFLAGS@
libgtkspell3_3_la_includedir=$(includedir)/gtkspell-3.0/gtkspell
libgtkspell3_3_la_include_HEADERS = gtkspell.h

libgtkspell3_3_la_SOURCES = gtkspell.c gtkspell.h
libgtkspell3_3_la_LIBADD = @GTK_SPELL3_LIBS@
libgtkspell3_3_la_LDFLAGS = -version-info $(GTK_SPELL_SO_VERSION)

pkgconfig_DATA += gtkspell3-3.0.pc
endif

if ENABLE_GTK2
lib_LTLIBRARIES += libgtkspell3-2.la
libgtkspell3_2_la_CPPFLAGS = -DPACKAGE_LOCALE_DIR=\""$(localedir)"\" @GTK_SPELL2_CFLAGS@
libgtkspell3_2_la_includedir=$(includedir)/gtkspell-3.0/gtkspell
libgtkspell3_2_la_include_HEADERS = gtkspell.h

libgtkspell3_2_la_SOURCES = gtkspell.c gtkspell.h
libgtkspell3_2_la_LIBADD = @GTK_SPELL2_LIBS@
libgtkspell3_2_la_LDFLAGS = -version-info $(GTK_SPELL_SO_VERSION)

pkgconfig_DATA += gtkspell3-2.0.pc
endif

EXTRA_DIST = $(pkgconfig_DATA)

include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION
INTROSPECTION_GIRS =

if ENABLE_GTK3
GtkSpell-3.0.gir: libgtkspell3-3.la
GtkSpell_3_0_gir_INCLUDES = GObject-2.0 GLib-2.0 Gtk-3.0
GtkSpell_3_0_gir_CFLAGS = --warn-all @GTK_SPELL_CFLAGS@
GtkSpell_3_0_gir_LIBS = libgtkspell3-3.la
GtkSpell_3_0_gir_FILES = $(libgtkspell3_3_la_SOURCES)
INTROSPECTION_GIRS += GtkSpell-3.0.gir
endif

if ENABLE_GTK2
GtkSpell-2.0.gir: libgtkspell3-2.la
GtkSpell_2_0_gir_INCLUDES = GObject-2.0 GLib-2.0 Gtk-2.0
GtkSpell_2_0_gir_CFLAGS = --warn-all @GTK_SPELL2_CFLAGS@
GtkSpell_2_0_gir_LIBS = libgtkspell3-2.la
GtkSpell_2_0_gir_FILES = $(libgtkspell3_2_la_SOURCES)
INTROSPECTION_GIRS += GtkSpell-2.0.gir
endif

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(gir_DATA) $(typelib_DATA)
endif