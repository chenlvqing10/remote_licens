/*
 * sound_svc.h - Get the private data from private partition
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#ifndef __SOUND_SVC_H__
#define __SOUND_SVC_H__

#include "tts_ext.h"

/* string format of tts */
typedef enum {
	TTS_FORMAT_UTF8 = 0,
	TTS_FORMAT_MAX
} tts_format_e;

/* audio format of tts */
typedef enum {
	SOUND_FORMAT_WAV = 0,
	SOUND_FORMAT_MAX
} sound_format_e;

/* source data of tts (string) */
typedef struct _tts_data {
	tts_format_e format;
	char *string;
	int len;
	int greet_id;
	greet_param_t greet_param;
} tts_data_t;

/* audio data of tts (audio data buffer) */
typedef struct sound_data_s {
	sound_format_e format;
	char *buf;
	int len;
} sound_data_t;

/**
 * sound_svc_init - initialize sound service.
 *
 * Returns 0  if success, other if failure.
 */
int sound_svc_init(void);

/**
 * sound_svc_init - deinitialize sound service.
 *
 * Returns 0  if success, other if failure.
 */
int sound_svc_deinit(void);

/**
 * sound_play_data - play audio data buffer.
 * @sound: a pointer of audio data
 *
 * Returns 0  if success, other if failure.
 */
int sound_play_data(sound_data_t *sound);

/**
 * sound_play_file - play audio file.
 * @path: a pointer of file name
 *
 * path 0  if success, other if failure.
 * support wav only  now
 */
int sound_play_file(const char *path);

/**
 * sound_play_tts - play tts data.
 * @tts: a pointer of tts data
 *
 * path 0  if success, other if failure.
 */
int sound_play_tts(tts_data_t *tts);

/**
 * sound_play_data_faster - play audio data buffer.
 * @sound: a pointer of audio data
 *
 * Returns 0  if success, other if failure.
 */
int sound_play_data_faster(sound_data_t *sound);

/**
 * sound_play_file_faster - play audio file.
 * @path: a pointer of file name
 *
 * path 0  if success, other if failure.
 * support wav only  now
 */
int sound_play_file_faster(const char *path);

/**
 * sound_play_tts_faster - play tts data.
 * @tts: a pointer of tts data
 *
 * path 0  if success, other if failure.
 */
int sound_play_tts_faster(tts_data_t *tts);

int sound_stop_all(void);


#endif /* __SOUND_SVC_H__ */

