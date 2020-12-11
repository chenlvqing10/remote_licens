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

#ifndef __CSP_DPU_SE_C__
#define __CSP_DPU_SE_C__

#include <mach/csp.h>		/* for include dc.h in mach/csp.h */
#include "csp_dpu_se_reg.h"
#include "csp_dpu_se.h"

#define CSC_12BIT
#ifdef CSC_12BIT
#define CSCRNDS (2048)

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
#endif /* endif of CSC_12BIT */

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

/* post processing related */
static int  peakfilter0[9] =   {  0,   0,   0, -32, 64, -32,   0,  0,    0};
static int  peakfilter1[9] =   {  0,   0, -32,   0, 64,   0, -32,  0,    0};
static int  peakfilter2[9] =   {-16, -32, -16,  32, 64,  32, -16, -32, -16};
static int  ltifilter_int[9] = { -1,  -2,  -1,   2,  4,   2,  -1,  -2,  -1};
static int  ctifilter_int[9] = { -1,  -2,  -1,   2,  4,   2,  -1,  -2,  -1};

static u32 reg_size[SE_DEV_COUNT] = { 0 };
regs_se_t *se_reg[SE_DEV_COUNT] = { 0 };
tabs_se_t *se_tab[SE_DEV_COUNT] = { 0 };
lut_se_t *se_lut[SE_DEV_COUNT] = { 0 };
hist_se_t *se_hist[SE_DEV_COUNT] = { 0 };

s32 csp_se_init(u32 id)
{
	/* Improve the SE module performance, burst length */
	WRITEREG32((unsigned long)(se_reg[id]) + 0x0300, 0x010f0003);
	WRITEREG32((unsigned long)(se_reg[id]) + 0x0304, 0x010f0003);
	WRITEREG32((unsigned long)(se_reg[id]) + 0x0308, 0x010f0003);

	return 0;
}

s32 csp_se_exit(u32 id)
{
	return 0;
}

s32 csp_se_set_register_base(u32 id, void *addr, u32 size)
{
	se_reg[id] = (regs_se_t *)(addr);
	se_tab[id] = (tabs_se_t *)(addr + REG_SE_CH0_XCOEF0);
	se_lut[id] = (lut_se_t *)(addr + REG_SE_CH0LUT);
	se_hist[id]	= (hist_se_t *)(addr + REG_SE_CHHIST);
	reg_size[id] = size;

	return 0;
}

s32 csp_se_get_register_base(u32 id, unsigned long *addr)
{
	addr[0] = (unsigned long)(se_reg[id]);

	return 0;
}

s32 csp_se_enable(u32 id)
{
	reg_se_ctl_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.se_en = 1;
	tmpreg.bits.rstn = 1;
	WRITEREG32(&(se_reg[id]->ctl), tmpreg.val);

	return 0;
}

s32 csp_se_disable(u32 id)
{
	reg_se_ctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->ctl));
	tmpreg.bits.se_en = 0;
	WRITEREG32(&(se_reg[id]->ctl), tmpreg.val);

	return 0;
}

s32 csp_se_stream_start(u32 id)
{
	reg_se_strmctl_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.strm_start = 1;
	WRITEREG32(&(se_reg[id]->strmctl), tmpreg.val);

	return 0;
}

s32 csp_se_stream_stop(u32 id)
{
	reg_se_strmctl_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.strm_start = 0;
	WRITEREG32(&(se_reg[id]->strmctl), tmpreg.val);

	return 0;
}

s32 csp_se_set_update_mode(u32 id, se_upd_mode_t update_mode)
{
	reg_se_updctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->updctl));
	tmpreg.bits.sync_mode = update_mode;
	WRITEREG32(&(se_reg[id]->updctl), tmpreg.val);

	return 0;
}

s32 csp_se_update_dbr(u32 id)
{
	reg_se_updctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->updctl));
	tmpreg.bits.upd_im = 1;
	WRITEREG32(&(se_reg[id]->updctl), tmpreg.val);

	return 0;
}

s32 csp_se_set_infmt(u32 id, se_img_fmt_t infmt)
{
	reg_se_cfg1_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.infmt = infmt;
	WRITEREG32(&(se_reg[id]->cfg1), tmpreg.val);

	return 0;
}

s32 csp_se_set_input_sel(u32 id, se_input_sel_t sel)
{
	reg_se_cfg0_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg0));
	tmpreg.bits.in_sel = sel;
	WRITEREG32(&(se_reg[id]->cfg0), tmpreg.val);

	return 0;
}

s32 csp_se_set_online_mode(u32 id)
{
	reg_se_cfg0_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg0));
	tmpreg.bits.dcout_en = 1;
	WRITEREG32(&(se_reg[id]->cfg0), tmpreg.val);

	return 0;
}

s32 csp_se_set_offline_mode(u32 id)
{
	reg_se_cfg0_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg0));
	tmpreg.bits.dcout_en = 0;
	WRITEREG32(&(se_reg[id]->cfg0), tmpreg.val);

	return 0;
}

/*
 * Used for online SE function
 */
s32 csp_se_set_online_field_pol(u32 id, u32 pol)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.field_pol = pol;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/*
 * Used for write-back only function
 */
s32 csp_se_set_offline_field_lv(u32 id, u32 field_lv)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.wb_field_lv = field_lv;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/*
 * Select progressive output mode
 */
s32 csp_se_set_output_progressive(u32 id)
{
	reg_se_cfg0_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg0));
	tmpreg.bits.out_mode = 0;
	WRITEREG32(&(se_reg[id]->cfg0), tmpreg.val);

	return 0;
}

/*
 * Select interlace output mode
 */
s32 csp_se_set_output_interlaced(u32 id)
{
	reg_se_cfg0_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg0));
	tmpreg.bits.out_mode = 1;
	WRITEREG32(&(se_reg[id]->cfg0), tmpreg.val);

	return 0;
}

s32 csp_se_enable_premul(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.premul_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

s32 csp_se_disable_premul(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.premul_en = 0;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/**
 * set all alpha value to 0xff.
 */
s32 csp_se_enable_alpha(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.alpha_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/**
 * 1. use original alpha value if fb has alpha.
 * 2. output all alpha value as 0x0 if fb has no alpha.
 */
s32 csp_se_disable_alpha(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.alpha_en = 0;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

s32 csp_se_enable_csci(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.csci_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

s32 csp_se_disable_csci(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.csci_en = 0;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

s32 csp_se_enable_csco(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.csco_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

s32 csp_se_disable_csco(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.csco_en = 0;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

s32 csp_se_set_csci_coef(u32 id,
			 enum tag_csc_conv yuv2rgb,
			 enum tag_csc_std csc_sel)
{
	u32 i = 0;
	u32 tmpval;
	int *coef;

	int max_mask = (1 << (CSC_SHIFT_BIT + 2 + 1)) - 1;

	if (yuv2rgb == CSC_YUV2RGB)
		coef = Y2RCOEF_ARRAY[csc_sel];
	else if (yuv2rgb == CSC_RGB2YUV)
		coef = R2YCOEF_ARRAY[csc_sel];
	else
		coef = CSC_BYPASS_COEF[csc_sel];

	for (i = 0; i < 12; i++) {
		tmpval = coef[i] & max_mask;
		WRITEREG32(&(se_reg[id]->csci_coef[i]), tmpval);
	}

	return 0;
}

s32 csp_se_set_csco_coef(u32 id,
			 enum tag_csc_conv yuv2rgb,
			 enum tag_csc_std csc_sel)
{
	u32 i = 0;
	u32 tmpval;
	int *coef;

	int max_mask = (1 << (CSC_SHIFT_BIT + 2 + 1)) - 1;

	if (yuv2rgb == CSC_YUV2RGB)
		coef = Y2RCOEF_ARRAY[csc_sel];
	else if (yuv2rgb == CSC_RGB2YUV)
		coef = R2YCOEF_ARRAY[csc_sel];
	else
		coef = CSC_BYPASS_COEF[csc_sel];

	for (i = 0; i < 12; i++) {
		tmpval = coef[i] & max_mask;
		WRITEREG32(&(se_reg[id]->csco_coef[i]), tmpval);
	}

	return 0;
}

/* irq and status */
s32 csp_se_set_linecounter_trig_num(u32 id, u32 num)
{
	reg_se_lncnt_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->lncnt));
	tmpreg.bits.trig_num = (num > 0x1FFFF) ? 0x1FFFF : num;
	WRITEREG32(&(se_reg[id]->lncnt), tmpreg.val);

	return 0;
}

s32 csp_se_set_linecounter_trig_field(u32 id, se_trig_field_t field_sel)
{
	reg_se_lncnt_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->lncnt));
	tmpreg.bits.trig_field = field_sel;
	WRITEREG32(&(se_reg[id]->lncnt), tmpreg.val);

	return 0;
}

s32 csp_se_enable_irq_src(u32 id, se_irq_src_t irq_trig)
{
	reg_se_intctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->intctl));
	tmpreg.bits.lntrig_en = (irq_trig & SE_IRQ_LINECNT) ? 1 : 0;
	tmpreg.bits.wbfin_en = (irq_trig & SE_IRQ_WB_FINISH) ? 1 : 0;
	tmpreg.bits.wbovfl_en = (irq_trig & SE_IRQ_WB_OVERFLOW) ? 1 : 0;
	tmpreg.bits.wbtmout_en = (irq_trig & SE_IRQ_WB_TIMEOUT) ? 1 : 0;
	WRITEREG32(&(se_reg[id]->intctl), tmpreg.val);

	return 0;
}

s32 csp_se_disable_irq_src(u32 id, se_irq_src_t irq_trig)
{
	reg_se_intctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->intctl));
	tmpreg.bits.lntrig_en = (irq_trig & SE_IRQ_LINECNT) ? 0 : 1;
	tmpreg.bits.wbfin_en = (irq_trig & SE_IRQ_WB_FINISH) ? 0 : 1;
	tmpreg.bits.wbovfl_en = (irq_trig & SE_IRQ_WB_OVERFLOW) ? 0 : 1;
	tmpreg.bits.wbtmout_en = (irq_trig & SE_IRQ_WB_TIMEOUT) ? 0 : 1;
	WRITEREG32(&(se_reg[id]->intctl), tmpreg.val);

	return 0;
}

s32 csp_se_get_irq_status(u32 id)
{
	reg_se_intsta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->intsta));

	return tmpreg.val;
}

s32 csp_se_clear_irq_status(u32 id, se_irq_src_t irq_trig)
{
	reg_se_intclr_t tmpreg;
	tmpreg.val = irq_trig;
	WRITEREG32(&(se_reg[id]->intclr), tmpreg.val);

	return 0;
}

/*
 * Used only for online mode SE function when output mode is interlaced
 */
s32 csp_se_get_field_status(u32 id)
{
	reg_se_csta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->strmctl));

	return tmpreg.bits.field_sta;
}

/*
 * Used only for online mode SE function
 */
s32 csp_se_get_frame_num(u32 id)
{
	reg_se_csta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->strmctl));

	return tmpreg.bits.frm_cnt;
}

/*
 * Get current output line_num for current frame
 */
s32 csp_se_get_line_num(u32 id)
{
	reg_se_csta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->sta));

	return tmpreg.bits.ln_num;
}

/*
 * Barely used
 */
s32 csp_se_get_stream_status(u32 id)
{
	reg_se_sta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->sta));

	return tmpreg.val & 0xF;
}

/*
 * Called by internal
 */
s32 csp_se_set_linebuffer_mode(u32 id, u32 lb_mode)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.lb_mode = (lb_mode > 2) ? 2 : lb_mode;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/*
 * Called by internal
 */
s32 csp_se_set_enable_rsmp(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.rsmp_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/*
 * Called by internal
 */
s32 csp_se_set_disable_rsmp(u32 id)
{
	reg_se_cfg2_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg.bits.rsmp_en = 0;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg.val);

	return 0;
}

/*
 * input buffer addr
 */
s32 csp_se_set_inbuf_addr(u32 id, unsigned long long addr[3])
{
	WRITEREG32(&(se_reg[id]->inaddr0), (unsigned int)addr[0]);
	WRITEREG32(&(se_reg[id]->inaddr1), (unsigned int)addr[1]);
	WRITEREG32(&(se_reg[id]->inaddr2), (unsigned int)addr[2]);

	return 0;
}

/*
 * input buffer size
 */
s32 csp_se_set_insize(u32 id, u32 inw[3], u32 inh[3])
{
	reg_se_insize0_t tmpreg;
	reg_se_insize1_t tmpreg1;
	tmpreg.bits.w = inw[0] - 1;
	tmpreg.bits.h = inh[0] - 1;
	tmpreg1.bits.w = inw[1] - 1;
	tmpreg1.bits.h = inh[1] - 1;

	WRITEREG32(&(se_reg[id]->insize0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->insize1), tmpreg1.val);

	return 0;
}

/*
 * Set se linestride in byte unit
 */
s32 csp_se_set_inlstr(u32 id, u32 lstr[3])
{
	reg_se_inlstr0_t tmpreg;
	reg_se_inlstr1_t tmpreg1;
	tmpreg.bits.lstr = lstr[0];
	tmpreg1.bits.lstr = lstr[1];

	WRITEREG32(&(se_reg[id]->inlstr0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->inlstr1), tmpreg1.val);

	return 0;
}

/*
 * Set the resampling size for SE
 */
s32 csp_se_set_rsmp_size(u32 id, u32 rsmpw, u32 rsmph)
{
	reg_se_rsmpsize_t tmpreg;
	tmpreg.bits.w = rsmpw - 1;
	tmpreg.bits.h = rsmph - 1;
	WRITEREG32(&(se_reg[id]->rsmpsize), tmpreg.val);

	return 0;
}

/*
 * Set the output crop size after resampling handle.
 */
s32 csp_se_set_out_crop_size(u32 id, u32 cropw, u32 croph)
{
	reg_se_out_cropsize_t tmpreg;
	tmpreg.bits.w = cropw - 1;
	tmpreg.bits.h = croph - 1;
	WRITEREG32(&(se_reg[id]->out_cropsize), tmpreg.val);

	return 0;
}

/*
 * Set the output crop offset after resampling handle.
 */
s32 csp_se_set_out_crop_offset(u32 id, u32 offsetx, u32 offsety)
{
	reg_se_out_cropofst_t tmpreg;
	tmpreg.bits.x = offsetx;
	tmpreg.bits.y = offsety;
	WRITEREG32(&(se_reg[id]->out_cropofst), tmpreg.val);

	return 0;
}

s32 csp_se_set_scaling_ratio_x(u32 id, u32 ratio[3])
{
	WRITEREG32(&(se_reg[id]->scrx0), ratio[0]);
	WRITEREG32(&(se_reg[id]->scrx1), ratio[1]);

	return 0;
}

s32 csp_se_set_scaling_ratio_y(u32 id, u32 ratio[3])
{
	WRITEREG32(&(se_reg[id]->scry0), ratio[0]);
	WRITEREG32(&(se_reg[id]->scry1), ratio[1]);

	return 0;
}

s32 csp_se_set_xoffset(u32 id, s32 offset[2])
{
	WRITEREG32(&(se_reg[id]->ch0xofst), offset[0]);
	WRITEREG32(&(se_reg[id]->ch1xofst), offset[1]);

	return 0;
}

s32 csp_se_set_yoffset(u32 id, s32 offset[2])
{
	WRITEREG32(&(se_reg[id]->ch0yofst), offset[0]);
	WRITEREG32(&(se_reg[id]->ch1yofst), offset[1]);

	return 0;
}

s32 csp_se_set_shift_ctl(u32 id, u32 shift_bitx[2], u32 shift_bity[2])
{
	reg_se_shiftctl_t tmpreg;
	tmpreg.val = 0;
	tmpreg.bits.ctlx0 = shift_bitx[0];
	tmpreg.bits.ctlx1 = shift_bitx[1];

	tmpreg.bits.ctly0 = shift_bity[0];
	tmpreg.bits.ctly1 = shift_bity[1];

	WRITEREG32(&(se_reg[id]->shiftctl), tmpreg.val);

	return 0;
}

/* WB function */
s32 csp_se_set_wb_format(u32 id, se_wb_fmt_t outfmt)
{
	reg_se_wbcfg_t tmpreg;

	tmpreg.val = READREG32(&(se_reg[id]->wbcfg));
	tmpreg.bits.wbfmt = outfmt;
	WRITEREG32(&(se_reg[id]->wbcfg), tmpreg.val);

	return 0;
}

s32 csp_se_start_wb(u32 id)
{
	reg_se_wbctl_t tmpreg;
	tmpreg.bits.wb_start = 1;
	WRITEREG32(&(se_reg[id]->wbctl), tmpreg.val);

	return 0;
}

s32 csp_se_stop_wb(u32 id)
{
	reg_se_wbctl_t tmpreg;
	tmpreg.bits.wb_start = 0;
	WRITEREG32(&(se_reg[id]->wbctl), tmpreg.val);

	return 0;
}

/*
 * Set the write-back size for SE
 */
s32 csp_se_set_wb_size(u32 id, u32 wbw[2], u32 wbh[2])
{
	reg_se_wbsize0_t tmpreg;
	reg_se_wbsize1_t tmpreg1;
	tmpreg.bits.w = wbw[0] - 1;
	tmpreg.bits.h = wbh[0] - 1;
	tmpreg1.bits.w = wbw[1] - 1;
	tmpreg1.bits.h = wbh[1] - 1;

	WRITEREG32(&(se_reg[id]->wbsize0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->wbsize1), tmpreg1.val);

	return 0;
}

/*
 * Set the write-back linestride for SE
 */
s32 csp_se_set_wb_linestride(u32 id, u32 lstr[3])
{
	reg_se_wblstr0_t tmpreg;
	reg_se_wblstr1_t tmpreg1;
	tmpreg.bits.lstr = lstr[0];
	tmpreg1.bits.lstr = lstr[1];

	WRITEREG32(&(se_reg[id]->wblstr0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->wblstr1), tmpreg1.val);

	return 0;
}

s32 csp_se_set_wb_addr(u32 id, unsigned long long addr[3])
{
	WRITEREG32(&(se_reg[id]->wbaddr0), (unsigned int)addr[0]);
	WRITEREG32(&(se_reg[id]->wbaddr1), (unsigned int)addr[1]);
	WRITEREG32(&(se_reg[id]->wbaddr2), (unsigned int)addr[2]);

	return 0;
}

s32 csp_se_set_wb_timer(u32 id, s32 timeout_len)
{
	reg_se_wbcfg_t tmpreg;

	tmpreg.val = READREG32(&(se_reg[id]->wbcfg));
	tmpreg.bits.wbthr = (timeout_len > 65535) ? 65535 : timeout_len;
	WRITEREG32(&(se_reg[id]->wbcfg), tmpreg.val);

	return 0;
}

s32 csp_se_get_wb_timer_status(u32 id)
{
	u32 timer_val;
	reg_se_wbtmr_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->wbtmr));
	timer_val = tmpreg.bits.wbtmr;
	return timer_val;
}

s32 csp_se_check_wb_finish(u32 id)
{
	reg_se_intsta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->intsta));

	return tmpreg.val & SE_IRQ_WB_FINISH;
}

s32 csp_se_check_wb_timeout(u32 id)
{
	reg_se_intsta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->intsta));

	return tmpreg.val & SE_IRQ_WB_TIMEOUT;
}

s32 csp_se_check_wb_overflow(u32 id)
{
	reg_se_intsta_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->intsta));

	return tmpreg.val & SE_IRQ_WB_OVERFLOW;
}

/* WB function end */

/*
 * may use other fast copy function
 */
s32 se_memcpy32(void *dst, void *src, u32 length)
{
	u32 i;
	unsigned long base;
	const u32 *ptr = (const u32 *)src;

	base = (unsigned long)dst;

	/* ((u32 *)dst)[i] = ((u32 *)src)[i]; */
	for (i = 0; i < length / 4; i++)
		WRITEREG32(base + i * 4, ptr[i]);

	return 0;
}

s32 csp_se_set_scaling_xcoef0(u32 id, u32 chnum, u32 *coef)
{
	reg_se_ctabswt_t tmpreg;
	tmpreg.val = 0;

	/* switch ahb */
	tmpreg.bits.tab_swt = 1;
	WRITEREG32(&(se_reg[id]->ctabswt), tmpreg.val);

	if (chnum == 0)
		se_memcpy32(se_tab[id]->ch0_xcoef0, coef, 128);
	else
		se_memcpy32(se_tab[id]->ch1_xcoef0, coef, 128);

	/* switch back */
	tmpreg.bits.tab_swt = 0;
	WRITEREG32(&(se_reg[id]->ctabswt), tmpreg.val);

	return 0;
}

s32 csp_se_set_scaling_xcoef1(u32 id, u32 chnum, u32 *coef)
{
	reg_se_ctabswt_t tmpreg;
	tmpreg.val = 0;

	/* switch ahb */
	tmpreg.bits.tab_swt = 1;
	WRITEREG32(&(se_reg[id]->ctabswt), tmpreg.val);

	if (chnum == 0)
		se_memcpy32(se_tab[id]->ch0_xcoef1, coef, 128);
	else
		se_memcpy32(se_tab[id]->ch1_xcoef1, coef, 128);

	/* switch back */
	tmpreg.bits.tab_swt = 0;
	WRITEREG32(&(se_reg[id]->ctabswt), tmpreg.val);

	return 0;
}

s32 csp_se_set_scaling_ycoef(u32 id, u32 chnum, u32 *coef)
{
	reg_se_ctabswt_t tmpreg;
	tmpreg.val = 0;

	/* switch ahb */
	tmpreg.bits.tab_swt = 1;
	WRITEREG32(&(se_reg[id]->ctabswt), tmpreg.val);

	if (chnum == 0)
		se_memcpy32(se_tab[id]->ch0_ycoef, coef, 128);
	else
		se_memcpy32(se_tab[id]->ch1_ycoef, coef, 128);

	/* switch back */
	tmpreg.bits.tab_swt = 0;
	WRITEREG32(&(se_reg[id]->ctabswt), tmpreg.val);

	return 0;
}

s32 csp_convert_dcfb_2_sefb(se_input_fb_t *sefb,
			    const disp_fb_t *dcfb,
			    s32 outw,
			    s32 outh)
{
	/* s32 xclip = 0; */
	/* s32 yclip = 0; */
	/* TODO, add planar1/2 size cal */
	/* add address crop calculation */
	/* convert source clip to be address offset */
	if (dcfb->clip.x) {
		sefb->inw[0] = dcfb->clip.w;
		sefb->clip.x = 0;
		/* xclip = 1; */
	}

	if (dcfb->clip.y) {
		sefb->inh[0] = dcfb->clip.h;
		sefb->clip.y = 0;
		/* yclip = 1; */
	}

	switch (sefb->infmt) {
	case ARGB8888:
	case BGRA8888:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->addr[0] = dcfb->addr[0] +
				dcfb->lstr[0] * dcfb->clip.y +
				(dcfb->clip.x) * 4;
		break;
	case ARGB4444:
	case BGRA4444:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->addr[0] = dcfb->addr[0] +
				dcfb->lstr[0] * dcfb->clip.y +
				(dcfb->clip.x) * 2;
		break;
	case ARGB1555:
	case BGRA5551:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 2;
		break;
	case RGB565:
	case BGR565:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 2;
		break;
	case UYVY:
	case YUYV:
	case VYUY:
	case YVYU:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 2;
		break;

	case YUV420SP:
	case YVU420SP:
		sefb->inw[0] = SE_DOWN_ALIGN(dcfb->clip.w, 2);
		sefb->inh[0] = SE_DOWN_ALIGN(dcfb->clip.h, 2);
		sefb->inw[1] = SE_DOWN_ALIGN(dcfb->clip.w / 2, 2);
		sefb->inh[1] = SE_DOWN_ALIGN(dcfb->clip.h / 2, 2);

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->lstr[1] = dcfb->lstr[1];

		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[1] = dcfb->addr[1] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x / 2) * 2;
		break;
	case YUV422SP:
	case YVU422SP:
		sefb->inw[0] = SE_DOWN_ALIGN(sefb->clip.w, 2);
		sefb->inh[0] = sefb->clip.h;
		sefb->inw[1] = SE_DOWN_ALIGN(sefb->clip.w, 2);
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->lstr[1] = dcfb->lstr[1];

		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[1] = dcfb->addr[1] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x / 2) * 2;
		break;

	case YUV420P:
		sefb->inw[0] = SE_DOWN_ALIGN(sefb->clip.w, 2);
		sefb->inh[0] = SE_DOWN_ALIGN(sefb->clip.h, 2);
		sefb->inw[1] = sefb->clip.w / 2;
		sefb->inh[1] = sefb->clip.h / 2;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->lstr[1] = dcfb->lstr[1];

		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[1] = dcfb->addr[1] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x / 2) * 1;
		sefb->addr[2] = dcfb->addr[2] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x / 2) * 1;
		break;
	case YUV422P:
		sefb->inw[0] = sefb->clip.w;
		sefb->inh[0] = sefb->clip.h;
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->lstr[1] = dcfb->lstr[1];

		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[1] = dcfb->addr[1] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x / 2) * 1;
		sefb->addr[2] = dcfb->addr[2] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x / 2) * 1;
		break;
	case YUV444P:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;

		sefb->lstr[0] = dcfb->lstr[0];
		sefb->lstr[1] = dcfb->lstr[1];

		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[1] = dcfb->addr[1] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[2] = dcfb->addr[2] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x) * 1;
		break;
	default:
		sefb->inw[1] = sefb->clip.w;
		sefb->inh[1] = sefb->clip.h;
		sefb->lstr[0] = dcfb->lstr[0];
		sefb->lstr[1] = dcfb->lstr[1];
		sefb->addr[0] = dcfb->addr[0] +
			dcfb->lstr[0] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[1] = dcfb->addr[1] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x) * 1;
		sefb->addr[2] = dcfb->addr[2] +
			dcfb->lstr[1] * dcfb->clip.y + (dcfb->clip.x) * 1;
		break;
	}

	return 0;
}

/* Video post processing function */
s32 csp_se_gmtab_set_coef(u32 id, s32 tab_sel, u8 coef[256])
{
	/* switch */
	reg_se_vpplutsw_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vpplutsw));
	if (tab_sel == 0)
		tmpreg.bits.lut0sw = 1;
	else if (tab_sel == 1)
		tmpreg.bits.lut1sw = 1;
	else if (tab_sel == 2)
		tmpreg.bits.lut2sw = 1;

	WRITEREG32(&(se_reg[id]->vpplutsw), tmpreg.val);

	/* copy coefficients */
	if (tab_sel == 0)
		se_memcpy32(se_lut[id]->ch0lut, coef, 256);
	else if (tab_sel == 1)
		se_memcpy32(se_lut[id]->ch1lut, coef, 256);
	else if (tab_sel == 2)
		se_memcpy32(se_lut[id]->ch2lut, coef, 256);

	/* switch back */
	if (tab_sel == 0)
		tmpreg.bits.lut0sw = 0;
	else if (tab_sel == 1)
		tmpreg.bits.lut1sw = 0;
	else if (tab_sel == 2)
		tmpreg.bits.lut2sw = 0;
	WRITEREG32(&(se_reg[id]->vpplutsw), tmpreg.val);

	return 0;
}

s32 csp_se_gmtab_enable(u32 id, u32 tab_sel)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	if (tab_sel == 0)
		tmpreg.bits.lut0en = 1;
	else if (tab_sel == 1)
		tmpreg.bits.lut1en = 1;
	else if (tab_sel == 2)
		tmpreg.bits.lut2en = 1;

	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_gmtab_disable(u32 id, u32 tab_sel)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	if (tab_sel == 0)
		tmpreg.bits.lut0en = 0;
	else if (tab_sel == 1)
		tmpreg.bits.lut1en = 0;
	else if (tab_sel == 2)
		tmpreg.bits.lut2en = 0;

	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

/*
 * Set LUT1/2 as RGB mode and map the r/g/b data as LUT data
 */
s32 csp_se_gmtab_set_rgb_mode(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));

	tmpreg.bits.lut_mod = 0;

	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

/*
 * Set LUT1/2 as yuv mode and add the u/v data with LUT data
 */
s32 csp_se_gmtab_set_yuv_mode(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));

	tmpreg.bits.lut_mod = 1;

	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_hist_enable(u32 id)
{
	reg_se_vppctl_t tmpreg;

	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.hist_en = 1;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_hist_disable(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.hist_en = 0;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_hist_clear(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.hist_clr = 1;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_peaking_enable(u32 id, s32 lvl)
{
	reg_se_vppctl_t tmpreg;
	reg_se_peakcfg_t tmpreg1;
	reg_se_cfg2_t tmpreg2;

	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e0en = 1;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	tmpreg1.bits.cor = 4;
	tmpreg1.bits.limit = 20;
	tmpreg1.bits.gain = max(1, min(15, lvl));
	WRITEREG32(&(se_reg[id]->peakcfg), tmpreg1.val);

	tmpreg2.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg2.bits.rsmp_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg2.val);

	return 0;
}

s32 csp_se_peaking_disable(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e0en = 0;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_ltiv_enable(u32 id, s32 lvl)
{
	reg_se_vppctl_t tmpreg;
	reg_se_ltivcfg_t tmpreg1;
	reg_se_cfg2_t tmpreg2;

	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e1en = 1;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	tmpreg1.bits.cor = 8;
	tmpreg1.bits.oslmt = 0;
	tmpreg1.bits.gain = max(1, min(31, lvl));
	tmpreg1.bits.gain2 = 10;
	WRITEREG32(&(se_reg[id]->ltivcfg), tmpreg1.val);

	tmpreg2.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg2.bits.rsmp_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg2.val);

	return 0;
}

s32 csp_se_ltiv_disable(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e1en = 0;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_lti_enable(u32 id, s32 lvl)
{
	reg_se_vppctl_t tmpreg;
	reg_se_lticfg_t tmpreg1;
	reg_se_cfg2_t tmpreg2;

	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e2en = 1;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	tmpreg1.bits.cor = 8;
	tmpreg1.bits.oslmt = 0;
	tmpreg1.bits.gain = max(1, min(31, lvl));
	tmpreg1.bits.gain2 = 10;
	WRITEREG32(&(se_reg[id]->lticfg), tmpreg1.val);

	tmpreg2.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg2.bits.rsmp_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg2.val);

	return 0;
}

s32 csp_se_lti_disable(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e2en = 0;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_cti_enable(u32 id, s32 lvl)
{
	reg_se_vppctl_t tmpreg;
	reg_se_cticfg_t tmpreg1;
	reg_se_cfg2_t tmpreg2;

	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e3en = 1;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	tmpreg1.bits.cor = 2;
	tmpreg1.bits.oslmt = 2;
	tmpreg1.bits.gain = max(1, min(31, lvl));
	tmpreg1.bits.gain2 = 6;
	WRITEREG32(&(se_reg[id]->cticfg), tmpreg1.val);

	tmpreg2.val = READREG32(&(se_reg[id]->cfg2));
	tmpreg2.bits.rsmp_en = 1;
	WRITEREG32(&(se_reg[id]->cfg2), tmpreg2.val);

	return 0;
}

s32 csp_se_cti_disable(u32 id)
{
	reg_se_vppctl_t tmpreg;
	tmpreg.val = READREG32(&(se_reg[id]->vppctl));
	tmpreg.bits.e3en = 0;
	WRITEREG32(&(se_reg[id]->vppctl), tmpreg.val);

	return 0;
}

s32 csp_se_peaking_set_coef(u32 id, s32 coef_type, s32 ext_coef[9])
{
	s32 i;
	reg_se_peakcoef0_t tmpreg;
	reg_se_peakcoef1_t tmpreg1;

	s32 *pkcoef;
	s32 pkcoef_int[9] = { 0 };
	s32 EdgeR0 = 6;
	s32 EdgeR1 = 8;
	s32 EdgeR2 = 2;

	if ((coef_type == -1) && (ext_coef != 0))
		pkcoef = ext_coef;
	else {
		for (i = 0; i < 9; i++)
			pkcoef_int[i] = (peakfilter0[i] * EdgeR0 +
					 peakfilter1[i] * EdgeR1 +
					 peakfilter2[i] * EdgeR2 + 16) >> 5;

		pkcoef = pkcoef_int;
	}

	tmpreg.bits.c0 = pkcoef[4];
	tmpreg.bits.c1 = pkcoef[3];
	tmpreg.bits.c2 = pkcoef[2];
	tmpreg.bits.c3 = pkcoef[1];
	tmpreg1.bits.c4 = pkcoef[0];
	WRITEREG32(&(se_reg[id]->peakcoef0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->peakcoef1), tmpreg1.val);

	return 0;
}

s32 csp_se_lti_set_coef(u32 id, s32 coef_type, s32 ext_coef[9])
{
	reg_se_lticoef0_t tmpreg;
	reg_se_lticoef1_t tmpreg1;
	s32 *ltifilter;

	if ((coef_type == -1) && (ext_coef != 0))
		ltifilter = ext_coef;
	else
		ltifilter = ltifilter_int;

	tmpreg.bits.c0 = ltifilter[4];
	tmpreg.bits.c1 = ltifilter[3];
	tmpreg.bits.c2 = ltifilter[2];
	tmpreg.bits.c3 = ltifilter[1];
	tmpreg1.bits.c4 = ltifilter[0];
	WRITEREG32(&(se_reg[id]->lticoef0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->lticoef1), tmpreg1.val);

	return 0;
}

s32 csp_se_cti_set_coef(u32 id, s32 coef_type, s32 ext_coef[9])
{
	reg_se_cticoef0_t tmpreg;
	reg_se_cticoef1_t tmpreg1;
	s32 *ctifilter;

	if ((coef_type == -1) && (ext_coef != 0))
		ctifilter = ext_coef;
	else
		ctifilter = ctifilter_int;

	tmpreg.bits.c0 = ctifilter[4];
	tmpreg.bits.c1 = ctifilter[3];
	tmpreg.bits.c2 = ctifilter[2];
	tmpreg.bits.c3 = ctifilter[1];
	tmpreg1.bits.c4 = ctifilter[0];
	WRITEREG32(&(se_reg[id]->cticoef0), tmpreg.val);
	WRITEREG32(&(se_reg[id]->cticoef1), tmpreg1.val);

	return 0;
}

s32 csp_se_calc_dynamic_rate(u32 id, struct csp_se_calc_rate_info *info)
{
#define WIDTH_720p	1280
#define HIGH_720P	720
#define WIDTH_1080p	1920
#define HIGH_1080P	1080

	unsigned long calc_rate = 0;
	int ret = 0;
	u32 xratio, yratio, ratio;
	u32 in_w, in_h;
	u32 scn_w, scn_h;

	xratio = (info->in_w << 16) / info->out_w;
	yratio = (info->in_h << 16) / info->out_h;
	ratio = max(xratio, yratio);
	in_w = max(info->in_w, info->in_h);
	in_h = min(info->in_w, info->in_h);
	scn_w = max(info->scn_w, info->scn_h);
	scn_h = min(info->scn_w, info->scn_h);

	if ((scn_h <= HIGH_720P) && (scn_w <= WIDTH_720p)) {
		if ((in_h <= HIGH_720P) && (in_w <= WIDTH_720p)) {
			calc_rate = 100000000;
		} else if ((in_h <= HIGH_1080P) && (in_w <= WIDTH_1080p)) {
			if ((ratio >= 0x00010000) && (ratio <= 0x00020000))
				calc_rate = 200000000;
			else
				calc_rate = 100000000;
		} else {
			ret = __LINE__;
		}
	} else if ((scn_h <= HIGH_1080P) && (scn_w <= WIDTH_1080p)) {
		if ((in_h <= HIGH_720P) && (in_w <= WIDTH_720p)) {
			if ((ratio >= 0x00010000) && (ratio <= 0x00020000))
				calc_rate = 160000000;
			else
				calc_rate = 150000000;
		} else if ((in_h <= HIGH_1080P) && (in_w <= WIDTH_1080p)) {
			if ((ratio > 0x00010000) && (ratio <= 0x00020000))
				calc_rate = 252000000;
			else
				calc_rate = 200000000;
		} else {
			ret = __LINE__;
		}
	}

	/* return the new rate to caller */
	info->calc_rate = calc_rate;

	return 0;
}

#endif /* __CSP_DPU_SE_C__ */
