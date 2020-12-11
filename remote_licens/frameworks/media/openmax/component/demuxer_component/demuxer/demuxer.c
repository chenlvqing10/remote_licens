/*
 * demuxer.c - code for demuxer.
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
#define LOG_TAG           "demuxer"

#include <dirent.h>
#include <oscl.h>
#include <stdio.h>
#include <dlfcn.h>
#include <oscl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <av_media_type.h>
#include <demuxer_plugin.h>
#include <iostream_plugin.h>
#include "media_probe.h"
#include "demuxer.h"
#include "../dmx_iostream/file_interface.h"

/* #define DEBUG_DEMUXER_C */
/* #define DBG_DMX_SAVE_PKT "/mnt/usb1/test/ext_stream/dmxpkt.h264" */

#define IOSTREAM_LIB_NAME	"libiostream.so"
#define PTS_THRED_VALUE		(300*1000)/*300ms*/

typedef void *(*get_plugin)(void);

typedef struct exsubdmuxer_priv {
	void *parser;
	dmx_plugin_t *plugin;
	void *dl_hdl;
	void *io_ctx;

	OMX_S64 last_pts;
	OMX_S32 duration;
	OMX_S64 last_os_st;
	OMX_S32 cur_stream_idx;
	OMX_S8 eof_flag;
	subtitle_status_e status;
	pthread_mutex_t lock;
} exsubdmuxer_priv_t;

typedef struct demuxer_priv {
	void *parser;

	dmx_plugin_t *plugin;
	void *dl_hdl;

	iostream_plugin_t *io_plugin;
	void *io_dl_hdl;

	/*the handle para of subtitle*/
	exsubdmuxer_priv_t exsub[DMX_SUBTITLE_STREAM_NUM];
	char pkt_sw_flag;
	int64_t audio_last_pts;
#ifdef DEBUG_DEMUXER_C
	int hold_pkt_cnt[DMX_MAX_VIDEO_STREAM+DMX_MAX_AUDIO_STREAM+DMX_MAX_SUB_STREAM];
	int hold_size[DMX_MAX_VIDEO_STREAM+DMX_MAX_AUDIO_STREAM+DMX_MAX_SUB_STREAM];
	pthread_mutex_t lock;
#endif

#ifdef DBG_DMX_SAVE_PKT
	int pktfd;
#endif
} demuxer_priv_t;

static void demuxer_exsub_reset_param(void *handle)
{
	exsubdmuxer_priv_t *psub = NULL;
	demuxer_priv_t *dmx = handle;
	OMX_S32 i = 0;

	if (NULL == handle) {
		OSCL_LOGE("para is error");
		return;
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (dmx->exsub[i].status == sub_status_closed)
			continue;
		psub = &dmx->exsub[i];
		psub->last_pts = 0;
		psub->eof_flag = 0;
		psub->duration = 0;
		psub->last_os_st = 0;
	}
	dmx->pkt_sw_flag = 0;
	dmx->audio_last_pts = 0;

}
OMX_S32 demuxer_exsub_change_status(void *handle, OMX_S32 stream_idx,
	subtitle_status_e status)
{
	demuxer_priv_t *dmx = handle;
	OMX_S32 i = 0;

	if (NULL == handle) {
		OSCL_LOGE("para is error");
		return -1;
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (dmx->exsub[i].status == sub_status_closed)
			continue;
		if (stream_idx != dmx->exsub[i].cur_stream_idx)
			continue;
		dmx->exsub[i].status = status;
	}
	return 0;
}

OMX_STRING demuxer_exsub_probe(OMX_STRING file_url)
{
	OMX_STRING suffix = NULL;
	OMX_S8 libname[64] = "lib";
	void *dl_hdl = NULL;

	suffix = sub_probe(file_url, NULL);
	if (NULL == suffix)
		return NULL;
	strcat((char *)libname, suffix);
	strcat((char *)libname, "demuxer.so");

	OSCL_LOGI("opening demuxer lib: %s", (char *)libname);
	dl_hdl = dlopen((char *)libname, RTLD_LAZY);
	if (dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", (char *)libname, dlerror());
		return NULL;
	}
	dlclose(dl_hdl);

	memset((char *)libname, 0x00, 64);
	strcat((char *)libname, "lib");
	strcat((char *)libname, suffix);
	strcat((char *)libname, "dec.so");

	OSCL_LOGI("opening decoder lib: %s", (char *)libname);
	dl_hdl = dlopen((char *)libname, RTLD_LAZY);
	if (dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", (char *)libname, dlerror());
		return NULL;
	}
	dlclose(dl_hdl);

	return suffix;
}
static OMX_S32 demuxer_exsub_update_info(exsub_info_t *si, exsubdmuxer_priv_t *sub,
	OMX_S32 codec_id, OMX_STRING name)
{
	OMX_S32 i;

	if (NULL == si || NULL == sub) {
		OSCL_LOGE("para is NULL: %p, %p", sub, si);
		return -1;
	}

	if (codec_id == AV_CODEC_TYPE_INVALID) {
		OSCL_LOGE("the codec id is invalid");
		return -1;
	}

	for (i = 0; i < DMX_MAX_EXSUB_NUM; i++) {
		if (!strcmp(name, si->stream[i].text))
			break;
	}

	if (i >= DMX_MAX_EXSUB_NUM) {
		OSCL_LOGE("subtitle streams is full");
		return -1;
	}

	si->stream[i].codec_id = codec_id;
	si->stream[i].stream_index = i + EXSUB_START_STREAM_INDEX;
	sub->cur_stream_idx = si->stream[i].stream_index;
	OSCL_LOGE("sub->cur_stream_idx: %d", sub->cur_stream_idx);
	sub->status = sub_status_pause;

	return 0;
}

OMX_S32 demuxer_exsub_open(void *handle, OMX_STRING name,
	dmx_buffer_info_t *buf_info, exsub_info_t *si)
{
	OMX_S32 ret = 0;
	get_plugin plugin_func;
	OMX_S8 libname[64] = "lib";
	OMX_STRING suffix = NULL;
	demuxer_priv_t *dmx = (demuxer_priv_t *)handle;
	exsubdmuxer_priv_t *psub = NULL;
	OMX_S32 codec_id = AV_CODEC_TYPE_INVALID;
	OMX_S8 i;

	if (NULL == handle || NULL ==  name || NULL == si || NULL == buf_info) {
		OSCL_LOGE("para is NULL: %p, %p, %p, %p", handle, name, buf_info, si);
		return -1;
	}

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (dmx->exsub[i].status == sub_status_closed) {
			psub = &dmx->exsub[i];
			break;
		}
	}
	if (i >= DMX_SUBTITLE_STREAM_NUM) {
		OSCL_LOGE("all exsub hanle has been used");
		return -1;
	}
	pthread_mutex_init(&psub->lock, NULL);
	/* subtitle probe */
	suffix = sub_probe((OMX_STRING)name, &codec_id);
	if (NULL == suffix) {
		OSCL_LOGE("the subtitle file is not exist");
		goto ERR_EXIT;
	}
	strcat((char *)libname, (char *)suffix);
	strcat((char *)libname, "demuxer.so");

	/*open subtitle dll*/
	OSCL_LOGI("opening lib: %s", libname);
	psub->dl_hdl = dlopen((char *)libname, RTLD_LAZY);
	if (psub->dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", libname, dlerror());
		goto ERR_EXIT;
	}

	/*export function symbol*/
	plugin_func = (get_plugin)dlsym(psub->dl_hdl, "get_sub_plugin_info");
	if (plugin_func == NULL) {
		OSCL_LOGE("get sub plugin info error!");
		goto ERR_EXIT;
	} else
		OSCL_LOGE("get sub plugin info");
	psub->plugin = plugin_func();
	if (psub->plugin == NULL) {
		OSCL_LOGE("get sub plugin method error!");
		goto ERR_EXIT;
	}
	if (NULL == dmx->io_dl_hdl) {
		/*open subtitle io stream dll*/
		dmx->io_dl_hdl = dlopen(IOSTREAM_LIB_NAME, RTLD_LAZY);
		if (dmx->io_dl_hdl == NULL) {
			OSCL_LOGE("Failed to open lib: %s(%s)",
				IOSTREAM_LIB_NAME, dlerror());
			goto ERR_EXIT;
		}

		/*export io stream function symbol*/
		plugin_func = (get_plugin)dlsym(dmx->io_dl_hdl, "get_plugin_info");
		if (plugin_func == NULL) {
			OSCL_LOGE("io get sub plugin info error!");
			goto ERR_EXIT;
		}
		dmx->io_plugin = plugin_func();
		if (dmx->io_plugin == NULL) {
			OSCL_LOGE("io get sub plugin method error!");
			goto ERR_EXIT;
		}
	}

	file_stream_para_t para;
	para.filename = name;
	para.io_intf.open = dmx_file_open;
	para.io_intf.close = dmx_file_close;
	para.io_intf.read = dmx_file_read;
	para.io_intf.lseek = dmx_file_seek;
	/*open subtitle file*/
	psub->io_ctx = dmx->io_plugin->create(&para, STREAM_TYPE_FILE);
	if (psub->io_ctx == NULL) {
		OSCL_LOGE("create io stream error!");
		goto ERR_EXIT;
	}

	/*read subtiltle file*/
	psub->parser = psub->plugin->read_header(psub->io_ctx, buf_info, NULL);
	if (psub->parser == NULL) {
		OSCL_LOGE("read header error!");
		goto ERR_EXIT;
	}

	ret = demuxer_exsub_update_info(si, psub, codec_id, name);
	if (ret == -1) {
		OSCL_LOGE("update subtitle media info failed");
		goto ERR_EXIT;
	}

	return 0;

ERR_EXIT:
	if (psub->io_ctx != NULL)
		dmx->io_plugin->destroy(psub->io_ctx);
	if (psub->dl_hdl != NULL)
		dlclose(psub->dl_hdl);
	if (dmx->io_dl_hdl)
		dlclose(dmx->io_dl_hdl);
	dmx->io_dl_hdl = NULL;
	memset((char *)psub, 0x00, sizeof(exsubdmuxer_priv_t));

	return -1;
}
OMX_S32 demuxer_exsub_read_pkt(void *handle, dmx_packet_t *pkt)
{
	demuxer_priv_t *dmx = handle;
	exsubdmuxer_priv_t *psub = NULL;
	OMX_S64 timeout;
	OMX_S32 ret = 0;
	static OMX_S32 exsub_idx;
	OMX_S32 i = 0;
	if (dmx == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}
	/*OSCL_LOGI("read exsub here: %d", dmx->pkt_sw_flag);*/

	exsub_idx++;
	i = exsub_idx % DMX_SUBTITLE_STREAM_NUM;
	psub = &dmx->exsub[i];
	if ((psub->status != sub_status_opened) ||
		(psub->eof_flag == AV_PACKET_FLAG_EOS)) {
		psub = &dmx->exsub[(i + 1) % DMX_SUBTITLE_STREAM_NUM];
		if ((psub->status != sub_status_opened) ||
			(psub->eof_flag == AV_PACKET_FLAG_EOS)) {
			/*OSCL_LOGE("exsub not be opened");*/
			return -1;
		}
	}
	pthread_mutex_lock(&psub->lock);

	if (psub->last_pts && (dmx->audio_last_pts > 0)) {
		/*for video file*/
		/*if last sub pts is not 0,then read sub pkt by pts and duration*/
		timeout = psub->last_pts + psub->duration - PTS_THRED_VALUE;
		if (dmx->audio_last_pts >= timeout) {
			/*OSCL_LOGE("demuxer->audio_last_pts: %lld--%lld",
				dmx->audio_last_pts, timeout);*/
			/*read subtitle pkt, if end then read video pkt*/
			ret = psub->plugin->read_packet(psub->parser, pkt);
			if (!ret) {
				psub->last_pts = pkt->pts;
				psub->duration = pkt->duration;
				pkt->stream_index = psub->cur_stream_idx;
			} else {
				if (pkt->flags == AV_PACKET_FLAG_EOS)
					psub->eof_flag = AV_PACKET_FLAG_EOS;
				memset(pkt, 0x00, sizeof(dmx_packet_t));
				OSCL_LOGE("read pkt failed00");
				goto error;
			}
		} else
			goto error;
	} else {
		/*for music file*/
		/*if last sub pts is 0,then read sub pkt and video pkt one by one*/
		if (!dmx->pkt_sw_flag) {
			dmx->pkt_sw_flag = 1;
			OMX_S64 st = oscl_get_usec();
			timeout = psub->duration -  PTS_THRED_VALUE/3;
			if ((timeout > 0) && (timeout > (st - psub->last_os_st)))
				goto error;
			ret = psub->plugin->read_packet(psub->parser, pkt);
			if (!ret) {
				/*OSCL_LOGE("PKT->FLAG: %d, %d", pkt->flags,
					psub->cur_stream_idx);*/
				pkt->stream_index = psub->cur_stream_idx;
				psub->last_pts = pkt->pts;
				psub->duration = pkt->duration;
				psub->last_os_st = oscl_get_usec();
				if (pkt->flags == AV_PACKET_FLAG_EOS)
					psub->eof_flag = AV_PACKET_FLAG_EOS;
			} else {
				if (pkt->flags == AV_PACKET_FLAG_EOS)
					psub->eof_flag = AV_PACKET_FLAG_EOS;
				memset(pkt, 0x00, sizeof(dmx_packet_t));
				OSCL_LOGE("read pkt failed11");
				goto error;
			}
		} else
			goto error;
	}

#if 0
	OSCL_LOGE("read_subtitle ok: %d, %lld, %d, %s", psub->cur_stream_idx,
		pkt->pts, *((OMX_S32 *)pkt->reserve), pkt->vir_addr);
#endif
	pthread_mutex_unlock(&psub->lock);
	return 0;

error:
	pthread_mutex_unlock(&psub->lock);
	return -1;
}
OMX_S32 demuxer_exsub_return_pkt(void *handle, dmx_packet_t *pkt)
{
	demuxer_priv_t *dmx = (demuxer_priv_t *)handle;
	exsubdmuxer_priv_t *psub = NULL;
	OMX_S32 ret = -1;
	OMX_S32 i;

	if (dmx == NULL) {
		OSCL_LOGE("handle para is NULL");
		return -1;
	}

	if (pkt->stream_index == dmx->exsub[0].cur_stream_idx)
		i = 0;
	else if (pkt->stream_index == dmx->exsub[1].cur_stream_idx)
		i = 1;
	else {
		OSCL_LOGE("exsub idx is error: %d, %d, %d", pkt->stream_index,
			dmx->exsub[0].cur_stream_idx,
			dmx->exsub[1].cur_stream_idx);
		return 0;
	}
	psub = &dmx->exsub[i];
	ret = psub->plugin->return_packet(psub->parser, pkt);
	if (ret) {
		OSCL_LOGE("set ctrl cmd failed");
		ret = -1;
	}

	return ret;
}

OMX_S32 demuxer_exsub_seek(void *handle, OMX_S64 st, OMX_S32 stream_idx)
{
	demuxer_priv_t *dmx = (demuxer_priv_t *)handle;
	exsubdmuxer_priv_t *psub = NULL;
	OMX_S32 ret = 0;
	OMX_S32 i;

	if (dmx == NULL) {
		OSCL_LOGE("handle para is NULL");
		return -1;
	}

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (dmx->exsub[i].status == sub_status_closed)
			continue;
		psub = &dmx->exsub[i];
		if ((stream_idx >= 0) && (stream_idx != psub->cur_stream_idx))
			continue;
		ret = psub->plugin->read_seek(psub->parser, st, DMX_SEEKDIR_ANY);
		if (ret) {
			OSCL_LOGE("seek new subtitle file failed");
			ret = -1;
		}
	}

	return ret;
}
OMX_S32 demuxer_exsub_ctrl(void *handle, int cmd, void *arg)
{
	demuxer_priv_t *dmx = (demuxer_priv_t *)handle;
	exsubdmuxer_priv_t *psub = NULL;
	OMX_S32 ret = -1;
	OMX_S32 i;

	if (dmx == NULL) {
		OSCL_LOGE("handle para is NULL");
		return -1;
	}

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		if (dmx->exsub[i].status == sub_status_closed)
			continue;
		psub = &dmx->exsub[i];

		ret = psub->plugin->read_ctrl(psub->parser, cmd, arg);
		if (ret) {
			OSCL_LOGE("set ctrl cmd failed");
			ret = -1;
		}
	}

	return ret;
}

OMX_S32 demuxer_exsub_close(void *handle, exsub_info_t *si,
	subtitle_close_e type, OMX_S32 exsub_idx)
{
	demuxer_priv_t *demuxer = handle;
	OMX_S32 i;
	exsubdmuxer_priv_t *psub = NULL;

	if (demuxer == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}

	switch (type) {
	case close_subtile_all:
		/*close all exsub handle*/
		for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
			psub = &demuxer->exsub[i];
			if (psub->status == sub_status_closed)
				continue;
			pthread_mutex_lock(&psub->lock);
			if (psub->plugin && psub->parser)
				psub->plugin->read_close(psub->parser);
			if (psub->io_ctx != NULL)
				demuxer->io_plugin->destroy(psub->io_ctx);
			if (psub->dl_hdl != NULL)
				dlclose(psub->dl_hdl);
			pthread_mutex_unlock(&psub->lock);
			pthread_mutex_destroy(&psub->lock);
			memset((char *)psub, 0x00, sizeof(exsubdmuxer_priv_t));
		}
		if (demuxer->io_dl_hdl)
			dlclose(demuxer->io_dl_hdl);
		demuxer->io_dl_hdl = NULL;
		if (NULL == si)
			break;
		/* clear external subtitle  info*/
		for (i = 0; i < DMX_MAX_EXSUB_NUM; i++) {
			if (si->stream[i].codec_id == AV_CODEC_TYPE_INVALID)
				continue;
			si->stream[i].codec_id = AV_CODEC_TYPE_INVALID;
			si->stream[i].stream_index = -1;
		}
		break;

	case close_subtile_one_way:
		for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
			psub = &demuxer->exsub[i];
			if ((psub->status != sub_status_closed) &&
			   (psub->cur_stream_idx == exsub_idx + EXSUB_START_STREAM_INDEX))
				break;
		}
		if (i >= DMX_SUBTITLE_STREAM_NUM)
			break;
		pthread_mutex_lock(&psub->lock);
		if (psub->plugin && psub->parser)
			psub->plugin->read_close(psub->parser);
		if (psub->io_ctx != NULL)
			demuxer->io_plugin->destroy(psub->io_ctx);
		if (psub->dl_hdl != NULL)
			dlclose(psub->dl_hdl);
		pthread_mutex_unlock(&psub->lock);
		pthread_mutex_destroy(&psub->lock);
		if (NULL == si)
			break;
		for (i = 0; i < DMX_MAX_EXSUB_NUM; i++) {
			if (si->stream[i].stream_index != psub->cur_stream_idx)
				continue;
			OSCL_LOGI("i: %d, idx: %d, name: %s", i,
				si->stream[i].stream_index,
				si->stream[i].text);
			si->stream[i].codec_id = AV_CODEC_TYPE_INVALID;
			si->stream[i].stream_index = -1;
		}
		memset((char *)psub, 0x00, sizeof(exsubdmuxer_priv_t));
		break;
	}


	return 0;
}

void *demuxer_open(file_stream_para_t *para, void *ioctx, int muxer_type,
	dmx_buffer_info_t *buf_info, dmx_mediainfo_t *mediainfo)
{
	demuxer_priv_t *demuxer;
	char libname[64] = "lib";
	get_plugin plugin_func;

	demuxer = oscl_zalloc(sizeof(demuxer_priv_t));
	if (demuxer == NULL) {
		OSCL_LOGE("alloc demuxer failed!");
		return NULL;
	}
	if (muxer_type == AV_MUXER_TYPE_RAW) {
		strcat(libname, "rawstream");
	} else if (muxer_type == AV_MUXER_TYPE_TS) {
		strcat(libname, "ts");
	} else {
		char *suffix;
		suffix = media_probe(para);
		if (suffix == NULL) {
			OSCL_LOGE("probe error, file not support!");
			goto ERR_EXIT;

		}
		strcat(libname, suffix);
		memset(mediainfo, 0, sizeof(dmx_mediainfo_t));
	}
	strcat(libname, "demuxer.so");

	OSCL_LOGI("opening lib: %s", libname);
	demuxer->dl_hdl = dlopen(libname, RTLD_LAZY);
	if (demuxer->dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", libname, dlerror());
		goto ERR_EXIT;
	}
	plugin_func = (get_plugin)dlsym(demuxer->dl_hdl, "get_plugin_info");
	if (plugin_func == NULL) {
		OSCL_LOGE("get plugin info error!");
		goto ERR_EXIT;
	}
	demuxer->plugin = plugin_func();
	if (demuxer->plugin == NULL) {
		OSCL_LOGE("get plugin method error!");
		goto ERR_EXIT;
	}

	demuxer->parser = demuxer->plugin->read_header(ioctx, buf_info, mediainfo);
	if (demuxer->parser == NULL) {
		OSCL_LOGE("read header error!");
		goto ERR_EXIT;
	}

#ifdef DEBUG_DEMUXER_C
	pthread_mutex_init(&demuxer->lock, NULL);
#endif
#ifdef DBG_DMX_SAVE_PKT
	demuxer->pktfd = open(DBG_DMX_SAVE_PKT, O_RDWR | O_CREAT, 0666);
	if (demuxer->pktfd < 0)
		OSCL_LOGE("open packet file error!");
#endif

	return demuxer;

ERR_EXIT:
	if (demuxer->dl_hdl != NULL)
		dlclose(demuxer->dl_hdl);
	oscl_free(demuxer);
	return NULL;
}

int demuxer_close(void *handle)
{
	demuxer_priv_t *demuxer = handle;
	int ret = 0;

	if (demuxer == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}

	demuxer->plugin->read_close(demuxer->parser);
	if (demuxer->dl_hdl != NULL)
		dlclose(demuxer->dl_hdl);
#ifdef DBG_DMX_SAVE_PKT
	close(demuxer->pktfd);
#endif
#ifdef DEBUG_DEMUXER_C
	pthread_mutex_destroy(&demuxer->lock);
#endif

	ret = demuxer_exsub_close(demuxer, NULL, close_subtile_all, 0);

	oscl_free(demuxer);

	return ret;
}

int demuxer_return_packet(void *handle, dmx_packet_t *pkt)
{
	demuxer_priv_t *demuxer = handle;

	if (demuxer == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}

#ifdef DEBUG_DEMUXER_C
	pthread_mutex_lock(&demuxer->lock);
	demuxer->hold_pkt_cnt[pkt->stream_index]--;
	int sz;
	if (pkt->media_type == AV_MEDIA_TYPE_VIDEO)
		sz = ALIGN_4K(pkt->data_size);
	else
		sz = pkt->data_size;
	demuxer->hold_size[pkt->stream_index] -= sz;
	OSCL_LOGI("str%d hold %d packets %d bytes: ts=%lld, size=%d, vir=%p",
		pkt->stream_index, demuxer->hold_pkt_cnt[pkt->stream_index],
		demuxer->hold_size[pkt->stream_index],
		pkt->pts, pkt->data_size, pkt->vir_addr);
	pthread_mutex_unlock(&demuxer->lock);
#endif
	if (pkt->stream_index < EXSUB_START_STREAM_INDEX)
		return demuxer->plugin->return_packet(demuxer->parser, pkt);
	else if (pkt->media_type == AV_MEDIA_TYPE_SUBTITLE)
		return demuxer_exsub_return_pkt(handle, pkt);

	return 0;
}

int demuxer_read_packet(void *handle, dmx_packet_t *pkt)
{
	demuxer_priv_t *demuxer = handle;
	int32_t ret = 0;

	if (demuxer == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}
	if (pkt == NULL) {
		OSCL_LOGE("pkt==NULL!");
		return -1;
	}

#ifdef DEBUG_DEMUXER_C
	OSCL_LOGI("start to read pkt");
#endif
	ret = demuxer_exsub_read_pkt(handle, pkt);
	if (ret) {
		ret = demuxer->plugin->read_packet(demuxer->parser, pkt);
		if (pkt->media_type == AV_MEDIA_TYPE_AUDIO)
			demuxer->audio_last_pts = pkt->pts;
		demuxer->pkt_sw_flag = 0;
	}
#ifdef DEBUG_DEMUXER_C
	pthread_mutex_lock(&demuxer->lock);
	if (ret == 0) {
		demuxer->hold_pkt_cnt[pkt->stream_index]++;
		int sz;
		if (pkt->media_type == AV_MEDIA_TYPE_VIDEO)
			sz = ALIGN_4K(pkt->data_size);
		else
			sz = pkt->data_size;
		demuxer->hold_size[pkt->stream_index] += sz;
	}
	OSCL_LOGI("str%d hold %d packets %d bytes:ts=%lld,size=%d,vir=%p,ret=%d",
		pkt->stream_index, demuxer->hold_pkt_cnt[pkt->stream_index],
		demuxer->hold_size[pkt->stream_index],
		(long long)pkt->pts, (int)pkt->data_size,
		pkt->vir_addr, ret);
	pthread_mutex_unlock(&demuxer->lock);
#endif

#ifdef DBG_DMX_SAVE_PKT
	if (write(demuxer->pktfd, pkt->vir_addr, pkt->data_size) != pkt->data_size) {
		OSCL_LOGE("write packet file %d bytes error!", pkt->data_size);
	}
#endif
	return ret;
}

int demuxer_seek(void *handle, int64_t timestamp)
{
	demuxer_priv_t *demuxer = handle;
	int ret = 0;

	if (demuxer == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}

	OSCL_LOGI("seek to %lld", (long long)timestamp);
	ret = demuxer->plugin->read_seek(demuxer->parser, timestamp, DMX_SEEKDIR_ANY);
	if (ret) {
		OSCL_LOGE("seek video file failed");
		return ret;
	}
	ret = demuxer_exsub_seek(demuxer, timestamp, -1);
	if (ret) {
		OSCL_LOGE("seek ex subtitle failed");
		return ret;
	}
	OSCL_LOGI("seek end, ret=%d", ret);
	demuxer_exsub_reset_param(handle);

	return ret;
}

int demuxer_ctrl(void *handle, int cmd, void *arg)
{
	demuxer_priv_t *demuxer = handle;
	OMX_S32 ret = -1;

	if (demuxer == NULL) {
		OSCL_LOGE("demuxer==NULL!");
		return -1;
	}
	ret = demuxer->plugin->read_ctrl(demuxer->parser, cmd, arg);
	if (ret)
		return ret;
	ret = demuxer_exsub_ctrl(handle, cmd, arg);

	return ret;
}

