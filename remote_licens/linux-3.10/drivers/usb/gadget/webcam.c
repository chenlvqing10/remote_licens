/*
 *	webcam.c -- USB webcam gadget driver
 *
 *	Copyright (C) 2009-2010
 *	    Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/usb/video.h>

#include "f_uvc.h"

/*
 * Kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */
#include "uvc_queue.c"
#include "uvc_video.c"
#include "uvc_v4l2.c"
#include "f_uvc.c"

#ifdef CONFIG_UVC_ADD_MSC
#include "f_mass_storage.c"

static struct fsg_module_parameters mod_data = {
	.stall = 1,
	.removable[0] = 1,
	.ro[0] = 0,
};

FSG_MODULE_PARAMETERS(/* no prefix */, mod_data);

static int msg_thread_exits(struct fsg_common *common)
{
	return 0;
}

static int __init msg_do_config(struct usb_configuration *c)
{
	static const struct fsg_operations ops = {
		.thread_exits = msg_thread_exits,
	};

	static struct fsg_common common;
	struct fsg_common *retp;
	struct fsg_config config;
	int ret;

	fsg_config_from_params(&config, &mod_data);

	config.ops = &ops;
	retp = fsg_common_init(&common, c->cdev, &config);
	if (IS_ERR(retp))
		return PTR_ERR(retp);

	ret = fsg_bind_config(c->cdev, c, &common);
	fsg_common_put(&common);

	return ret;
}
#endif	/* CONFIG_UVC_ADD_MSC */

#ifdef CONFIG_UVC_ADD_ADB
#include "f_adb.c"
#endif	/* CONFIG_UVC_ADD_ADB */

#ifdef CONFIG_UVC_ADD_UAC_MIC
#include "u_uac1_mic.h"
#include "u_uac1_mic.c"
#include "f_uac1_mic.c"
#endif /* CONFIG_UVC_ADD_UAC_MIC */

USB_GADGET_COMPOSITE_OPTIONS();
/* --------------------------------------------------------------------------
 * Device descriptor
 */

#ifdef CONFIG_UVC_ADD_ADB
#define WEBCAM_VENDOR_ID               0x18D1
#define WEBCAM_PRODUCT_ID              0x4EE7
#else
#define WEBCAM_VENDOR_ID               0x1d6b  /* Linux Foundation */
#define WEBCAM_PRODUCT_ID              0x0102  /* Webcam A/V gadget */
#endif /* CONFIG_UVC_ADD_ADB */

#define WEBCAM_DEVICE_BCD		0x0010	/* 0.10 */

static char webcam_vendor_label[] = "Linux Foundation";
static char webcam_product_label[] = "Webcam gadget";
static char webcam_config_label[] = "Video";

/* string IDs are assigned dynamically */

#define STRING_DESCRIPTION_IDX		USB_GADGET_FIRST_AVAIL_IDX

static struct usb_string webcam_strings[] = {
	[USB_GADGET_MANUFACTURER_IDX].s = webcam_vendor_label,
	[USB_GADGET_PRODUCT_IDX].s = webcam_product_label,
	[USB_GADGET_SERIAL_IDX].s = "",
	[STRING_DESCRIPTION_IDX].s = webcam_config_label,
	{  }
};

static struct usb_gadget_strings webcam_stringtab = {
	.language = 0x0409,	/* en-us */
	.strings = webcam_strings,
};

static struct usb_gadget_strings *webcam_device_strings[] = {
	&webcam_stringtab,
	NULL,
};

static struct usb_device_descriptor webcam_device_descriptor = {
	.bLength		= USB_DT_DEVICE_SIZE,
	.bDescriptorType	= USB_DT_DEVICE,
	.bcdUSB			= cpu_to_le16(0x0200),
	.bDeviceClass		= USB_CLASS_MISC,
	.bDeviceSubClass	= 0x02,
	.bDeviceProtocol	= 0x01,
	.bMaxPacketSize0	= 0, /* dynamic */
	.idVendor		= cpu_to_le16(WEBCAM_VENDOR_ID),
	.idProduct		= cpu_to_le16(WEBCAM_PRODUCT_ID),
	.bcdDevice		= cpu_to_le16(WEBCAM_DEVICE_BCD),
	.iManufacturer		= 0, /* dynamic */
	.iProduct		= 0, /* dynamic */
	.iSerialNumber		= 0, /* dynamic */
	.bNumConfigurations	= 0, /* dynamic */
};

DECLARE_UVC_HEADER_DESCRIPTOR(1);

static const struct UVC_HEADER_DESCRIPTOR(1) uvc_control_header = {
	.bLength		= UVC_DT_HEADER_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_HEADER,
	.bcdUVC			= cpu_to_le16(0x0100),
	.wTotalLength		= 0, /* dynamic */
	.dwClockFrequency	= cpu_to_le32(48000000),
	.bInCollection		= 0, /* dynamic */
	.baInterfaceNr[0]	= 0, /* dynamic */
};

static const struct uvc_camera_terminal_descriptor uvc_camera_terminal = {
	.bLength		= UVC_DT_CAMERA_TERMINAL_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_INPUT_TERMINAL,
	.bTerminalID		= 1,
	.wTerminalType		= cpu_to_le16(0x0201),
	.bAssocTerminal		= 0,
	.iTerminal		= 0,
	.wObjectiveFocalLengthMin	= cpu_to_le16(0),
	.wObjectiveFocalLengthMax	= cpu_to_le16(0),
	.wOcularFocalLength		= cpu_to_le16(0),
	.bControlSize		= 3,
	.bmControls[0]		= 0,
	.bmControls[1]		= 0,
	.bmControls[2]		= 0,
};

static const struct uvc_processing_unit_descriptor uvc_processing = {
	.bLength		= UVC_DT_PROCESSING_UNIT_SIZE(2),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_PROCESSING_UNIT,
	.bUnitID		= 2,
	.bSourceID		= 1,
	.wMaxMultiplier		= cpu_to_le16(16*1024),
	.bControlSize		= 2,
	.bmControls[0]		= 0,
	.bmControls[1]		= 0,
	.iProcessing		= 0,
};

static const struct uvc_output_terminal_descriptor uvc_output_terminal = {
	.bLength		= UVC_DT_OUTPUT_TERMINAL_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_OUTPUT_TERMINAL,
	.bTerminalID		= 3,
	.wTerminalType		= cpu_to_le16(0x0101),
	.bAssocTerminal		= 0,
	.bSourceID		= 2,
	.iTerminal		= 0,
};

DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 2);

static const struct UVC_INPUT_HEADER_DESCRIPTOR(1, 2) uvc_input_header = {
	.bLength		= UVC_DT_INPUT_HEADER_SIZE(1, 2),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_INPUT_HEADER,
	.bNumFormats		= 2,
	.wTotalLength		= 0, /* dynamic */
	.bEndpointAddress	= 0, /* dynamic */
	.bmInfo			= 0,
	.bTerminalLink		= 3,
	.bStillCaptureMethod	= 2,
	.bTriggerSupport	= 0,
	.bTriggerUsage		= 0,
	.bControlSize		= 1,
	.bmaControls[0][0]	= 0,
	.bmaControls[1][0]	= 0,
};

static const struct uvc_format_uncompressed uvc_format_yuv = {
	.bLength		= UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_UNCOMPRESSED,
	.bFormatIndex		= 2,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	.bNumFrameDescriptors	= 3,
#elif defined CONFIG_UVC_FRAME_SIZE_720P
	.bNumFrameDescriptors	= 2,
#endif
	.guidFormat		= {
		'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00,
		0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
	.bBitsPerPixel		= 16,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	.bDefaultFrameIndex	= 3,
#elif defined CONFIG_UVC_FRAME_SIZE_720P
	.bDefaultFrameIndex	= 2,
#endif
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
};

DECLARE_UVC_FRAME_UNCOMPRESSED(1);
DECLARE_UVC_FRAME_UNCOMPRESSED(3);

static const struct UVC_FRAME_UNCOMPRESSED(3) uvc_frame_yuv_360p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(36864000),
	.dwMaxBitRate		= cpu_to_le32(110592000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(460800),
	.dwDefaultFrameInterval = cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(333333),	/* 33ms  -> 30.00 fps */
	.dwFrameInterval[1]	= cpu_to_le32(666666),	/* 66ms  -> 15.00 fps */
	.dwFrameInterval[2]	= cpu_to_le32(1000000),	/* 100ms -> 10.00 fps */
};

static const struct UVC_FRAME_UNCOMPRESSED(1) uvc_frame_yuv_720p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(29491200),
	.dwMaxBitRate		= cpu_to_le32(29491200),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1843200),
	.dwDefaultFrameInterval	= cpu_to_le32(5000000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_UNCOMPRESSED(1) uvc_frame_yuv_1080p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 3,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1920),
	.wHeight		= cpu_to_le16(1080),
	.dwMinBitRate		= cpu_to_le32(66355200),
	.dwMaxBitRate		= cpu_to_le32(66355200),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(4147200),
	.dwDefaultFrameInterval	= cpu_to_le32(5000000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(5000000),
};

#if defined CONFIG_UVC_FORMAT_H264
static const struct uvc_format_framebased uvc_format_framebased = {
	.bLength		= UVC_DT_FORMAT_FRAMEBASED_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_FRAME_BASED,
	.bFormatIndex		= 1,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	.bNumFrameDescriptors	= 3,
#elif defined CONFIG_UVC_FRAME_SIZE_720P
	.bNumFrameDescriptors	= 2,
#endif
	.guidFormat		= {
		'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00,
		0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
	.bBitsPerPixel		= 0,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	.bDefaultFrameIndex	= 3,
#elif defined CONFIG_UVC_FRAME_SIZE_720P
	.bDefaultFrameIndex	= 2,
#endif
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
	.bVariableSize		= 1,
};

DECLARE_UVC_FRAME_FRAMEBASED(3);

static const struct UVC_FRAME_FRAMEBASED(3) uvc_frame_framebased_360p = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(3000000),
	.dwMaxBitRate		= cpu_to_le32(7000000),
#if defined CONFIG_UVC_FRMAE_RATE_30
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwBytesPerLine		= 0,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
#endif
};

static const struct UVC_FRAME_FRAMEBASED(3) uvc_frame_framebased_720p = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(6000000),
	.dwMaxBitRate		= cpu_to_le32(14000000),
#if defined CONFIG_UVC_FRMAE_RATE_30
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwBytesPerLine		= 0,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
#endif
};

#if defined CONFIG_UVC_FRAME_SIZE_1080P
static const struct UVC_FRAME_FRAMEBASED(3) uvc_frame_framebased_1080p = {
	.bLength		= UVC_DT_FRAME_FRAMEBASED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_FRAME_BASED,
	.bFrameIndex		= 3,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1920),
	.wHeight		= cpu_to_le16(1080),
	.dwMinBitRate		= cpu_to_le32(12000000),
	.dwMaxBitRate		= cpu_to_le32(28000000),
#if defined CONFIG_UVC_FRMAE_RATE_30
	.dwDefaultFrameInterval	= cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwBytesPerLine		= 0,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
#endif
};
#endif /* CONFIG_UVC_FRAME_SIZE_1080P */

#elif defined CONFIG_UVC_FORMAT_MJPG

static const struct uvc_format_mjpeg uvc_format_mjpg = {
	.bLength		= UVC_DT_FORMAT_MJPEG_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_MJPEG,
	.bFormatIndex		= 1,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	.bNumFrameDescriptors	= 3,
#elif defined CONFIG_UVC_FRAME_SIZE_720P
	.bNumFrameDescriptors	= 2,
#endif
	.bmFlags		= 0,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	.bDefaultFrameIndex	= 3,
#elif defined CONFIG_UVC_FRAME_SIZE_720P
	.bDefaultFrameIndex	= 2,
#endif
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
};

DECLARE_UVC_FRAME_MJPEG(1);
DECLARE_UVC_FRAME_MJPEG(3);

static const struct UVC_FRAME_MJPEG(3) uvc_frame_mjpg_360p = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(460800),
#if defined CONFIG_UVC_FRMAE_RATE_15
	.dwDefaultFrameInterval	= cpu_to_le32(666666),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(666666),
	.dwFrameInterval[1]	= cpu_to_le32(1000000),
	.dwFrameInterval[2]	= cpu_to_le32(5000000),
#elif defined CONFIG_UVC_FRMAE_RATE_30
	.dwDefaultFrameInterval = cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
#endif
};

static const struct UVC_FRAME_MJPEG(3) uvc_frame_mjpg_720p = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(29491200),
	.dwMaxBitRate		= cpu_to_le32(88473600),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1843200),
#if defined CONFIG_UVC_FRMAE_RATE_15
	.dwDefaultFrameInterval	= cpu_to_le32(666666),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(666666),
	.dwFrameInterval[1]	= cpu_to_le32(1000000),
	.dwFrameInterval[2]	= cpu_to_le32(5000000),
#elif defined CONFIG_UVC_FRMAE_RATE_30
	.dwDefaultFrameInterval = cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
#endif
};

#if defined CONFIG_UVC_FRAME_SIZE_1080P
static const struct UVC_FRAME_MJPEG(3) uvc_frame_mjpg_1080p = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 3,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1920),
	.wHeight		= cpu_to_le16(1080),
	.dwMinBitRate		= cpu_to_le32(165888000),
	.dwMaxBitRate		= cpu_to_le32(497664000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(4147200),
#if defined CONFIG_UVC_FRMAE_RATE_15
	.dwDefaultFrameInterval	= cpu_to_le32(666666),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(666666),
	.dwFrameInterval[1]	= cpu_to_le32(1000000),
	.dwFrameInterval[2]	= cpu_to_le32(5000000),
#elif defined CONFIG_UVC_FRMAE_RATE_30
	.dwDefaultFrameInterval = cpu_to_le32(333333),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(333333),
	.dwFrameInterval[1]	= cpu_to_le32(666666),
	.dwFrameInterval[2]	= cpu_to_le32(1000000),
#endif
};
#endif /* CONFIG_UVC_FRAME_SIZE_1080P */

#endif /* CONFIG_UVC_FORMAT_H264 */

#if defined CONFIG_UVC_FRAME_SIZE_1080P
DECLARE_UVC_STILL_IMAGE_FRAME_DESCRIPTOR(3, 0);
static const struct UVC_STILL_IMAGE_FRAME_DESCRIPTOR(3, 0)
	uvc_still_image_frame = {
	.bLength		= UVC_DT_STILL_IMAGE_FRAME_SIZE(3, 0),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_STILL_IMAGE_FRAME,
	.bEndpointAddress	= 0,
	.bNumImageSizePatterns	= 3,
	.wWidth_wHeight[0]	= {640, 360},
	.wWidth_wHeight[1]	= {1280, 720},
	.wWidth_wHeight[2]	= {1920, 1080},
	.bNumCompressionPattern = 0,
};
#elif defined CONFIG_UVC_FRAME_SIZE_720P
DECLARE_UVC_STILL_IMAGE_FRAME_DESCRIPTOR(2, 0);
static const struct UVC_STILL_IMAGE_FRAME_DESCRIPTOR(2, 0)
	uvc_still_image_frame = {
	.bLength		= UVC_DT_STILL_IMAGE_FRAME_SIZE(2, 0),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_STILL_IMAGE_FRAME,
	.bEndpointAddress	= 0,
	.bNumImageSizePatterns	= 2,
	.wWidth_wHeight[0]	= {640, 360},
	.wWidth_wHeight[1]	= {1280, 720},
	.bNumCompressionPattern = 0,
};
#endif

static const struct uvc_color_matching_descriptor uvc_color_matching = {
	.bLength		= UVC_DT_COLOR_MATCHING_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_COLORFORMAT,
	.bColorPrimaries	= 1,
	.bTransferCharacteristics	= 1,
	.bMatrixCoefficients	= 4,
};

static const struct uvc_descriptor_header * const uvc_fs_control_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_control_header,
	(const struct uvc_descriptor_header *) &uvc_camera_terminal,
	(const struct uvc_descriptor_header *) &uvc_processing,
	(const struct uvc_descriptor_header *) &uvc_output_terminal,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_control_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_control_header,
	(const struct uvc_descriptor_header *) &uvc_camera_terminal,
	(const struct uvc_descriptor_header *) &uvc_processing,
	(const struct uvc_descriptor_header *) &uvc_output_terminal,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_fs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
#if defined CONFIG_UVC_FORMAT_H264
	(const struct uvc_descriptor_header *) &uvc_format_framebased,
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_1080p,
#endif	/* CONFIG_UVC_FRAME_SIZE_1080P */
#elif defined CONFIG_UVC_FORMAT_MJPG
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
#endif	/* CONFIG_UVC_FRAME_SIZE_1080P */
#endif
	(const struct uvc_descriptor_header *) &uvc_color_matching,

	(const struct uvc_descriptor_header *) &uvc_format_yuv,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_1080p,
#endif
	(const struct uvc_descriptor_header *) &uvc_still_image_frame,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_hs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
#if defined CONFIG_UVC_FORMAT_H264
	(const struct uvc_descriptor_header *) &uvc_format_framebased,
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_1080p,
#endif	/* CONFIG_UVC_FRAME_SIZE_1080P */
#elif defined CONFIG_UVC_FORMAT_MJPG
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
#endif	/* CONFIG_UVC_FRAME_SIZE_1080P */
#endif
	(const struct uvc_descriptor_header *) &uvc_color_matching,

	(const struct uvc_descriptor_header *) &uvc_format_yuv,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_1080p,
#endif
	(const struct uvc_descriptor_header *) &uvc_still_image_frame,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
#if defined CONFIG_UVC_FORMAT_H264
	(const struct uvc_descriptor_header *) &uvc_format_framebased,
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_framebased_1080p,
#endif	/* CONFIG_UVC_FRAME_SIZE_1080P */
#elif defined CONFIG_UVC_FORMAT_MJPG
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_1080p,
#endif	/* CONFIG_UVC_FRAME_SIZE_1080P */
#endif
	(const struct uvc_descriptor_header *) &uvc_color_matching,

	(const struct uvc_descriptor_header *) &uvc_format_yuv,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
#if defined CONFIG_UVC_FRAME_SIZE_1080P
	(const struct uvc_descriptor_header *) &uvc_frame_yuv_1080p,
#endif
	(const struct uvc_descriptor_header *) &uvc_still_image_frame,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

/* --------------------------------------------------------------------------
 * USB configuration
 */

static int __init
webcam_config_bind(struct usb_configuration *c)
{
#if ((defined CONFIG_UVC_ADD_MSC) || \
	(defined CONFIG_UVC_ADD_ADB) || \
	(defined CONFIG_UVC_ADD_UAC_MIC))
	int ret;

	ret = uvc_bind_config(c, uvc_fs_control_cls, uvc_ss_control_cls,
		uvc_fs_streaming_cls, uvc_hs_streaming_cls,
		uvc_ss_streaming_cls);
	if (ret)
		goto out;

#ifdef CONFIG_UVC_ADD_UAC_MIC
	ret = audio_bind_config(c);
	if (ret)
		goto out;
#endif	/* CONFIG_UVC_ADD_UAC_MIC */

#ifdef CONFIG_UVC_ADD_MSC
	ret = msg_do_config(c);
	if (ret)
		goto out;
#endif	/* CONFIG_UVC_ADD_MSC */

#ifdef CONFIG_UVC_ADD_ADB
	ret = adb_bind_config(c);
	if (ret)
		goto out;
#endif	/* CONFIG_UVC_ADD_ADB */

out:
	return ret;
#else
	return uvc_bind_config(c, uvc_fs_control_cls, uvc_ss_control_cls,
		uvc_fs_streaming_cls, uvc_hs_streaming_cls,
		uvc_ss_streaming_cls);
#endif	/* CONFIG_UVC_ADD_MSC */
}

static struct usb_configuration webcam_config_driver = {
	.label			= webcam_config_label,
	.bConfigurationValue	= 1,
	.iConfiguration		= 0, /* dynamic */
	.bmAttributes		= USB_CONFIG_ATT_SELFPOWER,
	.MaxPower		= CONFIG_USB_GADGET_VBUS_DRAW,
};

static int /* __init_or_exit */
webcam_unbind(struct usb_composite_dev *cdev)
{
#ifdef CONFIG_UVC_ADD_UAC_MIC
	gaudio_cleanup();
#endif	/* CONFIG_UVC_ADD_UAC_MIC */

	return 0;
}

static int __init
webcam_bind(struct usb_composite_dev *cdev)
{
	int ret;

	/* Allocate string descriptor numbers ... note that string contents
	 * can be overridden by the composite_dev glue.
	 */
	ret = usb_string_ids_tab(cdev, webcam_strings);
	if (ret < 0)
		goto error;
	webcam_device_descriptor.iManufacturer =
		webcam_strings[USB_GADGET_MANUFACTURER_IDX].id;
	webcam_device_descriptor.iProduct =
		webcam_strings[USB_GADGET_PRODUCT_IDX].id;
	webcam_config_driver.iConfiguration =
		webcam_strings[STRING_DESCRIPTION_IDX].id;

	/* Register our configuration. */
	if ((ret = usb_add_config(cdev, &webcam_config_driver,
					webcam_config_bind)) < 0)
		goto error;

	usb_composite_overwrite_options(cdev, &coverwrite);
	INFO(cdev, "Webcam Video Gadget\n");
	return 0;

error:
	webcam_unbind(cdev);
	return ret;
}

/* --------------------------------------------------------------------------
 * Driver
 */

static __refdata struct usb_composite_driver webcam_driver = {
	.name		= "g_webcam",
	.dev		= &webcam_device_descriptor,
	.strings	= webcam_device_strings,
	.max_speed	= USB_SPEED_SUPER,
	.bind		= webcam_bind,
	.unbind		= webcam_unbind,
};

static int __init
webcam_init(void)
{
#ifdef CONFIG_UVC_ADD_ADB
	int ret;
	ret = adb_setup();
	if (ret)
		return -EFAULT;
#endif	/* CONFIG_UVC_ADD_ADB */

	return usb_composite_probe(&webcam_driver);
}

static void __exit
webcam_cleanup(void)
{
	usb_composite_unregister(&webcam_driver);

#ifdef CONFIG_UVC_ADD_ADB
	adb_cleanup();
#endif	/* CONFIG_UVC_ADD_ADB */

}

module_init(webcam_init);
module_exit(webcam_cleanup);

MODULE_AUTHOR("Laurent Pinchart");
MODULE_DESCRIPTION("Webcam Video Gadget");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1.0");

