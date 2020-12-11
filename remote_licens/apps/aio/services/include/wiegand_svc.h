/*
 * wiegand_svc.h - Interface to wiegand
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

#ifndef _WIEGAND_SVC_H
#define _WIEGAND_SVC_H

/**
 * open_door_start_service - start service of open door check
 * if not open door.
 * @cardid: IN, cardid
 *
 * Returns 0 if success, otherwise not 0.
 */
int wiegand_write_svc(uint64_t cardid);

/**
 * wiegand_write_set_data_size - set wiegand protocol
 * @data_size: wiegand data size contian even bit and odd bit
 *
 * Returns 0 if success, otherwise not 0.
 */
int wiegand_write_set_data_size(int data_size);

#endif /* _WIEGAND_SVC_H */

