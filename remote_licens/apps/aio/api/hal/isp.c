/*
 * isp.c - Define function for operate isp
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define LOG_TAG "system_isp"
#include <log/log.h>

#include "isp.h"
#include "face_svc_isp.h"

/*
* isp_get_brightness - get brightness from isp
* @type: camera type
*
* return 0 if success, otherwise return -1
*/
int isp_get_brightness(enum face_camera_id type)
{
	int ret = 0;
	struct viss_isp_exif exif;

	memset(&exif, 0, sizeof(exif));
	if (type == FACE_CAMERA_RGB) {
		ret = get_rgb_sensor_exif(0, &exif);
		if (ret < 0) {
			ALOGE("%s %d, ioctl failed %d.\n", __func__, __LINE__, ret);
			return -1;
		}
	} else if (type == FACE_CAMERA_NIR) {
		ret = get_nir_sensor_exif(0, &exif);
		if (ret < 0) {
			ALOGE("%s %d, ioctl failed %d.\n", __func__, __LINE__, ret);
			return -1;
		}
	}

	return exif.brightness;
}

