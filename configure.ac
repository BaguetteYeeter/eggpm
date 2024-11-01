AC_INIT([eggpm], [0.1], [baguetteyeeter@icloud.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_AR
PKG_PROG_PKG_CONFIG
LT_INIT([disable-static])

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_HEADERS([regex.h])
AC_CHECK_HEADERS([libxml/parser.h libxml/tree.h])
AC_CHECK_HEADERS([archive.h])
AC_CHECK_HEADERS([openssl/evp.h])

AC_CHECK_LIB([archive], [archive_read_new], [], [
  AC_MSG_ERROR([can't find libarchive])
])

AC_CHECK_LIB([xml2], [xmlParseDoc], [], [
  AC_MSG_ERROR([can't find libxml2])
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

AC_DEFINE_UNQUOTED([INSTALL_PREFIX], ["$prefix"], [Define the installation prefix])
AC_DEFINE_UNQUOTED([VAR_PREFIX], ["$prefix/var"], [Define the localstatedir prefix])
AC_DEFINE_UNQUOTED([ETC_PREFIX], ["$prefix/etc"], [Define the sysconfdir prefix])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT