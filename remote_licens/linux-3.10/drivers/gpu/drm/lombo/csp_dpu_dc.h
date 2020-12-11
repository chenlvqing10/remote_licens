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

#ifndef __CSP_DPU_DC_H__
#define __CSP_DPU_DC_H__

/* for Linux os var types defines*/
#include <linux/types.h>

#include "csp_dpu_common.h"

#define DC_END_REG       0x9c00

#ifndef ARRARSIZE
#define ARRARSIZE(x) (sizeof(x) / sizeof(*x))
#endif

/* 256 color for 8bpp, and 4bytes(argb) for each color */
#define DC_LUT_MAX_SIZE       ((1 << 8) * sizeof(unsigned int))
#define DC_HWC_INDEX_MAX_SIZE 0x400

#define DC_NUM           1
#define DC_WIN_NUM       4
#define DC_PIPE_NUM      2
#define DC_BKL_NUM       16

/* max  size supported */
#define DC_MAX_WIDTH     1920
#define DC_MAX_HEIGHT    1920

/* max size for hwc */
#define DC_HWC_MAX_WIDTH 64
#define DC_HWC_MAX_HEIGHT 64

/* Sram base */
#define DC_BLK_PALETTE_BASE       0x8000
#define DC_HWC_INDEX_BASE         0x8400
#define DC_HWC_PALETTE_BASE       0x8800
#define DC_WIN_PALETTE_BASE(w)    (0x8c00 + (w) * 0x400)
#define DC_WIN_GAMMA_BASE(w)      DC_WIN_PALETTE_BASE(w)
#define DC_WIN_IFB_BASE           (0x8000 - 0x1400)
#define DC_IFB_MAX_SIZE           (0x8c00 - DC_WIN_IFB_BASE)
#define DC_CECOEF_NUM             12 /* number of color enhance coef register */
#define DC_ALPHA_MAX_VALUE        255
#define DC_FMT_BPP_MAX            32
#define DC_HWC_OFFSET_BITWIDTH    6

/* Write back mode */
typedef enum tag_dc_wb_mode {
	DC_WB_DISPLAY_MODE,   /* Write back and Dispaly simultaneously */
	DC_WB_ONLY_MODE,      /* Write back only */
	DC_WB_MODE_MAX,
} dc_wb_mode_t;

/* Data source of window */
typedef enum tag_dc_window_src {
	/* Image data comes from DRAM via inner-DMA */
	DC_WINDOW_SRC_IDMA,
	/* Image data comes from external SCALING ENGINE0 */
	DC_WINDOW_SRC_SCALING0,
	/* Image data comes from external SCALING ENGINE1 */
	DC_WINDOW_SRC_SCALING1,
	/* Image data comes from external OV engine */
	DC_WINDOW_SRC_OVE,
	DC_WINDOW_SRC_MAX,
} dc_window_src_t;

/* Color key match rule */
typedef enum tag_dc_colorkey_rule {
	/* Always match */
	DC_COLORKEY_ALWAYS_MATCH = 0,
	/* Matched when (min <= color <= max) */
	DC_COLORKEY_INSIDE_RANGE_MATCH = 1,
	/* Matched when (color < min or color > max) */
	DC_COLORKEY_OUTSIDE_RANGE_MATCH = 2,
	DC_COLORKEY_RULE_MAX,
} dc_colorkey_rule_t;

/* Color key target rule */
typedef enum tag_dc_colorkey_target {
	/* Color key matching rule will target this window */
	DC_COLORKEY_TARGET_THIS,
	/* Color key matching rule will target the background window */
	DC_COLORKEY_TARGET_OTHER,
	DC_COLORKEY_TARGET_MAX,
} dc_colorkey_target_t;

/* Enhance information */
typedef struct tag_dc_enhance_info {
	dpu_pixel_format_t fmt; /* Pixel format */
	dpu_csc_std_t csstd;            /* Color space */
	dpu_data_range_t range; /* Data range */
	unsigned int enhance_enable; /* if the enhance function enable */
	unsigned int brightness;
	unsigned int saturation;
	unsigned int contrast;
	unsigned int hue;
} dc_enhance_info_t;

/* Color key information */
typedef struct tag_dc_colorkey {
	dc_colorkey_target_t target;
	/* The minimal color value for color key */
	dpu_color_t min;
	/* The maximum color value for color key */
	dpu_color_t max;
	/* Color key matching rule for r/g/b component */
	dc_colorkey_rule_t red_rule;
	dc_colorkey_rule_t green_rule;
	dc_colorkey_rule_t blue_rule;
} dc_colorkey_t;

/* 4x4 Matrix */
typedef struct tag_dc_matrix {
	int m00, m01, m02, m03;
	int m10, m11, m12, m13;
	int m20, m21, m22, m23;
	int m30, m31, m32, m33;
} dc_matrix;

/* global control interface */
int csp_dc_set_register_base(unsigned int dc_index, unsigned long addr,
			     unsigned int size);
int csp_dc_get_register_base(unsigned int dc_index, unsigned long *addr);
int csp_dc_init(unsigned int dc_index);
int csp_dc_exit(unsigned int dc_index);
int csp_dc_enable(unsigned int dc_index);
int csp_dc_disable(unsigned int dc_index);
int csp_dc_set_output_mode(unsigned int dc_index, unsigned int itl,
			   unsigned int field_pol);
int csp_dc_set_backcolor(unsigned int dc_index, const dpu_color_t *color);
int csp_dc_get_backcolor(unsigned int dc_index, dpu_color_t *color);
int csp_dc_enable_pipe(unsigned int dc_index, unsigned int pipe);
int csp_dc_disable_pipe(unsigned int dc_index, unsigned int pipe);
int csp_dc_set_screen_size(unsigned int dc_index, unsigned int width,
			   unsigned int height);
int csp_dc_get_screen_size(unsigned int dc_index, unsigned int *width,
			   unsigned int *height);
int csp_dc_load_dbr(unsigned int dc_index);
bool csp_dc_is_load_dbr_finish(unsigned int dc_index);

int csp_dc_autoload_dbr(unsigned int dc_index);
int csp_dc_unload_dbr(unsigned int dc_index);
int csp_dc_set_enhancement(unsigned int dc_index,
			   const dc_enhance_info_t *info);

/* window control interface */
int csp_dc_window_show(unsigned int dc_index, unsigned int win_index);
int csp_dc_window_hide(unsigned int dc_index, unsigned int win_index);

int csp_dc_window_set_alpha_mode(unsigned int dc_index,
				 unsigned int win_index,
				 dpu_alpha_mode_t mode);
int csp_dc_window_get_alpha_mode(unsigned int dc_index,
				 unsigned int win_index,
				 dpu_alpha_mode_t *mode);
int csp_dc_window_set_alpha_value(unsigned int dc_index, unsigned int win_index,
				  unsigned int value);
int csp_dc_window_get_alpha_value(unsigned int dc_index, unsigned int win_index,
				  unsigned int *value);
int csp_dc_window_set_dest_rectangle(unsigned int dc_index,
			unsigned int win_index, unsigned int planes,
			const dpu_rectangle_t *rect);
int csp_dc_window_get_dest_rectangle(unsigned int dc_index,
				     unsigned int win_index,
				     dpu_rectangle_t *rect);
int csp_dc_window_set_buffer(unsigned int dc_index, unsigned int win_index,
			     const dpu_fb_t *fb);
int csp_dc_window_get_buffer(unsigned int dc_index, unsigned int win_index,
			     dpu_fb_t *fb);
int csp_dc_window_set_data_source(unsigned int dc_index,
				  unsigned int win_index,
				  dc_window_src_t src);
int csp_dc_window_get_data_source(unsigned int dc_index, unsigned int win_index,
				  unsigned int *src);
int csp_dc_window_set_work_mode(unsigned int dc_index,
				unsigned int win_index,
				dpu_window_work_mode_t mode);
int csp_dc_window_get_work_mode(unsigned int dc_index,
				unsigned int win_index,
				dpu_window_work_mode_t *mode);
int csp_dc_window_enable_premultiply(unsigned int dc_index,
				     unsigned int win_index);
int csp_dc_window_disable_premultiply(unsigned int dc_index,
				      unsigned int window_index);
int csp_dc_window_set_colorkey(unsigned int dc_index,
			       unsigned int win_index,
			       const dc_colorkey_t *ck);
int csp_dc_window_get_colorkey(unsigned int dc_index, unsigned int win_index,
			       dc_colorkey_t *ck);
int csp_dc_window_enable_colorkey(unsigned int dc_index,
				  unsigned int win_index);
int csp_dc_window_disable_colorkey(unsigned int dc_index,
				   unsigned int win_index);
int csp_dc_window_set_blend_rule(unsigned int dc_index,
				 unsigned int win_index,
				 unsigned int premultiplied,
				 dpu_blend_rule_t rule);
int csp_dc_window_update_palette_table(unsigned int dc_index,
				       unsigned int win_index,
				       const unsigned char *palette,
				       unsigned int bytes,
				       unsigned int offset);
int csp_dc_window_get_palette_table(unsigned int dc_index,
				    unsigned int win_index,
				    unsigned char *palette,
				    unsigned int bytes,
				    unsigned int offset);
int csp_dc_window_update_gamma_table(unsigned int dc_index,
				     unsigned int win_index,
				     const unsigned char *gamma,
				     unsigned int bytes,
				     unsigned int offset);
int csp_dc_window_get_gamma_table(unsigned int dc_index,
				  unsigned int win_index,
				  unsigned char *gamma,
				  unsigned int bytes,
				  unsigned int offset);
int csp_dc_window_update_internal_fb(unsigned int dc_index,
				     unsigned int win_index,
				     const unsigned char *buf,
				     unsigned int bytes,
				     unsigned int offset);
int csp_dc_window_get_internal_fb(unsigned int dc_index,
				  unsigned int win_index,
				  unsigned char *buf,
				  unsigned int bytes,
				  unsigned int offset);

/* hardware cursor control interface */
int csp_dc_hwc_set_format(unsigned int dc_index, dpu_pixel_format_t format);
int csp_dc_hwc_get_format(unsigned int dc_index, dpu_pixel_format_t *format);
int csp_dc_hwc_update_palette_table(unsigned int dc_index,
				     const unsigned char *palette,
				     unsigned int bytes,
				     unsigned int offset);
int csp_dc_hwc_get_palette_table(unsigned int dc_index,
				 unsigned char *palette,
				 unsigned int bytes,
				 unsigned int offset);
int csp_dc_hwc_update_fb(unsigned int dc_index, const unsigned char *buf,
			 unsigned int bytes, unsigned int offset);
int csp_dc_hwc_get_fb(unsigned int dc_index, unsigned char *buf,
		      unsigned int bytes, unsigned int offset);
int csp_dc_hwc_show(unsigned int dc_index);
int csp_dc_hwc_hide(unsigned int dc_index);
int csp_dc_hwc_set_dest_coordinate(unsigned int dc_index, int x, int y);
int csp_dc_hwc_get_dest_coordinate(unsigned int dc_index, int *x, int *y);
int csp_dc_hwc_set_size(unsigned int dc_index, unsigned int width,
			unsigned int height);
int csp_dc_hwc_get_size(unsigned int dc_index, unsigned int *width,
			unsigned int *height);
int csp_dc_hwc_set_offset(unsigned int dc_index,
			  unsigned int x, unsigned int y);
int csp_dc_hwc_get_offset(unsigned int dc_index,
			  unsigned int *x, unsigned int *y);

/* block linker control interface */
int csp_dc_blocklinker_set_format(unsigned int dc_index,
				  dpu_pixel_format_t format);
int csp_dc_blocklinker_get_format(unsigned int dc_index,
				  dpu_pixel_format_t *format);
int csp_dc_blocklinker_update_palette_table(unsigned int dc_index,
					    const unsigned char *palette,
					    unsigned int bytes,
					    unsigned int offset);
int csp_dc_blocklinker_get_palette_table(unsigned int dc_index,
					 unsigned char *palette,
					 unsigned int bytes,
					 unsigned int offset);
int csp_dc_blocklinker_set_alpha_mode(unsigned int dc_index,
				      dpu_alpha_mode_t mode);
int csp_dc_blocklinker_get_alpha_mode(unsigned int dc_index,
				      dpu_alpha_mode_t *mode);
int csp_dc_blocklinker_set_alpha_value(unsigned int dc_index,
				       unsigned int value);
int csp_dc_blocklinker_get_alpha_value(unsigned int dc_index,
				       unsigned int *value);
int csp_dc_blocklinker_set_blend_rule(unsigned int dc_index,
				      unsigned int premultiplied,
				      dpu_blend_rule_t rule);
int csp_dc_blocklinker_enable(unsigned int dc_index);
int csp_dc_blocklinker_disable(unsigned int dc_index);
int csp_dc_blocklinker_set_dest_rectangle(unsigned int dc_index,
					  unsigned int bl_index,
					  const dpu_rectangle_t *rect);
int csp_dc_blocklinker_get_dest_rectangle(unsigned int dc_index,
					  unsigned int bl_index,
					  dpu_rectangle_t *rect);
int csp_dc_blocklinker_set_buffer(unsigned int dc_index,
				  unsigned int bl_index,
				  const dpu_fb_t *fb);
int csp_dc_blocklinker_get_buffer(unsigned int dc_index, unsigned int bl_index,
				  dpu_fb_t *fb);
int csp_dc_blocklinker_set_next(unsigned int dc_index, unsigned int bl_index,
				unsigned int bl_next);
int csp_dc_blocklinker_get_next(unsigned int dc_index, unsigned int bl_index,
				unsigned int *bl_next);

/* write back control interface */
int csp_dc_wb_set_mode(unsigned int dc_index, dc_wb_mode_t mode);
int csp_dc_wb_get_mode(unsigned int dc_index, dc_wb_mode_t *mode);
int csp_dc_wb_set_buffer(unsigned int dc_index, const dpu_fb_t *fb);
int csp_dc_wb_get_buffer(unsigned int dc_index, dpu_fb_t *fb);
int csp_dc_wb_start(unsigned int dc_index);
int csp_dc_wb_get_status(unsigned int dc_index, unsigned int *status);

#endif /* __CSP_DPU_DC_H__ */
