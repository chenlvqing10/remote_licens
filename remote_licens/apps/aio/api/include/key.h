/*
 * key.h - Interface to get reset key status
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

#ifndef _KEY_H
#define _KEY_H

/* key event callback function */
typedef void (*key_event_cb)(int code, int type, int value);

/**
 * key_input_init - Init key input
 * @code: key value
 * @cb： key event callback
 *
 * Returns key input devce object.
 */
void *key_input_init(int code, key_event_cb cb);

/**
 * key_input_deinit - deinit key input
 * @handle: key input devce object
 *
 * Returns 0 if success, otherwise -1.
 */
int key_input_deinit(void *handle);

#endif /* _KEY_H */

