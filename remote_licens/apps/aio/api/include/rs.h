/*
 * rs.h - Interface to operate serial port
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

#ifndef __SERICAL_PORT_H__
#define __SERICAL_PORT_H__

#define RS_OPEN_READ_ONLY 1
#define RS_OPEN_WRITE_ONLY 2
#define RS_OPEN_RDWR 3

/**
 * cb_trans_data_notify - callback function of serial port receive data
 * @trans_data: data of serial port received
 * @trans_len: length of data
 * @context: object of context
 *
 * Returns None.
 */
typedef void (*cb_trans_data_notify)(
			const char *trans_data, int trans_len, void *context);

/**
 * rs_open - open serial port
 * @dev_name: device name
 * @open_mode: open mode(1/2/3:read only/write only/read and write)
 * @speed: boud rate
 * @bits: data bits
 * @even: even/odd bit
 * @stop: stop bit
 * @_trans_data_notify: a pointer of callback
 * @context: object of context
 *
 * Returns object of device.
 */
void *rs_open(const char *dev_name, int open_mode,
				int speed, int bits, char even, int stop,
				cb_trans_data_notify _trans_data_notify,
				void *context);

/**
 * rs_close - close serial port
 * @handle: object of device
 *
 * Returns 0 if success, otherwise -1.
 */
int rs_close(void *handle);

/**
 * rs_send_data: send data to serial port
 * @handle: object of device
 * @data: data of send
 * &data_len: length of data
 *
 * Returns 0 if success, otherwise -1.
 */
int rs_send_data(void *handle, const char *data, int data_len);

#endif /* __SERICAL_PORT_H__ */

