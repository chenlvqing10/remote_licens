/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS driver code for LomboTech
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/bug.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_i2c.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-of.h>
#include <media/media-device.h>
#include <linux/delay.h>

/* #define ENABLE_TRACE */
#include <mach/debug.h>

#include <mach/csp.h>

#include "viss.h"
#include "media-dev.h"
#include "csp.h"
#include "cccore/ccc.h"

#define ADPLL_LOCK_TIMEOUT	5000	/* 5ms */
#define ADPLL_LOCK_CHK_INTVAL	100	/* 100 us */

#define ISP_1080P_INPUT_FREQ	(200 * 1000 * 1000)
#define VIC_FREQ		(1485 * 100 * 1000)
#define MCSI_FREQ		(1485 * 100 * 1000)


#define MAX_MCLK_DIV		32
#define SRCCLK_ID_MAX		2
struct viss_mclk_info {
	u32	val;
	u32	srcclk_id;
};

static const struct viss_mclk_info mclk_info[][SRCCLK_ID_MAX] = {
	{	/* VISS_VIC_SENSOR */
		{0, 2},
		{1, 0},
	}, {	/* VISS_MCSI_SENSOR */
		{0, 2},
		{1, 0},
	},

};

static int __viss_md_set_camclk(struct viss_md *vmd,
				struct viss_source_info *si,
				bool on);

struct media_entity *viss_pipeline_get_head(struct media_entity *me)
{
	struct media_pad *pad = &me->pads[0];

	PRT_TRACE_BEGIN("\n");

	PRT_DBG("me->name:%s\n", me->name);
	while (!(pad->flags & MEDIA_PAD_FL_SOURCE)) {
		pad = media_entity_remote_source(pad);
		if (!pad)
			break;
		me = pad->entity;
		pad = &me->pads[0];
		PRT_DBG("me->name:%s\n", me->name);
	}
	PRT_TRACE_END("\n");

	return me;
}

/**
 * viss_pipeline_prepare - update pipeline information with subdevice pointers
 * @me: media entity terminating the pipeline
 *
 * Caller holds the graph mutex.
 */
static void viss_pipeline_prepare(struct viss_pipeline *p,
				  struct media_entity *me)
{
	struct v4l2_subdev *sd;
	int i;

	PRT_TRACE_BEGIN("\n");

	/* clear the pipeline */
	for (i = 0; i < IDX_MAX; i++)
		p->subdevs[i] = NULL;

	while (1) {
		struct media_pad *pad = NULL;

		/* Find remote source pad */
		for (i = 0; i < me->num_pads; i++) {
			struct media_pad *spad = &me->pads[i];

			if (!(spad->flags & MEDIA_PAD_FL_SINK))
				continue;
			pad = media_entity_remote_source(spad);
			if (pad)
				break;
		}

		if (pad == NULL ||
		    media_entity_type(pad->entity) != MEDIA_ENT_T_V4L2_SUBDEV)
			break;
		sd = media_entity_to_v4l2_subdev(pad->entity);

		switch (sd->grp_id) {
		case GRP_ID_SENSOR:
			p->subdevs[IDX_SENSOR] = sd;
			PRT_DBG("pipeline add sd [IDX_SENSOR]\n");
			break;
		case GRP_ID_VIC:
			p->subdevs[IDX_VIC] = sd;
			PRT_DBG("pipeline add sd [IDX_VIC]\n");
			break;
		case GRP_ID_MCSI:
			p->subdevs[IDX_MCSI] = sd;
			PRT_DBG("pipeline add sd [IDX_MCSI]\n");
			break;
		case GRP_ID_VISS:
			/* No need to control VISS subdev through subdev ops */
			break;
		case GRP_ID_VISS_IS:
			p->subdevs[IDX_IS_ISP] = sd;
			PRT_DBG("pipeline add sd [IDX_IS_ISP]\n");
			break;

		default:
			break;
		}
		me = &sd->entity;

		/* num_pads == 1 means the subdev is source of pipeline */
		if (me->num_pads == 1)
			break;
	}
	PRT_TRACE_END("\n");
}

/**
 * __subdev_set_power - change power state of a single subdev
 * @sd: subdevice to change power state for
 * @on: 1 to enable power or 0 to disable
 *
 * Return result of s_power subdev operation or -ENXIO if sd argument
 * is NULL. Return 0 if the subdevice does not implement s_power.
 */
static int __subdev_set_power(struct v4l2_subdev *sd, int on)
{
	int *use_count;
	int ret;

	/* -ENXIO means subdev is not present in pipeline */
	if (sd == NULL)
		return -ENXIO;

	PRT_TRACE_BEGIN("%s %s\n", sd->name, on ? "on" : "off");

	use_count = &sd->entity.use_count;
	if (on && (*use_count)++ > 0) {
		/* subdev is in used, so power is already set */
		PRT_TRACE_END("\n");
		return 0;
	} else if (!on && (*use_count == 0 || --(*use_count) > 0)) {
		/* if subdev is already off or still there's other device
		 * use it, do nothing
		 */
		PRT_TRACE_END("\n");
		return 0;
	}
	ret = v4l2_subdev_call(sd, core, s_power, on);

	PRT_TRACE_END("ret=%d\n", ret);

	return ret != -ENOIOCTLCMD ? ret : 0;
}

/**
 * viss_pipeline_s_power - change power state of all pipeline subdevs
 * @p: video pipeline structure
 * @on: true to power on, false to power off
 *
 * Needs to be called with the graph mutex held.
 */
static int viss_pipeline_s_power(struct viss_pipeline *p, bool on)
{
	static const u8 seq[2][IDX_MAX] = {
		{IDX_IS_ISP, IDX_MCSI, IDX_VIC, IDX_SENSOR},
		{IDX_SENSOR, IDX_MCSI, IDX_VIC, IDX_IS_ISP},
	};
	int i, ret = 0;

	PRT_TRACE_BEGIN("%s\n", on ? "on" : "off");

	/* sensor is the source of pipeline, must be present */
	if (p->subdevs[IDX_SENSOR] == NULL)
		return -ENXIO;

	for (i = 0; i < IDX_MAX - 1; i++) {
		unsigned int idx = seq[on][i];

		ret = __subdev_set_power(p->subdevs[idx], on);

		/* -ENXIO means subdev is not present in pipeline */
		if (ret < 0 && ret != -ENXIO) {
			PRT_ERR("pipeline sd [%d] %s s_power %s failed\n",
				i, p->subdevs[idx]->name, on ? "on" : "off");
			goto error;
		}

		if (ret >= 0)
			PRT_DBG("pipeline sd [%d] %s s_power %s\n",
				i, p->subdevs[idx]->name, on ? "on" : "off");
	}

	PRT_TRACE_END("ret=0\n");
	return 0;
error:
	/* rewind the s_power operations that have already done */
	for (; i >= 0; i--) {
		unsigned int idx = seq[on][i];

		__subdev_set_power(p->subdevs[idx], !on);
	}
	PRT_TRACE_END("ret=%d\n", ret);
	return ret;
}

/**
 * __viss_pipeline_open - update the pipeline information, enable power
 *                        of all pipeline subdevs and the sensor clock
 * @p: video pipeline structure
 * @me: media entity to start graph walk with
 * @prepare: true to walk the current pipeline and acquire all subdevs
 *
 * Called with the graph mutex held.
 */
static int __viss_pipeline_open(struct viss_pipeline *p,
				struct media_entity *me, bool prepare)
{
	struct v4l2_subdev *sd = NULL;
	int ret;

	if (WARN_ON(p == NULL || me == NULL))
		return -EINVAL;

	PRT_TRACE_BEGIN("%s\n", prepare ? "prepare" : "");
	/*
	 * prepare the pipeline that preceding this media_entity,
	 * so if we want to enable the whole pipeline, "me" should
	 * point to the last media_entity of the pipeline
	 */
	if (prepare)
		viss_pipeline_prepare(p, me);

	/* the sensor subdev must be present */
	sd = p->subdevs[IDX_SENSOR];
	if (sd == NULL)
		return -EINVAL;

	/* enable sensor master clock */
	ret = viss_md_set_camclk(sd, true);

	if (ret < 0)
		return ret;


	ret = viss_pipeline_s_power(p, true);
	if (ret != 0)
		/* s_power failed,we should disable the sensor master clock */
		viss_md_set_camclk(sd, false);

	PRT_TRACE_END("\n");
	return ret;
}

/**
 * __viss_pipeline_close - disable the sensor clock and pipeline power
 * @p: video pipeline structure
 *
 * Disable power of all subdevs and turn the external sensor clock off.
 */
static int __viss_pipeline_close(struct viss_pipeline *p)
{
	struct v4l2_subdev *sd = p ? p->subdevs[IDX_SENSOR] : NULL;
	int ret = 0;

	if (WARN_ON(sd == NULL))
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	ret = viss_pipeline_s_power(p, false);
	if (sd != NULL)
		viss_md_set_camclk(sd, false);

	PRT_TRACE_END("\n");
	return ret == -ENXIO ? 0 : ret;
}

/**
 * __viss_pipeline_s_stream - call s_stream() on pipeline subdevs
 * @p: video pipeline structure
 * @on: passed as the s_stream() callback argument
 */
static int __viss_pipeline_s_stream(struct viss_pipeline *p, bool on)
{
	static const u8 seq[2][IDX_MAX - 1] =  {
		{IDX_IS_ISP, IDX_MCSI, IDX_VIC, IDX_SENSOR},
		{IDX_IS_ISP, IDX_MCSI, IDX_VIC, IDX_SENSOR},
	};
	int i, ret = 0;

	if ((p->subdevs[IDX_SENSOR] == NULL))
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", on ? "on" : "off");
	for (i = 0; i < IDX_MAX - 1; i++) {
		unsigned int idx = seq[on][i];

		ret = v4l2_subdev_call(p->subdevs[idx], video, s_stream, on);

		if (ret < 0 && ret != -ENOIOCTLCMD && ret != -ENODEV)
			goto error;

		if (ret >= 0)
			PRT_DBG("pipeline sd [%d] %s s_stream %s\n",
				i, p->subdevs[idx]->name, on ? "on" : "off");
	}
	PRT_TRACE_END("\n");
	return 0;
error:
	for (; i >= 0; i--) {
		unsigned int idx = seq[on][i];
		v4l2_subdev_call(p->subdevs[idx], video, s_stream, !on);
	}
	return ret;
}

/**
 * __viss_pipeline_restart - restart all pipeline subdevs
 * @p: video pipeline structure
 * @state: 1 to restart, 0 to disable
 *
 * Needs to be called with the graph mutex held.
 */
static int __viss_pipeline_restart(struct viss_pipeline *p, int state)
{
	int ret = 0;

	v4l2_subdev_call(p->subdevs[IDX_MCSI], core, reset, state);
	if (ret < 0 && ret != -ENOIOCTLCMD && ret != -ENODEV) {
		PRT_ERR("restart mcsi failed!\n");
		return ret;
	}

	v4l2_subdev_call(p->subdevs[IDX_VIC], core, reset, state);
	if (ret < 0 && ret != -ENOIOCTLCMD && ret != -ENODEV) {
		PRT_ERR("restart mcsi failed!\n");
		return ret;
	}

	v4l2_subdev_call(p->subdevs[IDX_SENSOR], core, reset, state);
	if (ret < 0 && ret != -ENOIOCTLCMD && ret != -ENODEV) {
		PRT_ERR("restart sensor failed!\n");
		return ret;
	}

	return ret;
}

/* Media pipeline operations for the VISS video device driver */
static const struct viss_pipeline_ops viss_pipeline_ops = {
	.open		= __viss_pipeline_open,
	.close		= __viss_pipeline_close,
	.set_stream	= __viss_pipeline_s_stream,
	.restart	= __viss_pipeline_restart,
};
static struct lombo_media_pipeline *viss_md_pipeline_create(
						struct viss_md *vmd)
{
	struct viss_pipeline *p;

	PRT_TRACE_BEGIN("\n");

	p = kzalloc(sizeof(*p), GFP_KERNEL);
	if (!p)
		return NULL;

	list_add_tail(&p->list, &vmd->pipelines);

	p->ep.ops = &viss_pipeline_ops;
	PRT_TRACE_END("\n");
	return &p->ep;
}

static void viss_md_pipelines_free(struct viss_md *vmd)
{
	while (!list_empty(&vmd->pipelines)) {
		struct viss_pipeline *p;

		p = list_entry(vmd->pipelines.next, typeof(*p), list);
		list_del(&p->list);
		kfree(p);
	}
}

static int __si_set_camclk(struct viss_source_info *si, bool on)
{

	if (si->if_type == VISS_IF_TYPE_MCSI)
		si->host_id = VISS_MCSI_SENSOR;
	else
		si->host_id = VISS_VIC_SENSOR;

	return __viss_md_set_camclk(si->host_data, si, on);
}

static int __viss_md_add_sensor(struct viss_md *vmd, struct v4l2_subdev *sd,
				int index)
{
	struct viss_sensor_info *si;
	struct viss_source_info *pdata;
	int ret;

	PRT_TRACE_BEGIN("index=%d\n", index);
	si = &vmd->sensor[index];

	if (!try_module_get(sd->owner)) {
		ret = -EPROBE_DEFER;
		PRT_ERR("No driver found for %s\n", sd->name);
		goto out;
	}

	pdata = &si->pdata;
	pdata->host_data = vmd;
	pdata->set_camclk = __si_set_camclk;

	v4l2_set_subdev_hostdata(sd, pdata);
	sd->grp_id = GRP_ID_SENSOR;

	ret = v4l2_device_register_subdev(&vmd->v4l2_dev, sd);
	if (ret < 0) {
		memset(pdata, 0, sizeof(*pdata));
		PRT_ERR("sd->name=%s\n", sd->name);
		goto mod_put;
	}

	if (pdata->if_type == VISS_IF_TYPE_MCSI)
		pdata->host_id = VISS_MCSI_SENSOR;
	else
		pdata->host_id = VISS_VIC_SENSOR;

	si->subdev = sd;

	PRT_INFO("Registered sensor subdevice: %s (%d)\n",
		  sd->name, vmd->num_sensors);
	vmd->num_sensors++;

mod_put:
	module_put(sd->owner);
out:
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_md_add_sensors(struct viss_md *vmd)
{
	struct v4l2_subdev *sd;
	int ret = 0;

	sd = cccore_get_dvp_sensor_subdev();
	if (sd != NULL)
		ret = __viss_md_add_sensor(vmd, sd, VISS_VIC_SENSOR);

	sd = cccore_get_mcsi_sensor_subdev();
	if (sd != NULL)
		ret = __viss_md_add_sensor(vmd, sd, VISS_MCSI_SENSOR);

	return 0;
}

static void viss_md_unregister_sensor(struct v4l2_subdev *sd)
{
	PRT_TRACE_BEGIN("\n");
	v4l2_device_unregister_subdev(sd);
	PRT_TRACE_END("\n");
}

static int viss_md_register_sensor_entities(struct viss_md *vmd)
{
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	/*
	 * Runtime resume one of the VISS entities to make sure
	 * the sclk_cam clocks are not globally disabled.
	 */
	if (WARN_ON(!vmd->pmf))
		return -ENXIO;

	ret = pm_runtime_get_sync(vmd->pmf);
	if (ret < 0) {
		PRT_ERR("\n");
		return ret;
	}

	vmd->num_sensors = 0;
	ret = viss_md_add_sensors(vmd);
	if (ret < 0) {
		PRT_ERR("\n");
		return ret;
	}

	pm_runtime_put(vmd->pmf);
	PRT_TRACE_END("\n");
	return ret;
}

/*
 * VISS-CSIS, VISS-VIC platform devices registration.
 */

static int register_viss_vic_entity(struct viss_md *vmd, struct viss_dev *vic)
{
	struct v4l2_subdev *sd;
	int ret;

	if (WARN_ON(vmd->vic))
		return -EBUSY;

	PRT_TRACE_BEGIN("\n");
	sd = &vic->subdev;
	sd->grp_id = GRP_ID_VIC;
	v4l2_set_subdev_hostdata(sd, (void *)&viss_pipeline_ops);

	ret = v4l2_device_register_subdev(&vmd->v4l2_dev, sd);
	if (!ret) {
		if (!vmd->pmf && vic->pdev)
			vmd->pmf = &vic->pdev->dev;
		vmd->vic = vic;
	} else
		PRT_ERR("Failed to register VISS.VIC (%d)\n", ret);
	PRT_TRACE_END("\n");
	return ret;
}

static int register_mcsi_entity(struct viss_md *vmd,
				     struct viss_dev *mcsi)
{
	struct v4l2_subdev *sd;
	int ret;

	if (WARN_ON(vmd->mcsi))
		return -EBUSY;

	PRT_TRACE_BEGIN("\n");
	sd = &mcsi->subdev;
	sd->grp_id = GRP_ID_MCSI;
	v4l2_set_subdev_hostdata(sd, (void *)&viss_pipeline_ops);

	ret = v4l2_device_register_subdev(&vmd->v4l2_dev, sd);
	if (!ret) {
		if (!vmd->pmf && mcsi->pdev)
			vmd->pmf = &mcsi->pdev->dev;
		vmd->mcsi = mcsi;
	} else
		PRT_ERR("Failed to register VISS.MCSI (%d)\n", ret);
	PRT_TRACE_END("\n");
	return ret;
}

static int register_viss_entity(struct viss_md *vmd, struct viss_is_dev *viss)
{
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (WARN_ON(viss->id >= VISS_ISP_NUM || vmd->viss[viss->id]))
		return -EBUSY;

	viss->pipeline_ops = &viss_pipeline_ops;

	ret = viss_register_video_device(viss, &vmd->v4l2_dev);
	if (!ret) {
		if (!vmd->pmf && viss->pdev)
			vmd->pmf = &viss->pdev->dev;
		vmd->viss[viss->id] = viss;
	} else {
		PRT_ERR("Failed to register VISS.%d\n", viss->id);
	}
	PRT_TRACE_END("\n");
	return ret;
}

static int register_viss_is_entity(struct viss_md *vmd, struct viss_is *is)
{
	struct v4l2_subdev *sd = &is->isp.subdev;
	struct lombo_media_pipeline *ep;
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (WARN_ON(is->isp.id >= VISS_ISP_NUM || vmd->viss_is[is->isp.id]))
		return -EBUSY;

	sd->grp_id = GRP_ID_VISS_IS;

	ep = viss_md_pipeline_create(vmd);
	if (!ep)
		return -ENOMEM;

	v4l2_set_subdev_hostdata(sd, ep);
	PRT_DBG("sd->name:%s\n", sd->name);
	ret = v4l2_device_register_subdev(&vmd->v4l2_dev, sd);
	if (ret) {
		PRT_ERR("Failed to register VISS-ISP%d\n", is->isp.id);
		return ret;
	}
	vmd->viss_is[is->isp.id] = is;
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_md_register_platform_entity(struct viss_md *vmd,
					    struct platform_device *pdev,
					    int plat_entity)
{
	struct device *dev = &pdev->dev;
	int ret = -EPROBE_DEFER;
	void *drvdata;

	PRT_TRACE_BEGIN("plat_entity=%d\n", plat_entity);
	/* Lock to ensure dev->driver won't change. */
	device_lock(dev);

	if (!dev->driver || !try_module_get(dev->driver->owner))
		goto dev_unlock;

	drvdata = dev_get_drvdata(dev);
	/* Some subdev didn't probe successfully id drvdata is NULL */
	if (drvdata) {
		switch (plat_entity) {
		case IDX_VIC:
			ret = register_viss_vic_entity(vmd, drvdata);
			break;
		case IDX_MCSI:
			ret = register_mcsi_entity(vmd, drvdata);
			break;
		case IDX_VISS:
			ret = register_viss_entity(vmd, drvdata);
			break;
		case IDX_IS_ISP:
			ret = register_viss_is_entity(vmd, drvdata);
			break;
		default:
			ret = -ENODEV;
		}
	}

	module_put(dev->driver->owner);
dev_unlock:
	device_unlock(dev);
	if (ret == -EPROBE_DEFER)
		PRT_INFO("deferring %s device registration\n", dev_name(dev));
	else if (ret < 0)
		PRT_ERR("%s device registration failed (%d)\n",
			dev_name(dev), ret);
	PRT_TRACE_END("\n");
	return ret;
}

/* Register VISS-VIC, and VISS-MCSI media entities */
#ifdef CONFIG_OF
static int viss_md_register_of_platform_entities(struct viss_md *vmd,
						 struct device_node *parent)
{
	struct device_node *node;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	PRT_DBG("parent->name:%s\n", parent->name);
	for_each_available_child_of_node(parent, node) {
		struct platform_device *pdev;
		int plat_entity = -1;

		pdev = of_find_device_by_node(node);
		PRT_DBG("node->name:%s pdev:0x%p\n", node->name, pdev);
		if (!pdev)
			continue;

		/* If driver of any entity isn't ready try all again later. */
		if (!strcmp(node->name, VISS_VIC_OF_NODE_NAME)) {
			PRT_DBG("IDX_VIC\n");
			plat_entity = IDX_VIC;
		} else if (!strcmp(node->name, VISS_MCSI_OF_NODE_NAME)) {
			PRT_DBG("IDX_MCSI\n");
			plat_entity = IDX_MCSI;
		} else if (!strcmp(node->name, VISS_OF_NODE_NAME)) {
			PRT_DBG("IDX_VISS\n");
			plat_entity = IDX_VISS;
		} else if (!strcmp(node->name, VISS_IS_OF_NODE_NAME)) {
			PRT_DBG("IDX_IS_ISP\n");
			plat_entity = IDX_IS_ISP;
		} else {
			PRT_ERR("UNKNOWN plat_entity %d\n", plat_entity);
		}

		if (plat_entity >= 0)
			ret = viss_md_register_platform_entity(vmd, pdev,
							plat_entity);
		put_device(&pdev->dev);
		if (ret < 0)
			break;
	}

	PRT_TRACE_END("\n");
	return ret;
}
#else
#define viss_md_register_of_platform_entities(vmd, node) (-ENOSYS)
#endif

static void viss_md_unregister_entities(struct viss_md *vmd)
{
	int i;

	PRT_TRACE_BEGIN("\n");
	for (i = 0; i < VISS_ISP_NUM; i++) {
		if (vmd->viss[i] == NULL)
			continue;
		viss_unregister_video_device(vmd->viss[i]);
		vmd->viss[i]->pipeline_ops = NULL;
		vmd->viss[i] = NULL;
	}
	if (vmd->vic != NULL) {
		v4l2_device_unregister_subdev(&vmd->vic->subdev);
		vmd->vic->pipeline_ops = NULL;
		vmd->vic = NULL;
	}

	if (vmd->mcsi != NULL) {
		v4l2_device_unregister_subdev(&vmd->mcsi->subdev);
		vmd->mcsi->pipeline_ops = NULL;
		vmd->mcsi = NULL;
	}

	for (i = 0; i < VISS_MAX_SENSORS; i++) {
		if (vmd->sensor[i].subdev == NULL)
			continue;
		viss_md_unregister_sensor(vmd->sensor[i].subdev);
		vmd->sensor[i].subdev = NULL;
	}
	for (i = 0; i < VISS_ISP_NUM; i++) {
		if (vmd->viss_is[i] == NULL)
			continue;
		v4l2_device_unregister_subdev(&vmd->viss_is[i]->isp.subdev);
	}
	PRT_DBG("Unregistered all entities\n");
	PRT_TRACE_END("\n");
}

static int __viss_md_create_sensor_links(struct viss_md *vmd)
{
	struct v4l2_subdev *sensor, *sd;
	struct viss_source_info *pdata;
	struct media_entity *source, *sink;
	int i, ret = 0;
	int src_pad, sink_pad;
	u32 flags = 0;

	PRT_TRACE_BEGIN("\n");
	for (i = 0; i < VISS_MAX_SENSORS; i++) {
		if (vmd->sensor[i].subdev == NULL)
			continue;

		sensor = vmd->sensor[i].subdev;
		pdata = v4l2_get_subdev_hostdata(sensor);
		if (!pdata)
			continue;

		source = &sensor->entity;

		switch (pdata->if_type) {
		case VISS_IF_TYPE_DC:
		case VISS_IF_TYPE_ITU_601:
		case VISS_IF_TYPE_ITU_656:
		case VISS_IF_TYPE_ITU_1120:
			sd = &vmd->vic->subdev;
			if (WARN(sd == NULL,
				 "VISS-VIC interface specified "
				 "but viss-vic module is not loaded!\n"))
				return -EINVAL;
			break;
		case VISS_IF_TYPE_MCSI:
			sd = &vmd->mcsi->subdev;
			if (WARN(sd == NULL,
				 "VISS-MCSI interface specified "
				 "but viss-mcsi module is not loaded!\n"))
				return -EINVAL;
			break;
		default:
			PRT_ERR("Wrong if_type (%x)\n", pdata->if_type);
			return -EINVAL;
		}

		sink = &sd->entity;
		sink_pad = VISS_SD_PAD_SINK;
		src_pad = 0;

		/* sensor and host is permanently one to one */
		flags = MEDIA_LNK_FL_IMMUTABLE | MEDIA_LNK_FL_ENABLED;
		ret = media_entity_create_link(source, src_pad, sink, sink_pad,
						flags);
		if (ret)
			return ret;

		/* Notify VIC/MCSI subdev entity */
		if (pdata->use_isp && vmd->viss_is[pdata->isp_id]) {
			ret = media_entity_call(sink, link_setup,
				&sink->pads[VISS_SD_PAD_SOURCE_ISP],
				&source->pads[src_pad], flags);
		}

		if (ret < 0)
			return ret;

		PRT_INFO("created link [%s] => [%s]\n",
				source->name, sink->name);
	}

	PRT_TRACE_END("\n");
	return ret;
}

static int __viss_md_create_viss_is_sink_links(struct viss_md *vmd,
		struct media_entity *source, int pad, u32 isp_id)
{
	struct media_entity *sink;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	sink = &vmd->viss_is[isp_id]->isp.subdev.entity;
	ret = media_entity_create_link(source, pad, sink,
			VISS_ISP_SD_PAD_SINK, MEDIA_LNK_FL_ENABLED);
	if (ret) {
		PRT_ERR("created link [%s] => [%s] failed(%d)\n",
				source->name, sink->name, ret);
		return ret;
	}
	/* Notify VISS capture subdev entity */
	ret = media_entity_call(sink, link_setup, &sink->pads[0],
				&source->pads[pad], MEDIA_LNK_FL_ENABLED);

	PRT_INFO("created link [%s] => [%s]\n", source->name, sink->name);
	PRT_TRACE_END("\n");
	return 0;
}

/* Create VISS-IS links */
static int __viss_md_create_viss_isp_links(struct viss_md *vmd, u32 isp_id)
{
	struct media_entity *source, *sink;
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (vmd->viss[isp_id] == NULL)
		PRT_WARN("vmd->viss[%d] is null\n", isp_id);

	source = &vmd->viss_is[isp_id]->isp.subdev.entity;
	/* Link from IS-ISP subdev to VISS */
	sink = &vmd->viss[isp_id]->vv[0].vfd.entity;

	ret = media_entity_create_link(source, VISS_ISP_SD_PAD_SRC_P0,
		sink, 0, MEDIA_LNK_FL_IMMUTABLE | MEDIA_LNK_FL_ENABLED);
	PRT_INFO("created link [%s]->[%s]\n", source->name, sink->name);
	if (ret)
		PRT_WARN("\n");


	sink = &vmd->viss[isp_id]->vv[1].vfd.entity;

	ret = media_entity_create_link(source, VISS_ISP_SD_PAD_SRC_P1,
		sink, 0, MEDIA_LNK_FL_IMMUTABLE | MEDIA_LNK_FL_ENABLED);
	PRT_INFO("created link [%s]->[%s]\n", source->name, sink->name);
	if (ret)
		PRT_WARN("\n");

	vmd->viss_is[isp_id]->viss = vmd->viss[isp_id];

	return 0;
}

static int __viss_md_create_host_links(struct viss_md *vmd,
					struct viss_dev *viss_dev,
					bool use_isp, u32 isp_id)
{
	struct media_entity *source, *sink;
	int ret = 0;
	u32 flags = 0;

	PRT_TRACE_BEGIN("\n");
	source = &viss_dev->subdev.entity;
	sink = &viss_dev->vfd.entity;

	/* Create immutable links between VIC/MCSI subdev and video node */
	flags = MEDIA_LNK_FL_IMMUTABLE | MEDIA_LNK_FL_ENABLED;
	ret = media_entity_create_link(source, VISS_SD_PAD_SOURCE_DMA,
				      sink, 0, flags);
	if (ret < 0)
		return ret;

	PRT_INFO("created link [%s] -> [%s]\n", source->name, sink->name);

	if (use_isp && vmd->viss_is[isp_id]) {
		/* Link from VIC/MCSI to VISS_IS subdev */
		ret = __viss_md_create_viss_is_sink_links(vmd, source,
					VISS_SD_PAD_SOURCE_ISP, isp_id);
		if (ret < 0) {
			PRT_ERR("Create viss is sink links failed(%d)!\n", ret);
			return ret;
		}
		ret = __viss_md_create_viss_isp_links(vmd, isp_id);
		if (ret < 0) {
			PRT_ERR("Create viss isp links failed(%d)!\n", ret);
			return ret;
		}

	}

	PRT_TRACE_END("\n");
	return ret;
}

/**
 * viss_md_create_links - create default links between registered entities
 *
 * Parallel interface sensor entities are connected directly to VISS-VIC
 * entities. MIPI-CSI2 interface sensor entities are connected directly to
 * VISS-CSIS. TVD interface sensor entities are connected directly to
 * VISS-TVD. Enabled links are created by default between each subsequent
 * registered sensor and subsequent VISS-XXX entity. The number of default
 * active links is determined by the number of available sensors or VISS-XXX
 * entities, whichever is less.
 */
static int viss_md_create_links(struct viss_md *vmd)
{
	int ret = 0;
	bool use_isp = false;
	u32 isp_id = 0;

	PRT_TRACE_BEGIN("\n");
	ret = __viss_md_create_sensor_links(vmd);
	if (ret < 0)
		return ret;

	if (vmd->mcsi != NULL) {
		use_isp = vmd->sensor[VISS_MCSI_SENSOR].pdata.use_isp;
		isp_id = vmd->sensor[VISS_MCSI_SENSOR].pdata.isp_id;

		if (use_isp && vmd->viss_is[isp_id])
			vmd->viss_is[isp_id]->srcif = IS_SOURCE_MCSI;
		ret = __viss_md_create_host_links(vmd, vmd->mcsi,
						use_isp, isp_id);
		if (ret < 0)
			return ret;
	}

	if (vmd->vic != NULL) {
		use_isp = vmd->sensor[VISS_VIC_SENSOR].pdata.use_isp;
		isp_id = vmd->sensor[VISS_VIC_SENSOR].pdata.isp_id;
		if (use_isp && vmd->viss_is[isp_id]) {
			if (vmd->viss_is[isp_id]->srcif == IS_SOURCE_MCSI) {
				PRT_ERR("link both VIC and MCSI to ISP!\n");
				return -EINVAL;
			}
			vmd->viss_is[isp_id]->srcif = IS_SOURCE_DVP;
		}
		ret = __viss_md_create_host_links(vmd, vmd->vic,
						use_isp, isp_id);
		if (ret < 0)
			return ret;
	}

	PRT_TRACE_END("\n");
	return ret;
}


static int viss_cfg_mclk(struct viss_md *vmd, u32 host_id,
		     unsigned long frequency)
{
	if (WARN_ON(host_id >= VISS_MAX_SENSORS))
		return -EINVAL;

	PRT_TRACE_BEGIN("host_id=%u,frequency=%lu\n", host_id, frequency);
	if (host_id == VISS_VIC_SENSOR) {
		/* VIC Interface */
		csp_viss_top_vic_mclk_init(frequency);
	} else {
		/* MCSI Interface */
		csp_viss_top_mcsi_mclk_init(frequency);
	}
	PRT_TRACE_END("\n");
	return 0;
}

static void viss_disable_mclk(struct viss_md *vmd, u32 host_id)
{
	PRT_TRACE_BEGIN("host_id=%d\n", host_id);

	if (host_id == VISS_VIC_SENSOR) {
		/* VIC Interface */
		csp_viss_top_vic_mclk_gate(0);
	} else {
		/* MCSI Interface */
		csp_viss_top_mcsi_mclk_gate(0);
	}

	PRT_TRACE_END("\n");
}

static int __viss_md_set_camclk(struct viss_md *vmd,
				struct viss_source_info *si,
				bool on)
{
	int ret = 0;

	if (WARN_ON(si->host_id >= VISS_MAX_SENSORS) || !vmd || !vmd->pmf)
		return -EINVAL;

	PRT_TRACE_BEGIN("%s\n", on ? "on" : "off");
	if (on) {
		if (si->vdd_1_8) {
			csp_viss_cam_ldo_enable();
			msleep(100);
		}
		ret = viss_cfg_mclk(vmd, si->host_id,
					    si->clk_frequency);
		return ret;
	}

	viss_disable_mclk(vmd, si->host_id);
	if (si->vdd_1_8)
		csp_viss_cam_ldo_disable();

	PRT_TRACE_END("\n");
	return ret;
}

/**
 * viss_md_set_camclk - peripheral sensor clock setup
 * @sd: sensor subdev to configure sclk_cam clock for
 * @on: 1 to enable or 0 to disable the clock
 *
 * There are 2 separate clock outputs available in the SoC for external
 * image processors. These clocks are shared between all registered VISS
 * devices to which sensors can be attached, either directly or through
 * the MIPI CSI receiver. The clock is allowed here to be used by
 * multiple sensors concurrently if they use same frequency.
 * This function should only be called when the graph mutex is held.
 */
int viss_md_set_camclk(struct v4l2_subdev *sd, bool on)
{
	struct viss_source_info *si = v4l2_get_subdev_hostdata(sd);
	struct viss_md *vmd = entity_to_viss_mdev(&sd->entity);

	return __viss_md_set_camclk(vmd, si, on);
}

static int viss_md_link_notify(struct media_pad *source,
			       struct media_pad *sink, u32 flags)
{
	struct viss_dev *viss_dev = NULL;
	struct viss_is_dev *viss = NULL;
	struct viss_pipeline *pipeline;
	struct v4l2_subdev *sd;
	struct mutex *lock;
	int i, ret = 0;
	int ref_count;

	if (media_entity_type(sink->entity) != MEDIA_ENT_T_V4L2_SUBDEV)
		return 0;

	PRT_TRACE_BEGIN("flags=0x%x\n", flags);
	sd = media_entity_to_v4l2_subdev(sink->entity);

	switch (sd->grp_id) {
	/* link from sensor to VIC/MCSI */
	case GRP_ID_VIC:
	case GRP_ID_MCSI:
		viss_dev = v4l2_get_subdevdata(sd);
		if (WARN_ON(viss_dev == NULL))
			return 0;
		pipeline = &viss_dev->pipeline;
		lock = &viss_dev->lock;
		break;
	/* link from VIC/MCSI to ISP/ISP-LITE */
	case GRP_ID_VISS:
		viss = v4l2_get_subdevdata(sd);
		if (WARN_ON(viss_dev == NULL))
			return 0;
		pipeline = &viss->pipeline;
		lock = &viss->lock;
		break;
	default:
		return 0;
	}

	mutex_lock(lock);

	if (viss_dev != NULL)
		ref_count = viss_dev->ref_count;

	if (!(flags & MEDIA_LNK_FL_ENABLED)) {
		if (ref_count > 0)
			ret = __viss_pipeline_close(pipeline);

		for (i = 0; i < IDX_MAX; i++)
			pipeline->subdevs[i] = NULL;
	} else if (ref_count > 0) {
		/*
		 * Link activation. Enable power of pipeline elements only if
		 * the pipeline is already in use, i.e. its video node is open.
		 * Recreate the controls destroyed during the link deactivation.
		 */
		ret = __viss_pipeline_open(pipeline, sink->entity, true);
	}

	mutex_unlock(lock);
	PRT_TRACE_END("\n");
	return ret ? -EPIPE : ret;
}

static ssize_t viss_md_sysfs_show(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_md *vmd = platform_get_drvdata(pdev);

	if (vmd->user_subdev_api)
		return strlcpy(buf, "Sub-device API (sub-dev)\n", PAGE_SIZE);

	return strlcpy(buf, "V4L2 video node only API (vid-dev)\n", PAGE_SIZE);
}

static ssize_t viss_md_sysfs_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_md *vmd = platform_get_drvdata(pdev);
	bool subdev_api;

	if (!strcmp(buf, "vid-dev\n"))
		subdev_api = false;
	else if (!strcmp(buf, "sub-dev\n"))
		subdev_api = true;
	else
		return count;

	vmd->user_subdev_api = subdev_api;
	if (vmd->vic)
		vmd->vic->user_subdev_api = subdev_api;
	if (vmd->mcsi)
		vmd->mcsi->user_subdev_api = subdev_api;

	return count;
}

/*
 * This device attribute is to select video pipeline configuration method.
 * There are following valid values:
 *  vid-dev - for V4L2 video node API only, subdevice will be configured
 *  by the host driver.
 *  sub-dev - for media controller API, subdevs must be configured in user
 *  space before starting streaming.
 */
static DEVICE_ATTR(subdev_conf_mode, S_IWUSR | S_IRUGO,
		   viss_md_sysfs_show, viss_md_sysfs_store);

static int viss_md_get_pinctrl(struct viss_md *vmd)
{
	struct device *dev = &vmd->pdev->dev;
	struct viss_pinctrl *pctl = &vmd->pinctl;

	pctl->pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR(pctl->pinctrl))
		return PTR_ERR(pctl->pinctrl);

	pctl->state_default = pinctrl_lookup_state(pctl->pinctrl,
					PINCTRL_STATE_DEFAULT);
	if (IS_ERR(pctl->state_default))
		return PTR_ERR(pctl->state_default);

	pctl->state_idle = pinctrl_lookup_state(pctl->pinctrl,
					PINCTRL_STATE_IDLE);

	return 0;
}

static void viss_md_config_child_freq(struct device_node *parent)
{
	struct device_node *node;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	for_each_available_child_of_node(parent, node) {
		struct platform_device *pdev;
		u32 freq = 0;

		pdev = of_find_device_by_node(node);
		PRT_DBG("node->name:%s pdev:0x%p\n", node->name, pdev);
		if (!pdev)
			continue;

		ret = of_property_read_u32(node, "clock-frequency", &freq);
		if (ret < 0)
			freq = 0;

		if (!strcmp(node->name, VISS_VIC_OF_NODE_NAME)) {
			if (freq < VIC_FREQ) {
				freq = VIC_FREQ;
				PRT_ERR("vic set as minimum %d\n", VIC_FREQ);
			}
			csp_viss_top_vic_init(freq);
		} else if (!strcmp(node->name, VISS_MCSI_OF_NODE_NAME)) {
			if (freq < MCSI_FREQ) {
				freq = MCSI_FREQ;
				PRT_ERR("mcsi set as minimum %d\n", MCSI_FREQ);
			}
			csp_viss_top_mcsi_init(freq);
		} else if (!strcmp(node->name, VISS_OF_NODE_NAME)) {
			if (freq < ISP_1080P_INPUT_FREQ) {
				freq = ISP_1080P_INPUT_FREQ;
				PRT_ERR("isp set as minimum %d\n",
				ISP_1080P_INPUT_FREQ);
			}
			csp_viss_top_isp_init(freq);
		}
	}

	PRT_TRACE_END("\n");
}

#ifdef CONFIG_ARCH_LOMBO_N7V1
static int viss_adpll_init(void)
{
	u32 wait_lock = 0;
	int ret = 0;

	csp_viss_top_adpll_init();
	wait_lock = ADPLL_LOCK_TIMEOUT / ADPLL_LOCK_CHK_INTVAL;
	while (wait_lock > 0) {
		if (csp_viss_top_adpll_lock())
			break;

		usleep_range(ADPLL_LOCK_CHK_INTVAL, ADPLL_LOCK_CHK_INTVAL + 10);
		wait_lock--;
	}

	if (!csp_viss_top_adpll_lock()) {
		PRT_ERR("csp_viss_top_adpll_lock failed!");
		ret = -1;
	}

	return ret;
}

static void viss_adpll_exit(void)
{
	csp_viss_top_adpll_disable();
}
#else
static int viss_adpll_init(void)
{
	return 0;
}

static void viss_adpll_exit(void)
{
}
#endif

static int viss_md_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct v4l2_device *v4l2_dev;
	struct viss_md *vmd;
	struct resource *res;
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (dev->of_node == NULL) {
		PRT_ERR("%s %d: no device data specified\n",
			__func__, __LINE__);
		return -ENOENT;
	}

	vmd = devm_kzalloc(dev, sizeof(*vmd), GFP_KERNEL);
	if (!vmd)
		return -ENOMEM;

	spin_lock_init(&vmd->slock);
	INIT_LIST_HEAD(&vmd->pipelines);
	vmd->pdev = pdev;

	strlcpy(vmd->media_dev.model, "LOMBO VISS",
		sizeof(vmd->media_dev.model));
	vmd->media_dev.link_notify = viss_md_link_notify;
	vmd->media_dev.dev = dev;

	v4l2_dev = &vmd->v4l2_dev;
	v4l2_dev->mdev = &vmd->media_dev;
	strlcpy(v4l2_dev->name, "viss-md", sizeof(v4l2_dev->name));

	vmd->use_isp = viss_md_is_isp_available(dev->of_node);
	PRT_DBG("vmd->use_isp:%d\n", vmd->use_isp);
	ret = v4l2_device_register(dev, &vmd->v4l2_dev);
	if (ret < 0) {
		PRT_ERR("Failed to register v4l2_device: %d\n", ret);
		return ret;
	}

	ret = media_device_register(&vmd->media_dev);
	if (ret < 0) {
		PRT_ERR("Failed to register media device: %d\n", ret);
		goto err_md;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	vmd->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(vmd->regs))
		return PTR_ERR(vmd->regs);
#if 0
	csp_viss_set_register_base(vmd->regs);
#endif
	csp_viss_top_set_register_base(vmd->regs);
	vmd->viss_reset = devm_clk_get(dev, "viss_reset");
	if (IS_ERR_OR_NULL(vmd->viss_reset)) {
		PRT_ERR("Failed to get clock viss_reset: %d\n", ret);
		ret = PTR_ERR(vmd->viss_reset);
		goto err_md;
	}

	vmd->viss_gate = devm_clk_get(dev, "viss_gate");
	if (IS_ERR_OR_NULL(vmd->viss_gate)) {
		PRT_ERR("Failed to get clock viss_gate: %d\n", ret);
		ret = PTR_ERR(vmd->viss_gate);
		goto err_md;
	}

	vmd->viss_axi_gate = devm_clk_get(dev, "viss_axi_gate");
	if (IS_ERR_OR_NULL(vmd->viss_axi_gate)) {
		PRT_ERR("Failed to get clock viss_axi_gate: %d\n", ret);
		ret = PTR_ERR(vmd->viss_axi_gate);
		goto err_md;
	}

	vmd->srcclk0 = devm_clk_get(dev, "viss_sclk0");
	if (IS_ERR_OR_NULL(vmd->srcclk0)) {
		PRT_ERR("Failed to get clock viss_sclk0: %d\n", ret);
		ret = PTR_ERR(vmd->srcclk0);
		goto err_md;
	}

	vmd->srcclk1 = devm_clk_get(dev, "viss_sclk1");
	if (IS_ERR_OR_NULL(vmd->srcclk1)) {
		PRT_ERR("Failed to get clock viss_sclk1: %d\n", ret);
		ret = PTR_ERR(vmd->srcclk1);
		goto err_md;
	}

	clk_prepare_enable(vmd->viss_reset);
	clk_prepare_enable(vmd->viss_gate);
	clk_prepare_enable(vmd->viss_axi_gate);
	clk_prepare_enable(vmd->srcclk0);
	clk_prepare_enable(vmd->srcclk1);
	viss_adpll_init();

	/* clock configurations in viss top for csis/vic/isp */
	viss_md_config_child_freq(dev->of_node);

	/* vmd->user_subdev_api = (dev->of_node != NULL); */
	/* 目前不使用userspace的API设置，使用驱动内部的。*/
	vmd->user_subdev_api = false;

	/* Protect the media graph while we're registering entities */
	mutex_lock(&vmd->media_dev.graph_mutex);

	ret = viss_md_get_pinctrl(vmd);
	if (ret < 0) {
#if 0
		if (ret != EPROBE_DEFER)
			PRT_ERR("Failed to get pinctrl: %d\n", ret);
		goto err_unlock;
#else
		PRT_WARN("Failed to get pinctrl: %d", ret);
#endif
	}

	ret = viss_md_register_of_platform_entities(vmd, dev->of_node);
	if (ret) {
		PRT_ERR("Failed to register entities (%d)\n", ret);
		goto err_unlock;
	}

	if (dev->platform_data || dev->of_node) {
		ret = viss_md_register_sensor_entities(vmd);
		if (ret) {
			PRT_ERR("Fail to register sensor entities(%d)\n",
				ret);
			goto err_unlock;
		}
	}

#if 0	/* TVD with no sensor */
	if (vmd->num_sensors == 0) {
		PRT_ERR("No sensor registered!\n");
		goto err_unlock;
	}
#endif

	ret = viss_md_create_links(vmd);
	if (ret) {
		PRT_ERR("Failed to create links (%d)\n", ret);
		goto err_unlock;
	}

	ret = v4l2_device_register_subdev_nodes(&vmd->v4l2_dev);
	if (ret) {
		PRT_ERR("Failed to register subdev (%d)\n", ret);
		goto err_unlock;
	}

	ret = device_create_file(&pdev->dev, &dev_attr_subdev_conf_mode);
	if (ret) {
		PRT_ERR("device_create_file failed (%d)\n", ret);
		goto err_unlock;
	}

	platform_set_drvdata(pdev, vmd);
	mutex_unlock(&vmd->media_dev.graph_mutex);
	return 0;

err_unlock:
	mutex_unlock(&vmd->media_dev.graph_mutex);
/* err_clk: */
	clk_disable_unprepare(vmd->viss_reset);
	clk_disable_unprepare(vmd->viss_gate);
	clk_disable_unprepare(vmd->viss_axi_gate);
	clk_disable_unprepare(vmd->srcclk0);
	clk_disable_unprepare(vmd->srcclk1);
	/* viss_md_put_clocks(vmd); */
	viss_md_unregister_entities(vmd);
	media_device_unregister(&vmd->media_dev);
err_md:
	v4l2_device_unregister(&vmd->v4l2_dev);
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_md_remove(struct platform_device *pdev)
{
	struct viss_md *vmd = platform_get_drvdata(pdev);
	struct viss_source_info *pd = NULL;
	u32 index = 0;

	if (!vmd)
		return 0;
	PRT_TRACE_BEGIN("\n");
	device_remove_file(&pdev->dev, &dev_attr_subdev_conf_mode);
	viss_md_unregister_entities(vmd);
	for (index = 0; index < VISS_MAX_SENSORS; index++) {
		pd = &vmd->sensor[index].pdata;
		if (pd->if_info != NULL) {
			kfree(pd->if_info);
			pd->if_info = NULL;
		}
	}

	viss_md_pipelines_free(vmd);
	media_device_unregister(&vmd->media_dev);
	viss_adpll_exit();
	/* viss_md_put_clocks(vmd); */
	clk_disable_unprepare(vmd->srcclk0);
	clk_disable_unprepare(vmd->srcclk1);
	PRT_TRACE_END("\n");
	return 0;
}

#ifdef CONFIG_PM_RUNTIME
static int media_runtime_resume(struct device *dev)
{
	/* TODO */
	return 0;
}

static int media_runtime_suspend(struct device *dev)
{
	return 0;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int media_resume(struct device *dev)
{
	struct viss_md *vmd = dev_get_drvdata(dev);

	clk_prepare_enable(vmd->viss_reset);
	clk_prepare_enable(vmd->viss_gate);
	clk_prepare_enable(vmd->viss_axi_gate);
	clk_prepare_enable(vmd->srcclk0);
	clk_prepare_enable(vmd->srcclk1);
	viss_adpll_init();

	csp_viss_top_isp_init(ISP_1080P_INPUT_FREQ);

	csp_viss_top_vic_init(VIC_FREQ);
	csp_viss_top_mcsi_init(MCSI_FREQ);
	return 0;
}

static int media_suspend(struct device *dev)
{
	struct viss_md *vmd = dev_get_drvdata(dev);

	clk_disable_unprepare(vmd->viss_reset);
	clk_disable_unprepare(vmd->viss_gate);
	clk_disable_unprepare(vmd->viss_axi_gate);
	viss_adpll_exit();
	clk_disable_unprepare(vmd->srcclk0);
	clk_disable_unprepare(vmd->srcclk1);
	return 0;
}

static const struct dev_pm_ops media_pm_ops = {
	/* SET_SYSTEM_SLEEP_PM_OPS(media_suspend, media_resume) */
	.suspend_late = media_suspend,
	.resume_early = media_resume,
	SET_RUNTIME_PM_OPS(media_runtime_suspend, media_runtime_resume,
			   NULL)
};
#endif /* CONFIG_PM_SLEEP */

static struct platform_device_id viss_driver_ids[] __always_unused = {
	{ .name = "s5p-viss-md" },
	{ },
};
MODULE_DEVICE_TABLE(platform, viss_driver_ids);

static const struct of_device_id viss_md_of_match[] = {
	{ .compatible = "lombo,viss" },
	{ },
};
MODULE_DEVICE_TABLE(of, viss_md_of_match);

static struct platform_driver viss_md_driver = {
	.probe		= viss_md_probe,
	.remove		= viss_md_remove,
	.id_table	= viss_driver_ids,
	.driver = {
		.of_match_table = of_match_ptr(viss_md_of_match),
		.name		= "viss-md",
		.owner		= THIS_MODULE,
		.pm		= &media_pm_ops,
	}
};

static int __init viss_md_init(void)
{
	return platform_driver_register(&viss_md_driver);
}

static void __exit viss_md_exit(void)
{
	platform_driver_unregister(&viss_md_driver);
}

late_initcall(viss_md_init);
module_exit(viss_md_exit);

MODULE_AUTHOR("LomboTech Co.Ltd");
MODULE_DESCRIPTION("LomboTech VISS postprocessor driver");
MODULE_LICENSE("GPL");
