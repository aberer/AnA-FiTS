#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([AnA-FiTS], [1.0.2], [andre.aberer-AT-h-its.org])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE(  [1.9 foreign ] )
# AM_SILENT_RULES([yes])

AC_LANG_PUSH([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET


# Checks for libraries.

AX_BOOST_BASE( [1.46], [], [ AC_MSG_ERROR(Please install the boost library version 1.46 or higher. ) ])
AX_BOOST_PROGRAM_OPTIONS
dnl ([1.46], [], [AC_MSG_ERROR(Please make sure your boost installation also includes the program_options package.) ]) 

AX_EXT

dnl AC_ARG_ENABLE(simd,
dnl [ --disable-simd  disable usage of streaming simd extensions   ],
dnl   [case "${enableval}" in
dnl   yes | no ) TRY_SIMD="${enableval}" ;;
dnl      *) AC_MSG_ERROR(bad value ${enableval} for --disable-simd) ;;
dnl    esac],
dnl [TRY_SIMD="yes"]
dnl )
dnl AS_IF( [test "x$TRY_SIMD" = "xyes"], AX_EXT )



AC_ARG_ENABLE(productive,
  [ --disable-productive    developmental version of the code ],
  [case "${enableval}" in
     yes | no ) ARE_PRODUCTIVE="${enableval}" ;;
     *) AC_MSG_ERROR(bad value ${enableval} for --disable-productive) ;;
   esac],
  [ARE_PRODUCTIVE="yes"]
)
AM_CONDITIONAL([ARE_PRODUCTIVE], [test  "x$ARE_PRODUCTIVE" = "xyes"])

AC_DEFINE([ARE_PRODUCTIVE], [0], ["is 1, if in productive mode "])
AS_IF([test "x$ARE_PRODUCTIVE" = "xyes"], AC_DEFINE(ARE_PRODUCTIVE, 1) )
AM_CONDITIONAL([ARE_PRODUCTIVE], [test  "x$ARE_PRODUCTIVE" = "xyes"])


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime floor getpagesize gettimeofday memmove memset pow sqrt strtoull])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
