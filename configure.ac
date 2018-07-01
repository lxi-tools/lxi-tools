AC_PREREQ([2.68])
AC_INIT([lxi-tools], [1.21], [], [lxi-tools], [https://lxi-tools.github.io])
AC_CONFIG_HEADERS([src/include/config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz no-dist-gzip subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])
AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
LT_INIT
AC_LANG([C])
PKG_PROG_PKG_CONFIG

AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR(libreadline not found)])
AC_CHECK_LIB([lxi], [lxi_connect], [], [AC_MSG_ERROR(liblxi not found)])

# Check for Lua 5.1 or newer
lua_found=0
PKG_CHECK_MODULES([lua], [lua5.3],[lua_found=1],[x=0])
PKG_CHECK_MODULES([lua], [lua5.2],[lua_found=1],[x=0])
PKG_CHECK_MODULES([lua], [lua5.1],[lua_found=1],[x=0])
PKG_CHECK_MODULES([lua], [lua >= 5.1],[lua_found=1],[x=0])
if test $lua_found == 0
then
    AC_MSG_ERROR([lua not found])
fi

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

# Handle GUI option
AC_ARG_ENABLE([lxi-gui], AS_HELP_STRING([--enable-lxi-gui], [Enable lxi-gui]))
AS_IF([test "x$enable_lxi_gui" = "xyes"], [

    if ! `$PKG_CONFIG Qt5Core`; then
        AC_MSG_ERROR([Qt5Core is required.])
    fi

    if ! `$PKG_CONFIG Qt5Gui`; then
        AC_MSG_ERROR([Qt5Gui is required.])
    fi

    if ! `$PKG_CONFIG Qt5Widgets`; then
        AC_MSG_ERROR([Qt5Widgets is required.])
    fi

    if ! `$PKG_CONFIG Qt5Charts`; then
        AC_MSG_ERROR([Qt5Charts is required.])
    fi

    AC_CHECK_PROGS(MOC, [moc-qt5 moc])
    AC_CHECK_PROGS(UIC, [uic-qt5 uic])
    AC_CHECK_PROGS(RCC, [rcc])
    AC_CHECK_PROGS(QMAKE, [qmake-qt5 qmake])
    AC_CHECK_PROGS(QTCHOOSER, [qtchooser])
    if test -z "$QMAKE" || test -z "$MOC" || test -z "$UIC" || test -z "$RCC"; then
        AC_MSG_ERROR([Qt utility programs qmake, qtchooser, moc, uic, and rcc not found])
    fi

    AC_ARG_VAR([QMAKE_ARGUMENTS],[arguments to pass on to Qt qmake])

    lxi_gui=yes
])
AM_CONDITIONAL([LXI_GUI], [test "$lxi_gui" = yes])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/gui/lxi-gui/Makefile])
AC_CONFIG_FILES([man/Makefile])
AC_OUTPUT
