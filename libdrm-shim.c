/*
 * libdrm-shim.c
 *
 * Shim library for runtime loading of the Tegra-specific libdrm.
 *
 * Copyright (c) 2018-2019, Matthew Madison
 * Distributed under license; see the LICENSE file for details.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
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
    FUNCDEF(int, drmIoctl, (int fd, unsigned long request, void *arg), (fd, request, arg), return 0) \
    FUNCDEF(void *, drmGetHashTable, (void), (), return 0) \
    FUNCDEF(drmHashEntry *, drmGetEntry, (int fd), (fd), return 0) \
    FUNCDEF(int, drmAvailable, (void), (), return 0) \
    FUNCDEF(int, drmOpen, (const char *name, const char *busid), (name, busid), return -EINVAL) \
    FUNCDEF(int, drmOpenWithType, (const char *name, const char *busid, int type), (name, busid, type), return 0) \
    FUNCDEF(int, drmOpenControl, (int minor), (minor), return 0) \
    FUNCDEF(int, drmOpenRender, (int minor), (minor), return 0) \
    FUNCDEF(int, drmClose, (int fd), (fd), return 0) \
    FUNCDEF(drmVersionPtr, drmGetVersion, (int fd), (fd), return 0) \
    FUNCDEF(drmVersionPtr, drmGetLibVersion, (int fd), (fd), return 0) \
    FUNCDEF(int, drmGetCap, (int fd, uint64_t capability, uint64_t *value), (fd, capability, value), return 0) \
    FUNCDEF(void, drmFreeVersion, (drmVersionPtr v), (v), return) \
    FUNCDEF(int, drmGetMagic, (int fd, drm_magic_t * magic), (fd,  magic), return 0) \
    FUNCDEF(char *, drmGetBusid, (int fd), (fd), return 0) \
    FUNCDEF(int, drmGetInterruptFromBusID, (int fd, int busnum, int devnum, int funcnum), (fd, busnum, devnum, funcnum), return 0) \
    FUNCDEF(int, drmGetMap, (int fd, int idx, drm_handle_t *offset, drmSize *size, drmMapType *type, drmMapFlags *flags, drm_handle_t *handle, int *mtrr), (fd, idx, offset, size, type, flags, handle, mtrr), return 0) \
    FUNCDEF(int, drmGetClient, (int fd, int idx, int *auth, int *pid, int *uid, unsigned long *magic, unsigned long *iocs), (fd, idx, auth, pid, uid, magic, iocs), return 0) \
    FUNCDEF(int, drmGetStats, (int fd, drmStatsT *stats), (fd, stats), return 0) \
    FUNCDEF(int, drmSetInterfaceVersion, (int fd, drmSetVersion *version), (fd, version), return 0) \
    FUNCDEF(int, drmCommandNone, (int fd, unsigned long drmCommandIndex), (fd, drmCommandIndex), return 0) \
    FUNCDEF(int, drmCommandRead, (int fd, unsigned long drmCommandIndex, void *data, unsigned long size), (fd, drmCommandIndex, data, size), return 0) \
    FUNCDEF(int, drmCommandWrite, (int fd, unsigned long drmCommandIndex, void *data, unsigned long size), (fd, drmCommandIndex, data, size), return 0) \
    FUNCDEF(int, drmCommandWriteRead, (int fd, unsigned long drmCommandIndex, void *data, unsigned long size), (fd, drmCommandIndex, data, size), return 0) \
    FUNCDEF(void, drmFreeBusid, (const char *busid), (busid), return) \
    FUNCDEF(int, drmSetBusid, (int fd, const char *busid), (fd, busid), return 0) \
    FUNCDEF(int, drmAuthMagic, (int fd, drm_magic_t magic), (fd, magic), return 0) \
    FUNCDEF(int, drmAddMap, (int fd, drm_handle_t offset, drmSize size, drmMapType type, drmMapFlags flags, drm_handle_t * handle), (fd, offset, size, type, flags,  handle), return 0) \
    FUNCDEF(int	, drmRmMap, (int fd, drm_handle_t handle), (fd, handle), return 0) \
    FUNCDEF(int	, drmAddContextPrivateMapping, (int fd, drm_context_t ctx_id, drm_handle_t handle), (fd, ctx_id, handle), return 0) \
    FUNCDEF(int, drmAddBufs, (int fd, int count, int size, drmBufDescFlags flags, int agp_offset), (fd, count, size, flags, agp_offset), return 0) \
    FUNCDEF(int, drmMarkBufs, (int fd, double low, double high), (fd, low, high), return 0) \
    FUNCDEF(int, drmCreateContext, (int fd, drm_context_t * handle), (fd,  handle), return 0) \
    FUNCDEF(int, drmSetContextFlags, (int fd, drm_context_t context, drm_context_tFlags flags), (fd, context, flags), return 0) \
    FUNCDEF(int, drmGetContextFlags, (int fd, drm_context_t context, drm_context_tFlagsPtr flags), (fd, context, flags), return 0) \
    FUNCDEF(int, drmAddContextTag, (int fd, drm_context_t context, void *tag), (fd, context, tag), return 0) \
    FUNCDEF(int, drmDelContextTag, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(void *, drmGetContextTag, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(void, drmFreeReservedContextList, (drm_context_t *c), (c), return) \
    FUNCDEF(int, drmSwitchToContext, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(int, drmDestroyContext, (int fd, drm_context_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmCreateDrawable, (int fd, drm_drawable_t * handle), (fd,  handle), return 0) \
    FUNCDEF(int, drmDestroyDrawable, (int fd, drm_drawable_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmCtlInstHandler, (int fd, int irq), (fd, irq), return 0) \
    FUNCDEF(int, drmCtlUninstHandler, (int fd), (fd), return 0) \
    FUNCDEF(int, drmSetClientCap, (int fd, uint64_t capability, uint64_t value), (fd, capability, value), return 0) \
    FUNCDEF(int, drmCrtcGetSequence, (int fd, uint32_t crtcId, uint64_t *sequence, uint64_t *ns), (fd, crtcId, sequence, ns), return 0) \
    FUNCDEF(int, drmCrtcQueueSequence, (int fd, uint32_t crtcId, uint32_t flags, uint64_t sequence, uint64_t *sequence_queued, uint64_t user_data), (fd, crtcId, flags, sequence, sequence_queued, user_data), return 0) \
    FUNCDEF(int, drmMap, (int fd, drm_handle_t handle, drmSize size, drmAddressPtr address), (fd, handle, size, address), return 0) \
    FUNCDEF(int, drmUnmap, (drmAddress address, drmSize size), (address, size), return 0) \
    FUNCDEF(drmBufInfoPtr, drmGetBufInfo, (int fd), (fd), return 0) \
    FUNCDEF(drmBufMapPtr, drmMapBufs, (int fd), (fd), return 0) \
    FUNCDEF(int, drmUnmapBufs, (drmBufMapPtr bufs), (bufs), return 0) \
    FUNCDEF(int, drmDMA, (int fd, drmDMAReqPtr request), (fd, request), return 0) \
    FUNCDEF(int, drmFreeBufs, (int fd, int count, int *list), (fd, count, list), return 0) \
    FUNCDEF(int, drmGetLock, (int fd, drm_context_t context, drmLockFlags flags), (fd, context, flags), return 0) \
    FUNCDEF(int, drmUnlock, (int fd, drm_context_t context), (fd, context), return 0) \
    FUNCDEF(int, drmFinish, (int fd, int context, drmLockFlags flags), (fd, context, flags), return 0) \
    FUNCDEF(int, drmAgpAcquire, (int fd), (fd), return 0) \
    FUNCDEF(int, drmAgpRelease, (int fd), (fd), return 0) \
    FUNCDEF(int, drmAgpEnable, (int fd, unsigned long mode), (fd, mode), return 0) \
    FUNCDEF(int, drmAgpAlloc, (int fd, unsigned long size, unsigned long type, unsigned long *address, drm_handle_t *handle), (fd, size, type, address, handle), return 0) \
    FUNCDEF(int, drmAgpFree, (int fd, drm_handle_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmAgpUnbind, (int fd, drm_handle_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmAgpVersionMajor, (int fd), (fd), return 0) \
    FUNCDEF(int, drmAgpVersionMinor, (int fd), (fd), return 0) \
    FUNCDEF(int, drmScatterGatherAlloc, (int fd, unsigned long size, drm_handle_t *handle), (fd, size, handle), return 0) \
    FUNCDEF(int, drmScatterGatherFree, (int fd, drm_handle_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmWaitVBlank, (int fd, drmVBlankPtr vbl), (fd, vbl), return 0) \
    FUNCDEF(void, drmSetServerInfo, (drmServerInfoPtr info), (info), return) \
    FUNCDEF(int, drmError, (int err, const char *label), (err, label), return 0) \
    FUNCDEF(void *, drmMalloc, (int size), (size), return 0) \
    FUNCDEF(void, drmFree, (void *pt), (pt), return) \
    FUNCDEF(void *, drmHashCreate, (void), (), return 0) \
    FUNCDEF(int, drmHashDestroy, (void *t), (t), return 0) \
    FUNCDEF(int, drmHashLookup, (void *t, unsigned long key, void **value), (t, key, value), return 0) \
    FUNCDEF(int, drmHashInsert, (void *t, unsigned long key, void *value), (t, key, value), return 0) \
    FUNCDEF(int, drmHashDelete, (void *t, unsigned long key), (t, key), return 0) \
    FUNCDEF(int, drmHashFirst, (void *t, unsigned long *key, void **value), (t, key, value), return 0) \
    FUNCDEF(int, drmHashNext, (void *t, unsigned long *key, void **value), (t, key, value), return 0) \
    FUNCDEF(void *, drmRandomCreate, (unsigned long seed), (seed), return 0) \
    FUNCDEF(int, drmRandomDestroy, (void *state), (state), return 0) \
    FUNCDEF(double, drmRandomDouble, (void *state), (state), return 0) \
    FUNCDEF(void *, drmSLCreate, (void), (), return 0) \
    FUNCDEF(int, drmSLDestroy, (void *l), (l), return 0) \
    FUNCDEF(int, drmSLLookup, (void *l, unsigned long key, void **value), (l, key, value), return 0) \
    FUNCDEF(int, drmSLInsert, (void *l, unsigned long key, void *value), (l, key, value), return 0) \
    FUNCDEF(int, drmSLDelete, (void *l, unsigned long key), (l, key), return 0) \
    FUNCDEF(int, drmSLNext, (void *l, unsigned long *key, void **value), (l, key, value), return 0) \
    FUNCDEF(int, drmSLFirst, (void *l, unsigned long *key, void **value), (l, key, value), return 0) \
    FUNCDEF(void, drmSLDump, (void *l), (l), return) \
    FUNCDEF(int, drmSLLookupNeighbors, (void *l, unsigned long key, unsigned long *prev_key, void **prev_value, unsigned long *next_key, void **next_value), (l, key, prev_key, prev_value, next_key, next_value), return 0) \
    FUNCDEF(int, drmOpenOnce, (void *unused, const char *BusID, int *newlyopened), (unused, BusID, newlyopened), return -1) \
    FUNCDEF(int, drmOpenOnceWithType, (const char *BusID, int *newlyopened, int type), (BusID, newlyopened, type), return 0) \
    FUNCDEF(void, drmCloseOnce, (int fd), (fd), return) \
    FUNCDEF(int, drmSetMaster, (int fd), (fd), return 0) \
    FUNCDEF(int, drmDropMaster, (int fd), (fd), return 0) \
    FUNCDEF(int, drmIsMaster, (int fd), (fd), return 0) \
    FUNCDEF(int, drmHandleEvent, (int fd, drmEventContextPtr evctx), (fd, evctx), return 0) \
    FUNCDEF(char *, drmGetDeviceNameFromFd, (int fd), (fd), return 0) \
    FUNCDEF(char *, drmGetDeviceNameFromFd2, (int fd), (fd), return 0) \
    FUNCDEF(int, drmGetNodeTypeFromFd, (int fd), (fd), return 0) \
    FUNCDEF(int, drmPrimeHandleToFD, (int fd, uint32_t handle, uint32_t flags, int *prime_fd), (fd, handle, flags, prime_fd), return 0) \
    FUNCDEF(int, drmPrimeFDToHandle, (int fd, int prime_fd, uint32_t *handle), (fd, prime_fd, handle), return 0) \
    FUNCDEF(char *, drmGetPrimaryDeviceNameFromFd, (int fd), (fd), return 0) \
    FUNCDEF(char *, drmGetRenderDeviceNameFromFd, (int fd), (fd), return 0) \
    FUNCDEF(int, drmGetDevice, (int fd, drmDevicePtr *device), (fd, device), return 0) \
    FUNCDEF(void, drmFreeDevice, (drmDevicePtr *device), (device), return) \
    FUNCDEF(int, drmGetDevices, (drmDevicePtr devices[], int max_devices), (devices, max_devices), return 0) \
    FUNCDEF(void, drmFreeDevices, (drmDevicePtr devices[], int count), (devices, count), return) \
    FUNCDEF(int, drmGetDevice2, (int fd, uint32_t flags, drmDevicePtr *device), (fd, flags, device), return -EINVAL) \
    FUNCDEF(int, drmGetDevices2, (uint32_t flags, drmDevicePtr devices[], int max_devices), (flags, devices, max_devices), return -EINVAL) \
    FUNCDEF(int, drmDevicesEqual, (drmDevicePtr a, drmDevicePtr b), (a, b), return 0) \
    FUNCDEF(int, drmSyncobjCreate, (int fd, uint32_t flags, uint32_t *handle), (fd, flags, handle), return 0) \
    FUNCDEF(int, drmSyncobjDestroy, (int fd, uint32_t handle), (fd, handle), return 0) \
    FUNCDEF(int, drmSyncobjHandleToFD, (int fd, uint32_t handle, int *obj_fd), (fd, handle, obj_fd), return 0) \
    FUNCDEF(int, drmSyncobjFDToHandle, (int fd, int obj_fd, uint32_t *handle), (fd, obj_fd, handle), return 0) \
    FUNCDEF(int, drmSyncobjImportSyncFile, (int fd, uint32_t handle, int sync_file_fd), (fd, handle, sync_file_fd), return 0) \
    FUNCDEF(int, drmSyncobjExportSyncFile, (int fd, uint32_t handle, int *sync_file_fd), (fd, handle, sync_file_fd), return 0) \
    FUNCDEF(int, drmSyncobjWait, (int fd, uint32_t *handles, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled), (fd, handles, num_handles, timeout_nsec, flags, first_signaled), return 0) \
    FUNCDEF(int, drmSyncobjReset, (int fd, const uint32_t *handles, uint32_t handle_count), (fd, handles, handle_count), return 0) \
    FUNCDEF(int, drmSyncobjSignal, (int fd, const uint32_t *handles, uint32_t handle_count), (fd, handles, handle_count), return 0) \
    FUNCDEF(int, drmSyncobjTimelineSignal, (int fd, const uint32_t *handles, uint64_t *points, uint32_t handle_count), (fd, handles, points, handle_count), return 0) \
    FUNCDEF(int, drmSyncobjTimelineWait, (int fd, uint32_t *handles, uint64_t *points, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled), (fd, handles, points, num_handles, timeout_nsec, flags, first_signaled), return 0) \
    FUNCDEF(int, drmSyncobjQuery, (int fd, uint32_t *handles, uint64_t *points, uint32_t handle_count), (fd, handles, points, handle_count), return 0) \
    FUNCDEF(int, drmSyncobjTransfer, (int fd, uint32_t dst_handle, uint64_t dst_point, uint32_t src_handle, uint64_t src_point, uint32_t flags), (fd, dst_handle, dst_point, src_handle, src_point, flags), return 0) \
    FUNCDEF(void, drmModeFreeModeInfo, ( drmModeModeInfoPtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreeResources, ( drmModeResPtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreeFB, ( drmModeFBPtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreeCrtc, ( drmModeCrtcPtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreeConnector, ( drmModeConnectorPtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreeEncoder, ( drmModeEncoderPtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreePlane, ( drmModePlanePtr ptr ), (ptr), return) \
    FUNCDEF(void, drmModeFreePlaneResources, (drmModePlaneResPtr ptr), (ptr), return) \
    FUNCDEF(drmModeResPtr, drmModeGetResources, (int fd), (fd), return 0) \
    FUNCDEF(drmModeFBPtr, drmModeGetFB, (int fd, uint32_t bufferId), (fd, bufferId), return 0) \
    FUNCDEF(int, drmModeAddFB, (int fd, uint32_t width, uint32_t height, uint8_t depth, uint8_t bpp, uint32_t pitch, uint32_t bo_handle, uint32_t *buf_id), (fd, width, height, depth, bpp, pitch, bo_handle, buf_id), return 0) \
    FUNCDEF(int, drmModeAddFB2, (int fd, uint32_t width, uint32_t height, uint32_t pixel_format, const uint32_t bo_handles[4], const uint32_t pitches[4], const uint32_t offsets[4], uint32_t *buf_id, uint32_t flags), (fd, width, height, pixel_format, bo_handles, pitches, offsets, buf_id, flags), return 0) \
    FUNCDEF(int, drmModeAddFB2WithModifiers, (int fd, uint32_t width, uint32_t height, uint32_t pixel_format, const uint32_t bo_handles[4], const uint32_t pitches[4], const uint32_t offsets[4], const uint64_t modifier[4], uint32_t *buf_id, uint32_t flags), (fd, width, height, pixel_format, bo_handles, pitches, offsets, modifier, buf_id, flags), return 0) \
    FUNCDEF(int, drmModeRmFB, (int fd, uint32_t bufferId), (fd, bufferId), return 0) \
    FUNCDEF(int, drmModeDirtyFB, (int fd, uint32_t bufferId, drmModeClipPtr clips, uint32_t num_clips), (fd, bufferId, clips, num_clips), return 0) \
    FUNCDEF(drmModeCrtcPtr, drmModeGetCrtc, (int fd, uint32_t crtcId), (fd, crtcId), return 0) \
    FUNCDEF(int, drmModeSetCrtc, (int fd, uint32_t crtcId, uint32_t bufferId, uint32_t x, uint32_t y, uint32_t *connectors, int count, drmModeModeInfoPtr mode), (fd, crtcId, bufferId, x, y, connectors, count, mode), return 0) \
    FUNCDEF(int, drmModeSetCursor, (int fd, uint32_t crtcId, uint32_t bo_handle, uint32_t width, uint32_t height), (fd, crtcId, bo_handle, width, height), return 0) \
    FUNCDEF(int, drmModeSetCursor2, (int fd, uint32_t crtcId, uint32_t bo_handle, uint32_t width, uint32_t height, int32_t hot_x, int32_t hot_y), (fd, crtcId, bo_handle, width, height, hot_x, hot_y), return 0) \
    FUNCDEF(int, drmModeMoveCursor, (int fd, uint32_t crtcId, int x, int y), (fd, crtcId, x, y), return 0) \
    FUNCDEF(drmModeEncoderPtr, drmModeGetEncoder, (int fd, uint32_t encoder_id), (fd, encoder_id), return 0) \
    FUNCDEF(drmModeConnectorPtr, drmModeGetConnector, (int fd, uint32_t connectorId), (fd, connectorId), return 0) \
    FUNCDEF(drmModeConnectorPtr, drmModeGetConnectorCurrent, (int fd, uint32_t connector_id), (fd, connector_id), return 0) \
    FUNCDEF(int, drmModeAttachMode, (int fd, uint32_t connectorId, drmModeModeInfoPtr mode_info), (fd, connectorId, mode_info), return 0) \
    FUNCDEF(int, drmModeDetachMode, (int fd, uint32_t connectorId, drmModeModeInfoPtr mode_info), (fd, connectorId, mode_info), return 0) \
    FUNCDEF(drmModePropertyPtr, drmModeGetProperty, (int fd, uint32_t propertyId), (fd, propertyId), return 0) \
    FUNCDEF(void, drmModeFreeProperty, (drmModePropertyPtr ptr), (ptr), return) \
    FUNCDEF(drmModePropertyBlobPtr, drmModeGetPropertyBlob, (int fd, uint32_t blob_id), (fd, blob_id), return 0) \
    FUNCDEF(void, drmModeFreePropertyBlob, (drmModePropertyBlobPtr ptr), (ptr), return) \
    FUNCDEF(int, drmModeConnectorSetProperty, (int fd, uint32_t connector_id, uint32_t property_id, uint64_t value), (fd, connector_id, property_id, value), return 0) \
    FUNCDEF(int, drmCheckModesettingSupported, (const char *busid), (busid), return 0) \
    FUNCDEF(int, drmModeCrtcSetGamma, (int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue), (fd, crtc_id, size, red, green, blue), return 0) \
    FUNCDEF(int, drmModeCrtcGetGamma, (int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue), (fd, crtc_id, size, red, green, blue), return 0) \
    FUNCDEF(int, drmModePageFlip, (int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data), (fd, crtc_id, fb_id, flags, user_data), return 0) \
    FUNCDEF(int, drmModePageFlipTarget, (int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data, uint32_t target_vblank), (fd, crtc_id, fb_id, flags, user_data, target_vblank), return 0) \
    FUNCDEF(drmModePlaneResPtr, drmModeGetPlaneResources, (int fd), (fd), return 0) \
    FUNCDEF(drmModePlanePtr, drmModeGetPlane, (int fd, uint32_t plane_id), (fd, plane_id), return 0) \
    FUNCDEF(int, drmModeSetPlane, (int fd, uint32_t plane_id, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, int32_t crtc_x, int32_t crtc_y, uint32_t crtc_w, uint32_t crtc_h, uint32_t src_x, uint32_t src_y, uint32_t src_w, uint32_t src_h), (fd, plane_id, crtc_id, fb_id, flags, crtc_x, crtc_y, crtc_w, crtc_h, src_x, src_y, src_w, src_h), return 0) \
    FUNCDEF(drmModeObjectPropertiesPtr, drmModeObjectGetProperties, (int fd, uint32_t object_id, uint32_t object_type), (fd, object_id, object_type), return 0) \
    FUNCDEF(void, drmModeFreeObjectProperties, (drmModeObjectPropertiesPtr ptr), (ptr), return) \
    FUNCDEF(int, drmModeObjectSetProperty, (int fd, uint32_t object_id, uint32_t object_type, uint32_t property_id, uint64_t value), (fd, object_id, object_type, property_id, value), return 0) \
    FUNCDEF(drmModeAtomicReqPtr, drmModeAtomicAlloc, (void), (), return 0) \
    FUNCDEF(drmModeAtomicReqPtr, drmModeAtomicDuplicate, (drmModeAtomicReqPtr req), (req), return 0) \
    FUNCDEF(int, drmModeAtomicMerge, (drmModeAtomicReqPtr base, drmModeAtomicReqPtr augment), (base, augment), return 0) \
    FUNCDEF(void, drmModeAtomicFree, (drmModeAtomicReqPtr req), (req), return) \
    FUNCDEF(int, drmModeAtomicGetCursor, (drmModeAtomicReqPtr req), (req), return 0) \
    FUNCDEF(void, drmModeAtomicSetCursor, (drmModeAtomicReqPtr req, int cursor), (req, cursor), return) \
    FUNCDEF(int, drmModeAtomicAddProperty, (drmModeAtomicReqPtr req, uint32_t object_id, uint32_t property_id, uint64_t value), (req, object_id, property_id, value), return 0) \
    FUNCDEF(int, drmModeAtomicCommit, (int fd, drmModeAtomicReqPtr req, uint32_t flags, void *user_data), (fd, req, flags, user_data), return 0) \
    FUNCDEF(int, drmModeCreatePropertyBlob, (int fd, const void *data, size_t size, uint32_t *id), (fd, data, size, id), return 0) \
    FUNCDEF(int, drmModeDestroyPropertyBlob, (int fd, uint32_t id), (fd, id), return 0) \
    FUNCDEF(int, drmModeCreateLease, (int fd, const uint32_t *objects, int num_objects, int flags, uint32_t *lessee_id), (fd, objects, num_objects, flags, lessee_id), return 0) \
    FUNCDEF(drmModeLesseeListPtr, drmModeListLessees, (int fd), (fd), return 0) \
    FUNCDEF(drmModeObjectListPtr, drmModeGetLease, (int fd), (fd), return 0) \
    FUNCDEF(int, drmModeRevokeLease, (int fd, uint32_t lessee_id), (fd, lessee_id), return 0)

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

void
drmMsg (const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}
