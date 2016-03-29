AC_PREREQ([2.68])
AC_INIT([lxi-tools], [0.1], [], [lxi-tools], [http://github.com/lxi])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz no-dist-gzip -Wall -Werror])
AM_SILENT_RULES([yes])
AM_PROG_AR
AC_PROG_CC
AC_PROG_INSTALL
LT_INIT
AC_LANG([C])
AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR(libreadline not found)])
AC_CHECK_LIB([lxi], [lxi_connect], [], [AC_MSG_ERROR(liblxi not found)])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([man/Makefile])
AC_OUTPUT
