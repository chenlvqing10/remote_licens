/*
* copyright (c) 2018 All Rights Reserved
*
* This file is part of media.
*
* File   : media_probe.c
* Version: V1.0
* Date   : 2019/8/7 10:21:02
* Other  : ffmpeg-3.2.7
*/

#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "media_probe"
#define _FILE_OFFSET_BITS 64

#include <dirent.h>
#include <oscl.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <base_component.h>
#include <av_media_type.h>
#include "media_probe.h"
#include <demuxer_plugin.h>


#define MIN_PROBE_SIZE (2048)
#define MAX_PROBE_SIZE (1 << 20)
/* < maximum score */
#define AVPROBE_SCORE_MAX       100
#define AVPROBE_SCORE_EXTENSION  50 /* < score for file extension */
#define AVPROBE_SCORE_RETRY (AVPROBE_SCORE_MAX/4)

#ifndef AV_RL16
#   define AV_RL16(x)                                \
	(((uint32_t)((const uint8_t *)(x))[1] << 8) | \
	((const uint8_t *)(x))[0])
#endif

#ifndef AV_RB24
#   define AV_RB24(x)                           \
	((((const uint8_t *)(x))[0] << 16) | \
	(((const uint8_t *)(x))[1] << 8) | \
	((const uint8_t *)(x))[2])
#endif

#ifndef AV_RB32
#   define AV_RB32(x)                                \
	(((uint32_t)((const uint8_t *)(x))[0] << 24) | \
	(((const uint8_t *)(x))[1] << 16) | \
	(((const uint8_t *)(x))[2] << 8) | \
	((const uint8_t *)(x))[3])
#endif

#ifndef AV_RL32
#   define AV_RL32(x)                                \
	(((uint32_t)((const uint8_t *)(x))[3] << 24) | \
	(((const uint8_t *)(x))[2] << 16) | \
	(((const uint8_t *)(x))[1] << 8) | \
	((const uint8_t *)(x))[0])
#endif

#ifndef AV_RB64
#   define AV_RB64(x)                                   \
	(((uint64_t)((const uint8_t *)(x))[0] << 56) | \
	((uint64_t)((const uint8_t *)(x))[1] << 48) | \
	((uint64_t)((const uint8_t *)(x))[2] << 40) | \
	((uint64_t)((const uint8_t *)(x))[3] << 32) | \
	((uint64_t)((const uint8_t *)(x))[4] << 24) | \
	((uint64_t)((const uint8_t *)(x))[5] << 16) | \
	((uint64_t)((const uint8_t *)(x))[6] << 8) | \
	(uint64_t)((const uint8_t *)(x))[7])
#endif

#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned int)(d) << 24))
#define FFMIN(a, b) ((a) > (b) ? (b) : (a))
#define FFMAX(a, b) ((a) > (b) ? (a) : (b))
#define FFMAX3(a, b, c) FFMAX(FFMAX(a, b), c)

#ifndef AV_RB16
#define AV_RB16(x) ((((const uint8_t *)(x))[0] << 8) | ((const uint8_t *)(x))[1])
#endif

typedef struct av_probe {
	char *suffix;
	char *name;
	av_muxer_type_e id;
	int (*probe)(uint8_t *data, int size);
} av_probe_t;

/*
********************************************************************
* audio probe
********************************************************************
*/
#define	SYNCWORDH			0xff
#define	SYNCWORDL			0xf0
#define	SYNCWORDL_LATM		0xE0
#define	SYNCWORDL_H			0x56

static int AACFindSyncWord(unsigned char *buf, int nBytes)
{
	int i;

	/* find byte-aligned syncword (12 bits = 0xFFF) */
	for (i = 0; i < nBytes - 1; i++) {
		if ((buf[i + 0] & SYNCWORDH) == SYNCWORDH &&
			(buf[i + 1] & SYNCWORDL) == SYNCWORDL)
			return i;
	}

	return -1;
}

static int AACFindSyncWordLATM(unsigned char *buf, int nBytes)
{
	int i;

	for (i = 0; i < nBytes - 1; i++) {
		if ((buf[i + 0] & SYNCWORDH) == SYNCWORDL_H
			&& (buf[i + 1] & SYNCWORDL_LATM) == SYNCWORDL_LATM)
			return i;
	}

	return -1;
}

static int aac_probe(uint8_t *data, int size)
{
	int ret = 0;

#define IS_ADIF(p) ((p)[0] == 'A' && (p)[1] == 'D' && \
	(p)[2] == 'I' && (p)[3] == 'F')

	if (IS_ADIF(data)) {
		ret = AVPROBE_SCORE_EXTENSION;
	} else {
		int offset = AACFindSyncWord(data, size);
		int offsetlatm = AACFindSyncWordLATM(data, size);

		if ((offset < 0) && (offsetlatm < 0))
			return ret;

		if ((offset < 0) && (offsetlatm >= 0)) {
			data += offsetlatm;
		} else if ((offset > offsetlatm) && (offsetlatm >= 0)) {
			data += offsetlatm;
		} else if (offset >= 0) {
			data += offset;
		}

		if ((data[0] == 0xff) && ((data[1] & 0xF6) == 0xF0)) {
			int layer = (data[1] >> 1) & 0x03;
			int sampRateIdx = (data[2] >> 2) & 0x0f;
			int channelConfig = ((data[2] << 2) & 0x04) |
				((data[3] >> 6) & 0x03);
			/* check validity of header */
			int frameLength = ((int)(data[3] & 0x3) << 11) |
				((int)(data[4] & 0xff) << 3) | ((
						data[5] >> 5) & 0x07);
			if (((layer != 0) && (layer != 3)) ||
				sampRateIdx >= 12 || channelConfig >= 8 ||
				frameLength > 4096) {
				OSCL_LOGE("err aac");
				ret = 0;
			} else
				ret = AVPROBE_SCORE_EXTENSION;
		} else if ((data[0] == 0x56) && ((data[1] & 0xE0) == 0xE0)) {
			ret = AVPROBE_SCORE_EXTENSION;
		}
	}

	return ret;
}

static int amr_probe(uint8_t *data, int size)
{
	/* Only check for "#!AMR" which could be amr-wb, amr-nb.
	 This will also trigger multichannel files: "#!AMR_MC1.0\n" and
	 "#!AMR-WB_MC1.0\n" (not supported) */
	const char AMR_header[]   = "#!AMR\n";

	if (!memcmp(data, AMR_header, 5))
		return AVPROBE_SCORE_MAX;
	else
		return 0;
}

static int ape_probe(uint8_t *data, int size)
{
	if (AV_RL32(data) != MKTAG('M', 'A', 'C', ' '))
		return 0;

	return AVPROBE_SCORE_MAX;
}

int ac3_probe(unsigned char *data, int size)
{
	const uint16_t ff_ac3_frame_size_tab[38][3] = {
		{ 64,   69,   96 },
		{ 64,   70,   96 },
		{ 80,   87,   120 },
		{ 80,   88,   120 },
		{ 96,   104,  144 },
		{ 96,   105,  144 },
		{ 112,  121,  168 },
		{ 112,  122,  168 },
		{ 128,  139,  192 },
		{ 128,  140,  192 },
		{ 160,  174,  240 },
		{ 160,  175,  240 },
		{ 192,  208,  288 },
		{ 192,  209,  288 },
		{ 224,  243,  336 },
		{ 224,  244,  336 },
		{ 256,  278,  384 },
		{ 256,  279,  384 },
		{ 320,  348,  480 },
		{ 320,  349,  480 },
		{ 384,  417,  576 },
		{ 384,  418,  576 },
		{ 448,  487,  672 },
		{ 448,  488,  672 },
		{ 512,  557,  768 },
		{ 512,  558,  768 },
		{ 640,  696,  960 },
		{ 640,  697,  960 },
		{ 768,  835,  1152 },
		{ 768,  836,  1152 },
		{ 896,  975,  1344 },
		{ 896,  976,  1344 },
		{ 1024, 1114, 1536 },
		{ 1024, 1115, 1536 },
		{ 1152, 1253, 1728 },
		{ 1152, 1254, 1728 },
		{ 1280, 1393, 1920 },
		{ 1280, 1394, 1920 },
	};

	int frames = 0;
	uint64_t bits_64 = 0;
	uint16_t sync_word = 0;
	const uint8_t *buf, *buf2, *buf4, *end;
	int found = 0, bitstream_id = 0, sr_code = 0, sr_code2 = 0;
	int frame_size_code = 0, frame_size = 0;
	uint8_t frame_type = 0;
	buf = data;
	end = buf + size;

	for (; buf < end; buf++) {
		if (buf > data
			&& !(buf[0] == 0x0B && buf[1] == 0x77)
			&& !(buf[0] == 0x77 && buf[1] == 0x0B))
			continue;
		buf2 = buf;
		for (frames = 0; buf2 < end; frames++) {
			uint8_t buf3[16];
			int i;
			if (!memcmp(buf2, "\x1\x10\0\0\0\0\0\0", 8))
				buf2 += 16;
			if (buf[0] == 0x77 && buf[1] == 0x0B) {
				for (i = 0; i < 8; i += 2) {
					buf3[i] = buf[i + 1];
					buf3[i + 1] = buf[i];
				}
				buf4 = buf3;
			}

			else
				buf4 = buf2;
			sync_word = (uint16_t)buf4[0] << 8 | (uint16_t)buf4[1];
			if (sync_word != 0x0b77)
				break;
			buf4 += 2;
			bits_64 = (uint64_t)buf4[0] << 56 |
				(uint64_t)buf4[1] << 48 |
				(uint64_t)buf4[2] << 40 |
				(uint64_t)buf4[3] << 32 |
				(uint64_t)buf4[4] << 24 |
				(uint64_t)buf4[5] << 16 |
				(uint64_t)buf4[6] << 8 |
				(uint64_t)buf4[7];

			bitstream_id = (bits_64 >> 35) & 0x1f;
			if (bitstream_id <= 10) {
				sr_code = (bits_64 >> 46) & 0x03;
				if (3 == sr_code)
					break;
				frame_size_code = (bits_64 >> 40) & 0x3f;
				if (frame_size_code > 37)
					break;
				frame_size =
					ff_ac3_frame_size_tab[frame_size_code][sr_code]
					* 2;
			} else if (bitstream_id > 10 && bitstream_id <= 16) {
				frame_type = (bits_64 >> 62) & 0x03;
				if (frame_type == 3)
					break;
				frame_size = (bits_64 >> 48) & 0x7ff;
				if (frame_size < 7)
					break;
				sr_code = (bits_64 >> 46) & 0x03;
				if (sr_code == 3) {
					sr_code2 = (bits_64 >> 44) & 0x03;
					if (sr_code2 == 3)
						break;
				}
			} else
				break;

			if (buf2 + frame_size > end)
				break;
			else {
				found = 1;
				return 26;
			}
			buf2 += 2;
		}
	}

	if (!found) {
		const uint8_t *end = data + size;
		size = 0;
		uint32_t sync_word;

		for (buf = data; buf + 8 <= end; buf++) {
			sync_word = (((uint32_t)((const uint8_t *)(buf + 4))[0] << 24) |
				(uint32_t)(((const uint8_t *)(buf + 4))[1] << 16) |
				(uint32_t)(((const uint8_t *)(buf + 4))[2] << 8) |
				(uint32_t)((const uint8_t *)(buf + 4))[3]);
			if (sync_word == 0xf8726fbb || sync_word == 0xf8726fba)
				return 27;
		}

		return 0;
	}
	return 0;
}

static int dts_probe(uint8_t *data, int size)
{
#define    DCA_SYNCWORD_CORE_BE              0x7FFE8001U
#define    DCA_SYNCWORD_CORE_LE              0xFE7F0180U
#define    DCA_SYNCWORD_CORE_14B_BE          0x1FFFE800U
#define    DCA_SYNCWORD_CORE_14B_LE          0xFF1F00E8U
#define    DCA_SYNCWORD_SUBSTREAM            0x64582025U
#define    DTSHDHDR0                         0x44545348U
#define    DTSHDHDR1                         0x44484452U
#define    DCA_WAVHEAD                       0X52494646U
#define    DCA_WAVDATA                       0X64617461U

	int i = 0;

	if (AV_RB32(data) == DCA_WAVHEAD) {
		data += 8;
		size -= 8;
		while (size > 0) {
			if (AV_RB32(data) == DCA_WAVDATA)
				break;
			data++;
			size--;
		}
		data += 8;
		size -= 8;
	}

	for (i = 0; i < size - 3; i++, data++) {
		/* extension substream (EXSS) */
		if (AV_RB32(data) == DCA_SYNCWORD_SUBSTREAM)
			return AVPROBE_SCORE_MAX / 2 + 1;

		/* regular bitstream */
		if (AV_RB32(data) == DCA_SYNCWORD_CORE_BE &&
			(AV_RB16(data + 4) & 0xFC00) == 0xFC00)
			return AVPROBE_SCORE_MAX / 2 + 1;
		else if (AV_RB32(data) == DCA_SYNCWORD_CORE_LE &&
			(AV_RB16(data + 4) & 0x00FC) == 0x00FC)
			return AVPROBE_SCORE_MAX / 2 + 1;

		/* 14 bits big-endian bitstream */
		else if (AV_RB32(data) == DCA_SYNCWORD_CORE_14B_BE &&
			(AV_RB16(data + 4) & 0xFFF0) == 0x07F0)
			return AVPROBE_SCORE_MAX / 2 + 1;

		/* 14 bits little-endian bitstream */
		else if (AV_RB32(data) == DCA_SYNCWORD_CORE_14B_LE &&
			(AV_RB16(data + 4) & 0xF0FF) == 0xF007)
			return AVPROBE_SCORE_MAX / 2 + 1;
		/* dts-hd */
		else if ((AV_RB32(data) == DTSHDHDR0) && (AV_RB32(data + 4) == DTSHDHDR1))
			return AVPROBE_SCORE_MAX;
	}

	return 0;
}


static int raw_flac_probe(uint8_t *data, int size)
{
	if ((data[2] & 0xF0) == 0)
		return 0;
	if ((data[2] & 0x0F) == 0x0F)
		return 0;
	if ((data[3] & 0xF0) >= (8 + 3) << 4)
		return 0;
	if ((data[3] & 0x06) == 0x06)
		return 0;
	if ((data[3] & 0x01) == 0x01)
		return 0;
	return AVPROBE_SCORE_EXTENSION  + 1;
}

int flac_probe(uint8_t *data, int size)
{
	if ((AV_RB16(data) & 0xFFFE) == 0xFFF8)
		return raw_flac_probe(data, size);
	if (size < 4 || memcmp(data, "fLaC", 4))
		return 0;
	if ((data[4] & 0x7f) != 0
		|| AV_RB24(data + 5) != 34
		|| AV_RB16(data + 8) < 16
		|| AV_RB16(data + 8) > AV_RB16(data + 10)
		|| !(AV_RB24(data + 18) >> 4)
		|| AV_RB24(data + 18) >> 4 > 655350)
		return AVPROBE_SCORE_EXTENSION;
	return AVPROBE_SCORE_MAX;
}

static int g729_probe(uint8_t *data, int size)
{
#define RATE_6400       64      /* Low  rate  (6400 bit/s)       */
#define RATE_8000       80      /* Full rate  (8000 bit/s)       */
#define RATE_11800      118     /* High rate (11800 bit/s)       */
#define RATE_SID        15      /* SID                           */
#define RATE_0          0       /* 0 bit/s rate                  */
#define RATE_SID_OCTET    16     /* number of bits in Octet Transmission mode */
	/* #define CONFIG_FFMPEG */
	int ret = 0;
	int serial_size = 0;

#ifdef CONFIG_FFMPEG
	return 1;
#endif
	if ((data[0] == 0x21) && (data[1] == 0x6b)) {
		serial_size = data[2] | ((int)data[3] << 8);

		if ((serial_size !=  RATE_6400) && (serial_size != RATE_8000)
			&& (serial_size != RATE_11800)) {
#ifndef OCTET_TX_MODE
			if (serial_size != RATE_SID)
#else
			if (serial_size != RATE_SID_OCTET)
#endif
				return 0;
		}
		ret = AVPROBE_SCORE_MAX;
	} else
		return 0;

	return ret;
}

#define MPA_DECODE_HEADER \
	int frame_size; \
	int error_protection; \
	int layer; \
	int sample_rate; \
	int sample_rate_index; /* between 0 and 8 */ \
	int bit_rate; \
	int nb_channels; \
	int mode; \
	int mode_ext; \
	int lsf;

typedef struct MPADecodeHeader {
	MPA_DECODE_HEADER
} MPADecodeHeader;

static const uint16_t avpriv_mpa_freq_tab[3] = { 44100, 48000, 32000 };

static const uint16_t avpriv_mpa_bitrate_tab[2][3][15] = {
	{	{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }
	},
	{	{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256 },
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 },
		{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 }
	}
};

/* fast header check for resync */
static inline int ff_mpa_check_header(uint32_t header)
{
	/* header */
	if ((header & 0xffe00000) != 0xffe00000)
		return -1;
	/* layer check */
	if ((header & (3 << 17)) == 0)
		return -1;
	/* bit rate */
	if ((header & (0xf << 12)) == 0xf << 12)
		return -1;
	/* frequency */
	if ((header & (3 << 10)) == 3 << 10)
		return -1;
	return 0;
}

static int avpriv_mpegaudio_decode_header(MPADecodeHeader *s, uint32_t header)
{
#define MPA_MONO    3

	int sample_rate, frame_size, mpeg25, padding;
	int sample_rate_index, bitrate_index;
	int ret;

	ret = ff_mpa_check_header(header);
	if (ret < 0)
		return ret;

	if (header & (1 << 20)) {
		s->lsf = (header & (1 << 19)) ? 0 : 1;
		mpeg25 = 0;
	} else {
		s->lsf = 1;
		mpeg25 = 1;
	}

	s->layer = 4 - ((header >> 17) & 3);
	/* extract frequency */
	sample_rate_index = (header >> 10) & 3;
	if (sample_rate_index >= FF_ARRAY_ELEMS(avpriv_mpa_freq_tab))
		sample_rate_index = 0;
	sample_rate = avpriv_mpa_freq_tab[sample_rate_index] >> (s->lsf + mpeg25);
	sample_rate_index += 3 * (s->lsf + mpeg25);
	s->sample_rate_index = sample_rate_index;
	s->error_protection = ((header >> 16) & 1) ^ 1;
	s->sample_rate = sample_rate;

	bitrate_index = (header >> 12) & 0xf;
	padding = (header >> 9) & 1;
	/* extension = (header >> 8) & 1; */
	s->mode = (header >> 6) & 3;
	s->mode_ext = (header >> 4) & 3;
	/* copyright = (header >> 3) & 1; */
	/* original = (header >> 2) & 1; */
	/* emphasis = header & 3; */

	if (s->mode == MPA_MONO)
		s->nb_channels = 1;
	else
		s->nb_channels = 2;

	if (bitrate_index != 0) {
		frame_size = avpriv_mpa_bitrate_tab[s->lsf][s->layer - 1][bitrate_index];
		s->bit_rate = frame_size * 1000;
		switch (s->layer) {
		case 1:
			frame_size = (frame_size * 12000) / sample_rate;
			frame_size = (frame_size + padding) * 4;
			break;
		case 2:
			frame_size = (frame_size * 144000) / sample_rate;
			frame_size += padding;
			break;
		default:
		case 3:
			frame_size = (frame_size * 144000) / (sample_rate << s->lsf);
			frame_size += padding;
			break;
		}
		s->frame_size = frame_size;
	} else {
		/* if no frame size computed, signal it */
		return 1;
	}

	return 0;
}

static int mp3_probe(uint8_t *data, int size)
{
	int max_frames, first_frames = 0;
	int whole_used = 0;
	int frames, ret;
	uint32_t header;
	const uint8_t *buf, *buf0, *buf2, *end;

	buf0 = data;
	end = data + size - sizeof(uint32_t);
	while (buf0 < end && !*buf0)
		buf0++;

	max_frames = 0;
	buf = buf0;

	for (; buf < end; buf = buf2 + 1) {
		buf2 = buf;
		for (frames = 0; buf2 < end; frames++) {
			MPADecodeHeader h;

			header = AV_RB32(buf2);
			ret = avpriv_mpegaudio_decode_header(&h, header);
			if (ret != 0)
				break;
			buf2 += h.frame_size;
		}
		max_frames = FFMAX(max_frames, frames);
		if (buf == buf0) {
			first_frames = frames;
			if (buf2 == end + sizeof(uint32_t))
				whole_used = 1;
		}
	}

	/* keep this in sync with ac3 probe, both need to avoid */
	/* issues with MPEG-files! */
	if (first_frames >= 7)
		return AVPROBE_SCORE_EXTENSION + 1;
	else if (max_frames > 200)
		return AVPROBE_SCORE_EXTENSION;
	else if (max_frames >= 4
		&& max_frames >= size / 10000)
		return AVPROBE_SCORE_EXTENSION / 2;
	else if (first_frames > 1 && whole_used)
		return 5;
	else if (max_frames >= 1 && max_frames >= size / 10000)
		return 1;
	else
		return 0;

	return 0;
}

static int ogg_probe(uint8_t *data, int size)
{
	if (!memcmp("OggS", data, 5) && data[5] <= 0x7)
		return AVPROBE_SCORE_MAX;
	return 0;
}

static int wav_probe(uint8_t *data, int size)
{
	/* check file header */
	if (size <= 32)
		return 0;
	if (!memcmp(data + 8, "WAVE", 4)) {
		if (!memcmp(data, "RIFF", 4) || !memcmp(data, "RIFX", 4))
			/* Since the ACT demuxer has a standard WAV header at the top of
			* its own, the returned score is decreased to avoid a probe
			* conflict between ACT and WAV. */
			return 100 - 1;
		else if (!memcmp(data, "RF64", 4) &&
			!memcmp(data + 12, "ds64", 4))
			return 100;
	}
	return 0;
}

/*
********************************************************************
* video probe
********************************************************************
*/
static int mov_probe(uint8_t *data, int size)
{
	int64_t offset;
	uint32_t tag;
	int score = 0;
	int moov_offset = -1;

	/* check file header */
	offset = 0;
	for (;;) {
		/* ignore invalid offset */
		if ((offset + 8) > size)
			break;
		tag = AV_RL32(data + offset + 4);
		switch (tag) {
		/* check for obvious tags */
		case MKTAG('m', 'o', 'o', 'v'):
			moov_offset = offset + 4;
		case MKTAG('m', 'd', 'a', 't'):
		/* detect movs with preview pics like ew.mov and april.mov */
		case MKTAG('p', 'n', 'o', 't'):
		/* Packet Video PVAuthor adds this and a lot of more junk */
		case MKTAG('u', 'd', 't', 'a'):
		case MKTAG('f', 't', 'y', 'p'):
			if (AV_RB32(data + offset) < 8 &&
				(AV_RB32(data + offset) != 1 ||
					offset + 12 > size ||
					AV_RB64(data + offset + 8) == 0)) {
				score = FFMAX(score, AVPROBE_SCORE_EXTENSION);
			} else if (tag == MKTAG('f', 't', 'y', 'p') &&
				(AV_RL32(data + offset + 8) ==
					MKTAG('j', 'p', '2', ' ')
					|| AV_RL32(data + offset + 8) ==
					MKTAG('j', 'p', 'x', ' ')
				)) {
				score = FFMAX(score, 5);
			} else {
				score = AVPROBE_SCORE_MAX;
			}
			offset = FFMAX(4, AV_RB32(data + offset)) + offset;
			break;
		/* those are more common words, so rate then a bit less */
		case MKTAG('e', 'd', 'i', 'w'): /* xdcam files have reverted first tags */
		case MKTAG('w', 'i', 'd', 'e'):
		case MKTAG('f', 'r', 'e', 'e'):
		case MKTAG('j', 'u', 'n', 'k'):
		case MKTAG('p', 'i', 'c', 't'):
			score = FFMAX(score, AVPROBE_SCORE_MAX - 5);
			offset = FFMAX(4, AV_RB32(data + offset)) + offset;
			break;
		case MKTAG(0x82, 0x82, 0x7f, 0x7d):
		case MKTAG('s', 'k', 'i', 'p'):
		case MKTAG('u', 'u', 'i', 'd'):
		case MKTAG('p', 'r', 'f', 'l'):
			/* if we only find those cause probedata
			is too small at least rate them */
			score = FFMAX(score, AVPROBE_SCORE_EXTENSION);
			offset = FFMAX(4, AV_RB32(data + offset)) + offset;
			break;
		default:
			offset = FFMAX(4, AV_RB32(data + offset)) + offset;
		}
	}
	if (score > AVPROBE_SCORE_MAX - 50 && moov_offset != -1) {
		/* moov atom in the header - we should make sure that this is not a
		 * MOV-packed MPEG-PS */
		offset = moov_offset;

		while (offset < (size - 16)) { /* Sufficient space */
			/* We found an actual hdlr atom */
			if (AV_RL32(data + offset) == MKTAG('h', 'd', 'l', 'r') &&
				AV_RL32(data + offset + 8) ==
				MKTAG('m', 'h', 'l', 'r') &&
				AV_RL32(data + offset + 12) ==
				MKTAG('M', 'P', 'E', 'G')) {
				/* printf("Found media data tag MPEG indicating
				this is a MOV-packed MPEG-PS.\n"); */
				/* We found a media handler reference atom describing an
				 * MPEG-PS-in-MOV, return a
				 * low score to force expanding the probe window until
				 * mpegps_probe finds what it needs */
				return 5;
			} else
				/* Keep looking */
				offset += 2;
		}
	}

	return score;
}

static int mkv_probe(uint8_t *data, int datasize)
{
	#define EBML_ID_HEADER	0x1A45DFA3
	const char *const matroska_doctypes[] = { "matroska", "webm" };
	uint64_t total = 0;
	int len_mask = 0x80, size = 1, n = 1, i;

	/* EBML header? */
	if (AV_RB32(data) != EBML_ID_HEADER)
		return 0;

	/* length of header */
	total = data[4];
	while (size <= 8 && !(total & len_mask)) {
		size++;
		len_mask >>= 1;
	}
	if (size > 8)
		return 0;
	total &= (len_mask - 1);
	while (n < size)
		total = (total << 8) | data[4 + n++];

	/* Does the probe data contain the whole header? */
	if (datasize < 4 + size + total)
		return 0;

	/* The header should contain a known document type. For now,
	* we don't parse the whole header but simply check for the
	* availability of that array of characters inside the header.
	* Not fully fool-proof, but good enough. */
	for (i = 0; i < FF_ARRAY_ELEMS(matroska_doctypes); i++) {
		size_t probelen = strlen(matroska_doctypes[i]);
		if (total < probelen)
			continue;
		for (n = 4 + size; n <= 4 + size + total - probelen; n++)
			if (!memcmp(data + n, matroska_doctypes[i], probelen))
				return AVPROBE_SCORE_MAX;
	}

	/* probably valid EBML header but no recognized doctype */
	return AVPROBE_SCORE_EXTENSION;
}


#define TS_FEC_PACKET_SIZE 204
#define TS_DVHS_PACKET_SIZE 192
#define TS_PACKET_SIZE 188
#define TS_MAX_PACKET_SIZE 204
static int analyze(const uint8_t *buf, int size, int packet_size, int probe)
{
	int stat[TS_MAX_PACKET_SIZE];
	int stat_all = 0;
	int i;
	int best_score = 0;

	memset(stat, 0, packet_size * sizeof(*stat));

	for (i = 0; i < size - 3; i++) {
		if (buf[i] == 0x47) {
			int pid = AV_RB16(buf+1) & 0x1FFF;
			int asc = buf[i + 3] & 0x30;
			if (!probe || pid == 0x1FFF || asc) {
				int x = i % packet_size;
				stat[x]++;
				stat_all++;
				if (stat[x] > best_score)
					best_score = stat[x];
			}
		}
	}

	return best_score - FFMAX(stat_all - 10*best_score, 0)/10;
}

static int mpegts_probe(uint8_t *data, int datasize)
{
	const int size = datasize;
	int maxscore = 0;
	int sumscore = 0;
	int i;
	int check_count = size / TS_FEC_PACKET_SIZE;
#define CHECK_COUNT 10
#define CHECK_BLOCK 100

	if (!check_count)
		return 0;

	for (i = 0; i < check_count; i += CHECK_BLOCK) {
		int left = FFMIN(check_count - i, CHECK_BLOCK);
		int score      = analyze(data + TS_PACKET_SIZE * i,
			TS_PACKET_SIZE * left, TS_PACKET_SIZE, 1);
		int dvhs_score = analyze(data + TS_DVHS_PACKET_SIZE * i,
			TS_DVHS_PACKET_SIZE * left, TS_DVHS_PACKET_SIZE, 1);
		int fec_score  = analyze(data + TS_FEC_PACKET_SIZE * i,
			TS_FEC_PACKET_SIZE * left, TS_FEC_PACKET_SIZE , 1);
		score = FFMAX3(score, dvhs_score, fec_score);
		sumscore += score;
		maxscore = FFMAX(maxscore, score);
	}

	sumscore = sumscore * CHECK_COUNT / check_count;
	maxscore = maxscore * CHECK_COUNT / CHECK_BLOCK;

	if (check_count > CHECK_COUNT && sumscore > 6)
		return AVPROBE_SCORE_MAX   + sumscore - CHECK_COUNT;
	else if (check_count >= CHECK_COUNT && sumscore > 6)
		return AVPROBE_SCORE_MAX/2 + sumscore - CHECK_COUNT;
	else if (check_count >= CHECK_COUNT && maxscore > 6)
		return AVPROBE_SCORE_MAX/2 + sumscore - CHECK_COUNT;
	else if (sumscore > 6)
		return 2;
	else
		return 0;
}

static int flv_probe(uint8_t *data, int datasize)
{
	const uint8_t *d = data;
	unsigned offset = AV_RB32(d + 5);

	if (d[0] == 'F' && d[1] == 'L' && d[2] == 'V' && d[3] < 5 && d[5] == 0 &&
		offset + 100 < datasize && offset > 8) {
		return AVPROBE_SCORE_MAX;
	}

	return 0;
}

static int check_pes(const uint8_t *p, const uint8_t *end)
{
	int pes1;
	int pes2 = (p[3] & 0xC0) == 0x80 &&
		(p[4] & 0xC0) != 0x40 &&
		((p[4] & 0xC0) == 0x00 ||
			(p[4] & 0xC0) >> 2 == (p[6] & 0xF0));

	for (p += 3; p < end && *p == 0xFF; p++)
		;
	if ((*p & 0xC0) == 0x40)
		p += 2;

	if ((*p & 0xF0) == 0x20)
		pes1 = p[0] & p[2] & p[4] & 1;
	else if ((*p & 0xF0) == 0x30)
		pes1 = p[0] & p[2] & p[4] & p[5] & p[7] & p[9] & 1;
	else
		pes1 = *p == 0x0F;

	return pes1 || pes2;
}

static int check_pack_header(const uint8_t *buf)
{
	return (buf[1] & 0xC0) == 0x40 || (buf[1] & 0xF0) == 0x20;
}

#define PACK_START_CODE             ((unsigned int)0x000001ba)
#define SYSTEM_HEADER_START_CODE    ((unsigned int)0x000001bb)

#define AUDIO_ID 0xc0
#define VIDEO_ID 0xe0

#define PRIVATE_STREAM_1   0x1bd

static int mpegps_probe(uint8_t *data, int datasize)
{
	uint32_t code = -1;
	int i;
	int sys = 0, pspack = 0, priv1 = 0, vid = 0;
	int audio = 0, invalid = 0, score = 0;
	int endpes = 0;

	for (i = 0; i < datasize; i++) {
		code = (code << 8) + data[i];
		if ((code & 0xffffff00) == 0x100) {
			int len  = data[i + 1] << 8 | data[i + 2];
			int pes  = endpes <= i && check_pes(data + i, data + datasize);
			int pack = check_pack_header(data + i);

			if (code == SYSTEM_HEADER_START_CODE)
				sys++;
			else if (code == PACK_START_CODE && pack)
				pspack++;
			else if ((code & 0xf0) == VIDEO_ID && pes) {
				endpes = i + len;
				vid++;
			}
			 /* skip pes payload to avoid start code emulation for private
			  * and audio streams
			  */
			else if ((code & 0xe0) == AUDIO_ID &&  pes) {
				audio++;
				i += len;
			} else if (code == PRIVATE_STREAM_1  &&  pes) {
				priv1++;
				i += len;
			} else if (code == 0x1fd             &&  pes)
				vid++; /* VC1 */

			else if ((code & 0xf0) == VIDEO_ID && !pes)
				invalid++;
			else if ((code & 0xe0) == AUDIO_ID && !pes)
				invalid++;
			else if (code == PRIVATE_STREAM_1  && !pes)
				invalid++;
		}
	}

	if (vid + audio > invalid + 1) /* invalid VDR files nd short PES streams */
		score = AVPROBE_SCORE_EXTENSION / 2;
	if (sys > invalid && sys * 9 <= pspack * 10)
		return (audio > 12 || vid > 3 || pspack > 2) ? AVPROBE_SCORE_EXTENSION + 2
			: AVPROBE_SCORE_EXTENSION / 2 + 1; /* 1 more than mp3 */
	if (pspack > invalid && (priv1 + vid + audio) * 10 >= pspack * 9)
		return pspack > 2 ? AVPROBE_SCORE_EXTENSION + 2
			: AVPROBE_SCORE_EXTENSION / 2; /* 1 more than .mpg */
	if ((!!vid ^ !!audio) && (audio > 4 || vid > 1) && !sys &&
		!pspack && datasize > 2048 && vid + audio > invalid) /* PES stream */
		return (audio > 12 || vid > 6 + 2 * invalid) ? AVPROBE_SCORE_EXTENSION + 2
			: AVPROBE_SCORE_EXTENSION / 2;

	return score;
}

static const char avi_headers[][8] = {
	{ 'R', 'I', 'F', 'F', 'A', 'V', 'I', ' '  },
	{ 'R', 'I', 'F', 'F', 'A', 'V', 'I', 'X'  },
	{ 'R', 'I', 'F', 'F', 'A', 'V', 'I', 0x19 },
	{ 'O', 'N', '2', ' ', 'O', 'N', '2', 'f'  },
	{ 'R', 'I', 'F', 'F', 'A', 'M', 'V', ' '  },
	{ 0 }
};
static int avi_probe(uint8_t *data, int datasize)
{
	int i;

	/* check file header */
	for (i = 0; avi_headers[i][0]; i++) {
		if (AV_RL32(data) == AV_RL32(avi_headers[i]) &&
			AV_RL32(data + 8) == AV_RL32(avi_headers[i] + 4))
				return AVPROBE_SCORE_MAX;
	}

	return 0;
}

static const uint8_t ff_asf_header[] = {
	0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11,
	0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C
};
static int asf_probe(uint8_t *data, int datasize)
{
	/* check file header */
	if (!memcmp(data, ff_asf_header, sizeof(ff_asf_header)))
		return AVPROBE_SCORE_MAX;
	else
		return 0;
}

static int rm_probe(uint8_t *data, int datasize)
{
	/* check file header */
	if ((data[0] == '.' && data[1] == 'R' &&
	     data[2] == 'M' && data[3] == 'F' &&
	     data[4] == 0 && data[5] == 0) ||
	    (data[0] == '.' && data[1] == 'r' &&
	     data[2] == 'a' && data[3] == 0xfd))
		return AVPROBE_SCORE_MAX;
	else
		return 0;
}

static const  av_probe_t demuxer_uri[] = {
	/* video probe */
	{"mov,mp4,m4a,3gp", "mov", AV_MUXER_TYPE_MOV, mov_probe},

	{"mkv,mk3v,mka,mks", "mkv", AV_MUXER_TYPE_MKV, mkv_probe},

	{"ts,m2ts", "ts", AV_MUXER_TYPE_TS, mpegts_probe},

	{"avi", "avi", AV_MUXER_TYPE_AVI, avi_probe},

	{"asf,wmv,wma", "asf", AV_MUXER_TYPE_ASF, asf_probe},

	{"rm,rmvb,ra", "rm", AV_MUXER_TYPE_RM, rm_probe},

	{"flv", "flv", AV_MUXER_TYPE_FLV, flv_probe},
	{"mpg,vob", "mpg", AV_MUXER_TYPE_MPG, mpegps_probe},
	{"ogm,ogg", "ogg", AV_MUXER_TYPE_OGG, ogg_probe},

	/* audio probe */
	{"aac", "aac", AV_MUXER_TYPE_AAC, aac_probe},
	{"amr", "amr", AV_MUXER_TYPE_AMR, amr_probe},
	{"ape", "ape", AV_MUXER_TYPE_APE, ape_probe},
	{"ac3,mlp", "ac3", AV_MUXER_TYPE_AC3, ac3_probe},
	{"dts", "dts", AV_MUXER_TYPE_DTS, dts_probe},
	{"flac", "flac", AV_MUXER_TYPE_FLAC, flac_probe},
	{"g729", "g729", AV_MUXER_TYPE_G729, g729_probe},
	{"mp1,mp2,mp3", "mp3", AV_MUXER_TYPE_MP3, mp3_probe},
	{"wav", "wav", AV_MUXER_TYPE_WAV, wav_probe},

	{NULL, NULL, AV_MUXER_TYPE_INVALID, NULL}
};

char *media_probe(file_stream_para_t *para)
{
	char *suffix = NULL;
	uint8_t *buf = NULL;
	char *name  = NULL;
	int data_size;
	int id3len = 0;
	int fd = -1;
	int i = 0;
	int found = 0;
	int probe_size = MIN_PROBE_SIZE;

	fd = para->io_intf.open(para->filename, O_RDONLY);
	if (fd == -1) {
		OSCL_LOGE("open file error:%s!", para->filename);
		return NULL;
	}

	buf = oscl_malloc(probe_size);
	if (buf == NULL)
		goto EXIT;

	data_size = para->io_intf.read(fd, buf, probe_size);
	if (data_size  == -1)
		goto EXIT;

	while ((buf[0] == 0x49
		&& buf[1] == 0x44
		&& buf[2] == 0x33)
		&& data_size == probe_size) {
		id3len += (((int)buf[6] & 0x7f) << 21) |
			(((int)buf[7] & 0x7f) << 14) |
			(((int)buf[8] & 0x7f) << 7) |
			((int)buf[9] & 0x7f);
		id3len += 10;
		para->io_intf.lseek(fd, id3len, SEEK_SET);
		OSCL_LOGI("id3len=%d", id3len);
		data_size = para->io_intf.read(fd, buf, probe_size);
	}

	suffix = strrchr(para->filename, '.');
	if (suffix != NULL) {
		int len;
		suffix++;
		len = strlen(suffix);
		i = 0;
		while (demuxer_uri[i].suffix != NULL) {
			char *suf = demuxer_uri[i].suffix;
			while (suf != NULL) {
				if (!strncasecmp(suf, suffix, len) &&
					(suf[len] == ',' || suf[len] == '\0')) {
					found = 1;
					break;
				}
				suf = strchr(suf, ',');
				if (suf != NULL)
					suf++;
			}
			if (found)
				break;
			i++;
		}
	}

	if (found) {
		if (demuxer_uri[i].probe(buf, data_size) > AVPROBE_SCORE_RETRY) {
			name = demuxer_uri[i].name;
			OSCL_LOGI("file:%s, type:%s, index:%d", para->filename, name, i);
			goto EXIT;
		} else
			OSCL_LOGW("file:%s suffix %s probe failed!",
			para->filename, suffix);
	} else
		OSCL_LOGW("suffix %s not support", suffix);

	int max_score = 0, idx = -1;
	while (1) {
		int score = 0;
		int rlen = 0;
		max_score = 0;
		idx = -1;
		for (i = 0; demuxer_uri[i].probe != NULL; i++) {
			score = demuxer_uri[i].probe(buf, data_size);
			if (score > max_score) {
				max_score = score;
				idx = i;
			}
		}
		OSCL_LOGD("probe_size=%d, max_score=%d, index=%d",
			probe_size, max_score, idx);
		if (max_score > AVPROBE_SCORE_RETRY)
			break;

		probe_size = probe_size << 1;
		if (probe_size > MAX_PROBE_SIZE)
			break;
		buf = oscl_realloc(buf, probe_size);
		if (buf == NULL) {
			OSCL_LOGE("realloc buf error!");
			break;
		}
		OSCL_LOGD("probe_size=%d, data_size=%d, readsize=%d",
			probe_size, data_size, probe_size - data_size);
		rlen = para->io_intf.read(fd, buf + data_size, probe_size - data_size);
		if (rlen <= 0) {
			OSCL_LOGE("read error or reach eof(ret=%d)!", rlen);
			break;
		}
		data_size += rlen;
	}

	if (max_score > 0 && idx >= 0) {
		name = demuxer_uri[idx].name;
		OSCL_LOGI("file:%s, max_score:%d, index:%d",
			para->filename, max_score, idx);
		if (max_score <= AVPROBE_SCORE_RETRY) {
			OSCL_LOGW("file %s format %s detected only with low score of %d",
				para->filename, name, max_score);
			OSCL_LOGW("misdetection possible!");
		}
	}

	if (name == NULL)
		OSCL_LOGW("file:%s not supported yet!", para->filename);

EXIT:
	if (buf != NULL)
		oscl_free(buf);
	if (fd >= 0)
		para->io_intf.close(fd);
	return name;
}


#if 1

typedef struct sub_probe_tag {
	OMX_STRING suffix;
	OMX_STRING name;
	av_muxer_type_e codec_id;
} sub_probe_t;

static sub_probe_t sub_url[] = {
	{"ssa,ass", "ass", AV_CODEC_TYPE_ASS},
	{"srt", "srt", AV_CODEC_TYPE_SRT},
	{"stl", "stl", AV_CODEC_TYPE_STL},
	{"lrc", "lrc", AV_CODEC_TYPE_LRC},
	{"txt", "txt", AV_CODEC_TYPE_TXT},
	{"tts", "tts", AV_CODEC_TYPE_TTS},
	{"sub", "sub", AV_CODEC_TYPE_SUB},
	{"smi", "smi", AV_CODEC_TYPE_SMI},
	{"vsf", "vsf", AV_CODEC_TYPE_VSF},
	{"jss", "jss", AV_CODEC_TYPE_JSS},
	{"asc", "asc", AV_CODEC_TYPE_ASC},
	{"ovr", "ovr", AV_CODEC_TYPE_OVR}
};

static OMX_S32 _split_whole_name(const OMX_S8 *whole_name, OMX_S8 *fname, OMX_S8 *ext)
{
	OMX_S8 *p_ext = NULL;

	oscl_param_check(whole_name, -1, NULL);

	p_ext = (OMX_S8 *)rindex((const char *)whole_name, '.');
	if (NULL != p_ext) {

		if (ext)
			strcpy((char *)ext, (char *)(p_ext + 1));
		if (fname) {
			snprintf((char *)fname, p_ext - whole_name + 1, "%s",
				(char *)whole_name);
		}
	} else {
		if (fname)
			strcpy((char *)fname, (char *)whole_name);
	}
	return 0;
}

static OMX_S32 __splitpath(const OMX_S8 *path, OMX_S8 *drive,
	OMX_S8 *dir, OMX_S8 *fname, OMX_S8 *ext)
{
	OMX_S8 *p_whole_name = NULL;

	oscl_param_check(path, -1, NULL);


	if ('/' == path[strlen((char *)path)]) {
		if (dir)
			strcpy((char *)dir, (char *)path);
		return 0;
	}

	p_whole_name = (int8_t *)rindex((const char *)path, '/');
	if (NULL != p_whole_name) {
		p_whole_name++;
		_split_whole_name(p_whole_name, fname, ext);
		if (dir)
			snprintf((char *)dir, p_whole_name - path, "%s", (char *)path);
	} else {
		_split_whole_name(path, fname, ext);
		dir[0] = '\0';
	}

	return 0;
}

OMX_STRING sub_probe(OMX_STRING media_name, OMX_S32 *codec_id)
{
	OMX_S8 fileext[MAX_FNAME] = {0};
	OMX_S32 total_num = sizeof(sub_url)/sizeof(sub_probe_t);
	OMX_S32 ret = -1, len = 0, found = 0;
	OMX_STRING suf = NULL;
	OMX_S32 i;

	OSCL_LOGI("here: %s", media_name);
	if (NULL ==  media_name) {
		OSCL_LOGE("para is NULL: %p", media_name);
		return NULL;
	}

	ret = __splitpath((const OMX_S8 *)media_name, NULL, NULL, NULL, fileext);
	if (ret) {
		OSCL_LOGE("spllit suffix failed from media name");
		return NULL;
	}
	len = strlen((char *)fileext);
	for (i = 0; i < total_num; i++) {
		suf = sub_url[i].suffix;
		while (suf != NULL) {
			if (!strncasecmp((char *)suf, (char *)fileext, len) &&
				(suf[len] == ',' || suf[len] == '\0')) {
				found = 1;
				break;
			}
			suf = strchr(suf, ',');
			if (suf != NULL)
				suf++;
			else
				break;
		}
		if (found)
			break;
	}


	if (i >= total_num) {
		OSCL_LOGE("the external subtitle is not supported");
		return NULL;
	}
	if (codec_id)
		*codec_id = sub_url[i].codec_id;

	return (OMX_STRING)sub_url[i].name;
}

#endif

