/*
 * arec_component.c - audio receive component.
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

#define DBG_LEVEL		DBG_ERROR
#ifdef __EOS__
#include <rtdevice.h>
#include <drivers/audio.h>
#include <audio/asoundlib.h>
#endif
#include <alsa/asoundlib.h>
#include <oscl.h>
#include <base_component.h>
#include "arec_component.h"
#include "speex_echo.h"

/* #define SAVE_PCM_FILE */
#define REC_PERIOD_CNT	4
#define REC_PERIOD_SIZE	1024
#define REC_BUFFER_SIZE	4096

typedef struct arec_ctx {
	snd_pcm_t	*pcm_dev;
	OMX_U32		channels;
	OMX_U32		samplefmt;
	OMX_U32		samplerate;
	OMX_U32		bytes_perframe;
	OMX_TICKS	timestamp;
	OMX_AUDIO_ECHOCANTYPE eEchoCancelation;
	void        *echo;
#ifdef SAVE_PCM_FILE
	FILE *reffile;
	FILE *farfile;
	FILE *outfile;
	int frames
#endif
} arec_ctx_t;

static snd_pcm_format_t to_pcm_fmt(int bitwidt)
{
	snd_pcm_format_t fmt = SND_PCM_FORMAT_S16_LE;

	if (bitwidt == 32)
		fmt = SND_PCM_FORMAT_S32_LE;
	else if (bitwidt == 24)
		fmt = SND_PCM_FORMAT_S24_LE;
	else if (bitwidt == 16)
		fmt = SND_PCM_FORMAT_S16_LE;
	else if (bitwidt == 8)
		fmt = SND_PCM_FORMAT_S8;

	return fmt;
}

static OMX_ERRORTYPE audio_receive_init(lb_omx_component_t *component)
{
	OMX_S32 ret = 0;
	arec_ctx_t *audio_ctx = NULL;
	snd_pcm_hw_params_t *hw;

	audio_ctx = component->component_private;

	/* audio_ctx->eEchoCancelation = OMX_AUDIO_EchoCanNormal; */
	if (audio_ctx->eEchoCancelation != OMX_AUDIO_EchoCanOff) {
		audio_ctx->channels = 2;
		audio_ctx->echo = (void *)speex_echo_state_init(NN, TAIL);
		if (!audio_ctx->echo) {
			OSCL_LOGE("echo init error\n");
			return OMX_ErrorResourcesLost;
		}
		speex_echo_ctl(audio_ctx->echo,
			SPEEX_ECHO_SET_SAMPLING_RATE, &audio_ctx->samplerate);
	}
	audio_ctx->bytes_perframe =
		audio_ctx->samplefmt * audio_ctx->channels / 8;
	ret = snd_pcm_open(&audio_ctx->pcm_dev, "default", SND_PCM_STREAM_CAPTURE, 0);
	if (ret < 0) {
		OSCL_LOGE("open pcm err\n");
		return OMX_ErrorResourcesLost;
	}
	OSCL_LOGE("samplerate=%d, ch=%d, %d bits",
		audio_ctx->samplerate, audio_ctx->channels, audio_ctx->samplefmt);
	snd_pcm_hw_params_malloc(&hw);
	snd_pcm_hw_params_any(audio_ctx->pcm_dev, hw);
	snd_pcm_hw_params_set_access(audio_ctx->pcm_dev,
		hw, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(audio_ctx->pcm_dev,
		hw, to_pcm_fmt(audio_ctx->samplefmt));
	snd_pcm_hw_params_set_channels(audio_ctx->pcm_dev, hw, audio_ctx->channels);
	snd_pcm_hw_params_set_rate(audio_ctx->pcm_dev, hw, audio_ctx->samplerate, 0);
	snd_pcm_hw_params_set_period_size(audio_ctx->pcm_dev, hw, REC_PERIOD_SIZE, 0);
	snd_pcm_hw_params_set_buffer_size(audio_ctx->pcm_dev, hw, REC_BUFFER_SIZE);
	if (snd_pcm_hw_params(audio_ctx->pcm_dev, hw) < 0) {
		OSCL_LOGE("snd_pcm_hw_params error\n");
		snd_pcm_close(audio_ctx->pcm_dev);
		return OMX_ErrorResourcesLost;
	}
	snd_pcm_hw_params_free(hw);
#ifdef SAVE_PCM_FILE
	audio_ctx->frames = 0;
	audio_ctx->reffile = fopen("/data/ref.pcm", "wb");
	if (audio_ctx->reffile == NULL)
		OSCL_LOGE("open ref file err!\n");
	audio_ctx->farfile = fopen("/data/far.pcm", "wb");
	if (audio_ctx->farfile == NULL)
		OSCL_LOGE("open far file err!\n");
	audio_ctx->outfile = fopen("/data/recout.pcm", "wb");
	if (audio_ctx->outfile == NULL)
		OSCL_LOGE("open out file err!\n");
#endif
	OSCL_LOGI("audio receive init success!");
	return ret;
}

static void audio_receive_exit(lb_omx_component_t *component)
{
	arec_ctx_t *audio_ctx = component->component_private;

	if (audio_ctx->pcm_dev == NULL)
		return;

	speex_echo_state_destroy(audio_ctx->echo);
	snd_pcm_close(audio_ctx->pcm_dev);
	audio_ctx->pcm_dev = NULL;
#ifdef SAVE_PCM_FILE
	fclose(audio_ctx->reffile);
	fclose(audio_ctx->farfile);
	fclose(audio_ctx->outfile);
#endif

}

short ref_buf[NN];
short echo_buf[NN];

void arec_buffer_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	arec_ctx_t *audio_ctx = NULL;
	lb_omx_component_t *component = NULL;
	OMX_S32 ret = 0;

	component = get_lb_component(stand_com);
	audio_ctx = component->component_private;
	if (audio_ctx->pcm_dev == NULL
		|| outbuffer == NULL
		|| outbuffer->nAllocLen == 0
		|| outbuffer->pBuffer == NULL) {
		OSCL_LOGE("fd :%p, inbuffer:%p, nAllocLen:%d, pBuffer:%p!!",
			audio_ctx->pcm_dev,
			outbuffer, outbuffer->nAllocLen, outbuffer->pBuffer);
		OSCL_LOGE("err param!");
		return;
	}

	outbuffer->nOffset = 0;
	outbuffer->nFlags = 0;

	ret = snd_pcm_readi(audio_ctx->pcm_dev, outbuffer->pBuffer,
			outbuffer->nAllocLen / audio_ctx->bytes_perframe);
	OSCL_LOGD("read ret %d, alloc len %d\n", ret, outbuffer->nAllocLen);
	if (ret == -EPIPE) {
		int res;
		OSCL_LOGE("audio read data error ret %d", ret);
		res = snd_pcm_prepare(audio_ctx->pcm_dev);
		if (res < 0)
			OSCL_LOGE("snd_pcm_prepare err(%d, %s)", res, snd_strerror(res));
		outbuffer->nFilledLen = 0;
		return;
	}  else if (ret <= 0) {
		OSCL_LOGE("audio read data error ret %d", ret);
		outbuffer->nFilledLen = 0;
		return;
	}

	if (audio_ctx->eEchoCancelation != OMX_AUDIO_EchoCanOff) {
		int i = 0;
		int num = 0;
		short *e_buf = (short *)outbuffer->pBuffer;
		short *out = (short *)outbuffer->pBuffer;
		while (num < ret) {
			for (i = 0; i < NN; i++) {
				ref_buf[i] = e_buf[2 * i];
				echo_buf[i] = e_buf[2 * i + 1];
			}

#ifdef SAVE_PCM_FILE
			if (audio_ctx->reffile)
				fwrite(ref_buf, 1, NN * 2, audio_ctx->reffile);
			if (audio_ctx->farfile)
				fwrite(echo_buf, 1, NN * 2, audio_ctx->farfile);

			audio_ctx->frames++;
#endif
			speex_echo_cancellation(audio_ctx->echo, ref_buf, echo_buf, out);
			num += NN;
			e_buf += 2 * NN;
			out += NN;
		}
		outbuffer->nFilledLen = outbuffer->nAllocLen >> 1;
	} else {
		outbuffer->nFilledLen = outbuffer->nAllocLen;
	}
	audio_ctx->timestamp += (outbuffer->nFilledLen >> 1) / audio_ctx->channels;

#ifdef SAVE_PCM_FILE
	if (audio_ctx->outfile)
		fwrite(outbuffer->pBuffer, 1, outbuffer->nFilledLen, audio_ctx->outfile);
#endif
}

void *arec_buffer_manager(void *param)
{
	lb_omx_component_t *component = NULL;
	OMX_COMPONENTTYPE *stand_comp = (OMX_COMPONENTTYPE *)param;
	base_port_t *outport = NULL;
	OMX_BUFFERHEADERTYPE *outbuffer = NULL;

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(stand_comp);
	outport = &component->port[AUDIO_OUTPUT_PORT_INDEX];

	while (component->state == OMX_StateIdle
		|| component->state == OMX_StateExecuting
		|| component->state == OMX_StatePause
		|| component->target_state == OMX_StateIdle) {

		while (sem_trywait(component->buf_mgnt_sem) == 0)
			continue;
		/*Wait till the ports are being flushed*/
		pthread_mutex_lock(&component->flush_mutex);
		while (outport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			if (outbuffer != NULL) {
				outport->return_buffer(outport, outbuffer);
				outbuffer = NULL;
				OSCL_LOGI("retrun buffer while flushing port");
			}
			OSCL_LOGI("%s sem_wait component->flush_sem",
				get_component_name(component));
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			pthread_mutex_lock(&component->flush_mutex);
		}
		pthread_mutex_unlock(&component->flush_mutex);

		OSCL_LOGD("component->state:%d, sem value:%d", component->state,
			sem_get(component->buf_mgnt_sem));
		if (component->state != OMX_StateExecuting) {
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		OSCL_LOGD("comp:%s, queue len:%d outbuf:%p",
			get_component_name(component),
			oscl_queue_get_num(&outport->buffer_queue), outbuffer);
		if (outbuffer == NULL) {
			outbuffer = oscl_queue_dequeue(&outport->buffer_queue);
			OSCL_LOGD("read buf manager:%p\n", outbuffer);
			if (outbuffer) {
				outbuffer->nFilledLen = 0;
				outbuffer->nFlags = 0;
			}
		}
		if (outbuffer == NULL) {
			OSCL_LOGD("sem_wait component->buf_mgnt_sem");
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		outbuffer->nFlags = 0;
		if (component->mark.hMarkTargetComponent) {
			outbuffer->hMarkTargetComponent
				= component->mark.hMarkTargetComponent;
			outbuffer->pMarkData
				= component->mark.pMarkData;
			component->mark.hMarkTargetComponent = NULL;
			component->mark.pMarkData = NULL;
		}

		OSCL_LOGD("component->state:%x,outbuffer->nAllocLen:%d",
			component->state,
			outbuffer->nAllocLen);
		/* only when we're at executing state, we handle the buffer */
		if (component->state == OMX_StateExecuting) {
			if (component->buf_handle && outbuffer->nAllocLen != 0)
				(*(component->buf_handle))(stand_comp, NULL, outbuffer);
		}
		base_check_mark(component, outbuffer);
		/* if we have an eos flag, then notify app from eventHandler */
		base_check_eos(component, outport, outbuffer);
		/* if we successfully fill the buffer, then return it */
		if (outbuffer->nFilledLen != 0
			|| component->target_state == OMX_StateLoaded) {
			OSCL_LOGI("outport return_buffer");
			outport->return_buffer(outport, outbuffer);
			outbuffer = NULL;
		}
	}
	if ((outbuffer != NULL) && (!PORT_IS_SUPPLIER(outport))) {
		OSCL_LOGI("outport return_buffer before exit");
		outport->return_buffer(outport, outbuffer);
	}
	OSCL_LOGI("exit from reader_buffer_manager\n");
	OSCL_TRACE(" %p\n", param);
	pthread_exit(NULL);

	return NULL;

}

static OMX_ERRORTYPE arec_get_config(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_IN OMX_PTR data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	arec_ctx_t *audio_ctx = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE *cur_time;

	oscl_param_check((cmp_hdl != NULL) && (data != NULL),
		OMX_ErrorBadParameter, NULL);

	component = get_lb_component(cmp_hdl);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (index) {
	case omx_index_lombo_config_cur_time:
		cur_time = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)data;
		cur_time->nTimestamp = audio_ctx->timestamp;
		break;
	default:
		ret = base_get_config(cmp_hdl, index, data);
		break;
	}
	return ret;
}

#ifdef __EOS__
OMX_ERRORTYPE arec_set_config(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_IN OMX_PTR data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	rt_device_t	snd_dev = NULL;
	OMX_AUDIO_CONFIG_MUTETYPE *mute = NULL;
	struct rt_audio_caps caps;
	lb_omx_component_t *component = NULL;
	arec_ctx_t *audio_ctx = NULL;

	component = get_lb_component(cmp_hdl);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (index) {
	case OMX_IndexConfigAudioMute:
		snd_dev = rt_device_find("sound0");
		if (snd_dev == NULL) {
			OSCL_LOGE("can't find sound device\n");
			return OMX_ErrorHardware;
		}
		mute = (OMX_AUDIO_CONFIG_MUTETYPE *)data;
		memset(&caps, 0, sizeof(caps));
		if (mute->bMute == OMX_TRUE)
			caps.udata.value = 1;
		else
			caps.udata.value = 0;
		caps.main_type = AUDIO_TYPE_INPUT;
		caps.sub_type  = AUDIO_MIXER_MUTE;
		ret = rt_device_control(snd_dev, AUDIO_CTL_CONFIGURE, &caps);
		if (ret) {
			OSCL_LOGE("set audio driver mute err\n");
			return OMX_ErrorHardware;
		}
		break;
	case OMX_IndexConfigAudioEchoCancelation: {
		OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE *audio_params;
		audio_params = (OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE *)data;
		audio_ctx->eEchoCancelation = audio_params->eEchoCancelation;
		}
		break;
	default:
		OSCL_LOGE("config index %d not support yet\n");
		break;
	}

	return ret;
}
#else
OMX_ERRORTYPE arec_set_config(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_INDEXTYPE index,
	OMX_IN OMX_PTR data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	arec_ctx_t *audio_ctx = NULL;

	component = get_lb_component(cmp_hdl);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (index) {
	case OMX_IndexConfigAudioMute:
		/* TODO: call pcm mixer to set mute */
		break;
	case OMX_IndexConfigAudioEchoCancelation: {
		OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE *audio_params;
		audio_params = (OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE *)data;
		audio_ctx->eEchoCancelation = audio_params->eEchoCancelation;
		}
		break;
	default:
		OSCL_LOGE("config index %d not support yet\n", index);
		break;
	}

	return ret;
}
#endif

OMX_ERRORTYPE arec_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	arec_ctx_t *audio_ctx = NULL;
	OMX_AUDIO_PARAM_PCMMODETYPE *audio_params = NULL;

	audio_params = (OMX_AUDIO_PARAM_PCMMODETYPE *)paramData;
	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, paramIndex);
	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (paramIndex) {
	case OMX_IndexParamAudioPcm:
		audio_ctx->channels = audio_params->nChannels;
		audio_ctx->samplefmt = audio_params->nBitPerSample;
		audio_ctx->samplerate = audio_params->nSamplingRate;
		break;
	default:
		OSCL_LOGI("default base set params index %d\n", paramIndex);
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE arec_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	arec_ctx_t *audio_ctx = NULL;
	OMX_STATETYPE pre_state;

	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	audio_ctx = component->component_private;

	if (dest_state == OMX_StateExecuting && (component->state == OMX_StateIdle ||
		component->state == OMX_StatePause)) {
		if (audio_ctx->pcm_dev != NULL) {
			OSCL_LOGW("Device not closed while entering StateIdle");
			snd_pcm_close(audio_ctx->pcm_dev);
			audio_ctx->pcm_dev = NULL;
		}
		audio_ctx->timestamp = -1;
		ret = audio_receive_init(component);
		if (OMX_ErrorNone != ret)
			return ret;
	}

	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);
	if (dest_state == OMX_StatePause && pre_state == OMX_StateExecuting) {
		OSCL_LOGI("audio receiver exit\n");
		audio_receive_exit(component);
	}
	if (dest_state == OMX_StateIdle &&
		(pre_state == OMX_StateExecuting || pre_state == OMX_StatePause)) {
		OSCL_LOGE("audio receiver exit\n");
		audio_receive_exit(component);
	}
	return ret;

}

OMX_ERRORTYPE arec_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *base_cmp = (OMX_COMPONENTTYPE *)hComponent;
	lb_omx_component_t *component = NULL;
	arec_ctx_t *audio_ctx = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	OSCL_TRACE("base_cmp_handle:%p\n", hComponent);
	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = (lb_omx_component_t *)base_cmp->pComponentPrivate;
	audio_ctx = (arec_ctx_t *)component->component_private;

	component->port[AUDIO_OUTPUT_PORT_INDEX].deinit(
		&component->port[AUDIO_OUTPUT_PORT_INDEX]);
	oscl_free(audio_ctx);
	ret = base_component_deinit(hComponent);

	return ret;
}

OMX_ERRORTYPE arec_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	arec_ctx_t *audio_ctx = NULL;

	audio_ctx = oscl_zalloc(sizeof(arec_ctx_t));
	if (!audio_ctx) {
		OSCL_LOGE("malloc arec_ctx_t error!\n");
		return OMX_ErrorInsufficientResources;
	}
	audio_ctx->timestamp = -1;

	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_component_init error!\n");
		goto error1;
	}
	cmp_handle->name = "OMX.LB.SOURCE.AREC";
	cmp_handle->component_private = audio_ctx;
	cmp_handle->buf_manager = arec_buffer_manager;
	cmp_handle->buf_handle = arec_buffer_handle;
	cmp_handle->base_comp.ComponentDeInit = arec_component_deinit;
	cmp_handle->base_comp.SetParameter = arec_set_parameter;
	cmp_handle->base_comp.SetConfig = arec_set_config;
	cmp_handle->base_comp.GetConfig = arec_get_config;
	cmp_handle->do_state_set = arec_set_state;
	cmp_handle->num_ports = 1;
	ret = base_port_init(cmp_handle, &cmp_handle->port[AUDIO_OUTPUT_PORT_INDEX],
			AUDIO_OUTPUT_PORT_INDEX,
			OMX_DirOutput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_port_init error!\n");
		goto error2;
	}
	cmp_handle->port[AUDIO_OUTPUT_PORT_INDEX].port_param.nBufferSize =
		AUDIO_OUTBUF_DEFAULT_SIZE;
	cmp_handle->port[AUDIO_OUTPUT_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainAudio;
	cmp_handle->port[AUDIO_OUTPUT_PORT_INDEX].port_param.nBufferCountActual =
		8;

	return ret;

error2:
	base_component_deinit(cmp_handle);
error1:
	if (audio_ctx != NULL)
		oscl_free(audio_ctx);
	OSCL_TRACE("%d arec:%p\n", ret, cmp_handle);

	return ret;

}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = arec_component_init;
#endif

