AC_PREREQ(2.52)
AC_INIT(gtkspell, 2.0.3, martine@cs.washington.edu)
AC_CONFIG_SRCDIR(gtkspell/gtkspell.c)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_CHECK_LIB(pspell,new_pspell_manager, ,
	AC_ERROR([You must have the pspell dev libraries to build gtkspell.]))

GTKSPELL_PACKAGES=gtk+-2.0
AC_SUBST(GTKSPELL_PACKAGES)
PKG_CHECK_MODULES(GTKSPELL, $GTKSPELL_PACKAGES)
AC_SUBST(GTKSPELL_CFLAGS)
AC_SUBST(GTKSPELL_LIBS)

AC_PROG_CC
AC_PROG_LIBTOOL

AC_C_CONST

dnl AM_MAINTAINER_MODE

AC_CONFIG_FILES(Makefile gtkspell/Makefile examples/Makefile gtkspell-2.0.pc gtkspell.spec)
AC_OUTPUT

