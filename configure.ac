dnl
dnl configure.ac - autoconf script for drm-shim
dnl
AC_INIT([drm-shim], [2.4.94])
AM_INIT_AUTOMAKE([subdir-objects foreign])
AM_SILENT_RULES([yes])
AC_COPYRIGHT([Copyright (c) 2018, Matthew Madison])
AC_CONFIG_MACRO_DIR([m4])

AC_PREREQ([2.69])
LT_PREREQ([2.4])
LT_INIT([])

AC_CANONICAL_HOST
AC_PROG_INSTALL

AC_ARG_WITH([target-library-path],
	    [AS_HELP_STRING([--with-target-library-path],
			    [specify path of directory containing the real Tegra-specific libdrm])],
	    [],
	    [with_target_library_path="${libdir}/tegra"])
AC_DEFINE_UNQUOTED([TARGET_LIBPATH], ["$with_target_library_path"], [Location of Tegra-specific libdrm])
pkgconfigdir="${libdir}/pkgconfig"
AC_SUBST(pkgconfigdir)

AC_CONFIG_FILES([Makefile libdrm.pc])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT
