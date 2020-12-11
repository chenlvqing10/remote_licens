/*
 * oscl.h - common lib api used by lombo media player.
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

#ifndef __LB_OSCL_H__
#define __LB_OSCL_H__

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>

#if defined(__EOS__)
#include <rtthread.h>
#include "soc_define.h"
#ifdef __QEMU__
#include "qemu.h"
#endif
#include "board.h"

#define prctl(fmt, ...)   do {} while (0)
#endif

#if defined(__linux__)
#include <sys/prctl.h>

#define OMX_DYNAMIC_LOADING
#endif

#include "oscl.h"
#include "oscl_debug.h"
#include "oscl_queue.h"
#include "oscl_mem.h"
#include "oscl_time.h"
#include "oscl_video_frame.h"
#include "oscl_semaphore.h"

#define ALIGN_4K(x) (((x) + (4095)) & ~(4095))
#define ALIGN_1K(x) (((x) + (1023)) & ~(1023))
#define ALIGN_32B(x) (((x) + (31)) & ~(31))
#define ALIGN_16B(x) (((x) + (15)) & ~(15))
#define ALIGN_8B(x) (((x) + (7)) & ~(7))

#ifndef INT_MAX
#define INT_MAX		((int)(~0U>>1)) /* maximum (signed) int value */
#endif
#ifndef LLONG_MAX
#define LLONG_MAX	((long long)(~0ULL>>1))
#endif

#define LB_FILE_SYSTEM_PRIO 16
#define LB_RECORDER_SINK_PRIO 15
#define LB_RECORDER_MUXER_PRIO 16
#define LB_RECORDER_AUDIO_PRIO 17
#define LB_RECORDER_VSRC_PRIO 17
#define LB_RECORDER_VSPLIT_PRIO 17
#define LB_RECORDER_VIDEO_PRIO 17
#define LB_FILE_CLOSE_PRIO 24

#endif /* __LB_OSCL_H__ */
