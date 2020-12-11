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

#ifndef __CSP_DPU_OVE_H__
#define __CSP_DPU_OVE_H__

#ifdef __KERNEL__
#include <linux/types.h>
#endif

#include "csp_dpu_common.h"

/* module spec */
#define OVE_NUM 1
#define OVE_OVL_NUM 4
#define OVE_LUT_MAX_SIZE ((1 << 8) * 4)
#define OVE_MAX_WIDTH 2048
#define OVE_MAX_HEIGHT 1536
#define OVE_REG_END 0x800

/* global control interface */
int csp_ove_set_register_base(unsigned int index, unsigned long addr,
			      unsigned int size);
int csp_ove_get_register_base(unsigned int index, unsigned long *addr);
int csp_ove_init(unsigned int index);
int csp_ove_exit(unsigned int index);
int csp_ove_enable(unsigned int index);
int csp_ove_disable(unsigned int index);
int csp_ove_set_output_mode(unsigned int index, unsigned int itl,
			    unsigned int field_pol);
int csp_ove_set_output_port(unsigned int index, unsigned int outsel);
int csp_ove_set_backcolor(unsigned int index, const unsigned int color);
int csp_ove_get_backcolor(unsigned int index, unsigned int *color);
int csp_ove_set_window_size(unsigned int index, unsigned int width,
			    unsigned int height);
int csp_ove_get_window_size(unsigned int index, unsigned int *width,
			    unsigned int *height);
int csp_ove_load_dbr(unsigned int index);
bool csp_ove_is_load_dbr_finish(unsigned int index);

/* overlay control interface */
int csp_ove_ovl_show(unsigned int index, unsigned int ovl_index);
int csp_ove_ovl_hide(unsigned int index, unsigned int ovl_index);
int csp_ove_ovl_set_alpha_mode(unsigned int index, unsigned int ovl_index,
			dpu_alpha_mode_t mode, unsigned int premultiplied);
int csp_ove_ovl_set_alpha_value(unsigned int index, unsigned int ovl_index,
				unsigned int value);
int csp_ove_ovl_get_alpha_value(unsigned int index, unsigned int ovl_index,
				unsigned int *value);
int csp_ove_ovl_set_dest_rectangle(unsigned int index, unsigned int ovl_index,
				   const dpu_rectangle_t *rect);
int csp_ove_ovl_get_dest_rectangle(unsigned int index, unsigned int ovl_index,
				   dpu_rectangle_t *rect);
int csp_ove_ovl_set_buffer(unsigned int index, unsigned int ovl_index,
			   const dpu_fb_t *fb);
int csp_ove_ovl_get_buffer(unsigned int index, unsigned int ovl_index,
			   dpu_fb_t *fb);
int csp_ove_ovl_set_work_mode(unsigned int index, unsigned int ovl_index,
			      dpu_window_work_mode_t mode);
int csp_ove_ovl_get_work_mode(unsigned int index, unsigned int ovl_index,
			      dpu_window_work_mode_t *mode);
int csp_ove_ovl_update_palette_table(unsigned int index, unsigned int ovl_index,
				unsigned int bpp, const unsigned char *palette,
				unsigned int bytes, unsigned int offset);
int csp_ove_ovl_get_palette_table(unsigned int index, unsigned int ovl_index,
				unsigned int bpp, unsigned char *palette,
				unsigned int bytes, unsigned int offset);

#endif /*__CSP_DPU_OVE_H__ */
