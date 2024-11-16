AC_INIT([eggpm], [0.3], [baguetteyeeter@icloud.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_AR
PKG_PROG_PKG_CONFIG
LT_INIT([disable-static])

AC_CHECK_HEADERS([getopt.h regex.h], [], [
  AC_MSG_ERROR([can't find essential header files])
])

PKG_CHECK_MODULES([LIBXML], [libxml-2.0], [], [
  AC_MSG_ERROR([can't find libxml2])
])

PKG_CHECK_MODULES([ARCHIVE], [libarchive], [], [
  AC_MSG_ERROR([can't find libarchive])
])

PKG_CHECK_MODULES([SQLITE3], [sqlite3], [], [
  AC_MSG_ERROR([can't find sqlite3 library])
])

PKG_CHECK_MODULES([LIBCURL], [libcurl], [], [
  AC_MSG_ERROR([can't find libcurl])
])

PKG_CHECK_MODULES([OPENSSL], [openssl], [], [
  AC_MSG_ERROR([can't find openssl])
])

AS_IF([test "$prefix" = "NONE"], [prefix=/usr/local], [])

# thank you openssl for this, and no there isn't a better way
A=`eval echo ${prefix}` ; A=`eval echo ${A}`
B=`eval echo ${localstatedir}` ; B=`eval echo ${B}`
C=`eval echo ${sysconfdir}` ; C=`eval echo ${C}`
D=`eval echo ${datarootdir}` ; D=`eval echo ${D}`

AC_DEFINE_UNQUOTED([INSTALL_PREFIX], ["$A"], [Define the installation prefix])
AC_DEFINE_UNQUOTED([VAR_PREFIX], ["$B"], [Define the localstatedir prefix])
AC_DEFINE_UNQUOTED([ETC_PREFIX], ["$C"], [Define the sysconfdir prefix])
AC_DEFINE_UNQUOTED([DATAROOTDIR_PREFIX], ["$D"], [Define the datarootdir prefix])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
