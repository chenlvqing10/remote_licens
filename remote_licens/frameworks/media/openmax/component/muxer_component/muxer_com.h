/*
* copyright (c) 2018 All Rights Reserved
*
* This file is part of audio.
*
* File   : muxer_com.h
* Version: V1.0
* Date   : 2018/11/15 17:08:37
*/

#ifndef MUXER_COM_H
#define MUXER_COM_H

#include "audio_common.h"
#include "OMX_Types.h"
#include "oscl.h"

#define VIDEO_INDEX 0
#define AUDIO_INDEX 1
#define MUXER_STREAM_NUM 2
#define AV_INPUT_BUFFER_PADDING_SIZE 32
#define VIDEOTIMEFRAME 512

#define AV_PKT_FLAG_KEY     0x0001 /* < The packet contains a keyframe */

typedef enum muxer_cmd_index {
	MUXER_CMD_UNKNOWN = 0,
	MUXER_CMD_SET_STREAM_CB,
	MUXER_CMD_SET_PPS_INKEYPKT,
} muxer_cmd_index_e;

typedef struct format_muxer_st {
	AV_CodecParameters para[MUXER_STREAM_NUM];
	void *priv;
	int mode;
	int nb_streams;
	int(*init)(struct format_muxer_st *muxer);
	int(*write_header)(struct format_muxer_st *muxer);
	int(*write_packet)(struct format_muxer_st *muxer, packet_t *pin);
	int(*write_trailer)(struct format_muxer_st *muxer);
	int(*deinit)(struct format_muxer_st *muxer);
	int(*check_bitstream)(struct format_muxer_st *muxer, packet_t *pin);
	int(*ctrl)(struct format_muxer_st *muxer, int cmd, void *param);
	AVIOFILE fp;
} format_muxer;

int emp4_deinit(format_muxer *muxer);
int emp4_init(format_muxer *muxer);
int emp4_write_header(format_muxer *muxer);
int emp4_write_packet(format_muxer *muxer, packet_t *pin);
int emp4_write_trailer(format_muxer *muxer);
int emp4_check_bitstream(format_muxer *muxer, packet_t *pin);

int raw_deinit(format_muxer *muxer);
int raw_init(format_muxer *muxer);
int raw_write_header(format_muxer *muxer);
int raw_write_packet(format_muxer *muxer, packet_t *pin);
int raw_write_trailer(format_muxer *muxer);
int raw_check_bitstream(format_muxer *muxer, packet_t *pin);
int raw_ctrl(format_muxer *muxer, int cmd, void *param);

int ts_deinit(format_muxer *muxer);
int ts_init(format_muxer *muxer);
int ts_write_head(format_muxer *muxer);
int ts_write(format_muxer *muxer, packet_t *pin);
int ts_write_end(format_muxer *muxer);
int ts_check_bitstream(format_muxer *muxer, packet_t *pin);
int ts_ctrl(format_muxer *muxer, int cmd, void *param);

#endif /* MUXER_COM_H */
