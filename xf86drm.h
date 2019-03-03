#ifndef _XF86DRM_H_
#define _XF86DRM_H_
/*
 * Copyright 1999, 2000 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * Stripped-down version for Tegra DRM-NVDC user-mode library.
 */
#if defined(__cplusplus)
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>
#include <stdarg.h>
#include "drm.h"
#include "tegra_drm.h"

typedef unsigned int drm_magic_t;
typedef struct _drmVersion {
    int     version_major;        /**< Major version */
    int     version_minor;        /**< Minor version */
    int     version_patchlevel;   /**< Patch level */
    int     name_len; 	          /**< Length of name buffer */
    char    *name;	          /**< Name of driver */
    int     date_len;             /**< Length of date buffer */
    char    *date;                /**< User-space buffer to hold date */
    int     desc_len;	          /**< Length of desc buffer */
    char    *desc;                /**< User-space buffer to hold desc */
} drmVersion, *drmVersionPtr;

typedef enum {
    DRM_VBLANK_ABSOLUTE = 0x0,	/**< Wait for specific vblank sequence number */
    DRM_VBLANK_RELATIVE = 0x1,	/**< Wait for given number of vblanks */
    /* bits 1-6 are reserved for high crtcs */
    DRM_VBLANK_HIGH_CRTC_MASK = 0x0000003e,
    DRM_VBLANK_EVENT = 0x4000000,	/**< Send event instead of blocking */
    DRM_VBLANK_FLIP = 0x8000000,	/**< Scheduled buffer swap should flip */
    DRM_VBLANK_NEXTONMISS = 0x10000000,	/**< If missed, wait for next vblank */
    DRM_VBLANK_SECONDARY = 0x20000000,	/**< Secondary display controller */
    DRM_VBLANK_SIGNAL   = 0x40000000	/* Send signal instead of blocking */
} drmVBlankSeqType;
#define DRM_VBLANK_HIGH_CRTC_SHIFT 1

typedef struct _drmVBlankReq {
	drmVBlankSeqType type;
	unsigned int sequence;
	unsigned long signal;
} drmVBlankReq, *drmVBlankReqPtr;

typedef struct _drmVBlankReply {
	drmVBlankSeqType type;
	unsigned int sequence;
	long tval_sec;
	long tval_usec;
} drmVBlankReply, *drmVBlankReplyPtr;

typedef union _drmVBlank {
	drmVBlankReq request;
	drmVBlankReply reply;
} drmVBlank, *drmVBlankPtr;

typedef struct _drmSetVersion {
	int drm_di_major;
	int drm_di_minor;
	int drm_dd_major;
	int drm_dd_minor;
} drmSetVersion, *drmSetVersionPtr;

typedef enum {
    DRM_FRAME_BUFFER    = 0,      /**< WC, no caching, no core dump */
    DRM_REGISTERS       = 1,      /**< no caching, no core dump */
    DRM_SHM             = 2,      /**< shared, cached */
    DRM_AGP             = 3,	  /**< AGP/GART */
    DRM_SCATTER_GATHER  = 4,	  /**< PCI scatter/gather */
    DRM_CONSISTENT      = 5	  /**< PCI consistent */
} drmMapType;

typedef enum {
    DRM_RESTRICTED      = 0x0001, /**< Cannot be mapped to client-virtual */
    DRM_READ_ONLY       = 0x0002, /**< Read-only in client-virtual */
    DRM_LOCKED          = 0x0004, /**< Physical pages locked */
    DRM_KERNEL          = 0x0008, /**< Kernel requires access */
    DRM_WRITE_COMBINING = 0x0010, /**< Use write-combining, if available */
    DRM_CONTAINS_LOCK   = 0x0020, /**< SHM page that contains lock */
    DRM_REMOVABLE	= 0x0040  /**< Removable mapping */
} drmMapFlags;

typedef enum {
    DRM_LOCK_READY      = 0x01, /**< Wait until hardware is ready for DMA */
    DRM_LOCK_QUIESCENT  = 0x02, /**< Wait until hardware quiescent */
    DRM_LOCK_FLUSH      = 0x04, /**< Flush this context's DMA queue first */
    DRM_LOCK_FLUSH_ALL  = 0x08, /**< Flush all DMA queues first */
				/* These *HALT* flags aren't supported yet
                                   -- they will be used to support the
                                   full-screen DGA-like mode. */
    DRM_HALT_ALL_QUEUES = 0x10, /**< Halt all current and future queues */
    DRM_HALT_CUR_QUEUES = 0x20  /**< Halt all current queues */
} drmLockFlags;

typedef struct _drmLock {
    volatile unsigned int lock;
    char                      padding[60];
    /* This is big enough for most current (and future?) architectures:
       DEC Alpha:              32 bytes
       Intel Merced:           ?
       Intel P5/PPro/PII/PIII: 32 bytes
       Intel StrongARM:        32 bytes
       Intel i386/i486:        16 bytes
       MIPS:                   32 bytes (?)
       Motorola 68k:           16 bytes
       Motorola PowerPC:       32 bytes
       Sun SPARC:              32 bytes
    */
} drmLock, *drmLockPtr;

#define DRM_EVENT_CONTEXT_VERSION 4

/*
 * DRM-NVDC only supports vblank_handler (?)
 */
typedef struct _drmEventContext {

	/* This struct is versioned so we can add more pointers if we
	 * add more events. */
	int version;

	void (*vblank_handler)(int fd,
			       unsigned int sequence, 
			       unsigned int tv_sec,
			       unsigned int tv_usec,
			       void *user_data);

	void (*page_flip_handler)(int fd,
				  unsigned int sequence,
				  unsigned int tv_sec,
				  unsigned int tv_usec,
				  void *user_data);

	void (*page_flip_handler2)(int fd,
				   unsigned int sequence,
				   unsigned int tv_sec,
				   unsigned int tv_usec,
				   unsigned int crtc_id,
				   void *user_data);

	void (*sequence_handler)(int fd,
				 uint64_t sequence,
				 uint64_t ns,
				 uint64_t user_data);
} drmEventContext, *drmEventContextPtr;

/*
 * The following are needed for mesa to compile,
 * and are not the full set of definitions.
 */
#define DRM_BUS_PCI       0
#define DRM_BUS_PLATFORM  2
#define DRM_BUS_HOST1X    3

#define DRM_NODE_PRIMARY 0
#define DRM_NODE_RENDER  2

#define DRM_DIR_NAME "/dev"
#define DRM_DEV_NAME "drm-nvdc"

typedef struct _drmPciBusInfo {
    uint16_t domain;
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
} drmPciBusInfo, *drmPciBusInfoPtr;

#define DRM_PLATFORM_DEVICE_NAME_LEN 512

typedef struct _drmPlatformBusInfo {
    char fullname[DRM_PLATFORM_DEVICE_NAME_LEN];
} drmPlatformBusInfo, *drmPlatformBusInfoPtr;

#define DRM_HOST1X_DEVICE_NAME_LEN 512

typedef struct _drmHost1xBusInfo {
    char fullname[DRM_HOST1X_DEVICE_NAME_LEN];
} drmHost1xBusInfo, *drmHost1xBusInfoPtr;

typedef struct _drmPciDeviceInfo {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t subvendor_id;
    uint16_t subdevice_id;
    uint8_t revision_id;
} drmPciDeviceInfo, *drmPciDeviceInfoPtr;

typedef struct _drmDevice {
    char **nodes; /* DRM_NODE_MAX sized array */
    int available_nodes; /* DRM_NODE_* bitmask */
    int bustype;
    union {
        drmPciBusInfoPtr pci;
        drmPlatformBusInfoPtr platform;
        drmHost1xBusInfoPtr host1x;
    } businfo;
    union {
        drmPciDeviceInfoPtr pci;
    } deviceinfo;
} drmDevice, *drmDevicePtr;

typedef unsigned int  drmSize,     *drmSizePtr;
typedef void          *drmAddress, **drmAddressPtr;

#if (__GNUC__ >= 3)
#define DRM_PRINTFLIKE(f, a) __attribute__ ((format(__printf__, f, a)))
#else
#define DRM_PRINTFLIKE(f, a)
#endif

typedef struct _drmServerInfo {
  int (*debug_print)(const char *format, va_list ap) DRM_PRINTFLIKE(1,0);
  int (*load_module)(const char *name);
  void (*get_perms)(gid_t *, mode_t *);
} drmServerInfo, *drmServerInfoPtr;

typedef struct drmHashEntry {
    int      fd;
    void     (*f)(int, void *, void *);
    void     *tagTable;
} drmHashEntry;

typedef enum {
    DRM_CONTEXT_PRESERVED = 0x01, /**< This context is preserved and
				     never swapped. */
    DRM_CONTEXT_2DONLY    = 0x02  /**< This context is for 2D rendering only. */
} drm_context_tFlags, *drm_context_tFlagsPtr;

#include <errno.h>
static inline __attribute__((unused)) int drmGetDevice2(int fd, uint32_t flags, drmDevicePtr *device) { return -EINVAL; }
static inline __attribute__((unused)) int drmGetDevices2(uint32_t flags, drmDevicePtr devices[], int max_devices) { return -EINVAL; }
static inline __attribute__((unused)) void drmFreeDevice(drmDevicePtr *device) { return; }
static inline __attribute__((unused)) void drmFreeDevices(drmDevicePtr devices[], int max_devices) { return; }
static inline __attribute__((unused)) char *drmGetDeviceNameFromFd2(int fd) { return 0; }
static inline __attribute__((unused)) int drmOpenOnce(void *unused, const char *BusID, int *newlyopened) { return -1; }
static inline __attribute__((unused)) void drmCloseOnce(int fd) { return; }
static inline __attribute__((unused)) int drmGetNodeTypeFromFd(int fd) { return -1; }


/*
 * end of mesa compilation hacks
 */

void* drmGetHashTable(void);
drmHashEntry* drmGetEntry(int fd);
int drmAvailable(void);
int drmOpen (const char *name, const char *busid);
int drmClose (int fd);
int drmGetCap (int fd, uint64_t capability, uint64_t *value);
int drmSetClientCap (int fd, uint64_t capability, uint64_t value);
int drmSetMaster (int fd);
int drmDropMaster (int fd);
int drmGetMagic (int fd, drm_magic_t *magic);
int drmAuthMagic (int fd, drm_magic_t magic);
drmVersionPtr drmGetVersion (int fd);
void drmFreeVersion (drmVersionPtr version);
int drmHandleEvent (int fd, drmEventContextPtr evctx);
int drmIoctl (int fd, unsigned long request, void *arg);
int drmWaitVBlank (int fd, drmVBlankPtr vbl);
void drmSetServerInfo(drmServerInfoPtr info);
int drmPrimeFDToHandle (int fd, int prime_fd, uint32_t *handle);
int drmPrimeHandleToFD (int fd, uint32_t handle, uint32_t flags, int *prime_fd);
int drmSetInterfaceVersion (int fd, drmSetVersion *version);
char* drmGetBusid (int fd);
int drmSetBusid(int fd, const char *busid);
void drmFreeBusid (const char *busid);
int drmAddMap(int fd, drm_handle_t offset, drmSize size, drmMapType type, drmMapFlags flags, drm_handle_t * handle);
int drmRmMap(int fd, drm_handle_t handle);
int drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address);
int drmUnmap(drmAddress address, drmSize size);
int drmGetLock(int fd, drm_context_t context, drmLockFlags flags);
int drmUnlock(int fd, drm_context_t context);
char* drmGetDeviceNameFromFd (int fd);
void drmFree (void *p);
int drmHashFirst(void *t, unsigned long *key, void **value);
int drmHashNext(void *t, unsigned long *key, void **value);
int drmCommandWriteRead (int fd, unsigned long drmCommandIndex, void *data, unsigned long size);
int drmCreateContext(int fd, drm_context_t * handle);
int drmSetContextFlags(int fd, drm_context_t context, drm_context_tFlags flags);
int drmAddContextTag(int fd, drm_context_t context, void *tag);
int drmDelContextTag(int fd, drm_context_t context);
void* drmGetContextTag(int fd, drm_context_t context);
drm_context_t* drmGetReservedContextList(int fd, int *count);
void drmFreeReservedContextList(drm_context_t *handle);
int drmDestroyContext(int fd, drm_context_t handle);
int drmCreateDrawable(int fd, drm_drawable_t * handle);
int drmDestroyDrawable(int fd, drm_drawable_t handle);
int drmUpdateDrawableInfo(int fd, drm_drawable_t handle, drm_drawable_info_type_t type, unsigned int num, void *data);

#if defined(__cplusplus)
}
#endif

#endif /* _XF86DRM_H_ */
