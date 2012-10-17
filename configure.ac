dnl Process this file with autoconf to produce a configure script.

m4_define(gtkspell_version_minor, 0)
m4_define(gtkspell_version_micro, 0)

AC_PREREQ(2.62)
AC_INIT(gtkspell3, 3.gtkspell_version_minor.gtkspell_version_micro)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_SRCDIR(gtkspell/gtkspell.c)
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall])

AM_SILENT_RULES([yes])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_PROG_CC
AM_PROG_CC_C_O

GTK_SPELL_VERSION_MINOR=gtkspell_version_minor
GTK_SPELL_VERSION_MICRO=gtkspell_version_micro

dnl ************************ Enable Gtk2 GtkSpell build ***********************
AC_ARG_ENABLE([gtk2],
     AS_HELP_STRING([--enable-gtk2], [enable gtk2 build (default is no)]),
     [case "${enableval}" in
       yes) gtk2=true ;;
       no)  gtk2=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-gtk2]) ;;
     esac],[gtk2=false])
AM_CONDITIONAL([ENABLE_GTK2], [test x$gtk2 = xtrue])

dnl ************************ Enable Gtk3 GtkSpell build ***********************
AC_ARG_ENABLE([gtk3],
     AS_HELP_STRING([--enable-gtk3], [enable gtk3 build (default is yes)]),
     [case "${enableval}" in
       yes) gtk3=true ;;
       no)  gtk3=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-gtk3]) ;;
     esac],[gtk3=true])
AM_CONDITIONAL([ENABLE_GTK3], [test x$gtk3 = xtrue])

dnl ************************** Gobject Introspection **************************
dnl See https://live.gnome.org/GObjectIntrospection/AutotoolsIntegration
GOBJECT_INTROSPECTION_CHECK([1.30.0])

dnl *************************** Internationalization **************************
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=$PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

dnl ************************** Libtool initialization *************************
LT_INIT

dnl ********************************* Modules *********************************
if test x$gtk3 = xtrue; then
	PKG_CHECK_MODULES(GTK_SPELL3, [glib-2.0 gtk+-3.0 enchant ])
	SPEC_ENABLE_GTK3=1
fi

if test x$gtk2 = xtrue; then
	PKG_CHECK_MODULES(GTK_SPELL2, [glib-2.0 gtk+-2.0 enchant ])
	SPEC_ENABLE_GTK2=1
fi

dnl ********************************* GTK-Doc *********************************
dnl See http://developer.gnome.org/gtk-doc-manual/unstable/settingup_autoconf.html.en
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
SPEC_ENABLE_GTK_DOC=1
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AC_SUBST(GTK_SPELL_VERSION_MINOR)
AC_SUBST(GTK_SPELL_VERSION_MICRO)
AC_SUBST(SPEC_ENABLE_GTK_DOC)
AC_SUBST(SPEC_ENABLE_GTK2)
AC_SUBST(SPEC_ENABLE_GTK3)
AC_SUBST(GTK_SPELL2_CFLAGS)
AC_SUBST(GTK_SPELL2_LIBS)
AC_SUBST(GTK_SPELL3_CFLAGS)
AC_SUBST(GTK_SPELL3_LIBS)

AC_OUTPUT([
Makefile
gtkspell3.spec
gtkspell/gtkspell3-2.0.pc
gtkspell/gtkspell3-3.0.pc
gtkspell/Makefile
examples/Makefile
docs/Makefile
docs/version.xml
po/Makefile.in])
