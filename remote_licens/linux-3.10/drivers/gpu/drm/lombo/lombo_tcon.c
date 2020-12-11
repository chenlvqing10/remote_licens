/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/component.h>
#include <linux/of_platform.h>

#include <drm/drmP.h>

#include "lombo_drv.h"
#include "lombo_dc.h"
#include "lombo_crtc.h"
#include "lombo_doss.h"
#include "lombo_rgb.h"
#include "csp_tcon/csp_doss_tcon.h"

#define CSCRNDS (2048) /* 12bit precision */

#define GAMMA_TBL_OFST		(0)
#define GAMMA_TBL_SIZE		(1024)

#define SYNC_DLY_LINE		(5)

#define LOMBO_TCON_TIMESTAMP_NUM 16

struct lombo_tcon_timestamp {
	ktime_t ts[LOMBO_TCON_TIMESTAMP_NUM]; /* unit: ns */
	int32_t curline[LOMBO_TCON_TIMESTAMP_NUM]; /* unit: line */
	int16_t head;
	int16_t num;
};

struct lombo_tcon {
	struct device *dev;
	struct device *doss_top;
	struct drm_device *drm;
	void __iomem *reg;
	uint32_t id;
	uint32_t dc_id;

	struct lombo_doss_clk *tcon_clk;
	struct lombo_doss_clk *lvds_clk;
	struct drm_encoder *enc;

	struct lombo_crtc *ecrtc;

	struct lombo_rgb *rgb;
	struct lombo_rgb *lvds;
	struct lombo_rgb *cpu;

	struct list_head list;

	tcon_host_t host;
	vo_dev_if_t dev_if;

	/**
	 * @width: output width of active;
	 * @height: output height of active;
	 * @period: frame period in ns;
	 * @ns_per_ht: time of scaning a ht line, in ns.
	 * @vblank_line_num: total line num of vblank.
	 * @sync_dly_line: the line of sync delay.
	 * @sync_line:
	 *      it is safety for sync dbr if curline less than @sync_line.
	 */
	uint32_t width;
	uint32_t height;
	uint64_t period;
	uint64_t ns_per_ht;
	uint32_t ns_per_ht_rem;
	uint32_t vblank_line_num;
	uint32_t sync_dly_line;
	uint32_t sync_line;
	uint32_t sync_max_lines;

	/**
	 * @dbld_skip_user: counter of skip-dbld due to user configing reg.
	 * @dbld_skip_oversync: counter of skip-dbld due to over @sync_line.
	 * @dbld_over_syncdly: counter of over-sync-dly at finishing dbld.
	 * @lack_data_cnt: counter of tcon lacking data to fetch.
	 */
	int irq; /* irq_no */
	int irq_flag;
	uint32_t dbld_skip_user;
	uint32_t dbld_skip_oversync;
	uint32_t dbld_over_syncdly;
	uint32_t lack_data_cnt;
	uint32_t irq_cnt;

	struct lombo_tcon_timestamp ts;

	/**
	 * @reg_cfg_cnt: init-value is 0.
	 *         inc before user config reg; dec after finishing config reg.
	 *         dec before dbld in irq; inc after finishing dbld.
	 *         if @reg_cfg_cnt < 0, @reg_cfg_cnt canot be inc by user.
	 *         if @reg_cfg_cnt > 0, canot dbld in irq.
	 */
	spinlock_t reg_slock;
	int reg_cfg_cnt;

	bool prepared;
	bool enabled;
	bool is_daul_link;
};

struct tcon_if_remap {
	vo_dev_if_t	dev_if;
	tcon_out_if_t	tcon_out_if;
};

uint32_t lombo_tcon_id(struct lombo_tcon *tcon)
{
	return tcon->id;
}
EXPORT_SYMBOL(lombo_tcon_id);

static inline struct lombo_tcon *to_lombo_tcon_by_host(
		tcon_host_t *host)
{
	return container_of(host, struct lombo_tcon, host);
}

bool lombo_tcon_is_lvds_dual_link(struct device *dev)
{
	struct lombo_tcon *tcon = dev_get_drvdata(dev);
	return tcon->is_daul_link;
}
EXPORT_SYMBOL(lombo_tcon_is_lvds_dual_link);

static void lombo_tcon_checkin_timestamp(struct lombo_tcon *tcon,
	ktime_t timestamp, uint32_t curline)
{
	struct lombo_tcon_timestamp *ts = &tcon->ts;
	int id;

	if (ts->num == LOMBO_TCON_TIMESTAMP_NUM) {
		id = ts->head;
		ts->head = (id + 1) % LOMBO_TCON_TIMESTAMP_NUM;
	} else {
		id = ts->head + ts->num;
		ts->num++;
	}
	ts->ts[id] = timestamp;
	ts->curline[id] = curline;
}

static uint64_t lombo_tcon_timestamp_get_period(
	struct lombo_tcon *tcon)
{
	struct lombo_tcon_timestamp *ts = &tcon->ts;
	int num = ts->num - 1;
	int head = (ts->head + 1) % LOMBO_TCON_TIMESTAMP_NUM;

	uint64_t period_total = 0;
	uint64_t min_period = ~0ULL;
	uint64_t max_period = 0;
	int64_t period;
	int cnt = 0;
	int id, id_n, i;

	if (num < 8)
		return 0;

	id = head;
	for (i = 0; i < num - 1; i++) {
		int64_t t1, t0;

		id_n = (id + 1) % LOMBO_TCON_TIMESTAMP_NUM;
		t0 = (int64_t)ktime_to_ns(ts->ts[id]);
		t1 = (int64_t)ktime_to_ns(ts->ts[id_n]);
		period = t1 - t0;
		if (period > 0) {
			cnt++;
			period_total += period;
			if (period > max_period)
				max_period = period;
			if (period < min_period)
				min_period = period;
		}
		id = id_n;
	}

	if (cnt > 2) {
		period_total -= (max_period + min_period);
		do_div(period_total, (cnt - 2));
		return period_total;
	} else
		return 0;
}

static irqreturn_t lombo_tcon_irq_handler(s32 irq, void *arg)
{
	struct lombo_tcon *tcon = arg;
	struct lombo_dc *dc = tcon->ecrtc->dc;
	irqreturn_t ret = IRQ_HANDLED;
	bool irq_query;
	int curline, curline0, curline1;
	int sync_lines = 0;
	ktime_t now;

	if (irq != tcon->irq)
		return IRQ_NONE;

	irq_query = csp_tcon_query_irq(tcon->id, tcon->irq_flag);
	if (!irq_query)
		return IRQ_NONE;

	csp_tcon_irq_clear(tcon->id, tcon->irq_flag);
	csp_tcon_irq_disable(tcon->id, tcon->irq_flag);

	now = ktime_get();
	csp_tcon_get_current_line(tcon->id, &curline);

	spin_lock(&tcon->reg_slock);
	if (tcon->reg_cfg_cnt > 0) {
		spin_unlock(&tcon->reg_slock);
		tcon->dbld_skip_user++;
		goto out;
	}
	tcon->reg_cfg_cnt--;
	spin_unlock(&tcon->reg_slock);

	csp_tcon_get_current_line(tcon->id, &curline0);
	if (curline0 < tcon->sync_line) {
		lombo_dc_commit(dc);
		lombo_crtc_handle_vblank_events(&tcon->ecrtc->crtc);
		csp_tcon_get_current_line(tcon->id, &curline1);
		if (curline1 >= tcon->sync_dly_line)
			tcon->dbld_over_syncdly++;
		sync_lines = curline1 - curline0;
	} else
		tcon->dbld_skip_oversync++;

	spin_lock(&tcon->reg_slock);
	tcon->reg_cfg_cnt++;
	spin_unlock(&tcon->reg_slock);

	if ((sync_lines > 0) && (sync_lines > tcon->sync_max_lines))
		tcon->sync_max_lines = sync_lines;

out:
	drm_handle_vblank(tcon->drm, tcon->id);
	lombo_tcon_checkin_timestamp(tcon, now, curline);
	tcon->irq_cnt++;
	if (csp_tcon_is_lack_data(tcon->id))
		tcon->lack_data_cnt++;
	csp_tcon_irq_enable(tcon->id, tcon->irq_flag);
	return ret;
}

static int lombo_tcon_do_protect_dbreg(
	struct lombo_tcon *tcon, struct lombo_dc *dc)
{
	const int lock_th = 100;
	int lock_cnt = 0;
	int ret = -ETIMEDOUT;
	int reg_cfg_cnt;

	while (lock_cnt < lock_th) {
		unsigned long flags;

		spin_lock_irqsave(&tcon->reg_slock, flags);
		reg_cfg_cnt = tcon->reg_cfg_cnt;
		if (reg_cfg_cnt >= 0) {
			tcon->reg_cfg_cnt++;
			spin_unlock_irqrestore(&tcon->reg_slock, flags);
			reg_cfg_cnt++;
			ret = 0;
			break;
		}
		spin_unlock_irqrestore(&tcon->reg_slock, flags);

		lock_cnt++;
		DRM_INFO("do protect lock_cnt=%d\n", lock_cnt);
		udelay(10);
	}

	if (ret)
		DRM_ERROR("protect,reg_cfg_cnt=%d,lock_cnt=%d\n",
			reg_cfg_cnt, lock_cnt);
	else
		DRM_DEBUG_KMS("protect,reg_cfg_cnt=%d,lock_cnt=%d\n",
			reg_cfg_cnt, lock_cnt);

	return ret;
}

static int lombo_tcon_undo_protect_dbreg(
	struct lombo_tcon *tcon, struct lombo_dc *dc)
{
	unsigned long flags;
	int reg_cfg_cnt;
	int ret = 0;

	spin_lock_irqsave(&tcon->reg_slock, flags);
	reg_cfg_cnt = tcon->reg_cfg_cnt;
	if (reg_cfg_cnt > 0) {
		tcon->reg_cfg_cnt--;
		spin_unlock_irqrestore(&tcon->reg_slock, flags);
		reg_cfg_cnt--;
	} else if (reg_cfg_cnt <= 0) {
		spin_unlock_irqrestore(&tcon->reg_slock, flags);
		ret = -EINVAL;
	}

	if (ret)
		DRM_ERROR("unprotect reg_cfg_cnt=%d\n", reg_cfg_cnt);
	else
		DRM_DEBUG_KMS("unprotect reg_cfg_cnt=%d\n", reg_cfg_cnt);

	return ret;
}

/**
 * protect db-regs of DPU from being updated in irq.
 * @protect: true at requesting to protection,
 *               false at releaseing to protection.
 * return 0 if success, otherwise return negative value.
 */
int lombo_tcon_protect_dbreg(struct lombo_tcon *tcon,
	struct lombo_dc *dc, const bool protect)
{
	if (protect)
		return lombo_tcon_do_protect_dbreg(tcon, dc);
	else
		return lombo_tcon_undo_protect_dbreg(tcon, dc);
}
EXPORT_SYMBOL(lombo_tcon_protect_dbreg);

static int lombo_tcon_match_dev_if(vo_dev_if_t dev_if, tcon_out_if_t *out_if)
{
	const struct tcon_if_remap remap_tbl[] = {
		{ VO_DEV_PRGB,			TCON_PRGB, },
		{ VO_DEV_SRGB_RGB,		TCON_SRGB_RGB, },
		{ VO_DEV_SRGB_DUMMY_RGB,	TCON_SRGB_DUMMY_RGB, },
		{ VO_DEV_SRGB_RGB_DUMMY,	TCON_SRGB_RGB_DUMMY, },
		{ VO_DEV_CPU_18BIT_RGB666,	TCON_CPU_18BIT_RGB666, },
		{ VO_DEV_CPU_16BIT_RGB888,	TCON_CPU_16BIT_RGB888, },
		{ VO_DEV_CPU_16BIT_RGB666,	TCON_CPU_16BIT_RGB888, },
		{ VO_DEV_CPU_16BIT_RGB565,	TCON_CPU_16BIT_RGB565, },
		{ VO_DEV_CPU_9BIT_RGB666,	TCON_CPU_9BIT_RGB666, },
		{ VO_DEV_CPU_8BIT_RGB888,	TCON_CPU_8BIT_RGB888, },
		{ VO_DEV_CPU_8BIT_RGB666,	TCON_CPU_8BIT_RGB888, },
		{ VO_DEV_CPU_8BIT_RGB565,	TCON_CPU_8BIT_RGB565, },
		{ VO_DEV_LVDS,			TCON_PRGB, },
		{ VO_DEV_MIPI_DSI_VIDEO,	TCON_PRGB, },
		{ VO_DEV_MIPI_DSI_CMD,		TCON_CPU_16BIT_RGB888, },
		{ VO_DEV_TVE_NTSC,		TCON_REG_TVE_NTSC, },
		{ VO_DEV_TVE_PAL,		TCON_REG_TVE_PAL,},
	};
	u32 i = 0;

	for (i = 0; i < ARRAY_SIZE(remap_tbl); i++)
		if (dev_if == remap_tbl[i].dev_if) {
			*out_if = remap_tbl[i].tcon_out_if;
			return 0;
		}

	DRM_ERROR("unknown dev if %d\n", dev_if);
	return -1;
}

/**
 * init tcon_host by vo_dev_cfg
 */
static int lombo_tcon_host_config_init(tcon_host_t *host,
	struct lombo_vo_dev_config *vo_dev_cfg)
{
	struct lombo_tcon *tcon = to_lombo_tcon_by_host(host);
	if (lombo_tcon_match_dev_if(
		vo_dev_cfg->dev_if, &host->out_if))
		return -EINVAL;

	if (VO_DEV_LVDS == vo_dev_cfg->dev_if)
		tcon->is_daul_link = vo_dev_cfg->lvds_if.is_dual_link;

	memcpy(&host->timing, &vo_dev_cfg->timing, sizeof(host->timing));
	host->format = vo_dev_cfg->format;

	host->is_rb_swap = vo_dev_cfg->is_rb_swap;
	host->csc_en = vo_dev_cfg->csc_en;
	host->csc = vo_dev_cfg->csc;
	host->frc_en = vo_dev_cfg->frc_en;
	host->frc_mode = vo_dev_cfg->frc_mode;
	host->gamma_en = vo_dev_cfg->gamma_en;
	host->gamma_tbl = vo_dev_cfg->gamma_tbl;

	host->has_te = vo_dev_cfg->has_te;
	host->te_pol = vo_dev_cfg->te_pol;

	host->trig_vbi = vo_dev_cfg->trig_vbi;
	host->tcon_trig_src_mode = vo_dev_cfg->tcon_trig_src_mode;

	return 0;
}

int lombo_tcon_mode_fixup(struct lombo_tcon *tcon,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{
	struct lombo_vo_dev_config *vo_dev_cfg =
		(struct lombo_vo_dev_config *)(adjusted_mode->private);

	if (!vo_dev_cfg) {
		DRM_DEBUG_KMS("null vo_dev_cfg\n");
		return 0;
	}

	lombo_tcon_host_config_init(&tcon->host, vo_dev_cfg);
	tcon->dev_if = vo_dev_cfg->dev_if;

	DRM_DEBUG_KMS("dclk_freq=%u, dev_if=%u\n",
		vo_dev_cfg->timing.dclk_freq, vo_dev_cfg->dev_if);
	lombo_doss_clk_set_rate(tcon->tcon_clk, vo_dev_cfg->timing.dclk_freq);
	if ((vo_dev_cfg->dev_if == VO_DEV_MIPI_DSI_VIDEO)
		|| (vo_dev_cfg->dev_if == VO_DEV_MIPI_DSI_CMD))
		vo_dev_cfg->dsi_if.tcon_host = &tcon->host;

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_mode_fixup);

static tcon_csc_info_t *lombo_tcon_get_csc_info_table(
	int infmt, int outfmt)
{
	/* conv_fmt: R2Y, std: 601f */
	static tcon_csc_info_t r2y_bt601_full = {
		/*
		 *   0.29900,    0.58700,   0.11400,      0,
		 * -0.16874,  -0.33126,    0.50000,  128,
		 *   0.50000,  -0.41869,  -0.08131,  128,
		 */
		/*
		 *  1225,   2404,   467,  0,
		 *  -691,	-1357,  2048,  524288,
		 *  2048,	-1715,  -333,  524288,
		*/
		{ 0x000004C9, 0x00000964, 0x000001D3, 0x00000000 + CSCRNDS,
		0xFFFFFD4D, 0xFFFFFAB3, 0x00000800, 0x00080000 + CSCRNDS,
		0x00000800, 0xFFFFF94D, 0xFFFFFEB3, 0x00080000 + CSCRNDS, },

		/* min clip value */
		{ 0, 0, 0, },
		/* max clip value */
		{ 255, 255, 255, },
	};

	/* conv_fmt: Y2R, std: 601f */
	static tcon_csc_info_t y2r_bt601_full = {
		{
		/*
		 * 1, 0,		1.402,	-179.456,
		 * 1, -0.34414, -0.71414, 135.45984,
		 * 1, 1.772,	0,	-226.816,
		 */
		/*
		 * 4096, 0, 5743, -735052,
		 * 4096,	-1410,	-2925,	554844,
		 * 4096,	7258,	0,	-929038,
		 */
		0x00001000, 0x00000000, 0x0000166F, (0xFFF4C8B4 + CSCRNDS),
		0x00001000, 0xFFFFFA7E, 0xFFFFF493, (0x00087758 + CSCRNDS),
		0x00001000, 0x00001C5A, 0x00000000, (0xFFF1D2F2 + CSCRNDS),
		},
		{
			/* min clip value */
			0, 0, 0,
		},
		{
			/* max clip value */
			255, 255, 255,
		},
	};

	if ((infmt == LOMBO_CSFMT_RGB)
		&& (outfmt == LOMBO_CSFMT_YUV))
		return &r2y_bt601_full;
	else if ((infmt == LOMBO_CSFMT_YUV)
		&& (outfmt == LOMBO_CSFMT_RGB))
		return &y2r_bt601_full;
	return NULL;
}

int lombo_tcon_get_out_csfmt(struct lombo_tcon *tcon)
{
	switch (tcon->host.format) {
	case TCON_FMT_RGB888:
	case TCON_FMT_RGB666:
	case TCON_FMT_RGB565:
	case TCON_FMT_RGB555:
		return LOMBO_CSFMT_RGB;
	case TCON_FMT_YUV422:
	case TCON_FMT_YUV444:
		return LOMBO_CSFMT_YUV;
	default:
		DRM_INFO("[%d]tconhost.format=%u\n",
			__LINE__, tcon->host.format);
		return LOMBO_CSFMT_RGB;
	}
}
EXPORT_SYMBOL(lombo_tcon_get_out_csfmt);

int lombo_tcon_set_in_csfmt(struct lombo_tcon *tcon, int csfmt)
{
	int outfmt = lombo_tcon_get_out_csfmt(tcon);

	if ((csfmt >= LOMBO_CSFMT_NUM)
		|| (outfmt >= LOMBO_CSFMT_NUM)) {
		DRM_DEBUG_KMS("bad fmt{%d,%d}\n", csfmt, outfmt);
		return -EINVAL;
	}

	if (tcon->host.csc_en && tcon->host.csc)
		return -EBUSY;

	if (csfmt == outfmt) {
		tcon->host.csc_en = 0;
		return 0;
	}

	tcon->host.csc = lombo_tcon_get_csc_info_table(csfmt, outfmt);
	if (tcon->host.csc)
		tcon->host.csc_en = 1;
	else
		tcon->host.csc_en = 0;

	return tcon->host.csc_en ? 0 : -ENOTSUPP;
}
EXPORT_SYMBOL(lombo_tcon_set_in_csfmt);

/* setup or close clk */
int lombo_tcon_set_state(struct lombo_tcon *tcon,
	struct drm_encoder *enc, int prepare)
{
	struct lombo_drv *drv = tcon->drm->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct lombo_doss_clk *clks[4];
	uint32_t num = 0;
	uint8_t sw = 0;

	DRM_DEBUG_KMS("tcon prepare %d --> %d\n",
		tcon->prepared, prepare);

	if (tcon->prepared == prepare)
		return 0;
	tcon->prepared = prepare;

	memset(clks, 0, sizeof(clks));

	clks[num] = tcon->tcon_clk;
	num++;
	if (prepare == 1) {
		if (!enc) {
			DRM_ERROR("null encoder\n");
			return -EINVAL;
		}

		tcon->enc = enc;
		if (enc->encoder_type == DRM_MODE_ENCODER_LVDS) {
			clks[num] = tcon->lvds_clk;
			num++;
		} else if (enc->encoder_type == DRM_MODE_ENCODER_DSI) {
			num += lombo_dsi_encoder_get_clk(
				enc, ARRAY_SIZE(clks) - num, &clks[num]);
		} else if (enc->encoder_type == DRM_MODE_ENCODER_TVDAC) {
			num += lombo_tve_encoder_get_clk(
				enc, ARRAY_SIZE(clks) - num, &clks[num]);
		}
		if (bd && (bd->tcon_id == tcon->id))
			sw = 1;
		lombo_doss_clks_enable(num, clks, tcon->dev_if, sw);
		switch (tcon->dc_id) {
		case 0:
			lombo_doss_route(tcon->doss_top,
					 tcon->id,
					 DOSS_IN_DC0);
			break;
		case 1:
			lombo_doss_route(tcon->doss_top,
					 tcon->id,
					 DOSS_IN_DC1);
			break;
		default:
			DRM_ERROR("doss route fail. dc_id=%u\n", tcon->dc_id);
			break;
		}
	} else {
		if (!enc)
			enc = tcon->enc;
		if (!enc) {
			DRM_ERROR("null encoder for dis\n");
			return -EINVAL;
		}
		if (enc->encoder_type == DRM_MODE_ENCODER_LVDS) {
			clks[num] = tcon->lvds_clk;
			num++;
		} else if (enc->encoder_type == DRM_MODE_ENCODER_DSI) {
			num += lombo_dsi_encoder_get_clk(
				enc, ARRAY_SIZE(clks) - num, &clks[num]);
		} else if (enc->encoder_type == DRM_MODE_ENCODER_TVDAC) {
			num += lombo_tve_encoder_get_clk(
				enc, ARRAY_SIZE(clks) - num, &clks[num]);
		}
		lombo_doss_clks_disable(num, clks, tcon->dev_if);
	}

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_set_state);

static int lombo_tcon_calc_timing_info(struct lombo_tcon *tcon)
{
	tcon_host_t *host = &tcon->host;
	tcon_timings_t *timing = &host->timing;
	uint64_t period;
	uint32_t rem;
	const uint64_t sec_in_ns = 1000000000ULL;
	uint32_t sync_dly = 0;
	uint64_t sync_time_ns = 80 * 1000ULL; /* 80 us */
	uint32_t dclk_freq;

	host->tcon_dclk_div = lombo_doss_clk_get_div(tcon->tcon_clk);
	host->tcon_dclk_freq = (u32)lombo_doss_clk_get_rate(tcon->tcon_clk);
	dclk_freq = host->tcon_dclk_freq;

	tcon->width = timing->hline.hact;
	tcon->height = timing->field1.vact + timing->field2.vact;

	if (true == timing->is_interlace)
		period = sec_in_ns * timing->hline.ht * timing->field1.vt;
	else
		period = sec_in_ns * timing->hline.ht
			* (timing->field1.vt + timing->field2.vt);
	do_div(period, dclk_freq);
	tcon->period = period;
	rem = do_div(period, timing->field1.vt + timing->field2.vt);
	tcon->ns_per_ht = period;
	tcon->ns_per_ht_rem = rem;

	tcon->vblank_line_num = timing->field1.vt - timing->field1.vact;

	sync_dly = tcon->vblank_line_num;
	if (sync_dly > SYNC_DLY_LINE)
		sync_dly -= SYNC_DLY_LINE;
	else
		sync_dly >>= 1;
	tcon->sync_dly_line = sync_dly;

	do_div(sync_time_ns, (uint32_t)tcon->ns_per_ht); /* change to in line */
	if ((uint32_t)sync_time_ns >= sync_dly) {
		DRM_INFO("no enough sync line for sync time\n");
		tcon->sync_line = sync_dly >> 1;
	} else
		tcon->sync_line = sync_dly - (uint32_t)sync_time_ns;

	DRM_DEBUG_KMS("W=%d, H=%d, P=%llu, nsPht=%llu.%u\n",
		tcon->width, tcon->height, tcon->period,
		tcon->ns_per_ht, tcon->ns_per_ht_rem);
	DRM_DEBUG_KMS("vbLineNum=%u, syncDlyLine=%u, syncLine=%u\n",
		tcon->vblank_line_num, tcon->sync_dly_line, tcon->sync_line);

	return 0;
}

int lombo_tcon_set_mode(struct lombo_tcon *tcon,
	const struct drm_encoder *enc,
	const struct drm_display_mode *mode,
	struct drm_display_mode *adjusted_mode)
{
	struct lombo_drv *drv = tcon->drm->dev_private;
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct lombo_vo_dev_config *vo_dev_cfg =
		(struct lombo_vo_dev_config *)(adjusted_mode->private);
	tcon_host_t *host = &tcon->host;
	tcon_timings_t *timing = &host->timing;
	tcon_out_if_t out_if = host->out_if;
	tcon_cpu_if_t cpu_if;
	tcon_rgb_if_t rgb_if;
	int ret;

	DRM_DEBUG_KMS("\n");
	if (vo_dev_cfg == NULL) {
		DRM_ERROR("vo_dev_cfg=%p\n", vo_dev_cfg);
		return -EINVAL;
	}

	lombo_tcon_calc_timing_info(tcon);

	if (bd && (bd->tcon_id == tcon->id)) {
		DRM_DEBUG("tcon set mode return for sw\n");
		/* for test */
		/* csp_tcon_set_source(tcon->id, TCON_SRC_CHECKER_BOARD); */
		return 0;
	}

	csp_tcon_init(tcon->id);

	/* Sync Delay setting */
	csp_tcon_set_dc_sync_delay(tcon->id, tcon->sync_dly_line);

	/* TCON timing */
	ret = csp_tcon_set_timing(tcon->id, timing, out_if);
	if (ret) {
		DRM_ERROR("csp_tcon_set_timing failed, ret=%d\n", ret);
		return ret;
	}
	csp_tcon_set_output_interface(tcon->id, out_if);

	/* Device Interface Specific Configuration */
	switch (vo_dev_cfg->dev_if) {
	case VO_DEV_PRGB:
	case VO_DEV_SRGB_RGB:
	case VO_DEV_SRGB_DUMMY_RGB:
	case VO_DEV_SRGB_RGB_DUMMY:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_path(tcon->id, TCON_PATH_LCD);
		csp_tcon_set_rgb_cfg(tcon->id, &vo_dev_cfg->rgb_if);
		break;
	case VO_DEV_CPU_18BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB888:
	case VO_DEV_CPU_16BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB565:
	case VO_DEV_CPU_9BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB565:
	case VO_DEV_CPU_8BIT_RGB888:
	case VO_DEV_CPU_8BIT_RGB666:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_path(tcon->id, TCON_PATH_LCD);
		csp_tcon_set_cpu_cfg(tcon->id, &vo_dev_cfg->cpu_if);
		csp_tcon_mipi_cm_disable(tcon->id);
		break;
	case VO_DEV_LVDS:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_path(tcon->id, TCON_PATH_LCD);
		csp_tcon_set_lvds_cfg(tcon->id, &vo_dev_cfg->lvds_if);
		if (vo_dev_cfg->lvds_if.is_dual_link)
			csp_tcon_set_out_mode(tcon->id, 1);
		else
			csp_tcon_set_out_mode(tcon->id, 0);
		csp_tcon_lvds_enable(tcon->id);
		break;
	case VO_DEV_MIPI_DSI_VIDEO:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_path(tcon->id, TCON_PATH_LCD);
		rgb_if.vsync_pol = TCON_IO_POS;
		rgb_if.hsync_pol = TCON_IO_POS;
		rgb_if.de_pol = TCON_IO_POS;
		rgb_if.clk_pol = TCON_IO_RISING;
		rgb_if.is_clk_ddr = false;
		csp_tcon_set_rgb_cfg(tcon->id, &rgb_if);
		/* TODO */
		break;
	case VO_DEV_MIPI_DSI_CMD:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		cpu_if.has_vsync_cmd = true;
		cpu_if.has_hsync_cmd = true;
		cpu_if.cmd_align = 0;
		csp_tcon_set_cpu_cfg(tcon->id, &cpu_if);
		csp_tcon_set_path(tcon->id, TCON_PATH_LCD);
		csp_tcon_mipi_cm_enable(tcon->id);
		/* TODO */
		break;
	case VO_DEV_BT601_24BIT:
	case VO_DEV_BT601_16BIT:
	case VO_DEV_BT601_8BIT:
	case VO_DEV_BT656_8BIT:
	case VO_DEV_BT1120_16BIT:
	case VO_DEV_BT1120_8BIT:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		/* TODO */
		break;
	case VO_DEV_TVE_NTSC:
	case VO_DEV_TVE_PAL:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_path(tcon->id, TCON_PATH_TV);
		rgb_if.field_pol = TCON_IO_POS;
		rgb_if.vblank_pol = TCON_IO_POS;
		rgb_if.hblank_pol = TCON_IO_POS;
		rgb_if.vsync_pol = TCON_IO_POS;
		rgb_if.hsync_pol = TCON_IO_POS;
		rgb_if.de_pol = TCON_IO_POS;
		rgb_if.clk_pol = TCON_IO_RISING;
		rgb_if.is_clk_ddr = false;
		csp_tcon_set_rgb_cfg(tcon->id, &rgb_if);
		/* TODO */
		break;
	default:
		DRM_ERROR("BAD dev_if=%u\n", vo_dev_cfg->dev_if);
		break;
	}

	/* CSC setting */
	DRM_DEBUG_KMS("csc_en=%d,csc=%p\n", host->csc_en, host->csc);
	if (host->csc_en == true) {
		if (host->csc) {
			csp_tcon_set_csc(tcon->id, host->csc);
			csp_tcon_csc_enable(tcon->id);
		}
	} else {
		csp_tcon_csc_disable(tcon->id);
	}

	/* FRC setting */
	if (host->frc_en == true)
		csp_tcon_frc_enable(tcon->id);
	else
		csp_tcon_frc_disable(tcon->id);

	switch (host->format) {
	case TCON_FMT_RGB666:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_frc(tcon->id, TCON_FRC_666);
		break;
	case TCON_FMT_RGB565:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_frc(tcon->id, TCON_FRC_565);
		break;
	case TCON_FMT_RGB555:
		DRM_DEBUG_KMS("[%d]\n", __LINE__);
		csp_tcon_set_frc(tcon->id, TCON_FRC_555);
		break;
	default:
		DRM_DEBUG_KMS("[%d]tconhost.format=%u\n",
			__LINE__, host->format);
		csp_tcon_frc_disable(tcon->id);
		break;
	}

	/* GAMMA Table */
	if (host->gamma_en == true) {
		csp_tcon_set_gamma_table(tcon->id, host->gamma_tbl,
				GAMMA_TBL_OFST, GAMMA_TBL_SIZE);
		csp_tcon_gamma_enable(tcon->id);
	} else
		csp_tcon_gamma_disable(tcon->id);

	/* FIFO Mode */
	csp_tcon_set_fifo_mode(tcon->id, host->fifo_mode);

	/* Red and Blue Swap */
	if (host->is_rb_swap)
		csp_tcon_rb_swap_enable(tcon->id);
	else
		csp_tcon_rb_swap_disable(tcon->id);

	/* TCON Video Source */
	csp_tcon_set_source(tcon->id, host->tcon_src);
	if (TCON_SRC_DMA == host->tcon_src)
		csp_tcon_dma_enable(tcon->id);
	else
		csp_tcon_dma_disable(tcon->id);
	csp_tcon_set_backcolor(tcon->id, 0x00123456);

	/* Trigger Mode and Trigger Source */
	csp_tcon_set_trigger_src_mode(tcon->id,
			host->tcon_trig_src_mode,
			host->te_pol);

	csp_tcon_enable(tcon->id);

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_set_mode);

int lombo_tcon_commit(struct lombo_tcon *tcon)
{
	DRM_DEBUG_KMS("\n");

	csp_tcon_start(tcon->id);
	csp_tcon_irq_clear(tcon->id, tcon->irq_flag);
	csp_tcon_irq_enable(tcon->id, tcon->irq_flag);

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_commit);

int lombo_tcon_disable(struct lombo_tcon *tcon)
{
	csp_tcon_irq_disable(tcon->id, tcon->irq_flag);
	csp_tcon_irq_clear(tcon->id, tcon->irq_flag);
	csp_tcon_stop(tcon->id);

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_disable);

int lombo_tcon_kms_show(struct lombo_tcon *tcon,
	struct seq_file *m, void *data)
{
	uint64_t fps = 1000000000ULL;
	uint64_t fps_float = 0;
	uint64_t p;
	uint64_t cfps = 1000000000ULL;
	uint64_t cfps_float = 0;

	if (tcon->period) {
		fps_float = do_div(fps, tcon->period);
		fps_float *= 100;
		do_div(fps_float, tcon->period);
	}

	p = lombo_tcon_timestamp_get_period(tcon);
	if (p) {
		cfps_float = do_div(cfps, p);
		cfps_float *= 100;
		do_div(cfps_float, p);
	}

	seq_printf(m, "TCON(%d): %dx%d, dclk=%luHz\n",
		lombo_tcon_id(tcon), tcon->width, tcon->height,
		lombo_doss_clk_get_rate(tcon->tcon_clk));

	seq_printf(m, "|-period=%lluns(%d.%02dfps);PERIOD=%lluns(%d.%02dFPS)\n",
		tcon->period, (uint32_t)fps, (uint32_t)fps_float,
		p, (uint32_t)cfps, (uint32_t)cfps_float);

	seq_printf(m, "|-Line(sync,syncDly,vbiNum)=(%u,%u,%u), nsPerHt=%llu.%uns\n",
		tcon->sync_line, tcon->sync_dly_line, tcon->vblank_line_num,
		tcon->ns_per_ht, tcon->ns_per_ht_rem);

	seq_printf(m, "|-dbld_skip(user,overSync)=(%u,%u), dbld_overSyncdly=%u, lack_data_cnt=%u, syncMaxLine=%u, irq_cnt=%u\n",
		tcon->dbld_skip_user, tcon->dbld_skip_oversync,
		tcon->dbld_over_syncdly, tcon->lack_data_cnt,
		tcon->sync_max_lines, tcon->irq_cnt);

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_kms_show);

int lombo_tcon_set_gamma_state(struct lombo_tcon *tcon,
	bool enable)
{
	if (enable)
		csp_tcon_gamma_enable(tcon->id);
	else
		csp_tcon_gamma_disable(tcon->id);

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_set_gamma_state);

int lombo_tcon_gamma_set(struct lombo_tcon *tcon,
	u16 *r, u16 *g, u16 *b, uint32_t start, uint32_t size)
{
	uint32_t i;
	uint32_t *gamma_tbl; /* argb format, alpha is invald */

	if (size != GAMMA_TBL_SIZE / 4) {
		DRM_ERROR("[%d] illegal gamma siex[%d]\n", __LINE__, size);
		return -EINVAL;
	}

	gamma_tbl = kzalloc(GAMMA_TBL_SIZE, GFP_KERNEL);
	if (!gamma_tbl) {
		DRM_ERROR("[%d] fail to malloc gamma_tbl\n", __LINE__);
		return -ENOMEM;
	}

	/* only need low 8 bits of r, g, b */
	for (i = 0; i < size; i++) {
		gamma_tbl[i] = r[i] & 0x00ff;
		gamma_tbl[i] = (gamma_tbl[i] << 8) | (g[i] & 0x00ff);
		gamma_tbl[i] = (gamma_tbl[i] << 8) | (b[i] & 0x00ff);
		DRM_DEBUG_KMS("%d r/g/b/gamma = 0x%04x/0x%04x/0x%04x/0x%08x\n",
			i, r[i], g[i], b[i], gamma_tbl[i]);
	}

	csp_tcon_set_gamma_table(tcon->id, gamma_tbl,
			GAMMA_TBL_OFST, GAMMA_TBL_SIZE);

	kfree(gamma_tbl);

	return 0;
}
EXPORT_SYMBOL(lombo_tcon_gamma_set);

static int lombo_tcon_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct device_node *np, *remote;
	struct platform_device *pdev, *doss_top_pdev;
	struct drm_device *drm = master_data;
	struct lombo_drv *drv = drm->dev_private;
	struct lombo_dc *dc = NULL, *dc_tmp;
	struct lombo_tcon *tcon;
	struct resource *res;
	uint32_t value;
	uint32_t possible_crtcs;
	int ret;

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	np = dev->of_node;
	pdev = to_platform_device(dev);

	remote = of_parse_phandle(dev->of_node, "top", 0);
	if (!remote) {
		DRM_ERROR("failed to parse phandle of top\n");
		return -EINVAL;
	}
	doss_top_pdev = of_find_device_by_node(remote);
	if (!doss_top_pdev) {
		DRM_ERROR("failed to find device doss top\n");
		return -EINVAL;
	}

	tcon = kzalloc(sizeof(*tcon), GFP_KERNEL);
	if (!tcon) {
		dev_err(dev, "alloc for tcon failed.\n");
		return -ENOMEM;
	}
	tcon->dev = dev;
	tcon->doss_top = &doss_top_pdev->dev;
	tcon->drm = drm;

	/* parse hardware index */
	if (!of_property_read_u32(np, "hw_id", &value))
		tcon->id = value;
	else
		DRM_INFO("get tcon hw_id failed\n");
	DRM_DEBUG_KMS("tcon id=%d\n", tcon->id);

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	tcon->reg = devm_ioremap(dev, res->start,
			resource_size(res));
	if (tcon->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_tcon_set_register_base(tcon->id, (uintptr_t)tcon->reg,
		(u32)resource_size(res));

	/* parse irq */
	tcon->irq_flag = TCON_VBI;
	if (drv->boot_disp && (drv->boot_disp->tcon_id == tcon->id)) {
		csp_tcon_irq_disable(tcon->id, tcon->irq_flag);
		csp_tcon_irq_clear(tcon->id, tcon->irq_flag);
	}
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "get tcon interrupt resource failed.\n");
		return -ENXIO;
	}
	ret = devm_request_irq(dev, res->start, lombo_tcon_irq_handler,
				0, "lombo_tcon", tcon);
	if (ret) {
		dev_err(dev, "request tcon interrupt failed.\n");
		return ret;
	}
	tcon->irq = res->start;

	/* tcon clk */
	tcon->tcon_clk = lombo_doss_get_clk(dev,
				&doss_top_pdev->dev,
				tcon->id, DOSS_CLK_TCON);
	if (!tcon->tcon_clk) {
		DRM_ERROR("lombo_doss_get_clk for tcon fail\n");
		return -EINVAL;
	}

	/* lvds clk */
	tcon->lvds_clk = lombo_doss_get_clk(dev,
				&doss_top_pdev->dev,
				tcon->id, DOSS_CLK_LVDS);
	if (!tcon->lvds_clk) {
		DRM_ERROR("lombo_doss_get_clk for lvds fail\n");
		return -EINVAL;
	}

	spin_lock_init(&tcon->reg_slock);
	tcon->reg_cfg_cnt = 0;

	dev_set_drvdata(dev, tcon);

	/* find suitable dc by parsing default dc index */
	if (!of_property_read_u32(np, "dc_id", &value))
		tcon->dc_id = value;
	DRM_DEBUG_KMS("tcon dc_id=%d\n", tcon->dc_id);
	list_for_each_entry(dc_tmp, &drv->dc_list, list) {
		if (dc_tmp && (dc_tmp->id == tcon->dc_id)) {
			dc = dc_tmp;
			break;
		}
	}
	if (!dc)
		DRM_ERROR("no found dc");

	tcon->ecrtc = lombo_crtc_init(drm, dc, tcon);
	if (IS_ERR(tcon->ecrtc)) {
		dev_err(dev, "create crtc failed.\n");
		return PTR_ERR(tcon->ecrtc);
	}

	/*
	 * we must call this function to init gamma size first
	 * if we want to use drm_mode_gamma_set_ioctl to set gamma
	 */
	drm_mode_crtc_set_gamma_size(&tcon->ecrtc->crtc, GAMMA_TBL_SIZE / 4);

	possible_crtcs = BIT(drm_crtc_index(&tcon->ecrtc->crtc));
	tcon->rgb = lombo_rgb_init(drm,
				tcon, np, "panel_rgb",
				possible_crtcs);
	tcon->lvds = lombo_rgb_init(drm,
				tcon, np, "panel_lvds",
				possible_crtcs);
	tcon->cpu = lombo_rgb_init(drm,
				tcon, np, "panel_cpu",
				possible_crtcs);

	list_add_tail(&tcon->list, &drv->tcon_list);

	return 0;
}

static void lombo_tcon_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_tcon *tcon = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	list_del(&tcon->list);
	lombo_rgb_deinit(tcon->rgb);
	lombo_rgb_deinit(tcon->lvds);
	lombo_rgb_deinit(tcon->cpu);
	lombo_doss_put_clk(&tcon->tcon_clk);
	csp_tcon_set_register_base(tcon->id, (uintptr_t)0, 0);
}

static const struct component_ops lombo_tcon_ops = {
	.bind	= lombo_tcon_bind,
	.unbind	= lombo_tcon_unbind,
};

static int lombo_tcon_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_tcon_ops);
}

static int lombo_tcon_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_tcon_ops);
	return 0;
}

static const struct of_device_id lombo_tcon_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-tcon-0" },
	{ .compatible = "lombo,lombo-n7v1-tcon-0" },
	{ .compatible = "lombo,lombo-n8v0-tcon-0" },
	{ .compatible = "lombo,lombo-n9v0-tcon-0" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_tcon_of_table);

static struct platform_driver lombo_tcon_platform_driver = {
	.probe		= lombo_tcon_probe,
	.remove		= lombo_tcon_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-tcon",
		.of_match_table	= lombo_tcon_of_table,
	},
};
module_platform_driver(lombo_tcon_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DOSS TCON Driver");
MODULE_LICENSE("GPL");
