/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_color_mgmt.h>

/**
 * DOC: overview
 *
 * Color management or color space adjustments is supported through a set of 5
 * properties on the &drm_crtc object. They are set up by calling
 * drm_crtc_enable_color_mgmt().
 *
 * "DEGAMMA_LUT”:
 *	Blob property to set the degamma lookup table (LUT) mapping pixel data
 *	from the framebuffer before it is given to the transformation matrix.
 *	The data is interpreted as an array of &struct drm_color_lut elements.
 *	Hardware might choose not to use the full precision of the LUT elements
 *	nor use all the elements of the LUT (for example the hardware might
 *	choose to interpolate between LUT[0] and LUT[4]).
 *
 *	Setting this to NULL (blob property value set to 0) means a
 *	linear/pass-thru gamma table should be used. This is generally the
 *	driver boot-up state too. Drivers can access this blob through
 *	&drm_crtc_state.degamma_lut.
 *
 * “DEGAMMA_LUT_SIZE”:
 *	Unsinged range property to give the size of the lookup table to be set
 *	on the DEGAMMA_LUT property (the size depends on the underlying
 *	hardware). If drivers support multiple LUT sizes then they should
 *	publish the largest size, and sub-sample smaller sized LUTs (e.g. for
 *	split-gamma modes) appropriately.
 *
 * “CTM”:
 *	Blob property to set the current transformation matrix (CTM) apply to
 *	pixel data after the lookup through the degamma LUT and before the
 *	lookup through the gamma LUT. The data is interpreted as a struct
 *	&drm_color_ctm.
 *
 *	Setting this to NULL (blob property value set to 0) means a
 *	unit/pass-thru matrix should be used. This is generally the driver
 *	boot-up state too. Drivers can access the blob for the color conversion
 *	matrix through &drm_crtc_state.ctm.
 *
 * “GAMMA_LUT”:
 *	Blob property to set the gamma lookup table (LUT) mapping pixel data
 *	after the transformation matrix to data sent to the connector. The
 *	data is interpreted as an array of &struct drm_color_lut elements.
 *	Hardware might choose not to use the full precision of the LUT elements
 *	nor use all the elements of the LUT (for example the hardware might
 *	choose to interpolate between LUT[0] and LUT[4]).
 *
 *	Setting this to NULL (blob property value set to 0) means a
 *	linear/pass-thru gamma table should be used. This is generally the
 *	driver boot-up state too. Drivers can access this blob through
 *	&drm_crtc_state.gamma_lut.
 *
 * “GAMMA_LUT_SIZE”:
 *	Unsigned range property to give the size of the lookup table to be set
 *	on the GAMMA_LUT property (the size depends on the underlying hardware).
 *	If drivers support multiple LUT sizes then they should publish the
 *	largest size, and sub-sample smaller sized LUTs (e.g. for split-gamma
 *	modes) appropriately.
 *
 * There is also support for a legacy gamma table, which is set up by calling
 * drm_mode_crtc_set_gamma_size(). Drivers which support both should use
 * drm_atomic_helper_legacy_gamma_set() to alias the legacy gamma ramp with the
 * "GAMMA_LUT" property above.
 *
 * Support for different non RGB color encodings is controlled through
 * &drm_plane specific COLOR_ENCODING and COLOR_RANGE properties. They
 * are set up by calling drm_plane_create_color_properties().
 *
 * "COLOR_ENCODING"
 *	Optional plane enum property to support different non RGB
 *	color encodings. The driver can provide a subset of standard
 *	enum values supported by the DRM plane.
 *
 * "COLOR_RANGE"
 *	Optional plane enum property to support different non RGB
 *	color parameter ranges. The driver can provide a subset of
 *	standard enum values supported by the DRM plane.
 */

static const char * const color_encoding_name[] = {
	[DRM_COLOR_YCBCR_BT601] = "ITU-R BT.601 YCbCr",
	[DRM_COLOR_YCBCR_BT709] = "ITU-R BT.709 YCbCr",
	[DRM_COLOR_YCBCR_BT2020] = "ITU-R BT.2020 YCbCr",
};

static const char * const color_range_name[] = {
	[DRM_COLOR_YCBCR_FULL_RANGE] = "YCbCr full range",
	[DRM_COLOR_YCBCR_LIMITED_RANGE] = "YCbCr limited range",
};

/**
 * drm_get_color_encoding_name - return a string for color encoding
 * @encoding: color encoding to compute name of
 *
 * In contrast to the other drm_get_*_name functions this one here returns a
 * const pointer and hence is threadsafe.
 */
const char *drm_get_color_encoding_name(enum drm_color_encoding encoding)
{
	if (WARN_ON(encoding >= ARRAY_SIZE(color_encoding_name)))
		return "unknown";

	return color_encoding_name[encoding];
}

/**
 * drm_get_color_range_name - return a string for color range
 * @range: color range to compute name of
 *
 * In contrast to the other drm_get_*_name functions this one here returns a
 * const pointer and hence is threadsafe.
 */
const char *drm_get_color_range_name(enum drm_color_range range)
{
	if (WARN_ON(range >= ARRAY_SIZE(color_range_name)))
		return "unknown";

	return color_range_name[range];
}
