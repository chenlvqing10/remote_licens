/*
 * media_api.h - Standard functionality for lombo media.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __MEDIA_COMMON_H__
#define __MEDIA_COMMON_H__
#include "sub_common.h"

#include <stdint.h>
#include <stddef.h>

typedef enum frame_type {
	VS_INVAILD_FRAME,
	VS_VIDEO_RAW_FRAME,
	VS_VIDEO_ROT_FRAME,
	VS_AUDIO_FRAME,
	VS_IMAGE_FRAME,
	VS_SUBTITLE_FRAME,
	VS_IMAGE_ROT_FRAME,
	VS_OTHER_FRAME
} frame_type_t;

typedef enum stream_type {
	STREAM_TYPE_UNKNOWN = 0,
	STREAM_TYPE_AUDIO,
	STREAM_TYPE_VIDEO,
	STREAM_TYPE_SUBTITLE,
} stream_type_e;

typedef enum muxer_type {
	MUXER_TYPE_UNKNOWN,
	MUXER_TYPE_RAW,
	MUXER_TYPE_TS,
} muxer_type_e;

typedef enum video_codec {
	VIDEO_CODEC_UNKNOWN = 0,
	VIDEO_CODEC_H264,
} video_codec_e;

typedef enum audio_codec {
	AUDIO_CODEC_UNKNOWN = 0,
	AUDIO_CODEC_PCM,
	AUDIO_CODEC_AAC,
	AUDIO_CODEC_G729,
} audio_codec_e;

typedef enum cb_port {
	CB_VCAM_OUT,
	CB_VROT_OUT,
	CB_OTHER_PORT
} cb_port_e;

/**
 * struct video_isp_info - structure for the EXIF information of VISS
 * @valid: 1: isp info vaild 0: isp info invaild
 * @exposure_time: exposure time(s) * 10000
 * @shutter_speed:  exposure time(s) * 10000
 * @aperture: aperture fno2.8 = 280
 * @brightness: LV = [0, 20], 0: star light, 20: sun light
 * @exposure_bias: it calls also EV bias
 * @iso_speed: ISO = gain * 100
 * @flash: status register value of the flash
 * @illu_id: AWB temperature id
 *    0: HOR(2100K)
 *    1: A(2900K)
 *    2: TL84(4000K)
 *    3: CWF(4100K)
 *    4: D50(5000K)
 *    5: D65(6500K)
 *    6: D75(7500K)
 * @back_score: back light score = [0, 100] percent
 * @res: reserved info
 * @res[0]: object luminance
 * @res[1]: back luminance
 * @res[2]: average luminance
 * @res[3]: original luminance
 * @res[4]: final luminance
 */
typedef struct video_isp_info {
	int valid;
	unsigned int exposure_time;
	unsigned int shutter_speed;
	unsigned int aperture;
	unsigned int brightness;
	unsigned int exposure_bias;
	unsigned int iso_speed;
	unsigned int flash;
	unsigned int illu_id;
	unsigned int back_score;
	unsigned int res[16];
} video_isp_info_t;

typedef struct video_frame_info {
	int color_fmt;
	int width;
	int height;
	int stride;
	int sliceHeight;
	int planar;
	unsigned char *addr[3];
	int size[3];
	long long time_stamp;
	video_isp_info_t isp_info;
} video_frame_info_t;

typedef struct audio_frame_info {
	int sample_rate;
	char *addr;
	char size;
	long long time_stamp;
} audio_frame_info_t;

typedef struct sub_style {
	int has_style_flag;			/* style valid flag, 0 or 1 */
	char *name;
	av_sub_font_style_e font_style;
	int fontsize;
	int primarycolour;			/* subtitle preferred color, &HBBGGRR*/
	int secondarycolour;			/* use in Karaoke */
	int outlinecolour;			/* border colour */
	int backcolour;				/* shadow colour */
	font_bold_flag_e bold;			/* 0 close, -1 open */
	font_italic_flag_e italic;		/* 0 close, -1 open */
	font_underline_flag_e underline;	/* 0 close, -1 open */
	font_strikeout_flag_e strikeout;	/* 0 close, -1 open */
	float scalex;		/* horizontal zoom, default 100% */
	float scaley;		/* vertical zoom, default 100% */
	int spacing;		/* word spacing */
	int angle;		/* rotate */
	int borderstyle;	/* border style.*/
	float outline;		/* border width 1-4*/
	float shadow;		/* shadow distance 0-4*/
	av_sub_alignment_e alignment;	/* alignment */
	int marginl;		/* distance from the left,invalid when left aligned
					value:0 ~ playResX*/
	int marginr;		/* distance from the right,invalid when right aligned
					value:0 ~ playResY */
	int marginv;		/* subtitle height, the distance to the bottom when
				bottom aligned, to the top when top aligned, invalid
				when center aligned. value:0 ~ playResY */
	int alphalevel;
	av_sub_encode_type_e encoding;	/* encoding style */
} sub_style_t;

typedef struct sub_event {
	int dialogue_marginl; /* if 0, then use the style marginl */
	int dialogue_marginr; /* if 0, then use the style marginr */
	int dialogue_marginv; /* if 0, then use the style marginv */
	av_sub_effect_type_e sub_effect_type;
	charset_encode_e charset_type;
	int effectstartx;
	int effectendx;
	int effectstarty;
	int effectendy;
	int effectdelaytime;
	av_subtitle_type_e type;
	int datasize;
	char *pdata;
} sub_data_t;

typedef struct sub_common {
	int index;
	int resx; /* screen width*/
	int resy; /* screen height*/
	long long start_disp_time;
	long long end_disp_time;
} sub_common_t;

typedef struct subtitle_frame_info {
	sub_common_t comm_info;
	sub_style_t style;
	sub_data_t data;
} subtitle_frame_info_t;


typedef struct frame {
	stream_type_e type;
	union {
		video_frame_info_t video;
		audio_frame_info_t audio;
		subtitle_frame_info_t subtitle;
	} info;
	void *priv_data;
	void *header;
} frame_t;

typedef struct audio_stream_info {
	audio_codec_e codec;
	int samplerate;
	int channels;
	int bits_per_sample;
	char *text;
} audio_stream_info_t;

typedef struct video_stream_info {
	video_codec_e codec;
	int width;
	int height;
	char *text;
} video_stream_info_t;

typedef struct avstream_info {
	muxer_type_e muxer_type;
	int astream_num;
	int vstream_num;
	audio_stream_info_t astream_info;
	video_stream_info_t vstream_info;
} avstream_info_t;

typedef struct avstream {
	stream_type_e type;
	uint8_t *data;
	int data_size;
	int64_t timestamp;
} avstream_t;

typedef struct app_frame_cb {
	frame_type_t type;
	void *app_data;
	int(*buf_handle)(void *app_data, frame_t *pframe);
} app_frame_cb_t;

typedef struct app_port_cb {
	cb_port_e type;
	void *app_data;
	int(*buf_handle)(void *app_data, frame_t *pframe);
} app_port_filter_t;

typedef struct app_stream_cb {
	void *app_data;
	int (*buf_handle)(void *, avstream_t *);
} app_stream_cb_t;

typedef enum AUDIO_ENCODER {
	AUDIO_ENCODER_INVALID = 0,
	AUDIO_ENCODER_AAC,
	AUDIO_ENCODER_AAC_RAW = AUDIO_ENCODER_AAC,
	AUDIO_ENCODER_AAC_MP4ADTS,
	AUDIO_ENCODER_ADPCM,
	AUDIO_ENCODER_PCM,
	AUDIO_ENCODER_G729,
	AUDIO_ENCODER_PCM_ALAW,
	AUDIO_ENCODER_PCM_MULAW,
	AUDIO_ENCODER_UNSUPPORT,
	AUDIO_ENCODER_NONE = 0x7fff
} audio_encoder_t;

typedef struct win_rect {
	int   x;
	int   y;
	int   width;
	int   height;
} win_rect_t;

typedef struct colli_file_cfg {
	int cache_time; /* time (seconds) save to cache before collision happened */
	int total_time;	/* total time (seconds) of collision file */
	char *filename;	/* saved file */
	app_stream_cb_t cb;
} colli_file_cfg_t;

long media_init(void);
void media_deinit(void);

#endif /* __MEDIA_COMMON_H__ */

