/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Camera Control Core driver header for LomboTech
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

#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/lombo_viss.h>
#include <linux/delay.h>

enum cc_pwr_act {
	CC_RESET,
	CC_PWDN,
	CC_DELAY
};

enum cc_pin_status {
	CC_DOWN,
	CC_UP
};

struct cc_power_act {
	u8	act;
	u8	ud;
};

typedef struct cc_sensor_reg cc_reg_t;
struct cc_sensor_reg {
	u32	reg;
	u32	val;
};

struct cc_frame_format {
	u32		mbus_code;	/* enum v4l2_mbus_pixelcode */
	u32		colorspace;	/* enum v4l2_colorspace */
	u32		field;
	const cc_reg_t	*regs;
	u32		regs_cnt;
	void		*prv_data;
};

struct cc_win_size {
	u32		left;
	u32		top;
	u32		width;
	u32		height;
	u32		vts;
	u32		hts;
	u32		pclk;

	const cc_reg_t	*regs;
	u32		regs_cnt;
	void		*prv_data;
};

struct cc_frame_cfg {
	u32			width;
	u32			height;

	struct v4l2_fract	framerate;
	const cc_reg_t		*regs;
	u32			regs_cnt;
	u32			mfreq;

	/* width and height of a single channel
	 * Multi channel config,
	 * single channel does not need to be configured
	 */
	u32			sub_width;
	u32			sub_height;
};

struct cc_sensor_info {
	u16			i2c_bus_num;
	u16			i2c_dev_addr;
	u16			i2c_ext_addr;
	u8			reg_bits;
	u8			val_bits;

	u32			clk_frequency;
	enum viss_if_type	if_type;
	u32			if_mode;
	u32			flags;
	bool			pclk_polarity;
	bool			vsync_polarity;

	u32			isp_id;
	u32			isp_path;
	bool			vdd_1_8;

	bool			vflip;
	bool			mirror;

	union {
		u32		interlaced;
		u32		dlanes;
	};

	u8			hdr_flag;
	u8			hdr_mode;

	u8			sort;
	u8			ch_number;
	/* Use 0 to ch_number DMA channels */

	/* dc/bt601/bt656/bt1120/mipi-csi */
	union {
		struct mcsi_if_info mcsi_if_info;
		struct bt601_timing bt601_timing;
	} if_info;

	char			*name;
	char			*ext_name;

	struct cc_power_act	*power_up_act;
	struct cc_power_act	*power_down_act;
	int			npwr_up_acts;
	int			npwr_dn_acts;

	const struct cc_frame_format	*framefmts;
	int			nfrmfmts;

	const struct cc_win_size	*win_sizes;
	int			nwsizes;

	const struct cc_frame_cfg	*framecfgs;
	int			nfrmcfgs;

	/* Protects the struct fields below */
	struct mutex		lock;
	struct i2c_client	*client;
	struct i2c_client	*ext_client[2];
	struct list_head	list;

	int			reset_pin;
	int			pwdn_pin;
	int			led_pin;
	int			cam_en_pin;

	const struct cc_frame_format	*cur_ccfmt;
	const struct cc_win_size	*cur_wsize;
	const struct cc_frame_cfg	*cur_frmcfg;

	struct v4l2_mbus_framefmt	format;
	struct delayed_work		sensor_work;

	int			streaming;
	int			power;

	unsigned long		priv_data;

	/* register from cccore */
	int (*power_up)(struct cc_sensor_info *cc_si);
	int (*power_down)(struct cc_sensor_info *cc_si);
	int (*write_reg)(struct cc_sensor_info *cc_si, u32 reg, u32 val);
	int (*read_reg)(struct cc_sensor_info *cc_si, u32 reg, void *val);
	int (*write_reg_list)(struct cc_sensor_info *cc_si,
				const cc_reg_t *regs_list, u32 regs_cnt);

	/* sensor implement */
	int (*detect)(struct cc_sensor_info *cc_si);
	int (*sensor_init)(struct cc_sensor_info *cc_si);
	int (*set_stream)(struct cc_sensor_info *cc_si, int on);
	long (*sensor_ioctl)(struct cc_sensor_info *cc_si, unsigned int cmd,
				void *arg);
};

static inline void sensor_msleep(unsigned int msecs)
{
#if 1
	if (msecs < 20)
		usleep_range(msecs * 1000, msecs * 1000 + 50);
	else
		msleep(msecs);
#else
	mdelay(msecs);
#endif
}

#define CC_REG_DELAY	(0xFFFFFFFF)
int cc_set_reg_ops(struct cc_sensor_info *cc_si);

struct cc_framesize {
	int	width;
	int	height;
	int	vts;
	int	framerate;
	const struct cc_sensor_reg *regs; /* Regs to tweak */
};

enum cc_if_type {
	CC_IF_DVP,
	CC_IF_MCSI,
	CC_IF_CNT,
};

struct cccore {
	struct device *dev;
	struct device class_dev;

	struct mutex		lock;
	struct list_head sensor_list[CC_IF_CNT];
	struct cc_sensor_info *cur_sensor[CC_IF_CNT];
	struct v4l2_subdev	sd[CC_IF_CNT];
	struct media_pad	pad[CC_IF_CNT];

	u32	cur_sensor_reg[CC_IF_CNT];

	int	reset_pin[CC_IF_CNT];
	int	pwdn_pin[CC_IF_CNT];
	int	led_pin;
	int	cam_en_pin[CC_IF_CNT];

	bool	skip_detect;
};

int cc_sensor_probe(struct platform_device *pdev,
			struct cc_sensor_info *cc_si);
int cc_sensor_remove(struct platform_device *pdev);

int cc_detect_sensor(struct cccore *ccc, int force);

void cc_switch_i2c_dev(struct cc_sensor_info *cc_si, u32 id);

struct v4l2_subdev *cccore_get_dvp_sensor_subdev(void);
struct v4l2_subdev *cccore_get_mcsi_sensor_subdev(void);
