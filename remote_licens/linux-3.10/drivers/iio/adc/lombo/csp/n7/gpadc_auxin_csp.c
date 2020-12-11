/*
 * gpadc_auxin_csp.c - gpadc csp aux operation
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

/* Set AUXIN enable */
void csp_auxin_set_en(void *base, AUXIN_CHAN chan, bool en)
{
	reg_gpadc_gpac_t reg;

	reg.val = readl(base + GPADC_GPAC_OFF);

	if (chan == AUXIN_CHAN_0)
		reg.bits.auxin0e = en ? 1 : 0;
	else if (chan == AUXIN_CHAN_1)
		reg.bits.auxin1e = en ? 1 : 0;

	writel(reg.val, base + GPADC_GPAC_OFF);
}

/* Get AUXINx data */
u32 csp_auxin_get_data(void *base, AUXIN_CHAN chan)
{
	u32 val;

	if (chan == AUXIN_CHAN_0) {
		reg_gpadc_ain0dat_t reg;

		reg.val = READREG32(base + GPADC_AIN0DAT_OFF);
		val = reg.bits.dat;
	} else if (chan == AUXIN_CHAN_1) {
		reg_gpadc_ain1dat_t reg;

		reg.val = READREG32(base + GPADC_AIN1DAT_OFF);
		val = reg.bits.dat;
	} else {
		pr_warn("[GPADC AUXIN CSP]should not run here!");
		val = 0;
	}

	return val;
}

/* Get AUXINx sample hold data */
u32 csp_aux_get_sh_data(void *base, AUXIN_CHAN chan)
{
	u32 val;

	if (chan == AUXIN_CHAN_0) {
		reg_gpadc_aux0hdat_t reg;

		reg.val = READREG32(base + GPADC_AUX0HDAT_OFF);
		val = reg.bits.auxin0shdat;
	} else if (chan == AUXIN_CHAN_1) {
		reg_gpadc_aux1hdat_t reg;

		reg.val = READREG32(base + GPADC_AUX1HDAT_OFF);
		val = reg.bits.auxin1hdat;
	} else {
		pr_warn("[GPADC AUXIN CSP]should not run here!");
		val = 0;
	}

	return val;
}

/* Set AUXINx threashold data */
void csp_aux_set_th_data(void *base, AUXIN_CHAN chan, u32 val)
{
	if (chan == AUXIN_CHAN_0) {
		reg_gpadc_aux0tdat_t reg;

		reg.val = READREG32(base + GPADC_AUX0TDAT_OFF);
		reg.bits.auxin0tdat = val;

		WRITEREG32(base + GPADC_AUX0TDAT_OFF, reg.val);
	} else if (chan == AUXIN_CHAN_1) {
		reg_gpadc_aux1tdat_t reg;

		reg.val = READREG32(base + GPADC_AUX1TDAT_OFF);
		reg.bits.auxin1tdat = val;

		WRITEREG32(base + GPADC_AUX1TDAT_OFF, reg.val);
	}
}

