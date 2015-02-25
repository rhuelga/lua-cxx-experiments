AC_INIT([testlua], [0.1.0])
AC_CONFIG_AUX_DIR([.])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

PKG_PROG_PKG_CONFIG

LIBLUAJIT_REQUIRED=2.0.3
PKG_CHECK_MODULES([LIBLUAJIT], luajit >= $LIBLUAJIT_REQUIRED)


case "${target_os}" in
darwin*)
        LIBLUAJIT_LIBS="${LIBLUAJIT_LIBS} -pagezero_size 10000 -image_base 100000000"
        ;;
esac

AC_ARG_WITH([bandit],
        AS_HELP_STRING([--with-bandit=<path>], [Set the path to bandit testing framework.]))

AS_IF([test "x$with_bandit" != x],
      AC_SUBST([TEST_CFLAGS],[-I$with_bandit]))

AC_DEFUN([AC_KCHECK_HEADERS],
[
  oldcppflags="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $2"
  AC_CHECK_HEADERS($1, $3, $4)
  CPPFLAGS="$oldcppflags"
])# AC_KCHECK_HEADERS

AC_KCHECK_HEADERS([bandit/bandit.h], [$TEST_CFLAGS], [], AC_MSG_ERROR([bandit framework was not found use --with-bandit=<path>]))


AC_CHECK_PROG(GMOCK_CONFIG, gmock-config, "yes")
if test x"$GMOCK_CONFIG" = "xyes"; then
   GMOCK_CFLAGS=`gmock-config --cppflags --cxxflags`
   GMOCK_LIBS=`gmock-config --ldflags --libs`
else
   AC_MSG_ERROR([Needs gmock to test])
fi


AC_SUBST([GMOCK_CFLAGS])
AC_SUBST([GMOCK_LIBS])

AC_CONFIG_FILES(Makefile
                src/Makefile
                tests/Makefile)

AC_OUTPUT
