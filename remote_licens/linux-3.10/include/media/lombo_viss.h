/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * LomboTech VISS driver header
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ___LOMBO__VISS__H___
#define ___LOMBO__VISS__H___

#include <linux/sizes.h>
#include <linux/io.h>
#include <linux/irqreturn.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/videodev2.h>

#include <media/media-entity.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mediabus.h>

/*
 * Enumeration of the VISS data bus types.
 */
enum viss_bus_type {
	/* Camera parallel bus */
	VISS_BUS_TYPE_ITU_601 = 1,
	/* Camera parallel bus with embedded synchronization */
	VISS_BUS_TYPE_ITU_656,
	/* Camera MIPI-CSI2 serial bus */
	VISS_BUS_TYPE_MIPI_CSI2,
	/* FIFO link from LCD controller (WriteBack A) */
	VISS_BUS_TYPE_LCD_WRITEBACK_A,
	/* FIFO link from LCD controller (WriteBack B) */
	VISS_BUS_TYPE_LCD_WRITEBACK_B,
	/* FIFO link from FIMC-IS */
	VISS_BUS_TYPE_ISP_WRITEBACK = VISS_BUS_TYPE_LCD_WRITEBACK_B,
};

/* interface type of VISS */
enum viss_if_type {
	VISS_IF_TYPE_DC,
	VISS_IF_TYPE_ITU_601,
	VISS_IF_TYPE_ITU_656,
	VISS_IF_TYPE_ITU_1120,
	VISS_IF_TYPE_MCSI,
	VISS_IF_TYPE_TVD,
};

/* interface type of VISS */
enum viss_mcsi_data_type {
	VISS_MCSI_YUV422,
	VISS_MCSI_RAW8,
	VISS_MCSI_RAW10,
	VISS_MCSI_RAW12,
};

struct i2c_board_info;

#define BT601_TIMING_PARA_CNT			6
struct bt601_timing {
	u32	h_off;
	u32	active_w;
	u32	t_off;
	u32	t_active_l;
	u32	b_off;
	u32	b_active_l;
};

struct mcsi_if_info {
	u32	dlanes;
	u32	data_type;
	u32	mfreq;
};

/**
 * struct viss_source_info - video source description required for the host
 *			     interface configuration
 *
 * @board_info: pointer to I2C subdevice's board info
 * @clk_frequency: frequency of the clock the host interface provides to sensor
 * @if_type: VISS camera input interface type, MIPI, ITU-R BT.601 etc
 *		for VIC: 0: dc; 1: BT.601; 2: BT.656; 3: BT.1120;
 * @flags: the parallel sensor bus flags defining signals polarity (V4L2_MBUS_*)
 * @i2c_bus_num: i2c control bus id the sensor is attached to
 * @host: index of the host
 */
struct viss_source_info {
	struct i2c_board_info *board_info;
	unsigned long clk_frequency;
	enum viss_bus_type viss_bus_type;
	enum viss_bus_type sensor_bus_type;
	enum viss_if_type if_type;
	u32 if_mode;

	u32 interlaced;
	/* dc/bt601/bt656/bt1120/mipi-csi */
	void *if_info;

	u32 flags;
	bool pclk_polarity;
	bool vsync_polarity;
	u16 i2c_bus_num;
	u16 host_id;
	u16 mux_id;
	u8 clk_id;
	u32 isp_id;
	u32 isp_path;
	bool use_isp;
	bool vdd_1_8;

	/* Use 0 to ch_number DMA channels */
	u8 ch_number;
	u8 sort;

	/* width and height of a single channel
	 * Multi channel config,
	 * single channel does not need to be configured
	 */
	u32		sub_width;
	u32		sub_height;

	int (*set_camclk)(struct viss_source_info *si, bool on);
	void *host_data;
};
/**
 * struct n7_platform_viss - camera host interface platform data
 *
 * @source_info: properties of an image source for the host interface setup
 * @num_clients: the number of attached image sources
 */
struct n7_platform_viss {
	struct viss_source_info *source_info;
	int num_clients;
};

#endif /* ___LOMBO__VISS__H___ */
