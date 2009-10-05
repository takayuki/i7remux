#
#  configure.ac -- this file is part of i7remux
#  Copyright (C) 2007 Takayuki Usui
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#
AC_PREREQ(2.59)
AC_INIT([i7remux], [0.1], [strscr@users.sourceforge.jp], [i7remux])
AC_CONFIG_SRCDIR([i7remux.cc])
AC_CONFIG_HEADER([config.h])
m4_include([m4/check_gnu_make.m4])
CHECK_GNU_MAKE
AC_PROG_CXX
AC_PROG_CC
AC_CHECK_PROG(AR, [ar], [ar])
AC_PROG_INSTALL
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/param.h unistd.h])
AC_CHECK_HEADERS([endian.h sys/endian.h])
AC_C_CONST
AC_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([memset])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
