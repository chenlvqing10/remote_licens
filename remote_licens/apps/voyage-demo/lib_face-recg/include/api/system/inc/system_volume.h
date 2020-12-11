/*
 * system_volume.h - Interface to modify the volume
 * information from Excel.
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

#ifndef _SYSTEM_VOLUME_H
#define _SYSTEM_VOLUME_H


/**
 * Get volume value from db and audio module
 * @volume IN, a pointer of saving volume value
 * Returns 0 successfully, other , failed.
 */
int system_volume_get_value(int *volume);

/**
 * set system volume
 * @volume IN, the current volume value
 * Returns 0 successful, -1 operate db failed , -2 set volume failed.
 */
int system_volume_set_value(int volume);

/**
 * init system volume
 * @NULL
 * Returns NULL
 */
int system_volume_init(void);


#endif /* _SYSTEM_ADMIN_H */
