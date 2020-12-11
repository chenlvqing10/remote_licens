/*
 * oscl_queue.c - queue api used by lombo media framework.
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
#include <oscl.h>
#include <stdio.h>
#include <dlfcn.h>

#ifndef __EOS__
#define _MALLOC_LIB_NAME "liblombo_malloc.so"
static pthread_mutex_t _malloc_lock = PTHREAD_MUTEX_INITIALIZER;
static void *(*_malloc)(int size, int flag, unsigned long *paddr, char *file, int line);
static void (*_free)(void *ptr, int flag);
static unsigned long (*_vir_to_phy)(void *vaddr);
static int (*_cache_flush_vir)(void *vaddr);
static int (*_cache_flush)(unsigned long);
static unsigned int (*_vir_to_fd)(void *vaddr);

static void *_lib_handle;
void _malloc_init(void)
{
	pthread_mutex_lock(&_malloc_lock);
	if (_lib_handle != NULL)
		goto EXIT;
	_lib_handle = dlopen(_MALLOC_LIB_NAME, RTLD_LAZY);
	if (_lib_handle == NULL) {
		OSCL_LOGE("Failed to open lib:%s: %s", _MALLOC_LIB_NAME, dlerror());
		goto EXIT;
	}
	_malloc = dlsym(_lib_handle, "lombo_malloc");
	_free = dlsym(_lib_handle, "lombo_free");
	_vir_to_phy = dlsym(_lib_handle, "lombo_vir_to_phy");
	_cache_flush_vir = dlsym(_lib_handle, "lombo_cache_flush_vir");
	_cache_flush = dlsym(_lib_handle, "lombo_cache_flush");
	_vir_to_fd = dlsym(_lib_handle, "lombo_vir_to_fd");
	if (_malloc == NULL || _free == NULL || _vir_to_phy == NULL) {
		dlclose(_lib_handle);
		OSCL_LOGE("%s dlsym fail:%s", _MALLOC_LIB_NAME, dlerror());
		_lib_handle = NULL;
		_malloc = NULL;
		_free = NULL;
		_vir_to_phy = NULL;
	}
EXIT:
	pthread_mutex_unlock(&_malloc_lock);
}

unsigned long oscl_virt_to_phys(void *p)
{
	unsigned long ret = 0;
	if (_vir_to_phy)
		ret = _vir_to_phy(p);
	return ret;
}

int oscl_cache_flush_vir(void *p)
{
	int ret = -1;
	if (_cache_flush_vir)
		ret = _cache_flush_vir(p);
	return ret;
}

int oscl_cache_flush(unsigned long p)
{
	int ret = -1;
	if (_cache_flush)
		ret = _cache_flush(p);
	return ret;
}

unsigned int oscl_vir_to_fd(void *p)
{
	unsigned int ret = 0;
	if (_vir_to_fd)
		ret = _vir_to_fd(p);
	return ret;
}

void *oscl_malloc_unca(int size)
{
	void *p = NULL;
	if (!_malloc)
		_malloc_init();
	if (_malloc)
		p = _malloc(size, MEM_UC, NULL, __FILE__, __LINE__);
	return p;
}

void oscl_free_unca(void *p)
{
	if (_free)
		_free(p, MEM_UC);
}

void *oscl_malloc_cached(int size)
{
	void *p = NULL;
	if (!_malloc)
		_malloc_init();
	if (_malloc)
		p = _malloc(size, MEM_WT, NULL, __FILE__, __LINE__);
	return p;
}

void oscl_free_cached(void *p)
{
	if (_free)
		_free(p, MEM_WT);
}

#endif

