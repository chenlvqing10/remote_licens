#define LOG_TAG                "audio_gain"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <log/log.h>

#include "audio_gain.h"

static const unsigned int preset_dB_value[PRESET_RESOLUTION] = {
	0x001027, 0x0010b9, 0x001151, 0x0011ef,
	0x001292, 0x00133b, 0x0013e9, 0x00149e,
	0x00155a, 0x00161c, 0x0016e5, 0x0017b5,
	0x00188c, 0x00196c, 0x001a53, 0x001b42,
	0x001c3a, 0x001d3a, 0x001e44, 0x001f57,
	0x002074, 0x00219b, 0x0022cc, 0x002408,
	0x002550, 0x0026a3, 0x002802, 0x00296e,
	0x002ae7, 0x002c6c, 0x002e00, 0x002fa2,
	0x003153, 0x003314, 0x0034e4, 0x0036c5,
	0x0038b6, 0x003aba, 0x003cd0, 0x003ef8,
	0x004135, 0x004385, 0x0045eb, 0x004867,
	0x004af9, 0x004da2, 0x005064, 0x00533e,
	0x005633, 0x005942, 0x005c6e, 0x005fb6,
	0x00631c, 0x0066a1, 0x006a45, 0x006e0b,
	0x0071f3, 0x0075ff, 0x007a30, 0x007e86,
	0x008304, 0x0087ab, 0x008c7c, 0x009179,
	0x0096a3, 0x009bfc, 0x00a186, 0x00a742,
	0x00ad32, 0x00b358, 0x00b9b6, 0x00c04e,
	0x00c722, 0x00ce34, 0x00d586, 0x00dd1b,
	0x00e4f5, 0x00ed16, 0x00f580, 0x00ffff,
	0x01073e, 0x011097, 0x011a45, 0x01244a,
	0x012eab, 0x01396a, 0x01448a, 0x015010,
	0x015bfe, 0x016859, 0x017524, 0x018264,
	0x01901c, 0x019e50, 0x01ad06, 0x01bc41,
	0x01cc07, 0x01dc5c, 0x01ed46, 0x01fec9,
};

typedef struct {
	unsigned int cchannels;
	unsigned int cur_vol;
	unsigned int max_val;     /* max index */
	unsigned int zero_dB_val; /* index at 0 dB */
	double min_dB;
	double max_dB;
	unsigned int *dB_value;
	unsigned int bit_per_sample;
	unsigned int step;
} audio_gain_t;

/* (32bit x 16bit) >> 16 */
typedef union {
	int i;
	short s[2];
} val_t;
static inline int MULTI_DIV_32x16(int a, unsigned short b)
{
	val_t v, x, y;
	v.i = a;
	y.i = 0;
	x.i = (unsigned short)v.s[0];
	x.i *= b;
	y.s[0] = x.s[1];
	y.i += (int)v.s[1] * b;
	return y.i;
}

static inline int MULTI_DIV_int(int a, unsigned int b)
{
	unsigned int gain = (b >> VOL_SCALE_SHIFT);
	int fraction;
	fraction = MULTI_DIV_32x16(a, b & VOL_SCALE_MASK);
	if (gain) {
		long long amp = (long long)a * gain + fraction;
		if (amp > (int)0x7fffffff)
			amp = (int)0x7fffffff;
		else if (amp < (int)0x80000000)
			amp = (int)0x80000000;
		return (int)amp;
	}
	return fraction;
}

/* always little endian */
static inline int MULTI_DIV_24(int a, unsigned int b)
{
	unsigned int gain = b >> VOL_SCALE_SHIFT;
	int fraction;
	fraction = MULTI_DIV_32x16(a, b & VOL_SCALE_MASK);
	if (gain) {
		long long amp = (long long)a * gain + fraction;
		if (amp > (int)0x7fffff)
			amp = (int)0x7fffff;
		else if (amp < (int)0x800000)
			amp = (int)0x800000;
		return (int)amp;
	}
	return fraction;
}

static inline short MULTI_DIV_short(short a, unsigned int b)
{
	unsigned int gain = b >> VOL_SCALE_SHIFT;
	int fraction;
	fraction = (int)(a * (b & VOL_SCALE_MASK)) >> VOL_SCALE_SHIFT;
	if (gain) {
		int amp = a * gain + fraction;
		if (abs(amp) > 0x7fff)
			amp = (a < 0) ? (short)0x8000 : (short)0x7fff;
		return (short)amp;
	}
	return (short)fraction;
}

#define CONVERT_AREA(TYPE) do {	\
	unsigned int ch, fr; \
	TYPE *src, *dst; \
	for (ch = 0; ch < audio_gain->cchannels; ch++) { \
		src = src_addr + ch * sizeof(TYPE); \
		dst = dst_addr + ch * sizeof(TYPE); \
		src_step = audio_gain->step / sizeof(TYPE); \
		dst_step = audio_gain->step / sizeof(TYPE); \
		fr = frames; \
		if (!vol_scale) { \
			while (fr--) { \
				*dst = 0; \
				dst += dst_step; \
			} \
		} else if (vol_scale == 0xffff) { \
			while (fr--) { \
				*dst = *src; \
				src += src_step; \
				dst += dst_step; \
			} \
		} else { \
			while (fr--) { \
				*dst = (TYPE) MULTI_DIV_##TYPE(*src, vol_scale); \
				src += src_step; \
				dst += dst_step; \
			} \
		} \
	} \
} while (0)

/* static int generate_db_table(audio_gain_t *gain){
	int i;

	gain->dB_value = calloc(PRESET_RESOLUTION, sizeof(unsigned int));
	if (! gain->dB_value) {
		ALOGE("cannot allocate dB table");
		return -1;
	}
	for (i = 0; i <= gain->max_val; i++) {
		double db = gain->min_dB +
			(i * (gain->max_dB - gain->min_dB)) /
				gain->max_val;
		double v = (pow(10.0, db / 20.0) *
				(double)(1 << VOL_SCALE_SHIFT));
		gain->dB_value[i] = (unsigned int)v;
	}
	if (gain->zero_dB_val)
		gain->dB_value[gain->zero_dB_val] = 65535;
	for (i = 0; i <= gain->max_val; i+= 4) {
		ALOGW("0x%06x, 0x%06x, 0x%06x, 0x%06x,\n", gain->dB_value[i],
			gain->dB_value[i + 1],
			gain->dB_value[i + 2], gain->dB_value[i + 3]);
	}

	return 0;
} */

void *audio_gain_open(unsigned int channel, unsigned int bit_per_sample)
{
	audio_gain_t *gain = NULL;

	if (channel != 1 && channel != 2
		&& bit_per_sample != 16
		&& bit_per_sample != 32) {
		ALOGE("error args chan %d, fmt %d\n", channel, bit_per_sample);
		return NULL;
	}
	gain = malloc(sizeof(audio_gain_t));
	if (gain == NULL) {
		ALOGE("malloc gain err\n");
		return NULL;
	}
	memset(gain, 0, sizeof(audio_gain_t));
	gain->bit_per_sample = bit_per_sample;
	gain->cchannels = channel;
	gain->max_dB = MAX_DB_UPPER_LIMIT;
	gain->min_dB = PRESET_MIN_DB;
	gain->max_val = PRESET_RESOLUTION - 1;
	gain->zero_dB_val = (gain->min_dB /
	    (gain->min_dB - gain->max_dB)) * gain->max_val;
	gain->cur_vol = gain->zero_dB_val;
	gain->step = gain->bit_per_sample * channel / 8;
	gain->dB_value = (unsigned int *)preset_dB_value;

	return gain;
}

void audio_gain_set_volume(void *gain, unsigned int volume)
{
	audio_gain_t *audio_gain = (audio_gain_t *)gain;

	if (gain == NULL) {
		ALOGE("gain null");
		return;
	}
	if (volume > audio_gain->max_val)
		audio_gain->cur_vol = audio_gain->max_val;
	else if (volume < 0)
		audio_gain->cur_vol = 0;
	else
		audio_gain->cur_vol = volume;
}

void audio_gain_close(void *gain)
{
	if (gain)
		free(gain);
}

int audio_gain_apply(void *gain, void *src_addr, void *dst_addr, int size)
{
	unsigned int src_step, dst_step;
	unsigned int vol_scale;
	unsigned int frames;
	audio_gain_t *audio_gain = (audio_gain_t *)gain;

	if (gain == NULL) {
		ALOGE("gain null");
		return 0;
	}

	if (audio_gain->cur_vol == 0) {
		memset(dst_addr, 0, size);
		return 0;
	}
	if (audio_gain->zero_dB_val == audio_gain->cur_vol) {
		if (src_addr == dst_addr)
			return 0;
		else {
			memcpy(dst_addr, src_addr, size);
			return 0;
		}
	}

	frames = size / audio_gain->step;
	vol_scale = audio_gain->dB_value[audio_gain->cur_vol];
	if (audio_gain->bit_per_sample == 16)
		CONVERT_AREA(short);
	else if (audio_gain->bit_per_sample == 32)
		CONVERT_AREA(int);
	else
		ALOGD("soft volume not support fmt %d", audio_gain->bit_per_sample);

	return 0;
}

