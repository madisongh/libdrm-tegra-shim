#ifndef _XF86DRMMODE_H_
#define _XF86DRMMODE_H_
/*
 * Copyright (c) 2007-2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * Copyright (c) 2007-2008 Dave Airlie <airlied@linux.ie>
 * Copyright (c) 2007-2008 Jakob Bornecrantz <wallbraker@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/*
 * Stripped-down version for Tegra NVDC DRM.
 */

#if defined(__cplusplus)
extern "C" {
#endif

#include <drm.h>
#include <errno.h>

/*
 * Feature defines
 *
 * Just because these are defined doesn't mean that the kernel
 * can do that feature, its just for new code vs old libdrm.
 */
#define DRM_MODE_FEATURE_KMS		1
#define DRM_MODE_FEATURE_DIRTYFB	1


typedef struct _drmModeRes {

	int count_fbs;
	uint32_t *fbs;

	int count_crtcs;
	uint32_t *crtcs;

	int count_connectors;
	uint32_t *connectors;

	int count_encoders;
	uint32_t *encoders;

	uint32_t min_width, max_width;
	uint32_t min_height, max_height;
} drmModeRes, *drmModeResPtr;

typedef struct _drmModeModeInfo {
	uint32_t clock;
	uint16_t hdisplay, hsync_start, hsync_end, htotal, hskew;
	uint16_t vdisplay, vsync_start, vsync_end, vtotal, vscan;

	uint32_t vrefresh;

	uint32_t flags;
	uint32_t type;
	char name[DRM_DISPLAY_MODE_LEN];
} drmModeModeInfo, *drmModeModeInfoPtr;

typedef struct _drmModeFB {
	uint32_t fb_id;
	uint32_t width, height;
	uint32_t pitch;
	uint32_t bpp;
	uint32_t depth;
	/* driver specific handle */
	uint32_t handle;
} drmModeFB, *drmModeFBPtr;

typedef struct _drmModePropertyBlob {
	uint32_t id;
	uint32_t length;
	void *data;
} drmModePropertyBlobRes, *drmModePropertyBlobPtr;

typedef struct _drmModeProperty {
	uint32_t prop_id;
	uint32_t flags;
	char name[DRM_PROP_NAME_LEN];
	int count_values;
	uint64_t *values; /* store the blob lengths */
	int count_enums;
	struct drm_mode_property_enum *enums;
	int count_blobs;
	uint32_t *blob_ids; /* store the blob IDs */
} drmModePropertyRes, *drmModePropertyPtr;

static __inline int drm_property_type_is(drmModePropertyPtr property,
		uint32_t type)
{
	/* instanceof for props.. handles extended type vs original types: */
	if (property->flags & DRM_MODE_PROP_EXTENDED_TYPE)
		return (property->flags & DRM_MODE_PROP_EXTENDED_TYPE) == type;
	return property->flags & type;
}

typedef struct _drmModeCrtc {
	uint32_t crtc_id;
	uint32_t buffer_id; /**< FB id to connect to 0 = disconnect */

	uint32_t x, y; /**< Position on the framebuffer */
	uint32_t width, height;
	int mode_valid;
	drmModeModeInfo mode;

	int gamma_size; /**< Number of gamma stops */

} drmModeCrtc, *drmModeCrtcPtr;

typedef struct _drmModeEncoder {
	uint32_t encoder_id;
	uint32_t encoder_type;
	uint32_t crtc_id;
	uint32_t possible_crtcs;
	uint32_t possible_clones;
} drmModeEncoder, *drmModeEncoderPtr;

typedef enum {
	DRM_MODE_CONNECTED         = 1,
	DRM_MODE_DISCONNECTED      = 2,
	DRM_MODE_UNKNOWNCONNECTION = 3
} drmModeConnection;

typedef enum {
	DRM_MODE_SUBPIXEL_UNKNOWN        = 1,
	DRM_MODE_SUBPIXEL_HORIZONTAL_RGB = 2,
	DRM_MODE_SUBPIXEL_HORIZONTAL_BGR = 3,
	DRM_MODE_SUBPIXEL_VERTICAL_RGB   = 4,
	DRM_MODE_SUBPIXEL_VERTICAL_BGR   = 5,
	DRM_MODE_SUBPIXEL_NONE           = 6
} drmModeSubPixel;

typedef struct _drmModeConnector {
	uint32_t connector_id;
	uint32_t encoder_id; /**< Encoder currently connected to */
	uint32_t connector_type;
	uint32_t connector_type_id;
	drmModeConnection connection;
	uint32_t mmWidth, mmHeight; /**< HxW in millimeters */
	drmModeSubPixel subpixel;

	int count_modes;
	drmModeModeInfoPtr modes;

	int count_props;
	uint32_t *props; /**< List of property ids */
	uint64_t *prop_values; /**< List of property values */

	int count_encoders;
	uint32_t *encoders; /**< List of encoder ids */
} drmModeConnector, *drmModeConnectorPtr;

#define DRM_PLANE_TYPE_OVERLAY 0
#define DRM_PLANE_TYPE_PRIMARY 1
#define DRM_PLANE_TYPE_CURSOR  2

typedef struct _drmModeObjectProperties {
	uint32_t count_props;
	uint32_t *props;
	uint64_t *prop_values;
} drmModeObjectProperties, *drmModeObjectPropertiesPtr;

typedef struct _drmModeAtomicReq drmModeAtomicReq, *drmModeAtomicReqPtr;

typedef struct _drmModePlane {
	uint32_t count_formats;
	uint32_t *formats;
	uint32_t plane_id;

	uint32_t crtc_id;
	uint32_t fb_id;

	uint32_t crtc_x, crtc_y;
	uint32_t x, y;

	uint32_t possible_crtcs;
	uint32_t gamma_size;
} drmModePlane, *drmModePlanePtr;

typedef struct _drmModePlaneRes {
	uint32_t count_planes;
	uint32_t *planes;
} drmModePlaneRes, *drmModePlaneResPtr;

int drmModeAddFB(int fd, uint32_t width, uint32_t height, uint8_t depth, uint8_t bpp, uint32_t pitch, uint32_t bo_handle, uint32_t *buf_id);
int drmModeAddFB2(int fd, uint32_t width, uint32_t height, uint32_t pixel_format, uint32_t bo_handles[4], uint32_t pitches[4], uint32_t offsets[4], uint32_t *buf_id, uint32_t flags);
int drmModeAddFB2WithModifiers(int fd, uint32_t width, uint32_t height, uint32_t pixel_format, const uint32_t bo_handles[4], const uint32_t pitches[4], const uint32_t offsets[4], const uint64_t modifier[4], uint32_t *buf_id, uint32_t flags);
int drmModeRmFB(int fd, uint32_t fb_id);
drmModeConnectorPtr drmModeGetConnector(int fd, uint32_t connector_id);
void drmModeFreeConnector(drmModeConnectorPtr ptr);
int drmModeConnectorSetProperty(int fd, uint32_t connector_id, uint32_t property_id, uint64_t value);
drmModeCrtcPtr drmModeGetCrtc(int fd, uint32_t crtc_id);
void drmModeFreeCrtc(drmModeCrtcPtr ptr);
int drmModeSetCrtc(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t *connectors, int count, drmModeModeInfoPtr drm_mode);
int drmModeCrtcSetGamma(int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue);
int drmModeCrtcGetGamma(int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue);
int drmModePageFlip(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data);
int drmModeSetCursor(int fd, uint32_t crtc_id, uint32_t bo_handle, uint32_t width, uint32_t height);
int drmModeSetCursor2(int fd, uint32_t crtcId, uint32_t bo_handle, uint32_t width, uint32_t height, int32_t hot_x, int32_t hot_y);
int drmModeMoveCursor(int fd, uint32_t crtc_id, int x, int y);
drmModeEncoderPtr drmModeGetEncoder(int fd, uint32_t encoder_id);
void drmModeFreeEncoder(drmModeEncoderPtr ptr);
drmModePlanePtr drmModeGetPlane(int fd, uint32_t plane_id);
void drmModeFreePlane(drmModePlanePtr ptr);
int drmModeSetPlane(int fd, uint32_t plane_id, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, uint32_t crtc_x, uint32_t crtc_y, uint32_t crtc_w, uint32_t crtc_h, uint32_t src_x, uint32_t src_y, uint32_t src_w, uint32_t src_h);
drmModeObjectPropertiesPtr drmModeObjectGetProperties(int fd, uint32_t object_id, uint32_t object_type);
void drmModeFreeObjectProperties(drmModeObjectPropertiesPtr ptr);
int drmModeObjectSetProperty(int fd, uint32_t object_id, uint32_t object_type, uint32_t property_id, uint64_t value);
drmModePropertyPtr drmModeGetProperty(int fd, uint32_t propertyId);
void drmModeFreeProperty(drmModePropertyPtr ptr);
drmModeResPtr drmModeGetResources(int fd);
void drmModeFreeResources(drmModeResPtr ptr);
drmModePlaneResPtr drmModeGetPlaneResources(int fd);
void drmModeFreePlaneResources(drmModePlaneResPtr ptr);
drmModeAtomicReqPtr drmModeAtomicAlloc(void);
int drmModeAtomicAddProperty(drmModeAtomicReqPtr req, uint32_t object_id, uint32_t property_id, uint64_t value);
int drmModeAtomicCommit(int fd, drmModeAtomicReqPtr req, uint32_t flags, void *user_data);
void drmModeAtomicFree(drmModeAtomicReqPtr req);
int drmModeCreatePropertyBlob(int fd, const void *data, size_t size, uint32_t *id);
int drmModeDestroyPropertyBlob(int fd, uint32_t id);
drmModePropertyBlobPtr drmModeGetPropertyBlob(int fd, uint32_t blob_id);
void drmModeFreePropertyBlob(drmModePropertyBlobPtr ptr);

#if defined(__cplusplus)
}
#endif

#endif /* _XF86DRMMODE_H_ */
