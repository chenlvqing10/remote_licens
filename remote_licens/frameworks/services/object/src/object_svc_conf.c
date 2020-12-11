/*
 * object_svc_conf.c
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

#define LOG_TAG "object_svc"
#include <log/log.h>

#include <iniparser.h>

#include "object_svc_main.h"

#include "object_svc_conf.h"

void object_svc_load_conf(object_svc_t *svc)
{
	dictionary *conf;

	int debug_log = DEFAULT_DEBUG_LOG;
	int dump_yuv = DEFAULT_DUMP_YUV;
	int dump_fps = DEFAULT_DUMP_FPS;

	int rgb_buffer_width = DEFAULT_RGB_BUFFER_WIDTH;
	int rgb_buffer_height = DEFAULT_RGB_BUFFER_HEIGHT;
	int recg_x = DEFAULT_RECG_X;
	int recg_y = DEFAULT_RECG_Y;
	int recg_width = DEFAULT_RECG_WIDTH;
	int recg_height = DEFAULT_RECG_HEIGHT;
	int rgb_x_margin = DEFAULT_RGB_X_MARGIN;
	int rgb_y_margin = DEFAULT_RGB_Y_MARGIN;
	int det_step = DEFAULT_DET_STEP;
	int object_language = 0;

	if (!svc) {
		ALOGE("%s invalid paramter", __func__);
		return;
	}

	conf = iniparser_load(CONF_PATH);
	if (conf) {
		debug_log = iniparser_getint(conf, "log:debug", DEFAULT_DEBUG_LOG);
		dump_yuv = iniparser_getint(conf, "log:dump_yuv", DEFAULT_DUMP_YUV);
		dump_fps = iniparser_getint(conf, "log:dump_fps", DEFAULT_DUMP_FPS);

		rgb_buffer_width = iniparser_getint(conf, "svc:rgb_buffer_width", DEFAULT_RGB_BUFFER_WIDTH);
		rgb_buffer_height = iniparser_getint(conf, "svc:rgb_buffer_height", DEFAULT_RGB_BUFFER_HEIGHT);
		recg_x = iniparser_getint(conf, "svc:recg_x", DEFAULT_RECG_X);
		recg_y = iniparser_getint(conf, "svc:recg_y", DEFAULT_RECG_Y);
		recg_width = iniparser_getint(conf, "svc:recg_width", DEFAULT_RECG_WIDTH);
		recg_height = iniparser_getint(conf, "svc:recg_height", DEFAULT_RECG_HEIGHT);
		rgb_x_margin = iniparser_getint(conf, "svc:rgb_x_margin", DEFAULT_RGB_X_MARGIN);
		rgb_y_margin = iniparser_getint(conf, "svc:rgb_y_margin", DEFAULT_RGB_Y_MARGIN);
		det_step = iniparser_getint(conf, "svc:det_step", DEFAULT_DET_STEP);
		object_language = iniparser_getint(conf, "svc:object_language", 0);

		iniparser_freedict(conf);
	} else {
		ALOGE("%s load %s failed, used default", __func__, CONF_PATH);
	}

	ALOGI("object service configuration");
	ALOGI("  debug_log: %d", debug_log);
	ALOGI("  dump_yuv: %d", dump_yuv);
	ALOGI("  dump_fps: %d", dump_fps);

	ALOGI("  rgb_buffer_width: %d", rgb_buffer_width);
	ALOGI("  rgb_buffer_height: %d", rgb_buffer_height);
	ALOGI("  recg_x: %d", recg_x);
	ALOGI("  recg_y: %d", recg_y);
	ALOGI("  recg_width: %d", recg_width);
	ALOGI("  recg_height: %d", recg_height);
	ALOGI("  rgb_x_margin: %d", rgb_x_margin);
	ALOGI("  rgb_y_margin: %d", rgb_y_margin);
	ALOGI("  det_step: %d", det_step);

	svc->debug_log = debug_log;
	svc->dump_yuv = dump_yuv;
	svc->dump_fps = dump_fps;

	svc->rgb_buffer_width = rgb_buffer_width;
	svc->rgb_buffer_height = rgb_buffer_height;
	svc->recg_x = recg_x;
	svc->recg_y = recg_y;
	svc->recg_width = recg_width;
	svc->recg_height = recg_height;
	svc->rgb_x_margin = rgb_x_margin;
	svc->rgb_y_margin = rgb_y_margin;
	svc->det_step = det_step;
	svc->object_language = object_language;
}
