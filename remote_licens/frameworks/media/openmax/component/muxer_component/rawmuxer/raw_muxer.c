/*
* copyright (c) 2019 All Rights Reserved
*
* This file is part of raw muxer.
*
* File   : raw_muxer.h
* Version: V1.0
* Date   : 2019/08/20 11:04:53
* Author : gusm
*/
#define DBG_LEVEL		DBG_INFO
#define LOG_TAG			"raw_muxer"

#include <oscl.h>
#include "muxer_com.h"
#include "muxer_component.h"

typedef struct raw_muxer_context {
	external_stream_writer_t ext_writer;
	int pps_inkeypkt;
	unsigned char *tmp_key_pkt;
	int buflen;
} raw_muxer_context_t;

int raw_init(format_muxer *muxer)
{
	raw_muxer_context_t *ctx = oscl_malloc(sizeof(raw_muxer_context_t));
	if (ctx == NULL)
		return -1;

	memset(ctx, 0, sizeof(raw_muxer_context_t));
	ctx->pps_inkeypkt = 1;
	muxer->priv = (void *)ctx;
	return 0;
}

int raw_deinit(format_muxer *muxer)
{
	raw_muxer_context_t *ctx = (raw_muxer_context_t *)(muxer->priv);

	if (ctx != NULL) {
		if (ctx->tmp_key_pkt) {
			oscl_free(ctx->tmp_key_pkt);
			ctx->tmp_key_pkt = NULL;
			ctx->buflen = 0;
		}
		oscl_free(ctx);
		muxer->priv = NULL;
	}

	return 0;
}

int raw_write_header(format_muxer *muxer)
{
	return 0;
}

static int _write_pps_keypkt(format_muxer *muxer, packet_t *pin)
{
	raw_muxer_context_t *ctx = (raw_muxer_context_t *)(muxer->priv);
	int size_extr;
	int ret = -1;

	size_extr = muxer->para[VIDEO_INDEX].extradata_size;
	if (ctx->buflen < pin->size + size_extr) {
		if (ctx->tmp_key_pkt) {
			oscl_free(ctx->tmp_key_pkt);
			ctx->tmp_key_pkt = NULL;
			ctx->buflen = 0;
		}
	}
	if (ctx->tmp_key_pkt == NULL) {
		ctx->buflen = pin->size + size_extr + 10 * 1024;
		ctx->tmp_key_pkt = oscl_malloc(ctx->buflen);
	}

	if (ctx->tmp_key_pkt) {
		memcpy(ctx->tmp_key_pkt, muxer->para[VIDEO_INDEX].extradata, size_extr);
		memcpy(ctx->tmp_key_pkt + size_extr, pin->data, pin->size);
		ret = ctx->ext_writer.write(ctx->ext_writer.app_data,
			AV_MEDIA_TYPE_VIDEO,
			ctx->tmp_key_pkt,
			size_extr + pin->size,
			pin->timestamp);
	}
	return ret;
}

int raw_write_packet(format_muxer *muxer, packet_t *pin)
{
	raw_muxer_context_t *ctx = (raw_muxer_context_t *)(muxer->priv);
	int type;

	if (ctx->ext_writer.write == NULL) {
		OSCL_LOGW("external write function not set yet");
		return -1;
	}
	if (pin->stream_index == VIDEO_INDEX) {
		type = AV_MEDIA_TYPE_VIDEO;
		if ((pin->flags & AV_PKT_FLAG_KEY) &&
			(muxer->para[VIDEO_INDEX].extradata)) {
			if (ctx->pps_inkeypkt != 1) {
				ctx->ext_writer.write(ctx->ext_writer.app_data,
					type,
					muxer->para[VIDEO_INDEX].extradata,
					muxer->para[VIDEO_INDEX].extradata_size,
					pin->timestamp);
			} else
				return _write_pps_keypkt(muxer, pin);
		}
	} else {
		type = AV_MEDIA_TYPE_AUDIO;
	}

	return ctx->ext_writer.write(ctx->ext_writer.app_data, type, pin->data,
			pin->size, pin->timestamp);
}

int raw_write_trailer(format_muxer *muxer)
{
	return 0;
}

int raw_check_bitstream(format_muxer *muxer, packet_t *pin)
{
	return 1;
}

int raw_ctrl(format_muxer *muxer, int cmd, void *param)
{
	raw_muxer_context_t *ctx = (raw_muxer_context_t *)(muxer->priv);
	int ret = 0;

	if (ctx == NULL)
		return -1;

	switch (cmd) {
	case MUXER_CMD_SET_STREAM_CB:
		ctx->ext_writer = *((external_stream_writer_t *)param);
		break;
	case MUXER_CMD_SET_PPS_INKEYPKT:
		ctx->pps_inkeypkt = *(int *)param;
		break;
	default:
		OSCL_LOGW("unknown cmd type %d", cmd);
		break;
	}
	return ret;
}

