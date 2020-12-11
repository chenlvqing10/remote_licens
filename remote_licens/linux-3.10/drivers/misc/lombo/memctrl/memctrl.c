/*
 * memctrl driver - Driver of LomboTech Memctrl Controller
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/of_platform.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <mach/debug.h>
#include "csp/csp_memctrl.h"
#include "memctrl.h"

static struct data_unit data_units[] = {
	{
		"B",
		1
	},
	{
		"KB",
		1024
	},
	{
		"MB",
		1024 * 1024
	}
};

static struct cfg_unit flaw_units[] = {
	{
		"1",
		MEMCTRL_UNIT_1B,
		1
	},
	{
		"16",
		MEMCTRL_UNIT_16B,
		16
	},
	{
		"64",
		MEMCTRL_UNIT_64B,
		64
	},
	{
		"256",
		MEMCTRL_UNIT_256B,
		256
	},
	{
		"1K",
		MEMCTRL_UNIT_1KB,
		1024
	},
	{
		"4K",
		MEMCTRL_UNIT_4KB,
		4096
	},
	{
		"64K",
		MEMCTRL_UNIT_64KB,
		64 * 1024
	},
	{
		"128K",
		MEMCTRL_UNIT_128KB,
		128 * 1024
	}
};

static struct cfg_unit sample_units[] = {
	{
		"1",
		MEMCTRL_SAMPLE_1MS,
		1,
		1,
		"ms"
	},
	{
		"10",
		MEMCTRL_SAMPLE_10MS,
		10,
		1,
		"ms"
	},
	{
		"100",
		MEMCTRL_SAMPLE_100MS,
		100,
		1,
		"ms"
	},
	{
		"1000",
		MEMCTRL_SAMPLE_1S,
		1000,
		1000,
		"s"
	},
	{
		"10000",
		MEMCTRL_SAMPLE_10S,
		10000,
		1000,
		"s"
	}
};

static struct channel_info n7_channels[] = {
	{
		"CPU",
		MEMCTRL_CPU
	},
	{
		"VC",
		MEMCTRL_VC
	},
	{
		"AX",
		MEMCTRL_AX
	},
	{
		"VISS0",
		MEMCTRL_VISS0
	},
	{
		"VISS1",
		MEMCTRL_VISS1
	},
	{
		"DPU",
		MEMCTRL_DPU
	},
	{
		"DMA",
		MEMCTRL_DMA
	}
};

static struct bw_bits_name n7_bw_default[] = {
	{
		.name	= "timewindow",
		.ref	= "tw",
		.val	= 0x200,
	},
	{
		.name	= "datasize",
		.ref	= "ds",
		.val	= 0x400,
	},
	{
		.name	= "randmode",
		.ref	= "rm",
		.val	= 0x0,
	},
	{
		.name	= "ctrlmode",
		.ref	= "cm",
		.val	= 0x0,
	},
	{
		.name	= "randomseed",
		.ref	= "rs",
		.val	= 0x10,
	},
};

static struct global_data globaldata;
static struct channel_data totaldata;
static struct channel_data channeldata;
static struct lombo_memctrl *pmemctrl;

static void mop(struct lombo_memctrl *mctrl, const char *fmt, ...)
{
	va_list args;
	size_t length;

	va_start(args, fmt);
	length = vsnprintf(mctrl->log_buf + mctrl->log_offset,
				sizeof(mctrl->log_buf) - 1, fmt, args);
	va_end(args);

	/* Not support now, should be false  */
	if (mctrl->op == OP_FILE)
		vfs_write(mctrl->pfile, mctrl->log_buf + mctrl->log_offset,
				length, &mctrl->fpos);
	else
		mctrl->log_offset += length;
}

static void memctrl_start(struct lombo_memctrl *mctrl)
{
	csp_memctrl_interrupt_enable(mctrl->reg, true);
	csp_memctrl_enable(mctrl->reg, true);
}

static void memctrl_stop(struct lombo_memctrl *mctrl)
{
	csp_memctrl_interrupt_enable(mctrl->reg, false);
	csp_memctrl_enable(mctrl->reg, false);
}

static void memctrl_set_data_unit(struct lombo_memctrl *mctrl,
					enum memctrl_data_unit data)
{
	csp_memctrl_set_data_unit(mctrl->reg, data);
}

static void memctrl_set_sample(struct lombo_memctrl *mctrl,
					enum memctrl_sample sample)
{
	csp_memctrl_set_sample(mctrl->reg, sample);
}

/* static u32 memctrl_get_cycle(struct lombo_memctrl *mctrl)
{
	return csp_memctrl_get_cycle(mctrl->reg);
} */

static void memctrl_get_global_data(struct lombo_memctrl *mctrl,
					struct global_data *data)
{
	data->total_size = csp_memctrl_get_total_data_size(mctrl->reg);
	data->total_rd_size = csp_memctrl_get_total_rd_data_size(mctrl->reg);
	data->total_wr_size = csp_memctrl_get_total_wr_data_size(mctrl->reg);
	data->total_rd_peak = csp_memctrl_get_total_rd_peak(mctrl->reg);
	data->total_wr_peak = csp_memctrl_get_total_wr_peak(mctrl->reg);
}

static void memctrl_get_channel_data(struct lombo_memctrl *mctrl,
					int channel, struct channel_data *data)
{
	data->rd_size = csp_memctrl_get_rd_data_size(mctrl->reg, channel);
	data->wr_size = csp_memctrl_get_wr_data_size(mctrl->reg, channel);

	data->rd_peak = csp_memctrl_get_rd_peak_data_size(mctrl->reg, channel);
	data->wr_peak = csp_memctrl_get_wr_peak_data_size(mctrl->reg, channel);

	data->rd_num = csp_memctrl_get_rd_cmd_num(mctrl->reg, channel);
	data->wr_num = csp_memctrl_get_wr_cmd_num(mctrl->reg, channel);

	data->rd_latency = csp_memctrl_get_rd_latency(mctrl->reg, channel)
				* LAT_CYCTLES / mctrl->lat_div_us;
	data->wr_latency = csp_memctrl_get_wr_latency(mctrl->reg, channel)
				* LAT_CYCTLES / mctrl->lat_div_us;
}

static void memctrl_dump_start(struct lombo_memctrl *mctrl)
{
	mop(mctrl,
	"\n  %-8s %10s %5s %10s %10s %10s %10s %5s %10s %10s %10s %10s %5s\n",
		"MASTER",
		"READ", "%R", "R_PEAK", "R_NUM", "R_LAT",
		"WRITE", "%W", "W_PEAK", "W_NUM", "W_LAT", "RW",
		"%RW");
}

static void memctrl_dump_channel(const char *name,
				struct global_data *gdata,
				struct channel_data *cdata,
				struct lombo_memctrl *mctrl)
{
	u32 data_unit_size = data_units[mctrl->size_unit].size;
	u64 rd_size = cdata->rd_size * flaw_units[mctrl->flaw_unit].size;
	u64 rd_percent = 0;
	u64 rd_peak = cdata->rd_peak * flaw_units[mctrl->flaw_unit].size;
	u64 wr_size = cdata->wr_size * flaw_units[mctrl->flaw_unit].size;
	u64 wr_percent = 0;
	u64 wr_peak = cdata->wr_peak * flaw_units[mctrl->flaw_unit].size;
	u64 rw_percent = 0;

	do_div(rd_size, data_unit_size);
	if (gdata->total_rd_size != 0) {
		rd_percent = cdata->rd_size * 100;
		do_div(rd_percent, gdata->total_rd_size);
	}
	do_div(rd_peak, data_unit_size);
	do_div(wr_size, data_unit_size);
	if (gdata->total_wr_size != 0) {
		wr_percent = cdata->wr_size * 100;
		do_div(wr_percent, gdata->total_wr_size);
	}
	do_div(wr_peak, data_unit_size);
	if (gdata->total_size != 0) {
		rw_percent = (cdata->rd_size + cdata->wr_size) * 100;
		do_div(rw_percent, gdata->total_size);
	}

	mop(mctrl,
	"  %-8s %10llu %5d %10llu %10llu %10llu %10llu %5d %10llu %10llu %10llu %10llu %5d\n",
		name,
		rd_size,
		rd_percent,
		rd_peak,
		cdata->rd_num, cdata->rd_latency,
		wr_size,
		wr_percent,
		wr_peak,
		cdata->wr_num, cdata->wr_latency,
		(rd_size + wr_size),
		rw_percent);
}

static void memctrl_dump_total(struct global_data *gdata,
					struct channel_data *cdata,
					struct lombo_memctrl *mctrl)
{
	int i = 12;

	u32 data_unit_size = data_units[mctrl->size_unit].size;

	u64 total_rd_size =
		gdata->total_rd_size * flaw_units[mctrl->flaw_unit].size;
	u64 rd_percent = 0;

	u64 total_wr_size =
		gdata->total_wr_size * flaw_units[mctrl->flaw_unit].size;
	u64 wr_percent = 0;

	u64 total_rw_size =
		gdata->total_size * flaw_units[mctrl->flaw_unit].size;
	u64 rw_percent = 0;

	u64 rd_peak =
		gdata->total_rd_peak * flaw_units[mctrl->flaw_unit].size;
	u64 wr_peak =
		gdata->total_wr_peak * flaw_units[mctrl->flaw_unit].size;

	/* Split line */
	while (i--)
		mop(mctrl, "----------");
	mop(mctrl, "\n");

	do_div(total_rd_size, data_unit_size);
	if (gdata->total_rd_size != 0) {
		rd_percent = cdata->rd_size * 100;
		do_div(rd_percent, gdata->total_rd_size);
	}

	do_div(total_wr_size, data_unit_size);
	if (gdata->total_wr_size != 0) {
		wr_percent = cdata->wr_size * 100;
		do_div(wr_percent, gdata->total_wr_size);
	}

	do_div(total_rw_size, data_unit_size);
	if (gdata->total_size != 0) {
		rw_percent = (cdata->rd_size + cdata->wr_size) * 100;
		do_div(rw_percent, gdata->total_size);
	}

	do_div(rd_peak, data_unit_size);
	do_div(wr_peak, data_unit_size);

	mop(mctrl,
	"  %-8s %10llu %5d %10llu %10llu %10llu %10llu %5d %10llu %10llu %10llu %10llu %5d\n",
		"total",
		total_rd_size,
		rd_percent,
		rd_peak,
		cdata->rd_num, cdata->rd_latency,
		total_wr_size,
		wr_percent,
		wr_peak,
		cdata->wr_num, cdata->wr_latency,
		total_rw_size,
		rw_percent);
}

static void memctrl_channel_trace(struct channel_data *gdata,
					struct channel_data *cdata)
{
	gdata->rd_size += cdata->rd_size;
	gdata->rd_num += cdata->rd_num;
	gdata->rd_latency += cdata->rd_latency;
	gdata->wr_size += cdata->wr_size;
	gdata->wr_num += cdata->wr_num;
	gdata->wr_latency += cdata->wr_latency;
	if (gdata->rd_peak < cdata->rd_peak)
		gdata->rd_peak = cdata->rd_peak;
	if (gdata->wr_peak < cdata->wr_peak)
		gdata->wr_peak = cdata->wr_peak;
}

static void memctrl_dump_end(struct lombo_memctrl *mctrl)
{
	const char *lat_unit = "us";
	char sample_unit[10], time_unit[10], data_unit[10];
	const char *sample_format = "%s/%d%s";
	const char *time_format = "/%d%s";
	const char *data_format = "%s%s";

	snprintf(time_unit, sizeof(time_unit), time_format,
			sample_units[mctrl->sample_unit].size * mctrl->time
			/ sample_units[mctrl->sample_unit].div,
			sample_units[mctrl->sample_unit].unit);

	snprintf(data_unit, sizeof(data_unit), data_format,
			data_units[mctrl->size_unit].name, time_unit);

	snprintf(sample_unit, sizeof(sample_unit), sample_format,
			data_units[mctrl->size_unit].name,
			sample_units[mctrl->sample_unit].size
			/ sample_units[mctrl->sample_unit].div,
			sample_units[mctrl->sample_unit].unit);

	mop(mctrl,
	"  %-8s %10s %5s %10s %10s %10s %10s %5s %10s %10s %10s %10s %5s\n",
			"unit",
			data_unit, "%", sample_unit, time_unit, lat_unit,
		data_unit, "%", sample_unit, time_unit, lat_unit, data_unit,
			"%");
	mop(mctrl, "\n");
}

static void memctrl_dump_once(struct lombo_memctrl *mctrl)
{
	struct channel_data *cdata = &channeldata;
	struct channel_data *total_data = &totaldata;
	struct global_data *gdata = &globaldata;
	struct channel_info *cinfo;
	int i;
	unsigned long min_sleep, max_sleep;

	/* Start -> wait for time -> stop */
	memctrl_set_data_unit(mctrl, flaw_units[mctrl->flaw_unit].cfg);
	memctrl_set_sample(mctrl, sample_units[mctrl->sample_unit].cfg);
	memctrl_start(mctrl);
	/*while (mctrl->time > memctrl_get_cycle(mctrl))
		; */
	min_sleep = ((mctrl->time * sample_units[mctrl->sample_unit].size)
			* 1000) - 100;
	max_sleep = ((mctrl->time * sample_units[mctrl->sample_unit].size)
			* 1000) + 100;
	usleep_range(min_sleep, max_sleep);

	memctrl_stop(mctrl);

	/* Dump each channel */
	memset(total_data, 0, sizeof(*total_data));
	memctrl_get_global_data(mctrl, gdata);
	memctrl_dump_start(mctrl);
	for (i = 0; i < ARRAY_SIZE(n7_channels); i++) {
		cinfo = &n7_channels[i];
		memctrl_get_channel_data(mctrl, cinfo->hwnum, cdata);
		memctrl_channel_trace(total_data, cdata);
		memctrl_dump_channel(cinfo->name, gdata, cdata, mctrl);
	}
	memctrl_dump_total(gdata, total_data, mctrl);
	memctrl_dump_end(mctrl);
}

static void memctrl_dump(struct lombo_memctrl *mctrl)
{
	PRT_DBG(
	"time:%d, period:%d, count:%d, sample:%s, flaw_unit:%s, data_unit:%s\n",
		mctrl->time, mctrl->period, mctrl->count,
		sample_units[mctrl->sample_unit].name,
		flaw_units[mctrl->flaw_unit].name,
		data_units[mctrl->size_unit].name);

	/* Not support now, should be false */
	if (mctrl->op == OP_FILE) {
		mctrl->pfile = filp_open(mctrl->file, O_WRONLY | O_CREAT, 0644);
		if (!mctrl->pfile) {
			PRT_ERR("fail to open file: %s\n", mctrl->file);
			return;
		}
		mctrl->fpos = 0;
	}
	/* Reset log buffer index */
	mctrl->log_offset = 0;

	memctrl_dump_once(mctrl);

	/* Not support now, should be false */
	if (mctrl->op == OP_FILE)
		filp_close(mctrl->pfile, NULL);
}

static irqreturn_t lombo_memctrl_irq(int irq, void *dev_id)
{
	struct lombo_memctrl *mctrl = (struct lombo_memctrl *)dev_id;
	u32 latency;

	if (csp_memctrl_is_cycle_overflow(mctrl->reg))
		PRT_WARN("cycle overflow!! Statistics may be incorrect\n");

	if (csp_memctrl_is_data_overflow(mctrl->reg))
		PRT_WARN("data overflow!! Statistics may be incorrect\n");

	csp_memctrl_clear_status(mctrl->reg);

	latency = csp_memctrl_get_rd_latency_status(mctrl->reg);
	if (latency) {
		PRT_WARN("read latency overflow!! status:0x%x\n", latency);
		csp_memctrl_clear_rd_latency_status(mctrl->reg);
	}

	latency = csp_memctrl_get_wr_latency_status(mctrl->reg);
	if (latency) {
		PRT_WARN("write latency overflow!! status:0x%x\n", latency);
		csp_memctrl_clear_wr_latency_status(mctrl->reg);
	}

	return IRQ_HANDLED;
}

/**
 * lombo_memctrl_wr_bw_limit - fix AX write band width
 *
 * @master_id: master id describe in memctrl spec
 * @cycle: time window cycle
 * @band_width: data size in time window
 *
 * Returns value is 1 on success, a negative error code on failure.
 */
int lombo_memctrl_wr_bw_limit(int master_id, u32 cycle, u32 band_width)
{
	int ret = 1;
	struct channel_info *cinfo;
	struct lombo_memctrl *mctrl = pmemctrl;
	int master = master_id;

	if (cycle <= 0 || band_width <= 0)
		return -EINVAL;

	cinfo = &n7_channels[master];
	/* E3006 just support limit AX band width */
	if (!strcmp(cinfo->name, "AX")) {
		csp_memctrl_set_ax_wr_timewin(mctrl->reg, cycle);
		csp_memctrl_set_ax_wr_datasize(mctrl->reg, band_width);
		csp_memctrl_set_ax_wr_ctrl_mode(mctrl->reg, 2);
		return ret;
	}
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(lombo_memctrl_wr_bw_limit);

/**
 * lombo_memctrl_rd_bw_limit - fix AX read band width
 *
 * @master_id: master id describe in memctrl spec
 * @cycle: time window cycle
 * @band_width: data size in time window
 *
 * Returns value is 1 on success, a negative error code on failure.
 */
int lombo_memctrl_rd_bw_limit(int master_id, u32 cycle, u32 band_width)
{
	int ret = 1;
	struct channel_info *cinfo;
	struct lombo_memctrl *mctrl = pmemctrl;
	int master = master_id;

	if (cycle <= 0 || band_width <= 0)
		return -EINVAL;

	cinfo = &n7_channels[master];
	/* E3006 just support limit AX band width */
	if (!strcmp(cinfo->name, "AX")) {
		csp_memctrl_set_ax_rd_timewin(mctrl->reg, cycle);
		csp_memctrl_set_ax_rd_datasize(mctrl->reg, band_width);
		csp_memctrl_set_ax_rd_ctrl_mode(mctrl->reg, 2);
		return ret;
	}
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(lombo_memctrl_rd_bw_limit);

int lombo_memctrl_get_bw_info(const char *buf, u32 *master_id, u32 *bits_id,
				u32 *val)
{
	int i;
	char *master, *bits_name;

	*master_id = -1;
	*bits_id = -1;
	*val = -1;

	/* get master */
	for (i = 0; i < sizeof(n7_channels)/sizeof(n7_channels[0]); i++) {
		master = strstr(buf, n7_channels[i].name);
		if (master) {
			master = (char *)n7_channels[i].name;
			*master_id = i;
			break;
		}
	}
	if (*master_id == -1)
		return 0;

	/* get bits_name */
	for (i = 0; i < sizeof(n7_bw_default)/sizeof(n7_bw_default[0]); i++) {
		if (strstr(buf, n7_bw_default[i].name) ||
					strstr(buf, n7_bw_default[i].ref)) {
			bits_name = (char *)n7_bw_default[i].name;
			*bits_id = i;
			break;
		}
	}
	if (*bits_id == -1)
		return 0;

	/* get val to be set */
	if (strstr(buf, "default") || strstr(buf, "def"))
		*val = n7_bw_default[*bits_id].val;
	else {
		if (kstrtol((strrchr(buf, '-') + 1), 10, (long *)val) ||
					*val <= 0)
			*val = -1;
	}
	if (*val == -1)
		return 0;

	return 1;
}

static const struct file_operations lombo_memctrl_fops = {
	.open		= NULL,
	.release	= NULL,
	.write		= NULL,
	.read		= NULL,
	.llseek		= NULL,
};

static ssize_t lombo_memctrl_bandwidth_limit_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int count, i = 9;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	/* Reset log buffer index */
	mctrl->log_offset = 0;

	mop(mctrl, "%s\n%s\n%s%s\n%s\n",
	"usage: <master-bits_name-value> set band width limit",
	"master just AX.",
	"bits_name:timewindow(tw),datasize(ds), randmode(rm),",
	"ctrlmode(cm),randomseed(rs).value set default(def) will reset bits",
	"e.g. AX-tw-300 AX-tw-def");

	/* Split line */
	while (i--)
		mop(mctrl, "----------");
	mop(mctrl, "\n");

	mop(mctrl, "\n  %-10s %10s %10s %10s %10s %10s %10s\n",
			"MASTER", "MODE", "TW", "DS", "RM", "CM", "RS");

	mop(mctrl, "\n  %-10s %10s %10d %10d %10d %10d %10d\n",
			"AX", "write",
			csp_memctrl_get_ax_wr_bw_timewin(mctrl->reg),
			csp_memctrl_get_ax_wr_bw_datasize(mctrl->reg),
			csp_memctrl_get_ax_wr_bw_randomode(mctrl->reg),
			csp_memctrl_get_ax_wr_ctrl_mode(mctrl->reg),
			csp_memctrl_get_ax_wr_bw_seed(mctrl->reg));

	mop(mctrl, "\n  %-10s %10s %10d %10d %10d %10d %10d\n",
			"AX", "read",
			csp_memctrl_get_ax_rd_bw_timewin(mctrl->reg),
			csp_memctrl_get_ax_rd_bw_datasize(mctrl->reg),
			csp_memctrl_get_ax_rd_bw_randomode(mctrl->reg),
			csp_memctrl_get_ax_rd_ctrl_mode(mctrl->reg),
			csp_memctrl_get_ax_rd_bw_seed(mctrl->reg));

	count = snprintf(buf, PAGE_SIZE, "%s", mctrl->log_buf);

	return count + 1;
}

static ssize_t lombo_memctrl_bw_rd_limit_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	u32 val, master_id, bits_id;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	if (!lombo_memctrl_get_bw_info(buf, &master_id, &bits_id, &val))
		goto error;

	switch (master_id) {
	case MEMCTRL_AX:
		switch (bits_id) {
		case BW_TIMEWINDOW:
			csp_memctrl_set_ax_rd_timewin(mctrl->reg, val);
			break;
		case BW_DATASIZE:
			csp_memctrl_set_ax_rd_datasize(mctrl->reg, val);
			break;
		case BW_RANDOMMODE:
			if (val < 0 || val > 3)
				goto error;
			csp_memctrl_set_ax_rd_bw_randomode(mctrl->reg, val);
			break;
		case BW_CTRLMODE:
			if (val < 0 || val > 3)
				goto error;
			csp_memctrl_set_ax_rd_ctrl_mode(mctrl->reg, val);
			break;
		case BW_RANDOMSEED:
			if (val <= 0 || val > 8388607) /* 23 bit max number */
				goto error;
			csp_memctrl_set_ax_rd_bw_seed(mctrl->reg, val);
			break;
		default:
			goto error;
			break;
		}
		break;
	default:
		goto error;
		break;
	}
	return count;

error:
	PRT_ERR("%s\n", "bad band width limit");
	return count;
}

static DEVICE_ATTR(memctrl_bandwidth_rd_limit,
			S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_bandwidth_limit_show,
			lombo_memctrl_bw_rd_limit_store);

static ssize_t lombo_memctrl_bw_wr_limit_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	u32 val, master_id, bits_id;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	if (!lombo_memctrl_get_bw_info(buf, &master_id, &bits_id, &val))
		goto error;

	switch (master_id) {
	case MEMCTRL_AX:
		switch (bits_id) {
		case BW_TIMEWINDOW:
			csp_memctrl_set_ax_wr_timewin(mctrl->reg, val);
			break;
		case BW_DATASIZE:
			csp_memctrl_set_ax_wr_datasize(mctrl->reg, val);
			break;
		case BW_RANDOMMODE:
			if (val < 0 || val > 3)
				goto error;
			csp_memctrl_set_ax_wr_bw_randomode(mctrl->reg, val);
			break;
		case BW_CTRLMODE:
			if (val < 0 || val > 3)
				goto error;
			csp_memctrl_set_ax_wr_ctrl_mode(mctrl->reg, val);
			break;
		case BW_RANDOMSEED:
			if (val < 0 || val > 8388607) /* 23 bit max number */
				goto error;
			csp_memctrl_set_ax_wr_bw_seed(mctrl->reg, val);
			break;
		default:
			goto error;
			break;
		}
		break;
	default:
		goto error;
		break;
	}
	return count;

error:
	PRT_ERR("%s\n", "bad band width limit");
	return count;
}

static DEVICE_ATTR(memctrl_bandwidth_wr_limit,
			S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_bandwidth_limit_show,
			lombo_memctrl_bw_wr_limit_store);

static ssize_t lombo_memctrl_size_unit_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int i;
	ssize_t offset;
	int count;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	count = snprintf(buf, PAGE_SIZE, "memctrl current size unit:%s\n",
				data_units[mctrl->size_unit].name);
	offset = count;

	count = snprintf(buf + offset, PAGE_SIZE - offset,
					"you can also choose:\n");
	offset += count;

	for (i = 0; i < sizeof(data_units) / sizeof(data_units[0]); ++i) {
		if (i == mctrl->size_unit)
			continue;
		count = snprintf(buf + offset, PAGE_SIZE - offset,
					"unit=%s\n", data_units[i].name);
		offset += count;
	}

	count = snprintf(buf + offset, PAGE_SIZE - offset,
					"useage: echo 'unit' > memctrl_size_unit\n");
	offset += count;

	return offset;
}

static ssize_t lombo_memctrl_size_unit_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	const char *unit;
	int i;

	for (i = 0; i < ARRAY_SIZE(data_units); i++) {
		unit = data_units[i].name;
		if (!strncmp(unit, buf, count - 1))
			break;
	}

	if (i >= ARRAY_SIZE(data_units))
		return -EINVAL;
	mctrl->size_unit = i;

	return count;
}

static DEVICE_ATTR(memctrl_size_unit,
			S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_size_unit_show,
			lombo_memctrl_size_unit_store);

static ssize_t lombo_memctrl_flaw_unit_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int i;
	ssize_t offset;
	int count;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	count = snprintf(buf, PAGE_SIZE, "memctrl current flaw unit:%sB\n",
				flaw_units[mctrl->flaw_unit].name);
	offset = count;

	count = snprintf(buf + offset, PAGE_SIZE - offset,
						"you can also choose:\n");
	offset += count;

	for (i = 0; i < sizeof(flaw_units) / sizeof(flaw_units[0]); ++i) {
		if (i == mctrl->flaw_unit)
			continue;
		count = snprintf(buf + offset, PAGE_SIZE - offset,
					"unit=%s\n", flaw_units[i].name);
		offset += count;
	}

	count = snprintf(buf + offset, PAGE_SIZE - offset,
					"useage: echo 'unit' > memctrl_flaw_unit\n");
	offset += count;

	return offset;
}

static ssize_t lombo_memctrl_flaw_unit_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	const char *unit;
	int i;

	for (i = 0; i < ARRAY_SIZE(flaw_units); i++) {
		unit = flaw_units[i].name;
		if (!strncmp(unit, buf, count - 1))
			break;
	}

	if (i >= ARRAY_SIZE(flaw_units))
		return -EINVAL;
	mctrl->flaw_unit = i;

	return count;
}

static DEVICE_ATTR(memctrl_flaw_unit,
			S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_flaw_unit_show,
			lombo_memctrl_flaw_unit_store);

static ssize_t lombo_memctrl_sample_unit_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int i;
	ssize_t offset;
	char sample_unit[64];
	const char *sample_format = " (num = %d): %d%s";
	int count;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	snprintf(sample_unit, sizeof(sample_unit), sample_format,
			sample_units[mctrl->sample_unit].size,
			sample_units[mctrl->sample_unit].size
			/ sample_units[mctrl->sample_unit].div,
			sample_units[mctrl->sample_unit].unit);

	count = snprintf(buf, PAGE_SIZE, "memctrl current sample unit:%s\n",
				sample_unit);
	offset = count;

	count = snprintf(buf + offset, PAGE_SIZE - offset,
						"you can also choose:\n");
	offset += count;

	for (i = 0; i < sizeof(sample_units) / sizeof(sample_units[0]); ++i) {
		if (i == mctrl->sample_unit)
			continue;

		snprintf(sample_unit, sizeof(sample_unit), sample_format,
				sample_units[i].size,
				sample_units[i].size
				/ sample_units[i].div,
				sample_units[i].unit);

		count = snprintf(buf + offset, PAGE_SIZE - offset,
					"%s\n", sample_unit);
		offset += count;
	}

	count = snprintf(buf + offset, PAGE_SIZE - offset,
				"useage:echo 'num' > memctrl_sample_unit\n");
	offset += count;

	return offset;
}

static ssize_t lombo_memctrl_sample_unit_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	unsigned long val, size;
	int i;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;

	for (i = 0; i < ARRAY_SIZE(sample_units); i++) {
		size = sample_units[i].size;
		if (val == size)
			break;
	}

	if (i >= ARRAY_SIZE(sample_units))
		return -EINVAL;
	mctrl->sample_unit = i;

	return count;
}

static DEVICE_ATTR(memctrl_sample_unit,
			S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_sample_unit_show,
			lombo_memctrl_sample_unit_store);

static ssize_t lombo_memctrl_time_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	char time_unit[10];
	const char *time_format = "%d%s";
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	snprintf(time_unit, sizeof(time_unit), time_format,
			sample_units[mctrl->sample_unit].size * mctrl->time
			/ sample_units[mctrl->sample_unit].div,
			sample_units[mctrl->sample_unit].unit);

	return snprintf(buf, PAGE_SIZE, "memctrl current time:%s\n",
								time_unit);
}

static ssize_t lombo_memctrl_time_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	unsigned long val;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;
	mctrl->time = val;

	return count;
}

static DEVICE_ATTR(memctrl_time, S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_time_show,
			lombo_memctrl_time_store);

static ssize_t lombo_memctrl_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	int count;
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	mctrl->running = true;
	memctrl_dump(mctrl);
	mctrl->running = false;

	buf[0] = '$';
	count = snprintf(buf + 1, PAGE_SIZE, "%s", mctrl->log_buf);

	return count + 1;
}

static ssize_t lombo_memctrl_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	return count;
}

static DEVICE_ATTR(memctrl, S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_show, lombo_memctrl_store);

struct master_info {
	char *name;
	int level;
};

struct master_info mctrl_master_prio[] =  {
	{
		.name = "CPU",
		.level = MEMCTRL_CPU
	},
	{
		.name = "VC",
		.level = MEMCTRL_VC
	},
	{
		.name = "AX",
		.level = MEMCTRL_AX
	},
	{
		.name = "VISS0",
		.level = MEMCTRL_VISS0
	},
	{
		.name = "VISS1",
		.level = MEMCTRL_VISS1
	},
	{
		.name = "DPU",
		.level = MEMCTRL_DPU
	},
	{
		.name = "DMA",
		.level = MEMCTRL_DMA
	},
	{
		.name = "USB",
		.level = MEMCTRL_USB
	},
	{
		.name = "SDC0_2",
		.level = MEMCTRL_SDC0_2
	},
	{
		.name = "SDC1",
		.level = MEMCTRL_SDC1
	},
};

static int master_2_channel(const char *master)
{
	int i;

	if (master == NULL)
		return -1;

	for (i = 0; i < ARRAY_SIZE(mctrl_master_prio); i++) {
		if (!strcmp(master, mctrl_master_prio[i].name))
			return mctrl_master_prio[i].level;
	}

	return -1;
}

static void memctrl_set_priority(struct lombo_memctrl *mctrl, int channel,
					int level, bool write)
{
	u32 addr, value;

	if (0 > channel || channel >= MEMCRTL_MAX)
		return;
	if (0 > level || level > 0x3ff)
		return;

	if (write)
		addr = csp_memctrl_get_pri_w(channel);
	else
		addr = csp_memctrl_get_pri_r(channel);

	value = READREG32(addr);
	WRITEREG32(addr, (value & ~0x3ff) | (1 << 12) | (level & 0x3ff));
}

static int memctrl_get_priority(struct lombo_memctrl *mctrl,
						int channel, bool write)
{
	u32 addr, value;

	if (0 > channel || channel >= MEMCRTL_MAX)
		return -1;

	if (write)
		addr = csp_memctrl_get_pri_w(channel);
	else
		addr = csp_memctrl_get_pri_r(channel);
	value = READREG32(addr) & 0x3ff;

	return value;
}

static void lombo_memctrl_priority_show(struct lombo_memctrl *mctrl)
{
	pr_warn("help:\n %s %s %s %s %s %s %s",
		"read/write priority of masters, low level[0~1023] is higher priority\n",
		"echo <master> > get_read_prioriry\n",
		"echo <master>=<level> > set_read_prioriry\n",
		"echo <master> > get_write_prioriry\n",
		"echo <master>=<level> > set_write_prioriry\n",
		"master:CPU, VC, AX, VISS0, VISS1, DPU, DMA, USB, SDC0_2, SDC1\n",
		"example: echo CPU=1 > set_read_prioriry\n");
}

static ssize_t lombo_memctrl_priority_help_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);

	lombo_memctrl_priority_show(mctrl);
	return snprintf(buf, PAGE_SIZE, "\n");
}

static ssize_t lombo_memctrl_get_read_priority_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	unsigned long channel, level;
	char *val;

	/* replace '\n' width '\0' */
	val = strchr(buf, '\n');
	if (!val)
		goto exit;
	*val = '\0';

	channel = master_2_channel(buf);
	if (channel == -1)
		goto exit;

	level = memctrl_get_priority(mctrl, channel, false);
	pr_warn("%ld\n", level);

	return count;
exit:
	lombo_memctrl_priority_show(mctrl);

	return count;
}

static DEVICE_ATTR(get_read_priority, S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_priority_help_show,
			lombo_memctrl_get_read_priority_store);

static ssize_t lombo_memctrl_set_read_priority_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	unsigned long channel, level;
	char *val;

	val = strchr(buf, '=');
	if (!val)
		goto exit;

	/* replace '=' width '\0' */
	*(val++) = '\0';

	if (kstrtoul(val, 0, &level))
		goto exit;

	channel = master_2_channel(buf);
	if (channel == -1)
		goto exit;

	memctrl_set_priority(mctrl, channel, level, false);

	return count;
exit:
	lombo_memctrl_priority_show(mctrl);

	return count;
}

static DEVICE_ATTR(set_read_priority, S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_priority_help_show,
			lombo_memctrl_set_read_priority_store);

static ssize_t lombo_memctrl_get_write_priority_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	unsigned long channel, level;
	char *val;

	/* replace '\n' width '\0' */
	val = strchr(buf, '\n');
	if (!val)
		goto exit;
	*val = '\0';

	channel = master_2_channel(buf);
	if (channel == -1)
		goto exit;

	level = memctrl_get_priority(mctrl, channel, true);
	pr_warn("%ld\n", level);

	return count;
exit:
	lombo_memctrl_priority_show(mctrl);

	return count;
}

static DEVICE_ATTR(get_write_priority, S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_priority_help_show,
			lombo_memctrl_get_write_priority_store);

static ssize_t lombo_memctrl_set_wirte_priority_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(dev);
	unsigned long channel, level;
	char *val;

	val = strchr(buf, '=');
	if (!val)
		goto exit;

	/* replace '=' width '\0' */
	*(val++) = '\0';

	if (kstrtoul(val, 0, &level))
		goto exit;

	channel = master_2_channel(buf);
	if (channel == -1)
		goto exit;

	memctrl_set_priority(mctrl, channel, level, true);

	return count;
exit:
	lombo_memctrl_priority_show(mctrl);

	return count;
}

static DEVICE_ATTR(set_write_priority, S_IRUGO | S_IWUSR | S_IWGRP | S_IWOTH,
			lombo_memctrl_priority_help_show,
			lombo_memctrl_set_wirte_priority_store);

static struct attribute *lombo_memctrl_attributes[] = {
	&dev_attr_memctrl.attr,
	&dev_attr_memctrl_time.attr,
	&dev_attr_memctrl_sample_unit.attr,
	&dev_attr_memctrl_flaw_unit.attr,
	&dev_attr_memctrl_size_unit.attr,
	&dev_attr_get_read_priority.attr,
	&dev_attr_set_read_priority.attr,
	&dev_attr_get_write_priority.attr,
	&dev_attr_set_write_priority.attr,
	&dev_attr_memctrl_bandwidth_rd_limit.attr,
	&dev_attr_memctrl_bandwidth_wr_limit.attr,
	NULL,
};

static const struct attribute_group lombo_memctrl_attr_group = {
	.attrs = lombo_memctrl_attributes,
};

static int lombo_memctrl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev, *this_device;
	struct resource *res;
	struct lombo_memctrl *mctrl;
	struct clk *sdram_bw_clk, *mem_axi_clk;
	int ret;

	mctrl = devm_kzalloc(dev, sizeof(*mctrl), GFP_KERNEL);
	if (!mctrl) {
		PRT_ERR("failed to alloc memory\n");
		return -ENOMEM;
	}

	/* Default parameters */
	mctrl->time = 1;
	mctrl->period = 1;
	mctrl->count = 1;
	mctrl->size_unit = 0;
	mctrl->flaw_unit = 0;
	mctrl->sample_unit = 0;
	mctrl->op = OP_CONSOLE;
	memset(mctrl->file, 0, sizeof(mctrl->file));
	dev_set_drvdata(dev, mctrl);
	pmemctrl = mctrl;

	/* Get register map */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mctrl->reg = devm_ioremap_resource(dev, res);
	if (IS_ERR(mctrl->reg)) {
		PRT_ERR("reg config invalid\n");
		return PTR_ERR(mctrl->reg);
	}
	PRT_INFO("reg:%p\n", mctrl->reg);

	/* Get pwm system irq number */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		PRT_ERR("failed to get irq resource\n");
		return -EINVAL;
	}
	mctrl->irq = res->start;
	PRT_INFO("irq:0x%x\n", mctrl->irq);

	/* clk resource */
	mem_axi_clk = devm_clk_get(dev, "mem_axi_clk");
	if (IS_ERR(mem_axi_clk)) {
		PRT_ERR("get mem axi clk failed\n");
		return -EINVAL;
	}
	mctrl->lat_div_us = clk_get_rate(mem_axi_clk) / 1000000;
	PRT_INFO("lat_div_us:0x%x\n", mctrl->lat_div_us);

	sdram_bw_clk = devm_clk_get(dev, "sdram_bandwidth_clk");
	if (IS_ERR(sdram_bw_clk)) {
		PRT_ERR("get sdram band width clk failed\n");
		return -EINVAL;
	}
	mctrl->sdram_bw_clk = sdram_bw_clk;

	ret = clk_prepare_enable(sdram_bw_clk);
	if (ret) {
		PRT_ERR("enable sdram band width clk failed\n");
		return ret;
	}

	/* Manual mode */
	csp_memctrl_set_mode(mctrl->reg, true);
	csp_memctrl_set_clk(mctrl->reg, MEMCTRL_CLK_24M);
	csp_memctrl_set_data_unit(mctrl->reg, MEMCTRL_UNIT_1B);
	csp_memctrl_set_sample(mctrl->reg, MEMCTRL_SAMPLE_1MS);

	/* Request the IRQ */
	ret = request_irq(mctrl->irq, lombo_memctrl_irq, IRQF_SHARED,
					"memctrl", mctrl);
	if (ret) {
		PRT_ERR("Unable to request IRQ %d\n", mctrl->irq);
		goto out_clk_disable;
	}

	/* Setup the miscdevice */
	mctrl->miscdev.minor = MISC_DYNAMIC_MINOR;
	mctrl->miscdev.name = "memctrl";
	mctrl->miscdev.fops = &lombo_memctrl_fops;

	/* Register the miscdevice */
	ret = misc_register(&mctrl->miscdev);
	if (ret) {
		PRT_ERR("Unable to register miscdevice\n");
		goto out_free_irq;
	}

	/* Create the sysfs files */
	this_device = mctrl->miscdev.this_device;
	ret = sysfs_create_group(&this_device->kobj, &lombo_memctrl_attr_group);
	if (ret) {
		PRT_ERR("Unable to create sysfs files\n");
		goto out_misc_deregister;
	}

	return 0;

out_misc_deregister:
	misc_deregister(&mctrl->miscdev);
out_free_irq:
	free_irq(mctrl->irq, mctrl);
out_clk_disable:
	clk_disable_unprepare(mctrl->sdram_bw_clk);

	return ret;
}

int lombo_memctrl_remove(struct platform_device *pdev)
{
	struct lombo_memctrl *mctrl = dev_get_drvdata(&pdev->dev);

	misc_deregister(&mctrl->miscdev);

	free_irq(mctrl->irq, mctrl);

	clk_disable_unprepare(mctrl->sdram_bw_clk);

	return 0;
}

static struct of_device_id lombo_memctrl_of_match[] = {
	{ .compatible = "lombo,memctrl", },
	{},
};

static struct platform_driver lombo_memctrl_driver = {
	.probe		= lombo_memctrl_probe,
	.remove		= lombo_memctrl_remove,
	.driver		= {
		.name		= "lombo-memctrl",
		.of_match_table	= lombo_memctrl_of_match,
		.owner		= THIS_MODULE,
	},
};

static int __init lombo_memctrl_init(void)
{
	return platform_driver_register(&lombo_memctrl_driver);
}

static void __exit lombo_memctrl_exit(void)
{
	platform_driver_unregister(&lombo_memctrl_driver);
}

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech memctrl controller");
MODULE_LICENSE("GPL");

module_init(lombo_memctrl_init);
module_exit(lombo_memctrl_exit);

