lib_LTLIBRARIES=libgtkspell-3.la
AM_CFLAGS=@GTKSPELL_CFLAGS@ -DG_LOG_DOMAIN=\"gtkspell\" -DLOCALEDIR=\""$(datadir)/locale"\"
libgtkspell_3_la_LIBADD=@GTKSPELL_LIBS@ @SPELLER_LIB@
libgtkspell_3_la_includedir=$(includedir)/gtkspell-3.0/gtkspell
libgtkspell_3_la_include_HEADERS=gtkspell.h
libgtkspell_3_la_SOURCES=gtkspell.c deprecated.c gtkspell.h
