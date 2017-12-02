AC_PREREQ([2.68])
AC_INIT([lxi-tools], [1.14], [], [lxi-tools], [https://lxi-tools.github.io])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz no-dist-gzip subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])
AM_PROG_AR
AC_PROG_CC
AC_PROG_INSTALL
LT_INIT
AC_LANG([C])
AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR(libreadline not found)])
AC_CHECK_LIB([lxi], [lxi_connect], [], [AC_MSG_ERROR(liblxi not found)])

# Handle bash completion
AC_ARG_WITH([bash-completion-dir],
    AS_HELP_STRING([--with-bash-completion-dir[=PATH]],
                   [Install the bash auto-completion script in this directory. @<:@default=yes@:>@]),
                   [],
                   [with_bash_completion_dir=yes])

if test "x$with_bash_completion_dir" = "xyes"; then
    BASH_COMPLETION_DIR="${datadir}/bash-completion/completions"
else
    BASH_COMPLETION_DIR="$with_bash_completion_dir"
fi
AC_SUBST([BASH_COMPLETION_DIR])
AM_CONDITIONAL([ENABLE_BASH_COMPLETION],[test "x$with_bash_completion_dir" != "xno"])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([man/Makefile])
AC_OUTPUT
