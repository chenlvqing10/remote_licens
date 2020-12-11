/*
 * lombo_spi.h - Generic definitions for LomboTech SPI Controller
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

#ifndef __LOMBO_SPI_H__
#define __LOMBO_SPI_H__

#include <mach/debug.h>

 /* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME "SPI"

#if defined(CONFIG_ARCH_LOMBO_N7)
#include "csp/n7/spi_operate.h"
#elif defined(CONFIG_ARCH_LOMBO_N9)
#include "csp/n9/spi_operate.h"
#endif

#define LOMBO_SPI_DEV_NAME              "lombo_spi"
#define LOMBO_SPI_MAX_LEN_PER_TRANSFER  (128 * 1024)
#define LOMBO_SPI_FIFO_MAX_CAP          (64)

#endif /* __LOMBO_SPI_H__ */

