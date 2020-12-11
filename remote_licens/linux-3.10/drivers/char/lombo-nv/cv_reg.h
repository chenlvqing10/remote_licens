/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __CV_REG_H__
#define __CV_REG_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define	CV_CTL				(0x210)
#define CV_STATUS			(0x218)
#define CV_IRQ_EN			(0x220)
#define CV_IRQ_PEND			(0x224)
#define CV_IRQ_CLEAR			(0x228)
#define	RSZ_DATAIN_CFG0			(0x230)
#define	RSZ_DATAIN_CFG1			(0x234)
#define	RSZ_DATAIN_CFG2			(0x238)
#define	RSZ_DATAIN_ADDR			(0x23c)
#define	RSZ_DATAIN_UV_ADDR		(0x240)
#define	RSZ_DATAOUT_CFG0		(0x250)
#define	RSZ_DATAOUT_PYM0_CFG		(0x260)
#define	RSZ_CFG0			(0x290)
#define	RSZ_PYM0_CFG0			(0x2a0)
#define	RSZ_PYM0_CFG1			(0x2a4)
#define	RSZ_ADDR_REG			(0x2e0)

typedef enum _IMAGE_FORMAT {
	RGB_WHC = 0x01,
	YUV420 = 0x02,
	GRAY = 0x03,
	GRAY_WHC = 0x04
} IMAGE_FORMAT;

/* 0x10 */
typedef union {
	int dwval;
	struct {
		int start:1;
		int rvd0:11;
		int op_mode:2;
		int rvd1:2;
		int pym_num:4;
		int rvd2:9;
		int ot_detect_en:1;
		int dbg_ctl:1;
		int rst:1;
	} bits;
} cvu_ctl_t;

/* 0x18 */
typedef union {
	int dwval;
	struct {
		int rvd0:4;
		int tmp_status:2;
		int rvd1:26;
	} bits;
} cvu_status_t;

/* 0x20 */
typedef union {
	int dwval;
	struct {
		int int_en:1;
		int rvd0:3;
		int para_err:1;
		int time_out_err:1;
		int rvd1:10;
		int time_out_cnt:16;
	} bits;
} cvu_irq_en_t;

/* 0x24 */
typedef union {
	int dwval;
	struct {
		int int_pd:1;
		int rvd0:3;
		int para_pd:1;
		int time_out_pd:1;
		int rvd1:26;
	} bits;
} cvu_irq_pend_t;

/* 0x28 */
typedef union {
	int dwval;
	struct {
		int cv_clr:1;
		int rvd0:3;
		int para_clr:1;
		int time_out_clr:1;
		int rvd1:26;
	} bits;
} cvu_irq_clear_t;

/* 0x30 */
typedef union {
	int dwval;
	struct {
		int in_fmt:3;
		int in_chn:11;
		int rvd:1; /* normal_flag: 1; */
		int rvd0:9;
		int stride_mode:3;
		int stride_h:5;
	} bits;
} rsz_datain_cfg0_t;

/* 0x34 */
typedef union {
	int dwval;
	struct {
		int y_stride:12;  /* *32 */
		int rvd0:4;
		int uv_stride:12; /* *32 */
		int rvd1:4;
	} bits;
} rsz_datain_cfg1_t;

/* 0x38 */
typedef union {
	int dwval;
	struct {
		int in_width:12;
		int in_height:12;  /* normal : offset y odd, +1 */
		int rvd0:8;
	} bits;
} rsz_datain_cfg2_t;

/* 0x3c */
typedef union {
	int dwval;
	struct {
		int in_addr:32; /* normal : offset y odd,addr - line */
	} bits;
} rsz_datain_addr_t;

/* 0x40 */
typedef union {
	int dwval;
	struct {
		int in_uv_addr:32; /* normal: y_ddr/2 */
	} bits;
} rsz_datain_uv_addr_t;

/* 0x50 */
typedef union {
	int dwval;
	struct {
		int out_fmt:3;
		int out_chn:11;
		int t_flag:1;
		int rvd0:17;
	} bits;
} rsz_dataout_cfg0_t;

/* 0x60 */
typedef union {
	int dwval;
	struct {
		int out_pym_width:12;
		int out_pym_height:12;
		int out_pym_stride_mode:3;
		int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym0_cfg_t;

/* 0x90 */
typedef union {
	int dwval;
	struct {
		int offset_x:12;
		int offset_y_flag:1;
		int rvd0:11;
		int flip_en:1;
		int pathway_en:1;
		int rvd1:6;
	} bits;
} rsz_cfg0_t;

/* 0xa0  pym0 */
typedef union {
	int dwval;
	struct {
		int scale_x:29;
		int rvd0:3;
	} bits;
} rsz_pym0_cfg0_t;

/* 0xa4 */
typedef union {
	int dwval;
	struct {
		int scale_y:29;
		int rvd0:3;
	} bits;
} rsz_pym0_cfg1_t;

/* 0xe0 */
typedef union {
	int dwval;
	struct {
		int rsz_addr:32;
	} bits;
} rsz_addr_reg_t;

typedef struct {
	int x;
	int y;
	int w;
	int h;
} nv_rect_t;

typedef struct {
	int iw;
	int ih;
	int istride;
	int fmt_in;
	unsigned int phyaddrin;
	nv_rect_t rect;
	int scale_x;
	int scale_y;
	int fmt_out;
	int ow;
	int oh;
	int oc;
	unsigned int phyaddrout;
} rsz_cfg_t;

void cv_register_set(void *regs_base, rsz_cfg_t reg_param);

#endif

