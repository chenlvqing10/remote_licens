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

#ifndef __CSP_DPU_WB_C__
#define __CSP_DPU_WB_C__

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>

#define WRITEREG32_RELAX(c, v) writel_relaxed(v, c)
#define WRITEREG32(c, v) writel(v, c)

#define READREG32_RELAX(c) readl_relaxed(c)
#define READREG32(c) readl(c)
#endif

#include "csp_dpu_wb.h"
#include "csp_dpu_wb_reg.h"

static reg_wb_t *wb;
static unsigned int wb_size;

/*
 * Global control interface
 * Set register base address
 */
int csp_wb_set_register_base(unsigned long addr, unsigned int size)
{
	int ret = 0;

	if (addr == 0) {
		ret = __LINE__;
		goto exit;
	}

	wb = (reg_wb_t *)addr;
	wb_size = size;

exit:
	return ret;
}

/* Get register base address */
int csp_wb_get_register_base(unsigned long *addr)
{
	int ret = 0;

	if (addr == NULL) {
		ret = __LINE__;
		goto exit;
	}

	*addr = (unsigned long)wb;
exit:
	return ret;
}

/* Initialize module */
int csp_wb_init(void)
{
	return csp_wb_enable();
}

/* Deinitialize module */
int csp_wb_exit(void)
{
	return csp_wb_disable();
}

/* Enable module */
int csp_wb_enable(void)
{
	reg_wb_ctl_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->ctl;
	reg.val = READREG32(reg_addr);
	/* Release module */
	reg.bits.rst = 1;
	WRITEREG32(reg_addr, reg.val);

	/* udelay */
	reg.val = READREG32(reg_addr);
	reg.val = READREG32(reg_addr);

	/* Enable module */
	reg.bits.en = 1;
	WRITEREG32(reg_addr, reg.val);

	return ret;
}

/* Disable module */
int csp_wb_disable(void)
{
	reg_wb_ctl_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	/* Disable module */
	reg_addr = (unsigned int *)&wb->ctl;
	reg.val = READREG32(reg_addr);
	reg.bits.en = 0;
	WRITEREG32(reg_addr, reg.val);

	/* udelay */
	reg.val = READREG32(reg_addr);
	reg.val = READREG32(reg_addr);

	/* Reset module */
	reg.bits.rst = 0;
	WRITEREG32(reg_addr, reg.val);

	return ret;
}

int csp_wb_get_version(void)
{
	unsigned int *reg_addr = (unsigned int *)&wb->ver;

	return READREG32(reg_addr);
}

/* start wb */
int csp_wb_start(void)
{
	reg_wb_strmctl_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->strmctl;
	reg.val = READREG32(reg_addr);
	reg.bits.start = 1;
	WRITEREG32(reg_addr, reg.val);

	return ret;
}

/* Get wb status */
int csp_wb_get_status(unsigned int *status)
{
	reg_wb_intpd_t reg;
	reg_wb_sta_t reg_status;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->intpd;
	reg.val = READREG32(reg_addr);

	reg_addr = (unsigned int *)&wb->sta;
	reg_status.val = READREG32(reg_addr);

	if (reg_status.bits.ovfl == 1)
		*status = DPU_ERROR;
	else if (reg_status.bits.to == 1)
		*status = DPU_TO;
	else if (reg_status.bits.sta == 1)
		*status = DPU_BUSY;
	else if (reg.bits.wbfnsh == 1)
		*status = DPU_OK;
	else
		ret = __LINE__;

	return ret;
}

/**
* csp_wb_enable_irq - enable irq
* @irqnr: use WB_IRQ\_ defines
*
* Returns 0 for sucess, or line number on error.
*/
int csp_wb_enable_irq(unsigned int irqnr)
{
	reg_wb_inten_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (irqnr >= 32) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->inten;
	reg.val = READREG32(reg_addr);
	reg.val |= (1u << irqnr);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/**
* csp_wb_disable_irq - enable irq
* @irqnr: use WB_IRQ\_ defines
*
* Returns 0 for sucess, or line number on error.
*/
int csp_wb_disable_irq(unsigned int irqnr)
{
	reg_wb_inten_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (irqnr >= 32) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->inten;
	reg.val = READREG32(reg_addr);
	reg.val &= ~(1u << irqnr);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/**
* csp_wb_query_irq - query whether irq is triggered
* @irqnr: use WB_IRQ\_ defines
*
* Returns true when triggered, or false when nothing happened
*/
bool csp_wb_query_irq(unsigned int irqnr)
{
	bool ret = false;
	unsigned int *reg_addr;
	reg_wb_intpd_t reg;

	if (irqnr >= 32) {
		ret = false;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->intpd;
	reg.val = READREG32(reg_addr);

	ret = (reg.val & (1u << irqnr)) != 0;
exit:
	return ret;
}

/**
* csp_wb_clear_irq - clear the pending flag of specified irq
* @irqnr: use WB_IRQ\_ defines
*
* Returns true when triggered, or false when nothing happened
*/
int csp_wb_clear_irq(unsigned int irqnr)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_intclr_t reg;

	if (irqnr >= 32) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->intclr;
	reg.val = READREG32(reg_addr);
	reg.val |= (1u << irqnr);
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/**
* csp_wb_query_and_clear_irq - query whether irq is triggered
* @irqnr: use WB_IRQ\_ defines
*
* When irq is trigger, clear it's pending flag and return true.
*
* Returns true when triggered, or false when nothing happened
*/
bool csp_wb_query_and_clear_irq(unsigned int irqnr)
{
	bool ret = false;

	ret = csp_wb_query_irq(irqnr);

	if (ret)
		csp_wb_clear_irq(irqnr);

	return ret;
}

/* Enable resample */
int csp_wb_enable_resample(void)
{
	reg_wb_cfg_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.rsmpen = 1;
	WRITEREG32_RELAX(reg_addr, reg.val);

	return ret;
}

/* Disable resample */
int csp_wb_disable_resample(void)
{
	reg_wb_cfg_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.rsmpen = 0;
	WRITEREG32_RELAX(reg_addr, reg.val);

	return ret;
}

/* Enable csc */
int csp_wb_enable_csc(void)
{
	reg_wb_cfg_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.cscen = 1;
	WRITEREG32_RELAX(reg_addr, reg.val);

	return ret;
}

/* Disable csc */
int csp_wb_disable_csc(void)
{
	reg_wb_cfg_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	reg_addr = (unsigned int *)&wb->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.cscen = 0;
	WRITEREG32_RELAX(reg_addr, reg.val);

	return ret;
}

/*
 * Set source resolution
 * @width/height: in pixel unit
 */
int csp_wb_set_src_size(unsigned int width, unsigned int height)
{
	int ret = 0;
	reg_wb_srcsize_t reg;
	unsigned int *reg_addr;

	if (width == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (height == 0) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->srcsize;
	reg.val = 0;
	reg.bits.w = width - 1;
	reg.bits.h = height - 1;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Load double buffer register
 * After we update the config of module, we need to load
 * the double buffer register content into registers
 */
int csp_wb_load_dbr(void)
{
	int ret = 0;
	reg_wb_ctl_t reg;
	unsigned int *reg_addr;

	reg_addr = (unsigned int *)&wb->ctl;
	reg.val = READREG32(reg_addr);
	/* Start load double buffer register */
	reg.bits.dbc = 1;
	/* Disable double buffer register auto load function */
	reg.bits.dbautold = 1;
	WRITEREG32(reg_addr, reg.val);

	return ret;
}

/*
 * After we load double buffer registe content into register,
 * we may need to check whether it has finished yet!
 */
bool csp_wb_is_load_dbr_finish(void)
{
	reg_wb_ctl_t reg;
	unsigned int *reg_addr;
	bool ret = false;

	/*
	 * When we want to load double buffer register, we set dbc bit,
	 * and this bit will be self clear when it finish.
	 */
	reg_addr = (unsigned int *)&wb->ctl;
	reg.val = READREG32(reg_addr);
	if (reg.bits.dbc == 0)
		ret = true;

	return ret;
}

/**
 * Set field state
 * @field_sta: field state, valid when interlaced out
 *             0: Low/High(Field 1/2) output (0/2/4/..)/(1/3/5/..) line
 *             1: Low/High(Field 1/2) output (1/3/5/..)/(0/2/4/..) line
 */
int csp_wb_set_field_state(unsigned int field_sta)
{
	int ret = 0;
	reg_wb_ctl_t reg;
	u32 *reg_addr;

	reg_addr = (unsigned int *)&wb->ctl;
	reg.val = READREG32(reg_addr);
	reg.bits.fieldsta = field_sta;
	WRITEREG32(reg_addr, reg.val);

	return ret;
}

/**
 * Set running mode
 * @mode: 0: normal mode; 1: continue mode
 */
int csp_wb_set_runing_mode(unsigned int mode)
{
	int ret = 0;
	reg_wb_ctl_t reg;
	u32 *reg_addr;

	reg_addr = (unsigned int *)&wb->ctl;
	reg.val = READREG32(reg_addr);
	reg.bits.continuemode = mode;
	WRITEREG32(reg_addr, reg.val);

	return ret;
}

/* Set the crop rectangle of input image */
int csp_wb_set_crop(const dpu_rectangle_t *rect)
{
	int ret = 0;
	reg_wb_cropsize_t reg;
	reg_wb_cropcoor_t reg_coor;
	unsigned int *reg_addr;

	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (rect->w == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (rect->h == 0) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->cropsize;
	reg.val = 0;
	reg.bits.w = (rect->w == 0) ? 0 : (rect->w - 1);
	reg.bits.h = (rect->h == 0) ? 0 : (rect->h - 1);
	WRITEREG32_RELAX(reg_addr, reg.val);

	reg_addr = (unsigned int *)&wb->cropcoor;
	reg_coor.val = 0;
	reg_coor.bits.x = rect->x;
	reg_coor.bits.y = rect->y;
	WRITEREG32_RELAX(reg_addr, reg_coor.val);

exit:
	return ret;
}

int csp_wb_set_dst_size(unsigned int dst_w[3], unsigned int dst_h[3])
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_dstsize0_t reg = {0};

	if (dst_w == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (dst_h == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/* destination size for channel 0 */
	reg_addr = (unsigned int *)&wb->dstsize0;
	reg.bits.w = (dst_w[0] == 0) ? 0 : dst_w[0] - 1;
	reg.bits.h = (dst_h[0] == 0) ? 0 : dst_h[0] - 1;
	WRITEREG32_RELAX(reg_addr, reg.val);

	/* destination size for channel 1 */
	reg_addr++;
	reg.bits.w = (dst_w[1] == 0) ? 0 : dst_w[1] - 1;
	reg.bits.h = (dst_h[1] == 0) ? 0 : dst_h[1] - 1;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;

}

/*
 * csp_wb_set_buffer - set buffer related information
 *
 * @fb        : Buffer related information
 * Returns negative errno on error, or Zero for success.
 */
int csp_wb_set_buffer(const dpu_fb_t *fb)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_cfg_t   reg;
	reg_wb_ch0_bufadd_t reg_bufaddr;
	reg_wb_ch0_buflnstd_t reg_buflnstd;

	if (fb == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&wb->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	/* Set buffer format and pixel order */
	reg.bits.fmt = DPU_FMT_WBINDEX(fb->format);
	WRITEREG32_RELAX(reg_addr, reg.val);

	/* Buffer address for channel 0/1 */
	reg_addr = (unsigned int *)&wb->ch0_bufadd;
	reg_bufaddr.bits.add = fb->addr[0] >> 3;
	WRITEREG32_RELAX(reg_addr, reg_bufaddr.val);
	reg_addr++;
	reg_bufaddr.bits.add = fb->addr[1] >> 3;
	WRITEREG32_RELAX(reg_addr, reg_bufaddr.val);

	/* Buffer linestride for channel 0/1 */
	reg_addr = (unsigned int *)&wb->ch0_buflnstd;
	reg_buflnstd.bits.lnstd = fb->linestride_bit[0] >> 3;
	WRITEREG32_RELAX(reg_addr, reg_buflnstd.val);
	reg_addr++;
	reg_buflnstd.bits.lnstd = fb->linestride_bit[1] >> 3;
	WRITEREG32_RELAX(reg_addr, reg_buflnstd.val);

exit:
	return ret;
}

int csp_wb_set_initial_offset(int xoffset[3], int yoffset[3])
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_ofstx0_t reg_off;

	if (xoffset == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (yoffset == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_off.val = 0;
	reg_addr = (unsigned int *)&wb->ofstx0;
	reg_off.bits.ofst = xoffset[0];
	WRITEREG32_RELAX(reg_addr, reg_off.val);
	reg_addr = (unsigned int *)&wb->ofstx1;
	reg_off.bits.ofst = xoffset[1];
	WRITEREG32_RELAX(reg_addr, reg_off.val);

	reg_addr = (unsigned int *)&wb->ofsty0;
	reg_off.bits.ofst = yoffset[0];
	WRITEREG32_RELAX(reg_addr, reg_off.val);
	reg_addr = (unsigned int *)&wb->ofsty1;
	reg_off.bits.ofst = yoffset[1];
	WRITEREG32_RELAX(reg_addr, reg_off.val);

exit:
	return ret;
}

int csp_wb_set_scaling_xratio(unsigned int crop_w, unsigned int dst_w[3])
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_sclrx0_t reg_ratio = {0};
	unsigned int ratio0 = 1 << WB_OFFSET_FRAC_BIT;
	unsigned int ratio1 = 1 << WB_OFFSET_FRAC_BIT;

	if (crop_w == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (dst_w == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (dst_w[0] != 0)
		ratio0 = (crop_w << WB_OFFSET_FRAC_BIT) / dst_w[0];
	if (dst_w[1] != 0)
		ratio1 = (crop_w << WB_OFFSET_FRAC_BIT) / dst_w[1];
	else
		ratio1 = ratio0;

	reg_addr = (unsigned int *)&wb->sclrx0;
	reg_ratio.bits.sclr = ratio0;
	WRITEREG32_RELAX(reg_addr, reg_ratio.val);
	reg_addr = (unsigned int *)&wb->sclrx1;
	reg_ratio.bits.sclr = ratio1;
	WRITEREG32_RELAX(reg_addr, reg_ratio.val);

exit:
	return ret;

}

int csp_wb_set_scaling_yratio(unsigned int crop_h, unsigned int dst_h[3])
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_sclry0_t reg_ratio = {0};
	unsigned int ratio0 = 1 << WB_OFFSET_FRAC_BIT;
	unsigned int ratio1 = 1 << WB_OFFSET_FRAC_BIT;

	if (crop_h == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (dst_h == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (dst_h[0] != 0)
		ratio0 = (crop_h << WB_OFFSET_FRAC_BIT) / dst_h[0];
	if (dst_h[1] != 0)
		ratio1 = (crop_h << WB_OFFSET_FRAC_BIT) / dst_h[1];
	else
		ratio1 = ratio0;

	reg_addr = (unsigned int *)&wb->sclry0;
	reg_ratio.bits.sclr = ratio0;
	WRITEREG32_RELAX(reg_addr, reg_ratio.val);
	reg_addr = (unsigned int *)&wb->sclry1;
	reg_ratio.bits.sclr = ratio1;
	WRITEREG32_RELAX(reg_addr, reg_ratio.val);

exit:
	return ret;

}

int csp_wb_set_scaling_ratio(int xratio[3], int yration[3])
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_wb_sclrx0_t reg_rtaio;

	if (xratio == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (yration == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_rtaio.val = 0;
	reg_addr = (unsigned int *)&wb->sclrx0;
	reg_rtaio.bits.sclr = xratio[0];
	WRITEREG32_RELAX(reg_addr, reg_rtaio.val);
	reg_addr = (unsigned int *)&wb->sclrx1;
	reg_rtaio.bits.sclr = xratio[1];
	WRITEREG32_RELAX(reg_addr, reg_rtaio.val);

	reg_addr = (unsigned int *)&wb->sclry0;
	reg_rtaio.bits.sclr = yration[0];
	WRITEREG32_RELAX(reg_addr, reg_rtaio.val);
	reg_addr = (unsigned int *)&wb->sclry1;
	reg_rtaio.bits.sclr = yration[1];
	WRITEREG32_RELAX(reg_addr, reg_rtaio.val);

exit:
	return ret;

}

int csp_wb_set_scaling_xcoef_core(unsigned int chan, unsigned int coef[32])
{
	int ret = 0;
	unsigned int *reg_addr;
	int i;

	if (coef == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (chan == 0) {
		reg_addr = (unsigned int *)&wb->ch0xcoef0;
	} else if (chan == 1) {
		reg_addr = (unsigned int *)&wb->ch1xcoef0;
	} else {
		ret = __LINE__;
		goto exit;
	}
	for (i = 0; i < 32; i++) {
		WRITEREG32_RELAX(reg_addr, coef[i]);
		reg_addr++;
	}

exit:
	return ret;
}

int csp_wb_set_scaling_ycoef_core(unsigned int chan, unsigned int coef[32])
{
	int ret = 0;
	unsigned int *reg_addr;
	int i;

	if (coef == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (chan == 0) {
		reg_addr = (unsigned int *)&wb->ch0ycoef0;
	} else if (chan == 1) {
		reg_addr = (unsigned int *)&wb->ch1ycoef0;
	} else {
		ret = __LINE__;
		goto exit;
	}
	for (i = 0; i < 32; i++) {
		WRITEREG32_RELAX(reg_addr, coef[i]);
		reg_addr++;
	}

exit:
	return ret;
}

#define CSCRNDS WB_CSC_COEF_ROUND
static int Y2RCOEF_ARRAY[4][12] = {
	/* conv_fmt: Y2R, std: 601l */
	{
	0x000012A1, 0x00000000, 0x00001989, (0xFFF21141 + CSCRNDS) >> 8,
	0x000012A1, 0xFFFFF9BB, 0xFFFFF2FE, (0x00087934 + CSCRNDS) >> 8,
	0x000012A1, 0x00002047, 0x00000000, (0xFFEEB2A0 + CSCRNDS) >> 8,
	},
	/* conv_fmt: Y2R, std: 601f */
	{
	0x00001000, 0x00000000, 0x0000166F, (0xFFF4C8B4 + CSCRNDS) >> 8,
	0x00001000, 0xFFFFFA7E, 0xFFFFF493, (0x00087758 + CSCRNDS) >> 8,
	0x00001000, 0x00001C5A, 0x00000000, (0xFFF1D2F2 + CSCRNDS) >> 8,
	},
	/* conv_fmt: Y2R, std: 709l */
	{
	0x000012A1, 0x00000000, 0x00001CAF, (0xFFF07E62 + CSCRNDS) >> 8,
	0x000012A1, 0xFFFFFC97, 0xFFFFF779, (0x0004CE0D + CSCRNDS) >> 8,
	0x000012A1, 0x000021CC, 0x00000000, (0xFFEDEFB8 + CSCRNDS) >> 8,
	},
	/* conv_fmt: Y2R, std: 709f */
	{
	0x00001000, 0x00000000, 0x00001932, (0xFFF366CF + CSCRNDS) >> 8,
	0x00001000, 0xFFFFFD01, 0xFFFFF883, (0x00053E5C + CSCRNDS) >> 8,
	0x00001000, 0x00001DB1, 0x00000000, (0xFFF127BB + CSCRNDS) >> 8,
	},
};

static int R2YCOEF_ARRAY[4][12] = {
	/* conv_fmt: R2Y, std: 601l */
	{
	0x0000041C, 0x00000811, 0x00000191, (0x00010000 + CSCRNDS) >> 8,
	0xFFFFFDA1, 0xFFFFFB58, 0x00000707, (0x00080000 + CSCRNDS) >> 8,
	0x00000707, 0xFFFFFA1E, 0xFFFFFEDB, (0x00080000 + CSCRNDS) >> 8,
	},
	/* conv_fmt: R2Y, std: 601f */
	{
	0x000004C9, 0x00000964, 0x000001D3, (0x00000000 + CSCRNDS) >> 8,
	0xFFFFFD4D, 0xFFFFFAB3, 0x00000800, (0x00080000 + CSCRNDS) >> 8,
	0x00000800, 0xFFFFF94D, 0xFFFFFEB3, (0x00080000 + CSCRNDS) >> 8,
	},
	/* conv_fmt: R2Y, std: 709l */
	{
	0x000002EC, 0x000009D4, 0x000000FE, (0x00010000 + CSCRNDS) >> 8,
	0xFFFFFE64, 0xFFFFFA95, 0x00000707, (0x00080000 + CSCRNDS) >> 8,
	0x00000707, 0xFFFFF99E, 0xFFFFFF5B, (0x00080000 + CSCRNDS) >> 8,
	},
	/* conv_fmt: R2Y, std: 709f */
	{
	0x00000367, 0x00000B71, 0x00000128, (0x00000000 + CSCRNDS) >> 8,
	0xFFFFFE2B, 0xFFFFF9D5, 0x00000800, (0x00080000 + CSCRNDS) >> 8,
	0x00000800, 0xFFFFF8BC, 0xFFFFFF44, (0x00080000 + CSCRNDS) >> 8,
	},
};

static int CSC_BYPASS_COEF[6][12] = {
	/* BYPASS */
	{
	0x00001000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00001000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00001000, 0x00000000,
	},
	/* 0/2/1 */
	{
	0x00001000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00001000, 0x00000000,
	0x00000000, 0x00001000, 0x00000000, 0x00000000,
	},
	/* 1/2/0 */
	{
	0x00000000, 0x00001000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00001000, 0x00000000,
	0x00001000, 0x00000000, 0x00000000, 0x00000000,
	},
	/* 2/0/1 */
	{
	0x00000000, 0x00000000, 0x00001000, 0x00000000,
	0x00001000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00001000, 0x00000000, 0x00000000,
	},
	/* 2/1/0 */
	{
	0x00000000, 0x00000000, 0x00001000, 0x00000000,
	0x00000000, 0x00001000, 0x00000000, 0x00000000,
	0x00001000, 0x00000000, 0x00000000, 0x00000000,
	},
};

int csp_wb_set_csc_coef(dpu_csc_mode_t mode, dpu_csc_std_t csc_std)
{
	u32 i = 0;
	unsigned int *reg_addr;
	int *coef;
	int mask = (1 << WB_CSC_BITS) - 1;

	if (mode == DPU_CSC_YUV2RGB)
		coef = Y2RCOEF_ARRAY[csc_std];
	else if (mode == DPU_CSC_RGB2YUV)
		coef = R2YCOEF_ARRAY[csc_std];
	else
		coef = CSC_BYPASS_COEF[csc_std];

	reg_addr = (unsigned int *)&wb->csccoef;
	for (i = 0; i < 12; i++)
		WRITEREG32_RELAX(reg_addr++, coef[i] & mask);

	if (mode == DPU_CSP_DISABLE)
		csp_wb_disable_csc();
	else
		csp_wb_enable_csc();

	return 0;
}

static unsigned int xfilter_coef_4tap_32phase_tbl0[] = {
	0x01073107, 0x01073107, 0x01083106, 0x01093006,
	0x010a2f06, 0x020b2d06, 0x020d2b06, 0x020e2a06,
	0x020f2906, 0x02112805, 0x02122705, 0x03142405,
	0x03152305, 0x03172105, 0x03192004, 0x041a1e04,
	0x041c1c04, 0x041e1a04, 0x04201903, 0x05211703,
	0x05231503, 0x05241403, 0x05271202, 0x05281102,
	0x06290f02, 0x062a0e02, 0x062b0d02, 0x062d0b02,
	0x062f0a01, 0x06300901, 0x06310801, 0x07310701,
};

static unsigned int xfilter_coef_4tap_32phase_tbl2[] = {
	0x00004000, 0x00013f00, 0x00033eff, 0x00053dfe,
	0x00063dfd, 0x00083bfd, 0x000a3afc, 0x000d37fc,
	0x000f35fc, 0x001133fc, 0xff1431fc, 0xff162ffc,
	0xff192cfc, 0xfe1c2afc, 0xfe1f27fc, 0xfd2125fd,
	0xfd2323fd, 0xfd2521fd, 0xfc271ffe, 0xfc2a1cfe,
	0xfc2c19ff, 0xfc2f16ff, 0xfc3114ff, 0xfc331100,
	0xfc350f00, 0xfc370d00, 0xfc3a0a00, 0xfd3b0800,
	0xfd3d0600, 0xfe3d0500, 0xff3e0300, 0x003f0100,
};

static unsigned int yfilter_coef_2tap_32phase_tbl0[] = {
	0x00000838, 0x00000937, 0x00000937, 0x00000b35,
	0x00000c34, 0x00000d33, 0x00000e32, 0x00001030,
	0x0000112f, 0x0000132d, 0x0000152b, 0x0000162a,
	0x00001828, 0x00001a26, 0x00001c24, 0x00001e22,
	0x00002020, 0x0000221e, 0x0000241c, 0x0000261a,
	0x00002818, 0x00002a16, 0x00002b15, 0x00002d13,
	0x00002f11, 0x00003010, 0x0000320e, 0x0000330d,
	0x0000340c, 0x0000350b, 0x00003709, 0x00003709,
};

static unsigned int yfilter_coef_2tap_32phase_tbl2[] = {
	0x00000040, 0x0000013f, 0x0000033d, 0x0000043c,
	0x0000063a, 0x00000838, 0x00000a36, 0x00000c34,
	0x00000e32, 0x00001030, 0x0000122e, 0x0000142c,
	0x0000162a, 0x00001927, 0x00001b25, 0x00001e22,
	0x00002020, 0x0000221e, 0x0000251b, 0x00002719,
	0x00002a16, 0x00002c14, 0x00002e12, 0x00003010,
	0x0000320e, 0x0000340c, 0x0000360a, 0x00003808,
	0x00003a06, 0x00003c04, 0x00003d03, 0x00003f01,
};

static unsigned int *xfilter_coef_4tap_32phase[4] = {
	xfilter_coef_4tap_32phase_tbl0,
	xfilter_coef_4tap_32phase_tbl0,
	xfilter_coef_4tap_32phase_tbl2,
	xfilter_coef_4tap_32phase_tbl0,
};

static unsigned int *yfilter_coef_2tap_32phase[4] = {
	yfilter_coef_2tap_32phase_tbl0,
	yfilter_coef_2tap_32phase_tbl0,
	yfilter_coef_2tap_32phase_tbl2,
	yfilter_coef_2tap_32phase_tbl0,
};

int csp_wb_set_scaling_xcoef(unsigned int crop_w, unsigned int dst_w[3])
{
	int ret = 0;
	unsigned int ratio0 = 1 << WB_OFFSET_FRAC_BIT;
	unsigned int ratio1 = 1 << WB_OFFSET_FRAC_BIT;
	unsigned int *xfilter_coef[2];
	unsigned int threshhold[2] = { 9 * (1 << WB_OFFSET_FRAC_BIT) / 4,
				       9 * (1 << WB_OFFSET_FRAC_BIT) / 4, };

	if (crop_w == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (dst_w == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (dst_w[0] != 0)
		ratio0 = (crop_w << WB_OFFSET_FRAC_BIT) / dst_w[0];
	if (dst_w[1] != 0)
		ratio1 = (crop_w << WB_OFFSET_FRAC_BIT) / dst_w[1];
	else
		ratio1 = ratio0; /* make sure as same as ratio0 if no plane1 */

	if (ratio0 <= threshhold[0])
		xfilter_coef[0] = xfilter_coef_4tap_32phase[2];
	else
		xfilter_coef[0] = xfilter_coef_4tap_32phase[0];

	if (ratio1 <= threshhold[1])
		xfilter_coef[1] = xfilter_coef_4tap_32phase[2];
	else
		xfilter_coef[1] = xfilter_coef_4tap_32phase[0];

	ret = csp_wb_set_scaling_xcoef_core(0, xfilter_coef[0]);
	if (ret != 0) {
		ret = __LINE__;
		goto exit;
	}
	ret = csp_wb_set_scaling_xcoef_core(1, xfilter_coef[0]);
	if (ret != 0) {
		ret = __LINE__;
		goto exit;
	}
exit:
	return ret;
}

int csp_wb_set_scaling_ycoef(unsigned int crop_h, unsigned int dst_h[3])
{
	int ret = 0;
	unsigned int ratio0 = 1 << WB_OFFSET_FRAC_BIT;
	unsigned int ratio1 = 1 << WB_OFFSET_FRAC_BIT;
	unsigned int *yfilter_coef[2];
	unsigned int threshhold[2] = { 9 * (1 << WB_OFFSET_FRAC_BIT) / 4,
				       9 * (1 << WB_OFFSET_FRAC_BIT) / 4, };

	if (crop_h == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (dst_h == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if (dst_h[0] != 0)
		ratio0 = (crop_h << WB_OFFSET_FRAC_BIT) / dst_h[0];
	if (dst_h[1] != 0)
		ratio1 = (crop_h << WB_OFFSET_FRAC_BIT) / dst_h[1];
	else
		ratio1 = ratio0; /* make sure as same as ratio0 if no plane1 */

	if (ratio0 <= threshhold[0])
		yfilter_coef[0] = yfilter_coef_2tap_32phase[2];
	else
		yfilter_coef[0] = yfilter_coef_2tap_32phase[0];

	if (ratio1 <= threshhold[1])
		yfilter_coef[1] = yfilter_coef_2tap_32phase[2];
	else
		yfilter_coef[1] = yfilter_coef_2tap_32phase[0];

	ret = csp_wb_set_scaling_ycoef_core(0, yfilter_coef[0]);
	if (ret != 0) {
		ret = __LINE__;
		goto exit;
	}
	ret = csp_wb_set_scaling_ycoef_core(1, yfilter_coef[0]);
	if (ret != 0) {
		ret = __LINE__;
		goto exit;
	}
exit:
	return ret;
}

#endif /*__CSP_DPU_WB_C__ */
