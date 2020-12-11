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

#ifndef __CSP_DPU_COMMON_H__
#define __CSP_DPU_COMMON_H__

#ifdef __KERNEL__

#include <linux/types.h>

#else

#if 1
#if !defined(int)
#define s32 int
#define u32 unsigned int
#define s16 short int
#define u16 unsigned short int
#define s8 char
#define u8 unsigned char
#define s64 long long
#define u64 unsigned long long
#endif

#if !defined(bool)
#define bool char
#endif /* defined(bool) */

#define NULL ((void *)0)
#define false 0
#define true 1

#if !defined(NULL)
#define NULL ((void *)0)
#endif

#ifndef ARRARSIZE
#define ARRARSIZE(x) (sizeof(x) / sizeof(*x))
#endif /* ARRARSIZE */

#endif /* 1 */

#endif /* #ifdef __KERNEL__ */

#define DPU_UP_ALIGN(x, a)   (((x) + ((a) - 1)) & (~((a) - 1)))
#define DPU_DOWN_ALIGN(x, a) ((x)               & (~((a) - 1)))
#define DPU_COLOR(a, r, g, b)   ((((a) & 0xff) << 24) | \
	(((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))

typedef enum tag_dpu_retval {
	DPU_OK,            /* No errror occured */
	DPU_ERROR,         /* A genernal or unkown error occured */
	DPU_BUSY,          /* The resource or device is busy */
	DPU_TO,            /* Timeout */
} dpu_retval_t;

/*
 * Pixel format encodes in the following way:
 *
 * @index: pixel format index, will be used direct by register
 * @colot_bits: effective color (or index) bits per pixel of format
 * @alpha_bits: effective alpha bits per pixel of format
 * @has_lut: color lookup table present
 * @plane_num: plane number consist per pixel of format
 */
#define DPU_PIXEL_FMT(index, dcindex, oveindex, seindex, wbindex) \
	((((index) & 0xff)) |   \
	  (((dcindex) & 0x1f) << 19) |   \
	  (((oveindex) & 0x1f) << 14) |   \
	  (((seindex) & 0x3f) << 8) |   \
	  (((wbindex) & 0x03) << 6))

#define DPU_FMT_DCINDEX(fmt)        (((fmt) >> 19) & 0x1f)
#define DPU_FMT_OVEINDEX(fmt)       (((fmt) >> 14) & 0x1f)
#define DPU_FMT_SEINDEX(fmt)        (((fmt) >> 8) & 0x3f)
#define DPU_FMT_WBINDEX(fmt)        (((fmt) >> 6) & 0x03)

/* pixel format */
typedef enum tag_dpu_pixel_fmt {
	/* 32bit, Alpha at the MSB, B at the LSB */
	DPU_FMT_ARGB8888  = DPU_PIXEL_FMT(0x00, 0x00, 0x00, 0x00, 0x00),
	/* 32ibt, Alpha at the MSB, R at the LSB */
	DPU_FMT_ABGR8888  = DPU_PIXEL_FMT(0x01, 0x01, 0x01, 0xFF, 0xFF),
	DPU_FMT_BGRA8888  = DPU_PIXEL_FMT(0x02, 0x02, 0x02, 0x01, 0xFF),
	DPU_FMT_RGBA8888  = DPU_PIXEL_FMT(0x03, 0x03, 0x03, 0xFF, 0xFF),
	DPU_FMT_ARGB4444  = DPU_PIXEL_FMT(0x04, 0x04, 0x04, 0x02, 0xFF),
	DPU_FMT_ABGR4444  = DPU_PIXEL_FMT(0x05, 0x05, 0x05, 0xFF, 0xFF),
	DPU_FMT_BGRA4444  = DPU_PIXEL_FMT(0x06, 0x06, 0x06, 0x03, 0xFF),
	DPU_FMT_RGBA4444  = DPU_PIXEL_FMT(0x07, 0x07, 0x07, 0xFF, 0xFF),
	DPU_FMT_ARGB1555  = DPU_PIXEL_FMT(0x08, 0x08, 0x08, 0x04, 0xFF),
	DPU_FMT_ABGR1555  = DPU_PIXEL_FMT(0x09, 0x09, 0x09, 0xFF, 0xFF),
	DPU_FMT_BGRA5551  = DPU_PIXEL_FMT(0x0A, 0x0A, 0x0A, 0x05, 0xFF),
	DPU_FMT_RGBA5551  = DPU_PIXEL_FMT(0x0B, 0x0B, 0x0B, 0xFF, 0xFF),
	DPU_FMT_RGB565    = DPU_PIXEL_FMT(0x0C, 0x0C, 0x0C, 0x06, 0x01),
	DPU_FMT_BGR565    = DPU_PIXEL_FMT(0x0D, 0x0D, 0x0D, 0x07, 0xFF),
	/* The same as BGR565 */
	DPU_FMT_RGB565_R  = DPU_PIXEL_FMT(0x0E, 0x0E, 0x0E, 0x07, 0xFF),
	/* The same as RGB565 */
	DPU_FMT_BGR565_R  = DPU_PIXEL_FMT(0x0F, 0x0F, 0x0F, 0x06, 0xFF),
	/* 24bit, R at the MSB, B at the LSB */
	DPU_FMT_RGB888    = DPU_PIXEL_FMT(0x10, 0x10, 0x10, 0xFF, 0xFF),
	DPU_FMT_BGR888    = DPU_PIXEL_FMT(0x11, 0x11, 0x11, 0xFF, 0xFF),
	/* The same as BGR888 */
	DPU_FMT_RGB888_R  = DPU_PIXEL_FMT(0x12, 0x12, 0x12, 0xFF, 0xFF),
	/* The same as RGB888 */
	DPU_FMT_BGR888_R  = DPU_PIXEL_FMT(0x13, 0x13, 0x13, 0xFF, 0xFF),
	DPU_FMT_1BPP      = DPU_PIXEL_FMT(0x14, 0x00, 0x00, 0xFF, 0xFF),
	DPU_FMT_2BPP      = DPU_PIXEL_FMT(0x15, 0x01, 0x01, 0xFF, 0xFF),
	DPU_FMT_4BPP      = DPU_PIXEL_FMT(0x16, 0x02, 0x02, 0xFF, 0xFF),
	DPU_FMT_8BPP      = DPU_PIXEL_FMT(0x17, 0x03, 0x03, 0xFF, 0xFF),
	DPU_FMT_YUV444P   = DPU_PIXEL_FMT(0x18, 0xFF, 0xFF, 0x22, 0xFF),
	DPU_FMT_YUV422P   = DPU_PIXEL_FMT(0x19, 0xFF, 0xFF, 0x21, 0xFF),
	DPU_FMT_YUV420P   = DPU_PIXEL_FMT(0x1A, 0xFF, 0xFF, 0x20, 0xFF),
	DPU_FMT_YUV411P   = DPU_PIXEL_FMT(0x1B, 0xFF, 0xFF, 0xFF, 0xFF),
	DPU_FMT_YUV410P   = DPU_PIXEL_FMT(0x1C, 0xFF, 0xFF, 0xFF, 0xFF),
	DPU_FMT_YUV444SP  = DPU_PIXEL_FMT(0x1D, 0xFF, 0xFF, 0xFF, 0xFF),
	DPU_FMT_YVU444SP  = DPU_PIXEL_FMT(0x1E, 0xFF, 0xFF, 0xFF, 0xFF),
	DPU_FMT_YUV422SP  = DPU_PIXEL_FMT(0x1F, 0x12, 0xFF, 0x12, 0x02),
	DPU_FMT_YVU422SP  = DPU_PIXEL_FMT(0x20, 0x13, 0xFF, 0x13, 0xFF),
	DPU_FMT_YUV420SP  = DPU_PIXEL_FMT(0x21, 0x10, 0xFF, 0x10, 0x03),
	DPU_FMT_YVU420SP  = DPU_PIXEL_FMT(0x22, 0x11, 0xFF, 0x11, 0xFF),
	DPU_FMT_AYUV      = DPU_PIXEL_FMT(0x23, 0x00, 0xFF, 0x00, 0x00),
	DPU_FMT_VUYA      = DPU_PIXEL_FMT(0x24, 0x02, 0xFF, 0x01, 0xFF),
	DPU_FMT_YUYV      = DPU_PIXEL_FMT(0x25, 0xFF, 0xFF, 0x09, 0xFF),
	DPU_FMT_YVYU      = DPU_PIXEL_FMT(0x26, 0xFF, 0xFF, 0x0B, 0xFF),
	DPU_FMT_UYVY      = DPU_PIXEL_FMT(0x27, 0xFF, 0xFF, 0x08, 0xFF),
	DPU_FMT_VYUY      = DPU_PIXEL_FMT(0x28, 0xFF, 0xFF, 0x0A, 0xFF),
} dpu_pixel_format_t;

/*
 * Pixel order
 * @PIXEL_REVERT_IN_BYTE: Valid in 1/2/4/8bpp format
 *     for example: 8bpp: The same in normal and revert mode
 *                  4bpp: P1 P0(normal, P1 in the MSB),
 *                           P0 P1(revert, P0 in the MSB)
 *                  2bpp: P3 P2 P1 P0(normal),  P0 P1 P2 P3(revert)
 *                  1bpp: P7 P6 P5 P4 P3 P2 P1 P0(normal),
 *                           P0 P1 P2 P3 P4 P5 P6 P7(revert)
 *                  and the rule is the same in each byte.
 * @BYTE_REVERT_IN_WORD:  Valid in 1/2/4/8bpp format
 * for example: 8bpp: P3 P2 P1 P0(normal[N], P3 in the MSB0)
 *                    P0 P1 P2 P3(revert[R], P0 in the MSB)
 *              4bpp: P7 P6 P5 P4 ... P1 P0(N)
 *                    P1 P0 P3 P2 ... P7 P6(R)
 *              2bpp: P15 ... P12 | P11 ... P8 | P7  ... P4 | P3  ... P0
 *                    P3  ...  P0 | P7 ... P4  | P11 ... P8 | P15 ... P12(R)
 *              1bpp: P31 ... P24 | P23 ... P16| P15 ... P8 | P7  ... P0
 *                    P7  ...  P0 | P15 ...  P8| P23 ... P16| P31 ... P24(R)
 * @PIXEL_REVERT_IN_WORD: Valid in 16bit pixel format
 *     for example: RGB565: P1 P0(normal, P1 in the MSB),
 *                          P0 P1(revert, P0 in the MSB)
 *
 * PIXEL_REVERT_IN_BYTE and BYTE_REVERT_IN_WORD can be "OR" together,
 * like PIXEL_REVERT_IN_BYTE | BYTE_REVERT_IN_WROD.
 */
typedef enum tag_dpu_pixel_order {
	DPU_PO_NORMAL                 = 0, /* defaault */
	DPU_PO_PIXEL_REVERT_IN_BYTE   = 1, /* Valid in 1/2/4/8bpp format */
	DPU_PO_BYTE_REVERT_IN_WORD    = 2, /* Valid in 1/2/4/8bpp format */
	DPU_PO_PIXEL_REVERT_IN_WORD   = 2, /* Valid in 16bit pixel format */
	DPU_PO_MAX                    = 3,
} dpu_pixel_order_t;

/* Color data range */
typedef enum tag_dpu_data_range {
	/* Full range, 0~255 when in 8bit; 0~1024 when in 10bit */
	DPU_DATA_FULL_RANGE,
	/* Limit range, 16~235(240) when in 8bit; 64~940(960) when in 10bit */
	DPU_DATA_LIMIT_RANGE,
	DPU_DATA_RANGE_MAX,
} dpu_data_range_t;

/* Color space conversion mode */
typedef enum tag_dpu_csc_mode {
	DPU_CSC_BYPASS,
	DPU_CSC_RGB2YUV,
	DPU_CSC_YUV2RGB,
	DPU_CSP_DISABLE,
} dpu_csc_mode_t;

/* Color space */
typedef enum tag_dpu_cs {
	DPU_CS_BT601, /* ITU BT.601 */
	DPU_CS_BT709, /* ITU BT.709 */
	DPU_CS_BT2020,/* ITU BT.2020 */
	DPU_CS_MAX,
} dpu_csc_std_t;

/* Color key match rule */
typedef enum tag_dpu_colorkey_rule {
	DPU_CK_ALWAYS_MATCH = 0, /* Always match */
	/* Matched when (min <= color <= max) */
	DPU_CK_INSIDE_RANGE_MATCH = 1,
	/* Matched when (color < min or color > max) */
	DPU_CK_OUTSIDE_RANGE_MATCH = 2,
	DPU_CK_RULE_MAX,
} dpu_colorkey_rule_t;

/*
 * Work mode of window
 * Normal mode, can support normal rgb format input
 * Paltte mode, can support 1/2/4/8bpp width max-256-color palette table
 * Gammma mode, enable gamma correction according to specified gamma table
 * Internal frame buffer mode, image index and lut will be
 * stored in the internal SRAM, can save power consumption
 */
typedef enum tag_dpu_window_work_mode {
	DPU_WINDOW_NORMAL_MODE,
	DPU_WINDOW_PALETTE_MODE,
	DPU_WINDOW_GAMMA_MODE,
	DPU_WINDOW_INTERNAL_FB_MODE,
	DPU_WINDOW_YUV_MODE,
	DPU_WINDOW_WORK_MODE_MAX,
} dpu_window_work_mode_t;

/*
 * blend rule encodes in the following way:
 * equation: cr=c0*cs+c1*cd; ar=c2*as+c3*ad;
 * cs: source color; as: source alpha;
 * cd: dest color;   ad: dest alpha
 * cr: result color; ar: result alpha
 *
 * @c0/1/2/3: blending coefficient
 * @c0p: blending coefficient 0 when source color carrys premultiplied alpha
 * @p       : 1: support, 0: unsupport when non-premultiplied
 */
#define DPU_BLEND(c0, c0p, c1, c2, c3, p) \
	((((c0) & 0xf)) |   \
	  (((c0p) & 0xf) << 4) |   \
	  (((c1) & 0xf) << 8) |   \
	  (((c2) & 0xf) << 12) |   \
	  (((c3) & 0xf) << 16) |   \
	  (((p) & 0x1) << 20))

#define DPU_BLEND_C0(bld, p)      (((bld) >> (4 * (p))) & 0xf)
#define DPU_BLEND_C1(bld)         (((bld) >> 8) & 0xf)
#define DPU_BLEND_C2(bld)         (((bld) >> 12) & 0xf)
#define DPU_BLEND_C3(bld)         (((bld) >> 16) & 0xf)
#define DC_BLEND_SUPPORT(bld, p)  ((p) ? 1 : ((bld) >> 20) & 0x1)

/* Blending rules(porter-duff rules) */
/* Default rule(src-over) */
/* None of the item are used */
/* Only the terms that contribute source color are used */
/* Only the terms that contribute destination color are used */
/* The source color is placed over the destination color */
/* The destination color is placed over the source color */
/* The source that overlaps the destination, replaces the destination */
/* The destination that overlaps the source, replaces the source */
/* The source that does not overlap the destination replaces the destination */
/* The destination that does not overlap the source replaces the source */
/* The source that overlaps the destination is composited */
/*       with the destination */
/* The destination that overlaps the source is composited */
/*       with the source and replaces the destination */
/* The non-overlapping regions of source and destination are combined */
typedef enum tag_dpu_blend_rule {
	DPU_BLEND_DEFAULT  = 0,
	DPU_BLEND_CLEAR    = DPU_BLEND(0, 0, 0, 0, 0, 1),
	DPU_BLEND_SRC      = DPU_BLEND(2, 1, 0, 1, 0, 1),
	DPU_BLEND_DST      = DPU_BLEND(0, 0, 1, 0, 1, 1),
	DPU_BLEND_SRC_OVER = DPU_BLEND(2, 1, 3, 5, 1, 1),
	DPU_BLEND_DST_OVER = DPU_BLEND(5, 5, 1, 5, 1, 0),
	DPU_BLEND_SRC_IN   = DPU_BLEND(4, 4, 0, 4, 0, 0),
	DPU_BLEND_DST_IN   = DPU_BLEND(0, 0, 2, 4, 0, 1),
	DPU_BLEND_SRC_OUT  = DPU_BLEND(5, 5, 0, 5, 0, 0),
	DPU_BLEND_DST_OUT  = DPU_BLEND(0, 0, 3, 0, 3, 1),
	DPU_BLEND_SRC_ATOP = DPU_BLEND(4, 4, 3, 0, 1, 0),
	DPU_BLEND_DST_ATOP = DPU_BLEND(5, 5, 2, 1, 0, 0),
	DPU_BLEND_XOR      = DPU_BLEND(5, 5, 3, 5, 3, 0),
} dpu_blend_rule_t;

/* Alpha mode */
/* Using the per-pixel alpha */
/* Using the per-plane alpha */
/* Using the per-plane and per-pixel alpha together */
typedef enum tag_dpu_alpha_mode {
	DPU_PIXEL_ALPHA,
	DPU_PLANE_ALPHA,
	DPU_PLANE_PIXEL_ALPHA,
	DPU_ALPHA_MODE_MAX,
} dpu_alpha_mode_t;

/* Pixel color */
typedef struct tag_dpu_color {
	unsigned char b;  /* blue component */
	unsigned char g;  /* green component */
	unsigned char r;  /* red component */
	unsigned char a;  /* alpha component */
} dpu_color_t;

/*
 * Rectangle specified by a start point and dimension
 * All in pixel unit
 */
typedef struct tag_dpu_rectangle {
	int x;            /* X coordinate of top-left point */
	int y;            /* Y coordinate of top-left point */
	unsigned int w;   /* Width of this rectangle */
	unsigned int h;   /* Height of this rectangle */
} dpu_rectangle_t;

/*
 * Framebuffer information
 */
typedef struct tag_dpu_fb {
	/* Physical or bus address of fb, in bit unit */
	unsigned long long addr[3];
	dpu_pixel_format_t format;       /* Pixel format */
	dpu_pixel_order_t pixel_order;   /* Pixel order */
	unsigned int width[3];           /* Buffer size of visual area */
	unsigned int height[3];
	unsigned int linestride_bit[3]; /* line stride of fb, in bit unit */
	unsigned int planes;            /* planes of buffer */
} dpu_fb_t;

unsigned int dpu_format_get_planes(unsigned int fmt);
unsigned int dpu_format_get_bits_per_pixel(
	unsigned int fmt, unsigned int plane);
unsigned int dpu_format_get_bytes_per_pixel(
	unsigned int fmt, unsigned int component);
int dpu_format_get_hsr(unsigned int fmt);
int dpu_format_get_vsr(unsigned int fmt);
bool dpu_format_has_lut(unsigned int fmt);

#endif /* __DPU_COMMON_H__ */
