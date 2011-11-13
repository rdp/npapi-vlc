dnl Autoconf settings for npapi-vlc

AC_COPYRIGHT([Copyright 2002-2011 the VideoLAN team])

AC_INIT(npapi-vlc, 1.2.0-git)
VERSION_MAJOR="1"
AC_SUBST(VERSION_MAJOR)
VERSION_MINOR="2"
AC_SUBST(VERSION_MINOR)
VERSION_REVISION="0"
AC_SUBST(VERSION_REVISION)
VERSION_EXTRA="-git"
AC_SUBST(VERSION_EXTRA)
dnl Win32 need s a numerical version_extra.
case $( echo ${VERSION_EXTRA}|wc -m ) in
       "1") VERSION_EXTRA_RC="0";;
       "2") VERSION_EXTRA_RC=$( echo ${VERSION_EXTRA}|tr "abcdefghi" "123456789") ;;
       *) if test ${VERSION_EXTRA} = "-git"; then VERSION_EXTRA_RC="666"; else VERSION_EXTRA_RC="99"; fi
esac
AC_SUBST(VERSION_EXTRA_RC)
COPYRIGHT_YEARS="2002-2010"
AC_SUBST(COPYRIGHT_YEARS)
COPYRIGHT_MESSAGE="Copyright © ${COPYRIGHT_YEARS} the VideoLAN team"
AC_SUBST(COPYRIGHT_MESSAGE)

AC_CONFIG_SRCDIR(npapi/vlcplugin.cpp)
AC_CONFIG_AUX_DIR(autotools)
AC_CONFIG_MACRO_DIR(m4)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_PRESERVE_HELP_ORDER

AM_INIT_AUTOMAKE(tar-ustar color-tests)
AM_CONFIG_HEADER(config.h)

# Disable with "./configure --disable-silent-rules" or "make V=1"
AM_SILENT_RULES([yes])

dnl
dnl Check for tools
dnl
AC_PROG_CC_C99
AC_USE_SYSTEM_EXTENSIONS
AC_DEFINE([_FORTIFY_SOURCE], 2, [Define to '2' to get glibc warnings.])
AC_PROG_CXX

dnl override platform specific check for dependent libraries
dnl otherwise libtool linking of shared libraries will
dnl fail on anything other than pass_all.
AC_CACHE_VAL(lt_cv_deplibs_check_method,
    [lt_cv_deplibs_check_method=pass_all])

AC_DISABLE_STATIC
AC_LIBTOOL_DLOPEN
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG
AC_CHECK_TOOL(DLLTOOL, dlltool, :)

dnl Check for compiler properties
AC_C_CONST

dnl
dnl  Check the operating system
dnl
case "${host_os}" in
  darwin*)
    SYS=darwin

    dnl Force gcc "-arch" flag
    ARCH_flag=""
    case "${host}" in
      i386*)
        ARCH_flag="-arch i386"
      ;;
      ppc64*)
        ARCH_flag="-arch ppc64"
      ;;
      ppc*)
        ARCH_flag="-arch ppc"
      ;;
      x86_64*)
        ARCH_flag="-arch x86_64"
      ;;
    esac

    CFLAGS="${CFLAGS} -D_INTL_REDIRECT_MACROS ${ARCH_flag}"
    CXXFLAGS="${CXXFLAGS} -D_INTL_REDIRECT_MACROS ${ARCH_flag}"
    CPPFLAGS="${CPPFLAGS} ${ARCH_flag}"
    LDFLAGS="${LDFLAGS} -Wl,-headerpad_max_install_names ${ARCH_flag}"
    ;;
  *mingw32*)
    SYS=mingw32
    AC_CHECK_TOOL(WINDRES, windres, :)
    AC_DEFINE([_WIN32_WINNT], 0x0501, [Define to '0x0501' for Windows XP APIs.])
    AC_DEFINE([_WIN32], 1, [Define to 1 if targetting Windows.])
    ;;
esac
AM_CONDITIONAL(HAVE_DARWIN, test "${SYS}" = "darwin")
AM_CONDITIONAL(HAVE_WIN32, test "${SYS}" = "mingw32")
AC_ARG_ENABLE(activex, AS_HELP_STRING(--disable-activex, Do not build ActiveX plugin))
AM_CONDITIONAL(BUILD_ACTIVEX, test "${SYS}" = "mingw32" -a "${enable_activex}" != "no")

dnl
dnl Buggy glibc prevention. Purposedly not cached.
dnl See sourceware.org bugs 5058 and 5443.
dnl Ubuntu alone has 20 bug numbers for this...
dnl
AC_MSG_CHECKING(for buggy GNU/libc versions)
AC_PREPROC_IFELSE([AC_LANG_SOURCE([
#include <limits.h>
#if defined (__GLIBC__) && (__GLIBC__ == 2) \
  && (__GLIBC_MINOR__ >= 5) && (__GLIBC_MINOR__ <= 7)
# error GNU/libc with dcgettext killer bug!
#endif
])], [
  AC_MSG_RESULT([not present])
], [
  AC_MSG_RESULT([found])
  AC_MSG_ERROR([Buggy GNU/libc (version 2.5 - 2.7) present. VLC would crash; there is no viable
work-around for this. Check with your distribution vendor on how to update the
glibc run-time.])
])

AC_PATH_XTRA
PKG_CHECK_MODULES([LIBVLC], [libvlc >= 1.1.0])

WIDL="no"
MIDL="no"
AS_IF([test "${SYS}" = "mingw32"],[
    AC_CHECK_PROGS(MIDL, [midl], no)

    dnl FIXME
    dnl AC_CHECK_PROGS(WIDL, [widl], no)
    dnl

    AC_LANG_PUSH(C++)
    AC_CHECK_HEADER(ole2.h,,[AC_MSG_ERROR([required OLE header ole2.h is missing from your system])])
    AC_CHECK_HEADER(olectl.h,,[AC_MSG_ERROR([required OLE header olectl.h is missing from your system])])
    AC_CHECK_HEADERS(objsafe.h,,,[#include <ole2.h>])
    AC_LANG_POP(C++)
])

AM_CONDITIONAL(HAS_MIDL_COMPILER, test "${MIDL}" != "no")
AM_CONDITIONAL(HAS_WIDL_COMPILER, test "${WIDL}" != "no")

AC_ARG_WITH(mozilla-pkg,
  [  --with-mozilla-pkg=PKG  look for PKG.pc to build the mozilla plugin.])
AC_ARG_VAR([MOZILLA_CFLAGS], [C compiler flags for Mozilla, overriding pkg-config])
AC_ARG_VAR([MOZILLA_LIBS], [linker flags for Mozilla, overriding pkg-config])


AC_LANG_PUSH(C++)
AC_MSG_CHECKING([Mozilla headers])

found=0
if test -z "$MOZILLA_CFLAGS" -a -z "$MOZILLA_LIBS" -a -n "$PKG_CONFIG"
then
dnl pkg-config
dnl As we want to do a loop due to the number of name possible for the .pc
dnl we can't use the pkg-config macros.
  for i in "${with_mozilla_pkg}" libxul {seamonkey,iceape,xulrunner,firefox,iceweasel,mozilla}-plugin; do
    if test -n "$i"; then
      echo "Trying to find $i package" >&AS_MESSAGE_LOG_FD
      if $PKG_CONFIG --exists --print-errors "$i >= 1.9.2" 2>&AS_MESSAGE_LOG_FD
      then
        echo "Using $i pkg-config package." >&AS_MESSAGE_LOG_FD
        MOZILLA_CFLAGS=$(eval $PKG_CONFIG --cflags "$i" )
        MOZILLA_LIBS=$(eval $PKG_CONFIG --libs "$i" )
        AC_MSG_RESULT([yes (${i})])
        found=1
        break
      fi
    fi
  done
fi
test "$found" = 0 && AC_MSG_RESULT([no])

CPPFLAGS="${CPPFLAGS} ${MOZILLA_CFLAGS}"

AC_CHECK_HEADERS(npapi.h)
AC_CHECK_HEADERS(npruntime.h, [
  dnl if found
  AS_IF([ test "${SYS}" != "mingw32" -a "${SYS}" != "darwin"],[
    PKG_CHECK_MODULES(XPM, [xpm xt],, [
      AC_MSG_ERROR([Please install the libXpm and libXt development files.])
    ])
  ])],[
  dnl if not found
  AC_MSG_ERROR([Please install the Mozilla development tools, required headers were not found.])
  ], [
  #if HAVE_NPAPI_H
   #include <npapi.h>
  #endif
  ]
)

AC_MSG_CHECKING([if NPP_GetMIMEDescription() returns const])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
   #ifdef WIN32
   # define XP_WIN 1
   #else
   # ifdef __APPLE__
   #  define XP_MACOSX 1
   # endif
   # define XP_UNIX 1
   #endif
   #include <npapi.h>
  const char *NPP_GetMIMEDescription(void);
])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(NPP_GET_MIME_CONST, [const], [Wether NPP_GetMIMEDescription returns const])
],[
  AC_MSG_RESULT(no)
  AC_DEFINE(NPP_GET_MIME_CONST, [], [Wether NPP_GetMIMEDescription returns const])
])

AC_LANG_POP(C++)


AM_COND_IF([HAVE_WIN32], [
  ACTIVEX_CPPFLAGS="${CPPFLAGS} -DUNICODE -D_UNICODE -D_MIDL_USE_GUIDDEF_"
  ACTIVEX_CXXFLAGS="${CXXFLAGS} -fno-exceptions"

  AC_ARG_VAR([ACTIVEX_LIBS], [linker flags for Activex])
  ACTIVEX_LIBS="${ACTIVEX_LIBS} -lole32 -loleaut32 -luuid -lshlwapi -lgdi32"
])

AC_CONFIG_FILES([
  Makefile
  share/Makefile
  npapi/Makefile
  npapi/install.js
  npapi/install.rdf
  npapi/manifest.json
])

AM_COND_IF([HAVE_WIN32], [
  AC_CONFIG_FILES([
    npapi/npvlc_rc.rc
    activex/Makefile
    activex/axvlc.inf
    activex/axvlc_rc.rc
  ])
])
AM_COND_IF([HAVE_DARWIN], [
  AC_CONFIG_FILES([
    npapi/vlc.r
    extras/macosx/Info.plist
    extras/macosx/InstallerInfo.plist
  ])
])
AC_OUTPUT
