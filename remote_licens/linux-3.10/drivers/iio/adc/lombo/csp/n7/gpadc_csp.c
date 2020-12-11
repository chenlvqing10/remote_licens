/*
 * gpadc_csp.c - gpadc csp operation
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
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
 */

#include "gpadc_csp.h"

/* Set GPADC enable */
void csp_gpadc_set_en(void *base, bool en)
{
	reg_gpadc_gpaen_t reg;

	reg.val = READREG32(base + GPADC_GPAEN_OFF);
	reg.bits.gen = en ? 1 : 0;

	WRITEREG32(base + GPADC_GPAEN_OFF, reg.val);
}

/* Set GPADC calibration enable */
void csp_gpadc_set_cali_en(void *base, bool en)
{
	reg_gpadc_gpaen_t reg;

	reg.val = READREG32(base + GPADC_GPAEN_OFF);
	reg.bits.cen = en ? 1 : 0;

	WRITEREG32(base + GPADC_GPAEN_OFF, reg.val);
}

/* Set GPADC sample data average enable */
void csp_gpadc_set_avg_en(void *base, bool en)
{
	reg_gpadc_gpac_t reg; /* control register setup */

	reg.val = READREG32(base + GPADC_GPAC_OFF);
	reg.bits.sdae = en ? 1 : 0;

	WRITEREG32(base + GPADC_GPAC_OFF, reg.val);
}

/* GPADC clock source select */
void csp_gpadc_set_clk(void *base, GPADC_CLK_SRC clk)
{
	reg_gpadc_gpac_t reg; /* control register setup */

	reg.val = READREG32(base + GPADC_GPAC_OFF);
	reg.bits.cce = clk;

	WRITEREG32(base + GPADC_GPAC_OFF, reg.val);
}

/* Set GPADC sample rate */
void csp_gpadc_set_sample_rate(void *base, GPADC_SRC_TYPE t)
{
	reg_gpadc_gpac_t reg; /* control register setup */

	reg.val = READREG32(base + GPADC_GPAC_OFF);
	reg.bits.src = t;

	WRITEREG32(base + GPADC_GPAC_OFF, reg.val);
}

void csp_gpadc_set_int_en(void *base, u32 source, bool en)
{
	reg_gpadc_gpaie_t reg;

	reg.val = READREG32(base + GPADC_GPAIE_OFF);
	if (en)
		reg.val |= source;
	else
		reg.val &= ~source;

	WRITEREG32(base + GPADC_GPAIE_OFF, reg.val);
}

u32 csp_gpadc_get_int_pend(void *base, u32 source)
{
	reg_gpadc_gpais_t reg;

	reg.val = READREG32(base + GPADC_GPAIS_OFF);
	return reg.val & source;
}

void csp_gpadc_clr_int_pend(void *base, u32 source)
{
	reg_gpadc_gpaic_t reg;

	reg.val = 0;
	reg.val |= source;

	WRITEREG32(base + GPADC_GPAIC_OFF, reg.val);
}

