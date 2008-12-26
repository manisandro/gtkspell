dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([gtkspell],[2.0.15],[gtkspell-devel@lists.sourceforge.net])
dnl N.B. After 0.40, please change to `0.40+cvs' instead of `0.41cvs'.
dnl Rationale: (i) placate simple version comparison software such as
dnl `dpkg --compare-versions'.  (ii) We don't always know what the next
dnl version is going to be called until about the time we release it
dnl (whereas we always know what the previous version was called).
dnl (extracted from inkscape/configure.ac)
AC_CONFIG_SRCDIR(gtkspell/gtkspell.c)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])

SPELLER_LIB=-lenchant
      
AC_SUBST(SPELLER_LIB)
GTKSPELL_PACKAGES=gtk+-2.0
AC_SUBST(GTKSPELL_PACKAGES)
PKG_CHECK_MODULES(GTKSPELL, $GTKSPELL_PACKAGES enchant >= 0.4.0 )
AC_SUBST(GTKSPELL_CFLAGS)
AC_SUBST(GTKSPELL_LIBS)

# check Pango version so we can use PANGO_UNDERLINE_ERROR when available:
PKG_CHECK_MODULES(PANGO, [pango >= 1.3.5],
   [AC_DEFINE(HAVE_PANGO_UNDERLINE_ERROR, 1, [Define to 1 if PANGO_UNDERLINE_ERROR is available.])],
   [AC_MSG_WARN([Disabling new error underlining since pango < 1.3.5])])

# check Pango version to see if word breaking bug is fixed:
#PKG_CHECK_MODULES(PANGO, [pango >= 1.8.0],
#   [AC_DEFINE(FIXED_PANGO, 1, [Define to 1 if pango >= 1.8.0.])],
#   [AC_MSG_WARN([Enabling word-breaking workarounds since pango < 1.8.0])])

# gtk-doc check:
# gtk-doc check:
# from http://cvs.gnome.org/lxr/source/gtk-doc/examples/configure.in
AC_ARG_WITH(html-dir, AC_HELP_STRING([--with-html-dir=PATH],[path to installed docs]))

if test "x$with_html_dir" = "x" ; then
  HTML_DIR='${datadir}/gtk-doc/html'
else
  HTML_DIR=$with_html_dir
fi

AC_SUBST(HTML_DIR)

gtk_doc_min_version=1.0
AC_MSG_CHECKING([gtk-doc version >= $gtk_doc_min_version])
if pkg-config --atleast-version=$gtk_doc_min_version gtk-doc; then
  AC_MSG_RESULT(yes)
  GTKDOC=true
else
  AC_MSG_RESULT(no)
  GTKDOC=false
fi

dnl Let people disable the gtk-doc stuff.
AC_ARG_ENABLE(gtk-doc, AC_HELP_STRING([--enable-gtk-doc],[Use gtk-doc to build documentation [[default=auto]]]), enable_gtk_doc="$enableval", enable_gtk_doc=auto)

if test x$enable_gtk_doc = xauto ; then
  if test x$GTKDOC = xtrue ; then
    enable_gtk_doc=yes
  else
    enable_gtk_doc=no 
  fi
fi

AM_CONDITIONAL(ENABLE_GTK_DOC, test x$enable_gtk_doc = xyes)

# end gtk-doc check.

AC_PROG_CC
AC_PROG_LIBTOOL

AC_C_CONST

# gettext 
ALL_LINGUAS="be cs da de el eo es eu fi fr ga gl hu hy id it ja ky lt mn nb nl pl pt_BR rm ro ru rw sk sl sr sv tr uk vi wa zh_CN zh_HK zh_TW"
AM_GLIB_GNU_GETTEXT
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["${GETTEXT_PACKAGE}"], [gettext domain])
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL



dnl AM_MAINTAINER_MODE

AC_CONFIG_FILES(Makefile gtkspell/Makefile examples/Makefile gtkspell-2.0.pc gtkspell.spec docs/Makefile po/Makefile.in)
AC_OUTPUT

