#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libpawdfox], [0.1], [sdepaula@gmail.com])

AM_INIT_AUTOMAKE
AC_OUTPUT(Makefile src/Makefile)

AC_LANG(C++)

AC_CONFIG_SRCDIR([src/libpawdfox.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([rapidjson/rapidjson.h],[rapidjson_found_int_headers=yes; break;])
AS_IF([test "x$rapidjson_found_int_headers" != "xyes"],
        [AC_MSG_ERROR([Unable to find rapidjson headers])])

AC_CHECK_HEADERS([nspr.h plarena.h],[nspr_found_int_headers=yes; break;])
AS_IF([test "x$nspr_found_int_headers" != "xyes"],
        [AC_MSG_ERROR([NSS reference NSPR direclty, add to CPPFLAGS as -I the nspr subdir inside its include. Example: CPPFLAGS=-I/usr/include/nspr/ ./configure])])



AC_CHECK_HEADERS([nss/pk11pub.h],[nss_found_int_headers=yes; break;])
AS_IF([test "x$nss_found_int_headers" != "xyes"],
        [AC_MSG_ERROR([Unable to find nss headers])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_SEARCH_LIBS(NSS_Init, nss3, [lnss_found_int_headers=yes; break;])
AS_IF([test "x$lnss_found_int_headers" != "xyes"],
        [AC_MSG_ERROR([Unable to find nss library])])

AC_OUTPUT
