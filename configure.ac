dnl Autoconf settings for npapi-vlc

AC_COPYRIGHT([Copyright 2002-2010 the VideoLAN team])

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
COPYRIGHT_YEARS="1996-2010"
AC_SUBST(COPYRIGHT_YEARS)
COPYRIGHT_MESSAGE="Copyright © ${COPYRIGHT_YEARS} the VideoLAN team"
AC_SUBST(COPYRIGHT_MESSAGE)

AC_CONFIG_SRCDIR(src/vlcplugin.cpp)
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

    CFLAGS_save="${CFLAGS_save} -D_INTL_REDIRECT_MACROS ${ARCH_flag}"; CFLAGS="${CFLAGS_save}"
    CXXFLAGS_save="${CXXFLAGS_save} -D_INTL_REDIRECT_MACROS ${ARCH_flag}"; CXXFLAGS="${CXXFLAGS_save}"
    CPPFLAGS_save="${CPPFLAGS_save} ${ARCH_flag}"; CPPFLAGS="${CPPFLAGS_save}"
    LDFLAGS_save="${LDFLAGS_save} -Wl,-headerpad_max_install_names ${ARCH_flag}"; LDFLAGS="${LDFLAGS_save}"
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

dnl
dnl Buggy glibc prevention. Purposedly not cached.
dnl See sourceware.org bugs 5058 and 5443.
dnl Ubuntu alone has 20 bug numbers for this...
dnl
AC_MSG_CHECKING(for buggy GNU/libc versions)
AC_PREPROC_IFELSE([
#include <limits.h>
#if defined (__GLIBC__) && (__GLIBC__ == 2) \
  && (__GLIBC_MINOR__ >= 5) && (__GLIBC_MINOR__ <= 7)
# error GNU/libc with dcgettext killer bug!
#endif
], [
  AC_MSG_RESULT([not present])
], [
  AC_MSG_RESULT([found])
  AC_MSG_ERROR([Buggy GNU/libc (version 2.5 - 2.7) present. VLC would crash; there is no viable
work-around for this. Check with your distribution vendor on how to update the
glibc run-time.])
])

AC_PATH_XTRA
PKG_CHECK_MODULES([LIBVLC], [libvlc >= 1.1.0])


AC_ARG_WITH(mozilla-sdk-path,
  [  --with-mozilla-sdk-path=PATH path to mozilla sdk])
AC_ARG_WITH(mozilla-pkg,
  [  --with-mozilla-pkg=PKG  look for PKG.pc to build the mozilla plugin.])
AC_ARG_VAR([MOZILLA_CFLAGS], [C compiler flags for Mozilla, overriding pkg-config])
AC_ARG_VAR([MOZILLA_LIBS], [linker flags for Mozilla, overriding pkg-config])

AC_LANG_PUSH(C++)
dnl pkg-config
dnl As we want to do a loop due to the number of name possible for the .pc
dnl we can't use the pkg-config macros.

AC_MSG_NOTICE([Checking for Mozilla])
AS_IF([test "${with_mozilla_sdk_path}" = "" -o "${with_mozilla_sdk_path}" = "no"], [
  found=0
  if test -n "$MOZILLA_CFLAGS" -o -n "$MOZILLA_LIBS"
  then
    found=1
  else
    if test -n "$PKG_CONFIG"; then
      for i in "${with_mozilla_pkg}" libxul {seamonkey,iceape,xulrunner,firefox,iceweasel,mozilla}-plugin; do
        if test -n "$i"; then
          echo "Trying to find $i package" >&AS_MESSAGE_LOG_FD
          AC_MSG_CHECKING([for $i >= 1.9.2])
          if $PKG_CONFIG --exists --print-errors "$i >= 1.9.2" 2>&AS_MESSAGE_LOG_FD
          then
            echo "Using $i pkg-config package." >&AS_MESSAGE_LOG_FD
             echo "Using $i package." >&AS_MESSAGE_FD
            found=1
            MOZILLA_CFLAGS=$(eval $PKG_CONFIG --cflags "$i" )
            MOZILLA_LIBS=$(eval $PKG_CONFIG --libs "$i" )
            AC_MSG_RESULT([yes])
            break
          fi
          AC_MSG_RESULT([no])
        fi
      done
    fi
  fi
  AS_IF( [test $found = 1],[
    CPPFLAGS="${CPPFLAGS_save} ${MOZILLA_CFLAGS}"
    MOZILLA_REQUIRED_HEADERS=1
    AC_CHECK_HEADERS(npapi.h,,MOZILLA_REQUIRED_HEADERS=0)
    AC_CHECK_HEADERS(npruntime.h,,MOZILLA_REQUIRED_HEADERS=0,
      [#if HAVE_NPAPI_H
       # include <npapi.h>
       #endif
      ])
    if test "${MOZILLA_REQUIRED_HEADERS}" = "0"; then
      AC_MSG_ERROR([Please install the Firefox development tools; plugin/npapi.h and/or plugin/npruntime.h were not found.])
    fi
    MOZILLA_REQUIRED_HEADERS=
    mozilla=:
    AS_IF([ test "${SYS}" != "mingw32" -a "${SYS}" != "darwin"],[
      PKG_CHECK_MODULES(XPM, [xpm xt],, [
        AC_MSG_ERROR([Please install the libXpm and libXt development files.])
      ])
    ])
  ], [
    AC_MSG_ERROR([Please install the Mozilla development tools (version 1.9.2 or higher).])
  ])
  dnl End of moz_sdk = ""
], [
  dnl special case for mingw32
  AS_IF([test "${SYS}" = "mingw32"], [
    AC_CHECK_TOOL(CYGPATH, cygpath, "")
    dnl latest gecko sdk does not have an xpcom directory
    AS_IF([test -d "${with_mozilla_sdk_path}/xpcom"], [
      mozilla_sdk_xpcom="/xpcom"
    ])
  ])

  real_mozilla_sdk="`cd ${with_mozilla_sdk_path} 2>/dev/null && pwd`"
  CPPFLAGS="${CPPFLAGS_save} -I${real_mozilla_sdk}/include"
  MOZILLA_REQUIRED_HEADERS=1
  AC_CHECK_HEADERS(npapi.h,,MOZILLA_REQUIRED_HEADERS=0)
  AC_CHECK_HEADERS(npruntime.h,,MOZILLA_REQUIRED_HEADERS=0,
      [#if HAVE_NPAPI_H
        #include <npapi.h>
        #endif
      ])
  AS_IF([test "${MOZILLA_REQUIRED_HEADERS}" = "0"], [
    AC_MSG_ERROR([Please install the Mozilla development tools, required headers were not found.])
  ])
])
AC_LANG_POP(C++)

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  share/Makefile
  src/install.js
  src/install.rdf
  src/manifest.json
])
AM_COND_IF([HAVE_WIN32], [
  AC_CONFIG_FILES([
    src/npvlc_rc.rc
  ])
])
AM_COND_IF([HAVE_DARWIN], [
  AC_CONFIG_FILES([
    src/vlc.r
    extras/macosx/Info.plist
    extras/macosx/InstallerInfo.plist
  ])
])
AC_OUTPUT
