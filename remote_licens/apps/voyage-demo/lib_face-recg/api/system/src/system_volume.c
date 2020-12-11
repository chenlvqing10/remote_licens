/*
 * system_volume.c - Interface to modify the volume
 * information from Excel.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "system_volume.h"
#include "sqlite3.h"
#include "db_device_hw_param.h"
#include "lombo_sound_mixer.h"

#undef LOG_TAG
#define LOG_TAG "SYSTEM_VOLUME"
#include "log/log.h"

/**
 * Get volume value from db and audio module
 * @volume IN, a pointer of saving volume value
 * Returns 0 successfully, other , failed.
 */
int system_volume_get_value(int *volume)
{
	int cout = 0;
	device_hw_param_t info;

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		*volume = info.volume;
	} else {
		ALOGE("%s: read hw para failed", __func__);
		return -1;
	}

	return 0;
}

/**
 * set system volume
 * @volume IN, the current volume value
 * Returns 0 successful, -1 set volume failed.
 */
int system_volume_set_value(int volume)
{
	int ret = -1;
	int cout = 0;
	device_hw_param_t info;
	int tmp_volume = 0;

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		tmp_volume = volume;
		/*set value to audio module*/
		ret = lombo_sound_mixer(LOMBO_SOUND_SET_VOLUME, &tmp_volume);
		if (ret) {
			ALOGE("%s: set volume failed", __func__);
			return -1;
		}
		/*save volume value to db*/
		tmp_volume = info.volume;
		info.volume = volume;
		ret = db_device_hw_param_update(&info);
		if (SQLITE_OK != ret) {
			ALOGE("%s: save volume to db failed", __func__);
			lombo_sound_mixer(LOMBO_SOUND_SET_VOLUME, &tmp_volume);
			return -1;
		}
	} else {
		ALOGE("%s: read hw para failed", __func__);
		return -1;
	}

	return 0;

}

/**
 * init system volume
 * @NULL
 * Returns NULL
 */
int system_volume_init(void)
{
	int ret = -1;
	int cout = 0;
	device_hw_param_t info;
	int tmp_volume = 0;

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		tmp_volume = info.volume;
		ret = lombo_sound_mixer(LOMBO_SOUND_SET_VOLUME, &tmp_volume);
		if (ret) {
			ALOGE("%s: init volume failed", __func__);
			return -2;
		}
	} else {
		ALOGE("%s: read hw para failed", __func__);
		return -1;
	}

	return 0;
}

