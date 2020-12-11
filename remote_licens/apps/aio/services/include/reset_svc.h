/*
 * reset_svc.h - resetting service background for monitoring reset key and
 *               provide resetting device api
 *
 *
 * Copyright (C) 2016-2020, LomboTech Co.Ltd.
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

#ifndef __RESET_SVC_H
#define __RESET_SVC_H

#define RESET_GPIO 115

typedef void (*reset_signal_cb)();

/*
* reset svc init
* @cb call back function if reset key has signal
* return 0 if success, otherwise return -1
*/
int reset_factory_svc_init(reset_signal_cb cb);

/*
* reset svc deinit
*
* return 0 if success, otherwise return -1
*/
int reset_factory_svc_deinit();

/*
* Enable the reset thread to monitor reset key if
* cb had been called and reset_device_immed() doesn't
* been called;
*/
void set_reset_flag();

/**
 * reset device immedialy.
 *
 * Returns 0 if success, otherwise -1.
 */
int reset_device_immed();

#endif /* __RESET_SVC_H */

