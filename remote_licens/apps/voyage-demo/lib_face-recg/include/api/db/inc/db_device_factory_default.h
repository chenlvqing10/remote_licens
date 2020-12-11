/*
 * device_factory_default.h - Interface for the restore factory default function
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

#ifndef __DEVICE_FACTORY_DEFAULT_H
#define __DEVICE_FACTORY_DEFAULT_H

/**
 * db_restore_factory_default - Restore all settings to factory defaults.
 * None.
 *
 * Returns SQLITE_OK if success, otherwise not SQLITE_OK.
 */
int db_restore_factory_default(void);

#endif /* __DEVICE_FACTORY_DEFAULT_H */

