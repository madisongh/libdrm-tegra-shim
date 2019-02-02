libdrm-tegra-shim
=================

Shim for the Tegra-specific version of libdrm that brings
in that copy of the shared library at runtime.

In Yocto/OE builds, many graphics programs link (either directly
or indirectly) against libdrm, and that library is expected to
work in an emulated environment (for gobject-introspection, etc.).

However, the Tegra-specific libdrm links against other
Tegra-specific libraries and will cause a segfault when
used under QEMU.

This library checks the runtime environment during its
initialization, and only maps in the NVIDIA binary if
the `/dev/nvhost-nvdec` device is present. Otherwise,
the stub routines in ths shim return error or null values
for all functions.

Note that while the shim advertises its version as 2.4.96
in its pkgconfig file, it only implements function vectors
for the functions provided by NVIDIA in their stripped-down
libdrm.


License
-------
All sources are released under the MIT license.  See the
[LICENSE](https://github.com/madisongh/drm-shim/blob/master/LICENSE)
file for the license text.

The xf86drm.h, xf86drmMode.h, libsync.h, and nouveau_drm.h header files
are adapted or copied from [libdrm](https://dri.freedesktop.org/libdrm),
which is also MIT-licensed. See those header files for the original
license text.


Branching
---------
The master branch is used for ongoing development. The l4t-r28.2
branch shims the API provided in the Linux for Tegra R28.2.x releases.
