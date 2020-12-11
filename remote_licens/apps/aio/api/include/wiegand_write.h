/*
 * wiegand_write.h - export some function of operate wiegand
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

#ifndef _WIEGAND_WRITE_H
#define _WIEGAND_WRITE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * wg_write - Write data to device
 * @data_size: IN, wiegand data size, should be 26 or 34
 * @cardid: IN, cardid
 *
 * Returns 0 if success, otherwise -1.
 */
int wg_write(int data_size, uint64_t cardid);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _WIEGAND_WRITE_H */

