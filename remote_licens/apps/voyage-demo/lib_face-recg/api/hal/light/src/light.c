/*
 * light.c - define some function of operate fill light
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
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>

#define LOG_TAG "hal-light"
#include <log/log.h>
#include "sqlite3.h"
#include "gpio.h"
#include "db_device_hw_param.h"
#include "db_config.h"
#include "config.h"
#include "light.h"
#include "face_svc_isp.h"
#include "microwave.h"
#include "face_svc_media.h"

#define ISP_DEV		"/dev/v4l-subdev3"

struct auto_fill_light {
	int enable;
	int isp_fd;
	int light_cur_state;
	int high_brightness;
	int low_brightness;
	int det_interval;
	int delay_time;
	int delay_count;
	int gpio;
	int running;
	int event_state;
	pthread_t auto_thread;
};

struct auto_fill_light auto_light;

void event_state_cb(int state)
{
	auto_light.event_state = state;
}

static int get_brightness(int fd)
{
	struct viss_isp_exif exif;
	int ret;

	/* Process EXIF */
	memset(&exif, 0, sizeof(exif));
	ret = ioctl(fd, VIDIOC_VISS_ISP_EXIF_REQ, &exif);
	if (ret < 0) {
		ALOGE("%s ioctl failed, return %d, errno: %d (%s)\n", __func__,
			ret, errno, strerror(errno));
		return -1;
	}

	return exif.brightness;
}

static int light_microwave_on(struct auto_fill_light *light, int brightness)
{
	int ret = 0;

#if 0
	if (light->event_state == OBJECT_MOVE &&
		light->infrared_cur_state != 1) {
		led_on();
		light->infrared_cur_state = 1;
		light->infrared_delay_count = 0;
	}

	if (light->event_state == OBJECT_STOP &&
		light->infrared_cur_state != 0) {
		if (++light->infrared_delay_count >= light->delay_time) {
			led_off();
			light->infrared_cur_state = 0;
		}
	} else
		light->infrared_delay_count = 0;
#endif

	if (brightness < light->low_brightness && light->light_cur_state != 1 &&
		light->event_state == OBJECT_MOVE) {
		/* turn on the fill light */
		ALOGD("open fill light");
		ret = gpio_set_output_level(light->gpio, 1);
		if (ret) {
			ALOGE("gpio set failed");
			return ret;
		}
		light->light_cur_state = 1;
		light->delay_count = 0;
	}

	if ((brightness > light->high_brightness ||
		light->event_state == OBJECT_STOP) &&
		light->light_cur_state != 0) {
		if (++light->delay_count >= light->delay_time) {
			/* turn off the fill light */
			ALOGD("close fill light");
			ret = gpio_set_output_level(light->gpio, 0);
			if (ret) {
				ALOGE("gpio set failed");
				return ret;
			}
			light->light_cur_state = 0;
		}
	} else
		light->delay_count = 0;

	return ret;
}

static int light_microwave_off(struct auto_fill_light *light, int brightness)
{
	int ret = 0;

#if 0
	if (light->infrared_cur_state != 1) {
		ALOGI("%s turn on ir light", __func__);
		led_on();
		light->infrared_cur_state = 1;
	}
#endif

	if (brightness < light->low_brightness && light->light_cur_state != 1) {
		/* turn on the fill light */
		ALOGD("open fill light");
		ret = gpio_set_output_level(light->gpio, 1);
		if (ret) {
			ALOGE("gpio set failed");
			return ret;
		}
		light->light_cur_state = 1;
		light->delay_count = 0;
	}

	if (brightness > light->high_brightness &&
			light->light_cur_state != 0) {
		if (++light->delay_count >= light->delay_time) {
			/* turn off the fill light */
			ALOGD("close fill light");
			ret = gpio_set_output_level(light->gpio, 0);
			if (ret) {
				ALOGE("gpio set failed");
				return ret;
			}
			light->light_cur_state = 0;
		}
	} else
		light->delay_count = 0;

	return ret;
}

static void *auto_light_thread(void *args)
{
	struct auto_fill_light *light = (struct auto_fill_light *)args;
	int brightness, microwave_sw;

	microwave_sw = db_config_get_int("device_microwave_param",
		"microwave_sw", 0);

	while (light->running) {
		brightness = get_brightness(light->isp_fd);
		ALOGD("brightness:%d", brightness);
		if (brightness < 0) {
			ALOGE("get brightness failed");
			usleep(light->det_interval * 1000000);
			continue;
		}

		if (microwave_sw)
			light_microwave_on(light, brightness);
		else
			light_microwave_off(light, brightness);

		usleep(light->det_interval * 1000000);
	}

	return NULL;
}

int light_enable_auto(int gpio, int light_state)
{
	int ret;

	ALOGD("auto light enable");

	if (auto_light.enable) {
		ALOGE("auto light is already enable");
		return -1;
	}

	event_state_set_cb(event_state_cb);
	auto_light.isp_fd = open(ISP_DEV, 0, 0);
	if (auto_light.isp_fd < 0) {
		ALOGE("%s: Failed to open subdev device node %s\n", __func__, ISP_DEV);
		return -1;
	}

	auto_light.high_brightness = config_get_int("system", "high_brightness", 800);
	auto_light.low_brightness = config_get_int("system", "low_brightness", 600);
	auto_light.det_interval = config_get_int("system", "det_interval", 1);
	auto_light.delay_time = config_get_int("system", "delay_time", 15);
	auto_light.gpio = gpio;
	auto_light.light_cur_state = light_state;
	ALOGD("high_brightness:%d, low_brightness:%d, det_interval:%d, gpio:%d",
			auto_light.high_brightness, auto_light.low_brightness,
			auto_light.det_interval, auto_light.gpio);

	auto_light.delay_count = 0;
	auto_light.running = 1;
	auto_light.event_state = OBJECT_STOP;
	ret = pthread_create(&auto_light.auto_thread, NULL, auto_light_thread,
					&auto_light);
	if (ret) {
		ALOGE("create auto light thread failed");
		return -1;
	}

	auto_light.enable = 1;

	return 0;
}

void light_disable_auto(void)
{
	ALOGD("auto light disable");

	if (!auto_light.enable) {
		ALOGE("auto light is disable");
		return;
	}

	auto_light.running = 0;
	pthread_join(auto_light.auto_thread, NULL);

	close(auto_light.isp_fd);

	memset(&auto_light, 0, sizeof(auto_light));
}

void system_filling_light_reset_count(void)
{
	if (auto_light.enable)
		auto_light.delay_count = 0;
}

/**
 * Get filling light para
 * @light_type: a pointer of saving filling light parameter
 *
 * Returns 0 if success, otherwise -1.
 */
int system_filling_light_get_para(int *light_type)
{
	int ret = -1;
	int cout = 0;
	device_hw_param_t info;

	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		ret = 0;
		*light_type = info.infrared_light_set;
	}

	return ret;
}

/**
 * Set fill light para
 * @light_type: IN, 0-colse, 1-open, 2-auto
 *
 * Returns 0 if success, otherwise -1.
 */
int system_filling_light_set_para(int light_type)
{
	int ret = -2;
	int cout = 0;
	int gpio_no = config_get_int("system", "white_light_gpio", 128);
	int sql_ret = SQLITE_OK;
	device_hw_param_t info;

	ALOGD("%s: gpio num is %d, light_type is:%d", __func__, gpio_no, light_type);
	memset(&info, 0, sizeof(device_hw_param_t));
	cout = db_device_hw_param_query(&info);
	if (cout > 0) {
		/*operate light*/
		if (light_type == FILL_LIGHT_CLOSE) {
			ret = gpio_set_output_level(gpio_no, 0);
			if (ret) {
				ALOGD("%s: close filling light failed", __func__);
				return -1;
			}
			info.infrared_light_set &= ~0x01;
		} else if (light_type == FILL_LIGHT_OPEN) {
			ret = gpio_set_output_level(gpio_no, 1);
			if (ret) {
				ALOGD("%s: open filling light failed", __func__);
				return -1;
			}
			info.infrared_light_set |= 0x01;
		} else if (light_type == FILL_LIGHT_AUTO_EN) {
			ret = light_enable_auto(gpio_no, info.infrared_light_set & 0x01);
			if (ret) {
				ALOGD("%s: FILL_LIGHT_AUTO_EN failed", __func__);
				return -1;
			}
			info.infrared_light_set |= 0x02;
		} else if (light_type == FILL_LIGHT_AUTO_DIS) {
			light_disable_auto();
			info.infrared_light_set &= ~0x02;
			/*disable auto fill light, rescovery before status*/
			if ((info.infrared_light_set & FILL_LIGHT_OPEN) ==
				FILL_LIGHT_OPEN) {
				ret = gpio_set_output_level(gpio_no, 1);
				if (ret) {
					ALOGD("%s: open filling light failed", __func__);
					return -1;
				}
			} else if (!info.infrared_light_set) {
				ret = gpio_set_output_level(gpio_no, 0);
				if (ret) {
					ALOGD("%s: close filling light failed", __func__);
					return -1;
				}

			}
		}
		/*svae parameters*/
		ALOGD("%s: %d", __func__, info.infrared_light_set);
		sql_ret = db_device_hw_param_update(&info);
		if (SQLITE_OK != sql_ret) {
			ALOGE("db_device_hw_param_update failed");
			/*if save failed ,and review operation before*/
			if (light_type == FILL_LIGHT_CLOSE)
				ret = gpio_set_output_level(gpio_no, 1);
			else if (light_type == FILL_LIGHT_OPEN)
				ret = gpio_set_output_level(gpio_no, 0);
			else if (light_type == FILL_LIGHT_AUTO_EN)
				light_disable_auto();
			else if (light_type == FILL_LIGHT_AUTO_DIS)
				light_enable_auto(gpio_no, light_type & 0x01);
			return -1;
		}
	} else {
		ALOGE("%s: read filling light para failed from db", __func__);
		return -1;
	}

	return 0;
}

/**
 * Init fill light
 * None
 *
 * Returns 0 if success, otherwise -1.
 */
int system_filling_light_init(void)
{
	int light_type = FILL_LIGHT_CLOSE;
	int gpio_no;

	gpio_no = config_get_int("system", "white_light_gpio", 128);
	system_filling_light_get_para(&light_type);
	ALOGD("%s: fill light_type: %d", __func__, light_type);

	/* light is off by default  after system boots */
	if ((light_type & 0x02) == 0x02) {
		gpio_set_output_level(gpio_no, 0);
		light_enable_auto(gpio_no, 0);
	}
	else if (light_type == FILL_LIGHT_OPEN)
		gpio_set_output_level(gpio_no, 1);
	else if (light_type == FILL_LIGHT_CLOSE)
		gpio_set_output_level(gpio_no, 0);

	return 0;
}

