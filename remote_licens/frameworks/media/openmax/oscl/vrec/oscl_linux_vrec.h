/*
 * oscl_linux_vrec.h - eos video receive api used by lombo media framework.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __OSCL_LINUX_VREC_H__
#define __OSCL_LINUX_VREC_H__

#include <sys/ioctl.h>
#include <poll.h>
#include <base_component.h>

#include "oscl_vrec_i.h"
#include "../../../hal/include/v4l2_camera.h"

typedef struct vrec_hdl {
	char name[10];		/* video receive device name */
	OMX_S32 v4l2_fd;
	OMX_S32 isp_fd;
	vrec_info_t info;	/* sensor information */
	vrec_pts_cb_t pts_cb;
	OMX_S32 plans;
	struct v4l2_plane *planes_qbuf;
} vrec_hdl_t;

/**
 * struct viss_isp_exif - structure for the EXIF information of VISS
 * @exposure_time: exposure time(s) * 10000
 * @shutter_speed:  exposure time(s) * 10000
 * @aperture: aperture fno2.8 = 280
 * @brightness: LV = [0, 20], 0: star light, 20: sun light
 * @exposure_bias: it calls also EV bias
 * @iso_speed: ISO = gain * 100
 * @flash: status register value of the flash
 * @illu_id: AWB temperature id
 *    0: HOR(2100K)
 *    1: A(2900K)
 *    2: TL84(4000K)
 *    3: CWF(4100K)
 *    4: D50(5000K)
 *    5: D65(6500K)
 *    6: D75(7500K)
 * @back_score: back light score = [0, 100] percent
 * @res: reserved info
 * @res[0]: object luminance
 * @res[1]: back luminance
 * @res[2]: average luminance
 * @res[3]: original luminance
 * @res[4]: final luminance
 */
typedef struct viss_isp_exif {
	__u32 exposure_time;
	__u32 shutter_speed;
	__u32 aperture;
	__u32 brightness;
	__u32 exposure_bias;
	__u32 iso_speed;
	__u32 flash;
	__u32 illu_id;
	__u32 back_score;
	__u32 res[16];
} viss_isp_exif_t;


#endif

