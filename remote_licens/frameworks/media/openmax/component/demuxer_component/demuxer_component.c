/*
 * demuxer_component.c - code for demuxer component.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "demuxer_component"

#include <oscl.h>
#include <base_component.h>
#include <iostream_plugin.h>
#include <media_common.h>
#include <omx_mediaplayer.h>
#include "demuxer/demuxer.h"
#include "dmx_iostream/dmx_iostream.h"
#include "dmx_iostream/file_interface.h"
#include "demuxer_component.h"
#include "clock_port.h"
#include <lombo_dec_common.h>
#include "demuxer_port.h"
#include "vdec_lib_handle.h"
#include "adec_lib_handle.h"
#include <omx_vendor_lb.h>
#include <demuxer_plugin.h>

#define DMX_SUPPORT_SUBTITLE

#define DMX_AUDIO_BUF_NUM       512
#define DMX_VIDEO_BUF_NUM       256
#define DMX_SUB_BUF_NUM         64

#define DMX_MAX_PORT_NUM 3
#define DMX_AUDIO_PORT	0
#define DMX_VIDEO_PORT	1
#ifdef DMX_SUPPORT_SUBTITLE
#define DMX_CLOCK_PORT	2
/* subtitle port number must be continuous*/
#define DMX_SUB_PORT	3

#else
#define DMX_CLOCK_PORT	2
#endif

#define DMX_VIDEO_BUFFER_SIZE		(10*1024*1024)
#define DMX_VIDEO_BUFFER_ALIGN		(4*1024)
#define DMX_AUDIO_BUFFER_SIZE		(1*1024*1024)
#define DMX_AUDIO_BUFFER_ALIGN		(1)
#define DMX_SUBTITLE_BUFFER_SIZE	(2*1024*1024)
#define DMX_SUBTITLE_BUFFER_ALIGN	(1)

#define DMX_MAX_CACHE_PKT_NUM	64

typedef struct DmxPktElem {
	dmx_packet_t pkt;
	struct DmxPktElem *next;
} DmxPktElem_t;

typedef struct DmxPktQueue {
	DmxPktElem_t *tail, *head;
	pthread_mutex_t lock;
	int count;
} DmxPktQueue_t;

typedef struct DmxPktQueueMgr {
	DmxPktQueue_t *free_q;
	DmxPktQueue_t *valid_q;
} DmxPktQueueMgr_t;
typedef struct exsub_context_tag {
	OMX_BOOL seek_flag;
	exsub_info_t info;
} exsub_context_t;
#if 0
typedef struct insub_context_tag {
	OMX_S32 status;
} emsub_context_t;
#endif
typedef struct sub_port_info_tag {
	OMX_S32 sub_idx;
	OMX_S32 stream_idx;
	OMX_S32 port_idx;
	OMX_S8 is_uesd;
	OMX_S8 status;
	OMX_S32 sw_strack;
	DmxPktQueueMgr_t *sq_mgr;
	/*dmx_buffer_manager_t s_mgr;*/
	void *iostream;
} sub_port_context_t;
typedef struct sub_context_tag {
	exsub_context_t ex;
	/*emsub_context_t em;*/
	OMX_S32 open_num;
	OMX_S32 sw_status_idx;
	sub_port_context_t port[DMX_SUBTITLE_STREAM_NUM];
} sub_context_t;

typedef struct demuxer_comp_priv {
	void *handle;
	void *iostream;
	omx_mediainfo_t omx_mediainfo;
	OMX_TICKS video_time;
	OMX_TICKS audio_time;
	av_muxer_type_e muxer_type;
	OMX_TICKS seek_time;
	OMX_BOOL seek_flag;
	DmxPktQueueMgr_t *videoqueue;
	DmxPktQueueMgr_t *audioqueue;
	DmxPktQueueMgr_t *subqueue;
	pthread_mutex_t mutex;
	int sw_atrack;
	dmx_stream_handle_t stream_hdl;
	sub_context_t sub;

	OMX_BOOL clkbuf_thd_run;
	pthread_t clkbuf_thd;
	OMX_BOOL reading_packet;

	file_stream_para_t file_para;
} demuxer_comp_priv_t;

typedef struct dmx_buffer_manager {
	base_port_t *outport;
	OMX_BUFFERHEADERTYPE *pbuf;
	OMX_BOOL eos;
	OMX_S64 wait_buf_time;
	char *name;
} dmx_buffer_manager_t;
typedef enum sub_err_tag {
	sub_err_none,
	sub_err_url_opened,
	sub_err_url_not_added,
	sub_err_open_failed,
	sub_err_seek_failed,
	sub_err_max
} sub_err_e;
static int _dmxPktQueue_push(DmxPktQueue_t *queue, DmxPktElem_t *elem)
{
	if (queue == NULL)
		return -1;

	elem->next = NULL;
	pthread_mutex_lock(&queue->lock);
	if (queue->tail == NULL) {
		queue->head = queue->tail = elem;
	} else {
		queue->tail->next = elem;
		queue->tail = elem;
	}
	queue->count++;
	pthread_mutex_unlock(&queue->lock);

	return 0;
}

static DmxPktElem_t *_dmxPktQueue_pop(DmxPktQueue_t *queue)
{
	DmxPktElem_t *elem;

	if (queue == NULL)
		return NULL;

	pthread_mutex_lock(&queue->lock);
	if (queue->head == NULL) {
		pthread_mutex_unlock(&queue->lock);
		return NULL;
	}
	elem = queue->head;
	queue->head = elem->next;
	if (queue->head == NULL)
		queue->tail = NULL;
	queue->count--;
	pthread_mutex_unlock(&queue->lock);

	return elem;
}

#if 0
static int _dmxPktQueue_count(DmxPktQueue_t *queue)
{
	int elem_num;

	pthread_mutex_lock(&queue->lock);
	elem_num = queue->count;
	pthread_mutex_unlock(&queue->lock);

	return elem_num;
}
#endif

static DmxPktQueue_t *_dmxPktQueue_init(void)
{
	DmxPktQueue_t *queue = (DmxPktQueue_t *)oscl_malloc(sizeof(DmxPktQueue_t));
	if (queue == NULL) {
		OSCL_LOGE("alloc DmxPktQueue_init error!");
		return NULL;
	}
	pthread_mutex_init(&queue->lock, NULL);
	queue->head = queue->tail = NULL;
	queue->count = 0;

	return queue;
}

static void _dmxPktQueue_deinit(DmxPktQueue_t *queue)
{
	DmxPktElem_t *elem;
	while ((elem = _dmxPktQueue_pop(queue)) != NULL)
		oscl_free(elem);
	oscl_free(queue);
}

static int subtitle_codec_supported(av_codec_type_e codectype)
{
	switch (codectype) {
	case AV_CODEC_TYPE_ASS:
	case AV_CODEC_TYPE_LRC:
	case AV_CODEC_TYPE_SRT:
	case AV_CODEC_TYPE_STL:
	case AV_CODEC_TYPE_TXT:
	case AV_CODEC_TYPE_TTS:
	case AV_CODEC_TYPE_SUB:
	case AV_CODEC_TYPE_VSF:
	case AV_CODEC_TYPE_SMI:
	case AV_CODEC_TYPE_JSS:
	case AV_CODEC_TYPE_ASC:
	case AV_CODEC_TYPE_OVR:
	case AV_CODEC_TYPE_TTXT:
	case AV_CODEC_TYPE_VOBSUB:
	case AV_CODEC_TYPE_PGS:
	case AV_CODEC_TYPE_DVD_SUBTITLE:
	case AV_CODEC_TYPE_DVDXSUB:
	case AV_CODEC_TYPE_DVDSUB:
		return 1;

	default:
		return 0;
	}
	return 0;
}

static int _dmx_save_packet(demuxer_comp_priv_t *priv, dmx_packet_t *pkt)
{
	DmxPktQueueMgr_t *qmgr = NULL;
	DmxPktElem_t *elem = NULL;
	omx_mediainfo_t *omx_mi = &priv->omx_mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;
	int i;

	if (pkt->media_type == AV_MEDIA_TYPE_VIDEO) {
		for (i = 0; i < mi->vstream_num; i++) {
			dmx_vinfo_t *vinfo = &mi->vinfo[i];
			if (pkt->stream_index == vinfo->stream_index &&
				(omx_mi->video_supported_flag & 1 << i)) {
				qmgr = &priv->videoqueue[i];
				break;
			}
		}
	} else if (pkt->media_type == AV_MEDIA_TYPE_AUDIO) {
		for (i = 0; i < mi->astream_num; i++) {
			dmx_ainfo_t *ainfo = &mi->ainfo[i];
			if (pkt->stream_index == ainfo->stream_index &&
				(omx_mi->audio_supported_flag & 1 << i)) {
				qmgr = &priv->audioqueue[i];
				break;
			}
		}
	} else if (pkt->media_type == AV_MEDIA_TYPE_SUBTITLE) {
		for (i = 0; i < mi->substream_num; i++) {
			dmx_subinfo_t *sinfo = &mi->subinfo[i];
			if (pkt->stream_index == sinfo->stream_index &&
				(omx_mi->sub_supported_flag & 1 << i)) {
				qmgr = &priv->subqueue[i];
				break;
			}
		}
	}

	if (qmgr == NULL) {
		OSCL_LOGD("unknown stream: index=%d, media_type=%d",
			pkt->stream_index, pkt->media_type);
		demuxer_return_packet(priv->handle, pkt);
		return -1;
	}
	elem = _dmxPktQueue_pop(qmgr->free_q);
	if (elem == NULL) {
		OSCL_LOGD("queue full, index=%d, media_type=%d",
			pkt->stream_index, pkt->media_type);
		elem = _dmxPktQueue_pop(qmgr->valid_q);
		demuxer_return_packet(priv->handle, &elem->pkt);
	}
	elem->pkt = *pkt;
	_dmxPktQueue_push(qmgr->valid_q, elem);
	return 0;
}

static void demuxer_clear_queue_pkt(demuxer_comp_priv_t *priv)
{
	omx_mediainfo_t *omx_mi = &priv->omx_mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;

	if (priv->subqueue != NULL) {
		int i;
		for (i = 0; i < mi->substream_num; i++) {
			DmxPktQueueMgr_t *q = &priv->subqueue[i];
			DmxPktElem_t *elem;
			while ((elem = _dmxPktQueue_pop(q->valid_q)) != NULL) {
				demuxer_return_packet(priv->handle, &elem->pkt);
				_dmxPktQueue_push(q->free_q, elem);
			}
		}
	}

	if (priv->audioqueue != NULL) {
		int i;
		for (i = 0; i < mi->astream_num; i++) {
			DmxPktQueueMgr_t *q = &priv->audioqueue[i];
			DmxPktElem_t *elem;
			while ((elem = _dmxPktQueue_pop(q->valid_q)) != NULL) {
				demuxer_return_packet(priv->handle, &elem->pkt);
				_dmxPktQueue_push(q->free_q, elem);
			}
		}
	}

	if (priv->videoqueue != NULL) {
		int i;
		for (i = 0; i < mi->vstream_num; i++) {
			DmxPktQueueMgr_t *q = &priv->videoqueue[i];
			DmxPktElem_t *elem;
			while ((elem = _dmxPktQueue_pop(q->valid_q)) != NULL) {
				demuxer_return_packet(priv->handle, &elem->pkt);
				_dmxPktQueue_push(q->free_q, elem);
			}
		}
	}
}
static OMX_S32 demuxer_sub_sw_strack(lb_omx_component_t *component,
	OMX_S32 *port_idx, DmxPktQueueMgr_t **sq, OMX_S32 strack,
	dmx_packet_t *pkt, dmx_subinfo_t **p)
{
	demuxer_comp_priv_t *priv = component->component_private;
	base_port_t *clkport = &component->port[DMX_CLOCK_PORT];
	OMX_HANDLETYPE clkcmp = clkport->tunneled_component;
	OMX_TIME_CONFIG_TIMESTAMPTYPE curtime;
	DmxPktQueueMgr_t *sq_mgr = NULL;
	DmxPktElem_t *elem;
	OMX_S32 ret = -1, i = 0;

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (strack == priv->sub.port[i].sub_idx)
			break;
	}

	if (i >= DMX_SUBTITLE_STREAM_NUM) {
		OSCL_LOGE("parameter strack is abnormal: %d", strack);
		return -1;
	}

	if (priv->sub.port[i].stream_idx >= EXSUB_START_STREAM_INDEX) {
		OSCL_LOGW("external subtitle can't be switch");
		return -1;
	}

	if (priv->sub.port[i].status != sub_status_opened) {
		/*OSCL_LOGE("sub component and other resource are not ready");*/
		return 2;
	}

	/* get cur mediatime */
	ret = OMX_GetConfig(clkcmp,
			OMX_IndexConfigTimeCurrentMediaTime,
			&curtime);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGW("get cur mediatime err!");
		return -1;
	}
	OSCL_LOGI("current media time %lld", curtime.nTimestamp);
	sq_mgr = priv->sub.port[i].sq_mgr;
	*port_idx = priv->sub.port[i].port_idx;
	*p = &priv->omx_mediainfo.mediainfo.subinfo[strack];
	*sq = sq_mgr;
	while ((elem = _dmxPktQueue_pop(sq_mgr->valid_q)) != NULL) {
		if (elem->pkt.pts >= curtime.nTimestamp) {
			memcpy(pkt, &elem->pkt, sizeof(dmx_packet_t));
			_dmxPktQueue_push(sq_mgr->free_q, elem);
			OSCL_LOGI("play time %lld", (OMX_S64)pkt->pts);
			break;
		}
		demuxer_return_packet(priv->handle, &elem->pkt);
		/*OSCL_LOGI("drop frame time %lld", elem->pkt.pts);*/
		_dmxPktQueue_push(sq_mgr->free_q, elem);
	}

	return ret;
}
static OMX_S32 demuxer_sub_judge_stream_idx(sub_context_t *psub,
	OMX_S32 stream_idx, OMX_S32 *port_idx)
{
	OMX_S32 i = 0;


	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (psub->port[i].status != sub_status_opened)
			continue;
		if (stream_idx == psub->port[i].stream_idx)
			break;
	}

	if (i >= DMX_SUBTITLE_STREAM_NUM)
		return -1;
	*port_idx = psub->port[i].port_idx;
	/*OSCL_LOGE("pkt_stream_idx: %d, open_stream_idx: %d, port_idx: %d",
		stream_idx, psub->port[i].stream_idx, *port_idx);*/
	return 0;
}

static OMX_S32 demuxer_open_emsub_stream(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	omx_mediainfo_t *omx_mi = &priv->omx_mediainfo;
	dmx_mediainfo_t *mi = &priv->omx_mediainfo.mediainfo;
	dmx_subinfo_t *sinfo;


	sinfo = &mi->subinfo[idx];
	if (subtitle_codec_supported(sinfo->codec_id)) {
		/* notify FormatDetected */
		(*(component->callbacks.EventHandler))(
			&component->base_comp,
			component->callback_data,
			OMX_EventPortFormatDetected,
			OMX_IndexParamSubtitlePortFormat,
			SUB_STYLE_EMBEDED,
			omx_mi);
		OSCL_LOGI("embeded sub codec %x supported", sinfo->codec_id);
	} else {
		OSCL_LOGW("embeded sub codec %x not support", sinfo->codec_id);
		return -1;
	}

	return OMX_ErrorNone;
}
static OMX_S32 demuxer_open_exsub_stream(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	exsub_context_t *exsub = &priv->sub.ex;
	exsub_info_t *si = &exsub->info;
	dmx_buffer_info_t buf_info;

		/* close old external subtitle */
		/*demuxer_exsub_close(priv->handle, si, close_subtile_all, 0);*/
		/* open new sub */
		buf_info.sub_buf_size = DMX_SUBTITLE_BUFFER_SIZE;
		buf_info.sub_buf_align = DMX_SUBTITLE_BUFFER_ALIGN;

		ret = demuxer_exsub_open(priv->handle, si->stream[idx].text,
			&buf_info, si);
		if (ret != 0) {
			OSCL_LOGE("open subtitle stream failed!");
			pthread_mutex_unlock(&priv->mutex);
			return sub_err_open_failed;
		}
		OSCL_LOGI("external subtitle codec_id %x, idx %d",
			si->stream[idx].codec_id, si->stream[idx].stream_index);
		/* notify FormatDetected */
		(*(component->callbacks.EventHandler))(
			&component->base_comp,
			component->callback_data,
			OMX_EventPortFormatDetected,
			OMX_IndexParamSubtitlePortFormat,
			SUB_STYLE_EXTERNAL,
			si);
		exsub->seek_flag = OMX_TRUE;
	/*}*/
	return ret;

}
static OMX_S32 demuxer_sub_enable(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	dmx_mediainfo_t *emsi = &priv->omx_mediainfo.mediainfo;
	sub_context_t *sub = &priv->sub;
	exsub_info_t *exsi = &sub->ex.info;
	OMX_S32 index = -1;
	OMX_S32 i = 0, ret = 0;

	OSCL_LOGI("sub enable: %d", idx);
	/*find free port*/
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (sub->port[i].is_uesd == 0)
			break;
		if (idx == sub->port[i].sub_idx) {
			OSCL_LOGE("the cur url has been opened ,don't do that again");
			return sub_err_url_opened;
		}
	}
	if (i >= DMX_SUBTITLE_STREAM_NUM) {
		OSCL_LOGE("number of subtitle opened equal %d", DMX_SUBTITLE_STREAM_NUM);
		return -1;
	}

	if (idx >= (emsi->substream_num + exsi->stream_num)) {
		OSCL_LOGE("param idx is invalid: %d; sub streams: %d", idx,
			(emsi->substream_num + exsi->stream_num));
		return sub_err_url_not_added;
	}

	if (idx < emsi->substream_num) {
		index = idx;
		ret = demuxer_open_emsub_stream(hComp, index);
		if (ret) {
			OSCL_LOGE("open embedded subtitle failed");
			return ret;
		}
		sub->port[i].sq_mgr = &priv->subqueue[index];
		sub->port[i].stream_idx = emsi->subinfo[index].stream_index;
		sub->port[i].sw_strack = idx;
	} else {
		index  = idx - emsi->substream_num;
		ret = demuxer_open_exsub_stream(hComp, index);
		if (ret) {
			OSCL_LOGE("open external subtitle failed");
			return ret;
		}
		sub->port[i].stream_idx = exsi->stream[index].stream_index;
	}

	sub->port[i].is_uesd = 1;
	sub->port[i].sub_idx = idx;
	sub->port[i].status = sub_status_pause;
	priv->sub.sw_status_idx = -1;
	/* notify portsetting changed */
	(*(component->callbacks.EventHandler))(
		&component->base_comp,
		component->callback_data,
		OMX_EventPortSettingsChanged,
		sub->port[i].port_idx,
		sub->port[i].sub_idx,
		NULL);
	OSCL_LOGI("subtitle enable successful: %d, %d", i, idx);
	/*sub->open_num++;*/

	return ret;
}
static OMX_S32 demuxer_close_exsub_stream(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	exsub_info_t *exsi = &priv->sub.ex.info;


	if (exsi->stream[idx].codec_id == AV_CODEC_TYPE_INVALID) {
		OSCL_LOGE("the cur url has been closed ,don't do that again");
		return sub_err_url_opened;
	} else {
		demuxer_exsub_close(priv->handle, exsi, close_subtile_one_way, idx);
	}
	return 0;
}


static OMX_S32 demuxer_sub_disable(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	dmx_mediainfo_t *emsi = &priv->omx_mediainfo.mediainfo;
	exsub_info_t *exsi = &priv->sub.ex.info;
	OMX_S32 index = -1, i = 0, ret = 0;

	OSCL_LOGI("idx: %d, emsi->substream_num: %d, exsi->stream_num: %d",
		idx, emsi->substream_num, exsi->stream_num);
	if (idx  >= (emsi->substream_num + exsi->stream_num)) {
		OSCL_LOGE("param idx is invalid: %d; sub streams: %d", idx,
			(emsi->substream_num + exsi->stream_num));
		return sub_err_url_not_added;
	}

	if (idx >= emsi->substream_num) {
		index  = idx - emsi->substream_num;
		ret = demuxer_close_exsub_stream(hComp, index);
		if (ret)
			OSCL_LOGE("disable external subtitle failed");
	}
	if (ret == 0) {
		for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
			if (idx == priv->sub.port[i].sub_idx) {
				priv->sub.port[i].is_uesd = 0;
				priv->sub.port[i].sq_mgr = NULL;
				priv->sub.port[i].stream_idx = -1;
				priv->sub.port[i].sub_idx = -1;
				priv->sub.port[i].sw_strack = -1;
				/*priv->sub.open_num--;*/
				priv->sub.sw_status_idx = -1;
			}
		}
	}
	return ret;
}
static OMX_S32 demuxer_sub_set_swstatus_index(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;

	priv->sub.sw_status_idx = idx;

	return 0;
}
static OMX_S32 demuxer_sub_change_status(OMX_HANDLETYPE hComp, OMX_S32 status)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	OMX_S32 ret = -1, i;

	if (priv->sub.sw_status_idx < 0) {
		OSCL_LOGE("the index of sub status changed is error: %d",
			priv->sub.sw_status_idx);
		return ret;
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if ((priv->sub.port[i].is_uesd) &&
			(priv->sub.sw_status_idx == priv->sub.port[i].sub_idx)) {
			priv->sub.port[i].status = status;
			break;
		}
	}
	if ((i < DMX_SUBTITLE_STREAM_NUM) &&
		(priv->sub.port[i].stream_idx >= EXSUB_START_STREAM_INDEX)) {
		ret = demuxer_exsub_change_status(priv->handle,
			priv->sub.port[i].stream_idx, status);
	} else
		ret = 0;
	priv->sub.sw_status_idx = -1;

	return ret;
}
static OMX_S32 demuxer_sub_get_port_index(OMX_HANDLETYPE hComp, OMX_S32 idx)
{
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv;
	sub_context_t *psub = NULL;
	OMX_S32 i = 0;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	priv = component->component_private;

	psub = &priv->sub;
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		OSCL_LOGI("i: %d, idx: %d, sub idx: %d, port idx: %d, is used: %d",
			i, idx, psub->port[i].sub_idx, psub->port[i].port_idx,
			psub->port[i].is_uesd);
		if ((idx == psub->port[i].sub_idx) && psub->port[i].is_uesd)
			break;
	}
	if (i < DMX_SUBTITLE_STREAM_NUM)
		return psub->port[i].port_idx;
	else
		return -1;

}
static OMX_S32 demuxer_add_exsub_fileurl(OMX_HANDLETYPE hComp, OMX_STRING url)
{
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv;
	exsub_info_t *si = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_S32 i = 0;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	priv = component->component_private;
	si = &priv->sub.ex.info;
	oscl_param_check(url != NULL, OMX_ErrorBadParameter, NULL);
	/*is exist?*/
	if (access(url, F_OK) == -1) {
		OSCL_LOGE("this subtitle file dose not exist: %s", url);
		return -1;
	}
	if ((si->stream_num + 1) > DMX_MAX_EXSUB_NUM) {
		OSCL_LOGE("the array of external subtitle is full: %d, can't added",
			si->stream_num);
		return -2;
	}

	/*if demuxer lib is supported for the "url"*/
	OSCL_LOGI("prob url: %s", url);
	if (NULL == demuxer_exsub_probe(url)) {
		OSCL_LOGE("the subtitle is not supported by demuxer lib");
		return -3;
	}
	/*add url pointer array*/
	for (i = 0; i < si->stream_num; i++) {
		if (!strcmp(url, si->stream[i].text)) {
			OSCL_LOGW("the cur url has been added sub array, don't again");
			return -4;
		}
	}
	si->stream[si->stream_num].text = oscl_strdup((char *)url);
	OSCL_LOGI("si->stream[%d].name: %s", si->stream_num,
		si->stream[si->stream_num].text);
	si->stream_num++;
	return ret;
}
static OMX_S32 demuxer_seek_exsub(OMX_HANDLETYPE hComp, OMX_IN OMX_PTR paramData)
{
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv;
	base_port_t *clkport = NULL;
	OMX_HANDLETYPE clkcmp = NULL;
	exsub_context_t *exsub = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE curtime;
	OMX_S32 ret = -1, i = 0;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	clkport =  &component->port[DMX_CLOCK_PORT];
	clkcmp = clkport->tunneled_component;
	priv = component->component_private;
	exsub = &priv->sub.ex;

	if (exsub->seek_flag == OMX_TRUE) {
		for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
			if ((priv->sub.port[i].is_uesd) &&
				(priv->sub.port[i].status == sub_status_pause))
				break;
		}
		if (i >= DMX_SUBTITLE_STREAM_NUM) {
			OSCL_LOGE("NO stream need to seek");
			return 0;
		}
		/*auto seek to latest position*/
		exsub->seek_flag = OMX_FALSE;
		/* get cur mediatime */
		ret = OMX_GetConfig(clkcmp,
				OMX_IndexConfigTimeCurrentMediaTime,
				&curtime);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGW("get cur mediatime err!");
			return sub_err_seek_failed;
		}

		ret = demuxer_exsub_seek(priv->handle, curtime.nTimestamp,
			priv->sub.port[i].stream_idx);
		if (ret) {
			OSCL_LOGE("subtitle seek failed");
			return sub_err_seek_failed;
		}
	} else if (paramData) {
		/*seek to specify position*/
		ret = demuxer_exsub_seek(priv->handle, *((OMX_S32 *)paramData), -1);
		if (ret) {
			OSCL_LOGE("subtitle seek failed");
			return sub_err_seek_failed;
		}
	} else
		ret = 0;

	return ret;
}
static int demuxer_init(OMX_HANDLETYPE hComp)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	dmx_buffer_info_t buf_info;
	omx_mediainfo_t *omx_mi = &priv->omx_mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;

	if (priv->handle != NULL) {
		OSCL_LOGW("demuxer handle!=NULL, close it first!");
		demuxer_close(priv->handle);
		priv->handle = NULL;
	}

	omx_mi->start_time = 9223372036854775807LL;
	priv->sw_atrack = -1;

	buf_info.video_buf_size = DMX_VIDEO_BUFFER_SIZE;
	buf_info.video_buf_align = DMX_VIDEO_BUFFER_ALIGN;
	buf_info.audio_buf_size = DMX_AUDIO_BUFFER_SIZE;
	buf_info.audio_buf_align = DMX_AUDIO_BUFFER_ALIGN;
	buf_info.sub_buf_size = DMX_SUBTITLE_BUFFER_SIZE;
	buf_info.sub_buf_align = DMX_SUBTITLE_BUFFER_ALIGN;
	omx_mi->cur_video_idx = -1;
	omx_mi->cur_audio_idx = -1;
	omx_mi->video_supported_flag = 0;
	omx_mi->audio_supported_flag = 0;
	omx_mi->sub_supported_flag = 0;
	for (int i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++)
		priv->sub.port[i].sw_strack = -1;

	priv->handle = demuxer_open(&priv->file_para,
		dmx_iostream_get_context(priv->iostream),
		priv->muxer_type,
		&buf_info,
		mi);
	if (priv->handle == NULL) {
		OSCL_LOGE("open demuxer failed!");
		return -1;
	}
	OSCL_LOGI("audio num %d, video num %d, subtitle num %d, duration %lld, %s",
		mi->astream_num,
		mi->vstream_num,
		mi->substream_num,
		(long long)mi->duration,
		mi->seek_enable ? "seekable" : "unseekable");

	if (mi->astream_num > 0) {
		OMX_AUDIO_PORTDEFINITIONTYPE *aportdef =
			&component->port[DMX_AUDIO_PORT].port_param.format.audio;
		dmx_ainfo_t *ainfo;
		int i;
		priv->audioqueue =
			oscl_malloc(sizeof(DmxPktQueueMgr_t) * mi->astream_num);
		for (i = 0; i < mi->astream_num; i++) {
			int j;
			priv->audioqueue[i].free_q = _dmxPktQueue_init();
			priv->audioqueue[i].valid_q = _dmxPktQueue_init();
			for (j = 0; j < DMX_MAX_CACHE_PKT_NUM; j++) {
				DmxPktElem_t *elem = oscl_malloc(sizeof(DmxPktElem_t));
				if (elem == NULL) {
					OSCL_LOGE("alloc DmxPktElem_t error!");
					break;
				}
				_dmxPktQueue_push(priv->audioqueue[i].free_q, elem);
			}
			ainfo = &mi->ainfo[i];
			if (!audio_codec_supported(ainfo->codec_id)) {
				OSCL_LOGW("audio codec %x not support", ainfo->codec_id);
				continue;
			}
			omx_mi->audio_supported_flag |= 1 << i;
			OSCL_LOGD("audio codec %x supported", ainfo->codec_id);
			if (mi->default_astream_idx == i)
				omx_mi->cur_audio_idx = i;
			if (omx_mi->cur_audio_idx == -1)
				omx_mi->cur_audio_idx = i;
			if (omx_mi->start_time > ainfo->start_time)
				omx_mi->start_time = ainfo->start_time;
			OSCL_LOGI("%d audio stream: index=%d, id=%x, start_time %lld",
				i, ainfo->stream_index,
				ainfo->codec_id, ainfo->start_time);
			OSCL_LOGI("samplerate %d, %d ch, %d bits, bitrate=%d",
				ainfo->sample_rate, ainfo->channels,
				ainfo->bits_per_coded_sample, ainfo->bit_rate);
		}
		if (omx_mi->cur_audio_idx != -1) {
			ainfo = &mi->ainfo[omx_mi->cur_audio_idx];
			aportdef->eEncoding = ainfo->codec_id;

			OSCL_LOGI("select audio stream %d", omx_mi->cur_audio_idx);
			/* notify FormatDetected */
			(*(component->callbacks.EventHandler))(
				&component->base_comp,
				component->callback_data,
				OMX_EventPortFormatDetected,
				OMX_IndexParamAudioPortFormat,
				DMX_AUDIO_PORT,
				omx_mi);
			/* notify portsetting changed */
			(*(component->callbacks.EventHandler))(
				&component->base_comp,
				component->callback_data,
				OMX_EventPortSettingsChanged,
				DMX_AUDIO_PORT,
				0,
				NULL);
		} else {
			OSCL_LOGW("could not find audio stream!");
		}
	}

	if (mi->vstream_num > 0) {
		OMX_VIDEO_PORTDEFINITIONTYPE *vportdef =
			&component->port[DMX_VIDEO_PORT].port_param.format.video;
		dmx_vinfo_t *vinfo;
		int i;
		priv->videoqueue =
			oscl_malloc(sizeof(DmxPktQueueMgr_t) * mi->vstream_num);
		for (i = 0; i < mi->vstream_num; i++) {
			int j;
			priv->videoqueue[i].free_q = _dmxPktQueue_init();
			priv->videoqueue[i].valid_q = _dmxPktQueue_init();
			for (j = 0; j < DMX_MAX_CACHE_PKT_NUM; j++) {
				DmxPktElem_t *elem = oscl_malloc(sizeof(DmxPktElem_t));
				if (elem == NULL) {
					OSCL_LOGE("alloc DmxPktElem_t error!");
					break;
				}
				_dmxPktQueue_push(priv->videoqueue[i].free_q, elem);
			}
			vinfo = &mi->vinfo[i];

			if (!video_codec_supported(vinfo->codec_id)) {
				OSCL_LOGW("video codec %x not support", vinfo->codec_id);
				continue;
			}
			omx_mi->video_supported_flag |= 1 << i;
			OSCL_LOGD("video codec %x supported, extra=%p, extrasize=%d",
				vinfo->codec_id, vinfo->extradata, vinfo->extradata_size);
			if (mi->default_vstream_idx == i)
				omx_mi->cur_video_idx = i;
			if (omx_mi->cur_video_idx == -1)
				omx_mi->cur_video_idx = i;
			if (omx_mi->start_time > vinfo->start_time)
				omx_mi->start_time = vinfo->start_time;
			OSCL_LOGI("%d video stream: index %d, id %x, size %dx%d, ",
				i, vinfo->stream_index, vinfo->codec_id,
				vinfo->width, vinfo->height);
			OSCL_LOGI("start_time %lld, duration %lld",
				vinfo->start_time, vinfo->duration);
		}
		if (omx_mi->cur_video_idx != -1) {
			vinfo = &mi->vinfo[omx_mi->cur_video_idx];
			vportdef->eCompressionFormat = vinfo->codec_id;
			vportdef->nFrameWidth = vinfo->width;
			vportdef->nFrameHeight = vinfo->height;

			OSCL_LOGI("select video stream_index %d", vinfo->stream_index);
			/* notify FormatDetected */
			(*(component->callbacks.EventHandler))(
				&component->base_comp,
				component->callback_data,
				OMX_EventPortFormatDetected,
				OMX_IndexParamVideoPortFormat,
				DMX_VIDEO_PORT,
				omx_mi);
			/* notify portsetting changed */
			(*(component->callbacks.EventHandler))(
				&component->base_comp,
				component->callback_data,
				OMX_EventPortSettingsChanged,
				DMX_VIDEO_PORT,
				0,
				NULL);
		} else {
			OSCL_LOGW("could not find supported video stream!");
		}
	}

	if (mi->substream_num > 0) {
		dmx_subinfo_t *sinfo;
		int i;
		priv->subqueue =
			oscl_malloc(sizeof(DmxPktQueueMgr_t) * mi->substream_num);
		for (i = 0; i < mi->substream_num; i++) {
			int j;
			priv->subqueue[i].free_q = _dmxPktQueue_init();
			priv->subqueue[i].valid_q = _dmxPktQueue_init();
			for (j = 0; j < DMX_MAX_CACHE_PKT_NUM; j++) {
				DmxPktElem_t *elem = oscl_malloc(sizeof(DmxPktElem_t));
				if (elem == NULL) {
					OSCL_LOGE("alloc DmxPktElem_t error!");
					break;
				}
				_dmxPktQueue_push(priv->subqueue[i].free_q, elem);
			}
			sinfo = &mi->subinfo[i];
			if (subtitle_codec_supported(sinfo->codec_id))
				omx_mi->sub_supported_flag |= 1 << i;
			OSCL_LOGI("%d sub stream:index %d,id %x,text %s, start_time %lld",
				i, sinfo->stream_index, sinfo->codec_id, sinfo->text,
				sinfo->start_time);
		}
	}

	OSCL_LOGI("min start_time %lld", omx_mi->start_time);

	return 0;
}

static int demuxer_deinit(OMX_HANDLETYPE hComp)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	omx_mediainfo_t *omx_mi = &priv->omx_mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;
	OMX_S32 i = 0;

	if (priv->handle == NULL)
		return 0;

	for (i = 0; i < priv->sub.ex.info.stream_num; i++) {
		priv->sub.ex.info.stream[i].codec_id = 0;
		priv->sub.ex.info.stream[i].stream_index = -1;
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		priv->sub.port[i].is_uesd = 0;
		priv->sub.port[i].sq_mgr = NULL;
		priv->sub.port[i].stream_idx = -1;
		priv->sub.port[i].sub_idx = -1;
		priv->sub.port[i].sw_strack = -1;
	}
	/*priv->sub.open_num = 0;*/
	if (priv->subqueue != NULL) {
		int i;
		for (i = 0; i < mi->substream_num; i++) {
			DmxPktQueueMgr_t *q = &priv->subqueue[i];
			DmxPktElem_t *elem;
			while ((elem = _dmxPktQueue_pop(q->valid_q)) != NULL)
				oscl_free(elem);
			_dmxPktQueue_deinit(q->valid_q);
			while ((elem = _dmxPktQueue_pop(q->free_q)) != NULL)
				oscl_free(elem);
			_dmxPktQueue_deinit(q->free_q);
		}
		oscl_free(priv->subqueue);
		priv->subqueue = NULL;
	}

	if (priv->audioqueue != NULL) {
		int i;
		for (i = 0; i < mi->astream_num; i++) {
			DmxPktQueueMgr_t *q = &priv->audioqueue[i];
			DmxPktElem_t *elem;
			while ((elem = _dmxPktQueue_pop(q->valid_q)) != NULL)
				oscl_free(elem);
			_dmxPktQueue_deinit(q->valid_q);
			while ((elem = _dmxPktQueue_pop(q->free_q)) != NULL)
				oscl_free(elem);
			_dmxPktQueue_deinit(q->free_q);
		}
		oscl_free(priv->audioqueue);
		priv->audioqueue = NULL;
	}

	if (priv->videoqueue != NULL) {
		int i;
		for (i = 0; i < mi->vstream_num; i++) {
			DmxPktQueueMgr_t *q = &priv->videoqueue[i];
			DmxPktElem_t *elem;
			while ((elem = _dmxPktQueue_pop(q->valid_q)) != NULL)
				oscl_free(elem);
			_dmxPktQueue_deinit(q->valid_q);
			while ((elem = _dmxPktQueue_pop(q->free_q)) != NULL)
				oscl_free(elem);
			_dmxPktQueue_deinit(q->free_q);
		}
		oscl_free(priv->videoqueue);
		priv->videoqueue = NULL;
	}

	demuxer_close(priv->handle);
	priv->handle = NULL;

	return 0;
}

static OMX_ERRORTYPE demuxer_get_parameter(
	OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_INOUT OMX_PTR paramData)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	OMX_ERRORTYPE ret = OMX_ErrorNone;


	switch ((OMX_U32)paramIndex) {
	case omx_index_vendor_input_filename: {
		*((char **)paramData) = priv->file_para.filename;
		break;
	}
	case omx_index_vendor_get_port_number: {
		*((int *)paramData) = component->num_ports;
		break;
	}
	case omx_index_media_duration:
		*((long *)paramData) =
			(long)(priv->omx_mediainfo.mediainfo.duration / 1000);
		break;
	case OMX_IndexConfigTimePosition: {
		OMX_TIME_CONFIG_TIMESTAMPTYPE *timestamptype =
			(OMX_TIME_CONFIG_TIMESTAMPTYPE *)paramData;
		timestamptype->nTimestamp = priv->video_time;
		break;
	}
	case omx_index_media_get_exsub_info:
		*((exsub_info_t *)paramData) = priv->sub.ex.info;
		break;
	case omx_index_media_info: {
		*((omx_mediainfo_t *)paramData) = priv->omx_mediainfo;
		break;
	}
	case omx_index_stream_handle: {
		dmx_stream_handle_t **handle =
			(dmx_stream_handle_t **)paramData;
		*handle = &priv->stream_hdl;
		break;
	}
	case omx_index_media_get_dmx_sub_outport: {
		if (paramData) {
			OMX_S32 port_index;
			port_index = demuxer_sub_get_port_index(hComp,
				*((OMX_S32 *)paramData));
			if (port_index < 0) {
				OSCL_LOGE("get sub out port index error");
				ret = -1;
				break;
			} else
				*((OMX_S32 *)paramData) = port_index;
		}
		break;
	}
	default:
		ret = base_get_parameter(hComp, paramIndex, paramData);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("base get parameter %d error\n", paramIndex);
		break;
	}

	return ret;
}

static OMX_ERRORTYPE demuxer_set_parameter(OMX_IN OMX_HANDLETYPE  hComp,
	OMX_IN OMX_INDEXTYPE  paramIndex,
	OMX_IN OMX_PTR        paramData)
{
	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = component->component_private;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	switch ((OMX_U32)paramIndex) {
	case omx_index_vendor_input_filename: {
		if (priv->file_para.filename != NULL) {
			OSCL_LOGW("free filename %p first!", priv->file_para.filename);
			free(priv->file_para.filename);
			priv->file_para.filename = NULL;
		}
		priv->file_para.filename = oscl_strdup((char *)paramData);
		if (priv->file_para.filename == NULL) {
			OSCL_LOGE("alloc filename failed!");
			return OMX_ErrorInsufficientResources;
		}
		priv->file_para.io_intf.open = dmx_file_open;
		priv->file_para.io_intf.close = dmx_file_close;
		priv->file_para.io_intf.read = dmx_file_read;
		priv->file_para.io_intf.lseek = dmx_file_seek;
		OSCL_LOGI("set filename:%s", priv->file_para.filename);
		break;
	}
	case omx_index_media_io_interface: {
		omx_io_interface_t *intf = paramData;
		priv->file_para.io_intf.open = intf->open;
		priv->file_para.io_intf.close = intf->close;
		priv->file_para.io_intf.read = intf->read;
		priv->file_para.io_intf.lseek = intf->lseek;
		OSCL_LOGI("set io interface");
		break;
	}
	case OMX_IndexConfigTimePosition: {
		OMX_TIME_CONFIG_TIMESTAMPTYPE *timestamptype =
			(OMX_TIME_CONFIG_TIMESTAMPTYPE *)paramData;

		pthread_mutex_lock(&priv->mutex);
		priv->seek_flag = OMX_TRUE;
		priv->seek_time = timestamptype->nTimestamp * 1000;
		pthread_mutex_unlock(&priv->mutex);
		OSCL_LOGI("seek to time :%lld", priv->seek_time);
		break;
	}
	case omx_index_media_audio_track: {
		pthread_mutex_lock(&priv->mutex);
		priv->sw_atrack = *(int *)paramData;
		pthread_mutex_unlock(&priv->mutex);
		break;
	}
	case omx_index_stream_info: {
		omx_stream_info_t *si = (omx_stream_info_t *)paramData;
		dmx_mediainfo_t dmx_mi;
		external_stream_para_t sibuf;

		priv->muxer_type = si->muxer_type;
		if (priv->iostream != NULL) {
			OSCL_LOGW("iostream created already, maybe something wrong!");
			return -1;
		}

		if (priv->muxer_type == AV_MUXER_TYPE_RAW) {
			memset(&dmx_mi, 0, sizeof(dmx_mediainfo_t));

			if (si->vstream_num) {
				OSCL_LOGI("set video stream parameter");
				dmx_mi.vstream_num = 1;
				dmx_mi.content |= HAS_VIDEO;
				dmx_mi.default_vstream_idx = 0;
				dmx_mi.vinfo[0].stream_index = 0;
				dmx_mi.vinfo[0].codec_id = si->video_info.codec_id;
				dmx_mi.vinfo[0].width = si->video_info.width;
				dmx_mi.vinfo[0].height = si->video_info.height;
				if (dmx_mi.vinfo[0].codec_id == AV_CODEC_TYPE_INVALID) {
					OSCL_LOGI("invalid video codec id %x!",
						dmx_mi.vinfo[0].codec_id);
					return -1;
				}
			}

			if (si->astream_num) {
				OSCL_LOGI("set audio stream parameter");
				dmx_mi.astream_num = 1;
				dmx_mi.content |= HAS_AUDIO;
				dmx_mi.default_astream_idx = 0;
				dmx_mi.ainfo[0].stream_index = 1;
				dmx_mi.ainfo[0].codec_id = si->audio_info.codec_id;
				dmx_mi.ainfo[0].sample_rate = si->audio_info.sample_rate;
				dmx_mi.ainfo[0].channels = si->audio_info.channels;
				dmx_mi.ainfo[0].bits_per_coded_sample =
					si->audio_info.bitspersample;
				if (dmx_mi.ainfo[0].codec_id == AV_CODEC_TYPE_INVALID) {
					OSCL_LOGI("invalid audio codec id %x!",
						dmx_mi.ainfo[0].codec_id);
					return -1;
				}
			}
		}

		memset(&sibuf, 0, sizeof(external_stream_para_t));
		sibuf.align = 1;
		sibuf.size = DMX_VIDEO_BUFFER_SIZE + DMX_AUDIO_BUFFER_SIZE;
		priv->iostream = dmx_iostream_create(&sibuf, STREAM_TYPE_EXTERNAL);
		if (priv->iostream == NULL) {
			OSCL_LOGE("dmx_iostream_create error!");
			return -1;
		}
		if (priv->muxer_type == AV_MUXER_TYPE_RAW) {
			ret = dmx_iostream_filldata(priv->iostream, (uint8_t *)&dmx_mi,
				sizeof(dmx_mediainfo_t));
			if (ret < 0) {
				OSCL_LOGE("dmx_iostream_filldata error!");
				return -1;
			}
			OSCL_LOGD("filldata %d bytes", sizeof(dmx_mediainfo_t));
		}
		break;
	}
	case omx_index_media_sub_set_status_idx: {
		pthread_mutex_lock(&priv->mutex);
		if (paramData)
			ret = demuxer_sub_set_swstatus_index(hComp,
				*(OMX_S32 *)paramData);
		pthread_mutex_unlock(&priv->mutex);
		break;
	}
	case omx_index_media_enable_sub:
		pthread_mutex_lock(&priv->mutex);
		if (paramData)
			ret = demuxer_sub_enable(hComp, *(OMX_S32 *)paramData);
		pthread_mutex_unlock(&priv->mutex);
		break;
	case omx_index_media_disable_sub:
		pthread_mutex_lock(&priv->mutex);
		if (paramData)
			ret = demuxer_sub_disable(hComp, *(OMX_S32 *)paramData);
		pthread_mutex_unlock(&priv->mutex);
		break;
	case omx_index_media_add_exsub_url:
		ret = demuxer_add_exsub_fileurl(hComp, (OMX_STRING)paramData);
		break;
	case omx_index_media_sw_sub_status:
		pthread_mutex_lock(&priv->mutex);
		if (paramData)
			ret = demuxer_sub_change_status(hComp, *(int *)paramData);
		else {
			OSCL_LOGE("paramData is NULL");
			ret = -1;
		}
		pthread_mutex_unlock(&priv->mutex);
		break;
	case OMX_index_media_sub_Position:
		pthread_mutex_lock(&priv->mutex);
		ret = demuxer_seek_exsub(hComp, paramData);
		pthread_mutex_unlock(&priv->mutex);
		break;

	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("base set parameter %d error", paramIndex);
		break;
	}
	return ret;
}

static OMX_ERRORTYPE demuxer_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv;
	OMX_STATETYPE pre_state;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	priv = component->component_private;

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));

	if (dest_state == OMX_StateIdle && component->state == OMX_StateLoaded) {
		if (priv->iostream == NULL) {
			priv->iostream = dmx_iostream_create(&priv->file_para,
				STREAM_TYPE_FILE);
			if (priv->iostream == NULL) {
				OSCL_LOGE("dmx_iostream_create error!");
				goto EXIT;
			}
		} else {
			OSCL_LOGI("iostream was created already!");
		}
		if (priv->handle == NULL) {
			ret = demuxer_init(component);
			if (ret != 0) {
				OSCL_LOGE("demuxer_init error");
				dmx_iostream_destroy(priv->iostream);
				priv->iostream = NULL;
				goto EXIT;
			}
		} else {
			OSCL_LOGW("demuxer has be initialized already");
		}
		component->eos_flag = OMX_FALSE;
	}

	if (dest_state == OMX_StateExecuting && component->state == OMX_StatePause)
		OSCL_LOGD("port0 buffer %d, port1 %d\n",
			component->port[0].buffer_queue.count,
			component->port[1].buffer_queue.count);


	pre_state = component->state;
	if (dest_state == OMX_StateIdle && ((pre_state == OMX_StateExecuting) ||
		(pre_state == OMX_StatePause))) {
		demuxer_clear_queue_pkt(priv);
		demuxer_ctrl(priv->handle, DMX_CMD_BLOCKING, (void *)0);
		while (priv->reading_packet) {
			OSCL_LOGI("waiting read packet end");
			oscl_mdelay(10);
		}
		demuxer_ctrl(priv->handle, DMX_CMD_BLOCKING, (void *)1);
	}


	ret = base_component_set_state(hComp, dest_state);

	if (dest_state == OMX_StateLoaded && pre_state == OMX_StateIdle) {
		if (priv->handle != NULL) {
			ret = demuxer_deinit(component);
			if (ret != 0) {
				OSCL_LOGE("demuxer deinit error\n");
				goto EXIT;
			}
		} else {
			OSCL_LOGE("demuxer has not init yet");
		}
		if (priv->iostream != NULL) {
			dmx_iostream_destroy(priv->iostream);
			priv->iostream = NULL;
		}
	}

#if 0
	if (dest_state == OMX_StatePause && pre_state == OMX_StateExecuting) {
		demuxer_ctrl(priv->handle, DMX_CMD_BLOCKING, (void *)0);
		while (priv->reading_packet) {
			OSCL_LOGI("wait read packet end");
			oscl_mdelay(10);
		}
		demuxer_ctrl(priv->handle, DMX_CMD_BLOCKING, (void *)1);
	}
#endif

EXIT:
	return ret;
}

static OMX_ERRORTYPE demuxer_fill_this_buffer(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_BUFFERHEADERTYPE * header)
{
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);

	lb_omx_component_t *component = get_lb_component(hComp);
	demuxer_comp_priv_t *priv = (demuxer_comp_priv_t *)component->component_private;
	dmx_packet_t *pPkt = (dmx_packet_t *)header->pBuffer;
	int ret;

	if (pPkt->vir_addr != NULL && !(header->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
		OSCL_LOGD("header=%p, pbuffer=%p, port=%d, vir=%p",
			header, header->pBuffer, header->nOutputPortIndex,
			pPkt->vir_addr);
		ret = demuxer_return_packet(priv->handle, pPkt);
		if (ret != 0) {
			OSCL_LOGE("return packet error(ret=%d)", ret);
			ret = OMX_ErrorUndefined;
		}
	} else {
		OSCL_LOGD("header=%p, pbuffer=%p, port=%d", header, header->pBuffer,
			header->nOutputPortIndex);
	}

	header->nFlags = 0;
	header->nFilledLen = 0;
	header->nOffset = 0;
	header->nTimeStamp = 0;
	memset(header->pBuffer, 0, header->nAllocLen);
	ret = base_fill_this_buffer(hComp, header);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE demuxer_send_command(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_COMMANDTYPE cmd,
	OMX_IN OMX_U32 param1,
	OMX_IN OMX_PTR cmd_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv;
	OSCL_TRACE("%p, %x, %x , %p\n", cmp_hdl, cmd, param1, cmd_data);

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);
	oscl_param_check(component != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(component->state != OMX_StateInvalid,
		OMX_ErrorInvalidState, NULL);
	priv = component->component_private;

	switch (cmd) {
	case OMX_CommandFlush:
		if (priv->handle != NULL) {
			demuxer_ctrl(priv->handle, DMX_CMD_BLOCKING, (void *)0);
			while (priv->reading_packet) {
				OSCL_LOGI("wait read packet end");
				oscl_mdelay(10);
			}
			demuxer_ctrl(priv->handle, DMX_CMD_BLOCKING, (void *)1);
		}
		ret = base_send_command(cmp_hdl, cmd, param1, cmd_data);
		break;
	default:
		ret = base_send_command(cmp_hdl, cmd, param1, cmd_data);
		break;
	}

	OSCL_TRACE(" %d\n", ret);

	return ret;
}

static void *clkbuf_manager(void *param)
{
	lb_omx_component_t *comp = param;
	demuxer_comp_priv_t *priv = comp->component_private;
	base_port_t *clkport = &comp->port[DMX_CLOCK_PORT];
	OMX_BUFFERHEADERTYPE *clk_buffer = NULL;

	OSCL_LOGD("running");
	while (priv->clkbuf_thd_run) {
		sem_wait(&clkport->buffer_sem);
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		if (clk_buffer == NULL)
			continue;
		clkport->return_buffer(clkport, clk_buffer);
	}

	OSCL_LOGD("exit");
	return NULL;
}

static int clk_buf_init(lb_omx_component_t *comp)
{
	demuxer_comp_priv_t *priv = comp->component_private;
	int ret;

	priv->clkbuf_thd_run = OMX_TRUE;
	ret = pthread_create(&priv->clkbuf_thd, NULL, clkbuf_manager, comp);
	if (ret != 0) {
		OSCL_LOGE(" create clkbuf_manager error!");
		return -1;
	}

	return 0;
}

static void clk_buf_deinit(lb_omx_component_t *comp)
{
	demuxer_comp_priv_t *priv = comp->component_private;
	base_port_t *clkport = &comp->port[DMX_CLOCK_PORT];

	OSCL_LOGD("joining clkbuf_thd");
	priv->clkbuf_thd_run = OMX_FALSE;
	sem_post(&clkport->buffer_sem);
	pthread_join(priv->clkbuf_thd, NULL);
	OSCL_LOGD("clkbuf_thd joined");
}

static void *demuxer_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv;
	dmx_vinfo_t *vinfo = NULL;
	dmx_ainfo_t *ainfo = NULL;
	dmx_packet_t *pPkt = NULL, pkt;
	int port_index = -1;
	dmx_buffer_manager_t *mgr, a_mgr, v_mgr;
	OMX_BOOL send_audio_extra = OMX_FALSE, send_video_extra = OMX_FALSE;
	OMX_U8 *extradata = NULL;
	OMX_U32 extrasize = 0;
	DmxPktQueueMgr_t *vq_mgr = NULL, *aq_mgr = NULL;
	omx_mediainfo_t *omx_mi;
	dmx_mediainfo_t *mi;
	OMX_BOOL flushed = OMX_FALSE, pkt_valid = OMX_FALSE;
	OMX_U32 ret, i;
	OMX_S32 sub_track = -1;

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, "");

	component = get_lb_component(param);
	priv = (demuxer_comp_priv_t *)(component->component_private);
	omx_mi = &priv->omx_mediainfo;
	mi = &omx_mi->mediainfo;
	if (omx_mi->cur_video_idx >= 0) {
		vinfo = &mi->vinfo[omx_mi->cur_video_idx];
		vq_mgr = &priv->videoqueue[omx_mi->cur_video_idx];
		send_video_extra = OMX_FALSE;
		OSCL_LOGI("cur_video_idx=%d, stream_index=%d",
			omx_mi->cur_video_idx, vinfo->stream_index);
	}
	v_mgr.outport = &component->port[DMX_VIDEO_PORT];
	v_mgr.name = "video";
	v_mgr.pbuf = NULL;
	v_mgr.eos = OMX_FALSE;
	v_mgr.wait_buf_time = oscl_get_msec();

	if (omx_mi->cur_audio_idx >= 0) {
		ainfo = &mi->ainfo[omx_mi->cur_audio_idx];
		aq_mgr = &priv->audioqueue[omx_mi->cur_audio_idx];
		send_audio_extra = OMX_FALSE;
		OSCL_LOGI("cur_audio_idx=%d, stream_index=%d",
			omx_mi->cur_audio_idx, ainfo->stream_index);
	}
	a_mgr.outport = &component->port[DMX_AUDIO_PORT];
	a_mgr.name = "audio";
	a_mgr.pbuf = NULL;
	a_mgr.eos = OMX_FALSE;
	a_mgr.wait_buf_time = oscl_get_msec();

#ifdef DMX_SUPPORT_SUBTITLE
	DmxPktQueueMgr_t *sq_mgr = NULL;
	dmx_subinfo_t *sinfo = NULL;
	dmx_buffer_manager_t s_mgr[DMX_SUBTITLE_STREAM_NUM];
	OMX_BOOL send_sub_extra = OMX_FALSE;
	OMX_BOOL is_flush = OMX_FALSE;

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		s_mgr[i].outport = &component->port[DMX_SUB_PORT + i];
		s_mgr[i].pbuf = NULL;
		s_mgr[i].eos = OMX_FALSE;
		s_mgr[i].wait_buf_time = oscl_get_msec();
		s_mgr[i].name = "subtitle";
	}
#endif

	component->dbg_flag = set_debug_state(component->dbg_flag,
			DEBUG_BUF_MGNT_SHT, DEBUG_THREAD_START);

	OSCL_LOGI("%s buffer_manager", component->name);

	/* checks if the component is in a state able to receive buffers */
	while (1) {
		OMX_STATETYPE state = component->state;
		if (state != OMX_StateIdle
			&& state != OMX_StateExecuting
			&& state != OMX_StatePause) {
			OSCL_LOGD("comp-state=%d, state=%d, exit",
				component->state, state);
			break;
		}
		component->dbg_wdog = 0;

		/*Wait till the ports are being flushed*/
		pthread_mutex_lock(&component->flush_mutex);
#ifdef DMX_SUPPORT_SUBTITLE
		for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
			if (s_mgr[i].outport->is_flushed) {
				is_flush = OMX_TRUE;
				break;
			}
		}
#endif

		while (v_mgr.outport->is_flushed || a_mgr.outport->is_flushed
#ifdef DMX_SUPPORT_SUBTITLE
			|| is_flush
#endif
		) {
			pthread_mutex_unlock(&component->flush_mutex);
			if (v_mgr.pbuf != NULL && v_mgr.outport->is_flushed) {
				v_mgr.outport->return_buffer(v_mgr.outport, v_mgr.pbuf);
				v_mgr.pbuf = NULL;
				OSCL_LOGW("retrun video buffer while flushing port");
				if (pkt.media_type == AV_MEDIA_TYPE_VIDEO)
					flushed = OMX_TRUE;
			}

			if (a_mgr.pbuf != NULL && a_mgr.outport->is_flushed) {
				a_mgr.outport->return_buffer(a_mgr.outport, a_mgr.pbuf);
				a_mgr.pbuf = NULL;
				OSCL_LOGW("retrun audio buffer while flushing port");
				if (pkt.media_type == AV_MEDIA_TYPE_AUDIO)
					flushed = OMX_TRUE;
			}
#ifdef DMX_SUPPORT_SUBTITLE

			is_flush = OMX_FALSE;
			for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
				if (s_mgr[i].pbuf != NULL &&
					s_mgr[i].outport->is_flushed) {
					s_mgr[i].outport->return_buffer(s_mgr[i].outport,
						s_mgr[i].pbuf);
					s_mgr[i].pbuf = NULL;
					OSCL_LOGW("retrun sub buffer while flush port");
					if (pkt.media_type == AV_MEDIA_TYPE_SUBTITLE)
						flushed = OMX_TRUE;
				}
			}

#endif
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			pthread_mutex_lock(&component->flush_mutex);
#ifdef DMX_SUPPORT_SUBTITLE
			OSCL_LOGW("flush: audio %d, video %d, subtitle %d",
				component->port[DMX_AUDIO_PORT].buffer_queue.count,
				component->port[DMX_VIDEO_PORT].buffer_queue.count,
				component->port[DMX_SUB_PORT].buffer_queue.count);
#else
			OSCL_LOGW("flush: audio %d, video %d",
				component->port[DMX_AUDIO_PORT].buffer_queue.count,
				component->port[DMX_VIDEO_PORT].buffer_queue.count);
#endif
		}
		pthread_mutex_unlock(&component->flush_mutex);

		if (component->state != OMX_StateExecuting) {
			OSCL_LOGV("==========%d", sem_get(component->buf_mgnt_sem));
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		if (flushed) {
			if (pkt_valid) {
				OSCL_LOGI("return packet(%p) after flush", pkt.vir_addr);
				demuxer_return_packet(priv->handle, &pkt);
				pkt_valid = OMX_FALSE;
			}
			port_index = -1;
			flushed = OMX_FALSE;
		}

		pthread_mutex_lock(&priv->mutex);
		if (priv->seek_flag) {
			if (pkt_valid) {
				OSCL_LOGI("return packet before seek");
				demuxer_return_packet(priv->handle, &pkt);
				pkt_valid = OMX_FALSE;
			}
			OSCL_LOGD("clearing queue");
			demuxer_clear_queue_pkt(priv);
			ret = demuxer_seek(priv->handle, priv->seek_time);
			if (ret != 0) {
				OSCL_LOGE("demuxer_seek error(ret=%d)", ret);
				((component->callbacks.EventHandler))(
					&component->base_comp,
					component->callback_data,
					OMX_EventError,
					OMX_ErrorDmxReadSeek,
					-1,
					NULL);
			}
			priv->seek_flag = OMX_FALSE;
			component->eos_flag = v_mgr.eos = a_mgr.eos = OMX_FALSE;
			port_index = -1;
		}
		pthread_mutex_unlock(&priv->mutex);

		if (component->eos_flag) {
			if ((a_mgr.eos || omx_mi->cur_audio_idx < 0)
				&& (v_mgr.eos || omx_mi->cur_video_idx < 0)) {
				OSCL_LOGD("reach eos");
				oscl_mdelay(10);
				continue;
			}
		}

		pthread_mutex_lock(&priv->mutex);
		while (priv->sw_atrack >= 0) {
			if (pkt_valid) {
				if (pkt.stream_index != ainfo->stream_index)
					break;
				OSCL_LOGI("return packet before switch track");
				demuxer_return_packet(priv->handle, &pkt);
				pkt_valid = OMX_FALSE;
				port_index = -1;
			}
			if (priv->sw_atrack >= omx_mi->mediainfo.astream_num) {
				OSCL_LOGW("invalid track value %d", priv->sw_atrack);
				priv->sw_atrack = -1;
				break;
			}
			if (priv->sw_atrack == omx_mi->cur_audio_idx) {
				OSCL_LOGW("need not to switch track");
				priv->sw_atrack = -1;
				break;
			}
			if (!(omx_mi->audio_supported_flag & 1 << priv->sw_atrack)) {
				OSCL_LOGW("audio track %d is not supported!",
					priv->sw_atrack);
				priv->sw_atrack = -1;
				break;
			}
			base_port_t *clkport = &component->port[DMX_CLOCK_PORT];
			OMX_HANDLETYPE clkcmp = clkport->tunneled_component;
			OMX_TIME_CONFIG_TIMESTAMPTYPE curtime;
			DmxPktElem_t *elem;
			/* get cur mediatime */
			ret = OMX_GetConfig(clkcmp,
					OMX_IndexConfigTimeCurrentMediaTime,
					&curtime);
			if (ret != OMX_ErrorNone) {
				OSCL_LOGW("get cur mediatime err!");
				priv->sw_atrack = -1;
				break;
			}
			OSCL_LOGI("current media time %lld", curtime.nTimestamp);
			omx_mi->cur_audio_idx = priv->sw_atrack;
			priv->sw_atrack = -1;
			ainfo = &mi->ainfo[omx_mi->cur_audio_idx];
			aq_mgr = &priv->audioqueue[omx_mi->cur_audio_idx];
			while ((elem = _dmxPktQueue_pop(aq_mgr->valid_q)) != NULL) {
				if (elem->pkt.pts >= curtime.nTimestamp) {
					port_index = -1;
					pkt = elem->pkt;
					pkt_valid = OMX_TRUE;
					_dmxPktQueue_push(aq_mgr->free_q, elem);
					OSCL_LOGI("play time %lld", pkt.pts);
					break;
				}
				demuxer_return_packet(priv->handle, &elem->pkt);
				OSCL_LOGD("drop frame time %lld", elem->pkt.pts);
				_dmxPktQueue_push(aq_mgr->free_q, elem);
			}
			send_audio_extra = OMX_TRUE;
		}
		pthread_mutex_unlock(&priv->mutex);

#ifdef DMX_SUPPORT_SUBTITLE
		while (sub_track >= 0) {
			if (pkt_valid) {
				for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
					if (pkt.stream_index !=
						priv->sub.port[i].stream_idx)
						continue;
					else
						break;
				}
				if (i <  DMX_SUBTITLE_STREAM_NUM) {
					OSCL_LOGI("return packet before switch track");
					demuxer_return_packet(priv->handle, &pkt);
					pkt_valid = OMX_FALSE;
					port_index = -1;
				} else
					break;
			}
			ret = demuxer_sub_sw_strack(component, &port_index, &sq_mgr,
				sub_track, &pkt, &sinfo);
			if (!ret) {
				if (sinfo->extradata && sinfo->extradata_size) {
					port_index = -1;
					send_sub_extra = OMX_TRUE;
				}
				pkt_valid = OMX_TRUE;
				/* OSCL_LOGI("timestamp: %lld, %p, %s", pkt.pts,
					pkt.vir_addr, pkt.vir_addr); */
				sub_track = -1;
			} else if (ret == 2)
				break;
			else
				OSCL_LOGE("sub switch strack failed");
		}
#endif
		if (port_index == -1) {
			if (send_video_extra) {
				if (vinfo->extradata != NULL) {
					port_index = DMX_VIDEO_PORT;
					extradata = vinfo->extradata;
					extrasize = vinfo->extradata_size;
					OSCL_LOGI("video extradata size %d", extrasize);
				} else {
					extradata = NULL;
					extrasize = 0;
				}
				send_video_extra = OMX_FALSE;
			}
		}

		if (port_index == -1) {
			if (send_audio_extra) {
				if (ainfo->extradata != NULL) {
					port_index = DMX_AUDIO_PORT;
					extradata = ainfo->extradata;
					extrasize = ainfo->extradata_size;
					OSCL_LOGI("audio extradata size %d", extrasize);
				} else {
					extradata = NULL;
					extrasize = 0;
				}
				send_audio_extra = OMX_FALSE;
			}
		}

#ifdef DMX_SUPPORT_SUBTITLE
	if (port_index == -1) {
		if (send_sub_extra && sinfo) {
			OSCL_LOGI("sinfo->extradata: %p", sinfo->extradata);
			if (sinfo->extradata != NULL) {
				OMX_S32 idx = -1;
				dmx_subinfo_t *tmp = NULL;
				for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
					idx = priv->sub.port[i].sub_idx;
					tmp = &priv->omx_mediainfo.mediainfo.subinfo[idx];
					if (sinfo == tmp) {
						port_index = priv->sub.port[i].port_idx;
						break;
					}
				}
				extradata = sinfo->extradata;
				extrasize = sinfo->extradata_size;
				OSCL_LOGI("sub extradata size %d, %s",
					extrasize, extradata);
			} else {
				extradata = NULL;
				extrasize = 0;
			}
			send_sub_extra = OMX_FALSE;
		}
	}
#endif

		if (port_index == -1 && pkt_valid) {
			if (pkt.media_type == AV_MEDIA_TYPE_VIDEO)
				port_index = DMX_VIDEO_PORT;
			else if (pkt.media_type == AV_MEDIA_TYPE_AUDIO)
				port_index = DMX_AUDIO_PORT;
#ifdef DMX_SUPPORT_SUBTITLE
			else if (pkt.media_type == AV_MEDIA_TYPE_SUBTITLE) {
				for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
					if (pkt.stream_index ==
						priv->sub.port[i].stream_idx) {
						port_index = priv->sub.port[i].port_idx;
						break;
					}
				}
			}
#endif
		}

		if (port_index == -1 && aq_mgr != NULL) {
			DmxPktElem_t *elem = _dmxPktQueue_pop(aq_mgr->valid_q);
			if (elem != NULL) {
				pkt = elem->pkt;
				pkt_valid = OMX_TRUE;
				port_index = DMX_AUDIO_PORT;
				_dmxPktQueue_push(aq_mgr->free_q, elem);
			}
		}
		if (port_index == -1 && vq_mgr != NULL) {
			DmxPktElem_t *elem = _dmxPktQueue_pop(vq_mgr->valid_q);
			if (elem != NULL) {
				pkt = elem->pkt;
				pkt_valid = OMX_TRUE;
				port_index = DMX_VIDEO_PORT;
				_dmxPktQueue_push(vq_mgr->free_q, elem);
			}
		}
#ifdef DMX_SUPPORT_SUBTITLE
		if (port_index == -1 && sq_mgr != NULL) {
			DmxPktElem_t *elem = _dmxPktQueue_pop(sq_mgr->valid_q);
			if (elem != NULL) {
				/*OSCL_LOGI("there is data in queue");*/
				pkt = elem->pkt;
				for (OMX_S32 i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
					if (pkt.stream_index ==
						priv->sub.port[i].stream_idx) {
						pkt_valid = OMX_TRUE;
						port_index = priv->sub.port[i].port_idx;
						break;
					}
				}
				_dmxPktQueue_push(sq_mgr->free_q, elem);
			} else
				sq_mgr = NULL;
		} else  if ((sq_mgr == NULL) && (sub_track == -1)) {
			for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
				if (priv->sub.port[i].sw_strack >= 0) {
					sub_track = priv->sub.port[i].sw_strack;
					priv->sub.port[i].sw_strack = -1;
					break;
				}
			}
			if (sub_track >= 0)
				continue;
		}
#endif

		if (port_index == -1 && !pkt_valid) {
			memset(&pkt, 0, sizeof(dmx_packet_t));
			priv->reading_packet = OMX_TRUE;
			ret = demuxer_read_packet(priv->handle, &pkt);
			priv->reading_packet = OMX_FALSE;
			if (ret == DMX_ERRNO_NOBUF) {
				OSCL_LOGW("DMX_ERRNO_NOBUF");
				oscl_mdelay(10);
				continue;
			} else if (ret != 0 || (int)pkt.data_size < 0) {
				OSCL_LOGE("read packet error! ret=%d, size=%ld, flag=%x",
					ret, (unsigned long)pkt.data_size, pkt.flags);
				((component->callbacks.EventHandler))(
					&component->base_comp,
					component->callback_data,
					OMX_EventError,
					OMX_ErrorDmxReadPacket,
					-1,
					NULL);
				continue;
			}

			if (pkt.vir_addr == NULL && pkt.flags == 0) {
				OSCL_LOGW("pkt.vir_addr is NULL");
				oscl_mdelay(10);
				continue;
			}
			/*invalid subtitle*/
			pkt_valid = OMX_TRUE;
			if (pkt.flags & AV_PACKET_FLAG_RESET) {
				OSCL_LOGI("stream %d start timestamp %lld, %d",
					(OMX_S32)pkt.stream_index,
					(OMX_S64)pkt.pts, pkt.media_type);
			}
			if (pkt.flags & AV_PACKET_FLAG_EOS) {
				OSCL_LOGI("str%d EOS ts %lld",
					pkt.stream_index, pkt.pts);
			}
			if (vinfo != NULL && pkt.stream_index == vinfo->stream_index) {
				port_index = DMX_VIDEO_PORT;
				OSCL_LOGD("read video packet, len=%d, ts=%lld, flag=%08x",
					(int)pkt.data_size, (long long)pkt.pts,
					pkt.flags);
			} else if (ainfo != NULL &&
				pkt.stream_index == ainfo->stream_index) {
				port_index = DMX_AUDIO_PORT;
				OSCL_LOGD("read audio packet, len=%d, ts=%lld, flag=%08x",
					(int)pkt.data_size, (long long)pkt.pts,
					pkt.flags);
#ifdef DMX_SUPPORT_SUBTITLE
			} else if (!demuxer_sub_judge_stream_idx(&priv->sub,
				pkt.stream_index, &port_index)) {

				/*OSCL_LOGE("pts: %lld, dub idx: %d, subtitle: %s",
					pkt.pts, pkt.stream_index, pkt.vir_addr);
				port_index = DMX_SUB_PORT1;*/
				OSCL_LOGD("read sub packet, len=%d, ts=%lld, flag=%08x",
					pkt.data_size, pkt.pts, pkt.flags);
#endif
			} else {

				/*if external subtitle then return buffer directory*/
				if (pkt.stream_index >= EXSUB_START_STREAM_INDEX)
					demuxer_return_packet(priv->handle, &pkt);
				else
					_dmx_save_packet(priv, &pkt);
				port_index = -1;
				pkt_valid = OMX_FALSE;
				continue;
			}
		}

		if (port_index == DMX_VIDEO_PORT)
			mgr = &v_mgr;
		else if (port_index == DMX_AUDIO_PORT)
			mgr = &a_mgr;
#ifdef DMX_SUPPORT_SUBTITLE
		else if (port_index >= DMX_SUB_PORT)
			mgr = &s_mgr[port_index - DMX_SUB_PORT];
#endif
		else {
			OSCL_LOGE("fatal error, port_index=%d", port_index);
			break;
		}
		if (mgr->pbuf == NULL) {
			mgr->pbuf = oscl_queue_dequeue(&mgr->outport->buffer_queue);
			if (mgr->pbuf == NULL) {
				int64_t cur_time = oscl_get_msec();
				if (cur_time - mgr->wait_buf_time > 1000) {
					OSCL_LOGW("wait %s buffer for over %lld ms",
						mgr->name, cur_time - mgr->wait_buf_time);
					mgr->wait_buf_time = cur_time;
				}
				ret = oscl_sem_timedwait_ms(
						component->buf_mgnt_sem,
						1000);
				if (ret == -1) {
					if (errno == ETIMEDOUT)
						OSCL_LOGE("wait timeout!");
					else {
						OSCL_LOGE("wait error(%s)!",
							strerror(errno));
					}
				}
				continue;
			}
			pPkt = (dmx_packet_t *)mgr->pbuf->pBuffer;
			memset(pPkt, 0, sizeof(dmx_packet_t));
			mgr->wait_buf_time = oscl_get_msec();
		}

		if (extradata != NULL) {
			OSCL_LOGI("extradata size:%d", extrasize);
			pPkt->vir_addr = extradata;
			pPkt->phy_addr = (unsigned long)extradata;
			pPkt->data_size = extrasize;
			pPkt->flags = AV_PACKET_FLAG_EXTRADATA |
				AV_PACKET_FLAG_ENDOFFRAME;
			extradata = NULL;
			extrasize = 0;
			mgr->pbuf->nFlags = OMX_BUFFERFLAG_CODECCONFIG;
		} else {
			*pPkt = pkt;
			mgr->pbuf->nFlags = 0;
			pkt_valid = OMX_FALSE;
		}
		mgr->pbuf->nFilledLen = sizeof(dmx_packet_t);

		if ((pPkt->flags & AV_PACKET_FLAG_EOS) && (port_index < DMX_SUB_PORT)) {
			OSCL_LOGI("str-%s: EOS detected, size=%d, ts=%lld",
				mgr->name, pPkt->data_size, pPkt->pts);
			(*(component->callbacks.EventHandler))(&component->base_comp,
				component->callback_data,
				OMX_EventBufferFlag, /* The command was completed */
				mgr->outport->port_param.nPortIndex,
				OMX_BUFFERFLAG_EOS,
				NULL);
			component->eos_flag = mgr->eos = OMX_TRUE;
		}

		OSCL_LOGD("return: header=%p, pbuf=%p, len=%d; vir=%p, size=%d",
			mgr->pbuf, mgr->pbuf->pBuffer, mgr->pbuf->nFilledLen,
			pPkt->vir_addr, (int)pPkt->data_size);
		ret = mgr->outport->return_buffer(mgr->outport, mgr->pbuf);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGW("Failed to return buffer!");
			demuxer_fill_this_buffer(param, mgr->pbuf);
		}
		mgr->pbuf = NULL;
		port_index = -1;
	}

	oscl_queue_flush(&component->port[DMX_CLOCK_PORT].buffer_queue);
#ifdef DMX_SUPPORT_SUBTITLE
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++)
		oscl_queue_flush(&component->port[DMX_SUB_PORT + i].buffer_queue);
#endif
	oscl_queue_flush(&component->port[DMX_AUDIO_PORT].buffer_queue);
	oscl_queue_flush(&component->port[DMX_VIDEO_PORT].buffer_queue);
	OSCL_LOGI("exit from buffer_manager, state %s",
		omx_state_as_string(component->state));
	OSCL_TRACE(" %p\n", param);
	component->dbg_flag = set_debug_state(component->dbg_flag,
			DEBUG_BUF_MGNT_SHT, DEBUG_THREAD_EXIT);
	return NULL;
}


static int demuxer_stream_fill_data(void *hdl, int type,
	void *data, size_t size, int64_t timestamp)
{
	demuxer_comp_priv_t *priv = hdl;
	uint8_t index;
	int ret;

	if (priv->muxer_type == AV_MUXER_TYPE_RAW) {
		if (type == AV_MEDIA_TYPE_VIDEO) {
			index = 0;
			OSCL_LOGD("fill video stream, index=%d, size=%d, ts=%lld",
				index, size, timestamp);
		} else {
			index = 1;
			OSCL_LOGD("fill audio stream, index=%d, size=%d, ts=%lld",
				index, size, timestamp);
		}
		if (priv->iostream == NULL) {
			OSCL_LOGE("iostream not ready yet, set_ext_param first!");
			return -1;
		}

		ret = dmx_iostream_filldata(priv->iostream, &index, sizeof(uint8_t));
		if (ret < 0) {
			OSCL_LOGE("dmx_iostream_filldata error!");
			return -1;
		}

		ret = dmx_iostream_filldata(priv->iostream, (uint8_t *)&timestamp,
			sizeof(int64_t));
		if (ret < 0) {
			OSCL_LOGE("dmx_iostream_filldata error!");
			return -1;
		}

		ret = dmx_iostream_filldata(priv->iostream, (uint8_t *)&size,
			sizeof(size_t));
		if (ret < 0) {
			OSCL_LOGE("dmx_iostream_filldata error!");
			return -1;
		}
	}
	ret = dmx_iostream_filldata(priv->iostream, data, size);
	if (ret < 0) {
		OSCL_LOGE("dmx_iostream_filldata error!");
		return -1;
	}

	return 0;
}

static OMX_ERRORTYPE demuxer_component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	demuxer_comp_priv_t *priv = NULL;
	exsub_info_t *si = NULL;
	OMX_S32 i = 0;

	component = get_lb_component(hComp);
	priv = component->component_private;
	si = &priv->sub.ex.info;

	clk_buf_deinit(component);

	clk_buf_deinit(component);

	pthread_mutex_destroy(&priv->mutex);

	if (priv->file_para.filename != NULL) {
		oscl_free(priv->file_para.filename);
		priv->file_para.filename = NULL;
	}

	for (i = 0; i < si->stream_num; i++) {
		if (si)
			free(si->stream[i].text);
	}

	if (si)
		memset(si, 0x00, sizeof(exsub_info_t));

	if (component->component_private != NULL) {
		oscl_free(component->component_private);
		component->component_private = NULL;
	}
	clock_port_deinit(&component->port[DMX_CLOCK_PORT]);
#ifdef DMX_SUPPORT_SUBTITLE
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++)
		base_port_deinit(&component->port[DMX_SUB_PORT + i]);
#endif
	base_port_deinit(&component->port[DMX_AUDIO_PORT]);
	base_port_deinit(&component->port[DMX_VIDEO_PORT]);
	ret = base_component_deinit(hComp);
	if (ret != OMX_ErrorNone)
		OSCL_LOGE("base_component_deinit error\n");
	return ret;
}


OMX_ERRORTYPE demuxer_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	demuxer_comp_priv_t *priv = NULL;
	OMX_COMPONENTTYPE *base_cmp = &(cmp_handle->base_comp);
	base_port_t *port;
	OMX_PARAM_PORTDEFINITIONTYPE *param;
	OMX_S32 i = 0;

	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE(" base init component error\n");
		return ret;
	}

	priv = oscl_zalloc(sizeof(demuxer_comp_priv_t));
	if (priv == NULL) {
		OSCL_LOGE("alloc demuxer_private error\n");
		ret = OMX_ErrorInsufficientResources;
		goto ERR_ALLOC_PRIV;
	}
	memset(priv, 0, sizeof(demuxer_comp_priv_t));

	pthread_mutex_init(&priv->mutex, NULL);

	priv->stream_hdl.priv = priv;
	priv->stream_hdl.fill_data = demuxer_stream_fill_data;

	cmp_handle->name = "OMX.LB.SOURCE.DEMUXER";
	cmp_handle->component_private = (OMX_PTR)priv;
	cmp_handle->buf_manager = demuxer_buffer_manager;
	cmp_handle->do_state_set = demuxer_set_state;
#ifdef DMX_SUPPORT_SUBTITLE
	cmp_handle->num_ports = DMX_MAX_PORT_NUM + DMX_SUBTITLE_STREAM_NUM;
#else
	cmp_handle->num_ports = DMX_MAX_PORT_NUM;
#endif

	port = &cmp_handle->port[DMX_AUDIO_PORT];
	ret = demuxer_port_init(cmp_handle, port, DMX_AUDIO_PORT, OMX_DirOutput);
	if (ret != OMX_ErrorNone)
		goto ERR_INIT_APORT;
	param = &port->port_param;
	param->nBufferSize = sizeof(dmx_packet_t);
	param->eDomain = OMX_PortDomainAudio;
	param->nBufferAlignment = 0;
	param->bBuffersContiguous = OMX_FALSE;
	param->nBufferCountActual = DMX_AUDIO_BUF_NUM;

	port = &cmp_handle->port[DMX_VIDEO_PORT];
	ret = demuxer_port_init(cmp_handle, port, DMX_VIDEO_PORT, OMX_DirOutput);
	if (ret != OMX_ErrorNone)
		goto ERR_INIT_VPORT;
	param = &port->port_param;
	param->nBufferSize = sizeof(dmx_packet_t);
	param->eDomain = OMX_PortDomainVideo;
	param->nBufferAlignment = 0;
	param->bBuffersContiguous = OMX_FALSE;
	param->nBufferCountActual = DMX_VIDEO_BUF_NUM;

#ifdef DMX_SUPPORT_SUBTITLE
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {

		port = &cmp_handle->port[DMX_SUB_PORT + i];
		ret = demuxer_port_init(cmp_handle, port, DMX_SUB_PORT + i,
			OMX_DirOutput);
		if (ret != OMX_ErrorNone)
			goto ERR_INIT_SPORT;
		param = &port->port_param;
		param->nBufferSize = sizeof(dmx_packet_t);
		param->eDomain = OMX_PortDomainSubtitle;
		param->nBufferAlignment = 0;
		param->bBuffersContiguous = OMX_FALSE;
		param->nBufferCountActual = DMX_SUB_BUF_NUM;

		priv->sub.port[i].port_idx = DMX_SUB_PORT + i;
		priv->sub.port[i].stream_idx = -1;
		priv->sub.port[i].sub_idx = -1;
	}


#endif

	port = &cmp_handle->port[DMX_CLOCK_PORT];
	ret = clock_port_init(cmp_handle, port, DMX_CLOCK_PORT, OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("clock_port_init error!");
		goto ERR_INIT_CPORT;
	}

	base_cmp->SetParameter        = demuxer_set_parameter;
	base_cmp->GetParameter        = demuxer_get_parameter;
	base_cmp->ComponentDeInit     = demuxer_component_deinit;
	base_cmp->FillThisBuffer      = demuxer_fill_this_buffer;
	base_cmp->SendCommand         = demuxer_send_command;

	clk_buf_init(cmp_handle);

	return ret;

ERR_INIT_CPORT:
#ifdef DMX_SUPPORT_SUBTITLE
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++)
		base_port_deinit(&cmp_handle->port[DMX_SUB_PORT + i]);
ERR_INIT_SPORT:
	for (int j = 0; j < i; j++)
		base_port_deinit(&cmp_handle->port[DMX_SUB_PORT + j]);
#endif
	base_port_deinit(&cmp_handle->port[DMX_VIDEO_PORT]);
ERR_INIT_VPORT:
	base_port_deinit(&cmp_handle->port[DMX_AUDIO_PORT]);
ERR_INIT_APORT:
	pthread_mutex_destroy(&priv->mutex);
	oscl_free(priv);
ERR_ALLOC_PRIV:
	base_component_deinit(base_cmp);
	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = demuxer_component_init;
#endif


