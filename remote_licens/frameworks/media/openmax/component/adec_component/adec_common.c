/*
 * adec_component.c - demo code for adec component.
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
#define LOG_TAG           "adec_common"

#include <dlfcn.h>
#include <oscl.h>
#include <base_component.h>
#include "adec_common.h"
#include "adec_lib.h"
#include "raw_stream.h"
#include <demuxer_plugin.h>

#include "adec_lib_handle.h"

/* #define DECRAWDATA */
#ifdef DECRAWDATA
static int pcmfd = -1;
#define PCMNAME "/mnt/card/audio.raw"
#endif

/* #define SAVE_SOURCE_DATA */
#ifdef SAVE_SOURCE_DATA
static int srcfd = -1;
#define SRCNAME "/mnt/card/adec_source.dat"
#endif

typedef int (*get_plugin)(struct  tag_audio_dec *pdec);

int adec_frame(adec_private_t *audio_private,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf)
{
	int ret = 0;
	packet_t packet_in;
	packet_t packet_out;
	audio_packet_t *apkt;
	int res = 0;

	if (inbuf == NULL || inbuf->pBuffer == NULL) {
		OSCL_LOGE("inbuf NULL ERROR, inbuf=%p, pBuffer=%p",
			inbuf, inbuf == NULL ? NULL : inbuf->pBuffer);
		return 0;
	}
	if (outbuf == NULL || outbuf->pBuffer == NULL) {
		OSCL_LOGE("outbuf NULL ERROR, outbuf=%p, pBuffer=%p",
			outbuf, outbuf == NULL ? NULL : outbuf->pBuffer);
		return 0;
	}
	apkt = &audio_private->pkt;
	if (apkt->data == NULL && (apkt->offset != 0 || apkt->size != 0)) {
		OSCL_LOGE("packet param err, vir=%p, size=%d, offset=%d!",
			apkt->data, (int)apkt->size, apkt->offset);
		outbuf->nFilledLen = 0;
		outbuf->nOffset = 0;
		inbuf->nFilledLen = 0;
		apkt->offset = 0;
		return 0;
	}

#ifdef SAVE_SOURCE_DATA
	if (srcfd >= 0) {
		int len;
		len = write(srcfd, apkt->data + apkt->offset, apkt->size);
		if (len != apkt->size)
			OSCL_LOGE("write err:ret:%d,len:%d\n", len, apkt->size);
	}
#endif

	pthread_mutex_lock(&audio_private->lock);
	OSCL_LOGD("packet: size=%d, ofs=%d, buf.len=%d, buf.offset=%d",
		apkt->size, apkt->offset,
		audio_private->buf.len, audio_private->buf.offset);
	res = getRawData(&audio_private->buf, apkt->data + apkt->offset,
			apkt->size, &packet_in);
	if (res > 0) {
		apkt->size -= res;
		apkt->offset += res;
	}

	outbuf->nFilledLen = 0;
	packet_out.data = outbuf->pBuffer + outbuf->nOffset;
	packet_out.size = 0;

	if ((apkt->flags & AV_PACKET_FLAG_EOS) && (packet_in.size == 0)) {
		pthread_mutex_unlock(&audio_private->lock);
		OSCL_LOGE("detect eos, don't handle, just pass to outbuf and return");
		outbuf->nFlags |= OMX_BUFFERFLAG_EOS;
		outbuf->nTimeStamp = apkt->timestamp;
		outbuf->nFilledLen = 0;
		apkt->size = 0;
		return 0;
	}

	OSCL_LOGD("adec start, in.size=%d, nOffset=%u, buf.len=%d, buf.offset=%d",
		packet_in.size, apkt->offset,
		audio_private->buf.len, audio_private->buf.offset);
	ret =  audio_private->paudio.decode(&audio_private->paudio,
			&packet_in, &packet_out);
	OSCL_LOGD("dec size:%d,ret:%d,frame:%lld",
		packet_out.size, ret, (long long)audio_private->nFrames);
	if (ret <= 0) {
		audio_private->dec_err_cnt++;
		if (audio_private->dec_err_cnt % 10 == 0) {
			OSCL_LOGW("in=%d, out=%d, ret=%d, cnt=%d",
				packet_in.size, packet_out.size,
				ret, audio_private->dec_err_cnt);
		}
	} else {
		audio_private->dec_err_cnt = 0;
	}
	outbuf->nOffset = 0;
	outbuf->nFilledLen = packet_out.size;

	if (apkt->timestamp >= 0) {
		outbuf->nTimeStamp = apkt->timestamp;
		audio_private->nFrames = outbuf->nTimeStamp *
			audio_private->paudio.para->sample_rate / 1000000;
		if (audio_private->paudio.para->channels)
			audio_private->nFrames += (packet_out.size >> 1) /
				audio_private->paudio.para->channels;
		if (packet_out.size)
			apkt->timestamp = -1;
	} else {
		if (audio_private->paudio.para->sample_rate)
			outbuf->nTimeStamp = (int64_t)audio_private->nFrames * 1000000 /
				audio_private->paudio.para->sample_rate;
		if (audio_private->paudio.para->channels) {
			audio_private->nFrames += (packet_out.size >> 1) /
				audio_private->paudio.para->channels;
		}
	}

	OSCL_LOGD("#####ret:%d, outtime:%lld, nFrames:%lld, outlen:%d, ch:%d, sr:%d",
		ret, outbuf->nTimeStamp, audio_private->nFrames, packet_out.size,
		audio_private->paudio.para->channels,
		audio_private->paudio.para->sample_rate);

	res = updataRawData(&audio_private->buf, apkt->data + apkt->offset,
			apkt->size, &packet_in, ret);
	if (res > 0) {
		apkt->size -= res;
		apkt->offset += res;
	}

	if ((apkt->flags & AV_PACKET_FLAG_EOS) &&
		(ret == DEC_UNDERFLOW || apkt->size <= 0))  {
		OSCL_LOGI("get the end of stream,len:%d",
			audio_private->buf.len);
		outbuf->nFlags |= OMX_BUFFERFLAG_EOS;
		apkt->size = 0;
	}

	if (apkt->size == 0)
		apkt->offset = 0;
	if (apkt->size == 0 && (audio_private->buf.len == 0 || ret == DEC_UNDERFLOW))
		inbuf->nFilledLen = 0;

	pthread_mutex_unlock(&audio_private->lock);

#ifdef DECRAWDATA
	if (pcmfd >= 0) {
		int len = write(pcmfd, packet_out.data, packet_out.size);
		if (len != packet_out.size)
			OSCL_LOGE("write err:ret:%d,len:%d\n", ret, len);
	}
#endif

	return ret;
}

int adec_open(adec_private_t *audio_private)
{
	get_plugin plugin_func;
	int ret = 0;

	audio_private->dl_hdl = open_adec_lib(
		audio_private->paudio.para->codec_id);
	if (audio_private->dl_hdl == NULL) {
		OSCL_LOGE("Failed to open lib: %s", dlerror());
		goto ERR_EXIT;
	}
	plugin_func = (get_plugin)dlsym(audio_private->dl_hdl, "get_plugin_info");
	if (plugin_func == NULL) {
		OSCL_LOGE("get plugin info error!");
		goto ERR_EXIT;
	}
	ret = plugin_func(&(audio_private->paudio));
	if (ret < 0) {
		OSCL_LOGE("paudio init error!");
		goto ERR_EXIT;
	}

	OSCL_LOGI("codec_id=0x%x, sample_rate=%d, ch=%d, bits=%d, bitrate=%lld",
		audio_private->paudio.para->codec_id,
		audio_private->paudio.para->sample_rate,
		audio_private->paudio.para->channels,
		audio_private->paudio.para->bits_per_coded_sample,
		(OMX_S64)audio_private->paudio.para->bit_rate);

	pthread_mutex_lock(&audio_private->lock);

	audio_private->paudio.init_flag = 0;
	if (audio_private->paudio.para->extradata) {
		oscl_free(audio_private->paudio.para->extradata);
		audio_private->paudio.para->extradata = NULL;
		audio_private->paudio.para->extradata_size = 0;
	}

	ret = audio_private->paudio.init(&(audio_private->paudio));

	pthread_mutex_unlock(&audio_private->lock);

#ifdef DECRAWDATA
#ifdef __linux__
	remove(PCMNAME);
#else
	rm(PCMNAME);
#endif
	pcmfd = open(PCMNAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	OSCL_LOGW("====open pcm file:%d", pcmfd);
#endif

#ifdef SAVE_SOURCE_DATA
	srcfd = open(SRCNAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	OSCL_LOGW("====open src file:%d", srcfd);
#endif

	return ret;

ERR_EXIT:
	if (audio_private->dl_hdl != NULL) {
		close_adec_lib(audio_private->dl_hdl);
		audio_private->dl_hdl = NULL;
	}
	return -1;
}

int adec_close(adec_private_t *audio_private)
{
	int ret = 0;
#ifdef DECRAWDATA
	if (pcmfd >= 0) {
		close(pcmfd);
		OSCL_LOGI("close pcm file :%d\n", pcmfd);
		pcmfd = -1;
	}
#endif
#ifdef SAVE_SOURCE_DATA
	if (srcfd >= 0) {
		close(srcfd);
		OSCL_LOGI("close src file :%d\n", srcfd);
		srcfd = -1;
	}
#endif
	pthread_mutex_lock(&audio_private->lock);
	ret =  audio_private->paudio.close(&(audio_private->paudio));
	pthread_mutex_unlock(&audio_private->lock);
	if (audio_private->dl_hdl != NULL) {
		close_adec_lib(audio_private->dl_hdl);
		audio_private->dl_hdl = NULL;
	}
	return ret;
}

int adec_flush(adec_private_t *audio_private)
{
	int ret;

	OSCL_LOGI("adec_flush");

	pthread_mutex_lock(&audio_private->lock);
	resetRawBuf(&audio_private->buf);
	audio_private->nFrames = 0;

	ret = audio_private->paudio.flush(&(audio_private->paudio));
	pthread_mutex_unlock(&audio_private->lock);

	return ret;
}

int adec_deinit(adec_private_t *audio_private)
{
	if (audio_private == NULL) {
		OSCL_LOGE("audio_private=NULL");
		return -1;
	}

	pthread_mutex_destroy(&audio_private->lock);
	if (audio_private->paudio.para) {
		if (audio_private->paudio.para->extradata) {
			oscl_free(audio_private->paudio.para->extradata);
			audio_private->paudio.para->extradata = NULL;
			audio_private->paudio.para->extradata_size = 0;
		}
		oscl_free(audio_private->paudio.para);
		audio_private->paudio.para = NULL;
	}

	oscl_free(audio_private);

	return 0;
}

adec_private_t *adec_init(void)
{
	adec_private_t *audio_private = NULL;

	audio_private = oscl_zalloc(sizeof(adec_private_t));
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
	pthread_mutex_init(&audio_private->lock, NULL);

	return audio_private;
}
