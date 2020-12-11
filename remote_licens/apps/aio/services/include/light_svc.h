/*
 * light_svc.h - export some function of operate fill light
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

#ifndef _LIGHT_SVC_H
#define _LIGHT_SVC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

enum {
	FILL_LIGHT_CLOSE, /*close fill light*/
	FILL_LIGHT_OPEN, /*open fill light*/
	FILL_LIGHT_AUTO_EN, /*enable auto fill light*/
	FILL_LIGHT_AUTO_DIS, /*disable auto fill light*/
};

/* fill light parameter */
typedef struct tag_fill_light_param {
	/* Infrared light control:
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 *    2: Automatic
	 */
	char infrared_light_set;

	/* Light sensitive control:
	 * Options:
	 *    0: Disable
	 *    1: Enable
	 */
	char light_sensitive;
	int light_high_brightness;
	int light_low_brightness;
	char light_det_interval;
	char light_delay_time;
	int gpio;

} fill_light_param_t;

/**
 * reset filling light count
 *
 * Returns None
 */
void filling_light_reset_count(void);

/**
 * Init fill light
 * @p_info: A pointer of fill_light_param_t
 *
 * Returns 0 if success, otherwise -1.
 */
int filling_light_init(fill_light_param_t *p_info);

/**
 * deinit fill light
 * @p_info: A pointer of fill_light_param_t
 *
 * Returns 0 if success, otherwise -1.
 */
void filling_light_deinit(void);

/**
 * light_disable_auto: disable auto filling light
 *
 * Returns None
 */
void light_disable_auto(void);

/**
 * light_enable_auto: enable auto filling light
 * @gpio: gpio id (deprecated)
 * @light_state: current light status(0/1:OFF/ON)
 *
 * Returns None
 */
void light_enable_auto1(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _LIGHT_SVC_H */

