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
#include <linux/clk.h>

#include <drm/drmP.h>

#include "lombo_doss.h"

#define LOMBO_DOSS_MAX_PATH 2

enum {
	LOMBO_DOSS_SCLK0 = 0,
	LOMBO_DOSS_SCLK1,
	LOMBO_DOSS_HFEOSC,
	LOMBO_DOSS_SCLK_NUM,
};

struct lombo_doss_top;

struct lombo_doss_sclk {
	struct clk *clk;
	struct clk *parent_clk;
};

struct lombo_doss_clk {
	struct lombo_doss_top *doss_top;
	struct device *dev; /* owner */
	uint32_t path_id;
	doss_clk_id_t clk_id;

	struct lombo_doss_sclk *sclk;

	unsigned long req_rate;
	unsigned long real_rate;
	uint32_t div0;
	uint32_t div1;

	struct kref ref_cnt;
	struct list_head list;
};

struct lombo_doss_top {
	uint32_t id;

	struct clk *ahb_gate;
	struct clk *ahb_reset;

	struct lombo_doss_sclk sclk[LOMBO_DOSS_SCLK_NUM];
	struct list_head clk_list;

	void __iomem *reg;

	/* for doss top access */
	spinlock_t slock;
	int en_cnt; /* host en_cnt */
};

int lombo_doss_route(struct device *doss_top,
	u32 tcon_id, u32 tcon_src)
{
	struct lombo_doss_top *top = dev_get_drvdata(doss_top);

	csp_doss_set_input(top->id, tcon_id, tcon_src);
	return 0;
}
EXPORT_SYMBOL(lombo_doss_route);

struct lombo_doss_clk *lombo_doss_get_clk(
	struct device *dev, struct device *doss_top,
	uint32_t path_id, doss_clk_id_t clk_id)
{
	struct lombo_doss_top *top = dev_get_drvdata(doss_top);
	struct lombo_doss_clk *clk = NULL;
	struct lombo_doss_sclk *sclk;
	uint32_t sclk_id = LOMBO_DOSS_SCLK_NUM;

	if (path_id == 0) {
		switch (clk_id) {
		case DOSS_CLK_TCON:
		case DOSS_CLK_LVDS:
		case DOSS_CLK_MIPI_DSI_REF:
		case DOSS_CLK_TVE:
			sclk_id = LOMBO_DOSS_SCLK0;
			break;
		case DOSS_CLK_MIPI_DSI_CFG:
			sclk_id = LOMBO_DOSS_HFEOSC;
			break;
		default:
			break;
		}
	} else if (path_id == 1) {
		switch (clk_id) {
		case DOSS_CLK_TCON:
		case DOSS_CLK_LVDS:
		case DOSS_CLK_MIPI_DSI_REF:
		case DOSS_CLK_TVE:
			sclk_id = LOMBO_DOSS_SCLK1;
			break;
		case DOSS_CLK_MIPI_DSI_CFG:
			sclk_id = LOMBO_DOSS_HFEOSC;
			break;
		default:
			break;
		}
	}
	if (sclk_id >= LOMBO_DOSS_SCLK_NUM) {
		DRM_ERROR("inval path_id=%d, clk_id=%x, sclk_id=%d\n",
			path_id, clk_id, sclk_id);
		return NULL;
	}

	sclk = &top->sclk[sclk_id];
	if (!sclk->clk) {
		DRM_ERROR("null sclk%d\n", sclk_id);
		return NULL;
	}

	/**
	 * here not use spinlock for clk_list, the reason is:
	 * lombo_dpu_get_clk should be called by moules in driver-probe/bind.
	 * Mouldes are binded in sequence.
	 */
	list_for_each_entry(clk, &top->clk_list, list) {
		if ((clk->path_id == path_id)
			&& (clk->clk_id == clk_id)
			&& (clk->dev == dev)) {
			kref_get(&clk->ref_cnt);
			return clk;
		}
	}

	clk = kzalloc(sizeof(*clk), GFP_KERNEL);
	if (!clk) {
		DRM_ERROR("%d\n", __LINE__);
		return NULL;
	}

	clk->doss_top = top;
	clk->dev = dev;
	clk->path_id = path_id;
	clk->clk_id = clk_id;
	clk->sclk = sclk;
	/* init clk rate equal sclk rate */
	clk->real_rate = clk_get_rate(sclk->clk);
	clk->div1 = clk->div0 = 1;

	kref_init(&clk->ref_cnt);
	list_add_tail(&clk->list, &top->clk_list);

	return clk;
}
EXPORT_SYMBOL(lombo_doss_get_clk);

static void lombo_doss_clk_free(struct kref *kref)
{
	struct lombo_doss_clk *mod_clk =
			container_of(kref, struct lombo_doss_clk, ref_cnt);

	list_del(&mod_clk->list);
	kfree(mod_clk);
}

int lombo_doss_put_clk(struct lombo_doss_clk **p_clk)
{
	struct lombo_doss_clk *clk;

	if (!(p_clk && (*p_clk))) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}

	clk = *p_clk;
	kref_put(&clk->ref_cnt, lombo_doss_clk_free);
	*p_clk = NULL;

	return 0;
}
EXPORT_SYMBOL(lombo_doss_put_clk);

struct clk *lombo_doss_clk_get_parent(struct lombo_doss_clk *child)
{
	if (child)
		return child->sclk->clk;
	else
		return NULL;
}
EXPORT_SYMBOL(lombo_doss_clk_get_parent);

unsigned long lombo_doss_clk_get_rate(struct lombo_doss_clk *clk)
{
	if (clk)
		return clk->real_rate;

	return 0;
}
EXPORT_SYMBOL(lombo_doss_clk_get_rate);

int lombo_doss_clk_set_rate(struct lombo_doss_clk *clk, unsigned long rate)
{
	if (!clk || (rate == 0)) {
		DRM_ERROR("clk=%p, rate=%lu\n", clk, rate);
		return -EINVAL;
	}
	clk->req_rate = rate;
	return 0;
}
EXPORT_SYMBOL(lombo_doss_clk_set_rate);

unsigned int lombo_doss_clk_get_div(struct lombo_doss_clk *clk)
{
	return clk->div0 * clk->div1;
}
EXPORT_SYMBOL(lombo_doss_clk_get_div);

static inline void lombo_csp_doss_init(
	struct lombo_doss_top *top, uint8_t sw)
{
	int en_cnt;

	spin_lock(&top->slock);
	top->en_cnt++;
	en_cnt = top->en_cnt;
	if ((en_cnt == 1) && !sw)
		csp_doss_init(top->id);
	spin_unlock(&top->slock);

	DRM_DEBUG_KMS("%s: en_cnt=%d, sw=%d\n", __func__, en_cnt, sw);
}

static inline void lombo_csp_doss_exit(struct lombo_doss_top *top)
{
	int en_cnt;

	spin_lock(&top->slock);
	top->en_cnt--;
	en_cnt = top->en_cnt;
	if (top->en_cnt == 0)
		csp_doss_exit(top->id);
	spin_unlock(&top->slock);

	DRM_DEBUG_KMS("%s: en_cnt=%d\n", __func__, en_cnt);
}

static int lombo_doss_clks_enable_for_rgb(
	uint32_t clk_num, struct lombo_doss_clk **clks, uint8_t sw)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *tcon_clk;
	unsigned long sclk_rate;
	uint64_t tmp;
	uint32_t rem;

	if (clk_num == 0) {
		DRM_ERROR("invalid clk_num=0\n");
		return -EINVAL;
	}

	tcon_clk = clks[0];
	if (!tcon_clk ||
		(tcon_clk->clk_id != DOSS_CLK_TCON) ||
		!tcon_clk->req_rate) {
		DRM_ERROR("tcon_clk=%p, clk_id=0x%x, req_rate=%lu\n",
			tcon_clk,
			tcon_clk ? tcon_clk->clk_id : 0,
			tcon_clk ? tcon_clk->req_rate : 0);
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	tmp = clk_get_rate(sclk->parent_clk);
	DRM_DEBUG_KMS("tmp=%llu\n", tmp);
	rem = do_div(tmp, tcon_clk->req_rate);
	if ((tmp == 0) || (rem != 0)) {
		sclk_rate = clk_round_rate(
			sclk->parent_clk, tcon_clk->req_rate);
		clk_set_rate(sclk->parent_clk, sclk_rate);
		clk_set_rate(sclk->clk, sclk_rate);
	}
	if (!sw)
		clk_prepare_enable(sclk->clk);
	sclk_rate = clk_get_rate(sclk->clk);
	DRM_DEBUG_KMS("sclk_rate=%lu\n", sclk_rate);

	tmp = sclk_rate;
	rem = do_div(tmp, tcon_clk->req_rate);
	if (rem > (uint32_t)(tcon_clk->req_rate + 1) >> 1)
		tmp++;
	if (tmp == 0)
		tmp++;
	tcon_clk->div0 = (uint32_t)tmp;
	tcon_clk->div1 = 1;
	tmp = sclk_rate;
	do_div(tmp, tcon_clk->div0);
	tcon_clk->real_rate = (unsigned long)tmp;
	DRM_DEBUG_KMS("tcon_clk: req_rate=%lu, real_rate=%lu, div[%u,%u]\n",
		      tcon_clk->req_rate, tcon_clk->real_rate,
		      tcon_clk->div0, tcon_clk->div1);

	lombo_csp_doss_init(top, sw);

	if (sw)
		return 0;

	csp_doss_clk_set(top->id, tcon_clk->path_id, tcon_clk->clk_id,
			DOSS_CLK_SCLK0, tcon_clk->div0 - 1, tcon_clk->div1 - 1);
	csp_doss_clk_enable(top->id, tcon_clk->path_id, DOSS_CLK_TCON);
	csp_doss_path_init(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_enable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);

	return 0;
}

static int lombo_doss_clks_disable_for_rgb(
	uint32_t clk_num, struct lombo_doss_clk **clks)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *tcon_clk;

	if (clk_num == 0) {
		DRM_ERROR("invalid clk_num=0\n");
		return -EINVAL;
	}

	tcon_clk = clks[0];
	if (!tcon_clk ||
		(tcon_clk->clk_id != DOSS_CLK_TCON)) {
		DRM_ERROR("tcon_clk=%p, clk_id=0x%x\n",
			tcon_clk,
			tcon_clk ? tcon_clk->clk_id : 0);
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	csp_doss_path_disable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_exit(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_clk_disable(top->id, tcon_clk->path_id, tcon_clk->clk_id);
	tcon_clk->real_rate = 0;

	lombo_csp_doss_exit(top);

	clk_disable_unprepare(sclk->clk);

	return 0;
}

static int lombo_doss_clks_enable_for_lvds(
	uint32_t clk_num, struct lombo_doss_clk **clks, uint8_t sw)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *clk;
	struct lombo_doss_clk *tcon_clk = NULL;
	struct lombo_doss_clk *lvds_clk = NULL;
	unsigned long sclk_rate, rate;
	uint64_t tmp;
	uint32_t rem, i;
	bool is_dual_link = false;

	for (i = 0; i < clk_num; i++) {
		clk = clks[i];
		if (clk && clk->clk_id == DOSS_CLK_TCON)
			tcon_clk = clk;
		else if (clk && clk->clk_id == DOSS_CLK_LVDS)
			lvds_clk = clk;
	}
	if (!tcon_clk || !tcon_clk->req_rate || !lvds_clk) {
		DRM_ERROR("null clks\n");
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	lvds_clk->req_rate = tcon_clk->req_rate * 7;
	rate = clk_round_rate(sclk->parent_clk, lvds_clk->req_rate);
	clk_set_rate(sclk->parent_clk, rate);
	clk_set_rate(sclk->clk, rate);
	if (!sw)
		clk_prepare_enable(sclk->clk);
	sclk_rate = clk_get_rate(sclk->clk);
	DRM_DEBUG_KMS("lvds rate=%lu, sclk_rate=%lu\n", rate, sclk_rate);

	lvds_clk->real_rate = sclk_rate;

	tmp = sclk_rate;
	rem = do_div(tmp, tcon_clk->req_rate);
	if (rem > (uint32_t)(tcon_clk->req_rate + 1) >> 1)
		tmp++;
	if (tmp == 0)
		tmp++;
	tcon_clk->div0 = (uint32_t)tmp;
	tcon_clk->div1 = 1;
	tmp = sclk_rate;
	do_div(tmp, tcon_clk->div0);
	tcon_clk->real_rate = (unsigned long)tmp;

	DRM_DEBUG_KMS("tcon req_rate=%lu, real_rate=%lu, div[%u,%u]\n",
		      tcon_clk->req_rate, tcon_clk->real_rate,
		      tcon_clk->div0, tcon_clk->div1);

	lombo_csp_doss_init(top, sw);

	if (sw)
		return 0;

	clk = lvds_clk;
	is_dual_link = lombo_tcon_is_lvds_dual_link(clk->dev);
	csp_doss_clk_set(top->id, clk->path_id,
			clk->clk_id, DOSS_CLK_SCLK0,
			clk->div0 - 1, clk->div1 - 1);
	csp_doss_clk_enable(top->id, clk->path_id, DOSS_CLK_LVDS);

	#ifdef CONFIG_ARCH_LOMBO_N9V0
	csp_doss_intf_vref_enable(top->id, 0);
	if (is_dual_link)
		csp_doss_intf_vref_enable(top->id, 1);
	#endif

	csp_doss_path_init(top->id, clk->path_id, DOSS_CTRL_LVDS);
	csp_doss_path_enable(top->id, clk->path_id, DOSS_CTRL_LVDS);
	if (is_dual_link) {
		csp_doss_clk_set(top->id, 1,
				clk->clk_id, DOSS_CLK_SCLK0,
				clk->div0 - 1, clk->div1 - 1);
		csp_doss_clk_enable(top->id, 1, DOSS_CLK_LVDS);
		csp_doss_path_init(top->id, 1, DOSS_CTRL_LVDS);
		csp_doss_path_enable(top->id, 1, DOSS_CTRL_LVDS);
		csp_doss_slave_enable(top->id, 1, DOSS_CTRL_LVDS);
	}

	clk = tcon_clk;
	csp_doss_clk_set(top->id, clk->path_id,
			clk->clk_id, DOSS_CLK_SCLK0,
			clk->div0 - 1, clk->div1 - 1);
	csp_doss_clk_enable(top->id, clk->path_id, DOSS_CLK_TCON);
	csp_doss_path_init(top->id, clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_enable(top->id, clk->path_id, DOSS_CTRL_TCON);

	return 0;
}

static int lombo_doss_clks_disable_for_lvds(
	uint32_t clk_num, struct lombo_doss_clk **clks)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *clk;
	struct lombo_doss_clk *tcon_clk = NULL;
	struct lombo_doss_clk *lvds_clk = NULL;
	uint32_t i;
	bool is_dual_link = false;

	for (i = 0; i < clk_num; i++) {
		clk = clks[i];
		if (clk && clk->clk_id == DOSS_CLK_TCON)
			tcon_clk = clk;
		else if (clk && clk->clk_id == DOSS_CLK_LVDS)
			lvds_clk = clk;
	}
	if (!tcon_clk || !lvds_clk) {
		DRM_ERROR("null clks\n");
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	csp_doss_path_disable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_exit(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_clk_disable(top->id, tcon_clk->path_id, tcon_clk->clk_id);
	tcon_clk->real_rate = 0;

	is_dual_link = lombo_tcon_is_lvds_dual_link(lvds_clk->dev);

	#ifdef CONFIG_ARCH_LOMBO_N9V0
	csp_doss_intf_vref_disable(top->id, 0);
	if (is_dual_link)
		csp_doss_intf_vref_disable(top->id, 1);
	#endif

	csp_doss_path_disable(top->id, lvds_clk->path_id, DOSS_CTRL_LVDS);
	csp_doss_path_exit(top->id, lvds_clk->path_id, DOSS_CTRL_LVDS);
	csp_doss_clk_disable(top->id, lvds_clk->path_id, lvds_clk->clk_id);
	lvds_clk->real_rate = 0;
	if (is_dual_link) {
		csp_doss_slave_disable(top->id, 1, DOSS_CTRL_LVDS);
		csp_doss_path_disable(top->id, 1, DOSS_CTRL_LVDS);
		csp_doss_path_exit(top->id, 1, DOSS_CTRL_LVDS);
		csp_doss_clk_disable(top->id, 1, lvds_clk->clk_id);
	}
	lombo_csp_doss_exit(top);
	clk_disable_unprepare(sclk->clk);

	return 0;
}

static int lombo_doss_clks_enable_for_dsi(
	uint32_t clk_num, struct lombo_doss_clk **clks, uint8_t sw)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *clk;
	struct lombo_doss_clk *tcon_clk = NULL;
	struct lombo_doss_clk *ref_clk = NULL;
	struct lombo_doss_clk *cfg_clk = NULL;
	unsigned long sclk_rate;
	uint64_t tmp;
	uint32_t rem, i;

	for (i = 0; i < clk_num; i++) {
		clk = clks[i];
		if (!clk)
			continue;
		DRM_DEBUG_KMS("clk_id=0x%x, req_rate=%lu\n",
			clk->clk_id, clk->req_rate);
		if (clk->clk_id == DOSS_CLK_TCON)
			tcon_clk = clk;
		else if (clk->clk_id == DOSS_CLK_MIPI_DSI_REF)
			ref_clk = clk;
		else if (clk->clk_id == DOSS_CLK_MIPI_DSI_CFG)
			cfg_clk = clk;
	}
	if (!tcon_clk || !ref_clk || !cfg_clk) {
		DRM_ERROR("null clks, %p,%p,%p\n",
			tcon_clk, ref_clk, cfg_clk);
		return -EINVAL;
	}
	if (!tcon_clk->req_rate || !ref_clk->req_rate) {
		DRM_ERROR("no req_rate,%lu,%lu\n",
			tcon_clk->req_rate, ref_clk->req_rate);
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	if (!sw)
		clk_prepare_enable(cfg_clk->sclk->clk);
	cfg_clk->real_rate = clk_get_rate(cfg_clk->sclk->clk);
	DRM_DEBUG_KMS("cfg_clk: req_rate=%lu, real_rate=%lu, div[%u,%u]\n",
		cfg_clk->req_rate, cfg_clk->real_rate,
		cfg_clk->div0, cfg_clk->div1);

	/* fixme: calc a best sclk_rate */
	if (!sw)
		clk_prepare_enable(sclk->clk);

	if (tcon_clk->req_rate < DOSS_TCON_DCLK_MID)
		clk_set_rate(sclk->parent_clk, DOSS_SCLK_FREQ_MID);

	sclk_rate = clk_get_rate(sclk->clk);
	DRM_DEBUG_KMS("sclk_rate=%lu\n", sclk_rate);

	tmp = sclk_rate;
	rem = do_div(tmp, tcon_clk->req_rate);
	if (rem > (uint32_t)(tcon_clk->req_rate + 1) >> 1)
		tmp++;
	if (tmp == 0)
		tmp++;
	tcon_clk->div0 = (uint32_t)tmp;

	if (tcon_clk->div0 > DOSS_CLK_DIV_MAX0) {
		for (i = 2; i <= DOSS_CLK_DIV_MAX1; i++) {
			uint32_t tmpt = tmp;
			rem = do_div(tmpt, i);
			if ((rem == 0) && (tmpt <= DOSS_CLK_DIV_MAX0)) {
				tcon_clk->div0 = tmpt;
				tcon_clk->div1 = i;
				break;
			}
		}
		if (i > DOSS_CLK_DIV_MAX1)
			DRM_ERROR("tcon_clk req_rate is too small!!!");
	} else {
		tcon_clk->div0 = tmp;
		tcon_clk->div1 = 1;
	}

	tmp = sclk_rate;
	do_div(tmp, tcon_clk->div0 * tcon_clk->div1);
	tcon_clk->real_rate = (unsigned long)tmp;
	DRM_DEBUG_KMS("tcon_clk: req_rate=%lu, real_rate=%lu, div[%u,%u]\n",
		tcon_clk->req_rate, tcon_clk->real_rate,
		tcon_clk->div0, tcon_clk->div1);

	tmp = sclk_rate;
	rem = do_div(tmp, ref_clk->req_rate);
	if (rem > (uint32_t)(ref_clk->req_rate + 1) >> 1)
		tmp++;
	if (tmp == 0)
		tmp++;
	ref_clk->div0 = (uint32_t)tmp;
	ref_clk->div1 = 1;
	tmp = sclk_rate;
	do_div(tmp, ref_clk->div0);
	ref_clk->real_rate = (unsigned long)tmp;
	DRM_DEBUG_KMS("ref_clk: req_rate=%lu, real_rate=%lu, div[%u,%u]\n",
		ref_clk->req_rate, ref_clk->real_rate,
		ref_clk->div0, ref_clk->div1);

	lombo_csp_doss_init(top, sw);

	if (sw)
		return 0;

	csp_doss_clk_enable(top->id, cfg_clk->path_id, DOSS_CLK_MIPI_DSI_CFG);

	csp_doss_clk_set(top->id, ref_clk->path_id, ref_clk->clk_id,
			 DOSS_CLK_SCLK0, ref_clk->div0 - 1, 0);
	csp_doss_clk_enable(top->id, ref_clk->path_id, DOSS_CLK_MIPI_DSI_REF);
	csp_doss_path_init(top->id, ref_clk->path_id, DOSS_CTRL_MIPI_DSI);
	csp_doss_path_enable(top->id, ref_clk->path_id, DOSS_CTRL_MIPI_DSI);

	csp_doss_clk_set(top->id, tcon_clk->path_id, tcon_clk->clk_id,
			DOSS_CLK_SCLK0, tcon_clk->div0 - 1, tcon_clk->div1 - 1);
	csp_doss_clk_enable(top->id, tcon_clk->path_id, DOSS_CLK_TCON);

	#ifdef CONFIG_ARCH_LOMBO_N9V0
	csp_doss_intf_vref_enable(top->id, 0);
	#endif

	csp_doss_path_init(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_enable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);

	return 0;
}

static int lombo_doss_clks_disable_for_dsi(
	uint32_t clk_num, struct lombo_doss_clk **clks)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *clk;
	struct lombo_doss_clk *tcon_clk = NULL;
	struct lombo_doss_clk *ref_clk = NULL;
	struct lombo_doss_clk *cfg_clk = NULL;
	uint32_t i;

	for (i = 0; i < clk_num; i++) {
		clk = clks[i];
		if (clk && clk->clk_id == DOSS_CLK_TCON)
			tcon_clk = clk;
		else if (clk && clk->clk_id == DOSS_CLK_MIPI_DSI_REF)
			ref_clk = clk;
		else if (clk && clk->clk_id == DOSS_CLK_MIPI_DSI_CFG)
			cfg_clk = clk;
	}
	if (!tcon_clk || !ref_clk || !cfg_clk) {
		DRM_ERROR("null clks\n");
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	csp_doss_path_disable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_exit(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_clk_disable(top->id, tcon_clk->path_id, tcon_clk->clk_id);
	tcon_clk->real_rate = 0;

	#ifdef CONFIG_ARCH_LOMBO_N9V0
	csp_doss_intf_vref_disable(top->id, 0);
	#endif

	csp_doss_path_disable(top->id, ref_clk->path_id, DOSS_CTRL_MIPI_DSI);
	csp_doss_path_exit(top->id, ref_clk->path_id, DOSS_CTRL_MIPI_DSI);
	csp_doss_clk_disable(top->id, ref_clk->path_id, ref_clk->clk_id);
	ref_clk->real_rate = 0;

	csp_doss_clk_disable(top->id, cfg_clk->path_id, cfg_clk->clk_id);
	cfg_clk->real_rate = 0;

	lombo_csp_doss_exit(top);

	clk_disable_unprepare(sclk->clk);
	clk_disable_unprepare(cfg_clk->sclk->clk);

	return 0;
}

static int lombo_doss_clks_enable_for_tve(
	uint32_t clk_num, struct lombo_doss_clk **clks, uint8_t sw)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *clk;
	struct lombo_doss_clk *tcon_clk = NULL;
	struct lombo_doss_clk *tve_clk = NULL;
	unsigned long sclk_rate, tmp;
	uint32_t i;

	for (i = 0; i < clk_num; i++) {
		clk = clks[i];
		if (!clk)
			continue;
		DRM_DEBUG_KMS("clk_id=0x%x, req_rate=%lu\n",
			clk->clk_id, clk->req_rate);
		if (clk->clk_id == DOSS_CLK_TCON)
			tcon_clk = clk;
		else if (clk->clk_id == DOSS_CLK_TVE)
			tve_clk = clk;
	}
	if (!tcon_clk || !tve_clk) {
		DRM_ERROR("null clks, %p,%p\n",
			tcon_clk, tve_clk);
		return -EINVAL;
	}
	if (!tcon_clk->req_rate || !tve_clk->req_rate) {
		DRM_ERROR("no req_rate,%lu,%lu\n",
			tcon_clk->req_rate, tve_clk->req_rate);
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	if (!sw)
		clk_prepare_enable(sclk->clk);

	sclk_rate = clk_get_rate(sclk->clk);
	tmp = sclk_rate;
	do_div(tmp, tve_clk->req_rate);
	tcon_clk->div0 = tmp;
	tcon_clk->div1 = TVE_TCON_CLK_RATIO;

	tmp = clk_get_rate(sclk->clk);
	DRM_DEBUG_KMS("sclk_rate=%lu\n", tmp);
	do_div(tmp, tcon_clk->div0);
	tve_clk->real_rate = tmp;
	DRM_DEBUG_KMS("tve real_rate=%lu\n", tmp);
	tmp = tve_clk->real_rate;
	do_div(tmp, TVE_TCON_CLK_RATIO);
	tcon_clk->real_rate = tmp;
	DRM_DEBUG_KMS("tcon real_rate=%lu\n", tmp);
	lombo_csp_doss_init(top, sw);

	DRM_DEBUG_KMS("req_rate, tcon:%lu, tve:%lu\n",
				tcon_clk->req_rate, tve_clk->req_rate);
	DRM_DEBUG_KMS("TVE TCON_div,%lu,%lu,%d,%d\n", tcon_clk->req_rate,
			tcon_clk->real_rate, tcon_clk->div0, tcon_clk->div1);
	DRM_DEBUG_KMS("TVE tve_div,,%lu,%lu,%d,%d\n", tve_clk->req_rate,
			tve_clk->real_rate, tve_clk->div0, tve_clk->div1);

	if (sw)
		return 0;

	csp_doss_clk_set(top->id, tcon_clk->path_id, tcon_clk->clk_id,
			DOSS_CLK_SCLK0, tcon_clk->div0 - 1, tcon_clk->div1 - 1);
	csp_doss_clk_enable(top->id, tcon_clk->path_id, DOSS_CLK_TCON);
	csp_doss_path_init(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_enable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);

	/* tve don't need csp_doss_clk_set() */
	csp_doss_clk_enable(top->id, tve_clk->path_id, DOSS_CLK_TVE);
	csp_doss_path_init(top->id, tve_clk->path_id, DOSS_CTRL_TVE);
	csp_doss_path_enable(top->id, tve_clk->path_id, DOSS_CTRL_TVE);

	return 0;
}

static int lombo_doss_clks_disable_for_tve(
	uint32_t clk_num, struct lombo_doss_clk **clks)
{
	struct lombo_doss_top *top;
	struct lombo_doss_sclk *sclk;
	struct lombo_doss_clk *clk;
	struct lombo_doss_clk *tcon_clk = NULL;
	struct lombo_doss_clk *tve_clk = NULL;
	uint32_t i;

	for (i = 0; i < clk_num; i++) {
		clk = clks[i];
		if (clk && clk->clk_id == DOSS_CLK_TCON)
			tcon_clk = clk;
		else if (clk && clk->clk_id == DOSS_CLK_TVE)
			tve_clk = clk;
	}
	if (!tcon_clk || !tve_clk) {
		DRM_ERROR("null clks\n");
		return -EINVAL;
	}
	top = tcon_clk->doss_top;
	sclk = tcon_clk->sclk;

	csp_doss_path_disable(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_path_exit(top->id, tcon_clk->path_id, DOSS_CTRL_TCON);
	csp_doss_clk_disable(top->id, tcon_clk->path_id, tcon_clk->clk_id);
	tcon_clk->real_rate = 0;

	csp_doss_path_disable(top->id, tve_clk->path_id, DOSS_CTRL_TVE);
	csp_doss_path_exit(top->id, tve_clk->path_id, DOSS_CTRL_TVE);
	csp_doss_clk_disable(top->id, tve_clk->path_id, tve_clk->clk_id);
	tve_clk->real_rate = 0;

	lombo_csp_doss_exit(top);
	clk_disable_unprepare(sclk->clk);

	return 0;
}

/**
 * @sw is true at smooth boot display.
 */
int lombo_doss_clks_enable(uint32_t clk_num, struct lombo_doss_clk **clks,
	vo_dev_if_t dev_if, uint8_t sw)
{
	struct lombo_doss_top *top;
	int ret = 0;

	if (!clks || !clks[0]) {
		DRM_ERROR("clk_num=%d, clk=%p\n", clk_num, clks);
		return -EINVAL;
	}

	DRM_DEBUG_KMS("sw=%d\n", sw);
	top = clks[0]->doss_top;
	if (!top) {
		DRM_ERROR("null doss top pointer\n");
		return -EINVAL;
	}

	if (!sw) {
		clk_prepare_enable(top->ahb_gate);
		clk_prepare_enable(top->ahb_reset);
	}

	switch (dev_if) {
	case VO_DEV_PRGB:
	case VO_DEV_SRGB_RGB:
	case VO_DEV_SRGB_DUMMY_RGB:
	case VO_DEV_SRGB_RGB_DUMMY:
	case VO_DEV_CPU_18BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB888:
	case VO_DEV_CPU_16BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB565:
	case VO_DEV_CPU_9BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB565:
	case VO_DEV_CPU_8BIT_RGB888:
		ret = lombo_doss_clks_enable_for_rgb(clk_num, clks, sw);
		break;
	case VO_DEV_LVDS:
		ret = lombo_doss_clks_enable_for_lvds(clk_num, clks, sw);
		break;
	case VO_DEV_MIPI_DSI_VIDEO:
	case VO_DEV_MIPI_DSI_CMD:
		ret = lombo_doss_clks_enable_for_dsi(clk_num, clks, sw);
		break;
	case VO_DEV_TVE_NTSC:
	case VO_DEV_TVE_PAL:
		ret = lombo_doss_clks_enable_for_tve(clk_num, clks, sw);
		break;
	default:
		DRM_ERROR("dev_if=%u\n", dev_if);
		ret = -EINVAL;
	}

	if (ret && !sw) {
		clk_disable_unprepare(top->ahb_reset);
		clk_disable_unprepare(top->ahb_gate);
	}

	return ret;
}
EXPORT_SYMBOL(lombo_doss_clks_enable);

int lombo_doss_clks_disable(uint32_t clk_num, struct lombo_doss_clk **clks,
	vo_dev_if_t dev_if)
{
	struct lombo_doss_top *top;
	int ret = 0;

	if (!clks || !clks[0]) {
		DRM_ERROR("clk_num=%d, clk=%p\n", clk_num, clks);
		return -EINVAL;
	}

	top = clks[0]->doss_top;
	if (!top) {
		DRM_ERROR("null doss top pointer\n");
		return -EINVAL;
	}

	switch (dev_if) {
	case VO_DEV_PRGB:
	case VO_DEV_SRGB_RGB:
	case VO_DEV_SRGB_DUMMY_RGB:
	case VO_DEV_SRGB_RGB_DUMMY:
	case VO_DEV_CPU_18BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB888:
	case VO_DEV_CPU_16BIT_RGB666:
	case VO_DEV_CPU_16BIT_RGB565:
	case VO_DEV_CPU_9BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB666:
	case VO_DEV_CPU_8BIT_RGB565:
	case VO_DEV_CPU_8BIT_RGB888:
		ret = lombo_doss_clks_disable_for_rgb(clk_num, clks);
		break;
	case VO_DEV_LVDS:
		ret = lombo_doss_clks_disable_for_lvds(clk_num, clks);
		break;
	case VO_DEV_MIPI_DSI_VIDEO:
	case VO_DEV_MIPI_DSI_CMD:
		ret = lombo_doss_clks_disable_for_dsi(clk_num, clks);
		break;
	case VO_DEV_TVE_NTSC:
	case VO_DEV_TVE_PAL:
		ret = lombo_doss_clks_disable_for_tve(clk_num, clks);
		break;
	default:
		DRM_ERROR("dev_if=%u\n", dev_if);
		return -EINVAL;
	}

	if (!ret) {
		clk_disable_unprepare(top->ahb_reset);
		clk_disable_unprepare(top->ahb_gate);
	}

	return ret;
}
EXPORT_SYMBOL(lombo_doss_clks_disable);

static int lombo_doss_bind(struct device *comp,
	struct device *master, void *master_data)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	return 0;
}

static void lombo_doss_unbind(struct device *comp,
	struct device *master, void *master_data)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);
}

static const struct component_ops lombo_doss_ops = {
	.bind	= lombo_doss_bind,
	.unbind	= lombo_doss_unbind,
};

static int lombo_doss_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct lombo_doss_top *doss_top;
	struct resource *res;
	int i;

	doss_top = kzalloc(sizeof(*doss_top), GFP_KERNEL);
	if (!doss_top) {
		dev_err(dev, "Failed to alloc for doss top\n");
		return -ENOMEM;
	}

	/* parse bus clks */
	doss_top->ahb_gate = devm_clk_get(dev, "ahb_gate");
	if (IS_ERR(doss_top->ahb_gate)) {
		dev_err(dev, "failed to get doss ahb_gate\n");
		return -ENODEV;
	}
	doss_top->ahb_reset = devm_clk_get(dev, "ahb_reset");
	if (IS_ERR(doss_top->ahb_reset)) {
		dev_err(dev, "failed to get doss ahb_reset\n");
		return -ENODEV;
	}

	/* parse sclk & psclk */
	for (i = 0; i < LOMBO_DOSS_SCLK_NUM; i++) {
		/* clk name sequence must according enum doss_sclk_id */
		const char *name[][3] = {
			{ "sclk0", "psclk0" },
			{ "sclk1", "psclk1" },
			{ "hfeosc", NULL },
		};
		struct lombo_doss_sclk *sclk;

		sclk = &doss_top->sclk[i];
		sclk->clk = devm_clk_get(dev, name[i][0]);
		if (IS_ERR_OR_NULL(sclk->clk)) {
			sclk->clk = NULL; /* null mean invalid sclk */
			continue;
		}
		if (name[i][1]) {
			sclk->parent_clk = devm_clk_get(dev, name[i][1]);
			if (IS_ERR_OR_NULL(sclk->parent_clk)) {
				dev_err(dev, "failed to get clock %s\n",
					name[i][1]);
				clk_put(sclk->clk);
				sclk->clk = NULL;
				sclk->parent_clk = NULL;
				continue;
			}
			clk_set_parent(sclk->clk, sclk->parent_clk);
		}
	}

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	doss_top->reg = devm_ioremap(dev, res->start,
				resource_size(res));
	if (doss_top->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_doss_set_register_base(doss_top->id, doss_top->reg,
		(u32)resource_size(res));

	INIT_LIST_HEAD(&doss_top->clk_list);
	spin_lock_init(&doss_top->slock);

	dev_set_drvdata(dev, doss_top);

	return component_add(dev, &lombo_doss_ops);
}

static int lombo_doss_remove(struct platform_device *pdev)
{
	struct lombo_doss_top *doss_top = dev_get_drvdata(&pdev->dev);

	component_del(&pdev->dev, &lombo_doss_ops);
	csp_doss_set_register_base(0, NULL, 0);
	dev_set_drvdata(&pdev->dev, NULL);
	kfree(doss_top);

	return 0;
}

static const struct of_device_id lombo_doss_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-doss" },
	{ .compatible = "lombo,lombo-n7v1-doss" },
	{ .compatible = "lombo,lombo-n8v0-doss" },
	{ .compatible = "lombo,lombo-n9v0-doss" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_doss_of_table);

static struct platform_driver lombo_doss_platform_driver = {
	.probe		= lombo_doss_probe,
	.remove		= lombo_doss_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-doss",
		.of_match_table	= lombo_doss_of_table,
	},
};
module_platform_driver(lombo_doss_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DOSS TOP Driver");
MODULE_LICENSE("GPL");
