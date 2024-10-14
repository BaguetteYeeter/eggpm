AC_INIT([eggpm], [0.1], [baguetteyeeter@icloud.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_AR
LT_INIT([disable-static])

AC_DEFINE_UNQUOTED([INSTALL_PREFIX], ["$prefix"], [Define the installation prefix])
AC_DEFINE_UNQUOTED([VAR_PREFIX], ["$prefix/var"], [Define the localstatedir prefix])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT