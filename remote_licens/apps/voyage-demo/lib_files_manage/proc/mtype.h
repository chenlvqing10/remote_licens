/*
 * mtype.h - media type code for LomboTech
 * media type interface and macro define
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

#ifndef __MTYPE_H__
#define __MTYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <files_manage.h>

typedef	struct tag_mformat {
	const char *suffix;
	media_type_t mtype;
} mformat_t;

media_type_t suffix_to_mtype(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* __MTYPE_H__ */

