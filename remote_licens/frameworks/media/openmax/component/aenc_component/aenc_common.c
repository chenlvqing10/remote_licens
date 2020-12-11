/*
 * aenc_component.c - demo code for aenc component.
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
#include <dlfcn.h>
#include <oscl.h>
#include <base_component.h>
#include "aenc_common.h"
#include "aenc_lib.h"

/* #define PCMFILE */
#ifdef PCMFILE
static int pcmfd;
#define PCMNAME "test.pcm"
#endif

typedef struct aenc_sux_t {
	char *suffix;
	av_codec_type_e id;
} aenc_sux_t;

static const  aenc_sux_t aenc_uri[] = {
	/* audio probe */
	{"pcm", AV_CODEC_TYPE_PCM_S16LE},
	{"wav", AV_CODEC_TYPE_ADPCM_IMA_WAV},
	{"aac", AV_CODEC_TYPE_AAC},
	{"g729", AV_CODEC_TYPE_G729},
	{NULL, AV_CODEC_TYPE_INVALID}
};

typedef int (*get_plugin)(struct  tag_audio_dec *penc);

int aenc_frame(aenc_private_t *audio_private,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf)
{
	int ret = 0;
	packet_t packet_in;
	packet_t packet_out;

	if (inbuf) { /* OSCL_LOGE("***********************inbuf"); */
		if (inbuf->nFilledLen && inbuf->pBuffer) {
			ret = writePcmData(inbuf->pBuffer + inbuf->nOffset,
					inbuf->nFilledLen, audio_private->pcm_manager);
			if (ret > 0)
				inbuf->nFilledLen -= ret;
		}
		if (inbuf->nFlags & OMX_BUFFERFLAG_EOS) {
			OSCL_LOGE("adv_buf get the end of stream\n");
			audio_private->pcm_manager->nFlags |= OMX_BUFFERFLAG_EOS;
		}
	}
	if (outbuf == NULL || outbuf->pBuffer == NULL)
		return 0;
	outbuf->nFilledLen = 0;
	ret = readPcmData(audio_private->pcm_buf, audio_private->paudio.para->frame_size
			* audio_private->paudio.para->channels * 2,
			audio_private->pcm_manager);
	if (ret > 0) {
		if (audio_private->paudio.para->channels > 0)
			packet_in.size = (ret >> 1) /
				audio_private->paudio.para->channels;

		packet_out.data = outbuf->pBuffer + outbuf->nOffset;
		packet_in.data = audio_private->pcm_buf;
#ifdef PCMFILE
		if (pcmfd > 0) {
			int len = write(pcmfd, audio_private->pcm_buf, ret);
			if (len != ret)
				OSCL_LOGE("write err:ret:%d,len:%d\n", ret, len);
		}
#endif

		ret =  audio_private->paudio.decode(&audio_private->paudio,
				&packet_in, &packet_out);
		/* OSCL_LOGE("dec size:%d,ret:%d,frame:%lld",
		packet_out.size, ret, audio_private->nFrames); */
		if ((audio_private->pcm_manager->nFlags & OMX_BUFFERFLAG_EOS) &&
			(!audio_private->pcm_manager->bufDataLen)) {
			OSCL_LOGE("aenc_buf_handle get the end of stream\n");
			outbuf->nFlags |= OMX_BUFFERFLAG_EOS;
		}
		outbuf->nOffset = 0;
		outbuf->nFilledLen = packet_out.size;
		outbuf->nTimeStamp = audio_private->nFrames;
		audio_private->nFrames += packet_in.size;
	} else {
		OSCL_LOGD("ret:%d,", ret);
	}

	return ret;
}

int aenc_open(aenc_private_t *audio_private)
{
	char libname[64] = "lib";
	get_plugin plugin_func;
	int i = 0;

	audio_private->nFrames = 0;
	pcm_buf_reset(audio_private->pcm_manager);

	while (aenc_uri[i].id != AV_CODEC_TYPE_INVALID) {
		if (aenc_uri[i].id == audio_private->acode)
			break;
		i++;
	}
	strcat(libname, aenc_uri[i].suffix);
	strcat(libname, "enc.so");

	OSCL_LOGI("opening lib: %s", libname);
	audio_private->dl_hdl = dlopen(libname, RTLD_LAZY);
	if (audio_private->dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s(%s)", libname, dlerror());
		goto ERR_EXIT;
	}
	plugin_func = (get_plugin)dlsym(audio_private->dl_hdl, "get_plugin_info");
	if (plugin_func == NULL) {
		OSCL_LOGE("get plugin info error!");
		goto ERR_EXIT;
	}
	plugin_func(&(audio_private->paudio));

	OSCL_LOGI("acode:%d,!!!!", audio_private->acode);

	return  audio_private->paudio.init(&(audio_private->paudio));

ERR_EXIT:
	if (audio_private->dl_hdl != NULL) {
		dlclose(audio_private->dl_hdl);
		audio_private->dl_hdl = NULL;
	}
	return -1;
}

int aenc_close(aenc_private_t *audio_private)
{
	int ret = 0;
	if (audio_private->paudio.close)
		ret = audio_private->paudio.close(&(audio_private->paudio));

	if (audio_private->dl_hdl != NULL) {
		dlclose(audio_private->dl_hdl);
		audio_private->dl_hdl = NULL;
	}

	return ret;
}

int aenc_deinit(aenc_private_t *audio_private)
{
	if (audio_private) {
		if (audio_private->paudio.para) {
			oscl_free(audio_private->paudio.para);
			audio_private->paudio.para = NULL;
		}
		pcm_buf_deinit(audio_private->pcm_manager);
		audio_private->pcm_manager = NULL;
		oscl_free(audio_private);
	}

#ifdef PCMFILE
	if (pcmfd > 0) {
		close(pcmfd);
		OSCL_LOGE("close pcm file :%d\n", pcmfd);
		pcmfd = 0;
	}
#endif
	return 0;
}

aenc_private_t *aenc_init(void)
{
	aenc_private_t *audio_private = NULL;

	audio_private = oscl_zalloc(sizeof(aenc_private_t));
	if (!audio_private) {
		OSCL_LOGE("malloc arec_ctx_t error!\n");
		return NULL;
	}
	audio_private->paudio.para = oscl_zalloc(sizeof(AV_CodecParameters));
	if (!audio_private->paudio.para) {
		OSCL_LOGE("malloc paudio.para error!\n");
		oscl_free(audio_private);
		return NULL;
	}
	audio_private->pcm_manager = pcm_buf_init();
	if (!audio_private->pcm_manager) {
		OSCL_LOGE("malloc pcm_manager error!\n");
		oscl_free(audio_private->paudio.para);
		oscl_free(audio_private);
		return NULL;
	}

#ifdef PCMFILE
#ifdef __linux__
	remove(PCMNAME);
#else
	rm(PCMNAME);
#endif
	pcmfd = open(PCMNAME, O_WRONLY | O_CREAT);
	OSCL_LOGW("====open pcm file:%d", pcmfd);
#endif

	return audio_private;
}
