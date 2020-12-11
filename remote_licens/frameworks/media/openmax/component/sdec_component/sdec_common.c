/*
 * sdec_component.c - demo code for sdec component.
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
#define DBG_LEVEL		DBG_INFO/* DBG_INFO */

#include <base_component.h>
#include "sdec_common.h"

#include "sdecode_com.h"
#include "demuxer_plugin.h"
#include "subdec.h"
#include "sub_common.h"
#include <oscl.h>
#include <stdio.h>
#include <dlfcn.h>
#include <oscl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct sdec_sux_t {
	char *suffix;
	av_codec_type_e id;
} sdec_sux_t;

static const sdec_sux_t sdec_uri[] = {
	/* subtitle probe */
	{"ass", AV_CODEC_TYPE_ASS},
	{"srt", AV_CODEC_TYPE_SRT},
	{"stl", AV_CODEC_TYPE_STL},
	{"lrc", AV_CODEC_TYPE_LRC},
	{"txt", AV_CODEC_TYPE_TXT},
	{"tts", AV_CODEC_TYPE_TTS},
	{"smi", AV_CODEC_TYPE_SMI},
	{"sub", AV_CODEC_TYPE_SUB},
	{"jss", AV_CODEC_TYPE_JSS},
	{"vsf", AV_CODEC_TYPE_VSF},
	{"asc", AV_CODEC_TYPE_ASC},
	{"ovr", AV_CODEC_TYPE_OVR},
	{"pgs", AV_CODEC_TYPE_PGS},
	{NULL, AV_CODEC_TYPE_INVALID}
};

typedef OMX_S32 (*get_plugin)(struct tag_sub_dec *pdec);


OMX_S32 sdec_frame(sdec_private_t *sub_private,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf)
{
	OMX_S32 ret = 0;
	OSCL_LOGD("sdec frame...");
	if (inbuf == NULL || inbuf->pBuffer == NULL) {
		OSCL_LOGE("inbuf NULL ERROR, inbuf=%p, pBuffer=%p",
			inbuf, inbuf == NULL ? NULL : inbuf->pBuffer);
		return -1;
	}
	if (outbuf == NULL || outbuf->pBuffer == NULL) {
		OSCL_LOGE("outbuf NULL ERROR, outbuf=%p, pBuffer=%p",
			outbuf, outbuf == NULL ? NULL : outbuf->pBuffer);
		return -1;
	}
	if (sub_private == NULL)
		return -1;
	dmx_packet_t *dmx_pkt = (dmx_packet_t *)inbuf->pBuffer;
	if (dmx_pkt->vir_addr == NULL)
		return -1;
	av_subtitle_t *sub = (av_subtitle_t *)(outbuf->pBuffer);
	memset(sub, 0, sizeof(av_subtitle_t));
	sub->info.index = sub_private->info.index;
	if ((dmx_pkt->flags & AV_PACKET_FLAG_EOS) && (dmx_pkt->data_size == 0)) {
		OSCL_LOGE("detect eos, don't handle, just pass to outbuf and return");
		sub->info.flags |= OMX_BUFFERFLAG_EOS;
		sub->info.pts = dmx_pkt->pts;
		sub->info.start_display_time = dmx_pkt->pts;
		sub->info.end_display_time = dmx_pkt->pts +
				dmx_pkt->duration;
		return 0;
	}

	/*OSCL_LOGI("sub start decode: %s", dmx_pkt->vir_addr);*/
	ret = sub_private->psub.decode(sub_private->decpara, sub,
			dmx_pkt, sub_private->info.sub_type);
	/*OSCL_LOGI("sub decode over: %s\n\n", sub->data.pdata);*/
	if (((dmx_pkt->pts == 0) && (dmx_pkt->duration == 0)) ||
		((dmx_pkt->flags & AV_PACKET_FLAG_EXTRADATA) ==
			AV_PACKET_FLAG_EXTRADATA)) {
#if 0
		OSCL_LOGE("read sdec style pkt: 0x%x, %p",
			dmx_pkt->flags, dmx_pkt->reserve);
#endif
		return ret;
	}
	outbuf->nFilledLen = sizeof(av_subtitle_t);
	if (dmx_pkt->pts >= 0) {
		outbuf->nTimeStamp = dmx_pkt->pts;
		sub->info.pts = dmx_pkt->pts;
		sub->info.start_display_time = dmx_pkt->pts;
		sub->info.end_display_time = dmx_pkt->duration +
				dmx_pkt->pts;
	} else {
		outbuf->nTimeStamp = 0;
		sub->info.pts = 0;
		sub->info.start_display_time = 0;
		sub->info.end_display_time = 0;
	}
	if (dmx_pkt->flags & AV_PACKET_FLAG_RESET) {
		sdec_flush(sub_private);
		OSCL_LOGI("FLAG: 0x%x", inbuf->nFlags);
		sub->info.flags |= inbuf->nFlags;
	}
	if ((dmx_pkt->flags & AV_PACKET_FLAG_EOS) &&
		(dmx_pkt->data_size <= 0)) {
		sub->info.flags |= OMX_BUFFERFLAG_EOS;
		inbuf->nFilledLen = 0;
	}

	return ret;
}


sdec_private_t *sdec_init(void)
{
	sdec_private_t *sub_private = NULL;
	sub_private = oscl_zalloc(sizeof(sdec_private_t));
	if (!sub_private) {
		OSCL_LOGE("malloc sdec_private_t error!\n");
		return NULL;
	}
	sub_private->para = oscl_zalloc(sizeof(AV_CodecParameters));
	if (!sub_private->para) {
		OSCL_LOGE("zalloc AV_CodecParameters error!\n");
		return NULL;
	}
	sub_private->para->codec_id = AV_CODEC_TYPE_ASS;
	return sub_private;
}

OMX_S32 sdec_deinit(sdec_private_t *sub_private)
{
	if (sub_private == NULL)
		return -1;
	if (sub_private) {
		if (sub_private->para) {
			if (sub_private->para->extradata) {
				oscl_free(sub_private->para->extradata);
				sub_private->para->extradata = NULL;
				sub_private->para->extradata_size = 0;
			}
			oscl_free(sub_private->para);
			sub_private->para = NULL;
		}
		oscl_free(sub_private);
	}
	return 0;
}

OMX_S32 sdec_return_frame(sdec_private_t *sub_private, av_subtitle_t *frame)
{
	OMX_S32 ret = 0;
	oscl_param_check((frame != NULL), -1, NULL);
	oscl_param_check((sub_private != NULL), -1, NULL);
	if (sub_private->psub.return_frame)
		ret = sub_private->psub.return_frame(sub_private->decpara, frame);

	return ret;

}
OMX_S32 sdec_open(sdec_private_t *sub_private)
{
	OSCL_LOGE("sdec open...");
	OMX_S8 libname[64] = "lib";
	get_plugin plugin_func;
	OMX_S32 i = 0;
	OMX_S32 ret = 0;
	if (sub_private == NULL)
		return -1;
	while (sdec_uri[i].id != AV_CODEC_TYPE_INVALID) {
		if (sdec_uri[i].id == sub_private->para->codec_id)
			break;
		i++;
	}
	if (sdec_uri[i].id == AV_CODEC_TYPE_INVALID) {
		OSCL_LOGE("Unsupport %d sub codec.",
			sub_private->para->codec_id);
		return -1;
	}

	strcat((char *)libname, (char *)sdec_uri[i].suffix);
	strcat((char *)libname, "dec.so");

	OSCL_LOGD("opening lib: %s", libname);
	sub_private->dl_hdl = dlopen((char *)libname, RTLD_LAZY);
	if (sub_private->dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", libname, dlerror());
		goto ERR_EXIT;
	}
	plugin_func = (get_plugin)dlsym(sub_private->dl_hdl, "get_sub_dec_plugin");
	if (plugin_func == NULL) {
		OSCL_LOGE("get_sub_dec_plugin error!");
		goto ERR_EXIT;
	}
	ret = plugin_func(&(sub_private->psub));
	if (ret < 0) {
		OSCL_LOGE("psub init error!");
		goto ERR_EXIT;
	}
	OSCL_LOGI("codec_id=0x%x", sub_private->para->codec_id);
	if (sub_private->para->extradata) {
		oscl_free(sub_private->para->extradata);
		sub_private->para->extradata = NULL;
		sub_private->para->extradata_size = 0;
	}
	return sub_private->psub.init(&(sub_private->decpara));

ERR_EXIT:
	if (sub_private->dl_hdl != NULL) {
		dlclose(sub_private->dl_hdl);
		sub_private->dl_hdl = NULL;
	}
	return -1;

}

OMX_S32 sdec_close(sdec_private_t *sub_private)
{
	OMX_S32 ret = 0;
	if (sub_private)
		return -1;
	ret =  sub_private->psub.close(sub_private->decpara);
	if (sub_private->dl_hdl != NULL) {
		dlclose(sub_private->dl_hdl);
		sub_private->dl_hdl = NULL;
	}
	return ret;

}

OMX_S32 sdec_flush(sdec_private_t *sub_private)
{
	OMX_S32 ret = 0;
	if (sub_private)
		return -1;
	ret = sub_private->psub.flush(sub_private->decpara);
	return ret;
}


