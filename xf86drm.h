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

#include <stdint.h>
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

typedef unsigned int drmLock, *drmLockPtr; // actual definition is a structure, but we don't care
typedef unsigned int  drmSize,     *drmSizePtr;
typedef void          *drmAddress, **drmAddressPtr;

#include <errno.h>
static inline __attribute__((unused)) int drmGetDevice2(int fd, uint32_t flags, drmDevicePtr *device) { return -EINVAL; }
static inline __attribute__((unused)) int drmGetDevices2(uint32_t flags, drmDevicePtr devices[], int max_devices) { return -EINVAL; }
static inline __attribute__((unused)) void drmFreeDevice(drmDevicePtr *device) { return; }
static inline __attribute__((unused)) void drmFreeDevices(drmDevicePtr devices[], int max_devices) { return; }
static inline __attribute__((unused)) char *drmGetDeviceNameFromFd2(int fd) { return 0; }
static inline __attribute__((unused)) int drmOpenOnce(void *unused, const char *BusID, int *newlyopened) { return -1; }
static inline __attribute__((unused)) void drmCloseOnce(int fd) { return; }
static inline __attribute__((unused)) int drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address) { return -EINVAL; }
static inline __attribute__((unused)) int drmUnmap(drmAddress address, drmSize size) { return 0; }
static inline __attribute__((unused)) int drmGetNodeTypeFromFd(int fd) { return -1; }


/*
 * end of mesa compilation hacks
 */

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
int drmPrimeFDToHandle (int fd, int prime_fd, uint32_t *handle);
int drmPrimeHandleToFD (int fd, uint32_t handle, uint32_t flags, int *prime_fd);
int drmSetInterfaceVersion (int fd, drmSetVersion *version);
char* drmGetBusid (int fd);
void drmFreeBusid (const char *busid);
char* drmGetDeviceNameFromFd (int fd);
void drmFree (void *p);
int drmCommandWriteRead (int fd, unsigned long drmCommandIndex, void *data, unsigned long size);

#if defined(__cplusplus)
}
#endif

#endif /* _XF86DRM_H_ */
