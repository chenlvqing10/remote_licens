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

#ifndef __CSP_DPU_DC_C__
#define __CSP_DPU_DC_C__

#ifndef DRM_NAME
#define DRM_NAME	"drm"	  /* < Name in Linux kernel, /dev, and /proc */
#endif

#if defined(__KERNEL__)
/* for include printk function */
#include <linux/kernel.h>
/* for include dc.h in mach/csp.h*/
#include <mach/csp.h>
#endif
#include "csp_dpu_dc_reg.h"
#include "csp_dpu_common.h"
#include "csp_dpu_dc.h"

typedef struct tag_blocklinker_reg_backups {
	unsigned int blkctl;
	unsigned int blkcoor;
	unsigned int blkbufadd;
	unsigned int blkbuflnstd;
} blocklinker_reg_backups;


#if !defined(assert)
#define assert(x)
#endif

#define ASSERT_DCINDEX(dc_index)   assert(dc_index < DC_NUM)
#define ASSERT_WININDEX(win_index) do { if (win_index >= DC_WIN_NUM) {   \
			ret = __LINE__;                                  \
			goto exit;                                       \
			}                                                \
			} while (0)
#define ASSERT_BLKINDEX(bl_index)  do { if (bl_index >= DC_BKL_NUM) {    \
			ret = __LINE__;                                  \
			goto exit;                                       \
			}                                                \
			} while (0)

#define DC_HWCCTL_W2REG(w)           (((w) == 32) ? 0 : 1)
#define DC_HWCCTL_H2REG(h)           (((h) == 32) ? 0 : 1)
#define DC_HWCCTL_REG2W(w)           (((w) == 0) ? 32 : 64)
#define DC_HWCCTL_REG2H(h)           (((h) == 0) ? 32 : 64)
#define DC_HWCCTL_SIZE_VALID(w, h)   ((((w) == 32) || ((w) == 64)) && \
				      (((h) == 32) || ((h) == 64)))
#define DC_BLKGNE_FMT(x)             (dpu_format_has_lut(x) ? \
				      4 : DPU_FMT_DCINDEX(x))

/* Enhancement coef bitwidth */
#define DC_CECOEF_BD 14
/* Enhancement coef fraction bitwidth */
#define DC_CECOEF_FBD 10
/* Enhancement constant bitwidth */
#define DC_CECONS_BD 15
/* Enhancemnent constant fraction bitwidth */
#define DC_CECONS_FBD 4

/* The fraction bitwidth for color space convertion table */
#define CSC_COEF_10BIT 10
#define CSC_10BIT_ROUND 0x200

static reg_dc_t *dc[DC_NUM];
static unsigned int dc_size[DC_NUM];
static blocklinker_reg_backups blk_reg_backups[DC_BKL_NUM] = { {0} };

static int rgb2yuv_coef_table_10bit[4][16] = {
	/* ITU_601 FULL */
	{
	/*
	 *  0.29900,  0.58700,  0.11400,   0,
	 * -0.16874, -0.33126,  0.50000, 128,
	 *  0.50000, -0.41869, -0.08131, 128,
	 */
		0x00000132, 0x00000259, 0x00000075, 0x00000000,
		0xFFFFFF53, 0xFFFFFEAD, 0x00000200, 0x00020000,
		0x00000200, 0xFFFFFE53, 0xFFFFFFAD, 0x00020000,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
	/* ITU_601 LIMIT */
	{
	/*
	 *  0.25679,  0.50413,   0.09791,	 16,
	 * -0.14822, -0.29099,   0.43922,	128,
	 *  0.43922, -0.36779,  -0.07143,	128,
	 */
		0x00000107, 0x00000204, 0x00000064, 0x00004000,
		0xFFFFFF68, 0xFFFFFED6, 0x000001C2, 0x00020000,
		0x000001C2, 0xFFFFFE87, 0xFFFFFFB7, 0x00020000,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
	/* ITU_709 FULL */
	{
	/*
	 *  0.21260,  0.71520,  0.07220,  0,
	 * -0.11457, -0.38543,  0.50000, 128,
	 *  0.50000, -0.45415, -0.04585, 128,
	 */
		0x000000DA, 0x000002DC, 0x0000004A, 0x00000000,
		0xFFFFFF8B, 0xFFFFFE75, 0x00000200, 0x00020000,
		0x00000200, 0xFFFFFE2F, 0xFFFFFFD1, 0x00020000,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
	/* ITU_709 LIMIT */
	{
	/*
	 *  0.18259,  0.61423,  0.06201,  16,
	 * -0.10064, -0.33857,  0.43922, 128,
	 *  0.43922, -0.39894, -0.04027, 128,
	 */
		0x000000BB, 0x00000275, 0x0000003F, 0x00004000,
		0xFFFFFF99, 0xFFFFFEA5, 0x000001C2, 0x00020000,
		0x000001C2, 0xFFFFFE67, 0xFFFFFFD7, 0x00020000,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
};

static int yuv2rgb_coef_table_10bit[4][16] = {
	/* ITU_601 FULL */
	{
	/*
	 * 1.00000,  0.00000,  1.40200,   0,
	 * 1.00000, -0.34414, -0.71414, 128,
	 * 1.00000,  1.77200, -0.00000, 128,
	 */
		0x00000400, 0x00000000, 0x0000059C, 0xFFFD322D,
		0x00000400, 0xFFFFFEA0, 0xFFFFFD25, 0x00021DD6,
		0x00000400, 0x00000717, 0x00000000, 0xFFFC74BC,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
	/* ITU_601 LIMIT */
	{
	/*
	 * 1.16438,  0.00000,  1.59603,  16,
	 * 1.16438, -0.39176, -0.81297, 128,
	 * 1.16438,  2.01723, -0.00000, 128,
	 */
		0x000004A8, 0x00000000, 0x00000662, 0xFFFC8450,
		0x000004A8, 0xFFFFFE6F, 0xFFFFFCC0, 0x00021E4D,
		0x000004A8, 0x00000812, 0x00000000, 0xFFFBACA8,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
	/* ITU_709 FULL */
	{
		0x00000400, 0x00000000, 0x0000064D, 0xFFFCD9B4,
		0x00000400, 0xFFFFFF40, 0xFFFFFE21, 0x00014F97,
		0x00000400, 0x0000076C, 0x00000000, 0xFFFC49EF,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
	/* ITU_709 LIMIT */
	{
	/*
	 * 1.16438, -0.00000,  1.79274,  16,
	 * 1.16438, -0.21325, -0.53291, 128,
	 * 1.16438,  2.11240,  0.00000, 128,
	 */
		0x000004A8, 0x00000000, 0x0000072C, 0xFFFC1F99,
		0x000004A8, 0xFFFFFF26, 0xFFFFFDDE, 0x00013383,
		0x000004A8, 0x00000873, 0x00000000, 0xFFFB7BEE,
		0x00000000, 0x00000000, 0x00000000, 0x00000400,
	},
};

/* define address offset for each bpp pixel format and pixel order */
static int pixel_order_tbl[16][32] = {
	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
	  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
	  22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  },    /* 1bpp order0 */
	{  7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13,
	  12, 11, 10,  9,  8, 23, 22, 21, 20, 19, 18,
	  17, 16, 31, 30, 29, 28, 27, 26, 25, 24,  },	/* 1bpp order1 */
	{ 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18,
	  19, 20, 21, 22, 23,  8,  9, 10, 11, 12, 13,
	  14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  },	/* 1bpp order2 */
	{ 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
	  20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,
	   9,  8,  7,  6,  5,  4,  3,  2,  1,  0,  },	/* 1bpp order3 */

	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
	  11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 2bpp order0 */
	{  3,  2,  1,  0,  7,  6,  5,  4, 11, 10,  9,
	   8, 15, 14, 13, 12,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 2bpp order1 */
	{ 12, 13, 14, 15,  8,  9, 10, 11,  4,  5,  6,
	   7,  0,  1,  2,  3,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 2bpp order2 */
	{ 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,
	   5,  4,  3,  2,  1,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, },  /* 2bpp order3 */

	{  0,  1,  2,  3,  4,  5,  6,  7,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 4bpp order0 */
	{  1,  0,  3,  2,  5,  4,  7,  6,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 4bpp order1 */
	{  6,  7,  4,  5,  2,  3,  0,  1,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 4bpp order2 */
	{  7,  6,  5,  4,  3,  2,  1,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 4bpp order3 */

	{  0,  1,  2,  3,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 8bpp order0 */
	{  0,  1,  2,  3,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 8bpp order1 */
	{  3,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 8bpp order2 */
	{  3,  2,  1,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  },	/* 8bpp order3 */
};

/*
 * matrix multiply matrix
 * Each matrix must be 4by4.
 *
 * @in0/in1 : input matrix
 * @out     : output matrix
 * @shiftbit: right shift bit for adjust decimal point
 */
static void matrix_mul_matrix(const dc_matrix *in0, const dc_matrix *in1,
						 dc_matrix *out, int shift_bit)
{
	/*
	 * Matrix multiply
	 * row 0
	 */
	out->m00 = in0->m00 * in1->m00 + in0->m01 * in1->m10 +
		   in0->m02 * in1->m20 + in0->m03 * in1->m30;
	out->m01 = in0->m00 * in1->m01 + in0->m01 * in1->m11 +
		   in0->m02 * in1->m21 + in0->m03 * in1->m31;
	out->m02 = in0->m00 * in1->m02 + in0->m01 * in1->m12 +
		   in0->m02 * in1->m22 + in0->m03 * in1->m32;
	out->m03 = in0->m00 * in1->m03 + in0->m01 * in1->m13 +
		   in0->m02 * in1->m23 + in0->m03 * in1->m33;

	/* row 1 */
	out->m10 = in0->m10 * in1->m00 + in0->m11 * in1->m10 +
		   in0->m12 * in1->m20 + in0->m13 * in1->m30;
	out->m11 = in0->m10 * in1->m01 + in0->m11 * in1->m11 +
		   in0->m12 * in1->m21 + in0->m13 * in1->m31;
	out->m12 = in0->m10 * in1->m02 + in0->m11 * in1->m12 +
		   in0->m12 * in1->m22 + in0->m13 * in1->m32;
	out->m13 = in0->m10 * in1->m03 + in0->m11 * in1->m13 +
		   in0->m12 * in1->m23 + in0->m13 * in1->m33;

	/* row 2 */
	out->m20 = in0->m20 * in1->m00 + in0->m21 * in1->m10 +
		   in0->m22 * in1->m20 + in0->m23 * in1->m30;
	out->m21 = in0->m20 * in1->m01 + in0->m21 * in1->m11 +
		   in0->m22 * in1->m21 + in0->m23 * in1->m31;
	out->m22 = in0->m20 * in1->m02 + in0->m21 * in1->m12 +
		   in0->m22 * in1->m22 + in0->m23 * in1->m32;
	out->m23 = in0->m20 * in1->m03 + in0->m21 * in1->m13 +
		   in0->m22 * in1->m23 + in0->m23 * in1->m33;

	/* row 3 */
	out->m30 = in0->m30 * in1->m00 + in0->m31 * in1->m10 +
		   in0->m32 * in1->m20 + in0->m33 * in1->m30;
	out->m31 = in0->m30 * in1->m01 + in0->m31 * in1->m11 +
		   in0->m32 * in1->m21 + in0->m33 * in1->m31;
	out->m32 = in0->m30 * in1->m02 + in0->m31 * in1->m12 +
		   in0->m32 * in1->m22 + in0->m33 * in1->m32;
	out->m33 = in0->m30 * in1->m03 + in0->m31 * in1->m13 +
		   in0->m32 * in1->m23 + in0->m33 * in1->m33;

	/* adjust decimal bit */
	out->m00 >>= shift_bit;
	out->m01 >>= shift_bit;
	out->m02 >>= shift_bit;
	out->m03 >>= shift_bit;

	out->m10 >>= shift_bit;
	out->m11 >>= shift_bit;
	out->m12 >>= shift_bit;
	out->m13 >>= shift_bit;

	out->m20 >>= shift_bit;
	out->m21 >>= shift_bit;
	out->m22 >>= shift_bit;
	out->m23 >>= shift_bit;

	out->m30 >>= shift_bit;
	out->m31 >>= shift_bit;
	out->m32 >>= shift_bit;
	out->m33 >>= shift_bit;
}

/* matrix copy from src to dst */
static void dc_matrix_copy(dc_matrix *dst, const dc_matrix *src)
{
	/* row 0 */
	dst->m00 = src->m00;
	dst->m01 = src->m01;
	dst->m02 = src->m02;
	dst->m03 = src->m03;

	/* row 1 */
	dst->m10 = src->m10;
	dst->m11 = src->m11;
	dst->m12 = src->m12;
	dst->m13 = src->m13;

	/* row 2 */
	dst->m20 = src->m20;
	dst->m21 = src->m21;
	dst->m22 = src->m22;
	dst->m23 = src->m23;

	/* row 3 */
	dst->m30 = src->m30;
	dst->m31 = src->m31;
	dst->m32 = src->m32;
	dst->m33 = src->m33;
}

static int dc_update_lut(unsigned char *dst,
	const unsigned char *lut, unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned int i;
	const unsigned int *ptr = (const unsigned int *)lut;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	if (lut == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if ((offset + bytes) > DC_LUT_MAX_SIZE) {
		ret = __LINE__;
		goto exit;
	}
	/* address must be 4bytes align */
	if ((((unsigned long)lut) & (0x4 - 1)) != 0) {
		ret = __LINE__;
		goto exit;
	}
	/* offset and bytes must be 4bytes align */
	if (((bytes & (0x4 - 1)) != 0) ||
		((offset & (0x4 - 1)) != 0)) {
		ret = __LINE__;
		goto exit;
	}

	base = (unsigned long)dst;

	for (i = 0; i < bytes / 4; i++)
		WRITEREG32_RELAX(base + offset + i * 4, ptr[i]);

exit:
	return ret;
}

static int dc_get_lut(unsigned char *dst,
	unsigned char *lut, unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned int i;
	unsigned int *ptr = (unsigned int *)lut;
	unsigned long base;

	if (lut == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if ((offset + bytes) > DC_LUT_MAX_SIZE) {
		ret = __LINE__;
		goto exit;
	}
	/* address must be 4bytes align */
	if ((((unsigned long)lut) & (0x4 - 1)) != 0) {
		ret = __LINE__;
		goto exit;
	}
	/* offset and bytes must be 4bytes align */
	if (((bytes & (0x4 - 1)) != 0) ||
		((offset & (0x4 - 1)) != 0)) {
		ret = __LINE__;
		goto exit;
	}

	base = (unsigned long)dst;
	for (i = 0; i < bytes / 4; i++)
		ptr[i] = READREG32_RELAX(base + offset + i * 4);

exit:
	return ret;
}

/*
 * Global control interface
 * Set register base address
 */
int csp_dc_set_register_base(unsigned int dc_index, unsigned long addr,
			     unsigned int size)
{
	ASSERT_DCINDEX(dc_index);
	assert(addr != 0);
	assert(size >= DC_END_REG);

	dc[dc_index] = (reg_dc_t *)addr;
	dc_size[dc_index] = size;

	return DPU_OK;
}

/* Get register base address */
int csp_dc_get_register_base(unsigned int dc_index, unsigned long *addr)
{
	ASSERT_DCINDEX(dc_index);
	assert(addr != NULL);

	*addr = (unsigned long)dc[dc_index];

	return DPU_OK;
}

/* Initialize dc */
int csp_dc_init(unsigned int dc_index)
{
	unsigned int i;

	for (i = 0; i < DC_WIN_NUM; i++)
		csp_dc_window_set_blend_rule(dc_index, i, 0, DPU_BLEND_DEFAULT);
	csp_dc_blocklinker_set_blend_rule(dc_index, 0, DPU_BLEND_DEFAULT);
	csp_dc_disable_pipe(dc_index, 0);
	csp_dc_disable_pipe(dc_index, 1);

	/* Improve the DC module performance */
	WRITEREG32((char *)dc[dc_index] + 0x140, 0x00000001);
	/* max burst length */
	WRITEREG32((char *)dc[dc_index] + 0x144, 0x00f1b6db);

	return DPU_OK;
}

/* Deinitialize dc */
int csp_dc_exit(unsigned int dc_index)
{
	ASSERT_DCINDEX(dc_index);
	/* Now dont' need to do anything */

	dc[dc_index] = (reg_dc_t *)NULL;
	dc_size[dc_index] = 0;

	return DPU_OK;
}

/* Enable dc */
int csp_dc_enable(unsigned int dc_index)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);

	/* Start dc */
	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	reg.bits.dcrst = 1;
	reg.bits.fiforst = 1;
	WRITEREG32(reg_addr, reg.val);

	/* Enable dc */
	reg.bits.dcen = 1;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Disable dc */
int csp_dc_disable(unsigned int dc_index)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);

	/* Disable dc */
	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	reg.bits.dcen = 0;
	WRITEREG32(reg_addr, reg.val);

	/* Reset dc */
	reg.bits.dcrst = 0;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/**
 * Set dc output mode
 * @itl: 0: output progreesive, 1: output interlace
 * @field_pol: field polarity, valid when itl is 1
 *             0:Low/High(Field 1/2) output (0/2/4/..)/(1/3/5/..) line
 *             1:Low/High(Field 1/2) output (1/3/5/..)/(0/2/4/..) line
 */
int csp_dc_set_output_mode(unsigned int dc_index, unsigned int itl,
			   unsigned int field_pol)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	reg.bits.outmode = itl;
	reg.bits.fieldpol = field_pol;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* set backcolor */
int csp_dc_set_backcolor(unsigned int dc_index, const dpu_color_t *color)
{
	reg_dc_bkcolor_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	assert(color != NULL);

	reg_addr = (unsigned int *)&dc[dc_index]->bkcolor;
	/* Fill backcolor component and set it */
	reg.bits.b = color->b;
	reg.bits.g = color->g;
	reg.bits.r = color->r;
	reg.bits.a = color->a;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* get current backcolor */
int csp_dc_get_backcolor(unsigned int dc_index, dpu_color_t *color)
{
	reg_dc_bkcolor_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	assert(color != NULL);

	reg_addr = (unsigned int *)&dc[dc_index]->bkcolor;
	/* Read backcolor */
	reg.val = READREG32(reg_addr);
	color->b = reg.bits.b;
	color->g = reg.bits.g;
	color->r = reg.bits.r;
	color->a = reg.bits.a;

	return DPU_OK;
}

/*
 * Enable pipe function when we want to reduce power consumption
 * @pipe: pipe index
 */
int csp_dc_enable_pipe(unsigned int dc_index, unsigned int pipe)
{
	reg_dc_imgsrcctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	assert(pipe < DC_PIPE_NUM);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	/*
	 * This bit means PIPE_BYPASS,
	 * so we clear it when we want to enable pipe
	 */
	if (pipe == 0)
		reg.bits.pipe0bypass = 0;
	else
		reg.bits.pipe1bypass = 0;

	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/*
 * Disable pipe function when we want to blend/colorkey effect
 * @pipe: pipe index
 */
int csp_dc_disable_pipe(unsigned int dc_index, unsigned int pipe)
{
	reg_dc_imgsrcctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	assert(pipe < DC_PIPE_NUM);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	/*
	 * This bit means PIPE_BYPASS,
	 * so we set it when we want to disable pipe
	 */
	if (pipe == 0)
		reg.bits.pipe0bypass = 1;
	else
		reg.bits.pipe1bypass = 1;

	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/*
 * Set screen resolution
 * @width/height: in pixel unit
 */
int csp_dc_set_screen_size(unsigned int dc_index, unsigned int width,
			   unsigned int height)
{
	int ret = DPU_OK;
	reg_dc_scrsiz_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	if ((width == 0) ||  (width > DC_MAX_WIDTH) ||
	    (height == 0) ||  (height > DC_MAX_HEIGHT)) {
		ret = __LINE__;
	} else {
		reg_addr = (unsigned int *)&dc[dc_index]->scrsiz;
		reg.val = 0;
		reg.bits.w = width - 1;
		reg.bits.h = height - 1;
		WRITEREG32(reg_addr, reg.val);
		ret = DPU_OK;
	}

	return ret;
}

/*
 * Get screen resolution
 * @width/height: in pixel unit
 */
int csp_dc_get_screen_size(unsigned int dc_index, unsigned int *width,
			   unsigned int *height)
{
	int ret;

	reg_dc_scrsiz_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	if ((width == NULL) || (height == NULL)) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->scrsiz;
	reg.val = READREG32(reg_addr);
	*width = reg.bits.w + 1;
	*height = reg.bits.h + 1;

	ret = DPU_OK;

exit:
	return ret;

}

/*
 * Load double buffer register
 * All control register in dc are in double buffer mode,
 * After we update the config of dc, we need to load
 * the double buffer register content into registers
 */
int csp_dc_load_dbr(unsigned int dc_index)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	/* Start load double buffer register */
	reg.bits.dbc = 1;
	/* Disable double buffer register auto load function */
	reg.bits.dbautold = 1;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

int csp_dc_unload_dbr(unsigned int dc_index)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	/* Start load double buffer register */
	reg.bits.dbc = 0;
	/* Disable double buffer register auto load function */
	reg.bits.dbautold = 1;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

int csp_dc_autoload_dbr(unsigned int dc_index)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	/* Start load double buffer register */
	reg.bits.dbc = 0;
	/* Disable double buffer register auto load function */
	reg.bits.dbautold = 0;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/*
 * After we load double buffer registe content into register,
 * we may need to check whether it has finished yet!
 */
bool csp_dc_is_load_dbr_finish(unsigned int dc_index)
{
	reg_dc_gnectl_t reg;
	unsigned int *reg_addr;
	bool ret = false;

	ASSERT_DCINDEX(dc_index);

	/*
	 * When we want to load double buffer register, we set dbc bit,
	 * and this bit will be self clear when it finish.
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->gnectl;
	reg.val = READREG32(reg_addr);
	if (reg.bits.dbc == 0)
		ret = true;

	return ret;
}
/*
 * enhance equation is as follows:
 * Y = ((Y - 16) x C) + B + 16
 * U = ((U - 128) x C x S) + 128
 * V = ((V - 128) x C x S) + 128
 *
 * @m: 4by4 matrix, S.10(10 bit fraction)
 * @bright/saturation/contrast: 0~100
 */
static void dc_fill_enhance_matrix(dc_matrix *m, unsigned int brightness,
	unsigned int saturation, unsigned int contrast)
{
	int b = (int)(brightness * 128 / 100);
	/* s: S1.5, 1 integer and 5 fraction */
	int s = (int)(saturation * 64 / 100);
	/* c: S1.5 */
	int c = (int)(contrast * 32 / 100);

	/* b: [-64, 64), 6 integer and 1 sign */
	b = b - 64;
	/* +0.5, [16, 48] */
	c += (1 << 4);

	/* S1.5 -> S1.10 */
	m->m00 = c << 5;
	m->m01 = 0;
	m->m02 = 0;
	/* b: integer -> S.10, c: S1.5 -> S1.10 */
	m->m03 = (b << 10) + (16 << 10) - 16 * (c << 5);

	m->m10 = 0;
	/* S1.5 * S1.5 -> S2.10 */
	m->m11 = c * s;
	m->m12 = 0;
	/* c*s: S1.5 * S1.5 -> S2.10 */
	m->m13 = (128 << 10) - 128 * (c * s);

	m->m20 = 0;
	m->m21 = 0;
	m->m22 = c * s;
	/* c*s: S1.5 * S1.5 -> S2.10 */
	m->m23 = (128 << 10) - 128 * (c * s);

	m->m30 = 0;
	m->m31 = 0;
	m->m32 = 0;
	/* 1: S1.10 */
	m->m33 = 1 << 10;
}

/* @shift_bit: right shift bit */
static void dc_limit_sign_int(int *val, unsigned int shift_bit,
			      unsigned int bit_width)
{
	unsigned int mask = 0xFFFFFFFF;
	unsigned int round;

	assert(bit_width >= 1);

	if (shift_bit != 0) {
		round = (*val >> (shift_bit - 1)) & 0x1;
		*val = (*val + round) >> shift_bit;
	}
	if (bit_width != 32)
		mask = (1U << bit_width) - 1;

	*val = (int)(((unsigned int)(*val)) & mask);
}

/* Set output data format/range and color adjustment */
int csp_dc_set_enhancement(unsigned int dc_index, const dc_enhance_info_t *info)
{
	unsigned int csc_enable = 0;
	dc_matrix *m0;
	dc_matrix matrix_enhance, matrix_result;
	reg_dc_ceen_t reg;
	unsigned int *reg_addr;
	unsigned int index;

	ASSERT_DCINDEX(dc_index);
	assert(info != NULL);

	if (info->fmt == DPU_FMT_RGB888) {
		if (info->enhance_enable == 1) {
			/*
			 * All the enhancement will be done in yuv gamut. So,
			 * first of all, we convert all the pixel from
			 * rgb gamut to yuv gamut,
			 * and then adjust brightness/saturation/contrast level,
			 * finally, we convert them back to rgb gamut.
			 */

			/*
			 * Each step, we use a convertion matrix. So there will
			 * be 3 matrixs,
			 * rgb2yuv matrix, enhancement matrix, yuv2rgb matrix.
			 * But there is only one matrix in the hareware,
			 * so we need to calculate
			 * the final matrix by software. The equation is
			 * as below:
			 * Final matrix = yuv2rgb x (enhancement x rgb2yuv)
			 */
			csc_enable = 1;
			/* Using bt709 limit */
			index = (DPU_CS_BT709 << 1) | DPU_DATA_LIMIT_RANGE;

			assert(index < ARRARSIZE(rgb2yuv_coef_table_10bit));

			m0 = (dc_matrix *)rgb2yuv_coef_table_10bit[index];
			dc_fill_enhance_matrix(&matrix_enhance,
						info->brightness,
						info->saturation,
						info->contrast);
			matrix_mul_matrix(&matrix_enhance,
				m0, &matrix_result, CSC_COEF_10BIT);
			m0 = (dc_matrix *)yuv2rgb_coef_table_10bit[index];
			dc_matrix_copy(&matrix_enhance, &matrix_result);
			matrix_mul_matrix(m0, &matrix_enhance, &matrix_result,
						CSC_COEF_10BIT);
		} else {
			/*
			 * Since input and output are all in rgb gamut,
			 * and no enhancemnet,
			 * there's nothing to do except for
			 * disable csc function.
			 */
			csc_enable = 0;
		}
	} else {
		index = (info->csstd << 1) | info->range;

		assert(index < ARRARSIZE(rgb2yuv_coef_table_10bit));

		csc_enable = 1;

		if (info->enhance_enable == 1) {
			/*
			 * All the enhancement will be done in yuv gamut. So,
			 * first of all, we convert all the pixel from
			 * rgb gamut to yuv gamut,
			 * and then adjust brightness/saturation/contrast level.
			 */
			m0 = (dc_matrix *)rgb2yuv_coef_table_10bit[index];
			dc_fill_enhance_matrix(&matrix_enhance,
						info->brightness,
						info->saturation,
						info->contrast);
			matrix_mul_matrix(&matrix_enhance, m0, &matrix_result,
						CSC_COEF_10BIT);
		} else {
			/*
			 * When output is in yuv gamut, and no enhancement,
			 * we only need to convert the color to yuv gamut.
			 */
			m0 = (dc_matrix *)rgb2yuv_coef_table_10bit[index];
			dc_matrix_copy(&matrix_result, m0);
		}
	}

	if (csc_enable == 1) {
		/* limit coef range */
		dc_limit_sign_int(&matrix_result.m00, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);
		dc_limit_sign_int(&matrix_result.m01, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);
		dc_limit_sign_int(&matrix_result.m02, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);

		dc_limit_sign_int(&matrix_result.m10, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);
		dc_limit_sign_int(&matrix_result.m11, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);
		dc_limit_sign_int(&matrix_result.m12, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);

		dc_limit_sign_int(&matrix_result.m20, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);
		dc_limit_sign_int(&matrix_result.m21, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);
		dc_limit_sign_int(&matrix_result.m22, CSC_COEF_10BIT -
				   DC_CECOEF_FBD, DC_CECOEF_BD);

		/* limit constant range */
		dc_limit_sign_int(&matrix_result.m03, CSC_COEF_10BIT -
				   DC_CECONS_FBD, DC_CECONS_BD);
		dc_limit_sign_int(&matrix_result.m13, CSC_COEF_10BIT -
				   DC_CECONS_FBD, DC_CECONS_BD);
		dc_limit_sign_int(&matrix_result.m23, CSC_COEF_10BIT -
				   DC_CECONS_FBD, DC_CECONS_BD);
	}

	reg_addr = (unsigned int *)&dc[dc_index]->ceen;
	reg.val = READREG32(reg_addr);
	if (csc_enable == 1) {
		unsigned int *src = (unsigned int *)&matrix_result.m00;
		unsigned int *dst = (unsigned int *)&dc[dc_index]->cerrcoef;
		unsigned int i, temp;

		/* Fill all the value of matrix in the register */
		for (i = 0; i < DC_CECOEF_NUM; i++) {
			temp = READREG32(src++);
			WRITEREG32(dst++, temp);
		}

		/* Enable color enhance function */
		reg.bits.en = 1;
	} else {
		/* Disable color enhance function */
		reg.bits.en = 0;
	}
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Window control interface */
/* Show window on the screen */
int csp_dc_window_show(unsigned int dc_index, unsigned int win_index)
{
	int ret = DPU_OK;
	reg_dc_imgsrcctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	switch (win_index) {
	case 0:
		reg.bits.win0en = 1;
		break;
	case 1:
		reg.bits.win1en = 1;
		break;
	case 2:
		reg.bits.win2en = 1;
		break;
	case 3:
		reg.bits.win3en = 1;
		break;
	default:
		ret = __LINE__;
		break;
	}
	WRITEREG32(reg_addr, reg.val);
	ret = DPU_OK;

exit:
	return ret;
}

/* Hide window on the screen */
int csp_dc_window_hide(unsigned int dc_index, unsigned int win_index)
{
	int ret = DPU_OK;
	reg_dc_imgsrcctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	switch (win_index) {
	case 0:
		reg.bits.win0en = 0;
		break;
	case 1:
		reg.bits.win1en = 0;
		break;
	case 2:
		reg.bits.win2en = 0;
		break;
	case 3:
		reg.bits.win3en = 0;
		break;
	default:
		ret = __LINE__;
		break;
	}

	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Set alpha mode for specified window
 * There are three alpha mode to be choose, When
 * each pixel carrys its alpha, you are recommended
 * to choose PIXEL_ALPHA or PLANE_PIXEL_ALPHA for
 * better effect.
 * If pixel doesn't have its own alpha, or you just
 * want the winow to overlay on another window,
 * you are recommended to choose PLANE_ALPHA
 * and fix plane alpha to 0XFF
 */
int csp_dc_window_set_alpha_mode(unsigned int dc_index, unsigned int win_index,
				 dpu_alpha_mode_t mode)
{
	int ret = DPU_OK;
	reg_dc_win0ctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (mode >= DPU_ALPHA_MODE_MAX) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	reg.bits.alphactl = mode;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get alpha mode */
int csp_dc_window_get_alpha_mode(unsigned int dc_index, unsigned int win_index,
				 dpu_alpha_mode_t *mode)
{
	int ret = DPU_OK;
	reg_dc_win0ctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (mode == NULL) {
		ret = __LINE__;
		goto exit;
	}
	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	*mode = (dpu_alpha_mode_t)reg.bits.alphactl;

exit:
	return ret;
}

/*
 * Set plane alpha value, only valid on
 * PLANE_ALPHA or PLANE_PIXEL_ALPHA mode
 */
int csp_dc_window_set_alpha_value(unsigned int dc_index, unsigned int win_index,
				  unsigned int value)
{
	int ret = DPU_OK;
	reg_dc_win0ctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (value > DC_ALPHA_MAX_VALUE) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	reg.bits.plaalpha = value;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get plane alpha value */
int csp_dc_window_get_alpha_value(unsigned int dc_index, unsigned int win_index,
				  unsigned int *value)
{
	int ret = DPU_OK;
	reg_dc_win0ctl_t reg;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	if ((win_index >= DC_WIN_NUM) ||
		(value == NULL)) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	*value = reg.bits.plaalpha;

exit:
	return ret;
}

/* Set the destination rectangle where the window show on the screen. */
int csp_dc_window_set_dest_rectangle(unsigned int dc_index,
			unsigned int win_index, unsigned int planes,
			const dpu_rectangle_t *rect)
{
	int ret = DPU_OK;
	reg_dc_win0siz_t reg;
	reg_dc_win0coor_t reg_coor;
	unsigned int *reg_addr;
	unsigned int i;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	for (i = 0; i < planes; i++) {
		/*
		 * Get the WINSIZ register base,
		 * and then move forward to the specified one
		 */
		reg_addr = (unsigned int *)&dc[dc_index]->win0siz;
		reg_addr += win_index + i;
		reg.val = 0;
		reg.bits.w = (rect[i].w == 0) ? 0 : (rect[i].w - 1);
		reg.bits.h = (rect[i].h == 0) ? 0 : (rect[i].h - 1);
		WRITEREG32(reg_addr, reg.val);
	}

	/*
	 * Get the WINCOOR register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0coor;
	reg_addr += win_index;
	reg_coor.val = 0;
	reg_coor.bits.x = rect[0].x;
	reg_coor.bits.y = rect[0].y;
	WRITEREG32(reg_addr, reg_coor.val);

exit:
	return ret;
}

/* Get the destination rectangle where the window show on the screen. */
int csp_dc_window_get_dest_rectangle(unsigned int dc_index,
				     unsigned int win_index,
				     dpu_rectangle_t *rect)
{
	int ret = DPU_OK;
	reg_dc_win0siz_t reg;
	reg_dc_win0coor_t reg_coor;
	unsigned int *reg_addr;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINSIZ register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0siz;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	rect->w = reg.bits.w + 1;
	rect->h = reg.bits.h + 1;

	/*
	 * Get the WINCOOR register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0coor;
	reg_addr += win_index;
	reg_coor.val = READREG32(reg_addr);
	rect->x = (int)((short)(reg_coor.bits.x));
	rect->y = (int)((short)(reg_coor.bits.y));

exit:
	return ret;
}

/*
 * csp_dc_window_set_buffer - set buffer related information
 *
 * @dc_index  : Specify which dc
 * @win_index : Specify which window
 * @fb        : Buffer related information
 * Returns negative errno on error, or Zero for success.
 */
int csp_dc_window_set_buffer(unsigned int dc_index, unsigned int win_index,
			     const dpu_fb_t *fb)
{
	long long offset;
	unsigned long long bit_addr[3];     /*In bit unit */
	int ret = DPU_OK;
	unsigned int haddr[3], laddr[3]; /* High address, low address */
	unsigned int *reg_addr;
	unsigned int i;
	unsigned int bpp;
	reg_dc_win0buffmt_t   reg;
	reg_dc_win0bufladd_t  reg_bufladdr;
	reg_dc_winbufhadd_t   reg_bufhaddr;
	reg_dc_win0buflnstd_t reg_buflnstd;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (fb == NULL) {
		ret = __LINE__;
		goto exit;
	}
	bpp = dpu_format_get_bits_per_pixel(fb->format, 0);
	/* bpp must be: 0 < bpp <= 32 */
	if ((bpp == 0) || (bpp > DC_FMT_BPP_MAX)) {
		ret = __LINE__;
		goto exit;
	}
	/*
	 * Get the WINBUFFMT register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0buffmt;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	/* Set buffer format and pixel order */
	reg.bits.fmt = DPU_FMT_DCINDEX(fb->format);
	reg.bits.po = (dpu_pixel_order_t)fb->pixel_order;
	WRITEREG32(reg_addr, reg.val);

	/*
	 * When we clip a image, we need to calculate
	 * the address of start point.
	 * The formula is as below(all in byte unit):
	 * clip start Address =  buffer address + y * line_stride + x * Bpp
	 * In practice, dc needs address in bit,
	 * so we use bit address in the code
	 */
	offset = fb->addr[0];

	/* adjust address acoording to pixel order */
	if (fb->pixel_order != 0) {
		/* handle palette format */
		if (bpp <= 8) {
			/* calculate pixel index and order */
			unsigned int oidx;
			unsigned int pidx;
			long long offset2;

			oidx = DPU_FMT_DCINDEX(fb->format) * 4 +
					fb->pixel_order;
			pidx = (unsigned int)(offset & 0x1f) / bpp;
			assert(oidx < 8);
			assert(pidx < 32);
			/*
			 * fetch address offset
			 * for this format and pixel order
			 */
			offset2 = ((long long)pixel_order_tbl[oidx][pidx] -
				(long long)pidx) * bpp;
			assert((offset2 > -32) && (offset2 < 32));

			/* revise offset */
			offset += offset2;
			assert(offset >= 0);
		} else if ((bpp == 16) &&
			  ((fb->pixel_order & DPU_PO_PIXEL_REVERT_IN_WORD) ==
			    DPU_PO_PIXEL_REVERT_IN_WORD)) {
			/* handle 16bit format */
			unsigned int pixel_idx = (offset & 0x1f) / bpp;

			/* revise offset */
			if (pixel_idx == 0)
				offset += bpp;
			else
				offset -= bpp;
			assert(offset >= 0);
		}
	}

	bit_addr[0] = (unsigned long long)offset;
	bit_addr[1] = fb->addr[1];

	for (i = 0; i < fb->planes; i++) {
		haddr[i] = (unsigned int)(bit_addr[i] >> 32);
		laddr[i] = (unsigned int)(bit_addr[i] & 0xffffffff);

		/* Set the low 32bit buffer address */
		reg_addr = (unsigned int *)&dc[dc_index]->win0bufladd;
		reg_addr += win_index + i;
		reg_bufladdr.bits.add = laddr[i];
		WRITEREG32(reg_addr, reg_bufladdr.val);

		/* Set the high 4bit buffer address */
		reg_addr = (unsigned int *)&dc[dc_index]->winbufhadd;
		reg_bufhaddr.val = READREG32(reg_addr);
		reg_bufhaddr.val &= ~((0xFF) << 8 * (win_index + i));
		reg_bufhaddr.val |= (haddr[i] << 8 * (win_index + i));
		WRITEREG32(reg_addr, reg_bufhaddr.val);

		/* Set the buffer linestride in bit unit */
		reg_addr = (unsigned int *)&dc[dc_index]->win0buflnstd;
		reg_addr += win_index + i;
		reg_buflnstd.bits.std = fb->linestride_bit[i];
		WRITEREG32(reg_addr, reg_buflnstd.val);
	}

exit:
	return ret;
}

/* We can't get the correct fb result, this interface is only for test */
int csp_dc_window_get_buffer(unsigned int dc_index, unsigned int win_index,
			     dpu_fb_t *fb)
{
	int ret = DPU_OK;

	return ret;
}

/*
 * Set the data source of window
 * Normally, it comes from IDMA.
 * It can also come from SCALING ENGINE
 * when it need to be extra process(scaling/yuv format/enhance)
 */
int csp_dc_window_set_data_source(unsigned int dc_index, unsigned int win_index,
				  dc_window_src_t winsrc)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_win0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (winsrc >= DC_WINDOW_SRC_MAX) {
		ret = __LINE__;
		goto exit;
	}
	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	reg.bits.winsrc = winsrc;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get the data source of window */
int csp_dc_window_get_data_source(unsigned int dc_index, unsigned int win_index,
				  unsigned int *winsrc)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_win0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (winsrc == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	*winsrc = reg.bits.winsrc;

exit:
	return ret;
}

/*
 * Set the work mode of window
 * The mode depends on the format and other requirement.
 * If the image need to do gamma correction, choose GAMMA MODE.
 * If the image format is 1/2/4/8bpp, and you want to save power consumption
 * extremely, recommend to choose IFB MODE.
 * If the image format is 1/2/4/8bpp, please choose PALETTE MODE,
 * and choose NORMAL MODE, when image format is normal rgb format.
 */
int csp_dc_window_set_work_mode(unsigned int dc_index, unsigned int win_index,
				dpu_window_work_mode_t winwkmod)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_win0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (winwkmod >= DPU_WINDOW_WORK_MODE_MAX) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	reg.bits.winwkmod = (dpu_window_work_mode_t)winwkmod;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get the work mode of window */
int csp_dc_window_get_work_mode(unsigned int dc_index, unsigned int win_index,
				dpu_window_work_mode_t *winwkmod)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_win0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (winwkmod == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the WINCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->win0ctl;
	reg_addr += win_index;
	reg.val = READREG32(reg_addr);
	*winwkmod = (dpu_window_work_mode_t)reg.bits.winwkmod;

exit:
	return ret;
}

/*
 * Set color key configuration
 * When you want to mask some color of the image,
 * you can try color key funtion. And you need to
 * provide color range and match rule
 */
int csp_dc_window_set_colorkey(unsigned int dc_index, unsigned int win_index,
			       const dc_colorkey_t *ck)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_bld0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (ck == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (ck->target >= DC_COLORKEY_TARGET_MAX) {
		ret = __LINE__;
		goto exit;
	}
	if ((ck->red_rule >= DC_COLORKEY_RULE_MAX) ||
	    (ck->green_rule >= DC_COLORKEY_RULE_MAX) ||
	    (ck->blue_rule >= DC_COLORKEY_RULE_MAX)) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the BLDCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ctl;
	reg_addr += win_index;
	/* Set color key match target and match rule */
	reg.val = READREG32(reg_addr);
	reg.bits.ckmatch = ck->target;
	reg.bits.ckrcon = ck->red_rule;
	reg.bits.ckgcon = ck->green_rule;
	reg.bits.ckbcon = ck->blue_rule;
	WRITEREG32(reg_addr, reg.val);

	/*
	 * Get the BLDCKMIN register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ckmin;
	reg_addr += win_index;
	/* Set the minimal color */
	WRITEREG32(reg_addr, DPU_COLOR(ck->min.a,
				      ck->min.r,
				      ck->min.g,
				      ck->min.b));

	/*
	 * Get the BLDCKMAX register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ckmax;
	reg_addr += win_index;
	/* Set the maximal color */
	WRITEREG32(reg_addr, DPU_COLOR(ck->max.a,
				      ck->max.r,
				      ck->max.g,
				      ck->max.b));

exit:
	return ret;
}

/* Get colorkey configuration */
int csp_dc_window_get_colorkey(unsigned int dc_index, unsigned int win_index,
			       dc_colorkey_t *ck)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_bld0ctl_t reg;
	reg_dc_bld0ckmin_t bld0ckmin;
	reg_dc_bld0ckmax_t bld0ckmax;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (ck == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the BLDCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ctl;
	reg_addr += win_index;
	/* Get color key match target and match rule */
	reg.val = READREG32(reg_addr);
	ck->target = (dc_colorkey_target_t)reg.bits.ckmatch;
	ck->red_rule = (dc_colorkey_rule_t)reg.bits.ckrcon;
	ck->green_rule = (dc_colorkey_rule_t)reg.bits.ckgcon;
	ck->blue_rule = (dc_colorkey_rule_t)reg.bits.ckbcon;

	/*
	 * Get the BLDCKMIN register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ckmin;
	reg_addr += win_index;
	/* Get the minimal color */
	bld0ckmin.val = READREG32(reg_addr);
	ck->min.a = 0xff;
	ck->min.r = bld0ckmin.bits.ckrmin;
	ck->min.g = bld0ckmin.bits.ckgmin;
	ck->min.b = bld0ckmin.bits.ckbmin;

	/*
	 * Get the BLDCKMAX register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ckmax;
	reg_addr += win_index;
	/* Get the maximal color */
	bld0ckmax.val = READREG32(reg_addr);
	ck->max.a = 0xff;
	ck->max.r = bld0ckmax.bits.ckrmax;
	ck->max.g = bld0ckmax.bits.ckgmax;
	ck->max.b = bld0ckmax.bits.ckbmax;

exit:
	return ret;
}

/*
 * Enble colorkey
 */
int csp_dc_window_enable_colorkey(unsigned int dc_index, unsigned int win_index)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_bld0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	/*
	 * Get the BLDCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ctl;
	reg_addr += win_index;
	/* Set color key match target and match rule */
	reg.val = READREG32(reg_addr);
	reg.bits.cken = 1;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Disable colorkey
 */
int csp_dc_window_disable_colorkey(unsigned int dc_index,
				   unsigned int win_index)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_bld0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	/*
	 * Get the BLDCTL register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->bld0ctl;
	reg_addr += win_index;
	/* Set color key match target and match rule */
	reg.val = READREG32(reg_addr);
	reg.bits.cken = 0;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Set the blend rule
 * We can support most of porter-duff rule
 */
int csp_dc_window_set_blend_rule(unsigned int dc_index, unsigned int win_index,
				 unsigned int premultiplied,
				 dpu_blend_rule_t rule)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_bld0ctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	if (rule == DPU_BLEND_DEFAULT)
		rule = DPU_BLEND_SRC_OVER;

	if (DC_BLEND_SUPPORT(rule, premultiplied) == 0)
		ret = __LINE__;
	else {
		reg_addr = (unsigned int *)&dc[dc_index]->bld0ctl;
		reg_addr += win_index;
		reg.val = READREG32(reg_addr);

		reg.bits.c0 = DPU_BLEND_C0(rule, premultiplied);
		reg.bits.c1 = DPU_BLEND_C1(rule);
		reg.bits.c2 = DPU_BLEND_C2(rule);
		reg.bits.c3 = DPU_BLEND_C3(rule);
		WRITEREG32(reg_addr, reg.val);
		ret = DPU_OK;
	}

exit:
	return ret;
}

/*
 * Update palette table content
 * If the image format is 1/2/4/8bpp, you need to provide a palette table
 *
 * @palette: the address of buffer contained palette table, must be 4bytes align
 * @bytes: the bytes of palette data, must be 4bytes align
 * @offset: the destination offset, in bytes, must be 4bytes align
 *
 * Cause the palette table is saved in sram, and sram is single buffer,
 * so when the specified window is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when the window is hide, you can call it at anytime as you like.
 */
int csp_dc_window_update_palette_table(unsigned int dc_index,
				       unsigned int win_index,
				       const unsigned char *palette,
				       unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	base = (unsigned long)dc[dc_index];
	base += DC_WIN_PALETTE_BASE(win_index);

	ret = dc_update_lut((unsigned char *)base, palette, bytes, offset);

exit:
	return ret;
}

int csp_dc_window_get_palette_table(unsigned int dc_index,
				    unsigned int win_index,
				    unsigned char *palette,
				    unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	base = (unsigned long)dc[dc_index];
	base += DC_WIN_PALETTE_BASE(win_index);

	ret = dc_get_lut((unsigned char *)base, palette, bytes, offset);

exit:
	return ret;
}

/*
 * Update gamma table content
 * If the window work on GAMMA MODE, you need to provide a gamma table
 *
 * @gamma: the address of buffer contained gamma table, must be 4bytes align
 * @bytes: the bytes of gamma data, must be 4bytes align
 * @offset: the destination offset, in bytes, must be 4bytes align
 *
 * Cause the gamma table is saved in sram, and sram is single buffer,
 * so when the specified window is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when the window is hide, you can call it at anytime as you like.
 */
int csp_dc_window_update_gamma_table(unsigned int dc_index,
				     unsigned int win_index,
				     const unsigned char *gamma,
				     unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	base = (unsigned long)dc[dc_index];
	base += DC_WIN_GAMMA_BASE(win_index);

	ret = dc_update_lut((unsigned char *)base, gamma, bytes, offset);

exit:
	return ret;
}

int csp_dc_window_get_gamma_table(unsigned int dc_index, unsigned int win_index,
				  unsigned char *gamma, unsigned int bytes,
				  unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);

	base = (unsigned long)dc[dc_index];
	base += DC_WIN_GAMMA_BASE(win_index);

	ret = dc_get_lut((unsigned char *)base, gamma, bytes, offset);

exit:
	return ret;
}

/*
 * Update internal framebuffer content
 * If the window work on IFB MODE, you need to provide a internal fb
 *
 * @buf: the address of buffer contained internal fb index
 * @bytes: the bytes of buffer data
 * @offset: the destination offset, in bytes.
 *
 * Because the ifb is saved in sram, and sram is single buffer,
 * so when the specified window is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when the window is hide, you can call it at anytime as you like.
 *
 * The sram size is limit, there is only one sram for ifb mode,
 * so, we can only support one ifb mode window, or maybe you want to
 * show the same image in two window.
 */
int csp_dc_window_update_internal_fb(unsigned int dc_index,
				     unsigned int win_index,
				     const unsigned char *buf,
				     unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned int i;
	const unsigned char *ptr = (const unsigned char *)buf;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (buf == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (offset >= DC_IFB_MAX_SIZE)
		offset = 0;

	if ((offset + bytes) > DC_IFB_MAX_SIZE)
		bytes = DC_IFB_MAX_SIZE - offset;

	base = (unsigned long)dc[dc_index];
	base += DC_WIN_IFB_BASE;

	for (i = 0; i < bytes; i++)
		WRITEREG8(base + offset + i, ptr[i]);

exit:
	return ret;
}

int csp_dc_window_get_internal_fb(unsigned int dc_index, unsigned int win_index,
				  unsigned char *buf, unsigned int bytes,
				  unsigned int offset)
{
	int ret = DPU_OK;
	unsigned int i;
	unsigned char *ptr = (unsigned char *)buf;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	ASSERT_WININDEX(win_index);
	if (buf == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if ((offset + bytes) > DC_IFB_MAX_SIZE) {
		ret = __LINE__;
		goto exit;
	}

	base = (unsigned long)dc[dc_index];
	base += DC_WIN_IFB_BASE;

	for (i = 0; i < bytes; i++)
		ptr[i] = READREG8(base + offset + i);

exit:
	return ret;
}

/* HWC control interface */
/*
 * Set hwc format
 * hwc can only support 1/2/4/8bpp format
 */
int csp_dc_hwc_set_format(unsigned int dc_index, dpu_pixel_format_t format)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_hwcctl_t reg;

	ASSERT_DCINDEX(dc_index);
	if (!dpu_format_has_lut(format)) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->hwcctl;
	reg.val = READREG32(reg_addr);
	reg.bits.fmt = DPU_FMT_DCINDEX(format);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get hwc format, only for test */
int csp_dc_hwc_get_format(unsigned int dc_index, dpu_pixel_format_t *format)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_hwcctl_t reg;

	ASSERT_DCINDEX(dc_index);
	assert(format != NULL);
	if (format == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->hwcctl;
	reg.val = READREG32(reg_addr);
	*format = (dpu_pixel_format_t)reg.bits.fmt;

exit:
	return ret;
}

/*
 * Update palette table content
 * Since hwc format is 1/2/4/8bpp, you need to provide a palette table
 *
 * @palette: the address of buffer contained palette table, must be 4bytes align
 * @bytes: the bytes of palette data, must be 4bytes align
 * @offset: the destination offset, in bytes, must be 4bytes align
 *
 * Cause the palette table is saved in sram, and sram is single buffer,
 * so when the hwc is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when the hwc is hide, you can call it at anytime as you like.
 */
int csp_dc_hwc_update_palette_table(unsigned int dc_index,
				    const unsigned char *palette,
				    unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);

	base = (unsigned long)dc[dc_index];
	base += DC_HWC_PALETTE_BASE;

	ret = dc_update_lut((unsigned char *)base, palette, bytes, offset);

	return ret;
}

int csp_dc_hwc_get_palette_table(unsigned int dc_index, unsigned char *palette,
				 unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);

	base = (unsigned long)dc[dc_index];
	base += DC_HWC_PALETTE_BASE;

	ret = dc_get_lut((unsigned char *)base, palette, bytes, offset);

	return ret;
}

/*
 * Update hwc image index content
 *
 * @buf: the address of buffer contained image index
 * @bytes: the bytes of palette data
 * @offset: the destination offset
 *
 * Cause the index content is saved in sram, and sram is single buffer,
 * so when the hwc is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when the hwc is hide, you can call it at anytime as you like.
 */
int csp_dc_hwc_update_fb(unsigned int dc_index, const unsigned char *buf,
			 unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned int i;
	const unsigned int *ptr = (const unsigned int *)buf;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	if (buf == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (offset >= DC_HWC_INDEX_MAX_SIZE)
		offset = 0;

	if ((offset + bytes) > DC_HWC_INDEX_MAX_SIZE)
		bytes = DC_HWC_INDEX_MAX_SIZE - offset;

	base = (unsigned long)dc[dc_index];
	base += DC_HWC_INDEX_BASE;

	for (i = 0; i < bytes / 4; i++)
		WRITEREG32_RELAX(base + offset + i * 4, ptr[i]);

exit:
	return ret;
}

int csp_dc_hwc_get_fb(unsigned int dc_index, unsigned char *buf,
		      unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned int i;
	unsigned char *ptr = (unsigned char *)buf;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);
	if (buf == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if ((offset + bytes) > DC_HWC_INDEX_MAX_SIZE) {
		ret = __LINE__;
		goto exit;
	}

	base = (unsigned long)dc[dc_index];
	base += DC_HWC_INDEX_BASE;

	for (i = 0; i < bytes; i++)
		ptr[i] = READREG8(base + offset + i);

exit:
	return ret;
}
/* Show the hwc on the screen */
int csp_dc_hwc_show(unsigned int dc_index)
{
	unsigned int *reg_addr;
	reg_dc_imgsrcctl_t reg;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	reg.bits.hwcen = 1;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Hide the hwc */
int csp_dc_hwc_hide(unsigned int dc_index)
{
	unsigned int *reg_addr;
	reg_dc_imgsrcctl_t reg;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	reg.bits.hwcen = 0;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Set the  destination coordination where the hwc show on the screen */
int csp_dc_hwc_set_dest_coordinate(unsigned int dc_index, int x, int y)
{
	unsigned int *reg_addr;
	reg_dc_hwccoor_t reg;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->hwccoor;
	reg.val = 0;
	reg.bits.x = x;
	reg.bits.y = y;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Get the destination coordination of hwc */
int csp_dc_hwc_get_dest_coordinate(unsigned int dc_index, int *x, int *y)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_hwccoor_t reg;

	ASSERT_DCINDEX(dc_index);
	if ((x == NULL) || (y == NULL)) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->hwccoor;
	reg.val = READREG32(reg_addr);
	*x = (int)((short)reg.bits.x);
	*y = (int)((short)reg.bits.y);

exit:
	return ret;
}

/*
 * Set the resolution of hwc
 * hwc only support 32x32/64x64/32x64/64x32 resolution
 */
int csp_dc_hwc_set_size(unsigned int dc_index, unsigned int width,
			unsigned int height)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_hwcctl_t reg;

	ASSERT_DCINDEX(dc_index);
	if (!DC_HWCCTL_SIZE_VALID(width, height)) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->hwcctl;
	reg.val = READREG32(reg_addr);
	/* In the register, 0 means 32 pixels, 1 means 64 */
	reg.bits.w = DC_HWCCTL_W2REG(width);
	reg.bits.h = DC_HWCCTL_H2REG(height);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get the resolution of hwc */
int csp_dc_hwc_get_size(unsigned int dc_index, unsigned int *width,
			unsigned int *height)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_hwcctl_t reg;

	ASSERT_DCINDEX(dc_index);
	if ((width == NULL) || (height == NULL)) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->hwcctl;
	reg.val = READREG32(reg_addr);
	/* In the register, 0 means 32 pixels, 1 means 64 */
	*width = DC_HWCCTL_REG2W(reg.bits.w);
	*height = DC_HWCCTL_REG2H(reg.bits.h);

exit:
	return ret;
}

/*
 * Set origin offset widthin the hwc image
 * The pixel at the <x,y> position of the hwc image
 * will be showed at the destination coordinate <0,0>
 * on the screen.
 */
int csp_dc_hwc_set_offset(unsigned int dc_index, unsigned int x, unsigned int y)
{
	unsigned int *reg_addr;
	reg_dc_hwcctl_t reg;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->hwcctl;
	reg.val = READREG32(reg_addr);
	x = (x >= DC_HWC_MAX_WIDTH) ? (DC_HWC_MAX_WIDTH - 1) : x;
	y = (y >= DC_HWC_MAX_WIDTH) ? (DC_HWC_MAX_WIDTH - 1) : y;
	reg.bits.xoff = x;
	reg.bits.yoff = y;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Get the origin offset widthin the hwc image */
int csp_dc_hwc_get_offset(unsigned int dc_index,
			  unsigned int *x, unsigned int *y)
{
	unsigned int *reg_addr;
	reg_dc_hwcctl_t reg;

	ASSERT_DCINDEX(dc_index);
	assert(x != NULL);
	assert(y != NULL);

	reg_addr = (unsigned int *)&dc[dc_index]->hwcctl;
	reg.val = READREG32(reg_addr);
	*x = reg.bits.xoff;
	*y = reg.bits.yoff;

	return DPU_OK;
}

/* Block linker control interface */

/*
 * Set format of block linker
 * Note: all block linker share the same format
 */
int csp_dc_blocklinker_set_format(unsigned int dc_index,
				  dpu_pixel_format_t format)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	if (!((format == DPU_FMT_ARGB8888) ||
	      (format == DPU_FMT_RGBA8888) ||
	      (format == DPU_FMT_ABGR8888) ||
	      (format == DPU_FMT_BGRA8888) ||
	      (format == DPU_FMT_8BPP))) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	reg.bits.fmt = DC_BLKGNE_FMT(format);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Get the format of block linker
 * Only for test
 */
int csp_dc_blocklinker_get_format(unsigned int dc_index,
				  dpu_pixel_format_t *format)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	if (format == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	*format = (dpu_pixel_format_t)reg.bits.fmt;

exit:
	return ret;
}

/*
 * Update palette table content, all block linker share one palette table
 * If the image format is 1/2/4/8bpp, you need to provide a palette table
 *
 * @palette: the address of buffer contained palette table, must be 4bytes align
 * @bytes: the bytes of palette data, must be 4bytes align
 * @offset: the destination offset, in bytes, must be 4bytes align
 *
 * Cause the palette table is saved in sram, and sram is single buffer,
 * so when any blocklinker is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when all blocklinker is hide,
 * you can call it at anytime as you like.
 */
int csp_dc_blocklinker_update_palette_table(unsigned int dc_index,
				const unsigned char *palette,
				unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);

	base = (unsigned long)dc[dc_index];
	base += DC_BLK_PALETTE_BASE;

	ret = dc_update_lut((unsigned char *)base, palette, bytes, offset);

	return ret;
}

int csp_dc_blocklinker_get_palette_table(unsigned int dc_index,
					unsigned char *palette,
					unsigned int bytes, unsigned int offset)
{
	int ret = DPU_OK;
	unsigned long base;

	ASSERT_DCINDEX(dc_index);

	base = (unsigned long)dc[dc_index];
	base += DC_BLK_PALETTE_BASE;

	ret = dc_get_lut((unsigned char *)base, palette, bytes, offset);

	return ret;
}

/*
 * Set alpha mode of block linker
 * All block linker share the same alpha mode
 */
int csp_dc_blocklinker_set_alpha_mode(unsigned int dc_index,
				      dpu_alpha_mode_t mode)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	if (mode >= DPU_ALPHA_MODE_MAX) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	reg.bits.alphactl = mode;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get the alpha mode of block linker */
int csp_dc_blocklinker_get_alpha_mode(unsigned int dc_index,
				      dpu_alpha_mode_t *mode)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	if (mode == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	*mode = (dpu_alpha_mode_t)reg.bits.alphactl;

exit:
	return ret;
}

/*
 * Set plane alpha value of block linker
 * All block linker share the same plane alpha value
 */
int csp_dc_blocklinker_set_alpha_value(unsigned int dc_index,
				       unsigned int value)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	if (value > DC_ALPHA_MAX_VALUE) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	reg.bits.plaalpha = value;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Get plane alpha value of block linker
 * All block linker share the same plane alpha value
 */
int csp_dc_blocklinker_get_alpha_value(unsigned int dc_index,
				       unsigned int *value)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	if (value == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	*value = reg.bits.plaalpha;

exit:
	return ret;
}

/*
 * Set the blend rule
 * We can support most of porter-duff rule
 */
int csp_dc_blocklinker_set_blend_rule(unsigned int dc_index,
	unsigned int premultiplied, dpu_blend_rule_t rule)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_bld0ctl_t reg;

	ASSERT_DCINDEX(dc_index);

	if (rule == DPU_BLEND_DEFAULT)
		rule = DPU_BLEND_SRC_OVER;

	if (DC_BLEND_SUPPORT(rule, premultiplied) == 0)
		ret = __LINE__;
	else {
		reg_addr = (unsigned int *)&dc[dc_index]->bld0ctl;
		reg_addr += DC_WIN_NUM;
		reg.val = READREG32(reg_addr);

		reg.bits.c0 = DPU_BLEND_C0(rule, premultiplied);
		reg.bits.c1 = DPU_BLEND_C1(rule);
		reg.bits.c2 = DPU_BLEND_C2(rule);
		reg.bits.c3 = DPU_BLEND_C3(rule);
		WRITEREG32(reg_addr, reg.val);
		ret = DPU_OK;
	}

	return ret;
}

/*
 * Enable block linker function
 * Before any number of block linker show on the screen,
 * you need to enable the block linker function.
 * On the other hand, if block linker functin is enabled,
 * at least 1 block linker(blk0) will be show on the screen,
 * so block linker0 should be configured before enable block linker.
 */
int csp_dc_blocklinker_enable(unsigned int dc_index)
{
	unsigned int *reg_addr;
	reg_dc_imgsrcctl_t reg;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	reg.bits.blken = 1;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/*
 * Disable block linker function
 * When disabled, no block linker could be show on the screen
 */
int csp_dc_blocklinker_disable(unsigned int dc_index)
{
	unsigned int *reg_addr;
	reg_dc_imgsrcctl_t reg;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->imgsrcctl;
	reg.val = READREG32(reg_addr);
	reg.bits.blken = 0;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Set destination rectangle where specified blk show on the screen */
int csp_dc_blocklinker_set_dest_rectangle(unsigned int dc_index,
					  unsigned int bl_index,
					  const dpu_rectangle_t *rect)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_ch0_blkctl_t reg;
	reg_dc_ch0_blkcoor_t blkcoor;

	ASSERT_DCINDEX(dc_index);
	ASSERT_BLKINDEX(bl_index);
	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/* Set the size of blk */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkctl;
	reg_addr += bl_index;
	reg.val = blk_reg_backups[bl_index].blkctl;
	reg.bits.w = (rect->w == 0) ? 0 : (rect->w - 1);
	reg.bits.h = (rect->h == 0) ? 0 : (rect->h - 1);
	WRITEREG32(reg_addr, reg.val);
	blk_reg_backups[bl_index].blkctl = reg.val;

	/* Set the coordinate of blk */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkcoor;
	reg_addr += bl_index;
	blkcoor.val = 0;
	blkcoor.bits.x = rect->x;
	blkcoor.bits.y = rect->y;
	WRITEREG32(reg_addr, blkcoor.val);
	blk_reg_backups[bl_index].blkcoor = blkcoor.val;

exit:
	return ret;
}

/* Get destination rectangle of blk */
int csp_dc_blocklinker_get_dest_rectangle(unsigned int dc_index,
					  unsigned int bl_index,
					  dpu_rectangle_t *rect)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_ch0_blkctl_t reg;
	reg_dc_ch0_blkcoor_t blkcoor;

	ASSERT_DCINDEX(dc_index);
	ASSERT_BLKINDEX(bl_index);
	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/* Set the size of blk */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkctl;
	reg_addr += bl_index;
	reg.val = blk_reg_backups[bl_index].blkctl;
	rect->w = reg.bits.w + 1;
	rect->h = reg.bits.h + 1;

	/* Set the coordinate of blk */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkcoor;
	reg_addr += bl_index;
	blkcoor.val = blk_reg_backups[bl_index].blkcoor;
	rect->x = (int)((short)(blkcoor.bits.x));
	rect->y = (int)((short)(blkcoor.bits.y));

exit:
	return ret;
}

/* Set the buffer information of blk */
int csp_dc_blocklinker_set_buffer(unsigned int dc_index, unsigned int bl_index,
	const dpu_fb_t *fb)
{
	int ret = DPU_OK;
	long long offset;
	unsigned int bpp;
	unsigned int line_stride; /* In bit unit */
	unsigned int addr;/*In byte unit */
	unsigned int *reg_addr;
	reg_dc_blkgne_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_BLKINDEX(bl_index);
	if (fb == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (!((fb->format == DPU_FMT_ARGB8888) ||
	      (fb->format == DPU_FMT_RGBA8888) ||
	      (fb->format == DPU_FMT_ABGR8888) ||
	      (fb->format == DPU_FMT_BGRA8888) ||
	      (fb->format == DPU_FMT_8BPP))) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * When we clip a image,
	 * we need to calculate the address of start point.
	 * The formula is as below(all in byte unit):
	 * clip start Address =  buffer address + y * line_stride + x * Bpp
	 */
	line_stride = fb->linestride_bit[0];
	offset = (long long)fb->addr[0];
	bpp = dpu_format_get_bits_per_pixel(fb->format, 0);

	/* adjust address acoording to pixel order */
	if (fb->pixel_order != 0) {
		/* handle palette format */
		if (bpp <= 8) {
			unsigned int oidx;
			unsigned int pidx;
			long long offset2;

			oidx = DPU_FMT_DCINDEX(fb->format) * 4 +
					fb->pixel_order;
			pidx = (unsigned int)(offset & 0x1f) / bpp;
			assert(oidx < 8);
			assert(pidx < 32);
			offset2 = ((long long)pixel_order_tbl[oidx][pidx] -
				(long long)pidx) * bpp;
			assert((offset2 > -32) && (offset2 < 32));

			offset += offset2;
			assert(offset >= 0);
		}
	}

	/* Turn offset's unit from bit into byte */
	addr = (unsigned int)(offset >> 3);
	/* Set the address */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkbufadd;
	reg_addr += bl_index;
	WRITEREG32(reg_addr, addr);
	blk_reg_backups[bl_index].blkbufadd = addr;

	/* Set the linestride */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkbuflnstd;
	reg_addr += bl_index;
	WRITEREG32(reg_addr, line_stride);
	blk_reg_backups[bl_index].blkbuflnstd = line_stride;

	/* Set the pixel order */
	reg_addr = (unsigned int *)&dc[dc_index]->blkgne;
	reg.val = READREG32(reg_addr);
	reg.bits.po = (dpu_pixel_order_t)(fb->pixel_order >> 1);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Only for test */
int csp_dc_blocklinker_get_buffer(unsigned int dc_index, unsigned int bl_index,
				  dpu_fb_t *fb)
{
	int ret = DPU_OK;

	return ret;
}

/*
 * Set the next block linker pointer
 * The block linker should be connected into a circle,
 * All the block linker in the circle will be show on the screen
 */
int csp_dc_blocklinker_set_next(unsigned int dc_index, unsigned int bl_index,
				unsigned int bl_next)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_ch0_blkctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_BLKINDEX(bl_index);
	ASSERT_BLKINDEX(bl_next);
	if ((bl_index != 0) && (bl_index == bl_next)) {
		ret = __LINE__;
		goto exit;
	}

	/* Set the next pointer of block linker */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkctl;
	reg_addr += bl_index;
	reg.val = blk_reg_backups[bl_index].blkctl;
	reg.bits.np = bl_next;
	WRITEREG32(reg_addr, reg.val);
	blk_reg_backups[bl_index].blkctl = reg.val;

exit:
	return ret;
}

int csp_dc_blocklinker_get_next(unsigned int dc_index, unsigned int bl_index,
				unsigned int *bl_next)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_ch0_blkctl_t reg;

	ASSERT_DCINDEX(dc_index);
	ASSERT_BLKINDEX(bl_index);

	/* Set the next pointer of block linker */
	reg_addr = (unsigned int *)&dc[dc_index]->ch0_blkctl;
	reg_addr += bl_index;
	reg.val = blk_reg_backups[bl_index].blkctl;
	*bl_next = reg.bits.np;

exit:
	return ret;
}

/* Write back control interface */
#if 0
/*
 * Set format
 * WB only support 32bit format
 */
int csp_dc_wb_set_format(unsigned int dc_index, dpu_pixel_format_t format)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;

	ASSERT_DCINDEX(dc_index);
	if (!((format == DPU_FMT_ARGB8888) ||
	      (format == DPU_FMT_RGBA8888) ||
	      (format == DPU_FMT_ABGR8888) ||
	      (format == DPU_FMT_BGRA8888))) {
		ret = __LINE__;
		goto exit;
	}

	/* Set format */
	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	reg.bits.fmt = DPU_FMT_DCINDEX(format);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get the wb format, only for test */
int csp_dc_wb_get_format(unsigned int dc_index, dpu_pixel_format_t *format)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;

	ASSERT_DCINDEX(dc_index);
	if (format == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/* Get format */
	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	*format = reg.bits.fmt;

exit:
	return ret;
}
#endif

/*
 * Set wb mode
 * If you want to capture the content showed on the screen,
 * you should choose the DISPLAY_MODE.
 * If you want to wb only, then choose WB_ONLY_MODE
 */
int csp_dc_wb_set_mode(unsigned int dc_index, dc_wb_mode_t wbmod)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;

	ASSERT_DCINDEX(dc_index);
	if (wbmod >= DC_WB_MODE_MAX) {
		ret = __LINE__;
		goto exit;
	}

	/* Set format */
	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	reg.bits.wbmod = wbmod;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Get wb mode */
int csp_dc_wb_get_mode(unsigned int dc_index, dc_wb_mode_t *wbmod)
{
	int ret = DPU_OK;
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;

	ASSERT_DCINDEX(dc_index);
	if (wbmod == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/* Get format */
	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	*wbmod = (dc_wb_mode_t)reg.bits.wbmod;

exit:
	return ret;
}

/* Set wb buffer information. Don't support clip actually. */
int csp_dc_wb_set_buffer(unsigned int dc_index, const dpu_fb_t *fb)
{
	int ret = DPU_OK;
	unsigned int line_stride;	/* In bit unit */
	unsigned int addr;		/*In byte unit */
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;

	ASSERT_DCINDEX(dc_index);
	if (fb == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (!((fb->format == DPU_FMT_ARGB8888) ||
	      (fb->format == DPU_FMT_RGBA8888) ||
	      (fb->format == DPU_FMT_ABGR8888) ||
	      (fb->format == DPU_FMT_BGRA8888))) {
		ret = __LINE__;
		goto exit;
	}

	/* Set format */
	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	reg.bits.fmt = DPU_FMT_DCINDEX(fb->format);
	WRITEREG32(reg_addr, reg.val);

	/*
	 * When we clip a image,
	 * we need to calculate the address of start point.
	 * The formula is as below(all in byte unit):
	 * clip start Address =  buffer address + y * line_stride + x * Bpp
	 */
	line_stride = fb->linestride_bit[0];

	addr = (unsigned int)(fb->addr[0] >> 3); /* byte unit */
	addr = addr >> 2;/* address unit convert from byte to word unit
	 * set wb buffer address, in word unit
	 */
	reg_addr = (unsigned int *)&dc[dc_index]->wbadd;
	WRITEREG32(reg_addr, addr);

	/* Set wb buffer linestride, in bit unit */
	reg_addr = (unsigned int *)&dc[dc_index]->wblnstd;
	WRITEREG32(reg_addr, line_stride);

exit:
	return ret;
}

/* only for test */
int csp_dc_wb_get_buffer(unsigned int dc_index, dpu_fb_t *fb)
{

	return DPU_OK;
}

/* Start wb process */
int csp_dc_wb_start(unsigned int dc_index)
{
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;

	ASSERT_DCINDEX(dc_index);

	/* Start wb */
	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	reg.bits.sta = 1;
	WRITEREG32(reg_addr, reg.val);

	return DPU_OK;
}

/* Get wb status */
int csp_dc_wb_get_status(unsigned int dc_index, unsigned int *status)
{
	unsigned int *reg_addr;
	reg_dc_wbc_t reg;
	int ret = DPU_OK;
	unsigned int hwstatus;
	unsigned int exception;
	unsigned int start;

	ASSERT_DCINDEX(dc_index);

	reg_addr = (unsigned int *)&dc[dc_index]->wbc;
	reg.val = READREG32(reg_addr);
	hwstatus = reg.bits.wbstatus;
	exception = reg.bits.wbexc;
	start = reg.bits.sta;

	if ((exception == 1) && (hwstatus == 0))
		*status = DPU_ERROR;
	else if ((hwstatus == 1) || (start == 1))
		*status = DPU_BUSY;
	else
		*status = DPU_OK;

	return ret;
}

#endif /* __CSP_DPU_DC_C__ */

