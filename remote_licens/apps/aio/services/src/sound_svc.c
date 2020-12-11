/*
 * sound_svc.c - Get the private data from private partition
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/prctl.h>
#include <sys/time.h>

#define LOG_TAG	"sound_svc"
#include <log/log.h>

#include "queue.h"
#include "file_ops.h"
#include "lombo_system_sound.h"
#include "sound_svc.h"

/*
#define DEBUG_TTS
*/

#define MAX_PLAY_QUEUE_SIZE 20
#define TTS_OUTBUF_SIZE (256*1024)

typedef enum {
	AUDIO_FORMAT_WAV = 0,
} audio_format_e;

typedef struct _sound_play_s {
	audio_format_e type;
	char *buf;
	int len;
} sound_play_t;

typedef struct _tts_convert_data_s {
	tts_format_e format;
	char *string;
	int len;
	int level;
	int greet_id;
	greet_param_t greet_param;
} tts_convert_data_t;

typedef struct _sound_svc_s {
	head_t sound_play_queue;
	pthread_mutex_t sound_queue_lock;
	pthread_cond_t sound_queue_cond;
	pthread_t sound_play_thrd;

	head_t tts_convert_queue;
	pthread_mutex_t tts_convert_queue_lock;
	pthread_cond_t tts_convert_queue_cond;
	pthread_t tts_convert_thrd;

	int stop;
	void *tts_hdl;
} sound_svc_t;

static sound_svc_t *sound_svc;

/**
 * get_systime_us - get current time (us).
 *
 * Returns current time.
 */
long long get_systime_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

/**
 * sound_play_thread - play audio data from queue.
 * @arg: a pointer of sound service
 *
 * Returns NULL.
 */
static void *sound_play_thread(void *arg)
{
	sound_svc_t *svc = (sound_svc_t *)arg;
	int ret = 0;
	sound_play_t *sound_play = NULL;

	ALOGV("enter %s", __func__);

	prctl(PR_SET_NAME, "sound-play");

	while (!svc->stop) {
		pthread_mutex_lock(&svc->sound_queue_lock);
		if (empty_queue(&svc->sound_play_queue)) {
			pthread_cond_wait(&svc->sound_queue_cond,
				&svc->sound_queue_lock);
		}
		if(svc->stop) {
			pthread_mutex_unlock(&svc->sound_queue_lock);
			break;
		}
		sound_play = (sound_play_t *)de_queue(&svc->sound_play_queue);
		pthread_mutex_unlock(&svc->sound_queue_lock);

		if (sound_play == NULL || sound_play->buf == NULL ||
				sound_play->len <= 0) {
			ALOGE("%s It should not happen", __func__);
			continue;
		}
		switch (sound_play->type) {
			case AUDIO_FORMAT_WAV: {
					ret = lombo_system_play_wav(sound_play->buf,
						sound_play->len);
					if (ret) {
						ALOGE("lombo system play wav failed");
					}
				}
				break;
			default:
				break;
		}
		/* free resource */
		free(sound_play->buf);
		sound_play->buf = NULL;
		free(sound_play);
		sound_play= NULL;
	}
	pthread_exit(NULL);
	return NULL;
}

/**
 * tts_convert_thread - play audio data from queue.
 * @arg: a pointer of sound service
 *
 * Returns NULL.
 */
static void *tts_convert_thread(void *arg)
{
	sound_svc_t *svc = (sound_svc_t *)arg;
	tts_convert_data_t *tts_convert_data = NULL;
	sound_play_t *sound_play = NULL;
	int flen = 0;
	long long begin;
	long long end;

	ALOGV("enter %s", __func__);

	prctl(PR_SET_NAME, "tts-convert");

	while (!svc->stop) {
		pthread_mutex_lock(&svc->tts_convert_queue_lock);
		if (empty_queue(&svc->tts_convert_queue)) {
			pthread_cond_wait(&svc->tts_convert_queue_cond,
				&svc->tts_convert_queue_lock);
		}
		if(svc->stop) {
			pthread_mutex_unlock(&svc->tts_convert_queue_lock);
			break;
		}
		tts_convert_data = (tts_convert_data_t *)de_queue(&svc->tts_convert_queue);
		pthread_mutex_unlock(&svc->tts_convert_queue_lock);

/*
		if (!tts_convert_data || tts_convert_data->format > TTS_FORMAT_MAX ||
			!tts_convert_data->string || tts_convert_data->len <= 0) {
			ALOGE("%s usually should not happen", __func__);
			continue;
		}
*/
		if (!tts_convert_data || tts_convert_data->format > TTS_FORMAT_MAX) {
			ALOGE("%s usually should not happen", __func__);
			continue;
		}

		sound_play = (sound_play_t *)malloc(sizeof(sound_play_t));
		if(!sound_play) {
			ALOGE("malloc sound_play_t failed, errno: %d (%s)\n",
				errno, strerror(errno));
			goto free_res;
		}
		memset(sound_play, 0, sizeof(sound_play_t));

		sound_play->buf = (char *)malloc(sizeof(char) * TTS_OUTBUF_SIZE);
		if(!sound_play->buf) {
			ALOGE("malloc sound_play buf failed, errno: %d (%s)\n",
				errno, strerror(errno));
			goto free_res;
		}
		memset(sound_play->buf, 0, sizeof(sizeof(char) * TTS_OUTBUF_SIZE));

		begin = get_systime_us();
		ALOGD("tts_getWavData begin = %lld", begin);
		ALOGD("tts_string: %s len: %d\n", tts_convert_data->string, tts_convert_data->len);
		tts_convert_data->greet_param.greeting_id = &tts_convert_data->greet_id;
		tts_convert_data->greet_param.greeting_num = 1;
		tts_getWavData(svc->tts_hdl, tts_convert_data->string, sound_play->buf,
			&flen, TTS_OUTBUF_SIZE, &tts_convert_data->greet_param);
		ALOGD("[%s][%d] id %d", __func__, __LINE__, tts_convert_data->greet_param.greeting_id[0]);
		end = get_systime_us();
		ALOGD("tts_getWavData end = %lld %lld", end, end - begin);
		ALOGD("tts_getWavData out size:%d", flen);

#ifdef DEBUG_TTS
		int count = 0;
		char path[128] = {0};
		FILE *tts_fp = NULL;

		sprintf(path, "/data/tts_%lld.wav", end);
		tts_fp = fopen(path, "ab");

		count = fwrite(sound_play->buf, 1, flen, tts_fp);
		ALOGD("count size:%d", count);
		fclose(tts_fp);
#endif

		if(flen <= 0) {
			ALOGE("tts_getWavData out size <= 0\n");
			goto free_res;
		}

		sound_play->type = AUDIO_FORMAT_WAV;
		sound_play->len = flen;
		pthread_mutex_lock(&svc->sound_queue_lock);
		if (tts_convert_data->level)
			en_queue_head(&svc->sound_play_queue, sound_play);
		else
			en_queue(&svc->sound_play_queue, sound_play);
		pthread_cond_signal(&svc->sound_queue_cond);
		pthread_mutex_unlock(&svc->sound_queue_lock);
		free(tts_convert_data->string);
		tts_convert_data->string = NULL;
		free(tts_convert_data);
		tts_convert_data = NULL;
		continue;

free_res:
		free(tts_convert_data->string);
		tts_convert_data->string = NULL;
		free(tts_convert_data);
		tts_convert_data = NULL;
		if (sound_play->buf) {
			free(sound_play->buf);
			sound_play->buf = NULL;
		}
		if (sound_play) {
			free(sound_play);
			sound_play = NULL;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

/**
 * sound_svc_init - initialize sound service.
 *
 * Returns 0  if success, other if failure.
 */
int sound_svc_init(void)
{
	sound_svc_t *svc = NULL;

	svc = (sound_svc_t *)malloc(sizeof(sound_svc_t));
	if (!svc) {
		ALOGE("malloc sound_svc_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}
	memset(svc, 0, sizeof(sound_svc_t));
	sound_svc = svc;

	svc->tts_hdl = tts_init();
	if (!svc->tts_hdl) {
		ALOGE("tts init failed");
		return -1;
	}

	TAILQ_INIT(&svc->sound_play_queue);
	if (pthread_mutex_init(&svc->sound_queue_lock, NULL)) {
		ALOGE("init sound_queue_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_cond_init(&svc->sound_queue_cond, NULL)) {
		ALOGE("init sound_queue_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	TAILQ_INIT(&svc->tts_convert_queue);
	if (pthread_mutex_init(&svc->tts_convert_queue_lock, NULL)) {
		ALOGE("init tts_convert_queue_lock failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	if (pthread_cond_init(&svc->tts_convert_queue_cond, NULL)) {
		ALOGE("init tts_convert_queue_cond failed, errno: %d (%s)\n",
			errno, strerror(errno));
		return -1;
	}

	ALOGV("create sound play thread");
	if (pthread_create(&svc->sound_play_thrd, NULL,
				sound_play_thread, (void *)svc)) {
		ALOGE("create sound play thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		return -1;
	}

	ALOGV("create tts convert thread");
	if (pthread_create(&svc->tts_convert_thrd, NULL,
				tts_convert_thread, (void *)svc)) {
		ALOGE("create tts convert thread failed, "
			"errno: %d (%s)", errno, strerror(errno));
		return -1;
	}

	return 0;
}

/**
 * sound_svc_init - deinitialize sound service.
 *
 * Returns 0  if success, other if failure.
 */
int sound_svc_deinit(void)
{
	sound_svc_t *svc = sound_svc;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

	svc->stop = 1;
	pthread_mutex_lock(&svc->sound_queue_lock);
	pthread_cond_signal(&svc->sound_queue_cond);
	pthread_mutex_unlock(&svc->sound_queue_lock);

	pthread_mutex_lock(&svc->tts_convert_queue_lock);
	pthread_cond_signal(&svc->tts_convert_queue_cond);
	pthread_mutex_unlock(&svc->tts_convert_queue_lock);

	pthread_join(svc->tts_convert_thrd, NULL);
	pthread_join(svc->sound_play_thrd, NULL);

	pthread_mutex_lock(&svc->sound_queue_lock);
	free_queue(&svc->sound_play_queue);
	pthread_mutex_unlock(&svc->sound_queue_lock);

	pthread_mutex_lock(&svc->tts_convert_queue_lock);
	free_queue(&svc->tts_convert_queue);
	pthread_mutex_unlock(&svc->tts_convert_queue_lock);

	pthread_cond_destroy(&svc->tts_convert_queue_cond);
	pthread_mutex_destroy(&svc->tts_convert_queue_lock);
	pthread_cond_destroy(&svc->sound_queue_cond);
	pthread_mutex_destroy(&svc->sound_queue_lock);

	free(svc);
	svc = NULL;

	return 0;
}

/**
 * sound_play_data - play audio data buffer.
 * @sound: a pointer of audio data
 *
 * Returns 0  if success, other if failure.
 */
int sound_play_data(sound_data_t *sound)
{
	sound_svc_t *svc = sound_svc;
	sound_play_t *sound_play = NULL;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

	if (!sound || sound->format > SOUND_FORMAT_MAX ||
		!sound->buf || sound->len <= 0) {
		ALOGE("sound data invalid");
		return -1;
	}

	sound_play = (sound_play_t *)malloc(sizeof(sound_play_t));
	if(!sound_play) {
		ALOGE("malloc sound_play_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play, 0, sizeof(sound_play_t));

	switch (sound->format) {
		case SOUND_FORMAT_WAV:
			sound_play->type = AUDIO_FORMAT_WAV;
			break;

		case SOUND_FORMAT_MAX:
		default:
			break;
	}

	sound_play->buf = (char *)malloc(sizeof(char) * sound->len);
	if(!sound_play->buf) {
		ALOGE("malloc sound_play buf failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play->buf, 0, sizeof(char) * sound->len);

	memcpy(sound_play->buf, sound->buf, sizeof(char) * sound->len);
	sound_play->len = sound->len;
	pthread_mutex_lock(&svc->sound_queue_lock);
	en_queue(&svc->sound_play_queue, sound_play);
	pthread_cond_signal(&svc->sound_queue_cond);
	pthread_mutex_unlock(&svc->sound_queue_lock);

	return 0;

exit:
	if (sound_play->buf) {
		free(sound_play->buf);
		sound_play->buf = NULL;
	}
	if (sound_play) {
		free(sound_play);
		sound_play = NULL;
	}

	return -1;
}

/**
 * sound_play_file - play audio file.
 * @path: a pointer of file name
 *
 * path 0  if success, other if failure.
 * support wav only  now
 */
int sound_play_file(const char *path)
{
	sound_svc_t *svc = sound_svc;
	sound_play_t *sound_play = NULL;
	FILE *fin = NULL;
	int flen = 0;
	int count = 0;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

	if (!path) {
		ALOGE("sound path invalid");
		return -1;
	}

	if (is_file_exist((char *)path)) {
		ALOGE("sound path no exist");
		return -1;
	}

	sound_play = (sound_play_t *)malloc(sizeof(sound_play_t));
	if(!sound_play) {
		ALOGE("malloc sound_play_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play, 0, sizeof(sound_play_t));

	fin = fopen(path, "rb");
	if (fin == NULL) {
		ALOGE("open %s failed\n", path);
		goto exit;
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	sound_play->buf = (char *)malloc(sizeof(char) * flen);
	if(!sound_play->buf) {
		ALOGE("malloc sound_play buf failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play->buf, 0, sizeof(char) * flen);

	count = fread(sound_play->buf, 1, flen, fin);
	if (count != flen) {
		ALOGE("fread count=%d len=%d\n", count, flen);
		goto exit;
	}

	sound_play->type = AUDIO_FORMAT_WAV;
	sound_play->len = flen;
	pthread_mutex_lock(&svc->sound_queue_lock);
	en_queue(&svc->sound_play_queue, sound_play);
	pthread_cond_signal(&svc->sound_queue_cond);
	pthread_mutex_unlock(&svc->sound_queue_lock);
	fclose(fin);
	return 0;

exit:
	if (sound_play->buf) {
		free(sound_play->buf);
		sound_play->buf = NULL;
	}
	if (sound_play) {
		free(sound_play);
		sound_play = NULL;
	}

	return -1;
}

/**
 * sound_play_tts - play tts data.
 * @tts: a pointer of tts data
 *
 * path 0  if success, other if failure.
 */
int sound_play_tts(tts_data_t *tts)
{
	sound_svc_t *svc = sound_svc;
	int q_size = 0;
	tts_convert_data_t *tts_convert_data = NULL;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

/*
	if (!tts || tts->format > TTS_FORMAT_MAX ||
		!tts->string || tts->len <= 0) {
		ALOGE("tts data invalid");
		return -1;
	}
*/
	if (!tts || tts->format > TTS_FORMAT_MAX) {
		ALOGE("tts data invalid");
		return -1;
	}

	pthread_mutex_lock(&svc->sound_queue_lock);
	q_size = queue_size(&svc->sound_play_queue);
	if (q_size > MAX_PLAY_QUEUE_SIZE) {
		pthread_mutex_unlock(&svc->sound_queue_lock);
		ALOGE("MAX_PLAY_QUEUE_SIZE LIMIT");
		return -2;
	}
	pthread_mutex_unlock(&svc->sound_queue_lock);

	tts_convert_data = (tts_convert_data_t *)malloc(sizeof(tts_convert_data_t));
	if(!tts_convert_data) {
		ALOGE("malloc tts_convert_data_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(tts_convert_data, 0, sizeof(tts_convert_data_t));

	tts_convert_data->string = (char *)malloc(sizeof(char) * (tts->len + 1));
	if(!tts_convert_data->string) {
		ALOGE("malloc tts_convert_data string failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(tts_convert_data->string, 0, tts->len + 1);

	tts_convert_data->format = tts->format;
	memcpy(tts_convert_data->string, tts->string, tts->len);
	tts_convert_data->len = tts->len;
	tts_convert_data->level = 0;
	tts_convert_data->greet_id = tts->greet_id;
	memcpy(&tts_convert_data->greet_param, &tts->greet_param, sizeof(greet_param_t));

	pthread_mutex_lock(&svc->tts_convert_queue_lock);
	en_queue(&svc->tts_convert_queue, tts_convert_data);
	pthread_cond_signal(&svc->tts_convert_queue_cond);
	pthread_mutex_unlock(&svc->tts_convert_queue_lock);
	return 0;

exit:
	if (tts_convert_data->string) {
		free(tts_convert_data->string);
		tts_convert_data->string = NULL;
	}
	if (tts_convert_data) {
		free(tts_convert_data);
		tts_convert_data = NULL;
	}

	return -1;
}

/**
 * sound_play_data - play audio data buffer.
 * @sound: a pointer of audio data
 *
 * Returns 0  if success, other if failure.
 */
int sound_play_data_faster(sound_data_t *sound)
{
	sound_svc_t *svc = sound_svc;
	sound_play_t *sound_play = NULL;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

	if (!sound || sound->format > SOUND_FORMAT_MAX ||
		!sound->buf || sound->len <= 0) {
		ALOGE("sound data invalid");
		return -1;
	}

	sound_play = (sound_play_t *)malloc(sizeof(sound_play_t));
	if(!sound_play) {
		ALOGE("malloc sound_play_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play, 0, sizeof(sound_play_t));

	switch (sound->format) {
		case SOUND_FORMAT_WAV:
			sound_play->type = AUDIO_FORMAT_WAV;
			break;

		case SOUND_FORMAT_MAX:
		default:
			break;
	}

	sound_play->buf = (char *)malloc(sizeof(char) * sound->len);
	if(!sound_play->buf) {
		ALOGE("malloc sound_play buf failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play->buf, 0, sizeof(char) * sound->len);

	memcpy(sound_play->buf, sound->buf, sizeof(char) * sound->len);
	sound_play->len = sound->len;
	pthread_mutex_lock(&svc->sound_queue_lock);
	en_queue_head(&svc->sound_play_queue, sound_play);
	pthread_cond_signal(&svc->sound_queue_cond);
	pthread_mutex_unlock(&svc->sound_queue_lock);

	return 0;

exit:
	if (sound_play->buf) {
		free(sound_play->buf);
		sound_play->buf = NULL;
	}
	if (sound_play) {
		free(sound_play);
		sound_play = NULL;
	}

	return -1;
}

/**
 * sound_play_file - play audio file.
 * @path: a pointer of file name
 *
 * path 0  if success, other if failure.
 * support wav only  now
 */
int sound_play_file_faster(const char *path)
{
	sound_svc_t *svc = sound_svc;
	sound_play_t *sound_play = NULL;
	FILE *fin = NULL;
	int flen = 0;
	int count = 0;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

	if (!path) {
		ALOGE("sound path invalid");
		return -1;
	}

	if (is_file_exist((char *)path)) {
		ALOGE("sound path no exist");
		return -1;
	}

	sound_play = (sound_play_t *)malloc(sizeof(sound_play_t));
	if(!sound_play) {
		ALOGE("malloc sound_play_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play, 0, sizeof(sound_play_t));

	fin = fopen(path, "rb");
	if (fin == NULL) {
		ALOGE("open %s failed\n", path);
		goto exit;
	}
	fseek(fin, 0, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	sound_play->buf = (char *)malloc(sizeof(char) * flen);
	if(!sound_play->buf) {
		ALOGE("malloc sound_play buf failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(sound_play->buf, 0, sizeof(char) * flen);

	count = fread(sound_play->buf, 1, flen, fin);
	if (count != flen) {
		ALOGE("fread count=%d len=%d\n", count, flen);
		goto exit;
	}

	sound_play->type = AUDIO_FORMAT_WAV;
	sound_play->len = flen;
	pthread_mutex_lock(&svc->sound_queue_lock);
	en_queue_head(&svc->sound_play_queue, sound_play);
	pthread_cond_signal(&svc->sound_queue_cond);
	pthread_mutex_unlock(&svc->sound_queue_lock);
	fclose(fin);
	return 0;

exit:
	if (sound_play->buf) {
		free(sound_play->buf);
		sound_play->buf = NULL;
	}
	if (sound_play) {
		free(sound_play);
		sound_play = NULL;
	}

	return -1;
}

/**
 * sound_play_tts - play tts data.
 * @tts: a pointer of tts data
 *
 * path 0  if success, other if failure.
 */
int sound_play_tts_faster(tts_data_t *tts)
{
	sound_svc_t *svc = sound_svc;
	int q_size = 0;
	tts_convert_data_t *tts_convert_data = NULL;

	if (!svc) {
		ALOGE("sound svc not init");
		return -1;
	}

/*
	if (!tts || tts->format > TTS_FORMAT_MAX ||
		!tts->string || tts->len <= 0) {
		ALOGE("tts data invalid");
		return -1;
	}
*/
	if (!tts || tts->format > TTS_FORMAT_MAX) {
		ALOGE("tts data invalid");
		return -1;
	}

	pthread_mutex_lock(&svc->sound_queue_lock);
	q_size = queue_size(&svc->sound_play_queue);
	if (q_size > MAX_PLAY_QUEUE_SIZE) {
		pthread_mutex_unlock(&svc->sound_queue_lock);
		ALOGE("MAX_PLAY_QUEUE_SIZE LIMIT");
		return -2;
	}
	pthread_mutex_unlock(&svc->sound_queue_lock);

	tts_convert_data = (tts_convert_data_t *)malloc(sizeof(tts_convert_data_t));
	if(!tts_convert_data) {
		ALOGE("malloc tts_convert_data_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(tts_convert_data, 0, sizeof(tts_convert_data_t));

	tts_convert_data->string = (char *)malloc(sizeof(char) * (tts->len + 1));
	if(!tts_convert_data->string) {
		ALOGE("malloc tts_convert_data string failed, errno: %d (%s)\n",
			errno, strerror(errno));
		goto exit;
	}
	memset(tts_convert_data->string, 0, tts->len + 1);

	tts_convert_data->format = tts->format;
	memcpy(tts_convert_data->string, tts->string, tts->len);
	tts_convert_data->len = tts->len;
	tts_convert_data->level = 1;
	memcpy(&tts_convert_data->greet_param, &tts->greet_param, sizeof(greet_param_t));

	pthread_mutex_lock(&svc->tts_convert_queue_lock);
	en_queue_head(&svc->tts_convert_queue, tts_convert_data);
	pthread_cond_signal(&svc->tts_convert_queue_cond);
	pthread_mutex_unlock(&svc->tts_convert_queue_lock);
	return 0;

exit:
	if (tts_convert_data->string) {
		free(tts_convert_data->string);
		tts_convert_data->string = NULL;
	}
	if (tts_convert_data) {
		free(tts_convert_data);
		tts_convert_data = NULL;
	}

	return -1;
}


int sound_stop_all(void)
{
	sound_svc_t *svc = sound_svc;
	sound_play_t *sound_play = NULL;
	tts_convert_data_t *tts_convert_data = NULL;


	pthread_mutex_lock(&svc->sound_queue_lock);
	while (!empty_queue(&svc->sound_play_queue)){
		sound_play = (sound_play_t *)de_queue(&svc->sound_play_queue);
		free(sound_play->buf);
		free(sound_play);
	}
	pthread_mutex_unlock(&svc->sound_queue_lock);

	pthread_mutex_lock(&svc->tts_convert_queue_lock);
	while (!empty_queue(&svc->tts_convert_queue)){
		tts_convert_data = (tts_convert_data_t *)de_queue(&svc->tts_convert_queue);
		free(tts_convert_data->string);
		free(tts_convert_data);
	}
	//free_queue(&svc->tts_convert_queue);
	//free_queue(&svc->sound_play_queue);
	pthread_mutex_unlock(&svc->tts_convert_queue_lock);

	return 0;
}

