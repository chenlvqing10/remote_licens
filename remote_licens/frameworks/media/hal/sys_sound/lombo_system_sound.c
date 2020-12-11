/*
* lombo_system_sound.c
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
#define LOG_TAG                "audio_gain"

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <log/log.h>
#include "lombo_system_sound.h"
#include "audio_gain.h"

#define DEFAULT_PERIOD_SIZE	512
#define DEFAULT_BUFFER_SIZE	2048
#define MAX_REG_V_CNT		25

static const unsigned int reg_volumes[MAX_REG_V_CNT] = {
	0x00, 0x16, 0x26, 0x29, 0x2c, 0x2f, 0x32,
	0x35, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42,
	0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50,
	0x51, 0x52, 0x53, 0x54,
};

static const unsigned int record_volumes[MAX_REG_V_CNT] = {
	0x50, 0x4e, 0x4c, 0x4a, 0x47, 0x44, 0x41,
	0x3e, 0x39, 0x35, 0x31, 0x2d, 0x29, 0x25,
	0x21, 0x1e, 0x1b, 0x18, 0x16, 0x12, 0x10,
	0xe, 0xc, 0xa, 0x8,
};

/* default sys sound volume is 79, 0db */
static unsigned int sys_snd_volume = 79;
/* default master volume 100 */
static unsigned int master_volume = 100;
static unsigned int master_record_volume = 100;

struct cmd {
	const char *filename;
	const char *filetype;
	unsigned int card;
	unsigned int device;
	int flags;
	unsigned int channels;
	unsigned int rate;
	unsigned int format;
	unsigned int bits;
	unsigned long file_length; /*bytes*/
};

void cmd_init(struct cmd *cmd)
{
	cmd->filename = NULL;
	cmd->filetype = NULL;
	cmd->card = 0;
	cmd->device = 0;
	cmd->flags = SND_PCM_STREAM_PLAYBACK;
	cmd->channels = 2;
	cmd->rate = 48000;
	cmd->format = SND_PCM_FORMAT_S16_LE;
	cmd->bits = 16;
}

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT	0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t wave_id;
};

struct chunk_header {
	uint32_t id;
	uint32_t sz;
};

struct chunk_fmt {
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
};

struct ctx {
	snd_pcm_t *pcm;

	struct riff_wave_header wave_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;
	unsigned int bytes_per_frame;

	FILE *file;
};

int ctx_init(struct ctx *ctx, struct cmd *cmd)
{
	int ret;
	snd_pcm_hw_params_t *hw = NULL;

	if (cmd->filename == NULL) {
		ALOGE("filename not specified\n");
		return -1;
	}

	ctx->file = fopen(cmd->filename, "rb");
	if (ctx->file == NULL) {
		ALOGE("failed to open '%s'\n", cmd->filename);
		return -1;
	}

	if ((cmd->filetype != NULL) && (strcmp(cmd->filetype, "wav") == 0)) {
		if (fread(&ctx->wave_header, sizeof(ctx->wave_header), 1,
			ctx->file) != 1){
			ALOGE("err: '%s' not contain a riff/wave header\n",
			    cmd->filename);
			fclose(ctx->file);
			return -1;
		}
		if ((ctx->wave_header.riff_id != ID_RIFF) ||
			(ctx->wave_header.wave_id != ID_WAVE)) {
			ALOGE("error: '%s' is not a riff/wave file\n",
				cmd->filename);
			fclose(ctx->file);
			return -1;
		}
		unsigned int more_chunks = 1;
		do {
			if (fread(&ctx->chunk_header, sizeof(ctx->chunk_header), 1,
				ctx->file) != 1) {
				ALOGE("err: '%s' not contain a data chunk\n",
				    cmd->filename);
				fclose(ctx->file);
				return -1;
			}
			switch (ctx->chunk_header.id) {
			case ID_FMT:
				if (fread(&ctx->chunk_fmt, sizeof(ctx->chunk_fmt), 1,
				ctx->file) != 1){
					ALOGE("err: '%s' incomplete format chunk\n",
					    cmd->filename);
					fclose(ctx->file);
					return -1;
				}
				/* If the format header is larger, skip the rest */
				if (ctx->chunk_header.sz > sizeof(ctx->chunk_fmt))
					fseek(ctx->file, ctx->chunk_header.sz -
						sizeof(ctx->chunk_fmt), SEEK_CUR);
				break;
			case ID_DATA:
				/* Stop looking for chunks */
				more_chunks = 0;
				cmd->file_length = ctx->chunk_header.sz;
				ALOGI("file size %d\n", ctx->chunk_header.sz);
				break;
			default:
				/* Unknown chunk, skip bytes */
				fseek(ctx->file, ctx->chunk_header.sz, SEEK_CUR);
			}
		} while (more_chunks);
		cmd->channels = ctx->chunk_fmt.num_channels;
		cmd->rate = ctx->chunk_fmt.sample_rate;
		cmd->bits = ctx->chunk_fmt.bits_per_sample;
		ctx->bytes_per_frame = cmd->bits * cmd->channels / 8;
		ret = ftell(ctx->file);
		ALOGI("file pos %d\n", ret);
	}

	if (cmd->bits == 8) {
		cmd->format = SND_PCM_FORMAT_U8;
	} else if (cmd->bits == 16) {
		cmd->format = SND_PCM_FORMAT_S16_LE;
	} else if (cmd->bits == 24) {
		if (ctx->chunk_fmt.block_align == 6)
			cmd->format = SND_PCM_FORMAT_S24_3LE;
		else
			cmd->format = SND_PCM_FORMAT_S24_LE;
	} else if (cmd->bits == 32) {
		cmd->format = SND_PCM_FORMAT_S32_LE;
	} else {
		ALOGE("bit count '%u' not supported\n", cmd->bits);
		fclose(ctx->file);
		return -1;
	}

	ret = snd_pcm_open(&ctx->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (ret < 0) {
		ALOGE("open pcm err\n");
		return -1;
	}
	snd_pcm_hw_params_malloc(&hw);
	snd_pcm_hw_params_any(ctx->pcm, hw);
	snd_pcm_hw_params_set_access(ctx->pcm,
		hw, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(ctx->pcm,
		hw, cmd->format);
	snd_pcm_hw_params_set_channels(ctx->pcm, hw, cmd->channels);
	snd_pcm_hw_params_set_rate(ctx->pcm, hw, cmd->rate, 0);

	snd_pcm_hw_params_set_period_size(ctx->pcm, hw, DEFAULT_PERIOD_SIZE, 0);
	snd_pcm_hw_params_set_buffer_size(ctx->pcm, hw, DEFAULT_BUFFER_SIZE);
	if (snd_pcm_hw_params(ctx->pcm, hw) < 0) {
		ALOGE("snd_pcm_hw_params error\n");
		snd_pcm_hw_params_free(hw);
		snd_pcm_close(ctx->pcm);
		return -1;
	}
	snd_pcm_hw_params_free(hw);
	return 0;
}

void ctx_free(struct ctx *ctx)
{
	if (ctx == NULL)
		return;

	if (ctx->pcm != NULL)
		snd_pcm_close(ctx->pcm);

	if (ctx->file != NULL)
		fclose(ctx->file);
}

static int lombo_sound_close;

static int lombo_system_sound_init()
{
	lombo_sound_close = 0;

	return 0;
}

int play_sample(struct ctx *ctx, struct cmd *cmd)
{
	int ret;
	char *buffer;
	int size;
	int num_read;
	void *gain;
	int init_volume = 0;
	snd_pcm_uframes_t count;
	unsigned int written = 0;
	unsigned int read_size = 0;
	unsigned int file_size = 0;
	/* Reinitialize the playing falg bit */
	lombo_system_sound_init();

	size = ctx->bytes_per_frame * DEFAULT_BUFFER_SIZE;
	buffer = malloc(size);
	if (!buffer) {
		ALOGE("unable to allocate %d bytes\n", size);
		return -1;
	}
	gain = audio_gain_open(ctx->chunk_fmt.num_channels,
			ctx->chunk_fmt.bits_per_sample);
	if (gain == NULL) {
		free(buffer);
		buffer = NULL;
		ALOGE("unable to open gain\n");
		return -1;
	}

	audio_gain_set_volume(gain, sys_snd_volume);
	init_volume = sys_snd_volume;

	/* catch ctrl-c to shutdown cleanly */
	/* signal(SIGINT, stream_close); */
	file_size = cmd->file_length;
	do {
		/* Dynamic volume change */
		if (init_volume != sys_snd_volume) {
			init_volume = sys_snd_volume;
			audio_gain_set_volume(gain, sys_snd_volume);
		}
		memset(buffer, 0, size);
		read_size = file_size - written;
		if (read_size > size)
			read_size = size;
		if (read_size == 0)
			break;
		num_read = fread(buffer, 1, read_size, ctx->file);
		count = num_read / ctx->bytes_per_frame;
		ALOGI("read bytes %d\n", num_read);
		if (num_read > 0) {
			audio_gain_apply(gain, buffer, buffer, num_read);
			if (count < DEFAULT_BUFFER_SIZE) {
				ret = snd_pcm_format_set_silence(cmd->format,
					buffer + count * ctx->bytes_per_frame,
					(DEFAULT_BUFFER_SIZE - count) * cmd->channels);
				if (ret < 0)
					ALOGE("set_silence error\n");
				ALOGI("set_silence %lu\n", count);
				count = DEFAULT_BUFFER_SIZE;
			}
			ret = snd_pcm_writei(ctx->pcm, buffer, count);
			if (ret == -EAGAIN ||
				(ret >= 0 && (snd_pcm_uframes_t)ret < count)) {
				snd_pcm_wait(ctx->pcm, 100);
				ALOGE("wait 100ms\n");
			} else if (ret == -EPIPE) {
				ALOGE("error underrun\n");
				snd_pcm_prepare(ctx->pcm);
			} else if (ret < 0) {
				ALOGE("write error: %d\n", ret);
				break;
			}
		}
		written += count * ctx->bytes_per_frame;
		/*ALOGI("written bytes %d\n", written);*/
	} while (!lombo_sound_close && num_read > 0 && written < file_size);

	snd_pcm_drain(ctx->pcm);
	audio_gain_close(gain);
	free(buffer);
	return 0;
}

void stream_close(int sig)
{
	/* allow the stream to be closed gracefully */
	signal(sig, SIG_IGN);
	lombo_sound_close = 1;
}

void lombo_system_set_volume(int volume)
{
	sys_snd_volume = volume;
}

int lombo_system_get_volume(void)
{
	return sys_snd_volume;
}

int lombo_system_sound_play(const char *file_name)
{
	struct cmd cmd;
	struct ctx ctx;

	cmd_init(&cmd);

	cmd.filename = file_name;

	if ((cmd.filename != NULL)
	&& (cmd.filetype == NULL)) {
		cmd.filetype = strrchr(cmd.filename, '.');
		if (cmd.filetype != NULL)
			cmd.filetype++;
	}

	if (ctx_init(&ctx, &cmd) < 0)
		return EXIT_FAILURE;

	/* TODO get parameters from context */
	ALOGD("lombo sound playing '%s' '%s': %u ch, %u hz, %u bit\n",
		cmd.filename,
		cmd.filetype,
		cmd.channels,
		cmd.rate,
		cmd.bits);

	if (play_sample(&ctx, &cmd) < 0) {
		ctx_free(&ctx);
		return EXIT_FAILURE;
	}

	ctx_free(&ctx);
	return EXIT_SUCCESS;
}

int lombo_system_play_wav(char *wav_data, int size)
{
	struct riff_wave_header wav_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;
	unsigned int alsa_fmt;
	unsigned int chunk_header_size;
	unsigned int data_left;
	unsigned int write_size;
	unsigned int bytes_per_frame;
	void *gain = NULL;
	int init_volume = 0;
	char *tmp = NULL;
	snd_pcm_hw_params_t *hw = NULL;
	snd_pcm_t *pcm = NULL;
	int ret = 0;

	if (wav_data == NULL || size <= 0) {
		ALOGE("error wav data\n");
		return -1;
	}

	/* Reinitialize the playing falg bit */
	lombo_system_sound_init();

	tmp = wav_data;
	data_left = size;
	memcpy(&wav_header, tmp, sizeof(wav_header));
	if ((wav_header.riff_id != ID_RIFF) ||
			(wav_header.wave_id != ID_WAVE)) {
		ALOGE("error: not riff/wave data\n");
		return -1;
	}
	tmp += sizeof(wav_header);
	data_left -= sizeof(wav_header);

	chunk_header_size = sizeof(chunk_header);
	while (1) {
		if (data_left < chunk_header_size) {
			ALOGE("no more data than chunk_header_size left\n");
			return -1;
		}
		memcpy(&chunk_header, tmp, chunk_header_size);
		tmp += chunk_header_size;
		data_left -= chunk_header_size;
		if (chunk_header.id == ID_FMT)
			memcpy(&chunk_fmt, tmp, sizeof(chunk_fmt));
		if (chunk_header.id == ID_DATA)
			break;
		tmp += chunk_header.sz;
		data_left -= chunk_header.sz;
	};
	bytes_per_frame = chunk_fmt.num_channels * chunk_fmt.bits_per_sample / 8;
	ALOGD("get wav info rate %d, fmt %d, channel %d\n",
		chunk_fmt.sample_rate, chunk_fmt.bits_per_sample,
		chunk_fmt.num_channels);

	if (chunk_fmt.bits_per_sample == 8) {
		alsa_fmt = SND_PCM_FORMAT_U8;
	} else if (chunk_fmt.bits_per_sample == 16) {
		alsa_fmt = SND_PCM_FORMAT_S16_LE;
	} else if (chunk_fmt.bits_per_sample == 24) {
		if (chunk_fmt.block_align == 6)
			alsa_fmt = SND_PCM_FORMAT_S24_3LE;
		else
			alsa_fmt = SND_PCM_FORMAT_S24_LE;
	} else if (chunk_fmt.bits_per_sample == 32) {
		alsa_fmt = SND_PCM_FORMAT_S32_LE;
	} else {
		ALOGE("bit count '%u' not supported\n", chunk_fmt.bits_per_sample);
		return -1;
	}

	ret = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (ret < 0) {
		ALOGE("open pcm err\n");
		return -1;
	}
	snd_pcm_hw_params_malloc(&hw);
	snd_pcm_hw_params_any(pcm, hw);
	snd_pcm_hw_params_set_access(pcm,
		hw, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm,
		hw, alsa_fmt);
	snd_pcm_hw_params_set_channels(pcm, hw, chunk_fmt.num_channels);
	snd_pcm_hw_params_set_rate(pcm, hw, chunk_fmt.sample_rate, 0);
	snd_pcm_hw_params_set_period_size(pcm, hw, DEFAULT_PERIOD_SIZE, 0);
	snd_pcm_hw_params_set_buffer_size(pcm, hw, DEFAULT_BUFFER_SIZE);
	if (snd_pcm_hw_params(pcm, hw) < 0) {
		ALOGE("snd_pcm_hw_params error\n");
		snd_pcm_hw_params_free(hw);
		snd_pcm_close(pcm);
		return -1;
	}
	snd_pcm_hw_params_free(hw);

	gain = audio_gain_open(chunk_fmt.num_channels,
			chunk_fmt.bits_per_sample);
	if (gain == NULL) {
		ALOGE("unable to open gain\n");
		return -1;
	}

	audio_gain_set_volume(gain, sys_snd_volume);
	init_volume = sys_snd_volume;

	write_size = bytes_per_frame * DEFAULT_BUFFER_SIZE;
	while ((!lombo_sound_close) && (data_left > write_size)) {
		/* Dynamic volume change */
		if (init_volume != sys_snd_volume) {
			init_volume = sys_snd_volume;
			audio_gain_set_volume(gain, sys_snd_volume);
		}
		audio_gain_apply(gain, tmp, tmp, write_size);
		ret = snd_pcm_writei(pcm, tmp,
			write_size / bytes_per_frame);
		if (ret < 0) {
			ALOGE("error playing sample\n");
			if (ret == -EPIPE) {
				ALOGE("error underrun\n");
				snd_pcm_prepare(pcm);
			} else
				break;
		}
		tmp += write_size;
		data_left -= write_size;
	}

	if ((!lombo_sound_close) && (data_left > 0)) {
		audio_gain_apply(gain, tmp, tmp, data_left);
		ret = snd_pcm_writei(pcm, tmp,
			data_left / bytes_per_frame);
	}

	audio_gain_close(gain);
	snd_pcm_drain(pcm);
	snd_pcm_close(pcm);

	return 0;
}

int lombo_system_sound_close()
{
	lombo_sound_close = 1;

	return 0;
}

void lombo_set_master_volume(int volume)
{
	int ret = 0;
	unsigned int volume_reg;
	snd_mixer_t *handle = NULL;
	snd_mixer_elem_t *elem = NULL;
	snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);

	if (volume >= 100) {
		volume_reg = reg_volumes[MAX_REG_V_CNT - 1];
		master_volume = 100;
	} else if (volume < 0) {
		volume_reg = 0;
		master_volume = 0;
	} else {
		volume_reg = reg_volumes[volume / 4];
		master_volume = volume;
	}
	ALOGI("volume_reg %d", volume_reg);

	ret = snd_mixer_open(&handle, 0);
	if (ret < 0) {
		ALOGE("snd_mixer_open err %d\n", ret);
		return;
	}
	ret = snd_mixer_attach(handle, "default");
	if (ret < 0) {
		ALOGE("snd_mixer_attach err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_selem_register(handle, NULL, NULL);
	if (ret < 0) {
		ALOGE("snd_mixer_selem_register err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_load(handle);
	if (ret < 0) {
		ALOGE("snd_mixer_load err %d\n", ret);
		goto EXIT;
	}
	snd_mixer_selem_id_set_name(sid, "Playback Master");
	snd_mixer_selem_id_set_index(sid, 0);
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		ALOGE("snd_mixer_find_selem err\n");
		goto EXIT;
	}
	ret = snd_mixer_selem_set_playback_volume(elem,
			SND_MIXER_SCHN_FRONT_LEFT, volume_reg);
	if (ret < 0) {
		ALOGE("set left channel volume err %d\n", ret);
		goto EXIT;
	}
EXIT:
	if (handle) {
		snd_mixer_close(handle);
		handle = NULL;
	}
}

int lombo_get_master_volume(void)
{
	return master_volume;
}

void lombo_set_master_record_volume(int volume)
{
	int ret = 0;
	unsigned int volume_reg;
	snd_mixer_t *handle = NULL;
	snd_mixer_elem_t *elem = NULL;
	snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);

	if (volume >= 100) {
		volume_reg = record_volumes[MAX_REG_V_CNT - 1];
		master_record_volume = 100;
	} else if (volume < 0) {
		volume_reg = 0;
		master_record_volume = 0;
	} else {
		volume_reg = record_volumes[volume / 4];
		master_record_volume = volume;
	}
	ALOGI("volume_reg %d", volume_reg);

	ret = snd_mixer_open(&handle, 0);
	if (ret < 0) {
		ALOGE("snd_mixer_open err %d\n", ret);
		return;
	}
	ret = snd_mixer_attach(handle, "default");
	if (ret < 0) {
		ALOGE("snd_mixer_attach err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_selem_register(handle, NULL, NULL);
	if (ret < 0) {
		ALOGE("snd_mixer_selem_register err %d\n", ret);
		goto EXIT;
	}
	ret = snd_mixer_load(handle);
	if (ret < 0) {
		ALOGE("snd_mixer_load err %d\n", ret);
		goto EXIT;
	}
	snd_mixer_selem_id_set_name(sid, "Record Master");
	snd_mixer_selem_id_set_index(sid, 0);
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		ALOGE("snd_mixer_find_selem err\n");
		goto EXIT;
	}
	ret = snd_mixer_selem_set_capture_volume(elem,
			SND_MIXER_SCHN_FRONT_LEFT, volume_reg);
	if (ret < 0) {
		ALOGE("set left channel volume err %d\n", ret);
		goto EXIT;
	}
EXIT:
	if (handle) {
		snd_mixer_close(handle);
		handle = NULL;
	}
}

int lombo_get_master_record_volume(void)
{
	return master_record_volume;
}

