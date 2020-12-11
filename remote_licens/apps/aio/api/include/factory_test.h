/*
 * factory_test.h - For testing aio face interface.
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
#ifndef _AIO_API_FACTORY_TEST_H
#define _AIO_API_FACTORY_TEST_H

/**
 * factory_test_light - test white light in factory test mode.
 * @freq_time: test time of once
 * @count: test count
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_light(int freq_time, int count);

/**
 * factory_test_light_infrared - test infrared light in factory test mode.
 * @freq_time: test time of once
 * @count: test count
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_light_infrared(int freq_time, int count);

/**
 * factory_test_speaker - test speaker in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_speaker(void);

/**
 * factory_test_wifi - test wifi in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_wifi(void);

/**
 * factory_test_illuminance - test light sensor in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_illuminance(void);

/**
 * factory_test_get_illuminance_value - get value of light sensor
 * in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_get_illuminance_value(int *value);

/**
 * factory_test_rtc - test rtc in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_rtc(void);

/**
 * factory_test_camera - test camera in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_camera(void);

/**
 * factory_test_eth - test eth in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_eth(void);

/**
 * factory_test_bluetooth - test ble in factory test mode.
 *
 * Returns 0 if success, otherwise -1.
 */
int factory_test_bluetooth(void);

/**
 * @brief  usb test interface
 * @note
 * @retval
 */
int factory_test_usb(void);

#endif /* _AIO_API_FACTORY_TEST_H */

