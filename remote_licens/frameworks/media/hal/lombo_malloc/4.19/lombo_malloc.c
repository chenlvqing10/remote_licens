/*
 * lombo_malloc.c - Standard functionality for lombo malloc.
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

#define LOG_TAG		"lombo_malloc"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <log/log.h>
#include <linux/dma-buf.h>
#include "lombo_malloc.h"
#include "ion.h"
#include "lombo_util.h"

#define DEBUG_MALLOC
#define MAX_HANDLE_NUM 512
#define ION_ALLOC_ALIGN 0x1000	/* 4k align */
#define DEV_NAME		"/dev/ion"
#define UTIL_DEV		"/dev/lb_util"

typedef struct buffer_hanlde_t {
	int buffer_type;
	int handle;
	unsigned int fd;
	void *vir_addr;
	unsigned long phy_addr;
	int use_flag;
	int buf_size;
	int offset;
#ifdef DEBUG_MALLOC
	char file[128];
	int line;
#endif
} buffer_hanlde;

static int fd_ion;
static int fd_util;
static unsigned int cma_heap_id;
static int buffer_handle_num;
static buffer_hanlde handles[MAX_HANDLE_NUM];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int get_heap_id(unsigned int type)
{
	int ret, i;
	struct ion_heap_query query;
	struct ion_heap_data data[ION_HEAP_TYPE_CUSTOM];

	ret = -1;
	memset(&query, 0, sizeof(query));
	query.cnt = ION_HEAP_TYPE_CUSTOM;
	query.heaps = (__u64)&data[0];
	ret = ioctl(fd_ion, ION_IOC_HEAP_QUERY, &query);
	if (ret < 0) {
		ALOGE("query err %d\n", ret);
		return ret;
	}
	for (i = 0; i < query.cnt; i++)
		ALOGE("heap name %s, type %d, id %d\n",
			data[i].name, data[i].type, data[i].heap_id);
	for (i = 0; i < query.cnt; i++) {
		if (data[i].type == type)
			return data[i].heap_id;
	}

	return ret;
}

void *lombo_memcpy(void *dst, void *src, int size)
{
	if (size < 0)
		ALOGE("memcpy size smaller than 0, need debug\n");
	return memcpy(dst, src, size);
}

void *lombo_memset(void *prt, int val, int size)
{
	if (size <= 0 || prt == NULL) {
		ALOGE("memset err size %d, prt %p\n", size, prt);
		return NULL;
	}
	return memset(prt, val, size);
}

int get_unuse_handle_index()
{
	int i = 0;

	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if (handles[i].use_flag == 0) {
			handles[i].use_flag = 1;
			return i;
		}
	}
	ALOGE("not enough handle num , need debug\n");
	return -1;
}

int find_index_by_viraddr(void *vir_addr)
{
	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr == vir_addr))
			return i;
	}
	ALOGE("the buffer 0x%p can not be find , need debug\n", vir_addr);
	return -1;
}

unsigned int lombo_vir_to_fd(void *vir_addr)
{
	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr == vir_addr))
			return handles[i].fd;
	}
	ALOGE("the buffer 0x%p can not be find,need debug\n", vir_addr);
	return -1;
}

unsigned long lombo_fd_to_phy(unsigned int fd)
{
	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) && (handles[i].fd == fd))
			return handles[i].phy_addr;
	}
	ALOGE("the buffer fd:%d can not be find ,we can not get fd ,need debug\n", fd);
	return -1;
}

unsigned long lombo_vir_to_phy(void *vir_addr)
{
	int i = 0;

	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr == vir_addr))
			return handles[i].phy_addr;
	}
	ALOGE("can not find the buffer in the list\n");
	return 0;
}

int lombo_cache_flush(unsigned long phy_addr)
{
	int ret = 0;
	struct util_sync sync;

	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].phy_addr == phy_addr))
			break;
	}
	if (i == MAX_HANDLE_NUM) {
		ALOGE("can not find the buffer in the list\n");
		return -1;
	}
	if ((handles[i].buffer_type != MEM_WB) && (handles[i].buffer_type != MEM_WT)) {
		ALOGI("buf_addr : 0x%lx do not need sync\n", phy_addr);
		return 0;
	}
	sync.fd = handles[i].fd;
	ret = ioctl(fd_util, LB_UTIL_SYNC, &sync);
	if (ret) {
		ALOGE("sync fd error\n");
		return -1;
	}

	return 0;
}

int lombo_cache_flush_vir(void *vir_addr)
{
	int ret = 0;
	struct util_sync sync;

	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr == vir_addr))
			break;
	}
	if (i == MAX_HANDLE_NUM) {
		ALOGE("can not find the buffer in the list\n");
		return -1;
	}
	sync.fd = handles[i].fd;
	ret = ioctl(fd_util, LB_UTIL_SYNC, &sync);
	if (ret) {
		ALOGE("sync fd error\n");
		return -1;
	}

	return 0;
}

void *lombo_malloc(int size, int buffer_type,
	unsigned long *phy_addr, char *file, int line)
{
	void *ptr = NULL;
	int length = size;
	int index = -1;
	int ret = 0;
	void *addr_vir = NULL;
	struct ion_allocation_data alloc_data;
	struct util_fd2phy fd2phy;

	if (length <= 0) {
		ALOGE("malloc buf size %d smaller <= 0, file %s, line %d\n",
			size, file, line);
		return NULL;
	}

	switch (buffer_type) {
	case MEM_VIRT:
		ptr = (void *)malloc(length);
		if (ptr == NULL) {
			ALOGE("malloc buf error\n");
			return NULL;
		}
		break;
	case MEM_UC:
	case MEM_WT:
	case MEM_WB:
		/* printf("malloc buf buffer_handle_num : %d\n",buffer_handle_num); */
		pthread_mutex_lock(&mutex);
		if (buffer_handle_num == 0) {
			fd_ion = open(DEV_NAME, O_RDONLY, 0);
			if (!fd_ion) {
				ALOGE("open ion fd error\n");
				return NULL;
			}
			fd_util = open(UTIL_DEV, O_RDONLY, 0);
			if (!fd_util) {
				ALOGE("open util fd error\n");
				return NULL;
			}
			cma_heap_id = get_heap_id(ION_HEAP_TYPE_DMA);
			ALOGE("dma heap id %d\n", cma_heap_id);
		}
		buffer_handle_num++;
		index = get_unuse_handle_index();
		pthread_mutex_unlock(&mutex);

		if (index == -1) {
			ALOGE("can't find valid handle index\n");
			return NULL;
		}

		alloc_data.len = (size_t)length;
		alloc_data.heap_id_mask = 1 << cma_heap_id;
		if (buffer_type == MEM_UC)
			alloc_data.flags = 0;
		else if ((buffer_type == MEM_WT) || (buffer_type == MEM_WB))
			alloc_data.flags = ION_FLAG_CACHED;
		ret = ioctl(fd_ion, ION_IOC_ALLOC, &alloc_data);
		if (ret < 0) {
			ALOGE("alloc ion buffer error %s --- %d\n", file, line);
			return NULL;
		}

		addr_vir = mmap(NULL, alloc_data.len,
			PROT_READ | PROT_WRITE, MAP_SHARED, alloc_data.fd, 0);

		fd2phy.fd = alloc_data.fd;
		ret = ioctl(fd_util, LB_UTIL_FD2PHY, &fd2phy);
		if (ret) {
			ALOGE("fd2phy error %s --- %d\n", file, line);
			return NULL;
		}

		handles[index].fd = alloc_data.fd;
		handles[index].buffer_type = buffer_type;
		handles[index].vir_addr = (void *)addr_vir;
		handles[index].phy_addr = fd2phy.phy_addr;
		handles[index].buf_size = size;

#ifdef DEBUG_MALLOC
		handles[index].line = line;
		handles[index].file[127] = '\0';
		strncpy(handles[index].file, file, 127);
#endif
		ptr = (void *)addr_vir;
		if (phy_addr)
			*phy_addr = (fd2phy.phy_addr);
		break;

	default:
		ALOGE("malloc not support : %d\n", buffer_type);
		ptr = NULL;
		break;
	}

	return ptr;
}

void *lombo_calloc(int num, int size, char *file, int line)
{
	if ((num <= 0) || (size <= 0)) {
		ALOGE("num or size is smaller than 0, So return NULL\n");
		return NULL;
	} else {
		return calloc(num, size);
	}
}

void *lombo_realloc(void *ptr, int size, int buffer_type,
			unsigned long *phy_addr, char *file, int line)
{
	void *ptr_new = NULL;

	if (buffer_type != MEM_VIRT) {
		ALOGE("only support AL_MEM_VIRT, need debug\n");
		return NULL;
	}

	ptr_new = realloc(ptr, size);

	if (ptr_new == NULL) {
		ALOGE("realloc buffer error\n");
		return NULL;
	}
	return ptr_new;
}

void lombo_free(void *ptr, int flag)
{
	int index;
	int ret = 0;

	if (!ptr) {
		ALOGE("free arg null pointer prt\n");
		return;
	}
	switch (flag) {
	case MEM_VIRT:
		free(ptr);
		break;
	case MEM_UC:
	case MEM_WT:
	case MEM_WB:
		pthread_mutex_lock(&mutex);
		index = find_index_by_viraddr(ptr);
		if (index != -1) {
			ret = munmap(handles[index].vir_addr, handles[index].buf_size);
			if (ret) {
				ALOGE("munmap failed\n");
				return;
			}
			handles[index].buffer_type = 0;
			handles[index].vir_addr = NULL;
			handles[index].phy_addr = 0;
			close(handles[index].fd);
			handles[index].fd = 0;
			handles[index].use_flag = 0;
			handles[index].handle = 0;
		} else {
			pthread_mutex_unlock(&mutex);
			break;
		}
		buffer_handle_num--;
		ALOGI("free buffer_handle_num : %d\n", buffer_handle_num);
		if (buffer_handle_num == 0) {
			close(fd_ion);
			close(fd_util);
		}
		pthread_mutex_unlock(&mutex);
		break;
	default:
		ALOGE("free not support type: %d\n", flag);
		break;
	}
}

void lombo_malloc_dump()
{
#ifdef DEBUG_MALLOC
	int i;

	ALOGI("lombo_malloc_dump start\n");
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if (handles[i].use_flag == 1)
			ALOGI("unfree buffer size %d, file %s, line %d, index %d\n",
				handles[i].buf_size, handles[i].file,
				handles[i].line, i);
	}
	ALOGI("lombo_malloc_dump end\n");
#endif
}
