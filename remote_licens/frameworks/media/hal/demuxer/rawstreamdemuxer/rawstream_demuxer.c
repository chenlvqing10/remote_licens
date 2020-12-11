#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "rawstream_demuxer"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "oscl.h"
#include "lombo_cycle_buf.h"
#include "iostream.h"
#include "demuxer_plugin.h"


#define RAWSTREAM_DEMUXER_VERSION    "1.0"
#define STREAM_MAX 16

typedef struct RawStreamDemuxer {
	AVIOContext *pIOCtx;
	dmx_mediainfo_t mediaInfo;
	void *vbuf;
	void *abuf;

	dmx_packet_t pre_read_pkt;

	int vrst_flag;
	int arst_flag;
} RawStreamDemuxer_t;

#define H264_NAL_FLAG_SPS      (1 << 0)
#define H264_NAL_FLAG_PPS      (1 << 1)
#define H264_NAL_FLAG_KEY      (1 << 2)
#define H264_NAL_FLAG_OTHER    (1 << 3)

static int _check_h264_nal_all(uint8_t *data, int size)
{
	const uint8_t *p = data;
	const uint8_t *end = data + size;
	int flag = 0;

	while (p < end - 4) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
			if ((p[4] & 0x1f) == 0x07)
				flag |= H264_NAL_FLAG_SPS;
			else if ((p[4] & 0x1f) == 0x08)
				flag |= H264_NAL_FLAG_PPS;
			else if ((p[4] & 0x1f) == 0x05)
				flag |= H264_NAL_FLAG_KEY;
			else
				flag |= H264_NAL_FLAG_OTHER;
			p += 4;
		} else if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
			if ((p[3] & 0x1f) == 0x07)
				flag |= H264_NAL_FLAG_SPS;
			else if ((p[3] & 0x1f) == 0x08)
				flag |= H264_NAL_FLAG_PPS;
			else if ((p[3] & 0x1f) == 0x05)
				flag |= H264_NAL_FLAG_KEY;
			else
				flag |= H264_NAL_FLAG_OTHER;
			p += 3;
		} else {
			p++;
		}
	}

	return flag;
}

static int _check_h264_nal(uint8_t *data, int size)
{
	const uint8_t *p = data;
	int flag = 0;

	if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
		if ((p[4] & 0x0f) == 0x07)
			flag |= H264_NAL_FLAG_SPS;
		else if ((p[4] & 0x0f) == 0x08)
			flag |= H264_NAL_FLAG_PPS;
		else if ((p[4] & 0x0f) == 0x05)
			flag |= H264_NAL_FLAG_KEY;
	} else if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
		if ((p[3] & 0x0f) == 0x07)
			flag |= H264_NAL_FLAG_SPS;
		else if ((p[3] & 0x0f) == 0x08)
			flag |= H264_NAL_FLAG_PPS;
		else if ((p[3] & 0x0f) == 0x05)
			flag |= H264_NAL_FLAG_KEY;
	}

	return flag;
}

static int _read_packet(RawStreamDemuxer_t *demuxer, dmx_packet_t *pkt)
{
	AVIOContext *ctx = demuxer->pIOCtx;
	dmx_mediainfo_t *mi = &demuxer->mediaInfo;
	dmx_vinfo_t *vi = &mi->vinfo[mi->default_vstream_idx];
	dmx_ainfo_t *ai = &mi->ainfo[mi->default_astream_idx];
	void *buf = NULL;
	uint8_t index;
	ssize_t ret;

	ret = ctx->read(ctx, &index, sizeof(uint8_t));
	if (ret < 0) {
		OSCL_LOGE("read stream_index error!");
		return -1;
	}
	pkt->stream_index = index;

#ifdef VIRGO_CUSTOMIZED
	ret = ctx->read(ctx, (uint8_t *)&pkt->timestamp, sizeof(int64_t));
#else
	ret = ctx->read(ctx, (uint8_t *)&pkt->pts, sizeof(int64_t));
#endif
	if (ret < 0) {
		OSCL_LOGE("read pts error!");
		return -1;
	}

	ret = ctx->read(ctx, (uint8_t *)&pkt->data_size, sizeof(int32_t));
	if (ret < 0) {
		OSCL_LOGE("read data_size error!");
		return -1;
	}
	OSCL_LOGD("index=%d(vidx=%d, aidx=%d), size=%d",
		index, vi->stream_index, ai->stream_index,
		pkt->data_size);

	if (pkt->stream_index == vi->stream_index) {
		buf = demuxer->vbuf;
		pkt->media_type = AV_MEDIA_TYPE_VIDEO;
	} else if (pkt->stream_index == ai->stream_index) {
		buf = demuxer->abuf;
		pkt->media_type = AV_MEDIA_TYPE_AUDIO;
	} else {
		OSCL_LOGE("unknown stream index %d", pkt->stream_index);
		return -1;
	}
	pkt->vir_addr =  cycle_buffer_malloc(buf, pkt->data_size);
	if (pkt->vir_addr == NULL) {
		OSCL_LOGW("run out of cycle_buffer!");
		return DMX_ERRNO_NOBUF;
	}
	pkt->phy_addr = cycle_buffer_vir_to_phy(buf, pkt->vir_addr);
	ret = ctx->read(ctx, pkt->vir_addr, pkt->data_size);
	if (ret < 0) {
		OSCL_LOGE("read data %d bytes error!", pkt->data_size);
		goto ERROR;
	}

	return 0;

ERROR:
	cycle_buffer_free(buf, pkt->vir_addr);
	return -1;
}

static int _return_packet(RawStreamDemuxer_t *demuxer, dmx_packet_t *pkt)
{
	dmx_mediainfo_t *mi = &demuxer->mediaInfo;
	dmx_vinfo_t *vi = &mi->vinfo[mi->default_vstream_idx];
	dmx_ainfo_t *ai = &mi->ainfo[mi->default_astream_idx];
	void *buf = NULL;

	if (pkt->stream_index == vi->stream_index) {
		buf = demuxer->vbuf;
	} else if (pkt->stream_index == ai->stream_index) {
		buf = demuxer->abuf;
	} else {
		OSCL_LOGE("unknown stream index %d", pkt->stream_index);
		return -1;
	}
	cycle_buffer_free(buf, pkt->vir_addr);

	return 0;
}

static int _get_extradata(RawStreamDemuxer_t *demuxer)
{
	dmx_mediainfo_t *mi = &demuxer->mediaInfo;
	dmx_vinfo_t *vi = &mi->vinfo[mi->default_vstream_idx];
	dmx_packet_t pkt;
	char *extra = NULL;
	int extra_sz = 0;
	int flags = 0;

	if (vi->codec_id != AV_CODEC_TYPE_H264) {
		OSCL_LOGW("not h264!");
		return 0;
	}

	OSCL_LOGI("get video extradata start");

	while (1) {
		int ret = _read_packet(demuxer, &pkt);
		if (ret != 0) {
			OSCL_LOGE("_read_packet error");
			break;
		}
		int fg = _check_h264_nal_all(pkt.vir_addr, pkt.data_size);
		if (fg & H264_NAL_FLAG_SPS)
			flags |= fg;
		else if (fg & H264_NAL_FLAG_PPS)
			flags |= fg;
		if (flags & (H264_NAL_FLAG_SPS | H264_NAL_FLAG_PPS)) {
			if (extra == NULL) {
				extra = oscl_malloc(pkt.data_size);
				memcpy(extra, pkt.vir_addr, pkt.data_size);
				extra_sz = pkt.data_size;
			} else {
				extra = oscl_realloc(extra,
					extra_sz + pkt.data_size);
				memcpy(extra + extra_sz, pkt.vir_addr, pkt.data_size);
				extra_sz += pkt.data_size;
			}
		}
		demuxer->pre_read_pkt.vir_addr = NULL;
		if ((flags & (H264_NAL_FLAG_SPS | H264_NAL_FLAG_PPS)) ==
			(H264_NAL_FLAG_SPS | H264_NAL_FLAG_PPS)) {
			if ((flags & (~(H264_NAL_FLAG_SPS | H264_NAL_FLAG_PPS))) != 0)
				demuxer->pre_read_pkt = pkt;
			else
				_return_packet(demuxer, &pkt);
			break;
		} else {
			_return_packet(demuxer, &pkt);
		}
	}
	vi->extradata = extra;
	vi->extradata_size = extra_sz;

	OSCL_LOGI("get video extradata end, size=%d", extra_sz);

	return 0;
}

static void *rawStreamDmx_readHeader(void *io_ctx,
	dmx_buffer_info_t *bufinfo, dmx_mediainfo_t *mediainfo)
{
	if (io_ctx == NULL || mediainfo == NULL) {
		OSCL_LOGE("invalid parameter(io=%p, mediainfo=%p)!", io_ctx, mediainfo);
		return NULL;
	}

	RawStreamDemuxer_t *demuxer;
	dmx_mediainfo_t *mi;
	int ret;

	demuxer = oscl_zalloc(sizeof(RawStreamDemuxer_t));
	if (demuxer == NULL) {
		OSCL_LOGE("alloc demuxer error!");
		return NULL;
	}
	demuxer->pIOCtx = io_ctx;
	mi = &demuxer->mediaInfo;

	ret = demuxer->pIOCtx->read(demuxer->pIOCtx, (uint8_t *)mi,
		sizeof(dmx_mediainfo_t));
	if (ret < 0) {
		OSCL_LOGE("read error!");
		goto ERROR;
	}

	if (mi->vstream_num > 0) {
		demuxer->vbuf = cycle_buffer_creat("demuxer-v-0",
			bufinfo->video_buf_size, bufinfo->video_buf_align);
		if (demuxer->vbuf == NULL) {
			OSCL_LOGE("video cycle_buffer_creat error!");
			goto ERROR;
		}
	}

	if (mi->astream_num > 0) {
		demuxer->abuf = cycle_buffer_creat("demuxer-a-1",
			bufinfo->audio_buf_size, bufinfo->audio_buf_align);
		if (demuxer->abuf == NULL) {
			OSCL_LOGE("audio cycle_buffer_creat error!");
			goto ERROR;
		}
	}

	_get_extradata(demuxer);
	*mediainfo = *mi;

	return demuxer;

ERROR:
	if (demuxer->vbuf != NULL)
		cycle_buffer_destory(demuxer->vbuf);
	if (demuxer->abuf != NULL)
		cycle_buffer_destory(demuxer->abuf);
	free(demuxer);
	return NULL;
}

static int rawStreamDmx_readClose(void *handle)
{
	if (handle == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	RawStreamDemuxer_t *demuxer = handle;
	if (demuxer->vbuf != NULL)
		cycle_buffer_destory(demuxer->vbuf);
	if (demuxer->abuf != NULL)
		cycle_buffer_destory(demuxer->abuf);
	free(demuxer);
	return 0;
}

static int rawStreamDmx_readPacket(void *handle, dmx_packet_t *pkt)
{
	RawStreamDemuxer_t *demuxer = handle;
	int fg;
	int ret;

	if (demuxer->pre_read_pkt.vir_addr != NULL) {
		*pkt = demuxer->pre_read_pkt;
		demuxer->pre_read_pkt.vir_addr = NULL;
		return 0;
	}

	ret = _read_packet(handle, pkt);
	if (ret != 0) {
		OSCL_LOGE("_read_packet error");
		return ret;
	}
	fg = _check_h264_nal(pkt->vir_addr, pkt->data_size);
	if (fg & H264_NAL_FLAG_KEY)
		pkt->flags |= AV_PACKET_FLAG_KEY;
	pkt->flags |= AV_PACKET_FLAG_ENDOFFRAME;
	if (pkt->media_type == AV_MEDIA_TYPE_VIDEO) {
		if (!demuxer->vrst_flag) {
			pkt->flags |= AV_PACKET_FLAG_RESET;
			demuxer->vrst_flag = 1;
		}
	} else {
		if (!demuxer->arst_flag) {
			pkt->flags |= AV_PACKET_FLAG_RESET;
			demuxer->arst_flag = 1;
		}
	}

	return 0;
}

static int rawStreamDmx_returnPacket(void *handle, dmx_packet_t *pkt)
{
	return _return_packet(handle, pkt);
}

#ifdef VIRGO_CUSTOMIZED
static int rawStreamDmx_readSeek(void *handle, int64_t timestamp)
#else
static int rawStreamDmx_readSeek(void *handle, int64_t timestamp, dmx_seek_dir_e dir)
#endif
{
	OSCL_LOGW("seek is not support!");
	return 0;
}

static int rawStreamDmx_readProbe(void *io_ctx)
{
	OSCL_LOGW("probe is not support!");
	return 0;
}

static int rawStreamDmx_readCtrl(void *handle, dmx_cmd_e cmd, void *arg)
{
	RawStreamDemuxer_t *demuxer = handle;
	AVIOContext *ctx = demuxer->pIOCtx;

	switch (cmd) {
	case DMX_CMD_BLOCKING:
		ctx->ctrl(ctx, IOSTREAM_CMD_BLOCKING, arg);
		break;
	default:
		OSCL_LOGW("cmd %d not support!", cmd);
		return -1;
	}
	return 0;
}

static dmx_plugin_t g_rawStreamDmx_plugin = {
	.read_header = rawStreamDmx_readHeader,
	.read_packet = rawStreamDmx_readPacket,
	.return_packet = rawStreamDmx_returnPacket,
	.read_close = rawStreamDmx_readClose,
	.read_seek = rawStreamDmx_readSeek,
	.read_probe = rawStreamDmx_readProbe,
	.read_ctrl = rawStreamDmx_readCtrl,
};

#if defined(__EOS__)
dmx_plugin_t *rawstream_plugin_info(void)
{
	OSCL_LOGI("rawstream demuxer version: %s", RAWSTREAM_DEMUXER_VERSION);
	return &g_rawStreamDmx_plugin;
}
#else
dmx_plugin_t *get_plugin_info(void)
{
	OSCL_LOGI("rawstream demuxer version: %s", RAWSTREAM_DEMUXER_VERSION);
	return &g_rawStreamDmx_plugin;
}
#endif
