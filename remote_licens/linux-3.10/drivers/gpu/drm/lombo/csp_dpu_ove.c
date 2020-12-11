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

#include "csp_dpu_ove.h"
#include "csp_dpu_ove_reg.h"

#ifndef __CSP_DPU_OVE_C__
#define __CSP_DPU_OVE_C__

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>

#define WRITEREG32_RELAX(c, v) writel_relaxed(v, (void *)(c))
#define WRITEREG32(c, v) writel(v, c)

#define READREG32_RELAX(c) readl_relaxed((void *)(c))
#define READREG32(c) readl(c)
#endif

static reg_ove_t *ove[OVE_NUM];
static unsigned int ove_size[OVE_NUM];

#define ove_assert(formula)

static int ove_update_lut(unsigned char *dst,
	const unsigned char *lut, unsigned int bytes, unsigned int offset)
{
	int ret = 0;
	unsigned int i;
	const unsigned int *ptr = (const unsigned int *)lut;
	unsigned long base;

	if (dst == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (lut == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if ((offset + bytes) > OVE_LUT_MAX_SIZE) {
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

static int ove_get_lut(unsigned char *dst,
	unsigned char *lut, unsigned int bytes, unsigned int offset)
{
	int ret = 0;
	unsigned int i;
	unsigned int *ptr = (unsigned int *)lut;
	unsigned long base;

	if (dst == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (lut == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if ((offset + bytes) > OVE_LUT_MAX_SIZE) {
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
int csp_ove_set_register_base(unsigned int index, unsigned long addr,
			      unsigned int size)
{
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (addr == 0) {
		ret = __LINE__;
		goto exit;
	}

	ove[index] = (reg_ove_t *)addr;
	ove_size[index] = size;

exit:
	return ret;
}

/* Get register base address */
int csp_ove_get_register_base(unsigned int index, unsigned long *addr)
{
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (addr == NULL) {
		ret = __LINE__;
		goto exit;
	}

	*addr = (unsigned long)ove[index];
exit:
	return ret;
}

/* Initialize module */
int csp_ove_init(unsigned int index)
{
	int ret = 0;
	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
exit:
	return ret;
}

/* Deinitialize module */
int csp_ove_exit(unsigned int index)
{
	int ret = 0;
	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
exit:
	return ret;
}

/* Enable module */
int csp_ove_enable(unsigned int index)
{
	reg_ove_ctl_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->ctl;
	reg.val = READREG32_RELAX(reg_addr);
	/* Enable ove */
	reg.bits.en = 1;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* Disable module */
int csp_ove_disable(unsigned int index)
{
	reg_ove_ctl_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}

	/* Disable module */
	reg_addr = (unsigned int *)&ove[index]->ctl;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.en = 0;
	WRITEREG32(reg_addr, reg.val);
exit:
	return ret;
}

/**
 * Set ove output mode
 * @itl: 0: output progreesive, 1: output interlace
 * @field_pol: field polarity, valid when itl is 1
 *             0:Low/High(Field 1/2) output (0/2/4/..)/(1/3/5/..) line
 *             1:Low/High(Field 1/2) output (1/3/5/..)/(0/2/4/..) line
 */
int csp_ove_set_output_mode(unsigned int index, unsigned int itl,
			    unsigned int field_pol)
{
	reg_ove_ctl_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->ctl;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.outmode = itl;
	reg.bits.fieldpol = field_pol;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/**
 * Select output port
 * @outsel: 0: output to dc; 1: to se0; 2: to se1
 */
int csp_ove_set_output_port(unsigned int index, unsigned int outsel)
{
	reg_ove_cfg_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->cfg;
	reg.val = READREG32(reg_addr);
	reg.bits.outsel = outsel;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/* set backcolor */
int csp_ove_set_backcolor(unsigned int index, const unsigned int color)
{
	reg_ove_bkcolor_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->bkcolor;
	/* Fill backcolor component and set it */
	reg.val = color;
	WRITEREG32_RELAX(reg_addr, reg.val);
exit:
	return ret;
}

/* get current backcolor */
int csp_ove_get_backcolor(unsigned int index, unsigned int *color)
{
	reg_ove_bkcolor_t reg;
	unsigned int *reg_addr;
	int ret = 0;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (color == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->bkcolor;
	/* Read backcolor */
	reg.val = READREG32_RELAX(reg_addr);
	*color = reg.val;

exit:
	return ret;
}

/*
 * Set screen resolution
 * @width/height: in pixel unit
 */
int csp_ove_set_window_size(unsigned int index, unsigned int width,
			    unsigned int height)
{
	int ret = 0;
	reg_ove_winsize_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (width == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (height == 0) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->winsize;
	reg.val = 0;
	reg.bits.w = width - 1;
	reg.bits.h = height - 1;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Get screen resolution
 * @width/height: in pixel unit
 */
int csp_ove_get_window_size(unsigned int index, unsigned int *width,
			    unsigned int *height)
{
	int ret = 0;

	reg_ove_winsize_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (width == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (height == NULL) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->winsize;
	reg.val = READREG32_RELAX(reg_addr);
	*width = reg.bits.w + 1;
	*height = reg.bits.h + 1;

exit:
	return ret;

}

/*
 * Load double buffer register
 * After we update the config of module, we need to load
 * the double buffer register content into registers
 */
int csp_ove_load_dbr(unsigned int index)
{
	int ret = 0;
	reg_ove_ctl_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->ctl;
	reg.val = READREG32(reg_addr);
	/* Start load double buffer register */
	reg.bits.dbc = 1;
	/* Disable double buffer register auto load function */
	reg.bits.dbautold = 1;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * After we load double buffer registe content into register,
 * we may need to check whether it has finished yet!
 */
bool csp_ove_is_load_dbr_finish(unsigned int index)
{
	reg_ove_ctl_t reg;
	unsigned int *reg_addr;
	bool ret = false;

	if (index >= OVE_NUM) {
		ret = false;
		goto exit;
	}
	/*
	 * When we want to load double buffer register, we set dbc bit,
	 * and this bit will be self clear when it finish.
	 */
	reg_addr = (unsigned int *)&ove[index]->ctl;
	reg.val = READREG32(reg_addr);
	if (reg.bits.dbc == 0)
		ret = true;

exit:
	return ret;
}

/* overlay control interface */

/* Show overlay on the screen */
int csp_ove_ovl_show(unsigned int index, unsigned int ovl_index)
{
	int ret = 0;
	reg_ove_cfg_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	switch (ovl_index) {
	case 0:
		reg.bits.ovlen0 = 1;
		break;
	case 1:
		reg.bits.ovlen1 = 1;
		break;
	case 2:
		reg.bits.ovlen2 = 1;
		break;
	case 3:
		reg.bits.ovlen3 = 1;
		break;
	default:
		ret = __LINE__;
		break;
	}
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/* Hide ovlery on the screen */
int csp_ove_ovl_hide(unsigned int index, unsigned int ovl_index)
{
	int ret = 0;
	reg_ove_cfg_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}

	reg_addr = (unsigned int *)&ove[index]->cfg;
	reg.val = READREG32_RELAX(reg_addr);
	switch (ovl_index) {
	case 0:
		reg.bits.ovlen0 = 0;
		break;
	case 1:
		reg.bits.ovlen1 = 0;
		break;
	case 2:
		reg.bits.ovlen2 = 0;
		break;
	case 3:
		reg.bits.ovlen3 = 0;
		break;
	default:
		ret = __LINE__;
		break;
	}

	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Set alpha mode for specified overlay
 * There are three alpha mode to be choose, When
 * each pixel carrys its pixel alpha, you are recommended
 * to choose PIXEL_ALPHA or PLANE_PIXEL_ALPHA for
 * better effect.
 * If pixel doesn't have its own alpha, or you just
 * want the overlay to overlap on another window,
 * you are recommended to choose PLANE_ALPHA
 * and fix plane alpha to 0XFF
 */
int csp_ove_ovl_set_alpha_mode(unsigned int index, unsigned int ovl_index,
			dpu_alpha_mode_t mode, unsigned int premultiplied)
{
	int ret = 0;
	reg_ove_ch0_ovlcfg_t reg;
	unsigned int *reg_addr;
	unsigned int alphactl, colorctl;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (mode >= DPU_ALPHA_MODE_MAX) {
		ret = __LINE__;
		goto exit;
	}
	if ((premultiplied == 1) && (mode == DPU_PLANE_ALPHA)) {
		/* Not support this case */
		ret = __LINE__;
		goto exit;
	}
	/*
	 * none-premultiplied     colorctl          alphactl
	 * pixel_alpha               1                  0
	 * plane_alpha               2                  1
	 * plane_pixel_alpha         3                  2
	 *
	 * premultiplied:
	 * pixel_alpha               0                  0
	 * plane_alpha               x                  x
	 * plane_pixel_alpha         2                  2
	 *
	 * colorctl: 0 - original color
	 *           1 - color * pixel alpha value
	 *           2 - color * plane alpha value
	 *           3 - color * pixel alpha value * plane alpha value
	 * alphactl: 0 - alpha = pixel alpha value
	 *           1 - alpha = plane alpha value
	 *           2 - alpha = pixel alpha value * plane alpha value
	 */

	if (premultiplied == 0) {
		alphactl = mode;
		colorctl = alphactl + 1;
	} else {
		alphactl = mode;
		colorctl = alphactl;
	}
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.alphactl = alphactl;
	reg.bits.colorctl = colorctl;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/*
 * Set plane alpha value, only valid on
 * PLANE_ALPHA or PLANE_PIXEL_ALPHA mode
 */
int csp_ove_ovl_set_alpha_value(unsigned int index, unsigned int ovl_index,
				unsigned int value)
{
	int ret = 0;
	reg_ove_ch0_ovlcfg_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	value = value & 0xff;
	/*
	 * Get the ovlcfg register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.plaalpha = value;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/* Get plane alpha value */
int csp_ove_ovl_get_alpha_value(unsigned int index, unsigned int ovl_index,
				unsigned int *value)
{
	int ret = 0;
	reg_ove_ch0_ovlcfg_t reg;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (value == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the ovlcfg register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	*value = reg.bits.plaalpha;

exit:
	return ret;
}

/* Set the destination rectangle where the overlay show on the window. */
int csp_ove_ovl_set_dest_rectangle(unsigned int index, unsigned int ovl_index,
				     const dpu_rectangle_t *rect)
{
	int ret = 0;
	reg_ove_ch0_ovlsize_t reg;
	reg_ove_ch0_ovlcoor_t reg_coor;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the ovlsize register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlsize;
	reg_addr += ovl_index;
	reg.val = 0;
	reg.bits.w = (rect->w == 0) ? 0 : (rect->w - 1);
	reg.bits.h = (rect->h == 0) ? 0 : (rect->h - 1);
	WRITEREG32_RELAX(reg_addr, reg.val);

	/*
	 * Get the ovlcoor register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcoor;
	reg_addr += ovl_index;
	reg_coor.val = 0;
	reg_coor.bits.x = rect->x;
	reg_coor.bits.y = rect->y;
	WRITEREG32_RELAX(reg_addr, reg_coor.val);

exit:
	return ret;
}

/* Get the destination rectangle where the overlay show on the window. */
int csp_ove_ovl_get_dest_rectangle(unsigned int index, unsigned int ovl_index,
				     dpu_rectangle_t *rect)
{
	int ret = 0;
	reg_ove_ch0_ovlsize_t reg;
	reg_ove_ch0_ovlcoor_t reg_coor;
	unsigned int *reg_addr;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (rect == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the ovlsize register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlsize;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	rect->w = reg.bits.w + 1;
	rect->h = reg.bits.h + 1;

	/*
	 * Get the ovlcoor register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcoor;
	reg_addr += ovl_index;
	reg_coor.val = READREG32_RELAX(reg_addr);
	rect->x = reg_coor.bits.x;
	rect->y = reg_coor.bits.y;

exit:
	return ret;
}

/*
 * csp_ove_ovl_set_buffer - set buffer related information
 *
 * @index     : Specify which ove
 * @ovl_index : Specify which overlay
 * @fb        : Buffer related information
 * Returns negative errno on error, or Zero for success.
 */
int csp_ove_ovl_set_buffer(unsigned int index, unsigned int ovl_index,
			   const dpu_fb_t *fb)
{
	int ret = 0;
	unsigned int haddr, laddr; /* High address, low address */
	unsigned int line_stride;  /* In bit unit */
	unsigned int *reg_addr;
	reg_ove_ch0_ovlcfg_t   reg;
	reg_ove_ch0_ovlbufladd_t  reg_bufladdr;
	reg_ove_ovlbufhadd_t   reg_bufhaddr;
	reg_ove_ch0_ovllnstd_t reg_buflnstd;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (fb == NULL) {
		ret = __LINE__;
		goto exit;
	}
	/*
	 * Get the ovlcfg register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	/* Set buffer format and pixel order */
	reg.bits.fmt = DPU_FMT_OVEINDEX(fb->format);
	WRITEREG32_RELAX(reg_addr, reg.val);

	line_stride = fb->linestride_bit[0];
	haddr = (unsigned int)(fb->addr[0] >> 32);
	laddr = (unsigned int)(fb->addr[0] & 0xffffffff);

	/* Set the low 32bit buffer address */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlbufladd;
	reg_addr += ovl_index;
	reg_bufladdr.bits.add = laddr;
	WRITEREG32_RELAX(reg_addr, reg_bufladdr.val);

	/* Set the high 4bit buffer address */
	reg_addr = (unsigned int *)&ove[index]->ovlbufhadd;
	reg_bufhaddr.val = READREG32_RELAX(reg_addr);
	switch (ovl_index) {
	case 0:
		reg_bufhaddr.bits.ovl0add = haddr;
		break;
	case 1:
		reg_bufhaddr.bits.ovl1add = haddr;
		break;
	case 2:
		reg_bufhaddr.bits.ovl2add = haddr;
		break;
	case 3:
		reg_bufhaddr.bits.ovl3add = haddr;
		break;
	default:
		ret = __LINE__;
		break;
	}
	WRITEREG32_RELAX(reg_addr, reg_bufhaddr.val);

	/* Set the buffer linestride in bit unit */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovllnstd;
	reg_addr += ovl_index;
	reg_buflnstd.bits.lnstd = line_stride;
	WRITEREG32_RELAX(reg_addr, reg_buflnstd.val);

exit:
	return ret;
}

/* We can't get the correct fb result, this interface is only for test */
int csp_ove_ovl_get_buffer(unsigned int index, unsigned int ovl_index,
			   dpu_fb_t *fb)
{
	int ret = 0;
	unsigned int haddr, laddr; /* High address, low address */
	unsigned int *reg_addr;
	reg_ove_ch0_ovlcfg_t   reg;
	reg_ove_ch0_ovlbufladd_t  reg_bufladdr;
	reg_ove_ovlbufhadd_t   reg_bufhaddr;
	reg_ove_ch0_ovllnstd_t reg_buflnstd;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (fb == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the ovlcfg register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	fb->format = (dpu_pixel_format_t)reg.bits.fmt;

	/* Get the low 32bit buffer address */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlbufladd;
	reg_addr += ovl_index;
	reg_bufladdr.val = READREG32_RELAX(reg_addr);
	laddr = reg_bufladdr.bits.add;

	/* Get the high 4bit buffer address */
	reg_addr = (unsigned int *)&ove[index]->ovlbufhadd;
	reg_bufhaddr.val = READREG32_RELAX(reg_addr);
	switch (ovl_index) {
	case 0:
		haddr = reg_bufhaddr.bits.ovl0add;
		break;
	case 1:
		haddr = reg_bufhaddr.bits.ovl1add;
		break;
	case 2:
		haddr = reg_bufhaddr.bits.ovl2add;
		break;
	case 3:
		haddr = reg_bufhaddr.bits.ovl3add;
		break;
	default:
		break;
	}
	/* we fill the address in bit unit, this only for test */
	fb->addr[0] = laddr | ((unsigned long long)haddr << 32);

	/* Set the buffer linestride in bit unit */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovllnstd;
	reg_addr += ovl_index;
	reg_buflnstd.val = READREG32_RELAX(reg_addr);
	fb->linestride_bit[0] = reg_buflnstd.bits.lnstd;

exit:
	return ret;
}

/*
 * Set the work mode of overlay
 * The mode depends on the format and other requirement.
 * If the image format is 1/2/4/8bpp, please choose PALETTE MODE,
 * and choose NORMAL MODE, when image format is normal rgb format.
 */
int csp_ove_ovl_set_work_mode(unsigned int index, unsigned int ovl_index,
				dpu_window_work_mode_t wkmod)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_ove_ch0_ovlcfg_t reg;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (wkmod >= DPU_WINDOW_WORK_MODE_MAX) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the ovcfg register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	reg.bits.wkmod = (dpu_window_work_mode_t)wkmod;
	WRITEREG32_RELAX(reg_addr, reg.val);

exit:
	return ret;
}

/* Get the work mode of ovlery */
int csp_ove_ovl_get_work_mode(unsigned int index, unsigned int ovl_index,
				dpu_window_work_mode_t *wkmod)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_ove_ch0_ovlcfg_t reg;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (wkmod == NULL) {
		ret = __LINE__;
		goto exit;
	}

	/*
	 * Get the ovlcfg register base,
	 * and then move forward to the specified one
	 */
	reg_addr = (unsigned int *)&ove[index]->ch0_ovlcfg;
	reg_addr += ovl_index;
	reg.val = READREG32_RELAX(reg_addr);
	*wkmod = (dpu_window_work_mode_t)reg.bits.wkmod;

exit:
	return ret;
}

/*
 * Update palette table content
 * If the image format is 1/2/4/8bpp, you need to provide a palette table
 *
 * @bpp:     the bit per pixel of buffer for this overlay
 * @palette: the address of buffer contained palette table, must be 4bytes align
 * @bytes: the bytes of palette data, must be 4bytes align
 * @offset: the destination offset, in bytes, must be 4bytes align
 *
 * Cause the palette table is saved in sram, and sram is single buffer,
 * so when the specified window is show on the screen, this funtion
 * can only be called in vertical blank interval.
 * Ofcourse, when the window is hide, you can call it at anytime as you like.
 */
int csp_ove_ovl_update_palette_table(unsigned int index, unsigned int ovl_index,
				unsigned int bpp, const unsigned char *palette,
				unsigned int bytes, unsigned int offset)
{
	int ret = 0;
	unsigned long base;
	unsigned int *reg_addr;
	reg_ove_sramacc_t reg;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (!((bpp == 1) || (bpp == 2) || (bpp == 4) || (bpp == 8))) {
		ret = __LINE__;
		goto exit;
	}
	if ((offset + bytes) > (((unsigned int)1 << bpp) * 4)) {
		ret = __LINE__;
		goto exit;
	}
	if (palette == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if ((bpp == 1) || (bpp == 2)) {
		/*  1bpp or 2bpp, each overlay has its own palette table */
		base = (unsigned long)&ove[index]->ch0_ovlpal0;
		base += ovl_index * 0x10;
	} else if (bpp == 4) {
		/*
		 * 4bpp, each overlay has its own palette table,
		 * and each table takes one parts of sram
		 */
		base = (unsigned long)&ove[index]->pal;
		base = base + 0x100 * ovl_index;
	} else if (bpp == 8) {
		/*
		 * 8bpp, all overlay share the same palette table
		 * located in the sram
		 */
		base = (unsigned long)&ove[index]->pal;
	} else {
		ret = __LINE__;
		goto exit;
	}

	/* sram accessed by ahb port */
	reg_addr = (unsigned int *)&ove[index]->sramacc;
	reg.val = READREG32(reg_addr);
	reg.bits.sramacc = 1;
	WRITEREG32(reg_addr, reg.val);
	ret = ove_update_lut((unsigned char *)base, palette, bytes, offset);
	/* sram accessed by hardware */
	reg.bits.sramacc = 0;
	WRITEREG32(reg_addr, reg.val);

exit:
	return ret;
}

int csp_ove_ovl_get_palette_table(unsigned int index, unsigned int ovl_index,
				  unsigned int bpp, unsigned char *palette,
				  unsigned int bytes, unsigned int offset)
{
	int ret = 0;
	unsigned long base;

	if (index >= OVE_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (ovl_index >= OVE_OVL_NUM) {
		ret = __LINE__;
		goto exit;
	}
	if (!((bpp == 0) || (bpp == 1) || (bpp == 2) || (bpp == 3))) {
		ret = __LINE__;
		goto exit;
	}
	if ((offset + bytes) > (((unsigned int)1 << bpp) * 4)) {
		ret = __LINE__;
		goto exit;
	}
	if (palette == NULL) {
		ret = __LINE__;
		goto exit;
	}

	if ((bpp == 0) || (bpp == 1)) {
		/*  1bpp or 2bpp, each overlay has its own palette table */
		base = (unsigned long)&ove[index]->ch0_ovlpal0;
		base += ovl_index * 0x10;
	} else if (bpp == 2) {
		/*
		 * 4bpp, each overlay has its own palette table,
		 * and each table takes one parts of sram
		 */
		base = (unsigned long)&ove[index]->pal;
		base = base + 0x100 * ovl_index;
	} else if (bpp == 3) {
		/*
		 * 8bpp, all overlay share the same palette table
		 * located in the sram
		 */
		base = (unsigned long)&ove[index]->pal;
	}

	ret = ove_get_lut((unsigned char *)base, palette, bytes, offset);

exit:
	return ret;
}

#endif /*__CSP_DPU_OVE_C__ */
