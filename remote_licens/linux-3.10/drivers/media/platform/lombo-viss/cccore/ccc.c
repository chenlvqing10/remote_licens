/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Camera Control Core driver code for LomboTech
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>

#include <media/v4l2-subdev.h>
#include <media/lombo_viss.h>

#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/viss_isp.h>

/* #define ENABLE_TRACE */
#include <mach/debug.h>

#include <mach/csp.h>

#include "ccc.h"
#include "ir_cut.h"

#define subdev_to_sensor_info(sd)	v4l2_get_subdevdata(sd)
#define to_sensor_info(sensor)	\
		container_of(sensor, struct cc_sensor_info, list)

struct cccore *g_ccc;

static int cc_sensor_power_up(struct cc_sensor_info *cc_si);
static int cc_sensor_power_down(struct cc_sensor_info *cc_si);

/*
 * parser sensor configures from fdt
 */
static int cc_parse_sensor_node(struct platform_device *pdev,
				struct cc_sensor_info *cc_si)
{
	struct device_node *node = pdev->dev.of_node;
	u32 args[BT601_TIMING_PARA_CNT];
	u32 val;
	int len;
	int ret = 0;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	if (of_property_read_u32(node, "i2c_bus", &val)) {
		PRT_ERR("No i2c_bus specified at node %s\n",
			node->full_name);
		return -EINVAL;
	}
	cc_si->i2c_bus_num = (u16)val;

	if (of_property_read_u32(node, "i2c_addr", &val)) {
		PRT_ERR("No i2c_addr specified at node %s\n",
			node->full_name);
		return -EINVAL;
	}
	cc_si->i2c_dev_addr = (u16)val;

	if (!of_property_read_u32(node, "i2c_ext_addr", &val))
		cc_si->i2c_ext_addr = (u16)val;
	else
		cc_si->i2c_ext_addr = 0;

	if (!of_property_read_u32(node, "clock-frequency", &val))
		cc_si->clk_frequency = val;

	PRT_DBG("cc_si->clk_frequency: %u, node->name: %s\n",
		cc_si->clk_frequency, node->name);

	if (cc_si->clk_frequency == 0) {
		PRT_ERR("Wrong clock frequency at node %s\n",
			node->full_name);
		return -EINVAL;
	}

	if (of_find_property(node, "lombo,vdd_1_8", &len))
		cc_si->vdd_1_8 = true;
	else
		cc_si->vdd_1_8 = false;

	if (of_find_property(node, "vflip", &len))
		cc_si->vflip = true;
	else
		cc_si->vflip = false;

	if (of_find_property(node, "mirror", &len))
		cc_si->mirror = true;
	else
		cc_si->mirror = false;

	if (!of_property_read_u32(node, "isp_path", &val)) {
		if (val >= 4)
			return -EINVAL;
		cc_si->isp_path = val;
	}
	PRT_INFO("isp_path: %d\n", cc_si->isp_path);

	if (!of_property_read_u32(node, "isp_id", &val)) {
		cc_si->isp_id = val;
		PRT_INFO("isp_id: %d\n", cc_si->isp_id);
	}

	if (of_find_property(node, "interlaced", &len))
		cc_si->interlaced = 1;

	if (!of_property_read_u32(node, "sort", &val))
		cc_si->sort = val;

	if (of_property_read_u32(node, "ch_count", &val)) {
		PRT_DBG("Use channel 1 by default\n");
		cc_si->ch_number = 0;
	} else {
		if ((val > 4) || (val < 1)) {
			PRT_ERR("%d channel is not configured\n", val);
			return -EINVAL;
		}
		cc_si->ch_number = val - 1;
		PRT_INFO("Use channels 1 to %d\n", val);
	}

	if (of_property_read_u32(node, "if-type", &val)) {
		PRT_ERR("No if-type specified at node %s\n",
			node->full_name);
		return -EINVAL;
	}

	if ((val != VISS_IF_TYPE_DC) &&
	    (val != VISS_IF_TYPE_ITU_601) &&
	    (val != VISS_IF_TYPE_ITU_656) &&
	    (val != VISS_IF_TYPE_ITU_1120) &&
	    (val != VISS_IF_TYPE_MCSI)) {
		PRT_ERR("Wrong if-type (%d) at node %s\n",
			val, node->full_name);
		return -EINVAL;
	}

	cc_si->if_type = val;
	PRT_DBG("if-type (%d) at node %s\n", val, node->full_name);

	of_property_read_u32(node, "if-mode", &cc_si->if_mode);

	if (cc_si->if_type != VISS_IF_TYPE_MCSI) {
		unsigned int flags = 0;
		u32 v;

		if (!of_property_read_u32(node, "hsync-active", &v))
			flags |= v ? V4L2_MBUS_HSYNC_ACTIVE_HIGH :
				V4L2_MBUS_HSYNC_ACTIVE_LOW;

		if (!of_property_read_u32(node, "vsync-active", &v))
			flags |= v ? V4L2_MBUS_VSYNC_ACTIVE_HIGH :
				V4L2_MBUS_VSYNC_ACTIVE_LOW;

		if (!of_property_read_u32(node, "pclk-sample", &v))
			flags |= v ? V4L2_MBUS_PCLK_SAMPLE_RISING :
				V4L2_MBUS_PCLK_SAMPLE_FALLING;

		if (!of_property_read_u32(node, "field-even-active", &v))
			flags |= v ? V4L2_MBUS_FIELD_EVEN_HIGH :
				V4L2_MBUS_FIELD_EVEN_LOW;

		if (!of_property_read_u32(node, "data-active", &v))
			flags |= v ? V4L2_MBUS_DATA_ACTIVE_HIGH :
				V4L2_MBUS_DATA_ACTIVE_LOW;

		cc_si->flags = flags;

		cc_si->pclk_polarity = false;
		if (of_find_property(node, "pclk-vary", &len))
			cc_si->pclk_polarity = true;
		else
			cc_si->pclk_polarity = false;

		cc_si->vsync_polarity = false;
		if (of_find_property(node, "vsync-vary", &len))
			cc_si->vsync_polarity = true;
		else
			cc_si->vsync_polarity = false;
	}

	if (cc_si->if_type == VISS_IF_TYPE_ITU_601) {
		struct bt601_timing *bt601_timing = NULL;

		ret = of_property_read_u32_array(node, "timing",
					args, BT601_TIMING_PARA_CNT);
		if (ret != 0) {
			PRT_ERR("Can't get timing at node %s\n",
				node->full_name);
			return -EINVAL;
		}

		bt601_timing = &cc_si->if_info.bt601_timing;

		bt601_timing->h_off		= args[0];
		bt601_timing->active_w		= args[1];
		bt601_timing->t_off		= args[2];
		bt601_timing->t_active_l	= args[3];
		bt601_timing->b_off		= args[4];
		bt601_timing->b_active_l	= args[5];
	}

	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

static int cc_i2c_bus_add_sensor(struct cc_sensor_info *cc_si)
{
	struct i2c_adapter *adapter = NULL;
	struct i2c_client *client = NULL;
	struct i2c_board_info info;
	int ret = 0;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	adapter = i2c_get_adapter(cc_si->i2c_bus_num);
	if (adapter == NULL) {
		PRT_ERR("Can't find i2c adapter %d\n", cc_si->i2c_bus_num);
		ret = -ENODEV;
		goto out;
	}

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		PRT_ERR("i2c adapter %d no functionality\n",
			cc_si->i2c_bus_num);
		ret = -ENODEV;
		goto out;
	}

	memset(&info, 0, sizeof(struct i2c_board_info));
	strncpy(info.type, cc_si->name, I2C_NAME_SIZE);
	info.addr = cc_si->i2c_dev_addr;

	client = i2c_new_device(adapter, &info);
	if (client == NULL) {
		PRT_ERR("Can't add dev 0x%x to adapter %d\n",
			cc_si->i2c_dev_addr, cc_si->i2c_bus_num);
		i2c_put_adapter(adapter);
		ret = -ENODEV;
		goto out;
	}

	client->flags = I2C_CLIENT_SCCB;

	cc_si->client = client;

	PRT_INFO("chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

	if (cc_si->i2c_ext_addr == 0)
		goto out;

	cc_si->ext_client[0] = client;

	memset(&info, 0, sizeof(struct i2c_board_info));
	strncpy(info.type, cc_si->ext_name, I2C_NAME_SIZE);
	info.addr = cc_si->i2c_ext_addr;

	client = NULL;
	client = i2c_new_device(adapter, &info);
	if (client == NULL) {
		PRT_ERR("Can't add dev 0x%x to adapter %d\n",
			cc_si->i2c_ext_addr, cc_si->i2c_bus_num);
		i2c_put_adapter(adapter);
		ret = -ENODEV;
		goto out;
	}

	client->flags = I2C_CLIENT_SCCB;

	cc_si->ext_client[1] = client;

	PRT_INFO("chip found @ 0x%02x (%s)\n",
		client->addr << 1, client->adapter->name);

out:
	PRT_TRACE_END("%s\n", cc_si->name);
	return ret;
}

static void cc_i2c_bus_remove_sensor(struct cc_sensor_info *cc_si)
{
	struct i2c_adapter *adapter = cc_si->client->adapter;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	if (cc_si->i2c_ext_addr == 0) {
		i2c_unregister_device(cc_si->client);
	} else {
		if (cc_si->ext_client[0] != NULL)
			i2c_unregister_device(cc_si->ext_client[0]);
		if (cc_si->ext_client[1] != NULL)
			i2c_unregister_device(cc_si->ext_client[1]);
		cc_si->ext_client[0] = NULL;
		cc_si->ext_client[1] = NULL;
	}
	i2c_put_adapter(adapter);
	cc_si->client = NULL;
	PRT_TRACE_END("%s\n", cc_si->name);
}

static int cc_register_sensor(struct cc_sensor_info *cc_si)
{
	struct cccore *ccc = g_ccc;
	u32 cc_if = CC_IF_DVP;
	int ret = 0;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	if (cc_si->if_type == VISS_IF_TYPE_MCSI)
		cc_if = CC_IF_MCSI;

	list_add_tail(&cc_si->list, &ccc->sensor_list[cc_if]);

	PRT_DBG("%s if_type %d cc_if %d\n", cc_si->name, cc_si->if_type, cc_if);

	cc_si->reset_pin = ccc->reset_pin[cc_if];
	cc_si->pwdn_pin	= ccc->pwdn_pin[cc_if];
	cc_si->led_pin = ccc->led_pin;
	cc_si->cam_en_pin = ccc->cam_en_pin[cc_if];

	cc_si->power_up  = cc_sensor_power_up;
	cc_si->power_down  = cc_sensor_power_down;

	ret = cc_set_reg_ops(cc_si);

	PRT_TRACE_END("%s\n", cc_si->name);
	return ret;
}

static void cc_unregister_sensor(struct cc_sensor_info *cc_si)
{
	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	list_del(&cc_si->list);
	PRT_TRACE_END("%s\n", cc_si->name);
}

void cc_switch_i2c_dev(struct cc_sensor_info *cc_si, u32 id)
{
	int ret = -1;
	if (cc_si->i2c_ext_addr == 0)
		return;

	PRT_INFO("id=%d\n", id);

	cc_si->client = cc_si->ext_client[!!id];

	ret = cc_set_reg_ops(cc_si);
	if (ret)
		PRT_ERR("set reg_bit or val_bit err\n");
}
EXPORT_SYMBOL(cc_switch_i2c_dev);

static int cc_sensor_power_up(struct cc_sensor_info *cc_si)
{
	int i;
	int ret = 0;

	if (cc_si->cam_en_pin != -1)
		gpio_direction_output(cc_si->cam_en_pin, 1);

	if ((cc_si->reset_pin == -1) && (cc_si->pwdn_pin == -1))
		goto out;

	if (cc_si->power_up_act == NULL)
		goto out;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	for (i = 0; i < cc_si->npwr_up_acts; i++) {
		switch (cc_si->power_up_act[i].act) {
		case CC_RESET:
			if (cc_si->reset_pin == -1) {
				PRT_ERR("no reset pin for sensor %s!\n",
					cc_si->name);
			}
			gpio_direction_output(cc_si->reset_pin,
						cc_si->power_up_act[i].ud);
			break;
		case CC_PWDN:
			if (cc_si->pwdn_pin == -1) {
				PRT_ERR("no pwdn pin for sensor %s!\n",
					cc_si->name);
			}
			gpio_direction_output(cc_si->pwdn_pin,
						cc_si->power_up_act[i].ud);
			break;
		case CC_DELAY:
			sensor_msleep(cc_si->power_up_act[i].ud);
			break;
		default:
			PRT_ERR("Invalid power up sequence of sensor %s!\n",
				cc_si->name);
			ret = -EINVAL;
			break;
		}
	}
	if (cc_si->led_pin != -1)
		gpio_direction_output(cc_si->led_pin, 1);

out:
	cc_si->power = 1;
	PRT_TRACE_END("%s\n", cc_si->name);
	return ret;
}

static int cc_sensor_power_down(struct cc_sensor_info *cc_si)
{
	int i;
	int ret = 0;

	if ((cc_si->reset_pin == -1) && (cc_si->pwdn_pin == -1))
		goto out;

	if (cc_si->power_down_act == NULL)
		goto out;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	for (i = 0; i < cc_si->npwr_dn_acts; i++) {
		switch (cc_si->power_down_act[i].act) {
		case CC_RESET:
			if (cc_si->reset_pin == -1) {
				PRT_ERR("no reset pin for sensor %s!\n",
					cc_si->name);
			}
			gpio_direction_output(cc_si->reset_pin,
						cc_si->power_down_act[i].ud);
			break;
		case CC_PWDN:
			if (cc_si->pwdn_pin == -1) {
				PRT_ERR("no pwdn pin for sensor %s!\n",
					cc_si->name);
			}
			gpio_direction_output(cc_si->pwdn_pin,
						cc_si->power_down_act[i].ud);
			break;
		case CC_DELAY:
			sensor_msleep(cc_si->power_down_act[i].ud);
			break;
		default:
			PRT_ERR("Invalid power down sequence of sensor %s!\n",
				cc_si->name);
			ret = -EINVAL;
			break;
		}
	}

	if (cc_si->cam_en_pin != -1)
		gpio_direction_output(cc_si->cam_en_pin, 1);

out:
	cc_si->power = 0;
	PRT_TRACE_END("%s\n", cc_si->name);
	return ret;
}

int cc_sensor_detect(struct cc_sensor_info *cc_si)
{
	return cc_si->detect(cc_si);
}

static int cc_try_fmt_internal(struct v4l2_subdev *sd,
				struct v4l2_mbus_framefmt *fmt,
				const struct cc_frame_format **ret_fmt,
				const struct cc_win_size **ret_wsize)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	const struct cc_frame_format *frmfmt = NULL;
	const struct cc_win_size *win_size = NULL;
	int index;

	if (cc_si == NULL)
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	for (index = 0; index < cc_si->nfrmfmts; index++)
		if (cc_si->framefmts[index].mbus_code == fmt->code)
			break;

	if (index >= cc_si->nfrmfmts) {
		/* default to first format */
		index = 0;
		fmt->code = cc_si->framefmts[0].mbus_code;
	}

	frmfmt = cc_si->framefmts + index;

	if (ret_fmt != NULL)
		*ret_fmt = frmfmt;

	/*
	 * Round requested image size up to the nearest we support,
	 * but not over the largest.
	 * (assume that win size is list from small to large)
	 */
	for (index = 0; index < cc_si->nwsizes; index++) {
		int r1, r2;
		win_size = &cc_si->win_sizes[index];
		r1 = win_size->width * 10 / win_size->height;
		r2 = fmt->width * 10 / fmt->height;
		if ((win_size->width >= fmt->width) &&
			(win_size->height >= fmt->height) &&
			(r1 == r2))
			break;
	}

	if (index >= cc_si->nwsizes)
		win_size = &cc_si->win_sizes[cc_si->nwsizes - 1];

	if (ret_wsize != NULL)
		*ret_wsize = win_size;

	/*
	 * Note the size we'll actually handle.
	 */
	fmt->width = win_size->width;
	fmt->height = win_size->height;
	fmt->colorspace = frmfmt->colorspace;
	fmt->field = frmfmt->field;

	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

/*
 * Frame size enumeration
 */
static int cc_enum_framesizes(struct v4l2_subdev *sd,
			struct v4l2_frmsizeenum *fsize)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	int i;
	int num_valid = -1;
	__u32 index = fsize->index;
	unsigned int n_win_sizes = cc_si->nwsizes;

	/*
	 * If a minimum width/height was requested, filter out the capture
	 * windows that fall outside that.
	 */
	for (i = 0; i < n_win_sizes; i++) {
		const struct cc_win_size *win = &cc_si->win_sizes[i];
		if (index == ++num_valid) {
			fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
			fsize->discrete.width = win->width;
			fsize->discrete.height = win->height;
			return 0;
		}
	}

	return -EINVAL;
}

static int cc_enum_frameintervals(struct v4l2_subdev *sd,
		struct v4l2_frmivalenum *interval)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	const struct cc_frame_cfg *framecfg = NULL;
	u32 pref_width = 0;
	u32 pref_height = 0;
	int fidx = 0;
	int midx = -1;

	if (interval->index >= cc_si->nfrmcfgs)
		return -EINVAL;

	for (fidx = 0; fidx < cc_si->nfrmcfgs; fidx++) {
		framecfg = &cc_si->framecfgs[fidx];
		if ((interval->width > framecfg->width) ||
			(interval->height > framecfg->height))
			continue;

		if (pref_width == 0) {
			pref_width = framecfg->width;
			pref_height = framecfg->height;
		} else if ((pref_width != framecfg->width) ||
			(pref_height != framecfg->height)) {
			break;
		}

		midx++;
		if (midx == interval->index) {
			interval->discrete = framecfg->framerate;
			return 0;
		}
	}

	return -EINVAL;
}

#if 0
static int cc_g_frame_interval(struct v4l2_subdev *sd,
				   struct v4l2_subdev_frame_interval *fi)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);

	mutex_lock(&cc_si->lock);
	fi->interval = cc_si->cur_frmcfg->framerate;
	mutex_unlock(&cc_si->lock);

	return 0;
}

static int cc_s_frame_interval(struct v4l2_subdev *sd,
				   struct v4l2_subdev_frame_interval *fi)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	const struct cc_frame_cfg *framecfg = NULL;
	const struct cc_win_size *cur_wsize = NULL;
	int fidx = 0;
	int ret = 0;

	cur_wsize = cc_si->cur_wsize;

	for (fidx = 0; fidx < cc_si->nfrmcfgs; fidx++) {
		framecfg = &cc_si->framecfgs[fidx];
		if ((cur_wsize->width > framecfg->width) ||
			(cur_wsize->height > framecfg->height)) {
			ret = -EINVAL;
			break;
		}

		if ((cur_wsize->width != framecfg->width) ||
			(cur_wsize->height != framecfg->height))
			continue;

		if ((fi->interval.numerator ==
			framecfg->framerate.numerator) &&
			(fi->interval.denominator ==
			framecfg->framerate.denominator)) {

			cc_si->cur_frmcfg = framecfg;
			break;
		}
	}

	return ret;
}
#endif

/*
 * Implement G/S_PARM.  There is a "high quality" mode we could try
 * to do someday; for now, we just do the frame rate tweak.
 */
static int cc_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	struct v4l2_captureparm *cp = &parms->parm.capture;

	if ((parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) &&
		(parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)) {
		PRT_ERR("unsupport type: %d\n", parms->type);
		return -EINVAL;
	}

	memset(cp, 0, sizeof(struct v4l2_captureparm));
	cp->capability = V4L2_CAP_TIMEPERFRAME;
	cp->timeperframe = cc_si->cur_frmcfg->framerate;

	return 0;
}

static int cc_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	struct v4l2_captureparm *cp = &parms->parm.capture;
	struct v4l2_fract *tpf = &cp->timeperframe;
	const struct cc_frame_cfg *framecfg = NULL;
	const struct cc_win_size *cur_wsize = NULL;
	u32 frmrate = 0;
	u32 exp_frmrate = 0;
	int fidx = 0;
	int ret = 0;

	if ((parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) &&
		(parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)) {
		PRT_ERR("unsupport type: %d\n", parms->type);
		return -EINVAL;
	}
	if (cp->extendedmode != 0)
		return -EINVAL;

	cur_wsize = cc_si->cur_wsize;
	exp_frmrate = tpf->denominator * 1000 / tpf->numerator;
	for (fidx = 0; fidx < cc_si->nfrmcfgs; fidx++) {
		framecfg = &cc_si->framecfgs[fidx];
		if ((cur_wsize->width < framecfg->width) ||
			(cur_wsize->height < framecfg->height)) {
			ret = -EINVAL;
			break;
		}

		if ((cur_wsize->width != framecfg->width) ||
			(cur_wsize->height != framecfg->height))
			continue;

		frmrate = framecfg->framerate.denominator * 1000 /
			  framecfg->framerate.numerator;

		if (((exp_frmrate > frmrate) &&
			(exp_frmrate - frmrate > 10)) ||
			((exp_frmrate < frmrate) &&
			(frmrate - exp_frmrate > 10)))
			continue;

		cc_si->cur_frmcfg = framecfg;
		break;
	}

	if ((ret == 0) && (cc_si->if_type == VISS_IF_TYPE_MCSI)) {
		struct mcsi_if_info *mcsi_if = NULL;

		mcsi_if = &cc_si->if_info.mcsi_if_info;

		mcsi_if->mfreq = cc_si->cur_frmcfg->mfreq;
	}

	return ret;
}

/*
 * mbus code enumeration
 */
static int cc_pad_enum_mbus_code(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_mbus_code_enum *code)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	__u32 index = code->index;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	if (index >= cc_si->nfrmfmts)
		return -EINVAL;

	code->code = cc_si->framefmts[index].mbus_code;

	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

#if 0
/*
 * Frame size enumeration
 */
static int cc_pad_enum_frame_sizes(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_frame_size_enum *fse)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	int i;
	int num_valid = -1;
	__u32 index = fse->index;
	unsigned int n_win_sizes = cc_si->nwsizes;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	/*
	 * If a minimum width/height was requested, filter out the capture
	 * windows that fall outside that.
	 */
	for (i = 0; i < n_win_sizes; i++) {
		const struct cc_win_size *win = &cc_si->win_sizes[i];
		if (index == ++num_valid) {
			fse->min_width = win->width;
			fse->max_width = fse->min_width;
			fse->min_height = win->height;
			fse->max_height = fse->min_height;

			PRT_TRACE_END("%s\n", cc_si->name);
			return 0;
		}
	}

	PRT_TRACE_END("%s\n", cc_si->name);
	return -EINVAL;
}

static int cc_pad_enum_frame_interval(struct v4l2_subdev *sd,
			      struct v4l2_subdev_fh *fh,
			      struct v4l2_subdev_frame_interval_enum *fie)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	const struct cc_frame_cfg *framecfg = NULL;
	int fidx = 0;
	int midx = -1;

	if (fie->index >= cc_si->nfrmcfgs)
		return -EINVAL;

	for (fidx = 0; fidx < cc_si->nfrmcfgs; fidx++) {
		framecfg = &cc_si->framecfgs[fidx];
		if ((fie->width > framecfg->width) ||
			(fie->height > framecfg->height))
			continue;

		midx++;
		if (midx == fie->index) {
			fie->interval = framecfg->framerate;
			return 0;
		}
	}

	return -EINVAL;
}
#endif

static int cc_pad_set_fmt(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_format *fmt)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	const struct cc_frame_format *ccfmt = NULL;
	const struct cc_frame_cfg *frame_cfg;
	const struct cc_win_size *win_size;
	struct v4l2_mbus_framefmt *mf = NULL;
	int index;
	int ret;

	if (cc_si == NULL)
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	ret = cc_try_fmt_internal(sd, &fmt->format, &ccfmt, &win_size);
	if (ret)
		return ret;

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
	} else if (cc_si->streaming != 0)
		return -EBUSY;

	if (mf) {
		mutex_lock(&cc_si->lock);
		if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {
			PRT_DBG("\n");
			*mf = fmt->format;
		}
		mutex_unlock(&cc_si->lock);
	}

	for (index = 0; index < cc_si->nfrmcfgs; index++) {
		frame_cfg = &cc_si->framecfgs[index];
		if ((frame_cfg->width == win_size->width) &&
			(frame_cfg->height == win_size->height))
			break;
	}

	if (index >= cc_si->nfrmcfgs) {
		PRT_ERR("%s no frame configs match window size!\n",
			cc_si->name);
		return -EINVAL;
	}

	cc_si->cur_ccfmt = ccfmt;
	cc_si->cur_wsize = win_size;
	cc_si->cur_frmcfg = frame_cfg;
	cc_si->format = fmt->format;

	PRT_DBG("%s width=%d,height=%d\n",
		cc_si->name, win_size->width, win_size->height);

	if (cc_si->if_type == VISS_IF_TYPE_MCSI) {
		struct mcsi_if_info *mcsi_if = NULL;

		mcsi_if = &cc_si->if_info.mcsi_if_info;
		mcsi_if->dlanes = cc_si->dlanes;

		mcsi_if->data_type = VISS_MCSI_YUV422;
		switch (ccfmt->mbus_code) {
		case V4L2_MBUS_FMT_SBGGR8_1X8:
		case V4L2_MBUS_FMT_SGBRG8_1X8:
		case V4L2_MBUS_FMT_SGRBG8_1X8:
		case V4L2_MBUS_FMT_SRGGB8_1X8:
			mcsi_if->data_type = VISS_MCSI_RAW8;
			break;
		case V4L2_MBUS_FMT_SBGGR10_1X10:
		case V4L2_MBUS_FMT_SGBRG10_1X10:
		case V4L2_MBUS_FMT_SGRBG10_1X10:
		case V4L2_MBUS_FMT_SRGGB10_1X10:
			mcsi_if->data_type = VISS_MCSI_RAW10;
			break;
		case V4L2_MBUS_FMT_SBGGR12_1X12:
		case V4L2_MBUS_FMT_SGBRG12_1X12:
		case V4L2_MBUS_FMT_SGRBG12_1X12:
		case V4L2_MBUS_FMT_SRGGB12_1X12:
			mcsi_if->data_type = VISS_MCSI_RAW12;
			break;
		default:
			mcsi_if->data_type = VISS_MCSI_YUV422;
			break;
		}

		mcsi_if->mfreq = frame_cfg->mfreq;

		PRT_DBG("%s dlanes=%d,data_type=%d,mfreq=%d\n",
			cc_si->name, mcsi_if->dlanes, mcsi_if->data_type,
			mcsi_if->mfreq);
	}

	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

static int cc_pad_get_fmt(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_format *fmt)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	struct v4l2_mbus_framefmt *mf;

	PRT_DBG("\n");

	if (cc_si == NULL)
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		PRT_DBG("\n");
		mf = v4l2_subdev_get_try_format(fh, 0);
		fmt->format = *mf;
		return 0;
	}

	/* V4L2_SUBDEV_FORMAT_ACTIVE */
	mutex_lock(&cc_si->lock);
	fmt->format = cc_si->format;
	mutex_unlock(&cc_si->lock);

	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

/*
 * V4L2 subdev video and pad level operations
 */
static void cc_sensor_get_default_format(struct cc_sensor_info *cc_si,
					struct v4l2_mbus_framefmt *mf)
{
	PRT_TRACE_BEGIN("%s\n", cc_si->name);

	mf->width = cc_si->win_sizes[0].width;
	mf->height = cc_si->win_sizes[0].height;
	mf->code = cc_si->framefmts[0].mbus_code;
	mf->field = cc_si->framefmts[0].field;
	mf->colorspace = cc_si->framefmts[0].colorspace;

	cc_si->cur_ccfmt = &cc_si->framefmts[0];
	cc_si->cur_frmcfg = &cc_si->framecfgs[0];
	cc_si->cur_wsize = &cc_si->win_sizes[0];

	PRT_TRACE_END("%s\n", cc_si->name);
}

static long cc_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	int ret = 0;

	if (cc_si == NULL)
		return -ENODEV;

	if (cc_si->sensor_ioctl == NULL)
		return -ENOTSUPP;

	mutex_lock(&cc_si->lock);
	ret = cc_si->sensor_ioctl(cc_si, cmd, arg);
	mutex_unlock(&cc_si->lock);
	return ret;
}

static int cc_sensor_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);

	if (cc_si == NULL)
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

static void cc_set_source_info(struct cc_sensor_info *cc_si,
				struct viss_source_info *pd)
{
	pd->clk_frequency = cc_si->clk_frequency;
	pd->if_mode = cc_si->if_mode;
	pd->if_type = cc_si->if_type;
	pd->flags = cc_si->flags;
	pd->pclk_polarity = cc_si->pclk_polarity;
	pd->vsync_polarity = cc_si->vsync_polarity;
	pd->interlaced = cc_si->interlaced;
	pd->sort = cc_si->sort;

	if (cc_si->ch_number) {
		pd->ch_number = cc_si->ch_number;
		pd->sub_height = cc_si->framecfgs->sub_height;
		pd->sub_width = cc_si->framecfgs->sub_width;
		PRT_INFO("pd->sub_width:%d,pd->sub_height:%d\n",
			pd->sub_width, pd->sub_height);
	}

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	if (pd->if_type == VISS_IF_TYPE_ITU_601)
		pd->if_info = &cc_si->if_info.bt601_timing;
	else if (pd->if_type == VISS_IF_TYPE_MCSI)
		pd->if_info = &cc_si->if_info.mcsi_if_info;

	pd->vdd_1_8 = cc_si->vdd_1_8;

	if (cc_si->isp_path != 0)
		pd->use_isp = true;
	else
		pd->use_isp = false;

	pd->isp_id = cc_si->isp_id;
	pd->isp_path = cc_si->isp_path;

	pd->i2c_bus_num = cc_si->i2c_bus_num;
	PRT_TRACE_END("%s\n", cc_si->name);
}

static int cc_sensor_registered(struct v4l2_subdev *sd)
{
	struct cc_sensor_info *cc_si = NULL;
	struct viss_source_info *pd = v4l2_get_subdev_hostdata(sd);
	struct list_head *sensor_list;
	u32 cc_if;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	mutex_lock(&g_ccc->lock);

	cc_si = v4l2_get_subdevdata(sd);
	if (cc_si != NULL) {
		mutex_unlock(&g_ccc->lock);
		PRT_ERR("has already registered sensor %s\n", cc_si->name);
		PRT_TRACE_END("\n");
		return -EBUSY;
	}

	if (sd == &g_ccc->sd[CC_IF_MCSI]) {
		sensor_list = &g_ccc->sensor_list[CC_IF_MCSI];
		cc_if = CC_IF_MCSI;
	} else if (sd == &g_ccc->sd[CC_IF_DVP]) {
		sensor_list = &g_ccc->sensor_list[CC_IF_DVP];
		cc_if = CC_IF_DVP;
	} else {
		mutex_unlock(&g_ccc->lock);
		PRT_ERR("invalid subdev 0x%p\n", sd);
		PRT_TRACE_END("\n");
		return -ENODEV;
	}

	if (list_empty(sensor_list)) {
		mutex_unlock(&g_ccc->lock);
		PRT_INFO("No %s found!\n", sd->name);
		PRT_TRACE_END("\n");
		return -ENODEV;
	}

	/*
	 * no need do detect whether sensor is present when
	 * we are not going to support multi sensors adaption
	 */
	if (g_ccc->skip_detect) {
		cc_si = to_sensor_info(sensor_list->next);
		ret = cc_i2c_bus_add_sensor(cc_si);
		if (ret < 0)
			return ret;
		g_ccc->cur_sensor[cc_if] = cc_si;
		cc_set_source_info(cc_si, pd);
		v4l2_set_subdevdata(sd, cc_si);
		i2c_set_clientdata(cc_si->client, sd);
		/* change subdev name to current sensor name */
		strlcpy(sd->name, cc_si->name, sizeof(sd->name));
		mutex_unlock(&g_ccc->lock);
		PRT_INFO("skip sensor %s detect!\n", cc_si->name);
		PRT_TRACE_END("\n");
		return 0;
	}

	list_for_each_entry(cc_si, sensor_list, list) {
		mutex_lock(&cc_si->lock);
		ret = cc_i2c_bus_add_sensor(cc_si);
		if (ret < 0) {
			mutex_unlock(&cc_si->lock);
			continue;
		}

		cc_set_source_info(cc_si, pd);
		ret = pd->set_camclk(pd, true);
		if (ret != 0) {
			mutex_unlock(&cc_si->lock);
			continue;
		}

		ret = cc_sensor_power_up(cc_si);
		if (ret != 0) {
			cc_sensor_power_down(cc_si);
			pd->set_camclk(pd, false);
			mutex_unlock(&cc_si->lock);
			continue;
		}

		ret = cc_sensor_detect(cc_si);
		if (ret == 0) {
			cc_sensor_power_down(cc_si);
			pd->set_camclk(pd, false);
			g_ccc->cur_sensor[cc_if] = cc_si;
			v4l2_set_subdevdata(sd, cc_si);
			i2c_set_clientdata(cc_si->client, sd);
			/* change subdev name to current sensor name */
			strlcpy(sd->name, cc_si->name, sizeof(sd->name));
			mutex_unlock(&cc_si->lock);
			PRT_INFO("sensor %s present\n", cc_si->name);
			break;
		}

		PRT_INFO("sensor %s not present!\n", cc_si->name);

		cc_sensor_power_down(cc_si);
		pd->set_camclk(pd, false);
		cc_i2c_bus_remove_sensor(cc_si);
		mutex_unlock(&cc_si->lock);
		ret = -ENODEV;
	}

	mutex_unlock(&g_ccc->lock);
	PRT_TRACE_END("\n");
	return ret;
}

static void cc_sensor_unregistered(struct v4l2_subdev *sd)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	i2c_set_clientdata(cc_si->client, NULL);
	v4l2_set_subdevdata(sd, NULL);
	cc_i2c_bus_remove_sensor(cc_si);
	PRT_TRACE_END("%s\n", cc_si->name);
}

static int cc_set_power(struct v4l2_subdev *sd, int on)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	int ret = 0;

	if (cc_si == NULL)
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	if (cc_si->power == on) {
		PRT_INFO("sensor %s power state is alread %d!\n",
			cc_si->name, on);
		PRT_TRACE_END("%s\n", cc_si->name);
		return 0;
	}

	if (on) {
		ret = cc_sensor_power_up(cc_si);
		if (ret != 0)
			goto out;

		ret = cc_si->sensor_init(cc_si);
		if (ret != 0) {
			cc_sensor_power_down(cc_si);
			goto out;
		}
	} else {
		cc_sensor_power_down(cc_si);
		cc_si->streaming = 0;
	}

out:
	PRT_TRACE_END("%s\n", cc_si->name);
	return ret;
}

static int cc_s_stream(struct v4l2_subdev *sd, int on)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	int ret = 0;

	if (cc_si == NULL)
		return -ENODEV;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	if (cc_si->streaming == on) {
		PRT_INFO("sensor %s streaming state is alread %d!\n",
			cc_si->name, on);
		PRT_TRACE_END("%s\n", cc_si->name);
		return 0;
	}

	if (on != 0) {
		ret = cc_si->write_reg_list(cc_si, cc_si->cur_ccfmt->regs,
					cc_si->cur_ccfmt->regs_cnt);
		if (ret != 0)
			return ret;

		ret = cc_si->write_reg_list(cc_si, cc_si->cur_wsize->regs,
					cc_si->cur_wsize->regs_cnt);
		if (ret != 0)
			return ret;

		ret = cc_si->write_reg_list(cc_si, cc_si->cur_frmcfg->regs,
				cc_si->cur_frmcfg->regs_cnt);
		if (ret != 0)
			return ret;
	}

	ret = cc_si->set_stream(cc_si, on);
	if (ret == 0)
		cc_si->streaming = on;
	else
		cc_si->streaming = 0;

	PRT_TRACE_END("%s\n", cc_si->name);
	return ret;
}

static int cc_sensor_reset(struct v4l2_subdev *sd, u32 val)
{
	struct cc_sensor_info *cc_si = subdev_to_sensor_info(sd);
	int ret = 0;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	if (val == 0) {
		/* stream off sensor */
		ret = cc_s_stream(sd, 0);
		if (ret != 0) {
			PRT_WARN("stream off sensor %s failed\n",
				cc_si->name);
			ret = 0;
		}

		cc_sensor_power_down(cc_si);
	} else {
		ret = cc_set_power(sd, 1);
		if (ret != 0)
			return ret;

		ret = cc_si->write_reg_list(cc_si, cc_si->cur_ccfmt->regs,
					cc_si->cur_ccfmt->regs_cnt);
		if (ret != 0)
			return ret;

		ret = cc_si->write_reg_list(cc_si, cc_si->cur_wsize->regs,
					cc_si->cur_wsize->regs_cnt);
		if (ret != 0)
			return ret;

		ret = cc_si->write_reg_list(cc_si, cc_si->cur_frmcfg->regs,
				cc_si->cur_frmcfg->regs_cnt);
		if (ret != 0)
			return ret;

		/* stream on sensor */
		ret = cc_s_stream(sd, 1);
		if (ret != 0)
			return ret;
	}

	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}

static const struct v4l2_subdev_internal_ops cc_sensor_subdev_internal_ops = {
	.registered		= cc_sensor_registered,
	.unregistered		= cc_sensor_unregistered,
	.open			= cc_sensor_open,
};

static const struct v4l2_subdev_pad_ops cc_sensor_pad_ops = {
	.enum_mbus_code		= cc_pad_enum_mbus_code,
#if 0
	.enum_frame_size	= cc_pad_enum_frame_sizes,
	.enum_frame_interval	= cc_pad_enum_frame_interval,
#endif
	.get_fmt		= cc_pad_get_fmt,
	.set_fmt		= cc_pad_set_fmt,
};

/*
 * V4L2 subdev internal operations
 */
static const struct v4l2_subdev_core_ops cc_sensor_subdev_core_ops = {
	.s_power		= cc_set_power,
	.reset			= cc_sensor_reset,
	.ioctl			= cc_ioctl,
	.log_status		= v4l2_ctrl_subdev_log_status,
	.subscribe_event	= v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event	= v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops cc_sensor_video_ops = {
	.s_stream		= cc_s_stream,
#if 0
	.g_frame_interval	= cc_g_frame_interval,
	.s_frame_interval	= cc_s_frame_interval,
#endif
	.s_parm			= cc_s_parm,
	.g_parm			= cc_g_parm,
	.enum_frameintervals	= cc_enum_frameintervals,
	.enum_framesizes	= cc_enum_framesizes,
};

static const struct v4l2_subdev_ops cc_sensor_subdev_ops = {
	.core			= &cc_sensor_subdev_core_ops,
	.pad			= &cc_sensor_pad_ops,
	.video			= &cc_sensor_video_ops,
};

int cc_sensor_probe(struct platform_device *pdev,
			struct cc_sensor_info *cc_si)
{
	int ret = 0;

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	PRT_DBG("*********%s probe begin\n", cc_si->name);

	mutex_init(&cc_si->lock);

	ret = cc_parse_sensor_node(pdev, cc_si);
	if (ret < 0)
		return ret;

	ret = cc_register_sensor(cc_si);
	if (ret < 0)
		return ret;

	cc_sensor_get_default_format(cc_si, &cc_si->format);

	platform_set_drvdata(pdev, cc_si);

	PRT_DBG("%s probe end\n", cc_si->name);
	PRT_TRACE_END("%s\n", cc_si->name);
	return 0;
}
EXPORT_SYMBOL(cc_sensor_probe);

int cc_sensor_remove(struct platform_device *pdev)
{
	struct cc_sensor_info *cc_si = platform_get_drvdata(pdev);

	PRT_TRACE_BEGIN("%s\n", cc_si->name);
	cc_unregister_sensor(cc_si);
	PRT_TRACE_END("%s\n", cc_si->name);

	return 0;
}
EXPORT_SYMBOL(cc_sensor_remove);

static int cc_dvp_cam_request_pins(struct cccore *ccc, struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret = 0;

	ccc->cam_en_pin[CC_IF_DVP] = of_get_named_gpio_flags(np,
						"dvp_cam_en_pin",
						0, &flags);

	if (!gpio_is_valid(ccc->cam_en_pin[CC_IF_DVP])) {
		PRT_INFO("no pin for dvp camera en (%d)\n",
			 ccc->cam_en_pin[CC_IF_DVP]);
			 ccc->cam_en_pin[CC_IF_DVP] = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->cam_en_pin[CC_IF_DVP],
					"dvp_cam_en_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (dvp_cam_en_pin)\n",
				ccc->cam_en_pin[CC_IF_DVP]);
			ccc->cam_en_pin[CC_IF_DVP] = -1;
			return -EBUSY;
		}
	}

	return 0;
}

static int cc_mcsi_cam_request_pins(struct cccore *ccc, struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret = 0;

	ccc->cam_en_pin[CC_IF_MCSI] = of_get_named_gpio_flags(np,
							"mcsi_cam_en_pin",
							0, &flags);

	if (!gpio_is_valid(ccc->cam_en_pin[CC_IF_MCSI])) {
		PRT_INFO("no pin for mcsi camera en (%d)\n",
			ccc->cam_en_pin[CC_IF_MCSI]);
			ccc->cam_en_pin[CC_IF_MCSI] = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->cam_en_pin[CC_IF_MCSI],
					"mcsi_cam_en_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (mcsi_cam_en_pin)\n",
				ccc->cam_en_pin[CC_IF_MCSI]);
			ccc->cam_en_pin[CC_IF_MCSI] = -1;
			return -EBUSY;
		}
	}

	return 0;
}

static int cc_led_request_pins(struct cccore *ccc, struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret = 0;

	ccc->led_pin = of_get_named_gpio_flags(np, "led_pwdn_pin", 0, &flags);

	if (!gpio_is_valid(ccc->led_pin)) {
		PRT_INFO("no pin for led (%d)\n", ccc->led_pin);
		ccc->led_pin = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->led_pin,
					"led_pwdn_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (led_pwdn_pin)\n",
				ccc->led_pin);
			ccc->led_pin = -1;
			return -EBUSY;
		}
	}
	return 0;
}

static int cc_dvp_request_pins(struct cccore *ccc, struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret = 0;

	ccc->reset_pin[CC_IF_DVP] = of_get_named_gpio_flags(np,
					"dvp_reset_pin", 0, &flags);
	ccc->pwdn_pin[CC_IF_DVP] = of_get_named_gpio_flags(np,
					"dvp_pwdn_pin", 0, &flags);

	if (!gpio_is_valid(ccc->reset_pin[CC_IF_DVP])) {
		PRT_INFO("no reset pin for dvp (%d)\n",
			ccc->reset_pin[CC_IF_DVP]);
		ccc->reset_pin[CC_IF_DVP] = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->reset_pin[CC_IF_DVP],
					"dvp_reset_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (dvp_reset_pin)\n",
				ccc->reset_pin[CC_IF_DVP]);
			ccc->reset_pin[CC_IF_DVP] = -1;
			ccc->pwdn_pin[CC_IF_DVP] = -1;
			return -EBUSY;
		}
		gpio_direction_output(ccc->reset_pin[CC_IF_DVP], 0);
	}

	if (!gpio_is_valid(ccc->pwdn_pin[CC_IF_DVP])) {
		PRT_INFO("no pwdn pin for dvp (%d)\n",
			ccc->pwdn_pin[CC_IF_DVP]);
		ccc->pwdn_pin[CC_IF_DVP] = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->pwdn_pin[CC_IF_DVP],
					"dvp_pwdn_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (dvp_pwdn_pin)\n",
				ccc->pwdn_pin[CC_IF_DVP]);
			ccc->reset_pin[CC_IF_DVP] = -1;
			ccc->pwdn_pin[CC_IF_DVP] = -1;
			return -EBUSY;
		}
		gpio_direction_output(ccc->pwdn_pin[CC_IF_DVP], 0);
	}

	return 0;
}

static int cc_mcsi_request_pins(struct cccore *ccc, struct device_node *np)
{
	enum of_gpio_flags flags;
	int ret = 0;

	ccc->reset_pin[CC_IF_MCSI] = of_get_named_gpio_flags(np,
					"mcsi_reset_pin", 0, &flags);
	ccc->pwdn_pin[CC_IF_MCSI] = of_get_named_gpio_flags(np,
					"mcsi_pwdn_pin", 0, &flags);

	if (!gpio_is_valid(ccc->reset_pin[CC_IF_MCSI])) {
		PRT_INFO("no reset pin for mcsi (%d)\n",
			ccc->reset_pin[CC_IF_MCSI]);
		ccc->reset_pin[CC_IF_MCSI] = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->reset_pin[CC_IF_MCSI],
					"mcsi_reset_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (mcsi_reset_pin)\n",
				ccc->reset_pin[CC_IF_MCSI]);
			ccc->reset_pin[CC_IF_MCSI] = -1;
			ccc->pwdn_pin[CC_IF_MCSI] = -1;
			return -EBUSY;
		}
		gpio_direction_output(ccc->reset_pin[CC_IF_MCSI], 0);
	}

	if (!gpio_is_valid(ccc->pwdn_pin[CC_IF_MCSI])) {
		PRT_INFO("no pwdn pin for mcsi (%d)\n",
			ccc->pwdn_pin[CC_IF_MCSI]);
		ccc->pwdn_pin[CC_IF_MCSI] = -1;
	} else {
		ret = devm_gpio_request(ccc->dev, ccc->pwdn_pin[CC_IF_MCSI],
					"mcsi_pwdn_pin");
		if (ret < 0) {
			PRT_ERR("pin %d request failed (mcsi_pwdn_pin)\n",
				ccc->pwdn_pin[CC_IF_MCSI]);
			ccc->reset_pin[CC_IF_MCSI] = -1;
			ccc->pwdn_pin[CC_IF_MCSI] = -1;
			return -EBUSY;
		}
		gpio_direction_output(ccc->pwdn_pin[CC_IF_MCSI], 0);
	}

	return 0;
}

struct v4l2_subdev *cccore_get_dvp_sensor_subdev(void)
{
	if (g_ccc == NULL)
		return NULL;

	return &g_ccc->sd[CC_IF_DVP];
}
EXPORT_SYMBOL(cccore_get_dvp_sensor_subdev);

struct v4l2_subdev *cccore_get_mcsi_sensor_subdev(void)
{
	if (g_ccc == NULL)
		return NULL;

	return &g_ccc->sd[CC_IF_MCSI];
}
EXPORT_SYMBOL(cccore_get_mcsi_sensor_subdev);

#define to_platform_driver(drv)	(container_of((drv), struct platform_driver, \
				 driver))

static int __cccore_register_subdev(struct cccore *ccc, u32 cc_if)
{
	struct v4l2_subdev *sd = &ccc->sd[cc_if];
	struct media_pad *pad = &ccc->pad[cc_if];
	int ret = 0;

	v4l2_subdev_init(sd, &cc_sensor_subdev_ops);
	sd->flags |= V4L2_SUBDEV_FL_IS_I2C;

	sd->owner = THIS_MODULE;

	/* subdevdata is point to current sensor info */
	/* v4l2_set_subdevdata */

	v4l2_set_subdevdata(sd, NULL);

	if (cc_if == CC_IF_DVP)
		strlcpy(sd->name, "dvp_sensor", sizeof("dvp_sensor"));
	else
		strlcpy(sd->name, "mcsi_sensor", sizeof("mcsi_sensor"));

	PRT_DBG("%s\n", sd->name);

	sd->internal_ops = &cc_sensor_subdev_internal_ops;

	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

#if defined(CONFIG_MEDIA_CONTROLLER)
	pad->flags = MEDIA_PAD_FL_SOURCE;
	sd->entity.type = MEDIA_ENT_T_V4L2_SUBDEV_SENSOR;
	ret = media_entity_init(&sd->entity, 1, pad, 0);
	if (ret < 0)
		return ret;
#endif

	return 0;
}

static void __cccore_unregister_subdev(struct cccore *ccc, u32 cc_if)
{
	struct v4l2_subdev *sd = &ccc->sd[cc_if];

#if defined(CONFIG_MEDIA_CONTROLLER)
	media_entity_cleanup(&sd->entity);
#endif
}

static int cccore_register_subdevs(struct cccore *ccc)
{
	int ret = 0;

	ret = __cccore_register_subdev(ccc, CC_IF_DVP);
	if (ret < 0)
		return ret;

	ret = __cccore_register_subdev(ccc, CC_IF_MCSI);
	if (ret < 0) {
		__cccore_unregister_subdev(ccc, CC_IF_DVP);
		return ret;
	}

	return 0;
}

static void cccore_unregister_subdevs(struct cccore *ccc)
{
	int cc_if;

	for (cc_if = 0; cc_if < CC_IF_CNT; cc_if++)
		__cccore_unregister_subdev(ccc, cc_if);
}

static ssize_t
cc_sensor_reg_node_store(struct device *dev, struct device_attribute *attr,
		      const char *buf, size_t count)
{
	struct cccore *ccc = dev_get_drvdata(dev);
	struct cc_sensor_info *cc_si = NULL;
	u32 *cur_reg;
	int addr, value;
	char end;
	int res;

	if (attr->attr.name[0] == 'd') {
		cc_si = ccc->cur_sensor[CC_IF_DVP];
		cur_reg = &ccc->cur_sensor_reg[CC_IF_DVP];
	} else {
		cc_si = ccc->cur_sensor[CC_IF_MCSI];
		cur_reg = &ccc->cur_sensor_reg[CC_IF_MCSI];
	}

	if (cc_si == NULL)
		return count;

	res = sscanf(buf, "%x %x%c", &addr, &value, &end);
	if (res < 1) {
		PRT_ERR("Can't parse reg address (%s)\n", buf);
		return -EINVAL;
	}

	if ((res > 2) && (end != '\n')) {
		PRT_ERR("Extra parameters (%s)\n", buf);
		return -EINVAL;
	}

	if (addr >= (1 << (cc_si->reg_bits))) {
		PRT_ERR("addr is too large (%s)!\n", buf);
		return -EINVAL;
	}

	*cur_reg = (u32)addr;

	if (res >= 2) {
		if (value >= (1 << (cc_si->val_bits))) {
			PRT_ERR("value is too large (%s)!\n", buf);
			return -EINVAL;
		}

		res = cc_si->write_reg(cc_si, *cur_reg, (u32)value);
		if (res) {
			PRT_ERR("\n");
			return res;
		}
	}

	return count;
}

static ssize_t
cc_sensor_reg_node_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct cccore *ccc = dev_get_drvdata(dev);
	struct cc_sensor_info *cc_si = NULL;
	u32 cur_reg = 0;
	u32 value = 0;
	int res;

	if (attr->attr.name[0] == 'd') {
		cc_si = ccc->cur_sensor[CC_IF_DVP];
		cur_reg = ccc->cur_sensor_reg[CC_IF_DVP];
	} else {
		cc_si = ccc->cur_sensor[CC_IF_MCSI];
		cur_reg = ccc->cur_sensor_reg[CC_IF_MCSI];
	}

	if (cc_si == NULL)
		return 0;

	if (cur_reg == 0xFFFFFFFF) {
		PRT_ERR("write reg addr to reg_node first!\n");
		return -EINVAL;
	}

	res = cc_si->read_reg(cc_si, cur_reg, &value);
	if (res != 0) {
		PRT_ERR("read reg (0x%08x) failed!\n", cur_reg);
		return res;
	}

	return sprintf(buf, "0x%08x\n", value);
}

static DEVICE_ATTR(dvp_sensor_reg, S_IRUGO | S_IWUSR | S_IWGRP,
			cc_sensor_reg_node_show,
			cc_sensor_reg_node_store);

static DEVICE_ATTR(mcsi_sensor_reg, S_IRUGO | S_IWUSR | S_IWGRP,
			cc_sensor_reg_node_show,
			cc_sensor_reg_node_store);

static ssize_t
cc_sensor_name_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct cccore *ccc = dev_get_drvdata(dev);
	struct cc_sensor_info *cc_si = NULL;

	if (attr->attr.name[0] == 'd')
		cc_si = ccc->cur_sensor[CC_IF_DVP];
	else
		cc_si = ccc->cur_sensor[CC_IF_MCSI];

	if (cc_si == NULL)
		return 0;

	return sprintf(buf, "%s\n", cc_si->name);
}

static DEVICE_ATTR(dvp_sensor_name, S_IRUGO, cc_sensor_name_show, NULL);
static DEVICE_ATTR(mcsi_sensor_name, S_IRUGO, cc_sensor_name_show, NULL);

static ssize_t
cc_sensor_reg_bits_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct cccore *ccc = dev_get_drvdata(dev);
	struct cc_sensor_info *cc_si = NULL;

	if (attr->attr.name[0] == 'd')
		cc_si = ccc->cur_sensor[CC_IF_DVP];
	else
		cc_si = ccc->cur_sensor[CC_IF_MCSI];

	if (cc_si == NULL)
		return 0;

	return sprintf(buf, "reg %d bits, val %d bits\n",
			cc_si->reg_bits, cc_si->val_bits);
}

static DEVICE_ATTR(dvp_reg_bits, S_IRUGO, cc_sensor_reg_bits_show, NULL);
static DEVICE_ATTR(mcsi_reg_bits, S_IRUGO, cc_sensor_reg_bits_show, NULL);

static struct attribute *ccc_dev_attrs[] = {
	&dev_attr_dvp_sensor_reg.attr,
	&dev_attr_mcsi_sensor_reg.attr,
	&dev_attr_dvp_sensor_name.attr,
	&dev_attr_mcsi_sensor_name.attr,
	&dev_attr_dvp_reg_bits.attr,
	&dev_attr_mcsi_reg_bits.attr,
	NULL
};

static struct attribute_group ccc_dev_attr_group = {
	.attrs		= ccc_dev_attrs,
};

static struct class lombo_camera_class = {
	.name		= "lombo_camera",
};

static int cccore_probe(struct platform_device *pdev)
{
	struct cccore *ccc = NULL;
	struct device_node *np = NULL;
	int len;
	int ret = 0;

	PRT_DBG("cccore probe enter!\n");
	ccc = devm_kzalloc(&pdev->dev, sizeof(*ccc), GFP_KERNEL);
	if (ccc == NULL) {
		PRT_ERR("Can't alloc memory!\n");
		return -ENOMEM;
	}

	ccc->dev = &pdev->dev;
	np = pdev->dev.of_node;

	INIT_LIST_HEAD(&ccc->sensor_list[CC_IF_DVP]);
	INIT_LIST_HEAD(&ccc->sensor_list[CC_IF_MCSI]);

	mutex_init(&ccc->lock);

	ret = cc_dvp_request_pins(ccc, np);
	if (ret < 0)
		return ret;

	ret = cc_mcsi_request_pins(ccc, np);
	if (ret < 0)
		return ret;

	ret = cc_led_request_pins(ccc, np);
	if (ret < 0)
		PRT_ERR("no led pin!\n");

	ret = cc_dvp_cam_request_pins(ccc, np);
	if (ret < 0)
		PRT_ERR("no dvp cam en pin!\n");

	ret = cc_mcsi_cam_request_pins(ccc, np);
	if (ret < 0)
		PRT_ERR("no mcsi cam en pin!\n");

	ret = class_register(&lombo_camera_class);
	if (ret != 0) {
		PRT_ERR("sysfs_create_group failed!\n");
		return ret;
	}

	dev_set_name(&ccc->class_dev, "cccore");
	ccc->class_dev.parent = &pdev->dev;
	ccc->class_dev.class = &lombo_camera_class;
	device_initialize(&ccc->class_dev);

	ret = device_add(&ccc->class_dev);
	if (ret != 0) {
		PRT_ERR("device_add failed!\n");
		goto unregister_class;
	}

	ret = sysfs_create_group(&ccc->class_dev.kobj, &ccc_dev_attr_group);
	if (ret != 0) {
		PRT_ERR("sysfs_create_group failed!\n");
		goto remove_dev;
	}

	if (of_find_property(np, "skip_detect", &len))
		ccc->skip_detect = true;
	else
		ccc->skip_detect = false;

	platform_set_drvdata(pdev, ccc);
	dev_set_drvdata(&ccc->class_dev, ccc);

	ret = cccore_register_subdevs(ccc);
	if (ret != 0) {
		PRT_ERR("cccore_register_subdevs failed!\n");
		goto remove_dev;
	}

	ret = ir_cut_init(pdev);
	if (ret != 0)
		PRT_ERR("ir cut init failed!\n");

	g_ccc = ccc;
	PRT_INFO("cccore probed!\n");

	return 0;

remove_dev:
	device_del(&ccc->class_dev);

unregister_class:
	class_unregister(&lombo_camera_class);
	return ret;
}

static int cccore_remove(struct platform_device *pdev)
{
	struct cccore *ccc = platform_get_drvdata(pdev);

	ir_cut_exit(ccc);

	cccore_unregister_subdevs(ccc);

	if (gpio_is_valid(ccc->reset_pin[CC_IF_DVP]))
		devm_gpio_free(ccc->dev, ccc->reset_pin[CC_IF_DVP]);
	if (gpio_is_valid(ccc->pwdn_pin[CC_IF_DVP]))
		devm_gpio_free(ccc->dev, ccc->pwdn_pin[CC_IF_DVP]);
	if (gpio_is_valid(ccc->reset_pin[CC_IF_MCSI]))
		devm_gpio_free(ccc->dev, ccc->reset_pin[CC_IF_MCSI]);
	if (gpio_is_valid(ccc->pwdn_pin[CC_IF_MCSI]))
		devm_gpio_free(ccc->dev, ccc->pwdn_pin[CC_IF_MCSI]);

	devm_kfree(ccc->dev, ccc);

	return 0;
}

static const struct of_device_id cccore_of_match[] = {
	{
		.compatible = "lombo,cccore",
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, cccore_of_match);

static struct platform_driver cccore_driver = {
	.probe		= cccore_probe,
	.remove		= cccore_remove,
	.driver = {
		.of_match_table = cccore_of_match,
		.name		= "cccore",
		.owner		= THIS_MODULE,
	}
};

static int __init lombo_cccore_init(void)
{
	return platform_driver_register(&cccore_driver);
}
subsys_initcall(lombo_cccore_init);

static void __exit lombo_cccore_exit(void)
{
	platform_driver_unregister(&cccore_driver);
}
module_exit(lombo_cccore_exit);

MODULE_DESCRIPTION("lombo camera control core driver");
MODULE_LICENSE("GPL v2");
