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

#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/lombo_drm.h>

#include "lombo_drv.h"
#include "lombo_subdrv.h"
#include "lombo_framebuffer.h"
#include "lombo_crtc.h"
#include "lombo_wb.h"
#include "lombo_dit.h"
#include "lombo_rot.h"
#include "lombo_scale.h"

#define lombo_exit_if(cond)			\
	do {					\
		if (cond) {			\
			line = __LINE__;	\
			goto exit;		\
		}				\
	} while (0)

static const char *boot_disp_cmdline;

#if !defined(MODULE) && defined(CONFIG_DRM_FBDEV_EMULATION)
static int __init lombo_disp_setup(char *str)
{
	boot_disp_cmdline = str;
	return 1;
}
__setup("disp=", lombo_disp_setup);
#endif

static int lombo_strtou64_base16(
	const char *s, char **end, uint64_t *res)
{
	char val[32];
	int i = 0;
	int ret;
	uint64_t rv;

	while ((-1 != hex_to_bin(*s)) && (i < sizeof(val) - 1)) {
		val[i] = *s;
		i++;
		s++;
	}
	if (i == sizeof(val) - 1) {
		DRM_DEBUG("val=%s, i=%d\n", val, i);
		return -EINVAL;
	}
	val[i] = '\0';
	ret = kstrtou64(val, 16, &rv);
	if (ret) {
		DRM_DEBUG("val=%s, ret=%d\n", val, ret);
		return ret;
	}

	*end = (char *)s;
	*res = rv;
	return 0;
}

static int lombo_strtou32_base16(
	const char *s, char **end, uint32_t *res)
{
	uint64_t tmp;
	int ret;

	ret = lombo_strtou64_base16(s, end, &tmp);
	if (ret)
		return ret;

	if (tmp != (uint64_t)(uint32_t)tmp)
		return -ERANGE;

	*res = (uint32_t)tmp;
	return 0;
}

static int lombo_do_parse_dp_cmdline(
	struct lombo_boot_disp *bd, const char *p)
{
	char *endp;
	int line;
	int len;
	int ret;

	endp = strstr(p, ":");
	lombo_exit_if(!endp);
	len = endp - p;
	if ((len == 3) && !strncmp(p, "LCD", 3)) {
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->output_w));
		lombo_exit_if(ret || (*endp != 'x'));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->output_h));
		lombo_exit_if(ret);
	} else {
		lombo_exit_if(1);
	}

	lombo_exit_if(*endp != '@');
	p = endp + 1;
	ret = lombo_strtou32_base16(p, &endp, &(bd->tcon_id));
	lombo_exit_if(ret);

	DRM_DEBUG("w=%d, h=%d, tcon_id=%d\n",
		bd->output_w, bd->output_h, bd->tcon_id);

	return 0;
exit:
	DRM_INFO("%s:%d. p=%s, endp=%s\n", __func__, line,
		p ? p : "null", endp ? endp : "null");
	return -EINVAL;
}

static int lombo_do_parse_fb_cmdline(
	struct lombo_boot_disp *bd, const char *p)
{
	char *endp;
	int line, ret;
	struct drm_format_name_buf buf;

	if (!strncmp(p, "AR24", 4)) {
		bd->fmt = DRM_FORMAT_ARGB8888;
		endp = (char *)p + 4;
	} else {
		lombo_exit_if(1);
	}

	/* parse w/h */
	if (*endp == ':') {
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->width));
		lombo_exit_if(ret || (*endp != 'x'));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->height));
		lombo_exit_if(ret);
	} else {
		bd->width = bd->output_w;
		bd->height = bd->output_h;
	}

	/* parse pitch */
	lombo_exit_if(*endp != '+');
	p = endp + 1;
	ret = lombo_strtou32_base16(p, &endp, &(bd->pitch));
	lombo_exit_if(ret);

	/* parse copy region */
	if (*endp == '(') {
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->copy_x));
		lombo_exit_if(ret || (*endp != ','));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->copy_y));
		lombo_exit_if(ret || (*endp != ','));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->copy_w));
		lombo_exit_if(ret || (*endp != ','));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->copy_h));
		lombo_exit_if(ret || (*endp != ')'));
		endp++;
	} else {
		bd->copy_x = 0;
		bd->copy_y = 0;
		bd->copy_w = bd->width;
		bd->copy_h = bd->height;
	}

	/* parse scn */
	if (*endp == '[') {
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->scn_x));
		lombo_exit_if(ret || (*endp != ','));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->scn_y));
		lombo_exit_if(ret || (*endp != ','));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->scn_w));
		lombo_exit_if(ret || (*endp != ','));
		p = endp + 1;
		ret = lombo_strtou32_base16(p, &endp, &(bd->scn_h));
		lombo_exit_if(ret || (*endp != ']'));
		endp++;
	} else {
		bd->scn_x = 0;
		bd->scn_y = 0;
		bd->scn_w = bd->output_w;
		bd->scn_h = bd->output_h;
	}

	/* parse phy addr */
	lombo_exit_if(*endp != '#');
	p = endp + 1;
	ret = lombo_strtou64_base16(p, &endp, &(bd->phy_addr));
	lombo_exit_if(ret);

	/* parse win id */
	lombo_exit_if(*endp != '@');
	p = endp + 1;
	ret = lombo_strtou32_base16(p, &endp, &(bd->win_id));
	lombo_exit_if(ret);

	/* parse dc id */
	lombo_exit_if(*endp != '@');
	p = endp + 1;
	ret = lombo_strtou32_base16(p, &endp, &(bd->dc_id));
	lombo_exit_if(ret);

	DRM_DEBUG("fmt(%s)\n", drm_get_format_name(bd->fmt, &buf));
	DRM_DEBUG("w=%d, h=%d, pitch=%d, addr=0x%llx\n",
		bd->width, bd->height, bd->pitch, bd->phy_addr);
	DRM_DEBUG("COPY(%d,%d,%d,%d), SCN[%d,%d,%d,%d]\n",
		bd->copy_x, bd->copy_y, bd->copy_w, bd->copy_h,
		bd->scn_x, bd->scn_y, bd->scn_w, bd->scn_h);
	DRM_DEBUG("win(%d),dc(%d)\n", bd->win_id, bd->dc_id);

	return 0;
exit:
	DRM_INFO("%s:%d. p=%s, endp=%s\n", __func__, line,
		p ? p : "null", endp ? endp : "null");
	return -EINVAL;
}

static int lombo_do_parse_boot_disp_cmdline(
	struct lombo_boot_disp *bd, const char *p)
{
	char *endp;
	int ret;

	endp = strstr(p, ";");
	if (!endp) {
		DRM_INFO("%s: line=%d\n", __func__, __LINE__);
		return -EINVAL;
	}

	ret = lombo_do_parse_dp_cmdline(bd, p);
	if (ret)
		return ret;

	p = endp + 1;
	ret = lombo_do_parse_fb_cmdline(bd, p);

	return ret;
}

static int lombo_parse_boot_disp_cmdline(struct lombo_drv *drv)
{
	const char *cmdline = boot_disp_cmdline;
	struct lombo_boot_disp *bd;
	int ret;

	if (!cmdline || (cmdline[0] == '\0'))
		return -EINVAL;

	bd = kzalloc(sizeof(*bd), GFP_KERNEL);
	if (!bd) {
		DRM_INFO("%s: alloc for boot_disp failed\n", __func__);
		return -ENOMEM;
	}

	ret = lombo_do_parse_boot_disp_cmdline(bd, cmdline);
	if (!ret)
		drv->boot_disp = bd;
	else
		kfree(bd);

	return ret;
}

static void lombo_free_boot_disp(struct lombo_drv *drv)
{
	struct lombo_boot_disp *bd = drv->boot_disp;
	struct drm_crtc *crtc;
	struct drm_crtc_helper_funcs *funcs;

	if (!bd)
		return;

	drv->boot_disp = NULL;
	crtc = bd->crtc;
	kfree(bd);

	/* do crtc commit */
	if (crtc) {
		funcs = crtc->helper_private;
		if (funcs && funcs->commit)
			funcs->commit(crtc);
		DRM_DEBUG("\n");
	}

}

static int lombo_drv_load(struct drm_device *drm, unsigned long flags)
{
	struct lombo_drv *drv;
	int ret;

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	drv = kzalloc(sizeof(*drv), GFP_KERNEL);
	if (!drv) {
		DRM_ERROR("alloc failed\n");
		return -ENOMEM;
	}

	/* parse fb_win_id */
	if (!of_property_read_u32(drm->dev->of_node, "fb_win_id", &ret))
		drv->fb_win_id = ret;
	else
		drv->fb_win_id = -EINVAL;

	INIT_LIST_HEAD(&drv->tcon_list);
	INIT_LIST_HEAD(&drv->dc_list);
	INIT_LIST_HEAD(&drv->se_list);
	INIT_LIST_HEAD(&drv->ove_list);
	INIT_LIST_HEAD(&drv->page_flip_event_list);
	drm->dev_private = drv;
	dev_set_drvdata(drm->dev, drv);

	drm_mode_config_init(drm);

	lombo_parse_boot_disp_cmdline(drv);

	ret = component_bind_all(drm->dev, drm);
	if (ret) {
		dev_err(drm->dev, "Couldn't bind all pipelines components\n");
		goto cleanup_mode_config;
	}

	ret = drm_vblank_init(drm, drm->mode_config.num_crtc);
	if (ret) {
		dev_err(drm->dev, "init drm vblank failed\n");
		goto cleanup_mode_config;
	}
	drm->irq_enabled = true;

	ret = lombo_framebuffer_init(drm);
	if (ret) {
		dev_err(drm->dev, "lombo_framebuffer_init fail\n");
		goto cleanup_mode_config;
	}

	lombo_free_boot_disp(drv);

	lombo_scale_module_init();
	lombo_subdrv_probe(drm);

	return 0;

/* free_framebuffer: */
	lombo_framebuffer_free(drm);
cleanup_mode_config:
	drm_mode_config_cleanup(drm);
	lombo_free_boot_disp(drv);
	return ret;
}

static int lombo_drv_unload(struct drm_device *drm)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	lombo_subdrv_remove(drm);
	lombo_framebuffer_free(drm);
	drm_vblank_cleanup(drm);
	drm_mode_config_cleanup(drm);
	component_unbind_all(drm->dev, drm);

	kfree(drm->dev_private);
	drm->dev_private = NULL;

	return 0;
}

static int lombo_drv_firstopen(struct drm_device *drm)
{
	DRM_DEBUG_DRIVER("\n");
	return 0;
}

static int lombo_drv_open(struct drm_device *drm,
	struct drm_file *file_priv)
{
	struct lombo_file_private *efile_priv;

	DRM_DEBUG_DRIVER("\n");

	efile_priv = kzalloc(sizeof(*efile_priv), GFP_KERNEL);
	if (!efile_priv)
		return -ENOMEM;

	file_priv->driver_priv = efile_priv;

	return lombo_subdrv_open(drm, file_priv);
}

static void lombo_drv_preclose(struct drm_device *drm,
	struct drm_file *file_priv)
{
	DRM_DEBUG_DRIVER("\n");

	lombo_subdrv_close(drm, file_priv);
}

static void lombo_drv_postclose(struct drm_device *drm,
	struct drm_file *file_priv)
{
	DRM_DEBUG_DRIVER("\n");
	if (!file_priv->driver_priv)
		return;

	kfree(file_priv->driver_priv);
	file_priv->driver_priv = NULL;
}

static void lombo_drv_lastclose(struct drm_device *file_priv)
{
	DRM_DEBUG_DRIVER("\n");
}

static const struct file_operations lombo_drv_fops = {
	.owner = THIS_MODULE,
	.open = drm_open,
	.mmap = drm_gem_cma_mmap,
	.poll = drm_poll,
	.read = drm_read,
	.unlocked_ioctl = drm_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = drm_compat_ioctl,
#endif
	.release = drm_release,
};

#ifdef CONFIG_DEBUG_FS

static int lombo_drv_dpu_top_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0xA0;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_DPU_TOP
	csp_dpu_get_register_base((unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get dpu top regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "dpu top regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "dpu top regs hex dump end.\n");

	return 0;
}

static int lombo_drv_dpu_ove0_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0xD0;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_OVE
	csp_ove_get_register_base(0, (unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get dpu ove0 regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "dpu ove0 regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "doss ove0 regs hex dump end.\n");

	return 0;
}

static int lombo_drv_dpu_se0_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0x1D0;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_SE
	csp_se_get_register_base(0, (unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get dpu se0 regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "dpu se0 regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	p[0x34] = 1; /* SE Coefficient Table Switch Register */
	size = 0x6D0;
	for (i = 0x400 / 4; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	p[0x34] = 0;
	size = 0xD00;
	for (i = 0x900 / 4; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "doss se0 regs hex dump end.\n");

	return 0;
}

static int lombo_drv_dpu_dc0_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0x600;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_DC
	csp_dc_get_register_base(0, (unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get dpu dc0 regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "dpu dc0 regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "doss dc0 regs hex dump end.\n");

	return 0;
}

static int lombo_drv_doss_top_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0x1000;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_DOSS_TOP
	csp_doss_get_register_base(0, (unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get doss top regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "doss top regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "doss top regs hex dump end.\n");

#if 1
	/* debug video pll setting */
	p = (int *)0xf400a600;
	size = 0x200;
	seq_printf(m, "video pll registers hex dump start, 0x%p\n", p);

	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "video pll registers hex dump end.\n");

	/* debug gpio setting */
	p = (int *)0xf4015124;
	size = 0x4;
	seq_printf(m, "gpioe8 output registers hex dump start, 0x%p\n", p);

	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "gpioe8 output registers hex dump end.\n");

	p = (int *)0xf4015150;
	size = 0x4;
	seq_printf(m, "gpioe8 value registers hex dump start, 0x%p\n", p);

	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "gpioe8 value registers hex dump end.\n");

	p = (int *)0xf4015120;
	size = 0x4;
	seq_printf(m, "gpioe7 output registers hex dump start, 0x%p\n", p);

	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "gpioe7 output registers hex dump end.\n");

	p = (int *)0xf4015150;
	size = 0x4;
	seq_printf(m, "gpioe7 value registers hex dump start, 0x%p\n", p);

	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "gpioe7 value registers hex dump end.\n");
#endif

	return 0;
}

static int lombo_drv_doss_tcon_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0x1400;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_TCON
	csp_tcon_get_register_base(0, (unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get doss top regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "doss tcon regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "tcon regs hex dump end.\n");

	return 0;
}

static int lombo_drv_doss_mipi_dsi_regs_show(
	struct seq_file *m, void *arg)
{
	/**
	 * struct drm_info_node *node = m->private;
	 * struct drm_device *dev = node->minor->dev;
	 */
	u32 size = 0x80;
	u32 i;
	u32 *p = NULL;

	#ifdef CONFIG_DRM_LOMBO_MIPI_DSI
	csp_mipi_dsi_get_register_base(0, (unsigned long *)&p);
	#endif
	if (!p) {
		DRM_DEBUG("get doss top regs failed\n");
		return -EINVAL;
	}

	seq_printf(m, "doss mipi dsi regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size / 4; i += 4) {
		seq_printf(m, "0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		i * 0x4, p[i], p[i + 1], p[i + 2], p[i + 3]);
	}
	seq_puts(m, "mipi dsi regs hex dump end.\n");

	return 0;
}

static int lombo_drv_doss_tve_regs_show(
	struct seq_file *m, void *arg)
{
	u32 size = 0x0100;
	u32 i;
	char *p = NULL;

#ifdef CONFIG_DRM_LOMBO_TVE
	csp_tve_get_register_base(0, (unsigned long *)&p);
#endif

	if (!p) {
		DRM_DEBUG("get doss tve regs failed\n");
		return -EINVAL;
	}
	seq_printf(m, "doss tve regs hex dump start, 0x%p\n", p);
	for (i = 0; i < size; i += 16) {
		seq_printf(m, "0x%08x: 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x ",
			i, p[i + 3], p[i + 2], p[i + 1], p[i],
			p[i + 7], p[i + 6], p[i + 5], p[i + 4]);
		seq_printf(m, "0x%02x%02x%02x%02x 0x%02x%02x%02x%02x\n",
			p[i + 11], p[i + 10], p[i + 9], p[i + 8],
			p[i + 15], p[i + 14], p[i + 13], p[i + 12]);
	}
	seq_puts(m, "tve regs hex dump end.\n");

	return 0;
}

static struct drm_info_list lombo_drv_debugfs_list[] = {
	{ "lombo_kms_dump", lombo_crtc_kms_show, 0 },
	{ "dpu_top_regs", lombo_drv_dpu_top_regs_show, 0 },
	{ "dpu_ove0_regs", lombo_drv_dpu_ove0_regs_show, 0 },
	{ "dpu_se0_regs", lombo_drv_dpu_se0_regs_show, 0 },
	{ "dpu_dc0_regs", lombo_drv_dpu_dc0_regs_show, 0 },
	{ "doss_top_regs", lombo_drv_doss_top_regs_show, 0 },
	{ "doss_tcon_regs", lombo_drv_doss_tcon_regs_show, 0 },
	{ "doss_mipi_dsi_regs", lombo_drv_doss_mipi_dsi_regs_show, 0 },
	{ "doss_tve_regs", lombo_drv_doss_tve_regs_show, 0 }
};

static int lombo_drv_debugfs_init(struct drm_minor *minor)
{
	struct drm_device *dev = minor->dev;
	int ret;

	ret = drm_debugfs_create_files(lombo_drv_debugfs_list,
			ARRAY_SIZE(lombo_drv_debugfs_list),
			minor->debugfs_root, minor);
	if (ret)
		dev_err(dev->dev, "install lombo_drv_debugfs_list failed\n");

	return ret;
}

static void lombo_drv_debugfs_cleanup(struct drm_minor *minor)
{
	drm_debugfs_remove_files(lombo_drv_debugfs_list,
			ARRAY_SIZE(lombo_drv_debugfs_list), minor);
}

#else
/*here is no CONFIG_DEBUG_FS */

#define lombo_drv_debugfs_init NULL
#define lombo_drv_debugfs_cleanup NULL

#endif

static int lombo_drv_enable_vblank(struct drm_device *dev, int crtc)
{
	DRM_DEBUG_KMS("crtc=%d\n", crtc);
	return 0;
}

static void lombo_drv_disable_vblank(struct drm_device *dev, int crtc)
{
	DRM_DEBUG_KMS("crtc=%d\n", crtc);
}

static int lombo_ignore_ioctl_permit(
	struct drm_file *file_priv, unsigned int cmd)
{
	struct lombo_file_private *pfile = file_priv->driver_priv;

	switch (cmd) {
	case LOMBO_IGNORE_IOCTL_PERMIT_SET:
		pfile->ignore_ioctl_permit = 1;
		break;
	case LOMBO_IGNORE_IOCTL_PERMIT_UNSET:
		pfile->ignore_ioctl_permit = 0;
		break;
	case LOMBO_IGNORE_IOCTL_PERMIT_GET:
		return pfile->ignore_ioctl_permit;
	default:
		return -EINVAL;
	}

	return 0;
}

int lombo_ignore_ioctl_permit_get(struct drm_file *file_priv)
{
	return lombo_ignore_ioctl_permit(file_priv,
					 LOMBO_IGNORE_IOCTL_PERMIT_GET);
}
EXPORT_SYMBOL(lombo_ignore_ioctl_permit_get);

static int lombo_ignore_ioctl_permit_ioctl(struct drm_device *dev,
	void *data, struct drm_file *file_priv)
{
	unsigned int *cmd = data;
	int ret;

	ret = lombo_ignore_ioctl_permit(file_priv, *cmd);
	if (*cmd == LOMBO_IGNORE_IOCTL_PERMIT_GET) {
		*cmd = ret;
		ret = 0;
	}

	return ret;
}

static struct drm_ioctl_desc lombo_drv_ioctls[] = {
	DRM_IOCTL_DEF_DRV(LOMBO_PAGE_FLIP_ATOMIC,
			lombo_page_flip_atomic_ioctl,
			DRM_MASTER | DRM_CONTROL_ALLOW | DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_WB_SET_PARA,
			lombo_wb_set_para_ioctl,
			DRM_MASTER | DRM_CONTROL_ALLOW | DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_WB_QUEUE_BUF,
			lombo_wb_queue_buf_ioctl,
			DRM_MASTER | DRM_CONTROL_ALLOW | DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_WB_DEQUEUE_BUF,
			lombo_wb_dequeue_buf_ioctl,
			DRM_MASTER | DRM_CONTROL_ALLOW | DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_WB_START,
			lombo_wb_start_ioctl,
			DRM_MASTER | DRM_CONTROL_ALLOW | DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_WB_STOP,
			lombo_wb_stop_ioctl,
			DRM_MASTER | DRM_CONTROL_ALLOW | DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_IGNORE_IOCTL_PERMIT,
			lombo_ignore_ioctl_permit_ioctl,
			DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_MODE_OBJ_SETPROPERTY,
			drm_mode_obj_set_property_ioctl, DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_MODE_SETGAMMA,
			drm_mode_gamma_set_ioctl, DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_DIT_GET_VER,
			lombo_dit_get_ver_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_DIT_OPEN,
			lombo_dit_open_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_DIT_CLOSE,
			lombo_dit_close_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_DIT_SET_TIMEOUT,
			lombo_dit_set_timeout_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_DIT_EXEC,
			lombo_dit_exec_ioctl, DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_ROT_GET_VER,
			lombo_rot_get_ver_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_ROT_OPEN,
			lombo_rot_open_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_ROT_CLOSE,
			lombo_rot_close_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_ROT_SET_TIMEOUT,
			lombo_rot_set_timeout_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_ROT_EXEC,
			lombo_rot_exec_ioctl, DRM_UNLOCKED),

	DRM_IOCTL_DEF_DRV(LOMBO_SCALE_OPEN,
			lombo_scale_open_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_SCALE_CLOSE,
			lombo_scale_close_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_SCALE_SET_TIMEOUT,
			lombo_scale_set_timeout_ioctl, DRM_UNLOCKED),
	DRM_IOCTL_DEF_DRV(LOMBO_SCALE_EXEC,
			lombo_scale_exec_ioctl, DRM_UNLOCKED),

};

static struct drm_driver lombo_drv_driver = {
	.driver_features = DRIVER_GEM | DRIVER_MODESET | DRIVER_PRIME
		| DRIVER_HAVE_IRQ,

	/* Generic Operations */
	.load = lombo_drv_load,
	.unload = lombo_drv_unload,
	.firstopen = lombo_drv_firstopen,
	.open = lombo_drv_open,
	.preclose = lombo_drv_preclose,
	.postclose = lombo_drv_postclose,
	.lastclose = lombo_drv_lastclose,
	.fops = &lombo_drv_fops,
	.name = "lombo",
	.desc = "Lombo Soc DRM",
	.date = "20190109",
	.major = 1,
	.minor = 0,

	/* Lombo ioctl Operations */
	.ioctls = lombo_drv_ioctls,
	.num_ioctls = ARRAY_SIZE(lombo_drv_ioctls),

	/* IRQ Operations */
	.get_vblank_counter = drm_vblank_count,
	.enable_vblank = lombo_drv_enable_vblank,
	.disable_vblank = lombo_drv_disable_vblank,

	/* GEM Operations */
	.gem_vm_ops = &drm_gem_cma_vm_ops,
	.gem_free_object = drm_gem_cma_free_object,
	.dumb_create = drm_gem_cma_dumb_create,
	.dumb_map_offset = drm_gem_cma_dumb_map_offset,
	.dumb_destroy = drm_gem_cma_dumb_destroy,

	/* PRIME Operations */
	.prime_handle_to_fd = drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle = drm_gem_prime_fd_to_handle,
	.gem_prime_export = drm_gem_prime_export,
	.gem_prime_import = drm_gem_prime_import,
	.gem_prime_get_sg_table = drm_gem_cma_prime_get_sg_table,
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table,

	/* DEBUGFS */
	.debugfs_init = lombo_drv_debugfs_init,
	.debugfs_cleanup = lombo_drv_debugfs_cleanup,
};

static int lombo_drv_bind(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);
	/* lombo_drm_driver.num_ioctls = DRM_ARRAY_SIZE(lombo_ioctls); */

	return drm_platform_init(&lombo_drv_driver, pdev);
}

static void lombo_drv_unbind(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	drm_platform_exit(&lombo_drv_driver, pdev);
}

static const struct component_master_ops lombo_drv_master_ops = {
	.bind	= lombo_drv_bind,
	.unbind	= lombo_drv_unbind,
};

static int compare_of(struct device *dev, void *data)
{
	DRM_DEBUG_DRIVER("Comparing of node %pOF with %pOF\n",
			dev->of_node, data);

	return dev->of_node == data;
}

static void lombo_drv_release_of(struct device *dev, void *data)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);
	of_node_put(data);
}

static void lombo_drv_of_component_match_add(
				struct device *master,
				struct component_match **matchptr,
				int (*compare)(struct device *, void *),
				struct device_node *node)
{
	of_node_get(node);
	component_match_add_release(master, matchptr,
				    lombo_drv_release_of,
				    compare, node);
}

static int is_configed_driver(const char *name)
{
	if (!strcmp(name, "dpu"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_DPU_TOP);
	else if (!strcmp(name, "dc"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_DC);
	else if (!strcmp(name, "ove"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_OVE);
	else if (!strcmp(name, "se"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_SE);
	else if (!strcmp(name, "wb"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_WB);
	else if (!strcmp(name, "doss"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_DOSS_TOP);
	else if (!strcmp(name, "tcon"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_TCON);
	else if (!strcmp(name, "mipi_dsi"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_MIPI_DSI);
	else if (!strcmp(name, "tve"))
		return IS_ENABLED(CONFIG_DRM_LOMBO_TVE);

	DRM_INFO("%s: drv[%s] is not configed\n", __func__, name);
	return 0;
}

static int lombo_drv_add_components(struct device *dev,
	struct component_match **matchptr)
{
	struct device_node *np = dev->of_node;
	int i, count  = 0;

	for (i = 0;; i++) {
		struct device_node *node = of_parse_phandle(
					np, "components", i);

		if (!node)
			break;

		if (!of_device_is_available(node)
			|| !is_configed_driver(node->name))
			continue;

		DRM_DEBUG_DRIVER("add component[%s]\n",
			of_node_full_name(node));
		lombo_drv_of_component_match_add(dev, matchptr,
			compare_of, node);
		count++;
	}

	return count;
}

static int lombo_drv_probe(struct platform_device *pdev)
{
	struct component_match *match = NULL;
	int count = 0;

	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	count = lombo_drv_add_components(&pdev->dev, &match);

	if (count)
		return component_master_add_with_match(&pdev->dev,
				&lombo_drv_master_ops, match);
	else
		return 0;
}

static int lombo_drv_remove(struct platform_device *pdev)
{
	DRM_DEBUG_DRIVER("%d\n", __LINE__);

	component_master_del(&pdev->dev, &lombo_drv_master_ops);
	return 0;
}

int lombo_drv_suspend(struct device *dev)
{
	struct lombo_drv *drv = dev_get_drvdata(dev);

	DRM_DEBUG_KMS("\n");
	drm_fbdev_cma_destory_mode(drv->fbdev);
	return 0;
}

int lombo_drv_resume(struct device *dev)
{
	struct lombo_drv *drv = dev_get_drvdata(dev);

	DRM_DEBUG_KMS("\n");
	drm_fbdev_cma_restore_mode(drv->fbdev);
	return 0;
}

static const struct dev_pm_ops lombo_drv_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(lombo_drv_suspend, lombo_drv_resume)
};

static const struct of_device_id lombo_drv_of_table[] = {
	{ .compatible = "lombo,lombo-display" },
	{ .compatible = "lombo,lombo-n7v0-display" },
	{ .compatible = "lombo,lombo-n7v1-display" },
	{ .compatible = "lombo,lombo-n8v0-display" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_drv_of_table);

static struct platform_driver lombo_drv_platform_driver = {
	.probe		= lombo_drv_probe,
	.remove		= lombo_drv_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-drm",
		.of_match_table	= lombo_drv_of_table,
		.pm = &lombo_drv_pm_ops,
	},
};
module_platform_driver(lombo_drv_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DRM/KMS Driver");
MODULE_LICENSE("GPL");
