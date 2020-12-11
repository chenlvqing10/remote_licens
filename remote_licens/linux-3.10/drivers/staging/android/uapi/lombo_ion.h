/*
 * lombo_ion.h - Generic definitions for LomboTech SoCs
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef __LOMBO_ION_H
#define __LOMBO_ION_H

#include "ion.h"

struct ion_handle;
int ion_handle_put(struct ion_handle *handle);

struct lombo_phys_data {
	ion_user_handle_t handle;
	unsigned long phys_addr;
	unsigned int size;
};

#define ION_IOC_LB_PHYS_ADDR		1

#endif /* __LOMBO_ION_H */

