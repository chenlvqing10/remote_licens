/*
 * system_chip_id.h - Get the chip id of the device
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

#ifndef _SYSTEM_CHIP_ID_H
#define _SYSTEM_CHIP_ID_H

/* Length of device ID */
#define CHIP_ID_LEN	8

/**
 * system_get_chip_id - Get the chip id of the device.
 * @p_chip_id: Out, Return device chip id information.
 * @len_max: In, Maximum length to save chip ID.
 * Returns 0 if success, otherwise not 0.
 */
int system_get_chip_id(char *p_chip_id, int len_max);

#endif /* _SYSTEM_CHIP_ID_H */

