/*
 * dmx_iostream.h - code for iostream.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __IOSTREAM_H__
#define __IOSTREAM_H__

#include <iostream_plugin.h>

void *dmx_iostream_create(void *param, iostream_type_e type);
int dmx_iostream_destroy(void *handle);
int dmx_iostream_filldata(void *handle, uint8_t *data, size_t size);
void *dmx_iostream_get_context(void *handle);

#endif /* __IOSTREAM_H__ */


