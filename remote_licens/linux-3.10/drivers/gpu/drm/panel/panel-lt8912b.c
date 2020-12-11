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

#include <drm/drmP.h>
#include <drm/drm_panel.h>
#include "panel-simple.h"
#include "lombo_panel_config.h"

#define LANE_NUM	(4)

#if 1
#define WIDTH		(1280)
#define HEIGHT		(720)
#define VIC		(19)
#define HBP		(220)
#define HFP		(440)
#define HSW		(40)
#define HACT		(WIDTH)
#define VBP_F1		(20)
#define VFP_F1		(5)
#define VSW_F1		(5)
#define VACT_F1		(HEIGHT)
#define VT_F1		(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT_F2		(VBP_F2 + VFP_F2 + VSW_F2 + VACT_F2)
#define DCLK		(74250000)
#else
#define WIDTH		(1920)
#define HEIGHT		(1080)
#define VIC		(16)
#define HBP		(148)
#define HFP		(88)
#define HSW		(44)
#define HACT		(WIDTH)
#define VBP_F1		(36)
#define VFP_F1		(4)
#define VSW_F1		(5)
#define VACT_F1		(HEIGHT)
#define VT_F1		(VBP_F1 + VFP_F1 + VSW_F1 + VACT_F1)
#define VBP_F2		(0)
#define VFP_F2		(0)
#define VSW_F2		(0)
#define VACT_F2		(0)
#define VT_F2		((VBP_F2 + VFP_F2 + VSW_F2 + VACT_F2) * 2)
#define DCLK		(148500000)
#endif

#define DCS_MDELAY_FLAG (0)

static struct i2c_client *panel_i2c_client[3];

static u32 lt8912b_init_code[] = {
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
	3, 0x92, 0x36, (u8)((VT_F1) % 256), /* V_total[7:0] */
	3, 0x92, 0x37, (u8)((VT_F1) / 256), /* V_total[15:8] */
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

static ssize_t panel_lt8912b_wr_init_code(
			struct panel_simple_host *host)
{
	u32 i = 0;
	u32 len = (sizeof(lt8912b_init_code) / sizeof((lt8912b_init_code)[0]));
	u32 *ini_code = &lt8912b_init_code[0];
	u32 bytes = 0;

	DRM_DEBUG_KMS("%s,%d\n", __func__, __LINE__);

	while (i < len) {
		if (DCS_MDELAY_FLAG == ini_code[i]) {
			mdelay(ini_code[i + 1]);
			i += ini_code[i] + 2;
		} else {
			char buf[4];
			struct i2c_client *client =
				panel_get_i2c_client(ini_code[i + 1] >> 1);

			if (client) {
				buf[0] = ini_code[i + 2]; /* reg addr */
				buf[1] = ini_code[i + 3]; /* reg value */
				if (2 != i2c_master_send(client, buf, 2)) {
					DRM_ERROR("%d\n", __LINE__);
					return -1;
				}
				bytes += 2;	/*count for bytes send success*/

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
				pr_debug("ini_code[%d,%d,%d,%d], read_buf=%d\n",
					ini_code[i + 0],
					ini_code[i + 1],
					ini_code[i + 2],
					ini_code[i + 3],
					read_buf[0]);
				#endif
			} else {
				pr_err("%d: null i2c_client @%x\n",
					__LINE__, ini_code[i + 1] >> 1);
			}
			i += ini_code[i] + 1;
		}
	}
	return bytes;
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

static int panel_lt8912b_attach(void *param)
{
	DRM_DEBUG_KMS("%d\n", __LINE__);

	if (i2c_add_driver(&panel_i2c_driver)) {
		DRM_ERROR("failed to register panel i2c driver\n");
		return -ENOENT;
	}

	return 0;
}

static struct lombo_vo_dev_config lt8912b_vo = {
	.dev_if = VO_DEV_MIPI_DSI_VIDEO,
	.dsi_if = {
		.index = 0,
		.is_dual = false,
		.is_bta = false,
		.lane_num = 4,
		.tx_fmt = DSI_RGB888,
		.tx_mode = DSI_VIDEO_MODE,
		.tx_mode_cfg.video_mode.trans_mode = DSI_NON_BURST_PULSE,
		.is_bitrate_fixed = false,
		.bit_clk = 999000000,
	},

	.timing.is_interlace = false,
	.timing.field1.vt = VT_F1,
	.timing.field1.vact = VACT_F1,
	.timing.field1.vfp = VFP_F1,
	.timing.field1.vsw = VSW_F1,
	.timing.field2.vt = VT_F2,
	.timing.field2.vact = VACT_F2,
	.timing.field2.vfp = VFP_F2,
	.timing.field2.vsw = VSW_F2,
	.timing.hline.ht = HSW + HBP + HACT + HFP,
	.timing.hline.hact = HACT,
	.timing.hline.hfp = HFP,
	.timing.hline.hsw = HSW,
	.timing.width = WIDTH,
	.timing.height = HEIGHT,
	.timing.dclk_freq = DCLK,
	.format = TCON_FMT_RGB888,
};

static struct drm_display_mode lt8912b_mode = {
	/* dclk_freq */
	.clock = DCLK / 1000,
	/* width */
	.hdisplay = WIDTH,
	/* hsync_start = hdisplay + hfp */
	.hsync_start = WIDTH + HFP,
	/* hsync_end = hdisplay + hfp + hsw */
	.hsync_end = WIDTH + HFP + HSW,
	/* htotal = hdisplay + hfp + hsw + hbp */
	.htotal = WIDTH + HFP + HSW + HBP,
	/* height */
	.vdisplay = HEIGHT,
	/* vsync_start = vdisplay + vfp */
	.vsync_start = HEIGHT + VFP_F1,
	/* vsync_end = vdisplay + vfp + vsw */
	.vsync_end = HEIGHT + VFP_F1 + VSW_F1,
	/* vtotal = vdisplay + vfp + vsw + vbp */
	.vtotal = HEIGHT + VFP_F1 + VSW_F1 + VBP_F1,
	.vrefresh = 50,

	.private_size = sizeof(lt8912b_vo),
	.private = (int *)&lt8912b_vo,
};

static struct panel_delay lt8912b_delay = {
	.prepare = 50,
	.unprepare = 50,
	.enable = 50,
	.disable = 50,
	.power_on = 100,
	.reset = 100,
};

const struct panel_simple_desc lt8912b_desc = {
	.modes = &lt8912b_mode,
	.num_modes = 1,
	.bpc = 6,
	.delay = &lt8912b_delay,
	.init_panel = PANEL_SIMPLE_I2C,
	.size = {
		.width = 69,
		.height = 123,
	},
	.ops.attach = panel_lt8912b_attach,
	.ops.transfer = panel_lt8912b_wr_init_code,
};
