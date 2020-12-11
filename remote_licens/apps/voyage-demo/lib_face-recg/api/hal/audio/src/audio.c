/*
 * app_md.c - define some function module
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

#include "audio.h"
#define LOG_TAG "audio"
#include <log/log.h>


#define MAX_VOLUME 16
#define MIN_VOLUME 0

/**
 * set system volume
 * @param volume volume value
 * @return 0-success, other-failed
 */
int audio_set_volume(int volume)
{
	int ret = 0;

	/*verify parameter*/
	if ((volume < MIN_VOLUME) || (volume > MAX_VOLUME)) {
		ALOGE("%s: volume para is invalid\n", __func__);
		return -1;
	}

	/*set para to voice module*/
	ret = 0;/*TODO*/
	return ret;
}



