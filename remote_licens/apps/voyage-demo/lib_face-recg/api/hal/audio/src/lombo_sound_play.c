/*
* lombo_sound_play.c
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

#include <pcm.h>
#include <lombo_sound_play.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

struct cmd {
	const char *filename;
	const char *filetype;
	unsigned int card;
	unsigned int device;
	int flags;
	struct pcm_config config;
	unsigned int bits;
};

void cmd_init(struct cmd *cmd)
{
	cmd->filename = NULL;
	cmd->filetype = NULL;
	cmd->card = 0;
	cmd->device = 0;
	cmd->flags = PCM_OUT;
	cmd->config.period_size = 1024;
	cmd->config.period_count = 2;
	cmd->config.channels = 2;
	cmd->config.rate = 48000;
	cmd->config.format = PCM_FORMAT_S16_LE;
	cmd->config.silence_threshold = 1024 * 2;
	cmd->config.stop_threshold = 1024 * 2;
	cmd->config.start_threshold = 1024;
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
	struct pcm *pcm;

	struct riff_wave_header wave_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;

	FILE *file;
};

int ctx_init(struct ctx *ctx, struct cmd *cmd)
{
	unsigned int bits = cmd->bits;
	struct pcm_config *config = &cmd->config;

	if (cmd->filename == NULL) {
		fprintf(stderr, "filename not specified\n");
		return -1;
	}

	ctx->file = fopen(cmd->filename, "rb");
	if (ctx->file == NULL) {
		fprintf(stderr, "failed to open '%s'\n", cmd->filename);
		return -1;
	}

	if ((cmd->filetype != NULL) && (strcmp(cmd->filetype, "wav") == 0)) {
		if (fread(&ctx->wave_header, sizeof(ctx->wave_header), 1,
			ctx->file) != 1){
			fprintf(stderr,
				"error: '%s' does not contain a riff/wave header\n",
				cmd->filename);
			fclose(ctx->file);
			return -1;
		}
		if ((ctx->wave_header.riff_id != ID_RIFF) ||
			(ctx->wave_header.wave_id != ID_WAVE)) {
			fprintf(stderr, "error: '%s' is not a riff/wave file\n",
				cmd->filename);
			fclose(ctx->file);
			return -1;
		}
		unsigned int more_chunks = 1;
		do {
			if (fread(&ctx->chunk_header, sizeof(ctx->chunk_header), 1,
				ctx->file) != 1) {
				fprintf(stderr,
					"error: '%s' does not contain a data chunk\n",
					cmd->filename);
				fclose(ctx->file);
				return -1;
			}
			switch (ctx->chunk_header.id) {
			case ID_FMT:
				if (fread(&ctx->chunk_fmt, sizeof(ctx->chunk_fmt), 1,
				ctx->file) != 1){
					fprintf(stderr,
					"error: '%s' has incomplete format chunk\n",
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
				break;
			default:
				/* Unknown chunk, skip bytes */
				fseek(ctx->file, ctx->chunk_header.sz, SEEK_CUR);
			}
		} while (more_chunks);
		config->channels = ctx->chunk_fmt.num_channels;
		config->rate = ctx->chunk_fmt.sample_rate;
		bits = ctx->chunk_fmt.bits_per_sample;
	}

	if (bits == 8) {
		config->format = PCM_FORMAT_S8;
	} else if (bits == 16) {
		config->format = PCM_FORMAT_S16_LE;
	} else if (bits == 24) {
		config->format = PCM_FORMAT_S24_3LE;
	} else if (bits == 32) {
		config->format = PCM_FORMAT_S32_LE;
	} else {
		fprintf(stderr, "bit count '%u' not supported\n", bits);
		fclose(ctx->file);
		return -1;
	}

	ctx->pcm = pcm_open(cmd->card,
			cmd->device,
			cmd->flags,
			config);
	if (ctx->pcm == NULL) {
		fprintf(stderr, "failed to allocate memory for pcm\n");
		fclose(ctx->file);
		return -1;
	} else if (!pcm_is_ready(ctx->pcm)) {
		fprintf(stderr, "failed to open for pcm %u,%u\n", cmd->card, cmd->device);
		fclose(ctx->file);
		pcm_close(ctx->pcm);
		return -1;
	}

	return 0;
}

void ctx_free(struct ctx *ctx)
{
	if (ctx == NULL)
		return;

	if (ctx->pcm != NULL)
		pcm_close(ctx->pcm);

	if (ctx->file != NULL)
		fclose(ctx->file);
}

static int close;

int play_sample(struct ctx *ctx);

void stream_close(int sig)
{
	/* allow the stream to be closed gracefully */
	signal(sig, SIG_IGN);
	close = 1;
}

int lombo_sound_play(const char *file_name)
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
	printf("lombo sound playing '%s' '%s': %u ch, %u hz, %u bit\n",
		cmd.filename,
			cmd.filetype,
		cmd.config.channels,
		cmd.config.rate,
		cmd.bits);

	if (play_sample(&ctx) < 0) {
		ctx_free(&ctx);
		return EXIT_FAILURE;
	}

	ctx_free(&ctx);
	return EXIT_SUCCESS;
}

int play_sample(struct ctx *ctx)
{
	char *buffer;
	int size;
	int num_read;

	size = pcm_frames_to_bytes(ctx->pcm, pcm_get_buffer_size(ctx->pcm));
	buffer = malloc(size);
	if (!buffer) {
		fprintf(stderr, "unable to allocate %d bytes\n", size);
		return -1;
	}

	/* catch ctrl-c to shutdown cleanly */
	/* signal(SIGINT, stream_close); */

	do {
		num_read = fread(buffer, 1, size, ctx->file);
		if (num_read > 0) {
			if (pcm_writei(ctx->pcm, buffer,
				pcm_bytes_to_frames(ctx->pcm, num_read)) < 0) {
				fprintf(stderr, "error playing sample\n");
				break;
			}
		}
	} while (!close && num_read > 0);

	free(buffer);
	return 0;
}

