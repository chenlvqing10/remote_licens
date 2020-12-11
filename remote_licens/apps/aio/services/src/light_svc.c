/*
 * light_svc.c - define some function of operate fill light
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
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <assert.h>

#define LOG_TAG "light"
#include <log/log.h>
#include "gpio.h"
#include "led.h"
#include "light_svc.h"
#include "face_svc.h"
#include "isp.h"
#include "sensor.h"

/* auto filling thread parameter */
struct auto_fill_light {
	int nir_enable;
	int is_det_people;
	int enable;
	int light_cur_state;
	int human_induction_dis;
	int high_brightness;
	int low_brightness;
	int det_interval;
	int delay_time;
	int delay_count;
	int gpio;
	int infrared_light_set;
	int running;
	pthread_t auto_thread;
};
struct auto_fill_light auto_light;

/**
 * det_cb: callback function of nir detect
 * @face_info: a pointer of face info
 * @face_rects: a pointer of face rects
 *
 * Returns NULL.
 */
static void *det_cb(face_frame_t *face_info, nir_det_rects_t *face_rects)
{
	if (NULL != face_rects && face_rects->size > 0) {
		auto_light.is_det_people = 1;
		ALOGD("nir det people\n");
	}
	return NULL;
}

/**
 * nir_det_init: init nir detect serivce
 *
 * Returns 0 if success, otherwise -1.
 */
static int nir_det_init(void)
{
	int ret = 0;

	ret = face_register_nir_det_cb(det_cb);
	if (ret < 0) {
		ALOGE("face_register_nir_det_cb failed\n");
		return ret;
	}

	ret = face_nir_det_start();
	if (ret < 0) {
		ALOGE("face_nir_det_start failed\n");
		return ret;
	}

	ALOGD("nir det start\n");
	return ret;
}

/**
 * nir_det_deinit: deinit nir detect serivce
 *
 * Returns None
 */
static int nir_det_deinit(void)
{
	ALOGD("nir det stop\n");
	face_register_nir_det_cb(NULL);
	return face_nir_det_stop();
}

/**
 * auto_light_thread: detect thread of auto filling light
 * @args: a pointer of detect thread
 *
 * Returns NULL
 */
static void *auto_light_thread(void *args)
{
	struct auto_fill_light *light = (struct auto_fill_light *)args;
	int illuminance = 0, ret;
	prctl(PR_SET_NAME, "auto_light");

	light->running = 1;
	while (light->running) {
		if (0 == light->enable) {
			usleep(500 * 1000);
			if(light->light_cur_state) {
				light->light_cur_state = 0;
				white_led_ctrl(LED_OFF);
			}
			continue;
		}
		illuminance = isp_get_brightness(FACE_CAMERA_RGB);
		if (illuminance < 0) {
			ALOGE("get illuminance failed");
			usleep(light->det_interval * 1000000);
			continue;
		}

		/*open nir det*/
		if (illuminance < light->low_brightness) {
			ALOGD("low illuminance = %d\n", illuminance);
			if (light->nir_enable == 0) {
				ALOGD("start nir det\n");
				ret = nir_det_init();
				if (ret == 0)
					light->nir_enable = 1;
			}
		}

		/*close nir det when brightness is high*/
		if (light->nir_enable &&
			illuminance >= light->high_brightness) {
			ALOGD("hight illuminance, stop nir det\n");
			nir_det_deinit();
			light->nir_enable = 0;
		}

		/*close nir det when people deted*/
		if (light->is_det_people && light->nir_enable) {
			ALOGD("face detected, stop nir det\n");
			nir_det_deinit();
			light->nir_enable = 0;
		}

		/*openlight when people deted and light is closed*/
		if (light->light_cur_state == 0 && light->is_det_people) {
			ALOGD("face detected, turn on light\n");
			ret = white_led_ctrl(LED_ON);
			if (ret) {
				ALOGE("turn on light failed");
				usleep(light->det_interval * 1000000);
				continue;
			}
			light->light_cur_state = 1;
			light->is_det_people = 0;
			light->delay_count = 0;
		}

		/*counter increat when light opened*/
		if (light->light_cur_state) {
			ALOGD("light on, count:%d\n", light->delay_count);
			if (++light->delay_count >= light->delay_time) {
				/* turn off the fill light */
				ALOGD("turn off light");
				ret = white_led_ctrl(LED_OFF);
				if (ret) {
					ALOGE("turn off light failed");
					usleep(light->det_interval * 1000000);
					continue;
				}
				light->light_cur_state = 0;
				light->is_det_people = 0;
			}
		}
		usleep(light->det_interval * 1000000);
	}
	if(light->light_cur_state) {
		light->light_cur_state = 0;
		white_led_ctrl(LED_OFF);
	}

	return NULL;
}

/**
 * light_enable_auto: enable auto filling light
 * @gpio: gpio id (deprecated)
 * @light_state: current light status(0/1:OFF/ON)
 *
 * Returns None
 */
int light_enable_auto(int gpio, int light_state)
{
	ALOGD("auto light enable");

	if (auto_light.enable) {
		ALOGE("auto light is already enable");
		return -1;
	}

	auto_light.gpio = gpio;
	auto_light.light_cur_state = light_state;

	ALOGD("hinduction_dis %d, hbright:%d,l_bright:%d, det_intval:%d, gpio:%d",
			auto_light.human_induction_dis,
			auto_light.high_brightness, auto_light.low_brightness,
			auto_light.det_interval, auto_light.gpio);

	auto_light.delay_count = 0;
	auto_light.enable = 1;

	return 0;
}

void light_enable_auto1(void)
{
	light_enable_auto(auto_light.gpio, 0);
}

/**
 * light_disable_auto: disable auto filling light
 *
 * Returns None
 */
void light_disable_auto(void)
{
	ALOGD("auto light disable");
	if (!auto_light.enable) {
		ALOGE("auto light is disable");
		return;
	}
	if (auto_light.nir_enable)
		nir_det_deinit();
	auto_light.nir_enable = 0;
	auto_light.enable = 0;
	/* pthread_join(auto_light.auto_thread, NULL);
	memset(&auto_light, 0, sizeof(auto_light)); */
}

/**
 * reset filling light count
 *
 * Returns None
 */
void filling_light_reset_count(void)
{
	if (auto_light.enable)
		auto_light.delay_count = 0;
}

/**
 * Init fill light
 * @p_info: A pointer of fill_light_param_t
 *
 * Returns 0 if success, otherwise -1.
 */
int filling_light_init(fill_light_param_t *p_info)
{
	int ret = 1;
	if (NULL == p_info)
		return -1;

	ALOGD("%s: fill gpio %d light_type: %d",
			__func__,  p_info->gpio, p_info->infrared_light_set);

	memset(&auto_light, 0, sizeof(struct auto_fill_light));

	auto_light.human_induction_dis = 50;
	auto_light.high_brightness = p_info->light_high_brightness;
	auto_light.low_brightness = p_info->light_low_brightness;
	auto_light.det_interval = p_info->light_det_interval;
	auto_light.delay_time = p_info->light_delay_time;

	auto_light.gpio = p_info->gpio;
	auto_light.infrared_light_set = p_info->infrared_light_set;

	ret = pthread_create(&auto_light.auto_thread, NULL, auto_light_thread,
			&auto_light);
	if (ret) {
		ALOGE("create auto light thread failed");
		return -1;
	}

	if ((auto_light.infrared_light_set & 0x02) == 0x02)
		light_enable_auto(auto_light.gpio,
				auto_light.infrared_light_set & 0x01);
	else if (auto_light.infrared_light_set == FILL_LIGHT_OPEN)
		white_led_ctrl(LED_ON);
	else if (auto_light.infrared_light_set == FILL_LIGHT_CLOSE)
		white_led_ctrl(LED_OFF);

	return 0;
}

/**
 * deinit fill light
 * @p_info: A pointer of fill_light_param_t
 *
 * Returns 0 if success, otherwise -1.
 */
void filling_light_deinit(void)
{
	if (auto_light.running) {
		auto_light.running = 0;
		pthread_join(auto_light.auto_thread, NULL);
	}

	if (auto_light.nir_enable)
		nir_det_deinit();
	memset(&auto_light, 0, sizeof(auto_light));
}

