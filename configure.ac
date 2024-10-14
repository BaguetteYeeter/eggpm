AC_INIT([eggpm], [0.1], [baguetteyeeter@icloud.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_AR
PKG_PROG_PKG_CONFIG
LT_INIT([disable-static])

AC_CHECK_HEADERS([getopt.h])

PKG_CHECK_MODULES([SQLITE3], [sqlite3], [], [
  AC_MSG_ERROR([can't find sqlite3 library])
])

AC_DEFINE_UNQUOTED([INSTALL_PREFIX], ["$prefix"], [Define the installation prefix])
AC_DEFINE_UNQUOTED([VAR_PREFIX], ["$prefix/var"], [Define the localstatedir prefix])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT