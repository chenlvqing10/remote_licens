/*
 * Copyright (C) 2005-2006 Micronas USA Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (Version 2) as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>

#include <media/v4l2-subdev.h>
#include <media/lombo_viss.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/viss_isp.h>

MODULE_DESCRIPTION("GC2145 sensor driver");
MODULE_LICENSE("GPL v2");

#include <mach/debug.h>

/* #define GC2145_1600x1200_IMAGE */

#define GC2145_800x600_WIDTH		800
#define GC2145_800x600_HEIGHT		600

#define GC2145_1600x1200_WIDTH		1600
#define GC2145_1600x1200_HEIGHT		1200

#define GC2145MIPI_2Lane

static struct i2c_client *gc2145_client;

typedef struct gc2145_config {
	struct v4l2_subdev sd;
	struct media_pad pad;
#if 0
	enum v4l2_mbus_type bus_type;

	/* External master clock frequency */
	unsigned long mclk_frequency;
#endif
	/* Protects the struct fields below */
	struct mutex lock;

	struct i2c_client *client;

	/* Exposure row interval in us */
	unsigned int exp_row_interval;

	unsigned short id;

	u8 mirror;
	u8 skip_detect;

	/* YUYV sequence (pixel format) control register */
	u8 tslb_reg;
	struct v4l2_mbus_framefmt format;

	int reset_gpio;
	int pwdn_gpio;
	int bsel_gpio;

#if 0
	int streaming;
	int power;
#endif
} gc2145_config_t;

typedef struct sensor_reg {
	u8 reg_add;
	u8 reg_value;
} sensor_reg_t;

static void gc2145_reset_cycle(gc2145_config_t *gc2145);
static void gc2145_pwdn(gc2145_config_t *gc2145);

/*
 * 2lane YUV init
 */
static const sensor_reg_t gc2145_reg_list[] = {
	{0xfe, 0xf0},
	{0xfe, 0xf0},
	{0xfe, 0xf0},
	{0xfc, 0x06},
	{0xf6, 0x00},
	{0xf7, 0x1d},
	{0xf8, 0x84},
	{0xfa, 0x00},
	{0xf9, 0x8e},
	{0xf2, 0x00},
	/* ISP reg */
	{0xfe, 0x00},
	{0x03, 0x04},
	{0x04, 0xe2},
	{0x09, 0x00},
	{0x0a, 0x00},
	{0x0b, 0x00},
	{0x0c, 0x00},
	{0x0d, 0x04},
	{0x0e, 0xc0},
	{0x0f, 0x06},
	{0x10, 0x52},
	{0x12, 0x2e},
	{0x17, 0x16}, /* mirror */
	{0x18, 0x22},
	{0x19, 0x0e},
	{0x1a, 0x01},
	{0x1b, 0x4b},
	{0x1c, 0x07},
	{0x1d, 0x10},
	{0x1e, 0x88},
	{0x1f, 0x78},
	{0x20, 0x03},
	{0x21, 0x40},
	{0x22, 0xdc},
	{0x24, 0x16},
	{0x25, 0x01},
	{0x26, 0x10},
	{0x2d, 0x60},
	{0x30, 0x01},
	{0x31, 0x90},
	{0x33, 0x06},
	{0x34, 0x01},

	{0xfe, 0x00},
	{0x80, 0x7f},
	{0x81, 0x26},
	{0x82, 0xfa},
	{0x83, 0x02},
	{0x84, 0x03},
	{0x86, 0x02},
	{0x88, 0x03},
	{0x89, 0x03},
	{0x85, 0x08},
	{0x8a, 0x00},
	{0x8b, 0x00},
	{0xb0, 0x55},
	{0xc3, 0x00},
	{0xc4, 0x80},
	{0xc5, 0x90},
	{0xc6, 0x3b},
	{0xc7, 0x46},
	{0xec, 0x06},
	{0xed, 0x04},
	{0xee, 0x60},
	{0xef, 0x90},
	{0xb6, 0x01},
	{0x90, 0x01},
	{0x91, 0x00},
	{0x92, 0x00},
	{0x93, 0x00},
	{0x94, 0x00},
	{0x95, 0x04},
	{0x96, 0xb0},
	{0x97, 0x06},
	{0x98, 0x40},
	/*  BLK */
	{0xfe, 0x00},
	{0x40, 0x42},
	{0x41, 0x00},
	{0x43, 0x5b},
	{0x5e, 0x00},
	{0x5f, 0x00},
	{0x60, 0x00},
	{0x61, 0x00},
	{0x62, 0x00},
	{0x63, 0x00},
	{0x64, 0x00},
	{0x65, 0x00},
	{0x66, 0x20},
	{0x67, 0x20},
	{0x68, 0x20},
	{0x69, 0x20},
	{0x76, 0x00},
	{0x6a, 0x08},
	{0x6b, 0x08},
	{0x6c, 0x08},
	{0x6d, 0x08},
	{0x6e, 0x08},
	{0x6f, 0x08},
	{0x70, 0x08},
	{0x71, 0x08},
	{0x76, 0x00},
	{0x72, 0xf0},
	{0x7e, 0x3c},
	{0x7f, 0x00},
	{0xfe, 0x02},
	{0x48, 0x15},
	{0x49, 0x00},
	{0x4b, 0x0b},
	{0xfe, 0x00},
	/* AEC */
	{0xfe, 0x01},
	{0x01, 0x04},
	{0x02, 0xc0},
	{0x03, 0x04},
	{0x04, 0x90},
	{0x05, 0x24},
	{0x06, 0x80},
	{0x07, 0x1b},
	{0x08, 0x60},
	{0x09, 0x00},
	{0x0a, 0x82},
	{0x0b, 0x11},
	{0x0c, 0x10},
	{0x11, 0x10},
	{0x13, 0x50},
	{0x17, 0x00},
	{0x1c, 0x11},
	{0x1e, 0x61},
	{0x1f, 0x35},
	{0x20, 0x40},
	{0x22, 0x40},
	{0x23, 0x20},
	{0xfe, 0x02},
	{0x0f, 0x04},
	{0xfe, 0x01},
	{0x12, 0x35},
	{0x15, 0x50},
	{0x10, 0x31},
	{0x3e, 0x28},
	{0x3f, 0xb0},
	{0x40, 0x90},
	{0x41, 0x0f},
	/* INTPEE */
	{0xfe, 0x02},
	{0x90, 0x6c},
	{0x91, 0x03},
	{0x92, 0xcb},
	{0x94, 0x33},
	{0x95, 0x84},
	{0x97, 0x65},
	{0xa2, 0x11},
	{0xfe, 0x00},
	/* DNDD */
	{0xfe, 0x02},
	{0x80, 0xc1},
	{0x81, 0x08},
	{0x82, 0x05},
	{0x83, 0x08},
	{0x84, 0x0a},
	{0x86, 0xf0},
	{0x87, 0x50},
	{0x88, 0x15},
	{0x89, 0xb0},
	{0x8a, 0x30},
	{0x8b, 0x10},
	/* ASDE */
	{0xfe, 0x01},
	{0x21, 0x04},
	{0xfe, 0x02},
	{0xa3, 0x50},
	{0xa4, 0x20},
	{0xa5, 0x40},
	{0xa6, 0x80},
	{0xab, 0x40},
	{0xae, 0x0c},
	{0xb3, 0x46},
	{0xb4, 0x64},
	{0xb6, 0x38},
	{0xb7, 0x01},
	{0xb9, 0x2b},
	{0x3c, 0x04},
	{0x3d, 0x15},
	{0x4b, 0x06},
	{0x4c, 0x20},
	{0xfe, 0x00},
	/* GAMMA */

	/* gamma1 */
	#if 1
	{0xfe, 0x02},
	{0x10, 0x09},
	{0x11, 0x0d},
	{0x12, 0x13},
	{0x13, 0x19},
	{0x14, 0x27},
	{0x15, 0x37},
	{0x16, 0x45},
	{0x17, 0x53},
	{0x18, 0x69},
	{0x19, 0x7d},
	{0x1a, 0x8f},
	{0x1b, 0x9d},
	{0x1c, 0xa9},
	{0x1d, 0xbd},
	{0x1e, 0xcd},
	{0x1f, 0xd9},
	{0x20, 0xe3},
	{0x21, 0xea},
	{0x22, 0xef},
	{0x23, 0xf5},
	{0x24, 0xf9},
	{0x25, 0xff},
	#else
	{0xfe, 0x02},
	{0x10, 0x0a},
	{0x11, 0x12},
	{0x12, 0x19},
	{0x13, 0x1f},
	{0x14, 0x2c},
	{0x15, 0x38},
	{0x16, 0x42},
	{0x17, 0x4e},
	{0x18, 0x63},
	{0x19, 0x76},
	{0x1a, 0x87},
	{0x1b, 0x96},
	{0x1c, 0xa2},
	{0x1d, 0xb8},
	{0x1e, 0xcb},
	{0x1f, 0xd8},
	{0x20, 0xe2},
	{0x21, 0xe9},
	{0x22, 0xf0},
	{0x23, 0xf8},
	{0x24, 0xfd},
	{0x25, 0xff},
	{0xfe, 0x00},
	#endif
	{0xfe, 0x00},
	{0xc6, 0x20},
	{0xc7, 0x2b},
	/* gamma2 */
	#if 1
	{0xfe, 0x02},
	{0x26, 0x0f},
	{0x27, 0x14},
	{0x28, 0x19},
	{0x29, 0x1e},
	{0x2a, 0x27},
	{0x2b, 0x33},
	{0x2c, 0x3b},
	{0x2d, 0x45},
	{0x2e, 0x59},
	{0x2f, 0x69},
	{0x30, 0x7c},
	{0x31, 0x89},
	{0x32, 0x98},
	{0x33, 0xae},
	{0x34, 0xc0},
	{0x35, 0xcf},
	{0x36, 0xda},
	{0x37, 0xe2},
	{0x38, 0xe9},
	{0x39, 0xf3},
	{0x3a, 0xf9},
	{0x3b, 0xff},
	#else
	/* Gamma outdoor */
	{0xfe, 0x02},
	{0x26, 0x17},
	{0x27, 0x18},
	{0x28, 0x1c},
	{0x29, 0x20},
	{0x2a, 0x28},
	{0x2b, 0x34},
	{0x2c, 0x40},
	{0x2d, 0x49},
	{0x2e, 0x5b},
	{0x2f, 0x6d},
	{0x30, 0x7d},
	{0x31, 0x89},
	{0x32, 0x97},
	{0x33, 0xac},
	{0x34, 0xc0},
	{0x35, 0xcf},
	{0x36, 0xda},
	{0x37, 0xe5},
	{0x38, 0xec},
	{0x39, 0xf8},
	{0x3a, 0xfd},
	{0x3b, 0xff},
	#endif
	/* YCP */
	{0xfe, 0x02},
	{0xd1, 0x32},
	{0xd2, 0x32},
	{0xd3, 0x40},
	{0xd6, 0xf0},
	{0xd7, 0x10},
	{0xd8, 0xda},
	{0xdd, 0x14},
	{0xde, 0x86},
	{0xed, 0x80},
	{0xee, 0x00},
	{0xef, 0x3f},
	{0xd8, 0xd8},
	/* abs */
	{0xfe, 0x01},
	{0x9f, 0x40},
	/* LSC */
	{0xfe, 0x01},
	{0xc2, 0x14},
	{0xc3, 0x0d},
	{0xc4, 0x0c},
	{0xc8, 0x15},
	{0xc9, 0x0d},
	{0xca, 0x0a},
	{0xbc, 0x24},
	{0xbd, 0x10},
	{0xbe, 0x0b},
	{0xb6, 0x25},
	{0xb7, 0x16},
	{0xb8, 0x15},
	{0xc5, 0x00},
	{0xc6, 0x00},
	{0xc7, 0x00},
	{0xcb, 0x00},
	{0xcc, 0x00},
	{0xcd, 0x00},
	{0xbf, 0x07},
	{0xc0, 0x00},
	{0xc1, 0x00},
	{0xb9, 0x00},
	{0xba, 0x00},
	{0xbb, 0x00},
	{0xaa, 0x01},
	{0xab, 0x01},
	{0xac, 0x00},
	{0xad, 0x05},
	{0xae, 0x06},
	{0xaf, 0x0e},
	{0xb0, 0x0b},
	{0xb1, 0x07},
	{0xb2, 0x06},
	{0xb3, 0x17},
	{0xb4, 0x0e},
	{0xb5, 0x0e},
	{0xd0, 0x09},
	{0xd1, 0x00},
	{0xd2, 0x00},
	{0xd6, 0x08},
	{0xd7, 0x00},
	{0xd8, 0x00},
	{0xd9, 0x00},
	{0xda, 0x00},
	{0xdb, 0x00},
	{0xd3, 0x0a},
	{0xd4, 0x00},
	{0xd5, 0x00},
	{0xa4, 0x00},
	{0xa5, 0x00},
	{0xa6, 0x77},
	{0xa7, 0x77},
	{0xa8, 0x77},
	{0xa9, 0x77},
	{0xa1, 0x80},
	{0xa2, 0x80},
	{0xfe, 0x01},
	{0xdf, 0x0d},
	{0xdc, 0x25},
	{0xdd, 0x30},
	{0xe0, 0x77},
	{0xe1, 0x80},
	{0xe2, 0x77},
	{0xe3, 0x90},
	{0xe6, 0x90},
	{0xe7, 0xa0},
	{0xe8, 0x90},
	{0xe9, 0xa0},
	{0xfe, 0x00},
	/* AWB */
	{0xfe, 0x01},
	{0x4f, 0x00},
	{0x4f, 0x00},
	{0x4b, 0x01},
	{0x4f, 0x00},
	{0x4c, 0x01}, /* D75 */
	{0x4d, 0x71},
	{0x4e, 0x01},
	{0x4c, 0x01},
	{0x4d, 0x91},
	{0x4e, 0x01},
	{0x4c, 0x01},
	{0x4d, 0x70},
	{0x4e, 0x01},
	{0x4c, 0x01}, /* D65 */
	{0x4d, 0x90},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0xb0},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0x8f},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0x6f},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0xaf},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0xd0},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0xf0},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0xcf},
	{0x4e, 0x02},
	{0x4c, 0x01},
	{0x4d, 0xef},
	{0x4e, 0x02},
	{0x4c, 0x01},/* D50 */
	{0x4d, 0x6e},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x8e},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xae},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xce},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x4d},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x6d},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x8d},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xad},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xcd},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x4c},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x6c},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x8c},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xac},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xcc},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xcb},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x4b},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x6b},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0x8b},
	{0x4e, 0x03},
	{0x4c, 0x01},
	{0x4d, 0xab},
	{0x4e, 0x03},
	{0x4c, 0x01},/* CWF */
	{0x4d, 0x8a},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0xaa},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0xca},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0xca},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0xc9},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0x8a},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0x89},
	{0x4e, 0x04},
	{0x4c, 0x01},
	{0x4d, 0xa9},
	{0x4e, 0x04},
	{0x4c, 0x02},/* tl84 */
	{0x4d, 0x0b},
	{0x4e, 0x05},
	{0x4c, 0x02},
	{0x4d, 0x0a},
	{0x4e, 0x05},
	{0x4c, 0x01},
	{0x4d, 0xeb},
	{0x4e, 0x05},
	{0x4c, 0x01},
	{0x4d, 0xea},
	{0x4e, 0x05},
	{0x4c, 0x02},
	{0x4d, 0x09},
	{0x4e, 0x05},
	{0x4c, 0x02},
	{0x4d, 0x29},
	{0x4e, 0x05},
	{0x4c, 0x02},
	{0x4d, 0x2a},
	{0x4e, 0x05},
	{0x4c, 0x02},
	{0x4d, 0x4a},
	{0x4e, 0x05},
	/* A */
	/* {0x4c , 0x02}, */
	/* {0x4d , 0x6a}, */
	/* {0x4e , 0x06}, */
	{0x4c, 0x02},
	{0x4d, 0x8a},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0x49},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0x69},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0x89},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0xa9},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0x48},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0x68},
	{0x4e, 0x06},
	{0x4c, 0x02},
	{0x4d, 0x69},
	{0x4e, 0x06},
	{0x4c, 0x02},/* H */
	{0x4d, 0xca},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xc9},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xe9},
	{0x4e, 0x07},
	{0x4c, 0x03},
	{0x4d, 0x09},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xc8},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xe8},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xa7},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xc7},
	{0x4e, 0x07},
	{0x4c, 0x02},
	{0x4d, 0xe7},
	{0x4e, 0x07},
	{0x4c, 0x03},
	{0x4d, 0x07},
	{0x4e, 0x07},
	{0x4f, 0x01},
	{0x50, 0x80},
	{0x51, 0xa8},
	{0x52, 0x47},
	{0x53, 0x38},
	{0x54, 0xc7},
	{0x56, 0x0e},
	{0x58, 0x08},
	{0x5b, 0x00},
	{0x5c, 0x74},
	{0x5d, 0x8b},
	{0x61, 0xdb},
	{0x62, 0xb8},
	{0x63, 0x86},
	{0x64, 0xc0},
	{0x65, 0x04},
	{0x67, 0xa8},
	{0x68, 0xb0},
	{0x69, 0x00},
	{0x6a, 0xa8},
	{0x6b, 0xb0},
	{0x6c, 0xaf},
	{0x6d, 0x8b},
	{0x6e, 0x50},
	{0x6f, 0x18},
	{0x73, 0xf0},
	{0x70, 0x0d},
	{0x71, 0x60},
	{0x72, 0x80},
	{0x74, 0x01},
	{0x75, 0x01},
	{0x7f, 0x0c},
	{0x76, 0x70},
	{0x77, 0x58},
	{0x78, 0xa0},
	{0x79, 0x5e},
	{0x7a, 0x54},
	{0x7b, 0x58},
	{0xfe, 0x00},
	/* CC */
	{0xfe, 0x02},
	{0xc0, 0x01},
	{0xc1, 0x44},
	{0xc2, 0xfd},
	{0xc3, 0x04},
	{0xc4, 0xf0},
	{0xc5, 0x48},
	{0xc6, 0xfd},
	{0xc7, 0x46},
	{0xc8, 0xfd},
	{0xc9, 0x02},
	{0xca, 0xe0},
	{0xcb, 0x45},
	{0xcc, 0xec},
	{0xcd, 0x48},
	{0xce, 0xf0},
	{0xcf, 0xf0},
	{0xe3, 0x0c},
	{0xe4, 0x4b},
	{0xe5, 0xe0},
	/* ABS */
	{0xfe, 0x01},
	{0x9f, 0x40},
	{0xfe, 0x00},
	/* OUTPUT */
	{0xfe, 0x00},
	{0xf2, 0x0f},  /* 0x00 */
	/* frame rate 50Hz */
	{0xfe, 0x00},
	{0x05, 0x01},
	{0x06, 0x56},
	{0x07, 0x02},  /* 0x00 */
	{0x08, 0x06},  /* 0x32 */
	{0xfe, 0x01},
	{0x25, 0x00},
	{0x26, 0xfa},
	{0x27, 0x04},
	{0x28, 0xe2}, /* 20fps */
#if 0
	{0x29, 0x06},
	{0x2a, 0xd6}, /* 14fps */
	{0x2b, 0x07},
	{0x2c, 0xd0}, /* 12fps */
	{0x2d, 0x0b},
	{0x2e, 0xb8}, /* 8fps */
#else
	{0x29, 0x05},
	{0x2a, 0xdc}, /* 14fps */
	{0x2b, 0x06},
	{0x2c, 0xd6}, /* 12fps */
	{0x2d, 0x0b},
	{0x2e, 0xb8}, /* 8fps */
#endif
	{0xfe, 0x00},
	/* dark sun */
	{0xfe, 0x02},
	{0x40, 0xbf},
	{0x46, 0xcf},
	{0xfe, 0x00},
	/* MIPI */
	{0xfe, 0x03},
	{0x02, 0x22},
	{0x03, 0x10}, /* 0x12 20140821 */
	{0x04, 0x10}, /* 0x01 */
	{0x05, 0x00},
	{0x06, 0x88},
	#ifdef GC2145MIPI_2Lane
		{0x01, 0x87},
		{0x10, 0x95},
	#else
		{0x01, 0x83},
		{0x10, 0x94},
	#endif
	{0x11, 0x1e},
	{0x12, 0x80},
	{0x13, 0x0c},
	{0x15, 0x10},
	{0x17, 0xf0},
	{0x21, 0x10},
	{0x22, 0x04},
	{0x23, 0x10},
	{0x24, 0x10},
	{0x25, 0x10},
	{0x26, 0x05},
	{0x29, 0x03},
	{0x2a, 0x0a},
	{0x2b, 0x06},
	{0xfe, 0x00},
};


/*
 * 800x600p 60fps
 * Input Clock = 24Mhz bit rate 480Mbps
 */
static const sensor_reg_t gc2145_yuv_800x600[] = {
	{0xfe, 0x00},
#if 0
	{0xfd, 0x01},
	{0xfa, 0x00},
	/* crop window */
	{0xfe, 0x00},
	{0x90, 0x01},
	{0x91, 0x00},
	{0x92, 0x00},
	{0x93, 0x00},
	{0x94, 0x00},
	{0x95, 0x02},
	{0x96, 0x58},
	{0x97, 0x03},
	{0x98, 0x20},
	{0x99, 0x11},
	{0x9a, 0x06},
#else
	{0xfa, 0x00},
	{0xfd, 0x01},
	/* crop window */
	{0xfe, 0x00},
	{0x99, 0x11},
	{0x9a, 0x06},
	{0x9b, 0x00},
	{0x9c, 0x00},
	{0x9d, 0x00},
	{0x9e, 0x00},
	{0x9f, 0x00},
	{0xa0, 0x00},
	{0xa1, 0x00},
	{0xa2, 0x00},
	{0x90, 0x01},
	{0x91, 0x00},
	{0x92, 0x00},
	{0x93, 0x00},
	{0x94, 0x00},
	{0x95, 0x02},
	{0x96, 0x58},
	{0x97, 0x03},
	{0x98, 0x20},
#endif

	/* AWB */
	{0xfe, 0x00},
	{0xec, 0x02},
	{0xed, 0x02},
	{0xee, 0x30},
	{0xef, 0x48},
	{0xfe, 0x02},
	{0x9d, 0x08},
	{0xfe, 0x01},
	{0x74, 0x00},
	/* AEC */
	{0xfe, 0x01},
	{0x01, 0x04},
	{0x02, 0x60},
	{0x03, 0x02},
	{0x04, 0x48},
	{0x05, 0x18},
	{0x06, 0x50},
	{0x07, 0x10},
	{0x08, 0x38},
	{0x0a, 0x80},
	{0x21, 0x04},
	{0xfe, 0x00},
	{0x20, 0x03},
	/* mipi */
	{0xfe, 0x03},
	{0x12, 0x40},
	{0x13, 0x06},
#if defined(GC2145MIPI_2Lane)
	{0x04, 0x90},
	{0x05, 0x01},
#else
	{0x04, 0x01},
	{0x05, 0x00},
#endif
	{0xfe, 0x00},
	{0xfe, 0x00},
};

static const sensor_reg_t gc2145_yuv_1600x1200[] = {
	{0xfe, 0x00},
#if 0
	{0xfa, 0x11},
#else
	{0xfa, 0x00},
#endif
	{0xfd, 0x00},

	/* crop window */
	{0xfe, 0x00},
	{0x99, 0x11},
	{0x9a, 0x06},
	{0x9b, 0x00},
	{0x9c, 0x00},
	{0x9d, 0x00},
	{0x9e, 0x00},
	{0x9f, 0x00},
	{0xa0, 0x00},
	{0xa1, 0x00},
	{0xa2, 0x00},
	{0x90, 0x01},
	{0x91, 0x00},
	{0x92, 0x00},
	{0x93, 0x00},
	{0x94, 0x00},
	{0x95, 0x04},
	{0x96, 0xb0},
	{0x97, 0x06},
	{0x98, 0x40},

	/* AWB */
	{0xfe, 0x00},
	{0xec, 0x02},
	{0xed, 0x04},
	{0xee, 0x60},
	{0xef, 0x90},
	{0xfe, 0x01},
	{0x74, 0x01},
	/* AEC */
	{0xfe, 0x01},
	{0x01, 0x08},
	{0x02, 0xc0},
	{0x03, 0x04},
	{0x04, 0x90},
	{0x05, 0x30},
	{0x06, 0x98},
	{0x07, 0x28},
	{0x08, 0x6c},
	{0x0a, 0xc2},
#if 0
	{0x21, 0x15}, /* if 0xfa=11,then 0x21=15;else if 0xfa=00,then 0x21=14 */
#else
	{0x21, 0x14}, /* if 0xfa=11,then 0x21=15;else if 0xfa=00,then 0x21=14 */
#endif
	{0xfe, 0x00},

	/* mipi */
	{0xfe, 0x03},
	{0x12, 0x80},
	{0x13, 0x0c},
#if defined(GC2145MIPI_2Lane)
	{0x04, 0x90},
	{0x05, 0x01},
#else
	{0x04, 0x01},
	{0x05, 0x00},
#endif
	{0xfe, 0x00},
	{0xfe, 0x00},
};

/* the supported format of this sensor is MEDIA_BUS_FMT_YUYV8_2X8 */
static const struct v4l2_mbus_framefmt gc2145_formats[] = {
	{
		.code = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.field = V4L2_FIELD_NONE,
	},
};

struct gc2145_framesize {
	int	width;
	int	height;
	int	vts;
	int	framerate;
	const sensor_reg_t *regs; /* Regs to tweak */
};

static struct gc2145_framesize gc2145_framesizes[] = {
	/* 1600x1200 */
	{
		.width		= GC2145_800x600_WIDTH,
		.height		= GC2145_800x600_HEIGHT,
		.vts		= 0x02e4,
		.framerate	= 14,
		.regs		= gc2145_yuv_800x600,
	},
	{
		.width		= GC2145_1600x1200_WIDTH,
		.height		= GC2145_1600x1200_HEIGHT,
		.vts		= 0x02e4,
		.framerate	= 14,
		.regs		= gc2145_yuv_1600x1200,
	},
};

static int reg_read8(struct i2c_client *client, u8 reg, u8 *val)
{
	struct i2c_msg msg[2];
	u8 buf[1];
	int ret;

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 1;
	msg[0].buf   = (u8 *)&reg;

	msg[1].addr  = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len   = 1;
	msg[1].buf   = buf;

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret < 0) {
		/* PRT_INFO("i2c read error, reg: %x\n", reg); */
		return ret;
	}

	memcpy(val, buf, 1);

	return 0;
}

static int reg_write8(struct i2c_client *client, u8 reg, u8 val)
{
	struct i2c_msg msg;
	u8 buf[2] = {reg, val};
	int ret;

	msg.addr  = client->addr;
	msg.flags = 0;
	msg.len   = 2;
	msg.buf   = buf;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret >= 0)
		ret = 0;

	return ret;

}

static int gc2145_check_present(void)
{
	u8 tmp[2];
	int ret = 0;
	u16 id;

	ret = reg_read8(gc2145_client, 0xf0, &tmp[0]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x\n", tmp[0]);
		return -ENODEV;
	}
	ret = reg_read8(gc2145_client, 0xf1, &tmp[1]);
	if (ret != 0) {
		PRT_ERR("tmp[0]=%x,tmp[1]=%x\n", tmp[0], tmp[1]);
		return -ENODEV;
	}
	id = (tmp[0] << 8) | tmp[1];
	PRT_INFO("id: 0x%04x.\n", id);

	if (id != 0x2145) {
		PRT_ERR("ID wrong! (0x%x)\n", id);
		return -ENODEV;
	}

	return 0;
}

static int gc2145_cmos_init(gc2145_config_t *gc2145)
{
	int i;
	int ret = 0;
	PRT_INFO("\n");

	ret = gc2145_check_present();
	if (ret != 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(gc2145_reg_list); i++) {
		ret = reg_write8(gc2145_client,
				gc2145_reg_list[i].reg_add,
				gc2145_reg_list[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}

	if (gc2145->mirror) {
		ret = reg_write8(gc2145_client, 0x17, 0x15);
		if (ret != 0) {
			PRT_ERR("set mirror failed ret: %d.\n", ret);
			return ret;
		}
	}

	PRT_INFO("write reg list finish.\n");

#ifdef GC2145_1600x1200_IMAGE
	for (i = 0; i < ARRAY_SIZE(gc2145_yuv_1600x1200); i++) {
			ret = reg_write8(gc2145_client,
				gc2145_yuv_1600x1200[i].reg_add,
				gc2145_yuv_1600x1200[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}
	PRT_INFO("write gc2145_yuv_1600x1200 list finish.\n");
#else
	for (i = 0; i < ARRAY_SIZE(gc2145_yuv_800x600); i++) {
			ret = reg_write8(gc2145_client,
				gc2145_yuv_800x600[i].reg_add,
				gc2145_yuv_800x600[i].reg_value);
		if (ret) {
			PRT_ERR("\n");
			return ret;
		}
	}
	PRT_INFO("write gc2145_yuv_800x600 list finish.\n");
#endif

	return 0;

}

static inline gc2145_config_t *to_gc2145(struct v4l2_subdev *sd)
{
	return container_of(sd, gc2145_config_t, sd);
}

static const struct v4l2_mbus_framefmt *find_sensor_format(
					struct v4l2_mbus_framefmt *mf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(gc2145_formats); i++)
		if (mf->code == gc2145_formats[i].code)
			return &gc2145_formats[i];

	return &gc2145_formats[0];
}

static void gc2145_try_format(struct v4l2_mbus_framefmt *mf)
{
	const struct v4l2_mbus_framefmt *fmt;

	fmt = find_sensor_format(mf);
	PRT_INFO("fmt->code:0x%x\n", fmt->code);
	PRT_INFO("mf->code:0x%x\n", mf->code);
	mf->code = fmt->code;
	mf->field = V4L2_FIELD_NONE;

#ifdef GC2145_1600x1200_IMAGE
	mf->width = GC2145_1600x1200_WIDTH;
	mf->height = GC2145_1600x1200_HEIGHT;
#else
	mf->width = GC2145_800x600_WIDTH;
	mf->height = GC2145_800x600_HEIGHT;
#endif

#if 0
	v4l_bound_align_image(&mf->width, GC2145_MIN_WIDTH,
		GC2145_MAX_WIDTH, 0,
		&mf->height, GC2145_MIN_HEIGHT,
		GC2145_MAX_HEIGHT, 0, 0);
#endif

}

static int gc2145_set_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	gc2145_config_t *gc2145 = to_gc2145(sd);
	struct v4l2_mbus_framefmt *mf = NULL;

	PRT_DBG("\n");

	gc2145_try_format(&fmt->format);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
	}

	if (mf) {
		mutex_lock(&gc2145->lock);
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
			PRT_DBG("\n");
			*mf = fmt->format;
		}
		mutex_unlock(&gc2145->lock);
	}

	{
	struct viss_source_info *si = NULL;
	struct mcsi_if_info *mcsi_if = NULL;

	si = v4l2_get_subdev_hostdata(sd);
	if (si == NULL) {
		PRT_ERR("can't get sensor configuration data!\n");
		return -EINVAL;
	}
	mcsi_if = (struct mcsi_if_info *)si->if_info;

	mcsi_if->dlanes = 2;
	mcsi_if->data_type = VISS_MCSI_YUV422;
#ifdef GC2145_1600x1200_IMAGE
	mcsi_if->mfreq = 960000000;
#else
	mcsi_if->mfreq = 480000000;
#endif
	PRT_INFO("dlanes=%d,data_type=%d,mfreq=%d\n",
		mcsi_if->dlanes, mcsi_if->data_type, mcsi_if->mfreq);
	}

	return 0;
}

static int gc2145_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh,
			   struct v4l2_subdev_format *fmt)
{
	gc2145_config_t *gc2145 = to_gc2145(sd);
	struct v4l2_mbus_framefmt *mf;

	PRT_DBG("\n");

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, 0);
		fmt->format = *mf;
		return 0;
	}

	/* V4L2_SUBDEV_FORMAT_ACTIVE */
	mutex_lock(&gc2145->lock);
	fmt->format = gc2145->format;
	mutex_unlock(&gc2145->lock);

	return 0;
}

/*
 * V4L2 subdev video and pad level operations
 */
static void gc2145_get_default_format(struct v4l2_mbus_framefmt *mf)
{
	mf->width = gc2145_framesizes[0].width;
	mf->height = gc2145_framesizes[0].height;
	mf->colorspace = gc2145_formats[0].colorspace;
	mf->code = gc2145_formats[0].code;
	mf->field = V4L2_FIELD_NONE;
}

/*
 * V4L2 subdev internal operations
 */

static int gc2145_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
#if 0
	struct v4l2_mbus_framefmt *format = v4l2_subdev_get_try_format(fh, 0);
	struct v4l2_rect *crop = v4l2_subdev_get_try_crop(fh, 0);
#endif

	return 0;
}

static int gc2145_registered(struct v4l2_subdev *sd)
{
	gc2145_config_t *gc2145 = to_gc2145(sd);
	struct i2c_client *client = gc2145->client;
	int ret = 0;

	mutex_lock(&gc2145->lock);

	if ((gc2145->reset_gpio != 0) && (gc2145->pwdn_gpio != 0)) {
		ret = devm_gpio_request(&client->dev, gc2145->reset_gpio,
					"reset_gpio");
		if (ret < 0) {
			PRT_ERR("reset_gpio %d request failed (reset_gpio)\n",
				gc2145->reset_gpio);
			gc2145->reset_gpio = 0;
			gc2145->pwdn_gpio = 0;
			goto unlock;
		}

		ret = devm_gpio_request(&client->dev, gc2145->pwdn_gpio,
					"pwdn_gpio");
		if (ret < 0) {
			PRT_ERR("pwdn_gpio %d request failed (pwdn_gpio)\n",
				gc2145->pwdn_gpio);
			devm_gpio_free(&client->dev, gc2145->reset_gpio);
			gc2145->reset_gpio = 0;
			gc2145->pwdn_gpio = 0;
			goto unlock;
		}
	}

	if (gc2145->skip_detect) {
		PRT_INFO("skip sensor gc2145 detect\n");
		goto unlock;
	}

	gc2145_reset_cycle(gc2145);
	ret = gc2145_check_present();
	gc2145_pwdn(gc2145);
	if (ret != 0) {
		PRT_ERR("sensor gc2145 not present!\n");
		if ((gc2145->reset_gpio != 0) && (gc2145->pwdn_gpio != 0)) {
			devm_gpio_free(&client->dev, gc2145->pwdn_gpio);
			devm_gpio_free(&client->dev, gc2145->reset_gpio);
			gc2145->reset_gpio = 0;
			gc2145->pwdn_gpio = 0;
		}
	} else
		PRT_INFO("sensor gc2145 present\n");

unlock:
	mutex_unlock(&gc2145->lock);

	return ret;
}

static const struct v4l2_subdev_internal_ops gc2145_subdev_internal_ops = {
	.registered = gc2145_registered,
	.open = gc2145_open,
};

static void gc2145_reset_cycle(gc2145_config_t *gc2145)
{
	PRT_INFO("\n");

	if ((gc2145->reset_gpio == 0) || (gc2145->pwdn_gpio == 0))
		return;

	/* pwdn high */
	gpio_direction_output(gc2145->pwdn_gpio, 1);

	mdelay(5);
	/* reset down */
	gpio_direction_output(gc2145->reset_gpio, 0);

	mdelay(5);

	/* pwdn down */
	gpio_direction_output(gc2145->pwdn_gpio, 0);

	mdelay(5);

	/* reset high */
	gpio_direction_output(gc2145->reset_gpio, 1);

	mdelay(5);
}

static void gc2145_pwdn(gc2145_config_t *gc2145)
{
#if 1
	if ((gc2145->reset_gpio == 0) || (gc2145->pwdn_gpio == 0))
		return;

	/* reset low */
	gpio_direction_output(gc2145->reset_gpio, 0);
	mdelay(10);
	/* pwdn high */
	gpio_direction_output(gc2145->pwdn_gpio, 1);
#endif
}

static int gc2145_set_power(struct v4l2_subdev *sd, int on)
{
	gc2145_config_t *gc2145 = to_gc2145(sd);
	int ret = 0;

	if (on) {
#if 1
		gc2145_reset_cycle(gc2145);
#endif
		ret = gc2145_cmos_init(gc2145);
	} else {
		gc2145_pwdn(gc2145);
	}

	PRT_INFO("on:%d\n", on);
	return ret;
}

static int gc2145_set_ae_target(struct v4l2_subdev *sd, int val)
{
	int ret = 0;

	ret += reg_write8(gc2145_client, 0xfe, 0x1);
	ret += reg_write8(gc2145_client, 0x13, val);
	ret += reg_write8(gc2145_client, 0x15, val);

	ret += reg_write8(gc2145_client, 0x18, 0xf7);
	ret += reg_write8(gc2145_client, 0x19, 0xf7);

	PRT_DBG("target: 0x%x\n", val);
	return ret;
}

/*
 * V4L2 subdev internal operations
 */
static long gc2145_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	int ret = 0, pix = 4;

	switch (cmd) {
	case VIDIOC_ISP_SET_AE_METER:{
#ifdef GC2145_1600x1200_IMAGE
		struct viss_isp_ae_para *ae_para =
			(struct viss_isp_ae_para *)arg;
		/*
		PRT_ERR("VIDIOC_VISS_MIPI_SET_AE_METER %d, %d, %d, %d, %d.\n",
			ae_para->rect.left,
			ae_para->rect.top, ae_para->rect.width,
			ae_para->rect.height, ae_para->ae_mode);
		*/
		if (ae_para->ae_mode ==
			V4L2_EXPOSURE_METERING_CENTER_WEIGHTED) {

			if ((ae_para->rect.width < 2 * 8 * pix) ||
					(ae_para->rect.height < 2 * 8 * pix))
				return ret;

			if (ae_para->rect.left < 8 * pix)
				ae_para->rect.left = 8 * pix;

			if (ae_para->rect.top < 8 * pix)
				ae_para->rect.top = 8 * pix;

			if (ae_para->rect.left +
				ae_para->rect.width >= (1600 - 8 * pix))
				ae_para->rect.width =
					1600 - 8 * pix - ae_para->rect.left;

			if (ae_para->rect.top +
				ae_para->rect.height >= (1200 - 8 * pix))
				ae_para->rect.height =
					1200 - 8 * pix - ae_para->rect.top;

			/*
			PRT_ERR("VIDIOC_VISS_MIPI_SET_AE_METER %d,
				%d, %d, %d.\n",
				ae_para->rect.left, ae_para->rect.top,
				ae_para->rect.width, ae_para->rect.height);
			*/

			ret += reg_write8(gc2145_client, 0xfe, 0x1);
			ret += reg_write8(gc2145_client, 0x01,
				ae_para->rect.left / 8 - pix);
			ret += reg_write8(gc2145_client, 0x03,
				ae_para->rect.top / 8 - pix);
			ret += reg_write8(gc2145_client, 0x02,
				(ae_para->rect.left + ae_para->rect.width)
				/ 8 + pix);
			ret += reg_write8(gc2145_client, 0x04,
				(ae_para->rect.top + ae_para->rect.height)
				/ 8 + pix);
			ret += reg_write8(gc2145_client, 0x05,
				ae_para->rect.left / 8);
			ret += reg_write8(gc2145_client, 0x07,
				ae_para->rect.top / 8);
			ret += reg_write8(gc2145_client, 0x06,
				(ae_para->rect.left + ae_para->rect.width)
				/ 8);
			ret += reg_write8(gc2145_client, 0x08,
				(ae_para->rect.top + ae_para->rect.height)
				/ 8);
		} else {
			ret += reg_write8(gc2145_client, 0xfe, 0x1);
			ret += reg_write8(gc2145_client, 0x01, 0x04);
			ret += reg_write8(gc2145_client, 0x02, 0xc0);
			ret += reg_write8(gc2145_client, 0x03, 0x04);
			ret += reg_write8(gc2145_client, 0x04, 0x90);
			ret += reg_write8(gc2145_client, 0x05, 0x24);
			ret += reg_write8(gc2145_client, 0x06, 0x80);
			ret += reg_write8(gc2145_client, 0x07, 0x1b);
			ret += reg_write8(gc2145_client, 0x08, 0x60);
		}
#else
		struct viss_isp_ae_para *ae_para =
			(struct viss_isp_ae_para *)arg;
		/*
		PRT_ERR("VIDIOC_VISS_MIPI_SET_AE_METER %d, %d, %d, %d, %d.\n",
			ae_para->rect.left,
			ae_para->rect.top, ae_para->rect.width,
			ae_para->rect.height, ae_para->ae_mode);
		*/
		if (ae_para->ae_mode ==
			V4L2_EXPOSURE_METERING_CENTER_WEIGHTED) {

			if ((ae_para->rect.width < 2 * 8 * pix) ||
					(ae_para->rect.height < 2 * 8 * pix))
				return ret;

			if (ae_para->rect.left < 8 * pix)
				ae_para->rect.left = 8 * pix;

			if (ae_para->rect.top < 8 * pix)
				ae_para->rect.top = 8 * pix;

			if (ae_para->rect.left +
				ae_para->rect.width >= (800 - 8 * pix))
				ae_para->rect.width =
					1600 - 8 * pix - ae_para->rect.left;

			if (ae_para->rect.top +
				ae_para->rect.height >= (600 - 8 * pix))
				ae_para->rect.height =
					1200 - 8 * pix - ae_para->rect.top;

			/*
			PRT_ERR("VIDIOC_VISS_MIPI_SET_AE_METER %d,
				%d, %d, %d.\n",
				ae_para->rect.left, ae_para->rect.top,
				ae_para->rect.width, ae_para->rect.height);
			*/

			ret += reg_write8(gc2145_client, 0xfe, 0x1);
			ret += reg_write8(gc2145_client, 0x01,
				ae_para->rect.left / 8 - pix);
			ret += reg_write8(gc2145_client, 0x03,
				ae_para->rect.top / 8 - pix);
			ret += reg_write8(gc2145_client, 0x02,
				(ae_para->rect.left + ae_para->rect.width)
				/ 8 + pix);
			ret += reg_write8(gc2145_client, 0x04,
				(ae_para->rect.top + ae_para->rect.height)
				/ 8 + pix);
			ret += reg_write8(gc2145_client, 0x05,
				ae_para->rect.left / 8);
			ret += reg_write8(gc2145_client, 0x07,
				ae_para->rect.top / 8);
			ret += reg_write8(gc2145_client, 0x06,
				(ae_para->rect.left + ae_para->rect.width)
				/ 8);
			ret += reg_write8(gc2145_client, 0x08,
				(ae_para->rect.top + ae_para->rect.height)
				/ 8);
		} else {
			ret += reg_write8(gc2145_client, 0xfe, 0x1);
			ret += reg_write8(gc2145_client, 0x01, 0x04);
			ret += reg_write8(gc2145_client, 0x02, 0x60);
			ret += reg_write8(gc2145_client, 0x03, 0x02);
			ret += reg_write8(gc2145_client, 0x04, 0x48);
			ret += reg_write8(gc2145_client, 0x05, 0x18);
			ret += reg_write8(gc2145_client, 0x06, 0x50);
			ret += reg_write8(gc2145_client, 0x07, 0x10);
			ret += reg_write8(gc2145_client, 0x08, 0x38);
		}
#endif
		break;
	}
	case VIDIOC_ISP_SET_AE_TARGET:
		gc2145_set_ae_target(sd, *(unsigned int *) arg);
		break;

	default:
		return -EINVAL;
	}
	return ret;
}


static int gc2145_s_stream(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

#if 0
	gc2145_config_t *gc2145 = to_gc2145(sd);

	gc2145_reset_cycle(gc2145);
	ret = gc2145_cmos_init(gc2145);
#endif

	PRT_INFO("*************************on:%d\n", on);
	return ret;
}

static const struct v4l2_subdev_pad_ops gc2145_pad_ops = {
#if 0
	.enum_mbus_code = gc2145_enum_mbus_code,
	.enum_frame_size = gc2145_enum_frame_sizes,
#endif
	.get_fmt = gc2145_get_fmt,
	.set_fmt = gc2145_set_fmt,
};

/*
 * V4L2 subdev internal operations
 */
static const struct v4l2_subdev_core_ops gc2145_subdev_core_ops = {
	.s_power = gc2145_set_power,
	.ioctl = gc2145_ioctl,
	.log_status = v4l2_ctrl_subdev_log_status,
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops gc2145_video_ops = {
	.s_stream = gc2145_s_stream,
};

#ifdef CONFIG_VIDEO_V4L2_SUBDEV_API

static const struct v4l2_subdev_ops gc2145_subdev_ops = {
	.core = &gc2145_subdev_core_ops,
	.pad = &gc2145_pad_ops,
	.video = &gc2145_video_ops,
};
#endif

/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_ops gc2145_ops;

#define DRIVER_NAME "gc2145_mipi"

static u16 g_cur_reg_addr = 0xFFFF;
static ssize_t
gc2145_sysfs_reg_node_store(struct device *dev, struct device_attribute *attr,
		      const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	int addr, value;
	char end;
	int res;

	res = sscanf(buf, "%x %x%c", &addr, &value, &end);
	if (res < 1) {
		PRT_ERR("Can't parse reg address (%s)\n", buf);
		return -EINVAL;
	}

	if ((res > 2) && (end != '\n')) {
		PRT_ERR("Extra parameters (%s)\n", buf);
		return -EINVAL;
	}

	if (addr > 0xFF) {
		PRT_ERR("addr is too large (%s)!\n", buf);
		return -EINVAL;
	}

	g_cur_reg_addr = (u8)addr;

	if (res >= 2) {
		if (value > 0xFF) {
			PRT_ERR("value is too large (%s)!\n", buf);
			return -EINVAL;
		}

		res = reg_write8(client, g_cur_reg_addr, (u8)value);
		if (res) {
			PRT_ERR("\n");
			return res;
		}
	}

	return count;
}

static ssize_t
gc2145_sysfs_reg_node_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	u8 value;
	int res;

	if (g_cur_reg_addr == 0xFFFF) {
		PRT_ERR("write reg addr to reg_node first!\n");
		return -EINVAL;
	}

	res = reg_read8(client, g_cur_reg_addr, &value);
	if (res != 0) {
		PRT_ERR("read reg (0x%02x) failed!\n", g_cur_reg_addr);
		return res;
	}

	return sprintf(buf, "0x%02x\n", value);
}

static DEVICE_ATTR(reg_node, S_IRUGO | S_IWUSR | S_IWGRP,
			gc2145_sysfs_reg_node_show,
			gc2145_sysfs_reg_node_store);

static int gc2145_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int ret = 0;
	enum of_gpio_flags flags;
	struct i2c_adapter *adapter = client->adapter;
	struct device_node *np = client->dev.of_node;
	struct v4l2_subdev *sd;
	gc2145_config_t *gc2145;

	PRT_INFO("*********probe begin\n");

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	gc2145 = devm_kzalloc(&client->dev, sizeof(*gc2145), GFP_KERNEL);
	if (gc2145 == NULL)
		return -ENOMEM;

	gc2145->reset_gpio = of_get_named_gpio_flags(np, "reset_gpio",
							0, &flags);

	gc2145->pwdn_gpio = of_get_named_gpio_flags(np, "pwdn_gpio",
						0, &flags);

	if (!gpio_is_valid(gc2145->reset_gpio) ||
		!gpio_is_valid(gc2145->pwdn_gpio)) {
		PRT_ERR("get gpio %d(reset) and gpio %d(pwdn) failed\n",
				gc2145->reset_gpio, gc2145->pwdn_gpio);
		gc2145->reset_gpio = 0;
		gc2145->pwdn_gpio = 0;
	}

	gc2145->bsel_gpio = of_get_named_gpio_flags(np, "bsel_gpio",
							0, &flags);
	if (!gpio_is_valid(gc2145->bsel_gpio)) {
		gc2145->mirror = 0;
	} else {
		gc2145->skip_detect = 1;
		ret = devm_gpio_request(&client->dev, gc2145->bsel_gpio,
					"bsel_gpio");
		if (ret < 0) {
			PRT_WARN("request bsel_gpio %d failed\n",
				gc2145->bsel_gpio);
		} else {
			ret = gpio_direction_input(gc2145->bsel_gpio);
			if (ret < 0) {
				PRT_WARN("set input of bsel_gpio %d failed\n",
					gc2145->bsel_gpio);
			} else {
				gc2145->mirror =
					__gpio_get_value(gc2145->bsel_gpio);
			}

			PRT_INFO("gc2145->mirror=%d\n", gc2145->mirror);
			devm_gpio_free(&client->dev, gc2145->bsel_gpio);
		}
	}

	mutex_init(&gc2145->lock);

	gc2145->client = client;
	sd = &gc2145->sd;

	v4l2_i2c_subdev_init(sd, client, &gc2145_subdev_ops);
	strlcpy(sd->name, DRIVER_NAME, sizeof(sd->name));

	sd->internal_ops = &gc2145_subdev_internal_ops;

	client->flags = I2C_CLIENT_SCCB;

	v4l_info(client, "chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

#if defined(CONFIG_MEDIA_CONTROLLER)
	gc2145->pad.flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, &gc2145->pad, 0);
	if (ret < 0)
		return ret;
#endif
	gc2145_get_default_format(&gc2145->format);

	gc2145_client = client;

	ret = device_create_file(&client->dev, &dev_attr_reg_node);
	if (ret < 0)
		PRT_WARN("Can't create reg_node!\n");

	PRT_INFO("probe end\n");
	return 0;
}

static int gc2145_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l_info(client, "%s %d -----------------\n",
		__func__, __LINE__);

	v4l2_device_unregister_subdev(sd);

#if defined(CONFIG_MEDIA_CONTROLLER)
	media_entity_cleanup(&sd->entity);
#endif

	return 0;
}

/* sensor devices id table */
static const struct i2c_device_id gc2145_mipi_id[] = {
	{ DRIVER_NAME, 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, gc2145_mipi_id);

static struct i2c_driver gc2145_mipi_driver = {
	.driver = {
		.name = DRIVER_NAME,
	},
	.probe = gc2145_probe,
	.remove = gc2145_remove,
	.id_table = gc2145_mipi_id,
};

module_i2c_driver(gc2145_mipi_driver);

