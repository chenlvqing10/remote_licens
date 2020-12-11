/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/backlight.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <uapi/linux/media-bus-format.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include "../lombo_legacy/lombo_doss.h"

/*
 * @INTERFACE
 * it can config value below, for reference only.
 * VO_DEV_PRGB, VO_DEV_SRGB_RGB, VO_DEV_SRGB_DUMMY_RGB, VO_DEV_SRGB_RGB_DUMMY,
 * VO_DEV_CPU_18BIT_RGB666,
 * VO_DEV_CPU_16BIT_RGB888, VO_DEV_CPU_16BIT_RGB666, VO_DEV_CPU_16BIT_RGB565,
 * VO_DEV_CPU_9BIT_RGB666,
 * VO_DEV_CPU_8BIT_RGB666, VO_DEV_CPU_8BIT_RGB565, VO_DEV_CPU_8BIT_RGB888,
 * VO_DEV_LVDS, VO_DEV_MIPI_DSI_VIDEO, VO_DEV_MIPI_DSI_CMD,
 * VO_DEV_BT601_24BIT, VO_DEV_BT601_16BIT, VO_DEV_BT601_8BIT,
 * VO_DEV_BT656_8BIT,
 * VO_DEV_BT1120_16BIT,VO_DEV_BT1120_8BIT
 * VO_DEV_TVE
 */
#define INTERFACE	(VO_DEV_MIPI_DSI_VIDEO)

/*
 * @FIFO_MODE
 * TCON_FIFO_NORMAL, TCON_FIFO_TWO_HALVES, TCON_FIFO_EC
 */
#define FIFO_MODE		(TCON_FIFO_NORMAL)

/*
 * @TCON_SRC
 * TCON_SRC_DISP_CTL, TCON_SRC_DMA, TCON_SRC_COLOR_BAR
 * TCON_SRC_GREY_SCALE,TCON_SRC_BLACK_WHITE,TCON_SRC_GRID
 * TCON_SRC_CHECKER_BOARD,TCON_SRC_BG_COLOR
 */
#define TCON_SRC		(TCON_SRC_COLOR_BAR)

/*
 * @TG_TRIG_SRC_MODE
 * TCON_TRIG_INT_HW, TCON_TRIG_INT_SW
 * TCON_TRIG_TE_HW, TCON_TRIG_TE_SW
 * TCON_TRIG_EXT_HW, TCON_TRIG_EXT_SW
 */
#define TG_TRIG_SRC_MODE	(TCON_TRIG_INT_HW)

/*
 * @VBI_TRIG
 * TCON_VBI_VFP, TCON_VBI_VSW, TCON_VBI_VBP, TCON_VBI_VACT
 */
#define VBI_TRIG		(TCON_VBI_VFP)

#define IS_RB_SWAP		(FALSE)

/* mipi dsi host config */
#define IS_MIPI_DUAL	(FALSE)
#define IS_BTA		(FALSE)
#define LANE_NUM	(4)
#define MIPI_TX_FMT	(DSI_RGB888)
#define MIPI_TX_MODE	(DSI_VIDEO_MODE)

/*
 * @TRANS_MODE
 * DSI_NON_BURST_PULSE, DSI_NON_BURST_EVENT, DSI_BURST
 */
#define TRANS_MODE	(DSI_NON_BURST_PULSE)
#define IS_FIXED_BITCLK	(FALSE)	  /* valid when TRANS_MODE == DSIBURST */
#define BITCLK (999000000) /* Unit: Hz, valid when IS_FIXED_BITCLK == TRUE */


/*
 * @COLOR_DEPTH
 * TCON_COLOR_24BPP, TCON_COLOR_18BPP, TCON_COLOR_16BPP
 */
#define COLOR_DEPTH	(TCON_COLOR_24BPP)

/*
 * TCON COMMON CONFIGURATION
 *
 * @TCON_FMT
 * it can config value below, for reference only.
 * TCON_FMT_RGB888, TCON_FMT_RGB666, TCON_FMT_RGB565
 * TCON_FMT_YUV444, TCON_FMT_YUV422,
 */
#define TCON_FMT	(TCON_FMT_RGB888)
#define IS_INTERLACE	(FALSE)
#define GAMMA_EN	(FALSE)
#define FRC_EN		(FALSE)
#define CSC_EN		(FALSE)
#define HAS_TE		(FALSE)
#define TE_POL		(TCON_IO_POS)

#if 1
#define WIDTH		(1280)
#define HEIGHT		(720)
#define VIC (19)

#define HBP		(220)
#define HFP		(440)
#define HSW		(40)
#define HACT		(WIDTH)
#define VBP_F1		(20)
#define VFP_F1		(5)
#define VSW_F1		(5)
#define VACT_F1		(HEIGHT)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT		((VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1) * 2)
#define DCLK		(74250000) /* 297/2 = 148.5MHz */
#else
#define WIDTH		(1920)
#define HEIGHT		(1080)
#define VIC (16)

#define HBP		(148)
#define HFP		(88)
#define HSW		(44)
#define HACT		(WIDTH)
#define VBP_F1		(36)
#define VFP_F1		(4)
#define VSW_F1		(5)
#define VACT_F1		(HEIGHT)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT		((VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1) * 2)
#define DCLK		(148500000) /* 297/2 = 148.5MHz */
#endif

/* DOSS TOP IO Config */
#define DCS_MDELAY_FLAG (0)

struct panel_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;
	const struct display_timing *timings;
	unsigned int num_timings;

	unsigned int bpc;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	/**
	 * @prepare: the time (in milliseconds) that it takes for the panel to
	 *           become ready and start receiving video data
	 * @enable: the time (in milliseconds) that it takes for the panel to
	 *          display the first valid frame after starting to receive
	 *          video data
	 * @disable: the time (in milliseconds) that it takes for the panel to
	 *           turn the display off (no content is visible)
	 * @unprepare: the time (in milliseconds) that it takes for the panel
	 *             to power itself down completely
	 */
	struct {
		unsigned int prepare;
		unsigned int enable;
		unsigned int disable;
		unsigned int unprepare;
	} delay;

	u32 bus_format;
};

struct panel_simple {
	struct drm_panel base;
	bool prepared;
	bool enabled;

	struct device *dev;
	const struct panel_desc *desc;

	struct backlight_device *backlight;
	struct regulator *supply;

	int reset_gpio;
	int power_gpio;
};

static struct i2c_client *panel_i2c_client[3];

static u32 gamma_tbl[256] = {
/* red   power = 2.2 */
/* green power = 1/2.2 */
/* blue  power = 1.4 */
0x000000, 0x001400, 0x001c00, 0x002100, 0x002600, 0x002a01, 0x002e01, 0x003101,
0x003402, 0x003702, 0x003a02, 0x003d03, 0x003f03, 0x004203, 0x004404, 0x004604,
0x004805, 0x004a05, 0x004c06, 0x004e06, 0x005007, 0x015207, 0x015308, 0x015508,
0x015709, 0x015809, 0x015a0a, 0x015c0a, 0x015d0b, 0x025f0c, 0x02600c, 0x02620d,
0x02630d, 0x02640e, 0x03660f, 0x03670f, 0x036810, 0x036a11, 0x036b11, 0x046c12,
0x046e13, 0x046f13, 0x047014, 0x057115, 0x057215, 0x057416, 0x057517, 0x067617,
0x067718, 0x067819, 0x07791a, 0x077a1a, 0x077c1b, 0x087d1c, 0x087e1c, 0x087f1d,
0x09801e, 0x09811f, 0x098220, 0x0a8320, 0x0a8421, 0x0a8522, 0x0b8623, 0x0b8723,
0x0c8824, 0x0c8925, 0x0c8a26, 0x0d8b27, 0x0d8c28, 0x0e8d28, 0x0e8d29, 0x0f8e2a,
0x0f8f2b, 0x10902c, 0x10912d, 0x11922d, 0x11932e, 0x12942f, 0x129530, 0x139631,
0x139632, 0x149733, 0x149834, 0x159934, 0x169a35, 0x169b36, 0x179b37, 0x179c38,
0x189d39, 0x199e3a, 0x199f3b, 0x1a9f3c, 0x1aa03d, 0x1ba13e, 0x1ca23e, 0x1ca33f,
0x1da340, 0x1ea441, 0x1ea542, 0x1fa643, 0x20a644, 0x21a745, 0x21a846, 0x22a947,
0x23a948, 0x24aa49, 0x24ab4a, 0x25ac4b, 0x26ac4c, 0x27ad4d, 0x27ae4e, 0x28af4f,
0x29af50, 0x2ab051, 0x2bb152, 0x2cb153, 0x2cb254, 0x2db355, 0x2eb456, 0x2fb457,
0x30b558, 0x31b659, 0x32b65a, 0x33b75b, 0x33b85c, 0x34b85d, 0x35b95e, 0x36ba5f,
0x37ba61, 0x38bb62, 0x39bc63, 0x3abc64, 0x3bbd65, 0x3cbe66, 0x3dbe67, 0x3ebf68,
0x3fc069, 0x40c06a, 0x41c16b, 0x42c16c, 0x43c26d, 0x44c36f, 0x46c370, 0x47c471,
0x48c572, 0x49c573, 0x4ac674, 0x4bc675, 0x4cc776, 0x4dc877, 0x4ec879, 0x50c97a,
0x51c97b, 0x52ca7c, 0x53cb7d, 0x54cb7e, 0x56cc7f, 0x57cc81, 0x58cd82, 0x59ce83,
0x5bce84, 0x5ccf85, 0x5dcf86, 0x5ed088, 0x60d189, 0x61d18a, 0x62d28b, 0x64d28c,
0x65d38d, 0x66d38f, 0x68d490, 0x69d591, 0x6ad592, 0x6cd693, 0x6dd695, 0x6ed796,
0x70d797, 0x71d898, 0x73d999, 0x74d99b, 0x75da9c, 0x77da9d, 0x78db9e, 0x7adba0,
0x7bdca1, 0x7ddca2, 0x7edda3, 0x80dda4, 0x81dea6, 0x83dfa7, 0x84dfa8, 0x86e0a9,
0x87e0ab, 0x89e1ac, 0x8be1ad, 0x8ce2ae, 0x8ee2b0, 0x8fe3b1, 0x91e3b2, 0x93e4b3,
0x94e4b5, 0x96e5b6, 0x98e5b7, 0x99e6b9, 0x9be6ba, 0x9de7bb, 0x9ee7bc, 0xa0e8be,
0xa2e8bf, 0xa3e9c0, 0xa5e9c2, 0xa7eac3, 0xa9eac4, 0xaaebc5, 0xacebc7, 0xaeecc8,
0xb0ecc9, 0xb1edcb, 0xb3edcc, 0xb5eecd, 0xb7eecf, 0xb9efd0, 0xbbefd1, 0xbcf0d3,
0xbef0d4, 0xc0f1d5, 0xc2f1d7, 0xc4f2d8, 0xc6f2d9, 0xc8f3db, 0xcaf3dc, 0xccf4dd,
0xcef4df, 0xd0f5e0, 0xd2f5e1, 0xd4f6e3, 0xd6f6e4, 0xd8f7e5, 0xdaf7e7, 0xdcf8e8,
0xdef8e9, 0xe0f9eb, 0xe2f9ec, 0xe4faed, 0xe6faef, 0xe8faf0, 0xeafbf2, 0xecfbf3,
0xeefcf4, 0xf0fcf6, 0xf2fdf7, 0xf5fdf9, 0xf7fefa, 0xf9fefb, 0xfbfffd, 0xfdfffe,
};

static u32 init_code[] = {
	/* DigitalClockEn(); */
	/* define _HDMI_Output_ */
	3, 0x90, 0x08, 0xff, /* Register address: 0x08; Value: 0xff */
	3, 0x90, 0x09, 0x81,
	3, 0x90, 0x0a, 0xff,
	3, 0x90, 0x0b, 0x64,
	3, 0x90, 0x0c, 0xff,
	3, 0x90, 0x44, 0x31,/* Close LVDS ouput */
	3, 0x90, 0x51, 0x1f,

	/* TxAnalog(); */
	3, 0x90, 0x31, 0xa1,
	3, 0x90, 0x32, 0xbf,
	/* 0x03 Open HDMI Tx, 0x00 Close HDMI Tx */
	3, 0x90, 0x33, 0x17,
	3, 0x90, 0x37, 0x00,
	3, 0x90, 0x38, 0x22,
	3, 0x90, 0x60, 0x82,

	3, 0x90, 0x3a, 0x00,

	/* CbusAnalog(); */
	3, 0x90, 0x39, 0x45,
	3, 0x90, 0x3b, 0x00,

	/* HDMIPllAnalog(); */
	3, 0x90, 0x44, 0x31,
	3, 0x90, 0x55, 0x44,
	3, 0x90, 0x57, 0x01,
	3, 0x90, 0x5a, 0x02,

	/* MipiBasicSet(); */
	3, 0x92, 0x10, 0x01, /* 0x05 */
	3, 0x92, 0x11, 0x08, /* 0x12 */
	3, 0x92, 0x12, 0x04,
	/* 00 4 lane , 01 lane , 02 2 lane , 03 3 lane */
	3, 0x92, 0x13, (LANE_NUM % 0x04),
	3, 0x92, 0x14, 0x00,
	3, 0x92, 0x15, 0x00,
	3, 0x92, 0x1a, 0x03,
	3, 0x92, 0x1b, 0x03,

	/* set MIPI Timing */
	/* MIPIDig(); */
	3, 0x92, 0x18, (u8)(HSW % 256), /* hsync  */
	3, 0x92, 0x19, (u8)(VSW_F1 % 256), /* vsync  */
	3, 0x92, 0x1c, (u8)(HACT % 256), /* H_active[7:0]  */
	3, 0x92, 0x1d, (u8)(HACT / 256), /* H_active[15:8]  */

	3, 0x92, 0x1e, 0x67, /* hs/vs/de pol hdmi sel pll sel  */
	3, 0x92, 0x2f, 0x0c, /* fifo_buff_length 12  */

	3, 0x92, 0x34, (u8)((HBP + HFP + HSW + HACT) % 256), /* H_total[7:0] */
	3, 0x92, 0x35, (u8)((HBP + HFP + HSW + HACT) / 256), /* H_total[15:8] */
	3, 0x92, 0x36, (u8)((VT >> 1) % 256), /* V_total[7:0] */
	3, 0x92, 0x37, (u8)((VT >> 1) / 256), /* V_total[15:8] */
	3, 0x92, 0x38, (u8)(VBP_F1 % 256), /* VBP[7:0] */
	3, 0x92, 0x39, (u8)(VBP_F1 / 256), /* VBP[15:8] */
	3, 0x92, 0x3a, (u8)(VFP_F1 % 256), /* VFP[7:0] */
	3, 0x92, 0x3b, (u8)(VFP_F1 / 256), /* VFP[15:8] */
	3, 0x92, 0x3c, (u8)(HBP % 256), /* HBP[7:0] */
	3, 0x92, 0x3d, (u8)(HBP / 256), /* HBP[15:8] */
	3, 0x92, 0x3e, (u8)(HFP % 256), /* HFP[7:0] */
	3, 0x92, 0x3f, (u8)(HFP / 256), /* HFP[15:8] */

	/* DDSConfig(); */
	3, 0x92, 0x4e, 0x52,
	3, 0x92, 0x4f, 0xde,
	3, 0x92, 0x50, 0xc0,
	3, 0x92, 0x51, 0x80,
	3, 0x92, 0x51, 0x00,
	3, 0x92, 0x1f, 0x5e,
	3, 0x92, 0x20, 0x01,
	3, 0x92, 0x21, 0x2c,
	3, 0x92, 0x22, 0x01,
	3, 0x92, 0x23, 0xfa,
	3, 0x92, 0x24, 0x00,
	3, 0x92, 0x25, 0xc8,
	3, 0x92, 0x26, 0x00,
	3, 0x92, 0x27, 0x5e,
	3, 0x92, 0x28, 0x01,
	3, 0x92, 0x29, 0x2c,
	3, 0x92, 0x2a, 0x01,
	3, 0x92, 0x2b, 0xfa,
	3, 0x92, 0x2c, 0x00,
	3, 0x92, 0x2d, 0xc8,
	3, 0x92, 0x2e, 0x00,

	3, 0x90, 0x03, 0x7f,
	DCS_MDELAY_FLAG, 100,
	3, 0x90, 0x03, 0xff,

	3, 0x92, 0x42, 0x64,
	3, 0x92, 0x43, 0x00,
	3, 0x92, 0x44, 0x04,
	3, 0x92, 0x45, 0x00,
	3, 0x92, 0x46, 0x59,
	3, 0x92, 0x47, 0x00,
	3, 0x92, 0x48, 0xf2,
	3, 0x92, 0x49, 0x06,
	3, 0x92, 0x4a, 0x00,
	3, 0x92, 0x4b, 0x72,
	3, 0x92, 0x4c, 0x45,
	3, 0x92, 0x4d, 0x00,
	3, 0x92, 0x52, 0x08,
	3, 0x92, 0x53, 0x00,
	3, 0x92, 0x54, 0xb2,
	3, 0x92, 0x55, 0x00,
	3, 0x92, 0x56, 0xe4,
	3, 0x92, 0x57, 0x0d,
	3, 0x92, 0x58, 0x00,
	3, 0x92, 0x59, 0xe4,
	3, 0x92, 0x5a, 0x8a,
	3, 0x92, 0x5b, 0x00,
	3, 0x92, 0x5c, 0x34,
	3, 0x92, 0x1e, 0x4f,
	3, 0x92, 0x51, 0x00,

	/* AudioIIsEn();  IIS Input */
	3, 0x90, 0xB2, 0x01,

	3, 0x94, 0x06, 0x08,
	3, 0x94, 0x07, 0xF0,
	3, 0x94, 0x34, 0xE2,
	3, 0x94, 0x3c, 0x41,


	/* Packet config() */
	3, 0x94, 0x3e, 0x0A,
	/*
	 * reg[0x43] is checksums, it will be changed
	 * after reg[0x45] or reg[0x47] changed.
	 * regs 0x43, 0x44, 0x45, 0x47 total sum is 0x6F.
	 */
	3, 0x94, 0x43, 0x25, /* avi packet checksum ,avi_pb0 */
	3, 0x94, 0x44, 0x10, /* avi packet output RGB0x10 */
	3, 0x94, 0x45, 0x2A, /*0x19:4:3 ; 0x2A : 16:9 */
	3, 0x94, 0x47, VIC, /* vic ,0x10: 1080P ;  0x04 : 720P */

	/* MIPIRxLogicRes(); */
	3, 0x90, 0x03, 0x7f,
	DCS_MDELAY_FLAG, 100,
	3, 0x90, 0x03, 0xff,

	/* MIPIRxLogicRes(); */
	3, 0x92, 0x51, 0x80,
	DCS_MDELAY_FLAG, 100,
	3, 0x92, 0x51, 0x00,

};

static void dev_config(vo_device_t *vo_dev)
{
	tcon_mipi_dsi_if_t *dsi_if;
	tcon_host_t *tcon_host;

	DRM_DEBUG_KMS("%s [%d]\n", __FILE__, __LINE__);

	/* device interface, reference to vo_dev_if_t */
	vo_dev->dev_if = INTERFACE;

	/* device specific configuation, reference to vo_if_cfg_t */
	dsi_if = &vo_dev->if_cfg.dsi_if;
	dsi_if->index = 0;
	dsi_if->tcon_host = vo_dev->tcon_host;
	dsi_if->is_dual = IS_MIPI_DUAL;
	dsi_if->is_bta = IS_BTA;
	dsi_if->lane_num = LANE_NUM;
	dsi_if->tx_fmt = MIPI_TX_FMT;
	dsi_if->tx_mode = MIPI_TX_MODE;
	dsi_if->tx_mode_cfg.video_mode.is_bitrate_fixed = IS_FIXED_BITCLK;
	dsi_if->tx_mode_cfg.video_mode.bit_clk = BITCLK;
	dsi_if->tx_mode_cfg.video_mode.trans_mode = TRANS_MODE;

	/* tcon host config */
	tcon_host = vo_dev->tcon_host;
	tcon_host->fifo_mode = FIFO_MODE;
	tcon_host->tcon_src = TCON_SRC;
	tcon_host->tcon_trig_src_mode = TG_TRIG_SRC_MODE;
	tcon_host->trig_vbi = VBI_TRIG;
	tcon_host->is_rb_swap = IS_RB_SWAP;
	/* timing, reference to tcon_timings_t */
	tcon_host->timing.is_interlace = IS_INTERLACE;
	tcon_host->timing.field1.vt = VT;
	tcon_host->timing.field1.vact = VACT_F1;
	tcon_host->timing.field1.vfp = VFP_F1;
	tcon_host->timing.field1.vsw = VSW_F1;
	tcon_host->timing.field2.vt = (VSW_F2 + VBP_F2 + VACT_F2 + VFP_F2) << 1;
	tcon_host->timing.field2.vact = VACT_F2;
	tcon_host->timing.field2.vfp = VFP_F2;
	tcon_host->timing.field2.vsw = VSW_F2;
	tcon_host->timing.hline.ht = HSW + HBP + HACT + HFP;
	tcon_host->timing.hline.hact = HACT;
	tcon_host->timing.hline.hfp = HFP;
	tcon_host->timing.hline.hsw = HSW;
	tcon_host->timing.dclk_freq = DCLK;
	/* resolution in pixel */
	tcon_host->timing.width = WIDTH;
	tcon_host->timing.height = HEIGHT;
	/* format, reference to tcon_output_fmt_t */
	tcon_host->format = TCON_FMT;
	/* gamma correction */
	tcon_host->gamma_en = GAMMA_EN;
	tcon_host->gamma_tbl = gamma_tbl;
	/* frame rate control (dither) enable */
	tcon_host->frc_en = FRC_EN;
	/* TODO */
	tcon_host->csc_en = CSC_EN;
	/* TE mode */
	tcon_host->has_te = HAS_TE;
	tcon_host->te_pol = TE_POL;
}

static struct i2c_client *panel_get_i2c_client(const unsigned short i2c_addr)
{
	const __u32 num = ARRAY_SIZE(panel_i2c_client);
	__u32 i;

	for (i = 0; i < num; i++) {
		struct i2c_client *client = panel_i2c_client[i];

		if (client && (client->addr == i2c_addr))
			return client;
	}

	return NULL;
}

static s32 panel_i2c_write_reg_array(u32 tidx, u32 *reg, u32 len)
{
	u32 i = 0;

	DRM_DEBUG_KMS("%s,%d\n", __func__, __LINE__);

	while (i < len) {
		if (DCS_MDELAY_FLAG == reg[i]) {
			mdelay(reg[i + 1]);
			i += reg[i] + 2;
		} else {
			char buf[4];
			struct i2c_client *client =
				panel_get_i2c_client(reg[i + 1] >> 1);

			if (client) {
				buf[0] = reg[i + 2]; /* reg addr */
				buf[1] = reg[i + 3]; /* reg value */
				if (2 != i2c_master_send(client, buf, 2)) {
					DRM_ERROR("%d\n", __LINE__);
					return -1;
				}

				#if 0
				char read_buf[4];
				if (1 != i2c_master_send(client, &buf[0], 1)) {
					DRM_ERROR("%d\n", __LINE__);
					return -1;
				}
				read_buf[0] = 0;
				if (1 != i2c_master_recv(client, read_buf, 1)) {
					DRM_ERROR("%d\n", __LINE__);
					return -1;
				}
				pr_debug("reg[%d,%d,%d,%d], read_buf=%d\n",
					reg[i + 0],
					reg[i + 1],
					reg[i + 2],
					reg[i + 3],
					read_buf[0]);
				#endif
			} else {
				pr_err("%d: null i2c_client @%x\n",
					__LINE__, reg[i + 1] >> 1);
			}
			i += reg[i] + 1;
		}
	}
	return 0;
}

static void dev_init(vo_device_t *vo_dev)
{
	panel_i2c_write_reg_array(3, init_code, ARRAY_SIZE(init_code));
}

static void dev_exit(vo_device_t *vo_dev)
{
	return;
}

void tcon_dev_probe(vo_device_t *vo_dev)
{
	DRM_DEBUG_KMS("[%d]\n", __LINE__);

	vo_dev->dev_ops.dev_init = dev_init;
	vo_dev->dev_ops.dev_exit = dev_exit;
	dev_config(vo_dev);
}

static inline struct panel_simple *to_panel_simple(struct drm_panel *panel)
{
	return container_of(panel, struct panel_simple, base);
}

static int panel_simple_get_fixed_modes(struct panel_simple *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	unsigned int i, num = 0;

	if (!panel->desc)
		return 0;

	for (i = 0; i < panel->desc->num_timings; i++) {
		const struct display_timing *dt = &panel->desc->timings[i];
		struct videomode vm;

		videomode_from_timing(dt, &vm);
		mode = drm_mode_create(drm);
		if (!mode) {
			dev_err(drm->dev, "failed to add mode %ux%u\n",
				dt->hactive.typ, dt->vactive.typ);
			continue;
		}

		drm_display_mode_from_videomode(&vm, mode);
		drm_mode_set_name(mode);

		drm_mode_probed_add(connector, mode);
		num++;
	}

	for (i = 0; i < panel->desc->num_modes; i++) {
		const struct drm_display_mode *m = &panel->desc->modes[i];

		mode = drm_mode_duplicate(drm, m);
		if (!mode) {
			dev_err(drm->dev, "failed to add mode %ux%u@%u\n",
				m->hdisplay, m->vdisplay, m->vrefresh);
			continue;
		}

		drm_mode_set_name(mode);

		drm_mode_probed_add(connector, mode);
		num++;
	}

	connector->display_info.bpc = panel->desc->bpc;
	connector->display_info.width_mm = panel->desc->size.width;
	connector->display_info.height_mm = panel->desc->size.height;

	/* TODO */
	/* if (panel->desc->bus_format)                                    */
	/*	drm_display_info_set_bus_formats(&connector->display_info, */
	/*				 &panel->desc->bus_format, 1); */

	return num;
}

static int panel_simple_of_get_native_mode(struct panel_simple *panel)
{
	struct drm_connector *connector = panel->base.connector;
	struct drm_device *drm = panel->base.drm;
	struct drm_display_mode *mode;
	struct device_node *timings_np;
	int ret;

	timings_np = of_get_child_by_name(panel->dev->of_node,
					  "display-timings");
	if (!timings_np) {
		dev_dbg(panel->dev, "failed to find display-timings node\n");
		return 0;
	}

	of_node_put(timings_np);
	mode = drm_mode_create(drm);
	if (!mode)
		return 0;

	ret = of_get_drm_display_mode(panel->dev->of_node, mode,
				      OF_USE_NATIVE_MODE);
	if (ret) {
		dev_dbg(panel->dev, "failed to find dts display timings\n");
		drm_mode_destroy(drm, mode);
		return 0;
	}

	drm_mode_set_name(mode);
	mode->type |= DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static int panel_simple_disable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (!p->enabled)
		return 0;

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_POWERDOWN;
		backlight_update_status(p->backlight);
	}

	if (p->desc && p->desc->delay.disable)
		msleep(p->desc->delay.disable);

	p->enabled = false;

	return 0;
}

static int panel_simple_unprepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (!p->prepared)
		return 0;

	if (p->reset_gpio)
		gpio_direction_output(p->reset_gpio, 0);

	if (p->power_gpio)
		gpio_direction_output(p->power_gpio, 0);

	regulator_disable(p->supply);

	if (p->desc && p->desc->delay.unprepare)
		msleep(p->desc->delay.unprepare);

	p->prepared = false;

	return 0;
}

static int panel_simple_prepare(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	int err;

	DRM_DEBUG_KMS("[%d] p->prepared:%d\n", __LINE__, p->prepared);

	if (p->prepared)
		return 0;

	err = regulator_enable(p->supply);
	if (err < 0) {
		dev_err(panel->dev, "failed to enable supply: %d\n", err);
		return err;
	}

	if (p->power_gpio) {
		gpio_direction_output(p->power_gpio, 1);
		msleep(100);
	}

	if (p->reset_gpio) {
		gpio_direction_output(p->reset_gpio, 1);
		msleep(100);
		gpio_direction_output(p->reset_gpio, 0);
		msleep(100);
		gpio_direction_output(p->reset_gpio, 1);
		msleep(100);
	}

	if (p->desc && p->desc->delay.prepare)
		msleep(p->desc->delay.prepare);

	p->prepared = true;

	return 0;
}

static int panel_simple_enable(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	if (p->enabled)
		return 0;

	if (p->desc && p->desc->delay.enable)
		msleep(p->desc->delay.enable);

	if (p->backlight) {
		p->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(p->backlight);
	}

	p->enabled = true;

	return 0;
}

static int panel_simple_get_modes(struct drm_panel *panel)
{
	struct panel_simple *p = to_panel_simple(panel);
	int num = 0;

	DRM_DEBUG_KMS("[%d] p->enabled:%d\n", __LINE__, p->enabled);

	/* add device node plane modes */
	num += panel_simple_of_get_native_mode(p);

	/* add hard-coded panel modes */
	num += panel_simple_get_fixed_modes(p);

	return num;
}

static int panel_simple_get_timings(struct drm_panel *panel,
				    unsigned int num_timings,
				    struct display_timing *timings)
{
	struct panel_simple *p = to_panel_simple(panel);
	unsigned int i;

	if (!p->desc)
		return 0;

	if (p->desc->num_timings < num_timings)
		num_timings = p->desc->num_timings;

	if (timings)
		for (i = 0; i < num_timings; i++)
			timings[i] = p->desc->timings[i];

	return p->desc->num_timings;
}

static int panel_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	__u32 i;

	if (!client) {
		DRM_ERROR("%d: null i2c_client\n", __LINE__);
		return -1;
	}

	panel_i2c_client[0] = client;
	for (i = 1; i < ARRAY_SIZE(panel_i2c_client); i++) {
		panel_i2c_client[i] = i2c_new_dummy(client->adapter,
					client->addr + i);
		if (!panel_i2c_client[i]) {
			dev_err(&client->adapter->dev,
				"address 0x%02x unavailable\n",
				client->addr + i);
			return -EADDRINUSE;
		} else {
			dev_info(&client->adapter->dev,
				"i2c_new_dummy 0x%02x 0x%02x ok\n",
				client->addr + i, panel_i2c_client[i]->addr);
		}
	}

	dev_info(&client->adapter->dev,
		"attached panel lt8912b into i2c adapter successfully\n");

	return 0;
}

static int panel_i2c_remove(struct i2c_client *client)
{
	dev_info(&client->adapter->dev,
		"detached panel lt8912b from i2c adapter successfully\n");

	return 0;
}

static const struct i2c_device_id panel_i2c_id[] = {
	{ "lt8912b_i2c", 0 },
	{ },
};

static struct of_device_id panel_i2c_match_types[] = {
	{
		.compatible = "lt8912b_i2c",
	}, {
		/* end node */
	}
};

struct i2c_driver panel_i2c_driver = {
	.driver = {
		.name = "lt8912b_i2c",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(panel_i2c_match_types),
	},
	.id_table = panel_i2c_id,
	.probe = panel_i2c_probe,
	.remove = panel_i2c_remove,
	.command = NULL,
};

static const struct drm_panel_funcs panel_simple_funcs = {
	.disable = panel_simple_disable,
	.unprepare = panel_simple_unprepare,
	.prepare = panel_simple_prepare,
	.enable = panel_simple_enable,
	.get_modes = panel_simple_get_modes,
	.get_timings = panel_simple_get_timings,
};

static int panel_simple_probe(struct device *dev, const struct panel_desc *desc)
{
/* TODO */
#if 0
	struct device_node *backlight, *ddc;
#endif
	struct device_node *np = dev->of_node;
	struct panel_simple *panel;
	struct panel_desc *of_desc;
	enum of_gpio_flags flags;
	u32 val;
	int err;

	dev_dbg(dev, "%s,%d\n", __FILE__, __LINE__);

	panel = devm_kzalloc(dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;

	if (i2c_add_driver(&panel_i2c_driver)) {
		DRM_ERROR("failed to register panel i2c driver\n");
		return -ENOENT;
	}

	panel->power_gpio = of_get_named_gpio_flags(
		np, "power_gpio", 0, &flags);
	DRM_DEBUG_KMS("power_gpio %d, flags 0x%x\n", panel->power_gpio, flags);
	if (!gpio_is_valid(panel->power_gpio)) {
		DRM_ERROR("power_gpio %d invalid\n", panel->power_gpio);
		panel->power_gpio = 0;
	} else {
		err = devm_gpio_request(dev, panel->power_gpio, "power_gpio");
		if (err < 0) {
			DRM_ERROR("power_gpio %d request failed\n",
				panel->power_gpio);
			panel->power_gpio = 0;
		}
	}
	if (panel->power_gpio)
		gpio_direction_output(panel->power_gpio, 0);

	panel->reset_gpio = of_get_named_gpio_flags(
		np, "reset_gpio", 0, &flags);
	DRM_DEBUG_KMS("reset_gpio %d, flags 0x%x\n", panel->reset_gpio, flags);
	if (!gpio_is_valid(panel->reset_gpio)) {
		DRM_ERROR("reset_gpio %d invalid\n", panel->reset_gpio);
		panel->reset_gpio = 0;
	} else {
		err = devm_gpio_request(dev, panel->reset_gpio, "reset_gpio");
		if (err < 0) {
			DRM_ERROR("power_gpio %d request failed\n",
				panel->reset_gpio);
			panel->reset_gpio = 0;
		}
	}
	if (panel->reset_gpio)
		gpio_direction_output(panel->reset_gpio, 0);

	if (!desc)
		of_desc = devm_kzalloc(dev, sizeof(*of_desc), GFP_KERNEL);
	else
		of_desc = devm_kmemdup(dev, desc, sizeof(*of_desc), GFP_KERNEL);

	if (!of_property_read_u32(dev->of_node, "bus-format", &val))
		of_desc->bus_format = val;
	if (!of_property_read_u32(dev->of_node, "delay,prepare", &val))
		of_desc->delay.prepare = val;
	if (!of_property_read_u32(dev->of_node, "delay,enable", &val))
		of_desc->delay.enable = val;
	if (!of_property_read_u32(dev->of_node, "delay,disable", &val))
		of_desc->delay.disable = val;
	if (!of_property_read_u32(dev->of_node, "delay,unprepare", &val))
		of_desc->delay.unprepare = val;

	panel->enabled = false;
	panel->prepared = false;
	panel->desc = of_desc;
	panel->dev = dev;

	drm_panel_init(&panel->base);
	panel->base.dev = dev;
	panel->base.funcs = &panel_simple_funcs;

	err = drm_panel_add(&panel->base);
	if (err < 0)
		goto free_ddc;

	dev_set_drvdata(dev, panel);

	DRM_DEBUG_KMS("[%d] over.\n", __LINE__);

	return 0;

free_ddc:

/* TODO */
#if 0
free_backlight:
	if (panel->backlight)
		put_device(&panel->backlight->dev);
#endif

	return err;
}

static int panel_simple_remove(struct device *dev)
{
	struct panel_simple *panel = dev_get_drvdata(dev);

	drm_panel_detach(&panel->base);
	drm_panel_remove(&panel->base);

	panel_simple_disable(&panel->base);

	if (panel->backlight)
		put_device(&panel->backlight->dev);

	return 0;
}

static void panel_simple_shutdown(struct device *dev)
{
	struct panel_simple *panel = dev_get_drvdata(dev);

	panel_simple_disable(&panel->base);
}

/* mipi dsi 1080x1920 LCD test */
static const struct drm_display_mode lt8912b_mode = {
	/* dclk_freq */
	.clock = DCLK,
	/* width */
	.hdisplay = WIDTH,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = HACT + HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = HACT + HFP + HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = HACT + HFP + HSW + HBP,
	/* height */
	.vdisplay = HEIGHT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = VACT_F1 + VFP_F1,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = VACT_F1 + VFP_F1 + VSW_F1,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = VACT_F1 + VFP_F1 + VSW_F1 + VBP_F1,
	.vrefresh = 50,
};

static const struct panel_desc lt8912b = {
	.modes = &lt8912b_mode,
	.num_modes = 1,
	.bpc = 6,
	.size = {
		.width = WIDTH,
		.height = HEIGHT,
	},
};

static const struct of_device_id platform_of_match[] = {
	{
		.compatible = "lt8912b",
		.data = &lt8912b,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, platform_of_match);

static int panel_simple_platform_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;

	dev_dbg(&pdev->dev, "%d\n", __LINE__);
	id = of_match_node(platform_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return panel_simple_probe(&pdev->dev, id->data);
}

static int panel_simple_platform_remove(struct platform_device *pdev)
{
	return panel_simple_remove(&pdev->dev);
}

static void panel_simple_platform_shutdown(struct platform_device *pdev)
{
	panel_simple_shutdown(&pdev->dev);
}

static struct platform_driver panel_simple_platform_driver = {
	.driver = {
		.name = "lt8912b",
		.of_match_table = platform_of_match,
	},
	.probe = panel_simple_platform_probe,
	.remove = panel_simple_platform_remove,
	.shutdown = panel_simple_platform_shutdown,
};

static int __init panel_simple_init(void)
{
	int err;

	DRM_DEBUG_KMS("%s,%d\n", __FILE__, __LINE__);
	err = platform_driver_register(&panel_simple_platform_driver);
	if (err < 0)
		return err;

	return 0;
}
arch_initcall(panel_simple_init);

static void __exit panel_simple_exit(void)
{
	platform_driver_unregister(&panel_simple_platform_driver);
}
module_exit(panel_simple_exit);

MODULE_AUTHOR("Thierry Reding <treding@nvidia.com>");
MODULE_DESCRIPTION("DRM Driver for Simple Panels");
MODULE_LICENSE("GPL and additional rights");
