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

#ifndef __CSP_DPU_WB_H__
#define __CSP_DPU_WB_H__

#include "csp_dpu_common.h"

/* module spec */
#define WB_MAX_IN_SIZE 2048
#define WB_MAX_SIZE    720
#define WB_REG_END 0x800
#define WB_XPHASE 5
#define WB_YPHASE 5
#define WB_HTAPS 4
#define WB_VTAPS 2
#define WB_OFFSET_FRAC_BIT 17
#define WB_OFFSET_ONE (1 << WB_OFFSET_FRAC_BIT)
#define WB_FILTER_FRAC_BIT 6
#define WB_CSC_BITS           15
#define WB_CSC_CONST_FRAC_BITS 4
#define WB_CSC_COEF_FRAC_BITS 12
#define WB_CSC_COEF_ROUND (1 << (WB_CSC_COEF_FRAC_BITS - 1))
#define WB_XSHIFT_BASE  (WB_OFFSET_FRAC_BIT - WB_XPHASE)
#define WB_YSHIFT_BASE  (WB_OFFSET_FRAC_BIT - WB_YPHASE)

#define WB_IRQ_FINISH 31

int csp_wb_set_register_base(unsigned long addr, unsigned int size);
int csp_wb_init(void);
int csp_wb_exit(void);
int csp_wb_enable(void);
int csp_wb_disable(void);
int csp_wb_get_version(void);
int csp_wb_enable_resample(void);
int csp_wb_disable_resample(void);
int csp_wb_enable_csc(void);
int csp_wb_disable_csc(void);
int csp_wb_set_src_size(unsigned int width, unsigned int height);
int csp_wb_load_dbr(void);
bool csp_wb_is_load_dbr_finish(void);
int csp_wb_set_field_state(unsigned int field_sta);
int csp_wb_set_runing_mode(unsigned int mode);
int csp_wb_start(void);
int csp_wb_get_status(unsigned int *status);

int csp_wb_enable_irq(unsigned int irqnr);
int csp_wb_disable_irq(unsigned int irqnr);
bool csp_wb_query_irq(unsigned int irqnr);
int csp_wb_clear_irq(unsigned int irqnr);
bool csp_wb_query_and_clear_irq(unsigned int irqnr);

int csp_wb_set_crop(const dpu_rectangle_t *rect);
int csp_wb_get_crop(dpu_rectangle_t *rect);
int csp_wb_set_dst_size(unsigned int dst_w[3], unsigned int dst_h[3]);
int csp_wb_set_buffer(const dpu_fb_t *fb);

int csp_wb_set_initial_offset(int xoffset[3], int yoffset[3]);
int csp_wb_set_scaling_ratio(int xratio[3], int yration[3]);
int csp_wb_set_scaling_xratio(unsigned int crop_w, unsigned int dst_w[3]);
int csp_wb_set_scaling_yratio(unsigned int crop_h, unsigned int dst_h[3]);
int csp_wb_set_scaling_xcoef(unsigned int crop_w, unsigned int dst_w[3]);
int csp_wb_set_scaling_ycoef(unsigned int crop_h, unsigned int dst_h[3]);

int csp_wb_set_csc_coef(dpu_csc_mode_t mode, dpu_csc_std_t csc_std);

#endif /*__CSP_DPU_WB_H__ */
