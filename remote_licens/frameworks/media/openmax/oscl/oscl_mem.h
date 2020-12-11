/*
 * oscl_mem.h - memory api used by lombo media framework.
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

#ifndef __OSCL_MEM_H__
#define __OSCL_MEM_H__

#include <stdlib.h>
#include <string.h>
#include "lombo_malloc.h"

#if defined(__EOS__)

#define oscl_malloc(count)		rt_malloc(count)
#define oscl_zalloc(count)		rt_zalloc(count)
#define oscl_free(p)			rt_free(p)
#define oscl_malloc_align(count, align)	rt_malloc_align(count, align)
#define oscl_free_align(p)		rt_free_align(p)
#define oscl_malloc_unca_align(x, y)	rt_zalloc_unca_align(x, y)
#define oscl_free_unca_align(x)		rt_free_unca_align(x)
#define oscl_unca_to_phys(x)		unca_to_phys(x)
#define oscl_virt_to_phys(x)		virt_to_phys(x)

#define oscl_strdup(str)		rt_strdup(str)
#define oscl_realloc(p, size)		rt_realloc(p, size)

#else /* __EOS__ */

static inline void *zalloc(unsigned int size)
{
	void *ptr = malloc(size);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

/* alloc data structure */
#define oscl_malloc(count)		malloc(count)
#define oscl_zalloc(count)		zalloc(count)
#define oscl_free(p)			free(p)
#define oscl_strdup(str)		strdup(str)
#define oscl_realloc(p, size)		realloc(p, size)

#if 1
#define oscl_malloc_align(count, align)        oscl_malloc_cached(count)
#define oscl_free_align(p)             oscl_free_cached(p)
#else
#define oscl_malloc_align(count, align)        oscl_malloc_unca(count)
#define oscl_free_align(p)             oscl_free_unca(p)
#endif

void *oscl_malloc_unca(int size);
void oscl_free_unca(void *p);
void *oscl_malloc_cached(int size);
void oscl_free_cached(void *p);
unsigned long oscl_virt_to_phys(void *p);
int oscl_cache_flush_vir(void *p);
int oscl_cache_flush(unsigned long phy_addr);
unsigned int oscl_vir_to_fd(void *p);

#endif

#endif /* __OSCL_MEM_H__ */

