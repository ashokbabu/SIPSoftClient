#
# Makefile.am for sipclient/src
#
# ----------------------------------------------------------------------
# Automake options

#managerdir = $(datadir)/telepathy/managers
sipphonedir = $(datadir)/telepathy/

# ----------------------------------------------------------------------
# Headers and libraries

AM_CFLAGS = $(ERROR_CFLAGS) $(HILDON_CFLAGS) $(OSSO_CFLAGS) @DBUS_CFLAGS@ @GLIB_CFLAGS@ @SOFIA_SIP_UA_CFLAGS@ \
	    @TELEPATHY_GLIB_CFLAGS@ @COVERAGE_CFLAGS@ -I/usr/include/gstreamer-0.10/ -I/usr/include/libxml2/ 
AM_LDFLAGS = $(HILDON_LIBS) $(OSSO_LIBS) @DBUS_LIBS@ @GLIB_LIBS@ @SOFIA_SIP_UA_LIBS@ @TELEPATHY_GLIB_LIBS@ -ltelepathy -lgstreamer-0.10

# ----------------------------------------------------------------------
# Build targets

#libexec_PROGRAMS = sipphone
bin_PROGRAMS = sipphone
sipphone_DATA = \
		sipphone.config \
		hang.png \
		dial.png \
		marslogo.png \
		loggedoff.png \
		loggedin.png \
		redial.png \
		micMute.png \
		micOn.png \
	  dialtone.wav	
#noinst_LTLIBRARIES = libtpsip-convenience.la

# ----------------------------------------------------------------------
# Tests

# ----------------------------------------------------------------------
# Rules for building the targets

#BUILT_SOURCES = \
#		signals-marshal.h \
#		signals-marshal.c \
#		sip-connection-enumtypes.h \
#		sip-connection-enumtypes.c
sipphone_HEADERS = \
	tp_caller.h \
	phoneguiinterface.h
	
sipphone_SOURCES = \
	tp_caller.h \
	phoneguiinterface.h \
	tp_caller.c \
	phoneguiinterface.c \
	sipclient.c 

#sipphone_LDADD = libtpsip-convenience.la


# ----------------------------------------------------------------------
# Install and distribution rules

EXTRA_DIST = $(sipphone_DATA)
#EXTRA_DIST = signals-marshal.list

# Correctly clean the generated headers, but keep the xml description
#CLEANFILES = $(BUILT_SOURCES) $(manager_DATA)
