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
#include <linux/clk-provider.h>

#include <drm/drmP.h>

#include "lombo_dpu.h"
#include "csp_dpu_se.h"

struct lombo_dpu_top;
struct lombo_dpu_clk;

/**
 * @ext_sclk: dpu_sck(x)_ext. this sclk is invalid if @clk is null.
 * @ext_psclk: parent clk of @ext_sclk.
 * @rate: request rate.
 * @real_rate: real rate of this sclk.
 * @sel: select DPU_SCLK_PARENT_EXT or DPU_SCLK_PARENT_ADPLL.
 * @adpll_cfg: configure of adpll.
 * @slock: lock for enable/disable.
 * @en_cnt: enable count.
 */
struct lombo_dpu_sclk {
	struct clk *ext_sclk;
	struct clk *ext_psclk;
	unsigned long rate;
	unsigned long real_rate;
	uint32_t sel;
	struct dpu_adpll_config adpll_cfg;

	spinlock_t slock;
	int en_cnt;
};

/**
 * @mod_id: see enum dpu_mod.
 * @sclk_id: source clock id of module clock.
 * @req_rate: request rate.
 * @real_rate: the actual rate after enabled.
 * @div: div factor.
 * @ref_cnt: referent count.
 * @list: list node to dpu_clk_list.
 * @slock: lock for enable/disable.
 * @en_cnt: enable count.
 */
struct lombo_dpu_clk {
	struct lombo_dpu_top *dpu_top;
	dpu_mod mod_id;
	uint32_t sclk_id;
	unsigned long req_rate;
	unsigned long real_rate;
	u32 div;

	struct kref ref_cnt;
	struct list_head list;

	spinlock_t slock;
	int en_cnt; /* warn if > 1 */
};

struct lombo_dpu_top {
	void __iomem *reg;

	struct clk *ahb_gate;
	struct clk *ahb_reset;
	struct clk *mem_axi_gate;
	struct clk *ext_sclk[LOMBO_DPU_EXT_SCLK_NUM];
	struct clk *ext_psclk[LOMBO_DPU_EXT_SCLK_NUM];

	struct lombo_dpu_sclk sclk[LOMBO_DPU_SCLK_NUM];
	struct list_head clk_list; /* list of module clks */

	/* for dpu top access */
	spinlock_t slock;
	int en_cnt; /* en_cnt of modules */
};

static void lombo_dpu_udelay(unsigned long us)
{
	udelay(us);
}

struct lombo_dpu_clk *lombo_dpu_get_enable_tck(struct device *dpu_top,
	uint32_t req_rate, uint32_t *real_rate)
{
	struct lombo_dpu_clk *clk =
		lombo_dpu_get_clk(NULL, dpu_top, DPU_MOD_TIMER);
	int ret;

	if (!clk)
		return NULL;

	if (req_rate)
		lombo_dpu_clk_set_rate(clk, req_rate);

	ret = lombo_dpu_clk_prepare_enable(clk, 0);
	if (ret) {
		lombo_dpu_put_clk(&clk);
		return NULL;
	}
	if (real_rate)
		*real_rate = (uint32_t)lombo_dpu_clk_get_real_rate(clk);

	return clk;
}

struct lombo_dpu_clk *lombo_dpu_get_clk(struct device *dev,
	struct device *dpu_top,
	uint32_t mod_id)
{
	struct lombo_dpu_top *top = dev_get_drvdata(dpu_top);
	struct lombo_dpu_clk *mod_clk = NULL;

	if (mod_id > DPU_MOD_TOP) {
		DRM_ERROR("%d: mod_id=%d\n", __LINE__, mod_id);
		return NULL;
	}
	if (!top) {
		if (dev)
			dev_err(dev, "dpu top dev is null\n");
		else
			DRM_ERROR("dev and dpu top dev is null\n");
		return NULL;
	}

	/**
	 * here not use spinlock for clk_list, the reason is:
	 * lombo_dpu_get_clk should be called by moules in driver-probe/bind.
	 * Mouldes are binded in sequence.
	 */
	list_for_each_entry(mod_clk, &top->clk_list, list) {
		if (mod_clk->mod_id == mod_id) {
			kref_get(&mod_clk->ref_cnt);
			return mod_clk;
		}
	}

	mod_clk = kzalloc(sizeof(*mod_clk), GFP_KERNEL);
	if (!mod_clk) {
		DRM_ERROR("%d\n", __LINE__);
		return NULL;
	}

	mod_clk->dpu_top = top;
	mod_clk->mod_id = mod_id;
	mod_clk->sclk_id = (mod_id != DPU_MOD_TIMER) ? LOMBO_DPU_SCLK_NUM : 2;
	kref_init(&mod_clk->ref_cnt);
	spin_lock_init(&mod_clk->slock);
	list_add_tail(&mod_clk->list, &top->clk_list);

	return mod_clk;
}
EXPORT_SYMBOL(lombo_dpu_get_clk);

static void lombo_dpu_clk_free(struct kref *kref)
{
	struct lombo_dpu_clk *mod_clk =
		container_of(kref, struct lombo_dpu_clk, ref_cnt);

	list_del(&mod_clk->list);
	if (mod_clk->en_cnt > 0)
		DRM_ERROR("dpu_mod=%d not disabled\n", mod_clk->mod_id);

	kfree(mod_clk);
}

int lombo_dpu_put_clk(struct lombo_dpu_clk **clk)
{
	struct lombo_dpu_clk *mod_clk;

	if (!(clk && (*clk))) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}

	mod_clk = *clk;
	kref_put(&mod_clk->ref_cnt, lombo_dpu_clk_free);
	*clk = NULL;

	return 0;
}
EXPORT_SYMBOL(lombo_dpu_put_clk);

uint32_t lombo_dpu_clk_get_parent(
	struct lombo_dpu_clk *child)
{
	if (child)
		return child->sclk_id;
	else
		return LOMBO_DPU_SCLK_NUM;
}
EXPORT_SYMBOL(lombo_dpu_clk_get_parent);

int lombo_dpu_clk_set_parent(
	struct lombo_dpu_clk *child, uint32_t sclk_id)
{
	if (child) {
		child->sclk_id = sclk_id;
		DRM_DEBUG_KMS("child=%d,sclk_id=%d\n", child->mod_id, sclk_id);
	}

	return 0;
}
EXPORT_SYMBOL(lombo_dpu_clk_set_parent);

unsigned long lombo_dpu_clk_get_rate(struct lombo_dpu_clk *clk, int which)
{
	if (!clk)
		return 0;

	switch (which) {
	case LOMBO_DPU_CLK_REQ_RATE:
		return clk->req_rate;
	case LOMBO_DPU_CLK_REAL_RATE:
		return clk->real_rate;
	case LOMBO_DPU_CLK_PARENT_RATE:
		if (clk->sclk_id < LOMBO_DPU_SCLK_NUM) {
			struct lombo_dpu_top *top = clk->dpu_top;
			struct lombo_dpu_sclk *sclk = top->sclk;

			sclk += clk->sclk_id;
			return sclk->real_rate;
		}
		return 0;
	default:
		return 0;
	}
}
EXPORT_SYMBOL(lombo_dpu_clk_get_rate);

static int lombo_dpu_clk_set_rate_do_sync(struct lombo_dpu_clk *clk,
	unsigned long rate)
{
	struct lombo_dpu_top *top;
	struct lombo_dpu_sclk *sclk;
	unsigned long sclk_rate;
	int en_cnt;
	uint64_t tmp;

	if (!clk || (clk->sclk_id >= LOMBO_DPU_SCLK_NUM)) {
		DRM_ERROR("clk%p or invalid sclk_id%d\n", clk,
			clk ? clk->sclk_id : 0);
		return -EINVAL;
	}
	if ((clk->mod_id < DPU_MOD_SE0) ||
		(clk->mod_id >= DPU_MOD_SE0 + SE_DEV_COUNT)) {
		/* only for se at present */
		return 0;
	}

	spin_lock(&clk->slock);
	en_cnt = clk->en_cnt;
	if ((en_cnt > 0) && (rate != clk->req_rate)) {
		top = clk->dpu_top;
		sclk = &top->sclk[clk->sclk_id];
		sclk_rate = sclk->real_rate;

		/* calc div */
		if (rate > 0) {
			tmp = sclk_rate;
			do_div(tmp, rate);
			if (tmp == 0)
				tmp++;
			clk->div = (uint32_t)tmp;
		} else {
			clk->div = 1;
		}

		spin_lock(&top->slock);
		csp_dpu_clk_disable(clk->mod_id);
		csp_dpu_clk_set_divider(clk->mod_id, clk->div);
		csp_dpu_clk_enable(clk->mod_id);
		spin_unlock(&top->slock);

		tmp = sclk_rate;
		do_div(tmp, clk->div);
		clk->real_rate = (unsigned long)tmp;
	}
	clk->req_rate = rate;
	spin_unlock(&clk->slock);

	WARN_ON(en_cnt <= 0);
	return 0;
}

int lombo_dpu_clk_set_rate_g(struct lombo_dpu_clk *clk,
	unsigned long rate, int sync)
{
	if (!clk)
		return -EINVAL;

	if (!sync) {
		clk->req_rate = rate;
		return 0;
	}

	return lombo_dpu_clk_set_rate_do_sync(clk, rate);
}
EXPORT_SYMBOL(lombo_dpu_clk_set_rate_g);

static inline int lombo_dpu_top_clk_assert(
	struct lombo_dpu_top *top, uint8_t sw)
{
	spin_lock(&top->slock);
	top->en_cnt++;
	if (!sw && (top->en_cnt == 1))
		csp_dpu_clk_assert(DPU_MOD_TOP);
	spin_unlock(&top->slock);

	return 0;
}

static inline int lombo_dpu_top_clk_deassert(
	struct lombo_dpu_top *top, uint8_t sw)
{
	spin_lock(&top->slock);
	top->en_cnt--;
	if (!sw && (top->en_cnt == 0))
		csp_dpu_clk_deassert(DPU_MOD_TOP);
	spin_unlock(&top->slock);

	return 0;
}

static int lombo_dpu_enable_sclk(
	struct lombo_dpu_top *top, struct lombo_dpu_clk *clk, uint8_t sw)
{
	int sclk_id = clk->sclk_id;
	struct lombo_dpu_sclk *sclk = &top->sclk[sclk_id];
	unsigned long ext_rate;
	int en_cnt;
	int ret = 0;

	if (!sclk->ext_sclk) {
		DRM_ERROR("clk%d: invalid sclk%d\n",
			clk->mod_id, clk->sclk_id);
		return -EINVAL;
	}

	if (!sw) {
		clk_prepare_enable(top->ahb_gate);
		clk_prepare_enable(top->ahb_reset);
		clk_prepare_enable(top->mem_axi_gate);
	}

	if ((sclk->sel == DPU_SCLK_PARENT_EXT) && (sclk->rate > 0))
		clk_set_rate(sclk->ext_sclk, sclk->rate);
	if (!sw)
		clk_prepare_enable(sclk->ext_sclk);
	ext_rate = clk_get_rate(sclk->ext_sclk);

	lombo_dpu_top_clk_assert(top, sw);

	spin_lock(&sclk->slock);
	sclk->en_cnt++;
	en_cnt = sclk->en_cnt;
	if (en_cnt > 1) {
		goto out;
	} else if (en_cnt < 1) {
		ret = -EINVAL;
		goto out;
	}

	if (sclk->sel == DPU_SCLK_PARENT_ADPLL) {
		uint64_t tmp;
		struct dpu_adpll_config *cfg = &(sclk->adpll_cfg);

		tmp = sclk->rate;
		do_div(tmp, ext_rate);
		cfg->n = (u32)tmp;
		if (!sw)
			ret = csp_dpu_adpll_enable(sclk_id, cfg);
		if (!ret) {
			sclk->real_rate = ext_rate * cfg->n;
		} else {
			ret = -ETIMEDOUT;
			goto out;
		}

		csp_dpu_set_sclk_parent(sclk_id, DPU_SCLK_PARENT_ADPLL);
	} else {
		sclk->real_rate = ext_rate;
	}

out:
	spin_unlock(&sclk->slock);

	if (ret) {
		lombo_dpu_top_clk_deassert(top, sw);
		if (!sw) {
			clk_disable_unprepare(sclk->ext_sclk);
			clk_disable_unprepare(top->mem_axi_gate);
			clk_disable_unprepare(top->ahb_reset);
			clk_disable_unprepare(top->ahb_gate);
		}
		DRM_ERROR("ret=%d\n", ret);
		return ret;
	}

	DRM_DEBUG("mod_id[%d];sclk[%d]:real_rate=%lu. en_cnt=%d\n",
		clk->mod_id, clk->sclk_id, sclk->real_rate, en_cnt);

	return 0;
}

static int lombo_dpu_disable_sclk(
	struct lombo_dpu_top *top, struct lombo_dpu_clk *clk)
{
	int sclk_id = clk->sclk_id;
	struct lombo_dpu_sclk *sclk = &top->sclk[sclk_id];
	int en_cnt;
	int ret = 0;

	if (!sclk->ext_sclk) {
		DRM_ERROR("clk%d: invalid sclk%d\n",
			clk->mod_id, clk->sclk_id);
		return -EINVAL;
	}

	spin_lock(&sclk->slock);
	sclk->en_cnt--;
	en_cnt = sclk->en_cnt;
	if (en_cnt > 0) {
		goto out;
	} else if (en_cnt < 0) {
		ret = -EINVAL;
		goto out;
	}

	if (sclk->sel == DPU_SCLK_PARENT_ADPLL) {
		csp_dpu_adpll_disable(sclk_id);
		csp_dpu_set_sclk_parent(sclk_id, DPU_SCLK_PARENT_EXT);
	}
	sclk->real_rate = 0;

out:
	spin_unlock(&sclk->slock);

	lombo_dpu_top_clk_deassert(top, 0);
	clk_disable_unprepare(sclk->ext_sclk);
	clk_disable_unprepare(top->mem_axi_gate);
	clk_disable_unprepare(top->ahb_reset);
	clk_disable_unprepare(top->ahb_gate);

	DRM_DEBUG("mod_id[%d];sclk[%d]:real_rate=%lu. en_cnt=%d\n",
		clk->mod_id, clk->sclk_id, sclk->real_rate, en_cnt);

	return ret;
}

static int lombo_dpu_enable_mod_clk(
	struct lombo_dpu_top *top, struct lombo_dpu_clk *clk, uint8_t sw)
{
	struct lombo_dpu_sclk *sclk = &top->sclk[clk->sclk_id];
	const unsigned long sclk_rate = sclk->real_rate;
	int en_cnt;
	uint64_t tmp;

	spin_lock(&clk->slock);
	clk->en_cnt++;
	en_cnt = clk->en_cnt;
	if (en_cnt == 1) {
		/* calc div */
		if (clk->req_rate > 0) {
			tmp = sclk_rate;
			if (clk->mod_id == DPU_MOD_TIMER)
				do_div(tmp, 1024);
			do_div(tmp, clk->req_rate);
			if (tmp == 0)
				tmp++;
			clk->div = (uint32_t)tmp;
		} else {
			clk->div = 1;
		}

		if (!sw) {
			spin_lock(&top->slock);
			csp_dpu_clk_assert(clk->mod_id);
			csp_dpu_clk_set_parent(clk->mod_id, clk->sclk_id);
			csp_dpu_clk_set_divider(clk->mod_id, clk->div);
			csp_dpu_clk_enable(clk->mod_id);
			spin_unlock(&top->slock);
		}

		tmp = sclk_rate;
		if (clk->mod_id == DPU_MOD_TIMER)
			do_div(tmp, clk->div * 1024);
		else
			do_div(tmp, clk->div);
		clk->real_rate = (unsigned long)tmp;
	}
	spin_unlock(&clk->slock);

	DRM_DEBUG("mod_id[%d],rate:req=%lu,real=%lu,div=%u, en_cnt=%d\n",
		clk->mod_id, clk->req_rate, clk->real_rate, clk->div, en_cnt);

	return 0;
}

static int lombo_dpu_disable_mod_clk(
	struct lombo_dpu_top *top, struct lombo_dpu_clk *clk)
{
	int en_cnt;

	spin_lock(&clk->slock);
	clk->en_cnt--;
	en_cnt = clk->en_cnt;
	if (en_cnt == 0) {
		spin_lock(&top->slock);
		csp_dpu_clk_disable(clk->mod_id);
		csp_dpu_clk_deassert(clk->mod_id);
		spin_unlock(&top->slock);
		clk->real_rate = 0;
	}
	spin_unlock(&clk->slock);

	DRM_DEBUG("mod_id[%d], en_cnt=%d\n", clk->mod_id, en_cnt);

	return 0;
}

int lombo_dpu_clk_prepare_enable(struct lombo_dpu_clk *clk, uint8_t sw)
{
	struct lombo_dpu_top *top;
	int ret;

	if (!clk || (clk->sclk_id >= LOMBO_DPU_SCLK_NUM)) {
		DRM_ERROR("clk%d or invalid sclk_id%d\n",
			clk ? clk->mod_id : DPU_MOD_SUB_MAX,
			clk ? clk->sclk_id : (LOMBO_DPU_SCLK_NUM + 1));
		return -EINVAL;
	}

	DRM_DEBUG_KMS("clk=%d,sw=%d\n", clk->mod_id, sw);
	top = clk->dpu_top;
	ret = lombo_dpu_enable_sclk(top, clk, sw);
	if (!ret)
		ret = lombo_dpu_enable_mod_clk(top, clk, sw);

	return ret;
}
EXPORT_SYMBOL(lombo_dpu_clk_prepare_enable);

int lombo_dpu_clk_disable_unprepare(struct lombo_dpu_clk *clk)
{
	struct lombo_dpu_top *top;

	if (!clk || (clk->sclk_id >= LOMBO_DPU_SCLK_NUM)) {
		DRM_ERROR("clk%p or invalid sclk_id%d\n", clk,
			clk ? clk->sclk_id : 0);
		return -EINVAL;
	}

	top = clk->dpu_top;
	lombo_dpu_disable_mod_clk(top, clk);
	lombo_dpu_disable_sclk(top, clk);

	return 0;
}
EXPORT_SYMBOL(lombo_dpu_clk_disable_unprepare);

static int lombo_dpu_parse_ext_sclk(struct platform_device *pdev,
	struct lombo_dpu_top *top)
{
	struct device *dev = &pdev->dev;
	char name[10] = { 0 };
	char name_1[12] = { 0 };
	int i;

	/* parse bus clks */
	top->ahb_gate = devm_clk_get(dev, "ahb_gate");
	if (IS_ERR_OR_NULL(top->ahb_gate)) {
		dev_err(dev, "failed to get dpu ahb_gate\n");
		return -ENODEV;
	}
	top->ahb_reset = devm_clk_get(dev, "ahb_reset");
	if (IS_ERR_OR_NULL(top->ahb_reset)) {
		dev_err(dev, "failed to get dpu ahb_reset\n");
		return -ENODEV;
	}
	top->mem_axi_gate = devm_clk_get(dev, "mem_axi_gate");
	if (IS_ERR_OR_NULL(top->mem_axi_gate)) {
		dev_err(dev, "failed to get dpu mem_axi_gate\n");
		return -ENODEV;
	}

	/* parse ext sclk & psclk */
	strncpy(name, "ext_sclk", 8);
	name[9] = '\0';
	strncpy(name_1, "ext_psclk", 9);
	name_1[10] = '\0';
	for (i = 0; i < LOMBO_DPU_EXT_SCLK_NUM; i++) {
		struct clk *sclk = NULL;
		struct clk *psclk = NULL;

		name[8] = i + '0';
		name_1[9] = i + '0';

		sclk = devm_clk_get(dev, name);
		if (IS_ERR_OR_NULL(sclk)) {
			sclk = devm_clk_get(dev, &(name[4]));
			if (IS_ERR_OR_NULL(sclk))
				continue;
		}
		top->ext_sclk[i] = sclk;

		psclk = devm_clk_get(dev, name_1);
		if (IS_ERR_OR_NULL(psclk)) {
			psclk = devm_clk_get(dev, &(name_1[4]));
			if (IS_ERR_OR_NULL(psclk))
				psclk = NULL;
		}
		if (psclk) {
			clk_set_parent(top->ext_sclk[i], psclk);
			top->ext_psclk[i] = psclk;
		}
		DRM_DEBUG("has %s, %s %s\n", name,
			psclk ? "has" : "no", name_1);
	}

	return 0;
}

static int lombo_dpu_create_sclk_old(struct platform_device *pdev,
	struct lombo_dpu_top *top, int sclk_id)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct lombo_dpu_sclk *sclk = &(top->sclk[sclk_id]);
	uint32_t value;
	int use_adpll = 0;
	int adpll_id = 0;

	if (!of_property_read_u32_index(np, "sclk_adpll_id", sclk_id, &value)) {
		DRM_DEBUG("sclk_adpll_id[%d]=%d\n", sclk_id, value);
		if (value < DPU_ADPLL_NUM) {
			use_adpll = 1;
			adpll_id = value;
		}
	}
	if (use_adpll) {
		struct device_node *np_adpll;
		struct dpu_adpll_config *cfg;
		char name[8] = { 0 };
		int ext_sclk = LOMBO_DPU_SCLK_NUM;

		snprintf(name, 8, "adpll%d", adpll_id);
		np_adpll = of_get_child_by_name(np, name);
		if (IS_ERR_OR_NULL(np_adpll)) {
			DRM_DEBUG("no %s np\n", name);
			return -EINVAL;
		}
		if (!of_property_read_u32(np_adpll, "sclk", &value))
			ext_sclk = value;
		else
			ext_sclk = 2;
		if (ext_sclk >= LOMBO_DPU_SCLK_NUM) {
			DRM_DEBUG("invalid sclk%d for %s", ext_sclk, name);
			return -EINVAL;
		}

		if (!top->ext_sclk[ext_sclk]) {
			DRM_DEBUG("no ext sclk%d for adpll-sclk%d\n",
				ext_sclk, sclk_id);
			return -EINVAL;
		}
		sclk->sel = DPU_SCLK_PARENT_ADPLL;
		sclk->ext_sclk = top->ext_sclk[ext_sclk];
		sclk->ext_psclk = top->ext_psclk[ext_sclk];

		if (!of_property_read_u32(np_adpll, "clk_rate", &value))
			sclk->rate = value;
		else
			sclk->rate = 600000000;

		cfg = &(sclk->adpll_cfg);
		if (!of_property_read_u32_index(np_adpll, "tune", 0, &value))
			cfg->tune0 = value;
		else
			cfg->tune0 = 0x139F0000;
		if (!of_property_read_u32_index(np_adpll, "tune", 1, &value))
			cfg->tune1 = value;
		else
			cfg->tune1 = 0x57379120;

	} else {
		if (!top->ext_sclk[sclk_id])
			return -EINVAL;
		sclk->sel = DPU_SCLK_PARENT_EXT;
		sclk->ext_sclk = top->ext_sclk[sclk_id];
		sclk->ext_psclk = top->ext_psclk[sclk_id];
	}

	spin_lock_init(&sclk->slock);

	DRM_DEBUG("sclk_id=%d: sel=%s,req_rate=%lu,adpll_tune[0x%x,0x%x]\n",
		sclk_id, sclk->sel ? "adpll" : "ext_sclk",
		sclk->rate, sclk->adpll_cfg.tune0, sclk->adpll_cfg.tune1);

	return 0;
}

static int lombo_dpu_create_sclk(struct platform_device *pdev,
	struct lombo_dpu_top *top)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *np_sclk, *np_adpll;
	struct lombo_dpu_sclk *sclk;
	char name[8] = { 0 };
	int i;
	uint32_t value;

	strncpy(name, "sclk", 4);
	name[5] = '\0';
	for (i = 0; i < LOMBO_DPU_SCLK_NUM; i++) {
		name[4] = '0' + i;

		np_sclk = of_get_child_by_name(np, name);
		if (IS_ERR_OR_NULL(np_sclk)
			|| !of_device_is_available(np_sclk)) {
			lombo_dpu_create_sclk_old(pdev, top, i);
			continue;
		}
		sclk = &(top->sclk[i]);

		if (!of_property_read_u32(np_sclk, "ext_sclk", &value)) {
			DRM_DEBUG("%s: ext_sclk=%d\n", name, value);
			if (value < LOMBO_DPU_EXT_SCLK_NUM) {
				sclk->ext_sclk = top->ext_sclk[value];
				sclk->ext_psclk = top->ext_psclk[value];
			}
		}
		if (!sclk->ext_sclk) {
			dev_err(dev, "not ext_sclk for sclk%d\n", i);
			return -EINVAL;
		}

		np_adpll = of_parse_phandle(np_sclk, "adpll", 0);
		if (!IS_ERR_OR_NULL(np_adpll)) {
			/* select adpll */
			sclk->sel = DPU_SCLK_PARENT_ADPLL;

			if (!of_property_read_u32(
					np_adpll, "clk_rate", &value))
				sclk->rate = value;
			else
				sclk->rate = 600000000;

			if (!of_property_read_u32_index(
					np_adpll, "tune", 0, &value))
				sclk->adpll_cfg.tune0 = value;
			else
				sclk->adpll_cfg.tune0 = 0x139F0000;
			if (!of_property_read_u32_index(
					np_adpll, "tune", 1, &value))
				sclk->adpll_cfg.tune1 = value;
			else
				sclk->adpll_cfg.tune1 = 0x57379120;

			of_node_put(np_adpll);
		} else {
			/* select ext sclk directly */
			sclk->sel = DPU_SCLK_PARENT_EXT;
		}
		DRM_DEBUG("%s: sel=%s,req_rate=%lu,adpll_tune[0x%x,0x%x]\n",
			name, sclk->sel ? "adpll" : "ext_sclk",
			sclk->rate,
			sclk->adpll_cfg.tune0, sclk->adpll_cfg.tune1);

		spin_lock_init(&sclk->slock);
	}

	return 0;
}

static int lombo_dpu_bind(struct device *comp,
	struct device *master, void *master_data)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	return 0;
}

static void lombo_dpu_unbind(struct device *comp,
	struct device *master, void *master_data)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);
}

static const struct component_ops lombo_dpu_ops = {
	.bind	= lombo_dpu_bind,
	.unbind	= lombo_dpu_unbind,
};

static int lombo_dpu_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct lombo_dpu_top *top;
	struct resource *res;
	int ret;

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	top = kzalloc(sizeof(*top), GFP_KERNEL);
	if (!top) {
		dev_err(dev, "Failed to alloc for dpu_top\n");
		return -ENOMEM;
	}

	/* parse register resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	top->reg = devm_ioremap(dev, res->start, resource_size(res));
	if (top->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_dpu_set_register_base(top->reg);
	csp_dpu_set_udelay_func(lombo_dpu_udelay);

	ret = lombo_dpu_parse_ext_sclk(pdev, top);
	if (ret)
		return ret;

	ret = lombo_dpu_create_sclk(pdev, top);
	if (ret)
		return ret;

	INIT_LIST_HEAD(&top->clk_list);
	spin_lock_init(&top->slock);

	dev_set_drvdata(dev, top);

	return component_add(&pdev->dev, &lombo_dpu_ops);
}

static int lombo_dpu_remove(struct platform_device *pdev)
{
	struct lombo_dpu_top *dpu_top = dev_get_drvdata(&pdev->dev);

	component_del(&pdev->dev, &lombo_dpu_ops);
	csp_dpu_set_register_base(NULL);
	dev_set_drvdata(&pdev->dev, NULL);
	kfree(dpu_top);

	return 0;
}

static const struct of_device_id lombo_dpu_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-dpu" },
	{ .compatible = "lombo,lombo-n7v1-dpu" },
	{ .compatible = "lombo,lombo-n8v0-dpu" },
	{ .compatible = "lombo,lombo-n9v0-dpu" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_dpu_of_table);

static struct platform_driver lombo_dpu_platform_driver = {
	.probe		= lombo_dpu_probe,
	.remove		= lombo_dpu_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-dpu",
		.of_match_table	= lombo_dpu_of_table,
	},
};
module_platform_driver(lombo_dpu_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU TOP Driver");
MODULE_LICENSE("GPL");
