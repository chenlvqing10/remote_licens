/*
 * copyright (c) 2018 All Rights Reserved
 *
 * This file is part of audio.
 *
 * File   : raw_stream.h
 * Version: V1.0
 * Date   : 2018/8/21 17:16:50
 */

#ifndef	RAW_STREAM_H
#define RAW_STREAM_H

#include "OMX_Core.h"
#include "audio_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define RAW_BUFFER_SIZE (128 * 1024)
#define PCM_FRAME_SIZE (4 * 1024)

/* not support buf encode with drv same one */
typedef struct raw_buf_manager {
	int len;
	int offset;
	unsigned char raw_buf[RAW_BUFFER_SIZE];
} raw_buf_manager;

int getRawData(raw_buf_manager *raw_manager,
	uint8_t *data, size_t size,
	packet_t *packet);
int updataRawData(raw_buf_manager *raw_manager,
	uint8_t *data, size_t size,
	packet_t *packet, int decres);
int resetRawBuf(raw_buf_manager *raw_manager);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /* RAW_STREAM_H */
