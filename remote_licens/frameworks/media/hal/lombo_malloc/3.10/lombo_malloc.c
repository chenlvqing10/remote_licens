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
#include "lombo_malloc.h"
#include "lombo_ion.h"

#define DEBUG_MALLOC
#define MAX_HANDLE_NUM 512
#define ION_ALLOC_ALIGN 0x1000	/* 4k align */
#define DEV_NAME		"/dev/ion"

typedef struct buffer_hanlde_t {
	int buffer_type;
	int handle;
	int fd;
	void *vir_addr;
	unsigned long phy_addr;
	void *vir_addr_align;
	unsigned long phy_addr_align;
	int use_flag;
	int buf_size;
	int offset;
#ifdef DEBUG_MALLOC
	char file[128];
	int line;
#endif
} buffer_hanlde;

static int fd_ion;
static int buffer_handle_num;
static buffer_hanlde handles[MAX_HANDLE_NUM];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *lombo_memcpy(void *dst, void *src, int size)
{
	if (size < 0)
		ALOGE("memcpy size smaller than 0, need debug\n");
	return memcpy(dst, src, size);
}

void *lombo_memset(void *prt, int val, int size)
{
	if (size < 0)
		ALOGE("memset size smaller than 0, need debug\n");
	return memset(prt, val, size);
}

static int get_unuse_handle_index()
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

static int find_index_by_viraddr_align(void *vir_addr_align)
{
	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr_align == vir_addr_align))
			return i;
	}
	ALOGE("the buffer 0x%p can not be find , need debug\n", vir_addr_align);
	return -1;
}

unsigned int lombo_vir_to_fd(void *vir_addr_align)
{
	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr_align == vir_addr_align))
			return handles[i].fd;
	}
	ALOGE("the buffer 0x%p can not be find,need debug\n", vir_addr_align);
	return -1;
}

unsigned long lombo_fd_to_phy(unsigned int fd)
{
	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) && (handles[i].fd == fd))
			return handles[i].phy_addr_align;
	}
	ALOGE("the buffer fd:%d can not be find ,we can not get fd ,need debug\n", fd);
	return -1;
}

unsigned long lombo_vir_to_phy(void *vir_addr_align)
{
	int i = 0;

	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr_align == vir_addr_align))
			return handles[i].phy_addr_align;
	}
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr_align <= vir_addr_align) &&
			(handles[i].vir_addr_align + handles[i].buf_size >
				vir_addr_align))
			return handles[i].phy_addr_align +
				(vir_addr_align - handles[i].vir_addr_align);
	}
	ALOGE("can not find the buffer %p in the list\n", vir_addr_align);
	return 0;
}

int lombo_cache_flush(unsigned long phy_addr_align)
{
	struct ion_fd_data fd_data;
	int ret = 0;

	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].phy_addr_align == phy_addr_align))
			break;
	}
	if (i == MAX_HANDLE_NUM) {
		ALOGE("can not find the buffer in the list\n");
		return -1;
	}
	fd_data.fd = handles[i].fd;
	if ((handles[i].buffer_type != MEM_WB) && (handles[i].buffer_type != MEM_WT)) {
		ALOGI("buf_addr : 0x%lx do not need sync\n", phy_addr_align);
		return 0;
	}
	ret = ioctl(fd_ion, ION_IOC_SYNC, &fd_data);
	if (ret) {
		ALOGE("get dmabuffer fd error\n");
		return -1;
	}
	return 0;
}

int lombo_cache_flush_vir(void *vir_addr_align)
{
	struct ion_fd_data fd_data;
	int ret = 0;

	int i = 0;
	for (i = 0; i < MAX_HANDLE_NUM; i++) {
		if ((handles[i].use_flag == 1) &&
			(handles[i].vir_addr_align == vir_addr_align))
			break;
	}
	if (i == MAX_HANDLE_NUM) {
		ALOGE("can not find the buffer in the list\n");
		return -1;
	}
	fd_data.fd = handles[i].fd;
	if ((handles[i].buffer_type != MEM_WB) && (handles[i].buffer_type != MEM_WT)) {
		ALOGI("buf_addr : %p do not need sync\n", vir_addr_align);
		return 0;
	}
	ret = ioctl(fd_ion, ION_IOC_SYNC, &fd_data);
	if (ret) {
		ALOGE("get dmabuffer fd error\n");
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
	struct ion_fd_data fd_data;
	void *addr_vir = NULL;
	struct ion_custom_data custom_data;
	struct lombo_phys_data phys_data;
	struct ion_allocation_data alloc_data;

	if (length <= 0) {
		ALOGE("malloc buf size smaller <= 0\n");
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
		}
		buffer_handle_num++;
		index = get_unuse_handle_index();
		pthread_mutex_unlock(&mutex);

		if (index == -1) {
			ALOGE("can't find valid handle index\n");
			return NULL;
		}

		alloc_data.len = (size_t)length;
		alloc_data.align = ION_ALLOC_ALIGN;
		alloc_data.heap_id_mask = ION_HEAP_CARVEOUT_MASK;

		if (buffer_type == MEM_UC)
			alloc_data.flags = 0;
		else if ((buffer_type == MEM_WT) || (buffer_type == MEM_WB))
			alloc_data.flags = ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC;

		ret = ioctl(fd_ion, ION_IOC_ALLOC, &alloc_data);
		if (ret) {
			ALOGE("alloc ion buffer error %s --- %d\n", file, line);
			return NULL;
		}

		fd_data.handle = alloc_data.handle;
		ret = ioctl(fd_ion, ION_IOC_MAP, &fd_data);
		if (ret) {
			ALOGE("alloc ION_IOC_MAP error.\n");
			return NULL;
		}
		addr_vir = mmap(NULL, alloc_data.len,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd_data.fd, 0);

		lombo_memset((void *)(&phys_data), 0, sizeof(phys_data));
		phys_data.handle = alloc_data.handle;
		custom_data.cmd = ION_IOC_LB_PHYS_ADDR;
		custom_data.arg = (unsigned long)&phys_data;
		ret = ioctl(fd_ion, ION_IOC_CUSTOM, &custom_data);
		handles[index].handle = alloc_data.handle;
		handles[index].fd = fd_data.fd;
		handles[index].buffer_type = buffer_type;
		handles[index].vir_addr = (void *)addr_vir;
		handles[index].vir_addr_align = (void *)addr_vir;
		handles[index].phy_addr = phys_data.phys_addr;
		handles[index].phy_addr_align = phys_data.phys_addr;
		handles[index].buf_size = size;

#ifdef DEBUG_MALLOC
		handles[index].line = line;
		handles[index].file[127] = '\0';
		strncpy(handles[index].file, file, 127);
#endif
		ptr = (void *)addr_vir;
		if (phy_addr)
			*phy_addr = (phys_data.phys_addr);

		ALOGI("vir_addr=%p  phys_addr=%lx fd=%d size=%d\n",
			ptr, phys_data.phys_addr, handles[index].fd, length);
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
	} else
		return calloc(num, size);
}

void lombo_free(void *ptr, int flag)
{
	int index;
	int ret = 0;
	struct ion_handle_data handle_data;

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
		index = find_index_by_viraddr_align(ptr);
		if (index != -1) {
			ret = munmap(handles[index].vir_addr, handles[index].buf_size);
			if (ret) {
				ALOGE("munmap failed\n");
				return;
			}
			handles[index].buffer_type = 0;
			handles[index].vir_addr = NULL;
			handles[index].phy_addr = 0;
			handles[index].vir_addr_align = NULL;
			handles[index].phy_addr_align = 0;
			close(handles[index].fd);
			handles[index].fd = 0;
			handles[index].use_flag = 0;
			handle_data.handle = handles[index].handle;
			ret = ioctl(fd_ion, ION_IOC_FREE, &handle_data);
			if (ret != 0)
				ALOGE("free buffer 0x%p error\n", ptr);
			handles[index].handle = 0;
		} else {
			pthread_mutex_unlock(&mutex);
			break;
		}
		buffer_handle_num--;
		ALOGI("free buffer_handle_num : %d\n", buffer_handle_num);
		if (buffer_handle_num == 0)
			close(fd_ion);
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
