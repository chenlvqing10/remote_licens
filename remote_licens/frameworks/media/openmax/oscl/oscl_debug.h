/*
 * oscl_debug.h - debug api used by lombo media framework.
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

#ifndef __OSCL_DEBUG_H__
#define __OSCL_DEBUG_H__

#if defined(__EOS__)
#include "oscl_debug_eos.h"
#else
#include "oscl_debug_linux.h"
#endif

#define MAX_ARGS_LEN (64)
#define MAX_ARGS_NUM (16)

#define dump_buffer(data, size) \
do { \
	int s_16 = (size+15) >> 4; \
	char *p = data; \
	int i; \
	OSCL_LOGI("buffer=%p, dump %d bytes:", data, size); \
	for (i = 0; i < s_16; i++, p += 16) { \
		OSCL_LOGI("%02x %02x %02x %02x %02x %02x %02x %02x " \
			  "%02x %02x %02x %02x %02x %02x %02x %02x", \
			p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], \
			p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]); \
	} \
} while (0)

#endif /* __OSCL_DEBUG_H__ */
