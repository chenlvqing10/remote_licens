/*
 * oscl_delay.h - time api used by lombo omx.
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

#ifndef __LB_OSCL_TIME_H__
#define __LB_OSCL_TIME_H__

#if defined(__EOS__)

#include <rtthread.h>

#define oscl_mdelay(m) rt_thread_mdelay(m)
#define oscl_get_msec()	rt_time_get_msec()
#define oscl_get_usec() rt_time_get_usec()

#else /* __EOS__ */

#include <unistd.h>
#include <sys/types.h>
#include <time.h>

static inline void msleep(unsigned int ms)
{
	usleep(ms * 1000);
}

static inline void mdelay(unsigned int ms)
{
	usleep(ms * 1000);
}

#define oscl_mdelay(m) mdelay(m)

static inline int64_t oscl_get_usec(void)
{
	struct timespec t;
	t.tv_sec = t.tv_nsec = 0;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (int64_t)(t.tv_sec)*1000000LL + t.tv_nsec/1000LL;
}

static inline int64_t oscl_get_msec(void)
{
	struct timespec t;
	t.tv_sec = t.tv_nsec = 0;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (int64_t)(t.tv_sec)*1000LL + t.tv_nsec/1000000LL;
}

#endif

#endif /* __LB_OSCL_TIME_H__ */

