/*
 * common.h - nna test common function
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
#ifndef __NNA_TEST_COMMON_H__
#define __NNA_TEST_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/time.h>

#define ERROR_CHECK_STATUS(status) { \
		vx_status status_ = (status); \
		if (status_ != VX_SUCCESS) { \
			printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
			exit(1); \
		} \
	}

#define ERROR_CHECK_OBJECT(obj) { \
		vx_status status_ = vxGetStatus((vx_reference)(obj)); \
		if (status_ != VX_SUCCESS) { \
			printf("ERROR: failed with status = (%d) at " __FILE__ "#%d\n", status_, __LINE__); \
			exit(1); \
		} \
	}

inline long int getMicrotime()
{
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int) 1e6 + currentTime.tv_usec;
}

#ifdef __cplusplus
}
#endif

#endif /*__NNA_TEST_COMMON_H__*/
