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
#if defined(__KERNEL__)
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/io.h>
#endif
#include "csp_dpu_rot_reg.h"
#include "csp_dpu_rot.h"

#ifndef __CSP_DPU_ROT_C__
#define __CSP_DPU_ROT_C__

#define WRITEREG32(reg, val) writel((val), (void __iomem *)(reg))
#define READREG32(reg)      readl((void __iomem *)(reg))

#define ROT_END_REG       sizeof(reg_rot_t)

static reg_rot_t *rot;
static unsigned int rot_size;
static void (*__rot_udelay)(unsigned long);

#define rot_assert(condition) do { if (!(condition)) { \
				    ret = __LINE__;    \
				    goto exit;       } \
				} while (0)

/* Internal interface */
static void rot_udelay(unsigned long us)
{
	if (__rot_udelay)
		__rot_udelay(us);
	else {
		/* Assume cpu runs at 1Ghz */
		unsigned long i = 1000 * us;
		while (i--)
			;
	}
}

/* Global control interface */

/**
 *csp_rot_set_delay_func - set udelay func whose delay is in microseconds
 */
int csp_rot_set_delay_func(void (*udelay)(unsigned long))
{
	__rot_udelay = udelay;

	return 0;
}

/* Set register base address */
int csp_rot_set_register_base(unsigned long addr, unsigned int size)
{
	int ret = 0;

	rot_assert(addr != 0);
	rot_assert(size >= ROT_END_REG);

	rot = (reg_rot_t *)addr;
	rot_size = size;

exit:
	return ret;
}

/* Get register base address */
int csp_rot_get_register_base(unsigned long *addr)
{
	int ret = 0;

	rot_assert(addr != NULL);

	*addr = (unsigned long)rot;

exit:
	return ret;
}

int csp_rot_get_ip_ver(unsigned int *major, unsigned int *minor)
{
	reg_rot_ver_t ver;

	ver.val = READREG32((unsigned int *)&rot->ver);

	if (major)
		*major = ver.bits.ver_h;
	if (minor)
		*minor = ver.bits.ver_l;

	return 0;
}

/* Initialize rot */
int csp_rot_init(void)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_ctl_t reg_ctl;
	reg_rot_cfg_t reg_cfg;
	reg_rot_intclr_t reg_intclr;

	/* clear interrupt pending flag */
	reg_addr = (unsigned int *)&rot->intclr;
	reg_intclr.val = 0xFFFFFFFF;
	WRITEREG32(reg_addr, reg_intclr.val);

	/* clear cfg */
	reg_addr = (unsigned int *)&rot->cfg;
	reg_cfg.val = 0;
	WRITEREG32(reg_addr, reg_cfg.val);

	reg_addr = (unsigned int *)&rot->ctl;
	reg_ctl.val = 0;
	reg_ctl.bits.rst = 1;    /* release rot module */
	WRITEREG32(reg_addr, reg_ctl.val);

	return ret;
}

/* Deinitialize rot */
int csp_rot_exit(void)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_ctl_t reg_ctl;
	reg_rot_cfg_t reg_cfg;
	reg_rot_intclr_t reg_intclr;
	reg_rot_inten_t reg_inten;

	/* clear interrupt pending flag */
	reg_addr = (unsigned int *)&rot->intclr;
	reg_intclr.val = 0xFFFFFFFF;
	WRITEREG32(reg_addr, reg_intclr.val);

	/* disable interrupt, irq won't be sent to interrupt controller */
	reg_addr = (unsigned int *)&rot->inten;
	reg_inten.val = 0x0;
	WRITEREG32(reg_addr, reg_inten.val);

	/* clear cfg */
	reg_addr = (unsigned int *)&rot->cfg;
	reg_cfg.val = 0;
	WRITEREG32(reg_addr, reg_cfg.val);

	reg_addr = (unsigned int *)&rot->ctl;
	reg_ctl.val = 0;
	reg_ctl.bits.rst = 0;    /* reset rot module */
	WRITEREG32(reg_addr, reg_ctl.val);

	return ret;
}

/* Reset rot module */
int csp_rot_reset(void)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_ctl_t reg_ctl;

	/* reset module */
	reg_addr = (unsigned int *)&rot->ctl;
	reg_ctl.val = READREG32(reg_addr);
	reg_ctl.bits.rst = 0;    /* reset rot module */
	WRITEREG32(reg_addr, reg_ctl.val);

	rot_udelay(1);

	/* release module */
	reg_ctl.bits.rst = 1;    /* release rot module */
	WRITEREG32(reg_addr, reg_ctl.val);

	return ret;
}

/* Start rot process */
int csp_rot_start(void)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_strmctl_t reg_strmctl;
	reg_rot_ctl_t reg_ctl;

	reg_addr = (unsigned int *)&rot->ctl;
	reg_ctl.val = READREG32(reg_addr);
	reg_ctl.bits.en = 1;     /* enable rot module */
	WRITEREG32(reg_addr, reg_ctl.val);

	/* start a new frame */
	reg_addr = (unsigned int *)&rot->strmctl;
	reg_strmctl.val = READREG32(reg_addr);
	reg_strmctl.bits.start = 1;
	WRITEREG32(reg_addr, reg_strmctl.val);

	return ret;
}

/* Stop rot */
int csp_rot_stop(void)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_ctl_t reg_ctl;
	reg_rot_strmctl_t reg_strmctl;

	reg_addr = (unsigned int *)&rot->strmctl;
	reg_strmctl.val = READREG32(reg_addr);
	reg_strmctl.bits.start = 0; /* clear dma transfering request */

	/* disable dma */
	reg_addr = (unsigned int *)&rot->ctl;
	reg_ctl.val = READREG32(reg_addr);
	/* disable module, this will stop the dma trasfering */
	reg_ctl.bits.en = 0;
	WRITEREG32(reg_addr, reg_ctl.val);

	/* dma disable finish */
	rot_udelay(100);

	return ret;
}

/* Interrupt control interface */

/**
 * csp_rot_enable_irq - enable irq
 * @irqnr: use ROT_IRQ\_ defines
 *
 * Returns 0 for sucess, or line number on error.
 */
int csp_rot_enable_irq(unsigned int irqnr)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_inten_t reg_inten;

	rot_assert(irqnr < 32);

	/* enable irq */
	reg_addr = (unsigned int *)&rot->inten;
	reg_inten.val = READREG32(reg_addr);
	reg_inten.val |= (1u << irqnr);
	WRITEREG32(reg_addr, reg_inten.val);
exit:
	return ret;
}

/**
 * csp_rot_irq_disable - disable irq
 * @irqnr: use ROT_IRQ\_ defines
 *
 * Returns 0 for sucess, or line number on error.
 */
int csp_rot_disable_irq(unsigned int irqnr)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_inten_t reg_inten;

	rot_assert(irqnr < 32);

	/* disable irq */
	reg_addr = (unsigned int *)&rot->inten;
	reg_inten.val = READREG32(reg_addr);
	reg_inten.val &= ~(1u << irqnr);
	WRITEREG32(reg_addr, reg_inten.val);
exit:
	return ret;
}

/**
 * csp_rot_query_irq - query whether irq is triggered
 * @irqnr: use ROT_IRQ\_ defines
 *
 * Returns true when triggered, or false when nothing happened
 */
bool csp_rot_query_irq(unsigned int irqnr)
{
	bool ret = false;
	unsigned int *reg_addr;
	reg_rot_intpd_t reg_intpd;

	if (irqnr > 31)
		return false;

	/* disable irq */
	reg_addr = (unsigned int *)&rot->intpd;
	reg_intpd.val = READREG32(reg_addr);

	ret = (reg_intpd.val & (1u << irqnr)) != 0;

	return ret;
}

/**
 * csp_rot_query_irq - query whether irq is triggered
 * @irqnr: use ROT_IRQ\_ defines
 *
 * Returns true when triggered, or false when nothing happened
 */
int csp_rot_clear_irq(unsigned int irqnr)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_intclr_t reg_intclr;

	rot_assert(irqnr < 32);

	/* clear irq */
	reg_addr = (unsigned int *)&rot->intclr;
	reg_intclr.val = READREG32(reg_addr);
	reg_intclr.val |= (1u << irqnr);
	WRITEREG32(reg_addr, reg_intclr.val);
exit:
	return ret;
}

/**
 * csp_rot_query_irq - query whether irq is triggered
 * @irqnr: use ROT_IRQ\_ defines
 *
 * When irq is trigger, clear it's pending flag and return true.
 *
 * Returns true when triggered, or false when nothing happened
 */
bool csp_rot_query_and_clear_irq(unsigned int irqnr)
{
	bool ret = csp_rot_query_irq(irqnr);

	if (ret)
		csp_rot_clear_irq(irqnr);

	return ret;
}

/* Config control interface */

/**
 * csp_rot_set_cfg_ch - set rot config for specified channel
 * @ch: channel id
 * @cfgs: rot config for channel
 *
 * Returns 0 for sucess, or line number on error.
 */
int csp_rot_set_cfg_ch(unsigned int chn, rot_cfg_ch_t *cfgs)
{
	int ret = 0;
	unsigned int bytespp;
	unsigned int *reg_addr;
	reg_rot_size_t reg_size;
	reg_rot_add_t reg_add;
	reg_rot_inlnstd_t reg_lnstd;
	reg_rot_cfg_t reg_cfg;

	rot_assert(chn < 3);
	rot_assert(cfgs != NULL);

	if (!cfgs->en) {
		reg_addr = (unsigned int *)&rot->cfg;
		reg_cfg.val = READREG32(reg_addr);
		reg_cfg.bits.chsel &= ~(1 << chn);
		WRITEREG32(reg_addr, reg_cfg.val);
		return 0;
	}

	rot_assert(cfgs->bytespp > 0);
	rot_assert(cfgs->bytespp <= 4);

	/* config bytes per pixel */
	reg_addr = (unsigned int *)&rot->cfg;
	reg_cfg.val = READREG32(reg_addr);
	bytespp = (cfgs->bytespp == 4) ? 3 : cfgs->bytespp;
	switch (chn) {
	case 0:
		reg_cfg.bits.bpp0 = bytespp;
		break;
	case 1:
		reg_cfg.bits.bpp1 = bytespp;
		break;
	case 2:
		reg_cfg.bits.bpp2 = bytespp;
		break;
	default:
		break;
	}
	/* select channel to be process */
	reg_cfg.bits.chsel |= cfgs->en << chn;
	WRITEREG32(reg_addr, reg_cfg.val);

	/* config input cfg */
	/* config size */
	reg_addr = (unsigned int *)&rot->insize0;
	reg_addr = (unsigned int *)((ulong)reg_addr
		+ chn * ROT_REG_STEP);
	reg_size.val = 0;
	reg_size.bits.w = cfgs->in_w - 1;
	reg_size.bits.h = cfgs->in_h - 1;
	WRITEREG32(reg_addr, reg_size.val);

	/* config buffer address */
	reg_addr = (unsigned int *)&rot->inadd0;
	reg_addr = (unsigned int *)((ulong)reg_addr
		+ chn * ROT_REG_STEP);
	reg_add.val = 0;
	reg_add.bits.add = (unsigned int)cfgs->in_addr;
	WRITEREG32(reg_addr, reg_add.val);

	/* config buffer line stride */
	reg_addr = (unsigned int *)&rot->inlnstd0;
	reg_addr = (unsigned int *)((ulong)reg_addr
		+ chn * ROT_REG_STEP);
	reg_lnstd.val = 0;
	reg_lnstd.bits.std = cfgs->in_lnstd;
	WRITEREG32(reg_addr, reg_lnstd.val);

	/* config output cfg */
	/* config size */
	reg_addr = (unsigned int *)&rot->outsize0;
	reg_addr = (unsigned int *)((ulong)reg_addr
		+ chn * ROT_REG_STEP);
	reg_size.val = 0;
	reg_size.bits.w = cfgs->out_w - 1;
	reg_size.bits.h = cfgs->out_h - 1;
	WRITEREG32(reg_addr, reg_size.val);

	/* config buffer address */
	reg_addr = (unsigned int *)&rot->outadd0;
	reg_addr = (unsigned int *)((ulong)reg_addr
		+ chn * ROT_REG_STEP);
	reg_add.val = 0;
	reg_add.bits.add = (unsigned int)cfgs->out_addr;
	WRITEREG32(reg_addr, reg_add.val);

	/* config buffer line stride */
	reg_addr = (unsigned int *)&rot->outlnstd0;
	reg_addr = (unsigned int *)((ulong)reg_addr
		+ chn * ROT_REG_STEP);
	reg_lnstd.val = 0;
	reg_lnstd.bits.std = cfgs->out_lnstd;
	WRITEREG32(reg_addr, reg_lnstd.val);

exit:
	return ret;
}

int csp_rot_set_cfg(rot_cfg_t *cfgs)
{
	int ret = 0;
	unsigned int bytespp;
	unsigned int *reg_addr;
	unsigned int chn;
	reg_rot_size_t reg_size;
	reg_rot_add_t reg_add;
	reg_rot_inlnstd_t reg_lnstd;
	reg_rot_cfg_t reg_cfg;

	rot_assert(cfgs != NULL);
	rot_assert(cfgs->infb.planes == cfgs->outfb.planes);
	rot_assert(cfgs->infb.format == cfgs->outfb.format);

	reg_addr = (unsigned int *)&rot->cfg;
	reg_cfg.val = READREG32(reg_addr);
	reg_cfg.bits.chsel = 0;   /* disable all channel */
	WRITEREG32(reg_addr, reg_cfg.val);

	for (chn = 0; chn < cfgs->infb.planes; chn++) {
		/* config bytes per pixel */
		reg_addr = (unsigned int *)&rot->cfg;
		reg_cfg.val = READREG32(reg_addr);
		bytespp = dpu_format_get_bytes_per_pixel(
			cfgs->infb.format, chn);
		bytespp = (bytespp == 4) ? 3 : bytespp;
		switch (chn) {
		case 0:
			reg_cfg.bits.bpp0 = bytespp;
			break;
		case 1:
			reg_cfg.bits.bpp1 = bytespp;
			break;
		case 2:
			reg_cfg.bits.bpp2 = bytespp;
			break;
		default:
			break;
		}
		/* select channel to be process */
		reg_cfg.bits.chsel |= 1 << chn;
		WRITEREG32(reg_addr, reg_cfg.val);

		/* config input cfg */
		/* config size */
		reg_addr = (unsigned int *)&rot->insize0;
		reg_addr = (unsigned int *)((ulong)reg_addr
			+ chn * ROT_REG_STEP);
		reg_size.val = 0;
		reg_size.bits.w = (cfgs->infb.width[chn] == 0)
			? 0 : cfgs->infb.width[chn] - 1;
		reg_size.bits.h = (cfgs->infb.height[chn] == 0)
			? 0 : cfgs->infb.height[chn] - 1;
		WRITEREG32(reg_addr, reg_size.val);

		/* config buffer address */
		reg_addr = (unsigned int *)&rot->inadd0;
		reg_addr = (unsigned int *)((ulong)reg_addr
			+ chn * ROT_REG_STEP);
		reg_add.val = 0;
		reg_add.bits.add = cfgs->infb.addr[chn] >> 3;
		WRITEREG32(reg_addr, reg_add.val);

		/* config buffer line stride */
		reg_addr = (unsigned int *)&rot->inlnstd0;
		reg_addr = (unsigned int *)((ulong)reg_addr
			+ chn * ROT_REG_STEP);
		reg_lnstd.val = 0;
		reg_lnstd.bits.std = cfgs->infb.linestride_bit[chn] >> 3;
		WRITEREG32(reg_addr, reg_lnstd.val);

		/* config output cfg */
		/* config size */
		reg_addr = (unsigned int *)&rot->outsize0;
		reg_addr = (unsigned int *)((ulong)reg_addr
			+ chn * ROT_REG_STEP);
		reg_size.val = 0;
		reg_size.bits.w = (cfgs->outfb.width[chn] == 0)
			? 0 : cfgs->outfb.width[chn] - 1;
		reg_size.bits.h = (cfgs->outfb.height[chn] == 0)
			? 0 : cfgs->outfb.height[chn] - 1;
		WRITEREG32(reg_addr, reg_size.val);

		/* config buffer address */
		reg_addr = (unsigned int *)&rot->outadd0;
		reg_addr = (unsigned int *)((ulong)reg_addr
			+ chn * ROT_REG_STEP);
		reg_add.val = 0;
		reg_add.bits.add = cfgs->outfb.addr[chn] >> 3;
		WRITEREG32(reg_addr, reg_add.val);

		/* config buffer line stride */
		reg_addr = (unsigned int *)&rot->outlnstd0;
		reg_addr = (unsigned int *)((ulong)reg_addr
			+ chn * ROT_REG_STEP);
		reg_lnstd.val = 0;
		reg_lnstd.bits.std = cfgs->outfb.linestride_bit[chn] >> 3;
		WRITEREG32(reg_addr, reg_lnstd.val);
	}

exit:
	return ret;
}

/**
 * csp_rot_set_mode - set rot process mode
 * @mode: rot process mode, use ROT_MODE\_ defines
 *
 * Returns 0 for sucess, or line number on error.
 */
int csp_rot_set_mode(unsigned int mode)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_cfg_t reg_cfg;

	rot_assert(mode < ROT_MODE_MAX);

	/* config rot mode */
	reg_addr = (unsigned int *)&rot->cfg;
	reg_cfg.val = READREG32(reg_addr);
	reg_cfg.bits.mode = mode;
	WRITEREG32(reg_addr, reg_cfg.val);

exit:
	return ret;
}

/* Debug interface */

/**
 * csp_rot_get_status - Get rot status
 * @status: stored rot status, 0: normal; 1: busy; other: error
 */
int csp_rot_get_status(rot_status_t *status)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_sta_t reg_sta;
	reg_rot_intpd_t reg_intpd;
	reg_rot_dmasctl0_t reg_dmasctl;
	reg_rot_dmadctl0_t reg_dmadctl;

	rot_assert(status != NULL);

	/* check whether there are something error */
	reg_addr = (unsigned int *)&rot->intpd;
	reg_intpd.val = READREG32(reg_addr);
	status->indma_error = reg_intpd.bits.srcdecerr;
	status->outdma_error = reg_intpd.bits.dstdecerr;
	status->indma_done = reg_intpd.bits.srctran;
	status->outdma_done = reg_intpd.bits.dsttran;

	/* check whether dma is transfering */
	reg_addr = (unsigned int *)&rot->sta;
	reg_sta.val = READREG32(reg_addr);
	status->indma_busy = reg_sta.bits.insta;
	status->outdma_busy = reg_sta.bits.outsta;

	/* check whether dma'shadow parameter status */
	reg_addr = (unsigned int *)&rot->dmasctl0;
	reg_dmasctl.val = READREG32(reg_addr);
	status->indma_shadow_valid = reg_dmasctl.bits.shadowvalid;
	status->indma_shadow_last = reg_dmasctl.bits.shadowlast;

	reg_addr = (unsigned int *)&rot->dmadctl0;
	reg_dmadctl.val = READREG32(reg_addr);
	status->outdma_shadow_valid = reg_dmadctl.bits.shadowvalid;
	status->outdma_shadow_last = reg_dmadctl.bits.shadowlast;

	if ((status->indma_busy == 0) &&
	    (status->outdma_busy == 0) &&
	    (status->indma_error == 0) &&
	    (status->outdma_error == 0))
		status->status = 0;
	else
		status->status = 1;
exit:
	return ret;
}

/**
 * csp_rot_get_time - Get the elapse time of the last process
 * @freq: clock frequency of timer in hz
 * @time: in milliseconds(ms)
 */
int csp_rot_get_time(unsigned int freq, unsigned int *ms)
{
	const unsigned int REG_DBG0_ADDR = 0xF0;
	int ret = 0;
	unsigned int cycles;
	unsigned long long time;
	unsigned int *reg_addr;
	reg_rot_dbg0_t reg_dbg0;

	rot_assert(freq != 0);
	rot_assert(ms != NULL);

	/* the the elapse time */
	reg_addr = (uint32_t *)((char *)rot + REG_DBG0_ADDR);
	reg_dbg0.val = READREG32(reg_addr);
	cycles = reg_dbg0.bits.tmr; /* in 1/freq unit */

	time = cycles;            /* cycles in 1/freq unit */
	time = time * 1000 * 1000;
	do_div(time, freq);       /* in microseconds(us) */
	do_div(time, 1000);       /* in millisecondsms */

	*ms = (unsigned int)time;

exit:
	return ret;
}

/**
 * csp_rot_set_input_dma - fine tuning intput dma config
 * @dma_cfgs: dma config
 */
int csp_rot_set_input_dma(rot_dma_cfgs_t *dma_cfgs)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_dmasctl0_t reg_dmasctl;
	reg_rot_dmacfg0_t reg_dmacfg;

	rot_assert(dma_cfgs != NULL);

	/* set dma control */
	reg_addr = (unsigned int *)&rot->dmasctl0;
	reg_dmasctl.val = READREG32(reg_addr);
	reg_dmasctl.bits.bstsize = dma_cfgs->burst_size;
	reg_dmasctl.bits.bstlen = dma_cfgs->burst_len;
	reg_dmasctl.bits.fixbstlen = (dma_cfgs->burst_len_valid == 1) ? 0 : 1;
	WRITEREG32(reg_addr, reg_dmasctl.val);

	/* set dma config */
	reg_addr = (unsigned int *)&rot->dmacfg0;
	reg_dmacfg.val = READREG32(reg_addr);
	reg_dmacfg.bits.srcosrlmt = dma_cfgs->osrlmt;
	reg_dmacfg.bits.srctrtype = dma_cfgs->transfer_type;
	WRITEREG32(reg_addr, reg_dmacfg.val);

exit:
	return ret;
}

/**
 * csp_rot_set_output_dma - fine tuning outtput dma config
 * @dma_cfgs: dma config
 */
int csp_rot_set_output_dma(rot_dma_cfgs_t *dma_cfgs)
{
	int ret = 0;
	unsigned int *reg_addr;
	reg_rot_dmadctl0_t reg_dmadctl;
	reg_rot_dmacfg0_t reg_dmacfg;

	rot_assert(dma_cfgs != NULL);

	/* set dma control */
	reg_addr = (unsigned int *)&rot->dmadctl0;
	reg_dmadctl.val = READREG32(reg_addr);
	reg_dmadctl.bits.bstsize = dma_cfgs->burst_size;
	reg_dmadctl.bits.bstlen = dma_cfgs->burst_len;
	reg_dmadctl.bits.fixbstlen = (dma_cfgs->burst_len_valid == 1) ? 0 : 1;
	WRITEREG32(reg_addr, reg_dmadctl.val);

	/* set dma config */
	reg_addr = (unsigned int *)&rot->dmacfg0;
	reg_dmacfg.val = READREG32(reg_addr);
	reg_dmacfg.bits.dstosrlmt = dma_cfgs->osrlmt;
	reg_dmacfg.bits.dsttrtype = dma_cfgs->transfer_type;
	WRITEREG32(reg_addr, reg_dmacfg.val);

exit:
	return ret;
}

#endif /* __CSP_DPU_ROT_C__ */
