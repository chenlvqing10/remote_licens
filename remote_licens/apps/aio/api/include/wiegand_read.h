/*
 * wiegand_read.h - export some function of operate wiegand
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

#ifndef _WIEGAND_READ_H
#define _WIEGAND_READ_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * wg_read_init - Open device file, need to invoke wg_read_deinit to close
 *
 * Returns 0 if success, otherwise -1.
 */
int wg_read_init(void);

/**
 * wg_write - Write data to device
 * @buf: OUT, wiegand data
 *
 * Returns wiegand data size if success, otherwise -1.
 */
int wg_read_block(char *buf);

/**
 * wg_read_deinit - Close device file and exit wiegand
 *
 * Returns 0 if success, otherwise -1.
 */
int wg_read_deinit(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _WIEGAND_READ_H */

