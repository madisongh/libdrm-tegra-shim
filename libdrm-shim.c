/*
 * libdrm-shim.c
 *
 * Shim library for runtime loading of the Tegra-specific libdrm.
 *
 * Copyright (c) 2018, Matthew Madison
 * Distributed under license; see the LICENSE file for details.
 */
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include "xf86drm.h"
#include "xf86drmMode.h"
#include "config.h"

#undef FUNCDEF
#define FUNCDEFS \
    FUNCDEF(void*, drmGetHashTable, (void), (), return 0) \
    FUNCDEF(drmHashEntry*, drmGetEntry, (int fd), (fd), return 0) \
    FUNCDEF(int, drmAvailable, (void), (), return 0) \
    FUNCDEF(int, drmOpen, (const char *name, const char *busid), (name, busid), return -EINVAL) \
    FUNCDEF(int, drmClose, (int fd), (fd), return 0) \
    FUNCDEF(int, drmGetCap, (int fd, uint64_t capability, uint64_t *value), (fd, capability, value), return 0) \
    FUNCDEF(int, drmSetClientCap, (int fd, uint64_t capability, uint64_t value), (fd, capability, value), return 0) \
    FUNCDEF(int, drmSetMaster, (int fd), (fd), return 0) \
    FUNCDEF(int, drmDropMaster, (int fd), (fd), return 0) \
    FUNCDEF(int, drmGetMagic, (int fd, drm_magic_t *magic), (fd, magic), return 0) \
    FUNCDEF(int, drmAuthMagic, (int fd, drm_magic_t magic), (fd, magic), return 0) \
    FUNCDEF(drmVersionPtr, drmGetVersion, (int fd), (fd), return 0) \
    FUNCDEF(void, drmFreeVersion, (drmVersionPtr version), (version), return) \
    FUNCDEF(int, drmHandleEvent, (int fd, drmEventContextPtr evctx), (fd, evctx), return 0) \
    FUNCDEF(int, drmIoctl, (int fd, unsigned long request, void *arg), (fd, request, arg), return 0) \
    FUNCDEF(int, drmWaitVBlank, (int fd, drmVBlankPtr vbl), (fd, vbl), return 0) \
    FUNCDEF(void, drmSetServerInfo, (drmServerInfoPtr info), (info), return) \
    FUNCDEF(int, drmPrimeFDToHandle, (int fd, int prime_fd, uint32_t *handle), (fd, prime_fd, handle), return 0) \
    FUNCDEF(int, drmPrimeHandleToFD, (int fd, uint32_t handle, uint32_t flags, int *prime_fd), (fd, handle, flags, prime_fd), return 0) \
    FUNCDEF(int, drmSetInterfaceVersion, (int fd, drmSetVersion *version), (fd, version), return 0) \
    FUNCDEF(char*, drmGetBusid, (int fd), (fd), return 0) \
    FUNCDEF(int, drmSetBusid, (int fd, const char *busid), (fd, busid), return 0) \
    FUNCDEF(void, drmFreeBusid, (const char *busid), (busid), return) \
    FUNCDEF(int, drmAddMap, (int fd, drm_handle_t offset, drmSize size, drmMapType type, drmMapFlags flags, drm_handle_t * handle), (fd, offset, size, type, flags,  handle), return 0) \
    FUNCDEF(int, drmRmMap, (int fd, drm_handle_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmMap, (int fd, drm_handle_t handle, drmSize size, drmAddressPtr address), (fd, handle, size, address), return 0) \
    FUNCDEF(int, drmUnmap, (drmAddress address, drmSize size), (address, size), return 0) \
    FUNCDEF(int, drmGetLock, (int fd, drm_context_t context, drmLockFlags flags), (fd, context, flags), return 0) \
    FUNCDEF(int, drmUnlock, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(char*, drmGetDeviceNameFromFd, (int fd), (fd), return 0) \
    FUNCDEF(void, drmFree, (void *p), (p), return) \
    FUNCDEF(int, drmHashFirst, (void *t, unsigned long *key, void **value), (t, key, value), return 0) \
    FUNCDEF(int, drmHashNext, (void *t, unsigned long *key, void **value), (t, key, value), return 0) \
    FUNCDEF(int, drmCommandWriteRead, (int fd, unsigned long drmCommandIndex, void *data, unsigned long size), (fd, drmCommandIndex, data, size), return 0) \
    FUNCDEF(int, drmCreateContext, (int fd, drm_context_t * handle), (fd,  handle), return 0) \
    FUNCDEF(int, drmSetContextFlags, (int fd, drm_context_t context, drm_context_tFlags flags), (fd, context, flags), return 0) \
    FUNCDEF(int, drmAddContextTag, (int fd, drm_context_t context, void *tag), (fd, context, tag), return 0) \
    FUNCDEF(int, drmDelContextTag, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(void*, drmGetContextTag, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(drm_context_t*, drmGetReservedContextList, (int fd, int *count), (fd, count), return 0) \
    FUNCDEF(void, drmFreeReservedContextList, (drm_context_t *handle), (handle), return) \
    FUNCDEF(int, drmDestroyContext, (int fd, drm_context_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmCreateDrawable, (int fd, drm_drawable_t * handle), (fd,  handle), return 0) \
    FUNCDEF(int, drmDestroyDrawable, (int fd, drm_drawable_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmUpdateDrawableInfo, (int fd, drm_drawable_t handle, drm_drawable_info_type_t type, unsigned int num, void *data), (fd, handle, type, num, data), return 0) \
    FUNCDEF(int, drmModeAddFB, (int fd, uint32_t width, uint32_t height, uint8_t depth, uint8_t bpp, uint32_t pitch, uint32_t bo_handle, uint32_t *buf_id), (fd, width, height, depth, bpp, pitch, bo_handle, buf_id), return 0) \
    FUNCDEF(int, drmModeAddFB2, (int fd, uint32_t width, uint32_t height, uint32_t pixel_format, uint32_t bo_handles[4], uint32_t pitches[4], uint32_t offsets[4], uint32_t *buf_id, uint32_t flags), (fd, width, height, pixel_format, bo_handles, pitches, offsets, buf_id, flags), return 0) \
    FUNCDEF(int, drmModeAddFB2WithModifiers, (int fd, uint32_t width, uint32_t height, uint32_t pixel_format, const uint32_t bo_handles[4], const uint32_t pitches[4], const uint32_t offsets[4], const uint64_t modifier[4], uint32_t *buf_id, uint32_t flags), (fd, width, height, pixel_format, bo_handles, pitches, offsets, modifier, buf_id, flags), return 0) \
    FUNCDEF(int, drmModeRmFB, (int fd, uint32_t fb_id), (fd, fb_id), return 0) \
    FUNCDEF(drmModeConnectorPtr, drmModeGetConnector, (int fd, uint32_t connector_id), (fd, connector_id), return 0) \
    FUNCDEF(void, drmModeFreeConnector, (drmModeConnectorPtr ptr), (ptr), return) \
    FUNCDEF(int, drmModeConnectorSetProperty, (int fd, uint32_t connector_id, uint32_t property_id, uint64_t value), (fd, connector_id, property_id, value), return 0) \
    FUNCDEF(drmModeCrtcPtr, drmModeGetCrtc, (int fd, uint32_t crtc_id), (fd, crtc_id), return 0) \
    FUNCDEF(void, drmModeFreeCrtc, (drmModeCrtcPtr ptr), (ptr), return) \
    FUNCDEF(int, drmModeSetCrtc, (int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t *connectors, int count, drmModeModeInfoPtr drm_mode), (fd, crtc_id, fb_id, x, y, connectors, count, drm_mode), return 0) \
    FUNCDEF(int, drmModeCrtcSetGamma, (int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue), (fd, crtc_id, size, red, green, blue), return 0) \
    FUNCDEF(int, drmModeCrtcGetGamma, (int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue), (fd, crtc_id, size, red, green, blue), return 0) \
    FUNCDEF(int, drmModePageFlip, (int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data), (fd, crtc_id, fb_id, flags, user_data), return 0) \
    FUNCDEF(int, drmModeSetCursor, (int fd, uint32_t crtc_id, uint32_t bo_handle, uint32_t width, uint32_t height), (fd, crtc_id, bo_handle, width, height), return 0) \
    FUNCDEF(int, drmModeSetCursor2, (int fd, uint32_t crtcId, uint32_t bo_handle, uint32_t width, uint32_t height, int32_t hot_x, int32_t hot_y), (fd, crtcId, bo_handle, width, height, hot_x, hot_y), return 0) \
    FUNCDEF(int, drmModeMoveCursor, (int fd, uint32_t crtc_id, int x, int y), (fd, crtc_id, x, y), return 0) \
    FUNCDEF(drmModeEncoderPtr, drmModeGetEncoder, (int fd, uint32_t encoder_id), (fd, encoder_id), return 0) \
    FUNCDEF(void, drmModeFreeEncoder, (drmModeEncoderPtr ptr), (ptr), return) \
    FUNCDEF(drmModePlanePtr, drmModeGetPlane, (int fd, uint32_t plane_id), (fd, plane_id), return 0) \
    FUNCDEF(void, drmModeFreePlane, (drmModePlanePtr ptr), (ptr), return) \
    FUNCDEF(int, drmModeSetPlane, (int fd, uint32_t plane_id, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, uint32_t crtc_x, uint32_t crtc_y, uint32_t crtc_w, uint32_t crtc_h, uint32_t src_x, uint32_t src_y, uint32_t src_w, uint32_t src_h), (fd, plane_id, crtc_id, fb_id, flags, crtc_x, crtc_y, crtc_w, crtc_h, src_x, src_y, src_w, src_h), return 0) \
    FUNCDEF(drmModeObjectPropertiesPtr, drmModeObjectGetProperties, (int fd, uint32_t object_id, uint32_t object_type), (fd, object_id, object_type), return 0) \
    FUNCDEF(void, drmModeFreeObjectProperties, (drmModeObjectPropertiesPtr ptr), (ptr), return) \
    FUNCDEF(int, drmModeObjectSetProperty, (int fd, uint32_t object_id, uint32_t object_type, uint32_t property_id, uint64_t value), (fd, object_id, object_type, property_id, value), return 0) \
    FUNCDEF(drmModePropertyPtr, drmModeGetProperty, (int fd, uint32_t propertyId), (fd, propertyId), return 0) \
    FUNCDEF(void, drmModeFreeProperty, (drmModePropertyPtr ptr), (ptr), return) \
    FUNCDEF(drmModeResPtr, drmModeGetResources, (int fd), (fd), return 0) \
    FUNCDEF(void, drmModeFreeResources, (drmModeResPtr ptr), (ptr), return) \
    FUNCDEF(drmModePlaneResPtr, drmModeGetPlaneResources, (int fd), (fd), return 0) \
    FUNCDEF(void, drmModeFreePlaneResources, (drmModePlaneResPtr ptr), (ptr), return) \
    FUNCDEF(drmModeAtomicReqPtr, drmModeAtomicAlloc, (void), (), return 0) \
    FUNCDEF(int, drmModeAtomicAddProperty, (drmModeAtomicReqPtr req, uint32_t object_id, uint32_t property_id, uint64_t value), (req, object_id, property_id, value), return 0) \
    FUNCDEF(int, drmModeAtomicCommit, (int fd, drmModeAtomicReqPtr req, uint32_t flags, void *user_data), (fd, req, flags, user_data), return 0) \
    FUNCDEF(void, drmModeAtomicFree, (drmModeAtomicReqPtr req), (req), return) \
    FUNCDEF(int, drmModeCreatePropertyBlob, (int fd, const void *data, size_t size, uint32_t *id), (fd, data, size, id), return 0) \
    FUNCDEF(int, drmModeDestroyPropertyBlob, (int fd, uint32_t id), (fd, id), return 0) \
    FUNCDEF(drmModePropertyBlobPtr, drmModeGetPropertyBlob, (int fd, uint32_t blob_id), (fd, blob_id), return 0) \
    FUNCDEF(void, drmModeFreePropertyBlob, (drmModePropertyBlobPtr ptr), (ptr), return)

static const char *target_libname = TARGET_LIBPATH "/libdrm.so.2";
static void *dlptr;

#define FUNCDEF(type__, name__, args__, actargs__, ret__) \
  static type__ (*ptr_##name__) args__;
FUNCDEFS
#undef FUNCDEF

#define FUNCDEF(type__, name__, args__, actargs__, ret__) \
    ptr_##name__ = dlsym(dlptr, #name__);

void __attribute__((constructor))
shim_init (void)
{
    struct stat sbuf;

    if (stat("/dev/nvhost-nvdec", &sbuf))
        return;
    if ((sbuf.st_mode & S_IFMT) != S_IFCHR)
        return;
    dlptr = dlopen(target_libname, RTLD_NOW|RTLD_LOCAL);
    if (dlptr == NULL)
        return;
    FUNCDEFS
}
#undef FUNCDEF

void __attribute__((destructor))
shim_fini (void)
{
    if (dlptr != NULL) {
        dlclose(dlptr);
        dlptr = NULL;
    }
}


static
void *sym_lookup (const char *symname)
{
    if (dlptr)
        return dlsym(dlptr, symname);
    else
        return 0;
}

#define FUNCDEF(type__, name__, args__, actargs__, ret__) \
  type__ name__ args__ { \
    if (ptr_##name__) \
        return ptr_##name__ actargs__; \
    else \
        ret__; \
  }

FUNCDEFS
#undef FUNCDEF
