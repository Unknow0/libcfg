#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libcfg], [1.0])
AC_CONFIG_SRCDIR([src/cfg.c])

LT_INIT
AM_INIT_AUTOMAKE([foreign])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB([container], [chunked_list_create], [], [
	AC_MSG_ERROR([install container: https://github.com/Unknow0/libcontainer])
	])
AC_CHECK_LIB([json-c], [json_object_new_object], [], [
	AC_MSG_ERROR([install json-c: https://github.com/json-c/json-c])
	])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([
	Makefile
	libcfg.pc
	])
AC_OUTPUT
