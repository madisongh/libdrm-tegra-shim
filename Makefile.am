# drm-shim makefile
#
# Copyright (c) 2018, Matthew Madison
#

AUTOMAKE_OPTIONS = subdir-objects foreign
ACLOCAL_AMFLAGS = -I m4
SUBDIRS =
DIST_SUBDIRS =

prefix = /usr
exec_prefix = $(prefix)
libdir = $(exec_prefix)/lib
pkgconfigdir=@pkgconfigdir@
includedir = $(exec_prefix)/include
klibdrmincludedir = ${includedir}/libdrm

include_HEADERS = xf86drm.h xf86drmMode.h
klibdrminclude_HEADERS = nouveau_drm.h tegra_drm.h
pkgconfig_DATA = libdrm.pc

lib_LTLIBRARIES = libdrm.la
libdrm_la_CFLAGS = -I=${includedir}/drm
libdrm_la_LDFLAGS = -version-number 2:4:0 -no-undefined -ldl
libdrm_la_SOURCES = libdrm-shim.c

