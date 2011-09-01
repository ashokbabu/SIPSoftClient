AC_PREREQ([2.59])

AC_INIT
AC_CONFIG_MACRO_DIR([m4])

AS_VERSION(sipclient, TELEPATHY_SIP_VERSION, 0, 5, 4, 0,
           WERROR="no", WERROR="yes")

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_PROG_LIBTOOL
AM_CONFIG_HEADER(config.h)

dnl check for tools
AC_PROG_CC
AC_PROG_CC_STDC

dnl decide on error flags
AS_COMPILER_FLAG(-Wall, WALL="yes", WALL="no")

if test "x$WALL" = "xyes"; then
  ERROR_CFLAGS="-Wall"

  if test "x$WERROR" = "xyes"; then
    AS_COMPILER_FLAG(-Werror,ERROR_CFLAGS="$ERROR_CFLAGS -Werror",ERROR_CFLAGS="$ERROR_CFLAGS")
  fi
fi

AC_SUBST(ERROR_CFLAGS)

AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--disable-debug],[omit debug code]),
    enable_debug=$enableval, enable_debug=yes )

if test x$enable_debug = xyes; then
  AC_DEFINE(ENABLE_DEBUG, [], [Enable debug code])
fi

AC_C_INLINE

AC_ARG_ENABLE(syslog,
  AC_HELP_STRING([--enable-syslog],[use syslog for Sofia log messages]),
  [enable_syslog=$enableval],
  [enable_syslog=no]
)
if test x$enable_syslog = xyes; then
  AC_DEFINE(ENABLE_SYSLOG, [], [Enable syslog handler for Sofia log messages])
fi

AC_ARG_ENABLE(coverage,
  AC_HELP_STRING([--enable-coverage],[compile with coverage info]),
  [enable_coverage=$enableval],
  [enable_coverage=no]
)
if test x$enable_coverage = xyes; then
  COVERAGE_CFLAGS="-g -fprofile-arcs -ftest-coverage"
fi
AC_SUBST(COVERAGE_CFLAGS)

# Hildon library dependsncies
PKG_CHECK_MODULES(HILDON, hildon-1 hildon-fm-2 hildon-help conbtdialogs)
AC_SUBST(HILDON_LIBS)
AC_SUBST(HILDON_CFLAGS)

# OSSO application framework dependencies
PKG_CHECK_MODULES(OSSO, osso-af-settings >= 0.8.5 libosso >= 0.9.17 gnome-vfs-2.0 >= 2.8.4.11 gnome-vfs-module-2.0 >= 2.8.4.11)
AC_SUBST(OSSO_LIBS)
AC_SUBST(OSSO_CFLAGS)


dnl GTK docs
#GTK_DOC_CHECK

dnl Check for Glib 
PKG_CHECK_MODULES(GLIB, gobject-2.0 >= 2.4, have_glib=yes, have_glib=no)

if test x$have_glib = xno ; then
    AC_MSG_ERROR([GLib development libraries not found])
fi

GLIB_GENMARSHAL=`$PKG_CONFIG --variable=glib_genmarshal glib-2.0`
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_GENMARSHAL)

dnl Check for DBus
PKG_CHECK_MODULES(DBUS, [dbus-1 >= 0.60, dbus-glib-1 >= 0.60], have_dbus=yes, have_dbus=no)

AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

AS_AC_EXPAND(DATADIR, $datadir)
DBUS_SERVICES_DIR="$DATADIR/dbus-1/services"
AC_SUBST(DBUS_SERVICES_DIR)
AC_DEFINE_UNQUOTED(DBUS_SERVICES_DIR, "$DBUS_SERVICES_DIR", [DBus services directory])

dnl Check for sofia
PKG_CHECK_MODULES(SOFIA_SIP_UA, sofia-sip-ua-glib >= 1.12.7)
AC_SUBST(SOFIA_SIP_UA_LIBS)
AC_SUBST(SOFIA_SIP_UA_CFLAGS)
AC_SUBST(SOFIA_SIP_UA_VERSION)

dnl Check for telepathy-glib
PKG_CHECK_MODULES(TELEPATHY_GLIB, [telepathy-glib >= 0.6.1])
AC_SUBST(TELEPATHY_GLIB_CFLAGS)
AC_SUBST(TELEPATHY_GLIB_LIBS)

# To make application visible in maemo Task Navigator it needs a Desktop
# file for the application.
# D-BUS service file is needed to be able to launch the maemo application
# and connect it to D-BUS services.
# The following line defines install directories for these files.
desktopentrydir=`$PKG_CONFIG osso-af-settings --variable=desktopentrydir`
serviceentrydir=`$PKG_CONFIG osso-af-settings --variable=dbusservicedir`

# Application locale install directory
localedir=`$PKG_CONFIG osso-af-settings --variable=localedir`

# Application pixmaps install directory
pixmapdir=`$PKG_CONFIG osso-af-settings --variable=hildonpixmapdir`

# Application icon install directories
icon_26x26dir=$datadir/icons/hicolor/26x26/hildon
icon_34x34dir=$datadir/icons/hicolor/34x34/hildon
icon_40x40dir=$datadir/icons/hicolor/40x40/hildon
icon_50x50dir=$datadir/icons/hicolor/50x50/hildon
icon_scalabledir=$datadir/icons/hicolor/scalable/hildon

# Help file install directory
helpfile_engbdir=$datadir/hildon-help/en_GB

# Hildon control panel plugin install directories
pluginlibdir=`$PKG_CONFIG hildon-control-panel --variable=plugindir`
plugindesktopentrydir=`$PKG_CONFIG hildon-control-panel --variable=plugindesktopentrydir`

# Define as variables in Makefiles
AC_SUBST(desktopentrydir)
AC_SUBST(serviceentrydir)
AC_SUBST(localedir)
AC_SUBST(pixmapdir)
AC_SUBST(icon_26x26dir)
AC_SUBST(icon_34x34dir)
AC_SUBST(icon_40x40dir)
AC_SUBST(icon_50x50dir)
AC_SUBST(icon_scalabledir)
AC_SUBST(pluginlibdir)
AC_SUBST(plugindesktopentrydir)
AC_SUBST(helpfile_engbdir)

AC_OUTPUT( Makefile \
data/Makefile \
src/Makefile
)
