/*
 * system_volume.h - Interface to modify the volume
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

#ifndef _SYSTEM_VOLUME_H
#define _SYSTEM_VOLUME_H


/**
 * Get volume value from db and audio module
 * @volume IN, a pointer of saving volume value
 * Returns 0 successfully, other , failed.
 */
int system_hw_volume_get_value(int *volume);

/**
 * system_hw_volume_set_value Set system volume
 * @volume IN, the current volume value
 * Returns 0 successful, -1 set volume failed.
 */
int system_hw_volume_set_value(int volume);

/**
 * system_volume_init Initialize the hardware volume of the device.
 * @None
 * Returns 0 successfully, other , failed.
 */
int system_volume_init(void);

/**
 * system_prompt_volume_init Initialize the prompt volume of the device.
 * @None
 * Returns 0 successfully, other , failed.
 */
int system_prompt_volume_init(void);

/**
 * system_volume_set_value - Set system prompt volume.
 * @volume: IN, the current volume value.
 *
 * Returns 0 if success, otherwise if failure.
 */
int system_prompt_volume_set_value(int volume);

/**
 * system_prompt_volume_get_value - Get system prompt volume.
 * @volume: IN, the current volume value.
 *
 * Returns 0 if success, otherwise if failure.
 */
int system_prompt_volume_get_value();

/**
 * system_ad_volume_init Initialize the ad volume of the device.
 * @None
 * Returns 0 successfully, other , failed.
 */
int system_ad_volume_init(void);

/**
 * system_ad_volume_get_value - Get ad volume.
 * @volume: IN, the current volume value.
 *
 * Returns 0 if success, otherwise if failure.
 */
int system_ad_volume_get_value(int *volume);

/**
 * system_ad_volume_set_value - Set ad volume.
 * @volume: IN, the current volume value.
 *
 * Returns 0 if success, otherwise if failure.
 */
int system_ad_volume_set_value(int volume);

#endif /* _SYSTEM_ADMIN_H */
