AC_PREREQ(2.52)
AC_INIT(gtkspell, 2.0.3, martine@cs.washington.edu)
AC_CONFIG_SRCDIR(gtkspell/gtkspell.c)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_CHECK_HEADER(aspell.h, [AC_DEFINE(HAVE_ASPELL_H,1, 
   [Define to 1 if you have the <aspell.h> header file.] )] )
AC_CHECK_HEADER(pspell/pspell.h, [AC_DEFINE(HAVE_PSPELL_H,1, 
   [Define to 1 if you have the <pspell/pspell.h> header file.] )] )
AC_CHECK_LIB(aspell, new_aspell_speller, SPELLER_LIB="-laspell",
   [AC_CHECK_LIB(pspell, new_pspell_manager, SPELLER_LIB="-lpspell",
      [AC_MSG_ERROR([You must have the aspell or pspell dev libraries to build gtkspell.]) ] ) ] )
      
AC_SUBST(SPELLER_LIB)
GTKSPELL_PACKAGES=gtk+-2.0
AC_SUBST(GTKSPELL_PACKAGES)
PKG_CHECK_MODULES(GTKSPELL, $GTKSPELL_PACKAGES)
AC_SUBST(GTKSPELL_CFLAGS)
AC_SUBST(GTKSPELL_LIBS)

# This is a check for gtk-doc which you can insert into your configure.in.
# You shouldn't need to change it at all.

##################################################
# Check for gtk-doc.
##################################################

AC_ARG_WITH(html-dir, [  --with-html-dir=PATH path to installed docs ])

if test "x$with_html_dir" = "x" ; then
  HTML_DIR='${datadir}/gtk-doc/html'
else
  HTML_DIR=$with_html_dir
fi

AC_SUBST(HTML_DIR)

AC_CHECK_PROG(GTKDOC, gtkdoc-mkdb, true, false)

dnl XXX EVAN HACK; TEST DOES NOT WORK FOR VERSION 0.10
dnl gtk_doc_min_version=0.6
dnl if $GTKDOC ; then 
dnl     gtk_doc_version=`gtkdoc-mkdb --version`
dnl     AC_MSG_CHECKING([gtk-doc version ($gtk_doc_version) >= $gtk_doc_min_version])
dnl     if perl <<EOF ; then
dnl       exit (("$gtk_doc_version" =~ /^[[0-9]]+\.[[0-9]]+$/) &&
dnl             ("$gtk_doc_version" >= "$gtk_doc_min_version") ? 0 : 1);
dnl EOF
dnl       AC_MSG_RESULT(yes)
dnl    else
dnl       AC_MSG_RESULT(no)
dnl       GTKDOC=false
dnl    fi
dnl fi
GTKDOC=true
dnl XXX END EVAN HACK

dnl Let people disable the gtk-doc stuff.
AC_ARG_ENABLE(gtk-doc, [  --enable-gtk-doc  Use gtk-doc to build documentation [default=auto]], enable_gtk_doc="$enableval", enable_gtk_doc=auto)

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

dnl AM_MAINTAINER_MODE

AC_CONFIG_FILES(Makefile gtkspell/Makefile examples/Makefile gtkspell-2.0.pc gtkspell.spec docs/Makefile)
AC_OUTPUT

