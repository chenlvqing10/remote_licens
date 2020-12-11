/*
* lombo_sound_cap.c
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

#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <lombo_limits.h>
#include <pthread.h>
#include "lombo_sound_cap.h"

#define LOG_TAG "SOUND_CAP"
#include <log/log.h>

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

struct wav_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t riff_fmt;
	uint32_t fmt_id;
	uint32_t fmt_sz;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint32_t data_id;
	uint32_t data_sz;
};

int capturing = 0;
int prinfo = 1;

unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
			unsigned int channels, unsigned int rate,
			enum pcm_format format, unsigned int period_size,
			unsigned int period_count, unsigned int capture_time)
{
	struct pcm_config config;
	struct pcm *pcm;
	char *buffer;
	unsigned int size;
	unsigned int frames_read;
	unsigned int total_frames_read;
	unsigned int bytes_per_frame;

	memset(&config, 0, sizeof(config));
	config.channels = channels;
	config.rate = rate;
	config.period_size = period_size;
	config.period_count = period_count;
	config.format = format;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

	pcm = pcm_open(card, device, PCM_IN, &config);
	if (!pcm || !pcm_is_ready(pcm)) {
		ALOGE("Unable to open PCM device (%s)\n",
		pcm_get_error(pcm));
		return 0;
	}

	size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
	buffer = malloc(size);
	if (!buffer) {
		ALOGE("Unable to allocate %u bytes\n", size);
		pcm_close(pcm);
		return 0;
	}

	if (prinfo) {
		ALOGD("Capturing sample: %u ch, %u hz, %u bit\n", channels, rate,
		pcm_format_to_bits(format));
	}

	bytes_per_frame = pcm_frames_to_bytes(pcm, 1);
	total_frames_read = 0;
	frames_read = 0;
	while (capturing) {
		frames_read = pcm_readi(pcm, buffer, pcm_get_buffer_size(pcm));
		total_frames_read += frames_read;
		if ((total_frames_read / rate) >= capture_time)
			capturing = 0;
		if (fwrite(buffer, bytes_per_frame, frames_read, file) != frames_read) {
			ALOGE("Error capturing sample\n");
			break;
		}
	}

	free(buffer);
	pcm_close(pcm);
	return total_frames_read;
}

void sigint_handler(int sig)
{
	if (sig == SIGINT) {
		/* capturing = 0; */
		lombo_sound_cap_stop();
	}
}

void *lombo_sound_cap_do(void *arg)
{
	FILE *file;
	struct wav_header header;
	unsigned int card = 0;
	unsigned int device = 0;
	unsigned int channels = 2;
	unsigned int rate = 16000;
	unsigned int bits = 16;
	unsigned int frames;
	unsigned int period_size = 1024;
	unsigned int period_count = 4;
	unsigned int capture_time = UINT_MAX;
	enum pcm_format format;
	int no_header = 0;
	const char *file_name = (const char *)arg;

	file = fopen(file_name, "wb");
	if (!file) {
		ALOGE("Unable to create file '%s'\n", file_name);
		return NULL;
	}

	header.riff_id = ID_RIFF;
	header.riff_sz = 0;
	header.riff_fmt = ID_WAVE;
	header.fmt_id = ID_FMT;
	header.fmt_sz = 16;
	header.audio_format = FORMAT_PCM;
	header.num_channels = channels;
	header.sample_rate = rate;

	switch (bits) {
	case 32:
		format = PCM_FORMAT_S32_LE;
		break;
	case 24:
		format = PCM_FORMAT_S24_LE;
		break;
	case 16:
		format = PCM_FORMAT_S16_LE;
		break;
	default:
		ALOGE("%u bits is not supported.\n", bits);
		return NULL;
	}

	header.bits_per_sample = pcm_format_to_bits(format);
	header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
	header.block_align = channels * (header.bits_per_sample / 8);
	header.data_id = ID_DATA;

	/* leave enough room for header */
	if (!no_header)
		fseek(file, sizeof(struct wav_header), SEEK_SET);

	/* install signal handler and begin capturing */
	signal(SIGINT, sigint_handler);
	frames = capture_sample(file, card, device, header.num_channels,
	header.sample_rate, format,
	period_size, period_count, capture_time);
	if (prinfo)
		ALOGD("Captured %u frames\n", frames);

	/* write header now all information is known */
	if (!no_header) {
		header.data_sz = frames * header.block_align;
		header.riff_sz = header.data_sz + sizeof(header) - 8;
		fseek(file, 0, SEEK_SET);
		fwrite(&header, sizeof(struct wav_header), 1, file);
	}

	fclose(file);

	return NULL;
}

int lombo_sound_cap_start(const char *file_name)
{
	pthread_t p;

	ALOGD("lombo_sound_cap_start file[%s]", file_name);
	if (capturing == 0)
		capturing = 1;
	else {
		ALOGD("lombo_sound_cap is runing, please stop it first");
		return -1;
	}

	pthread_create(&p, NULL, lombo_sound_cap_do, (void *)file_name);

	return 0;
}

int lombo_sound_cap_stop(void)
{
	capturing = 0;

	ALOGD("lombo_sound_cap_stop");

	return 0;
}

