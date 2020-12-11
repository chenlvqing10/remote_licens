/*
* lombo_limits.h
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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef TINYALSA_LIMITS_H
#define TINYALSA_LIMITS_H

#include <interval.h>

#include <limits.h>
#include <stdint.h>

#define TINYALSA_SIGNED_INTERVAL_MAX SSIZE_MAX
#define TINYALSA_SIGNED_INTERVAL_MIN SSIZE_MIN

#define TINYALSA_UNSIGNED_INTERVAL_MAX SIZE_MAX
#define TINYALSA_UNSIGNED_INTERVAL_MIN SIZE_MIN

#define TINYALSA_CHANNELS_MAX 32U
#define TINYALSA_CHANNELS_MIN 1U

#define TINYALSA_FRAMES_MAX (ULONG_MAX / (TINYALSA_CHANNELS_MAX * 4))
#define TINYALSA_FRAMES_MIN 0U

#if TINYALSA_FRAMES_MAX > TINYALSA_UNSIGNED_INTERVAL_MAX
#error "Frames max exceeds measurable value."
#endif

#if TINYALSA_FRAMES_MIN < TINYALSA_UNSIGNED_INTERVAL_MIN
#error "Frames min exceeds measurable value."
#endif

extern const struct tinyalsa_unsigned_interval tinyalsa_channels_limit;

extern const struct tinyalsa_unsigned_interval tinyalsa_frames_limit;

#endif /* TINYALSA_LIMITS_H */

