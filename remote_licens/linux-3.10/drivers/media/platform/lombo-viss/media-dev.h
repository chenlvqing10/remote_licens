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

#ifndef VISS_MDEVICE_H_
#define VISS_MDEVICE_H_

#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/pinctrl/consumer.h>
#include <media/media-device.h>
#include <media/media-entity.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/lombo_viss.h>

#include "viss-isp-preview.h"
#include "viss-isp.h"

#include "viss.h"
#include "viss-vic.h"
#if 0
#include "viss-mcsi.h"
#endif

enum {
	VISS_VIC_SENSOR,
	VISS_MCSI_SENSOR,
	VISS_MAX_SENSORS
};

#define VISS_MAX_SRCCLKS		2

#define VISS_OF_NODE_NAME		"viss"
#define VISS_LITE_OF_NODE_NAME		"viss-lite"
#define VISS_IS_OF_NODE_NAME		"viss-is"
#define CSIS_OF_NODE_NAME		"csis"
#define VISS_VIC_OF_NODE_NAME		"vic"
#define VISS_MCSI_OF_NODE_NAME		"mipi-csi"
#define VISS_ISP_OF_NODE_NAME		"isp"
#define VISS_ISP_LITE_OF_NODE_NAME	"isp-lite"

#define PINCTRL_STATE_IDLE	"idle"

#define VISS_WARN_MINMAX	0
#define VISS_WARN_PROBE_DEF	1
#define VISS_WARN_XU_GET_RES	2

/* LCD/ISP Writeback clocks (PIXELASYNCMx) */
enum {
	CLK_IDX_WB_A,
	CLK_IDX_WB_B,
	VISS_MAX_WBCLKS
};

struct viss_csis_info {
	struct v4l2_subdev *sd;
	int id;
};
/**
 * struct viss_sensor_info - image data source subdev information
 * @pdata: sensor's atrributes passed as media device's platform data
 * @subdev: image sensor v4l2 subdev
 * @host: viss device the sensor is currently linked to
 *
 * This data structure applies to image sensor and the writeback subdevs.
 */
struct viss_sensor_info {
	struct viss_source_info pdata;
	struct v4l2_subdev *subdev;
	struct viss_is_dev *host;
};

/**
 * struct viss_md - viss media device information
 * @csis: MIPI CSIS subdevs data
 * @sensor: array of registered sensor subdevs
 * @num_sensors: actual number of registered sensors
 * @camclk: external sensor clock information
 * @viss: array of registered viss devices
 * @viss_is: viss-is data structure
 * @use_isp: set to true when VISS-IS subsystem is used
 * @pmf: handle to the CAMCLK clock control VISS helper device
 * @media_dev: top level media device
 * @v4l2_dev: top level v4l2_device holding up the subdevs
 * @pdev: platform device this media device is hooked up into
 * @pinctrl: camera port pinctrl handle
 * @state_default: pinctrl default state handle
 * @state_idle: pinctrl idle state handle
 * @user_subdev_api: true if subdevs are not configured by the host driver
 * @slock: spinlock protecting @sensor array
 */
struct viss_md {
	struct viss_sensor_info sensor[VISS_MAX_SENSORS];
	int num_sensors;
	struct clk	*srcclk0;
	struct clk	*srcclk1;
	struct clk	*wbclk[VISS_MAX_WBCLKS];
	struct clk	*viss_reset;
	struct clk	*viss_gate;
	struct clk	*viss_axi_gate;
	void __iomem	*regs;
	struct viss_dev	*vic;
	struct viss_dev	*mcsi;
	/* TODO: ISP/ISP_LITE */
	struct viss_is_dev *viss[VISS_ISP_NUM];
	struct viss_is *viss_is[VISS_ISP_NUM];
	bool use_isp;
	struct device *pmf;
	struct media_device media_dev;
	struct v4l2_device v4l2_dev;
	struct platform_device *pdev;
	struct viss_pinctrl {
		struct pinctrl *pinctrl;
		struct pinctrl_state *state_default;
		struct pinctrl_state *state_idle;
	} pinctl;
	bool user_subdev_api;
	spinlock_t slock;
	struct list_head pipelines;
};

#define is_subdev_pad(pad) (pad == NULL || \
	media_entity_type(pad->entity) == MEDIA_ENT_T_V4L2_SUBDEV)

#define me_subtype(me) \
	((me->type) & (MEDIA_ENT_TYPE_MASK | MEDIA_ENT_SUBTYPE_MASK))

#define subdev_has_devnode(__sd) (__sd->flags & V4L2_SUBDEV_FL_HAS_DEVNODE)

static inline
struct viss_sensor_info *source_to_sensor_info(struct viss_source_info *si)
{
	return container_of(si, struct viss_sensor_info, pdata);
}

static inline struct viss_md *entity_to_viss_mdev(struct media_entity *me)
{
	return me->parent == NULL ? NULL :
		container_of(me->parent, struct viss_md, media_dev);
}

static inline void viss_md_graph_lock(struct viss_dev *viss)
{
	mutex_lock(&viss->vfd.entity.parent->graph_mutex);
}

static inline void viss_md_graph_unlock(struct viss_dev *viss)
{
	mutex_unlock(&viss->vfd.entity.parent->graph_mutex);
}

int viss_md_set_camclk(struct v4l2_subdev *sd, bool on);

struct media_entity *viss_pipeline_get_head(struct media_entity *me);

#ifdef CONFIG_OF
static inline bool viss_md_is_isp_available(struct device_node *node)
{
	node = of_get_child_by_name(node, VISS_IS_OF_NODE_NAME);
	return node ? of_device_is_available(node) : false;
}
#else
#define viss_md_is_isp_available(node) (false)
#endif /* CONFIG_OF */

#endif
