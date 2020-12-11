/*
 * light.h - export some function of operate fill light
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

#ifndef _LIGHT_H
#define _LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

enum {
	FILL_LIGHT_CLOSE,/*close fill light*/
	FILL_LIGHT_OPEN,/*open fill light*/
	FILL_LIGHT_AUTO_EN,/*enable auto fill light*/
	FILL_LIGHT_AUTO_DIS,/*disable auto fill light*/
};

/**
 * Get filling light para
 * @light_type: a pointer of saving filling light parameter
 *
 * Returns 0 if success, otherwise -1.
 */
int system_filling_light_get_para(int *light_type);

/**
 * Set fill light para
 * @light_type: IN, 0-colse, 1-open, 2-auto
 *
 * Returns 0 if success, otherwise -1.
 */
int system_filling_light_set_para(int light_type);

void system_filling_light_reset_count(void);

/**
 * Init fill light
 * None
 *
 * Returns 0 if success, otherwise -1.
 */
int system_filling_light_init(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

