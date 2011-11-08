###############################################################################
# Building the Mozilla plugin
###############################################################################

EXTRA_DIST = npvlc_rc.rc.in npvlc.dll.manifest
MOSTLYCLEANFILES =
CLEANFILES = $(BUILT_SOURCES)
npvlcdir = $(libdir)/mozilla/plugins

AM_CPPFLAGS = $(LIBVLC_CFLAGS)

libvlcplugin_la_SOURCES = \
	vlcshell.cpp \
	vlcshell.h \
	vlcplugin.cpp \
	vlcplugin.h \
	control/npolibvlc.cpp \
	control/npolibvlc.h \
	control/nporuntime.cpp \
	control/nporuntime.h \
	control/position.h \
	support/classinfo.h \
	$(SOURCES_support)

EXTRA_libvlcplugin_la_SOURCES = \
	support/npwin.cpp support/npmac.cpp support/npunix.cpp

libvlcplugin_la_DEPENDENCIES =
libvlcplugin_la_LIBADD = $(LIBVLC_LIBS)
libvlcplugin_la_LDFLAGS = \
	-module -avoid-version \
	-no-undefined

npvlc_la_SOURCES = $(libvlcplugin_la_SOURCES)
npvlc_la_DEPENDENCIES = $(libvlcplugin_la_DEPENDENCIES)
npvlc_la_LIBADD = $(libvlcplugin_la_LIBADD)
npvlc_la_LDFLAGS = $(libvlcplugin_la_LDFLAGS)

if !HAVE_DARWIN
if !HAVE_WIN32

# Unix (x11) platforms
#
AM_CPPFLAGS += -DXP_UNIX $(X_CFLAGS) \
	$(XPM_CFLAGS) \
	-DDATA_PATH=\"$(pkgdatadir)\"
libvlcplugin_la_LIBADD += $(X_LIBS) $(X_PRE_LIBS) \
	$(XT_LIBS) -lX11 -lSM -lICE -lXpm \
	$(XPM_LIBS) $(MOZILLA_LIBS)

npvlc_LTLIBRARIES = libvlcplugin.la

SOURCES_support = support/npunix.cpp

else # Win32

# Under Win32|Mac, Mozilla plugins need to be named NP******.DLL, but under Unix
# the common naming scheme is lib******plugin.so.
#
lib_LTLIBRARIES = npvlc.la

AM_CPPFLAGS += -DXP_WIN -DXP_WIN32 \
	-DXPCOM_GLUE -DHAVE_CPP_MODERN_SPECIALIZE_TEMPLATE_SYNTAX
SOURCES_support = support/npwin.cpp \
	../common/win32_fullscreen.cpp \
	../common/win32_fullscreen.h

npvlc_la_DEPENDENCIES += $(DATA_npvlc_rc)
npvlc_la_LDFLAGS += -Wl,--kill-at -Wl,$(DATA_npvlc_rc)
npvlc_la_LIBADD += -lgdi32

DATA_npvlc_rc = $(noinst_DATA)
noinst_DATA = npvlc_rc.$(OBJEXT)

npvlc_rc.$(OBJEXT): npvlc_rc.rc
	$(WINDRES) --include-dir $(srcdir) -i $< -o $@

endif
else

# MacOS X
# Mac OS plugins need .r files that will be processed into rsrc files
#
lib_LTLIBRARIES = npvlc.la

AM_CPPFLAGS += -I. -I$(top_builddir) -c \
	-F/System/Library/Frameworks/CoreFoundation.framework \
	-I/Developer/Headers/FlatCarbon -fno-common -fpascal-strings \
	-Wmost -Wno-four-char-constants -Wno-unknown-pragmas \
	-DXP_UNIX -DXP_MACOSX=1 \
	-DNO_X11=1 -DUSE_SYSTEM_CONSOLE=1 -pipe -fmessage-length=0

SOURCES_support = support/npmac.cpp
libvlcplugin_la_LDFLAGS += \
	-bundle -Wl,-read_only_relocs -Wl,suppress \
	-Wl,-headerpad_max_install_names \
	-Wl,-framework,Carbon -Wl,-framework,System \
	-shrext $(LIBEXT)

noinst_DATA = npvlc.rsrc VLC\ Plugin.plugin
MOSTLYCLEANFILES += npvlc.rsrc
CLEANFILES += VLC\ Plugin.plugin

npvlc.rsrc: vlc.r
	/Developer/Tools/Rez -useDF /Developer/Headers/FlatCarbon/Types.r $< -o $@

#
# Plugin uses shared libraries that are located relatively through @executable_path,
# which unfortunately references the path of the App using the Plugin, rather than the
# Plugin itself. Since this Plugin should always be installed in '/Library/Internet Plug-Ins',
# it is safer to force dylibs to locate dependents through a fixed path
#
define FIXEXECPATH
	otool -L "$$dylib" | \
	awk -v libdylib="$$dylib" ' \
	/@executable_path/ { \
	    newpath=$$1 ; \
	    sub("@executable_path","/Library/Internet Plug-Ins/VLC Plugin.plugin/Contents/MacOS",newpath) ; \
	    print "install_name_tool -change \""$$1"\" \""newpath"\" \""libdylib"\"" ; \
	}' | sh -x
endef

VLC\ Plugin.plugin: npvlc.rsrc $(lib_LTLIBRARIES)
	rm -Rf "$@"
	$(INSTALL) -d "VLC-Plugin.plugin/Contents/MacOS/lib"
	$(INSTALL) -d "VLC-Plugin.plugin/Contents/Resources"
	ACTION="release-makefile" PRODUCT="VLC-Plugin.plugin" src_dir=$(srcdir) build_dir=$(top_builddir) sh "$(top_srcdir)/project/macosx/framework/Pre-Compile.sh"
	$(INSTALL) npvlc.rsrc "VLC-Plugin.plugin/Contents/Resources/VLC Plugin.rsrc"
	$(INSTALL) "$(top_builddir)/extras/package/macosx/plugin/Info.plist" "VLC-Plugin.plugin/Contents/Info.plist"
	mv "VLC-Plugin.plugin" "VLC Plugin.plugin"
	find "VLC Plugin.plugin" -type d -exec chmod ugo+rx '{}' \;
	find "VLC Plugin.plugin" -type f -exec chmod ugo+r '{}' \;

# uncomment if dependencies on XPCOM libs is sought
#	if test -d "$(MOZILLA_SDK_PATH)/lib"; then \
#	  for i in "$(MOZILLA_SDK_PATH)"/lib/*.dylib ; do \
#	    dylib="$(srcdir)/$@/Contents/MacOS/`basename $${i}`" ; \
#	    $(INSTALL) -m 644 "$${i}" "$$dylib" ; \
#	    $(FIXEXECPATH); \
#	  done ; \
#	fi

endif # Mac
