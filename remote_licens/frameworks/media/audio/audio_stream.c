/*
 * omx_audio_recorder.c - Standard functionality for audio stream recorder.
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
#define DBG_LEVEL		DBG_WARNING
#include <oscl.h>
#include <base_component.h>
#include "omx_api.h"
#include "audio_stream.h"
#include "av_media_type.h"

#define AUDIO_DEFAULT_RATE		48000
#define AUDIO_DEFAULT_CHANNELS		2
#define AUDIO_DEFAULT_FMT_WIDTH		16
#define AUDIO_DEFAULT_CODINGTYPE	AUDIO_ENCODER_AAC

typedef enum astream_state {
	AS_STATE_ERR = 0,
	AS_STATE_INIT,
	AS_STATE_START,
	AS_STATE_ENCODE,
} astream_state_e;

typedef struct asrc_info {
	comp_info_t al_comp;
	char *dev_name;
	int channel;
	port_info_t *aout;
} asrc_info_t;

typedef struct aenc_info {
	comp_info_t al_comp;
	port_info_t *ain;
	port_info_t *aout;
} aenc_info_t;

typedef struct _asplt_info {
	comp_info_t al_comp;
	port_info_t *in;
	int outport_map;
} audio_splt_t;

typedef struct audio_stream {
	OMX_U32 channels;	/* Number of channels (e.g. 2 for stereo) */
	OMX_U32 bit_per_sample;	/* Bit per sample */
	OMX_U32 sample_rate;	/* Sampling rate of the source data.
				   Use 0 for variable or unknown sampling rate. */
	audio_encoder_t coding_type;
	int aec_on;

	astream_state_e state;
	asrc_info_t asrc_info;
	audio_splt_t asrc_splt;
	port_info_t *aenc_srcport;
	aenc_info_t aenc_info;
	audio_splt_t aenc_splt;

	pthread_mutex_t lock;
	int ref;
	int lock_init;
} audio_stream_t;


audio_stream_t g_audio_rec = {
	.lock_init = 0
};

static void _get_config_audio(audio_stream_t *astream)
{
	astream->channels = AUDIO_DEFAULT_CHANNELS;
	astream->bit_per_sample = AUDIO_DEFAULT_FMT_WIDTH;
	astream->sample_rate = AUDIO_DEFAULT_RATE;
	astream->coding_type = AUDIO_DEFAULT_CODINGTYPE;
	OSCL_LOGI("channels:%d, bit_per_sample:%d, sample_rate:%d, coding_type:%d)",
		astream->channels,
		astream->bit_per_sample,
		astream->sample_rate,
		astream->coding_type);
}

/**
 * _setup_asrc - set para for audio receiver components
 *
 * @astream: astream
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _setup_asrc(audio_stream_t *astream)
{
	OMX_AUDIO_PARAM_PCMMODETYPE pcm_type;
	OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE echo_type;
	int ret = 0;

	OSCL_TRACE("astream:%p", astream);
	/* setup para for audio receiver components*/
	memset(&echo_type, 0, sizeof(echo_type));
	echo_type.eEchoCancelation = astream->aec_on;
	ret = OMX_SetConfig(astream->asrc_info.al_comp.cmp_hdl,
			OMX_IndexConfigAudioEchoCancelation,
			&echo_type);
	memset(&pcm_type, 0, sizeof(pcm_type));
	pcm_type.nChannels = astream->channels;
	pcm_type.nBitPerSample = astream->bit_per_sample;
	pcm_type.nSamplingRate = astream->sample_rate;
	ret = OMX_SetParameter(astream->asrc_info.al_comp.cmp_hdl,
			OMX_IndexParamAudioPcm,
			&pcm_type);
	OSCL_TRACE("astream:%p", astream);
	return ret;
}

/**
 * _setup_aenc - set para for audio encoder components
 *
 * @astream: astream
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _setup_aenc(audio_stream_t *astream)
{
	OMX_AUDIO_PARAM_ADPCMTYPE adpcm_type;
	OMX_AUDIO_PARAM_PCMMODETYPE pcm_type;
	OMX_AUDIO_PARAM_AACPROFILETYPE aac_type;
	OMX_AUDIO_PARAM_G729TYPE g729_type;
	int ret = 0;

	switch (astream->coding_type) {
	case AUDIO_ENCODER_ADPCM:
		memset(&adpcm_type, 0, sizeof(OMX_AUDIO_PARAM_ADPCMTYPE));
		adpcm_type.nChannels = astream->channels;
		adpcm_type.nBitsPerSample = astream->bit_per_sample;
		adpcm_type.nSampleRate = astream->sample_rate;
		adpcm_type.nPortIndex = astream->aenc_info.aout->index;
		ret = OMX_SetParameter(astream->aenc_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioAdpcm,
				&adpcm_type);
		break;

	case AUDIO_ENCODER_AAC:
		memset(&aac_type, 0, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
		aac_type.nChannels = astream->channels;
		aac_type.nSampleRate = astream->sample_rate;
		aac_type.nBitRate = 128000;
		aac_type.eAACStreamFormat = OMX_AUDIO_AACStreamFormatRAW;
		aac_type.nPortIndex = astream->aenc_info.aout->index;
		OSCL_LOGI("aac setting: channels %d, samplerate:%d, bitrate:%d",
			aac_type.nChannels, aac_type.nSampleRate, aac_type.nBitRate);
		ret = OMX_SetParameter(astream->aenc_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioAac,
				&aac_type);
		break;
	case AUDIO_ENCODER_AAC_MP4ADTS:
		memset(&aac_type, 0, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
		aac_type.nChannels = astream->channels;
		aac_type.nSampleRate = astream->sample_rate;
		aac_type.nBitRate = 128000;
		aac_type.eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4ADTS;
		aac_type.nPortIndex = astream->aenc_info.aout->index;
		OSCL_LOGI("aac setting: channels %d, samplerate:%d, bitrate:%d",
			aac_type.nChannels, aac_type.nSampleRate, aac_type.nBitRate);
		ret = OMX_SetParameter(astream->aenc_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioAac,
				&aac_type);
		break;
	case AUDIO_ENCODER_G729:
		memset(&g729_type, 0, sizeof(OMX_AUDIO_PARAM_G729TYPE));
		g729_type.nChannels = astream->channels;
		ret = OMX_SetParameter(astream->aenc_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioG729,
				&g729_type);
		break;

	case AUDIO_ENCODER_PCM:
	case AUDIO_ENCODER_PCM_ALAW:
	case AUDIO_ENCODER_PCM_MULAW:
	default:
		memset(&pcm_type, 0, sizeof(pcm_type));
		pcm_type.nChannels = astream->channels;
		pcm_type.nBitPerSample = astream->bit_per_sample;
		pcm_type.nSamplingRate = astream->sample_rate;
		pcm_type.nPortIndex = astream->aenc_info.aout->index;
		if (astream->coding_type == AUDIO_ENCODER_PCM_ALAW)
			pcm_type.ePCMMode = AV_CODEC_TYPE_PCM_ALAW;
		if (astream->coding_type == AUDIO_ENCODER_PCM_MULAW)
			pcm_type.ePCMMode = AV_CODEC_TYPE_PCM_MULAW;
		ret = OMX_SetParameter(astream->aenc_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioPcm,
				&pcm_type);
		break;
	}
	OSCL_TRACE("astream:%p", astream);
	return ret;
}

static int _asplt_prepare(audio_splt_t *splt_info, OMX_PARAM_PORTDEFINITIONTYPE *port)
{
	int i = 0;
	int ret = 0;
	port_info_t *al_port;

	OSCL_TRACE("==");
	for (i = 0; i < splt_info->al_comp.num_port; i++) {
		al_port = &splt_info->al_comp.port_info[i];
		al_port->domain = port->eDomain;
		OSCL_LOGI("nbuffer(%d) edir(%d) domain(%d) size(%d)!",
			al_port->nbuffer,
			al_port->edir,
			al_port->domain,
			al_port->buf_size);
		if (al_port->edir == OMX_DirInput) {
			port->eDir = OMX_DirInput;
			port->nPortIndex = i;
			ret = OMX_SetParameter(splt_info->al_comp.cmp_hdl,
					OMX_IndexParamPortDefinition, port);
			OSCL_LOGI("%d buf:%d", i, port->nBufferCountActual);
			break;
		}
	}
	for (i = 0; i < splt_info->al_comp.num_port; i++) {
		al_port = &splt_info->al_comp.port_info[i];
		al_port->domain = port->eDomain;
		port->nPortIndex = i;
		ret = OMX_GetParameter(splt_info->al_comp.cmp_hdl,
				OMX_IndexParamPortDefinition, port);
		al_port->nbuffer = port->nBufferCountActual;
		al_port->buf_size = port->nBufferSize;
		if (al_port->edir == OMX_DirOutput)
			al_port->is_shared_buffer = 1;
	}
	OSCL_TRACE("==");
	return ret;
}

static port_info_t *_splt_get_outport(audio_splt_t *vsplt_info)
{
	port_info_t *port = NULL;
	port_info_t *splt_port = NULL;
	int nports;
	int index;

	OSCL_TRACE("==");
	oscl_param_check(vsplt_info != NULL, NULL, NULL);
	splt_port = vsplt_info->al_comp.port_info;
	nports = vsplt_info->al_comp.num_port;
	for (index = 0; index < nports; index++) {
		if (splt_port[index].edir == OMX_DirInput)
			continue;
		if ((vsplt_info->outport_map & (1 << index)) == 0) {
			vsplt_info->outport_map |= (1 << index);
			port = &splt_port[index];
			break;
		}
	}
	OSCL_TRACE("=index:%d, port:%p=", index, port);
	return port;
}

static int _splt_put_outport(audio_splt_t *vsplt_info, port_info_t *port)
{
	port_info_t *splt_port = NULL;
	int ret = -1;

	OSCL_TRACE("==");
	oscl_param_check(vsplt_info != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	splt_port = vsplt_info->al_comp.port_info;
	if (port == &splt_port[port->index]) {
		if ((vsplt_info->outport_map & (1 << port->index)) == 0)
			OSCL_LOGE("free a port already freed");
		else {
			vsplt_info->outport_map &= ~(1 << port->index);
			ret = 0;
		}
	}
	OSCL_TRACE("==");
	return ret;
}

int _splt_enable_port(audio_splt_t *vsplt_info, port_info_t *port)
{
	int ret;

	OSCL_TRACE("==");
	oscl_param_check(vsplt_info != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);
	oscl_param_check(port == &vsplt_info->al_comp.port_info[port->index], -1, NULL);

	ret = component_sendcom(&vsplt_info->al_comp, OMX_CommandPortEnable,
				port->index, NULL);
	OSCL_TRACE("==");
	return ret;
}

int _splt_disable_port(audio_splt_t *vsplt_info, port_info_t *port)
{
	int ret;

	OSCL_TRACE("==");
	oscl_param_check(vsplt_info != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	ret = component_sendcom(&vsplt_info->al_comp, OMX_CommandPortDisable,
				port->index, NULL);
	OSCL_TRACE("==");
	return ret;
}

static int _is_splt_busy(audio_splt_t *vsplt_info)
{
	return (vsplt_info->outport_map != 0);
}

/**
 * _astream_prepare - astream preapra, setup audio component and set state to idle.
 *
 * @astream_handle: astream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_prepare(audio_stream_t *astream)
{
	OMX_PARAM_PORTDEFINITIONTYPE omx_port;
	int ret = 0;
	OSCL_TRACE("hdl:%p", astream);

	/* set para to audio component */
	ret = _setup_asrc(astream);
	oscl_param_check_exit((ret == 0), ret, NULL);

	/* setup splitter port definition */
	omx_port.nPortIndex = astream->asrc_info.aout->index;
	omx_port.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(astream->asrc_info.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &omx_port);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	_asplt_prepare(&astream->asrc_splt, &omx_port);

	/* connect audio source with audio source splitter */
	ret = untunnel_setup_ports(astream->asrc_info.aout,
			astream->asrc_splt.in);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* change to idle state for audio components*/
	component_setstate(&astream->asrc_splt.al_comp, OMX_StateIdle);
	component_setstate(&astream->asrc_info.al_comp, OMX_StateIdle);

EXIT:
	OSCL_TRACE("hdl:%p ret:%d", astream, ret);
	return ret;
}


/**
 * astream_start - start audio stream.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_start(audio_stream_t *astream)
{
	int ret = 0;

	OSCL_TRACE("hdl:%p", astream);
	if (astream->state == AS_STATE_START
		|| astream->state == AS_STATE_ENCODE) {
		OSCL_TRACE("astream already started");
		goto EXIT;
	}

	ret = _astream_prepare(astream);
	oscl_param_check_exit(ret == 0, -1, NULL);

	/* change to executing state for recorder components*/
	component_setstate(&astream->asrc_splt.al_comp, OMX_StateExecuting);
	component_setstate(&astream->asrc_info.al_comp, OMX_StateExecuting);
	astream->state = AS_STATE_START;

EXIT:
	OSCL_TRACE("hdl:%p astream->state:%d", astream, astream->state);
	return ret;
}



/**
 * _astream_stop - stop audio source.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_stop(audio_stream_t *astream)
{
	int ret = 0;

	OSCL_TRACE("hdl:%p", astream);

	if (astream->state != AS_STATE_START) {
		OSCL_LOGI("astream state is not start:%x!", astream->state);
		goto EXIT;
	}

	/* check if audio source spliter is busy */
	if (_is_splt_busy(&astream->asrc_splt) || _is_splt_busy(&astream->aenc_splt)) {
		OSCL_LOGI("audio port is busy: %x:%x!", astream->asrc_splt.outport_map,
			astream->aenc_splt.outport_map);
		ret = -1;
		goto EXIT;
	}

	component_setstate(&astream->asrc_splt.al_comp, OMX_StateIdle);
	component_setstate(&astream->asrc_info.al_comp, OMX_StateIdle);
	untunnel_unset_ports(astream->asrc_info.aout, astream->asrc_splt.in);

	component_setstate(&astream->asrc_info.al_comp, OMX_StateLoaded);
	component_setstate(&astream->asrc_splt.al_comp, OMX_StateLoaded);
	astream->state = AS_STATE_INIT;

EXIT:
	OSCL_TRACE("hdl:%p state:%d, encmap:%x",
		astream, astream->state, astream->aenc_splt.outport_map);
	return ret;
}

/**
 * _astream_enc_prepare - setup audio encoder component and set state to idle.
 *
 * @astream_handle: astream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_enc_prepare(audio_stream_t *astream)
{
	OMX_PARAM_PORTDEFINITIONTYPE omx_port;
	int ret = 0;
	OSCL_TRACE("hdl:%p", astream);

	/* set para to audio component */
	ret = _setup_aenc(astream);
	oscl_param_check_exit((ret == 0), ret, NULL);

	oscl_param_check_exit((astream->aenc_srcport == NULL), -1, NULL);
	astream->aenc_srcport = _splt_get_outport(&astream->asrc_splt);

	/* setup splitter port definition */
	omx_port.nPortIndex = astream->aenc_info.aout->index;
	omx_port.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(astream->aenc_info.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &omx_port);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	_asplt_prepare(&astream->aenc_splt, &omx_port);

	/* connect audio source with audio source splitter */
	ret = untunnel_setup_ports(astream->aenc_srcport,
			astream->aenc_info.ain);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	ret = untunnel_setup_ports(astream->aenc_info.aout,
			astream->aenc_splt.in);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* change to idle state for audio components*/
	component_setstate(&astream->aenc_splt.al_comp, OMX_StateIdle);
	component_setstate(&astream->aenc_info.al_comp, OMX_StateIdle);

EXIT:
	OSCL_TRACE("hdl:%p ret:%d", astream, ret);
	return ret;
}

/**
 * _astream_stop_enc - stop audio encoder.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_stop_enc(audio_stream_t *astream)
{
	int ret = 0;

	OSCL_TRACE("hdl:%p", astream);

	if (astream->state != AS_STATE_ENCODE) {
		OSCL_LOGI("astream state is not encoding:%x!", astream->state);
		goto EXIT;
	}

	/* check if audio encoder spliter is busy */
	if (_is_splt_busy(&astream->aenc_splt)) {
		OSCL_LOGI("aenc port is busy: %x!", astream->aenc_splt.outport_map);
		ret = -1;
		goto EXIT;
	}

	/* 1. disable source port
	 * 2. change encoder component and encoder spliter component to idle
	 * 3. disconnect asrc splitter and audio encoder. disconnect audio encoder and
	 *    audio encoder splitter.
	 * 4. free aenc source port.
	 * 5. change encoder component and encoder spliter component to load.
	*/
	_splt_disable_port(&astream->asrc_splt, astream->aenc_srcport);

	component_setstate(&astream->aenc_info.al_comp, OMX_StateIdle);
	component_setstate(&astream->aenc_splt.al_comp, OMX_StateIdle);

	untunnel_unset_ports(astream->aenc_srcport, astream->aenc_info.ain);
	untunnel_unset_ports(astream->aenc_info.aout, astream->aenc_splt.in);

	_splt_put_outport(&astream->asrc_splt, astream->aenc_srcport);
	astream->aenc_srcport = NULL;

	component_setstate(&astream->aenc_info.al_comp, OMX_StateLoaded);
	component_setstate(&astream->aenc_splt.al_comp, OMX_StateLoaded);
	astream->state = AS_STATE_START;

EXIT:
	OSCL_TRACE("hdl:%p state:%d, encmap:%x",
		astream, astream->state, astream->aenc_splt.outport_map);
	return ret;
}

/**
 * _astream_start_enc - start audio encode.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_start_enc(audio_stream_t *astream)
{
	int ret = 0;

	OSCL_TRACE("hdl:%p", astream);
	if (astream->state == AS_STATE_ENCODE) {
		OSCL_TRACE("astream already started");
		goto EXIT;
	}

	ret = _astream_enc_prepare(astream);
	oscl_param_check_exit(ret == 0, -1, NULL);

	/* change to executing state for recorder components*/
	component_setstate(&astream->aenc_splt.al_comp, OMX_StateExecuting);
	component_setstate(&astream->aenc_info.al_comp, OMX_StateExecuting);
	_splt_enable_port(&astream->asrc_splt, astream->aenc_srcport);
	astream->state = AS_STATE_ENCODE;

EXIT:
	OSCL_TRACE("hdl:%p astream->state:%d", astream, astream->state);
	if (ret != 0) {
		OSCL_LOGE("start astream failed, try stop!");
		_astream_stop_enc(astream);
	}
	return ret;
}

/**
 * _astream_reset - reset audio stream.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int _astream_reset(audio_stream_t *astream)
{
	int ret = 0;

	OSCL_TRACE("hdl:%p", astream);
	ret = _astream_stop_enc(astream);
	if (ret != 0) {
		OSCL_LOGI("_astream_stop_enc fail!");
		goto EXIT;
	}

	ret = _astream_stop(astream);
	if (ret != 0) {
		OSCL_LOGI("_astream_stop fail!");
		goto EXIT;
	}
EXIT:
	OSCL_TRACE("hdl:%p", astream);
	return ret;
}

int audio_stream_get_sample_rate(astream_handle hdl)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	oscl_param_check(astream != NULL, 0, NULL);

	return astream->sample_rate;
}

int audio_stream_get_sample_width(astream_handle hdl)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	oscl_param_check(astream != NULL, 0, NULL);

	return astream->bit_per_sample;
}

int audio_stream_get_channels(astream_handle hdl)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	oscl_param_check(astream != NULL, 0, NULL);

	return astream->channels;
}

audio_encoder_t audio_stream_get_coding_type(astream_handle hdl)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	oscl_param_check(astream != NULL, 0, NULL);

	return astream->coding_type;
}

/**
 * audio_stream_get_cur_time - get audio stream time.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
signed long long audio_stream_get_time(astream_handle hdl)
{
	int ret = 0;
	audio_stream_t *astream = (audio_stream_t *)hdl;
	OMX_TIME_CONFIG_TIMESTAMPTYPE time;
	signed long long cur_time = 0;

	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream != NULL, -1, NULL);
	oscl_param_check(astream->asrc_info.aout != NULL, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	time.nPortIndex = astream->asrc_info.aout->index;
	ret = OMX_GetConfig(astream->asrc_info.al_comp.cmp_hdl,
			    omx_index_lombo_config_cur_time, &time);
	if (ret == 0)
		cur_time = time.nTimestamp;

	pthread_mutex_unlock(&astream->lock);
	return cur_time;
}

/**
 * audio_stream_set_sample_rate - audio_stream_set_sample_rate
 *
 * @hdl: recorder handle.
 * @rate: sample rate.
 */
int audio_stream_set_sample_rate(astream_handle hdl, int rate)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	pthread_mutex_lock(&astream->lock);
	if (rate != 0)
		astream->sample_rate = rate;
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p", hdl);
	return 0;
}

/**
 * audio_stream_set_bit_per_sample - audio_stream_set_bit_per_sample
 *
 * @hdl: recorder handle.
 * @bits: bits per sample.
 */
int audio_stream_set_sample_width(astream_handle hdl, int bits)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	pthread_mutex_lock(&astream->lock);
	if (bits == 8 || bits == 16)
		astream->bit_per_sample = bits;
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p", hdl);
	return 0;
}

/**
 * audio_stream_set_channels - audio_stream_set_channels
 *
 * @hdl: recorder handle.
 * @channels: number of channel.
 */
int audio_stream_set_channels(astream_handle hdl, int channels)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	pthread_mutex_lock(&astream->lock);
	if (channels != 0)
		astream->channels = channels;
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p", hdl);
	return 0;
}

/**
 * audio_stream_set_channels - audio_stream_set_channels
 *
 * @hdl: recorder handle.
 * @on: 1 for on, 0 for off.
 */
int audio_stream_set_aec(astream_handle hdl, int on)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	pthread_mutex_lock(&astream->lock);
	if (on)
		astream->aec_on = OMX_AUDIO_EchoCanNormal;
	else
		astream->aec_on = OMX_AUDIO_EchoCanOff;
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p", hdl);
	return 0;
}

/**
 * audio_stream_set_prio - audio_stream_set_prio
 *
 * @hdl: recorder handle.
 * @prio: prio.
 */
int audio_stream_set_prio(astream_handle hdl, int prio)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	OMX_PRIORITYMGMTTYPE priority;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	priority.nVersion.nVersion = OMX_VERSION;
	priority.nGroupPriority = prio;
	OSCL_TRACE("hdl:%p", hdl);
	OSCL_TRACE("astream:%p", astream);
	OSCL_TRACE("astream->asrc_info.al_comp.cmp_hdl:%p",
		   astream->asrc_info.al_comp.cmp_hdl);
	oscl_param_check(astream->asrc_info.al_comp.cmp_hdl, -1, NULL);
	OSCL_TRACE("astream->asrc_info.al_comp.cmp_hdl:%p",
		   astream->asrc_info.al_comp.cmp_hdl);
	OSCL_TRACE("astream->asrc_info.al_comp.cmp_hdl->SetParameter:%p",
		   astream->asrc_info.al_comp.cmp_hdl->SetParameter);
	OMX_SetParameter(astream->asrc_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);
	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream->aenc_info.al_comp.cmp_hdl, -1, NULL);
	OMX_SetParameter(astream->aenc_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);
	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream->aenc_splt.al_comp.cmp_hdl, -1, NULL);
	OMX_SetParameter(astream->aenc_splt.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);
	OSCL_TRACE("hdl:%p", hdl);
	return 0;
}


/**
 * audio_stream_set_coding - audio_stream_set_coding
 *
 * @hdl: recorder handle.
 * @coding: coding type.
 */
int audio_stream_set_coding_type(astream_handle hdl, audio_encoder_t coding)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	pthread_mutex_lock(&astream->lock);
	if (coding > AUDIO_ENCODER_INVALID && coding < AUDIO_ENCODER_UNSUPPORT)
		astream->coding_type = coding;
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p", hdl);
	return 0;
}

/**
 * audio_stream_set_mute - audio_stream_set_mute
 *
 * @hdl: recorder handle.
 * @coding: coding type.
 */
int audio_stream_set_mute(astream_handle hdl, int mute_on)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	OMX_AUDIO_CONFIG_MUTETYPE mute;
	int ret;

	OSCL_TRACE("hdl:%p", hdl);
	if (astream == NULL) {
		OSCL_LOGE("input hdl err!");
		return -1;
	}
	pthread_mutex_lock(&astream->lock);
	memset(&mute, 0, sizeof(OMX_AUDIO_CONFIG_MUTETYPE));
	if (mute_on)
		mute.bMute = OMX_TRUE;
	mute.nPortIndex = astream->asrc_info.aout->index;
	ret = OMX_SetConfig(astream->asrc_info.al_comp.cmp_hdl,
			    OMX_IndexConfigAudioMute, &mute);
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p", hdl);
	return ret;
}


/**
 * audio_stream_creat - get a astream handle.
 * This function will load all components audio stream needs.
 *
 * Returns audio stream handle on success, 0 otherwise..
 */
astream_handle audio_stream_creat(void)
{
	audio_stream_t *astream;
	OMX_PARAM_PORTDEFINITIONTYPE omx_port;
	int index;
	int ret = 0;

	if (g_audio_rec.lock_init == 0) {
		memset(&g_audio_rec, 0, sizeof(audio_stream_t));
		pthread_mutex_init(&g_audio_rec.lock, NULL);
		g_audio_rec.lock_init = 1;
	}

	astream = &g_audio_rec;
	pthread_mutex_lock(&astream->lock);
	astream->ref++;
	if (astream->ref > 1)
		goto EXIT;

	/* initialize audio config */
	_get_config_audio(astream);

	/* init audio source component */
	ret = component_init(&astream->asrc_info.al_comp, "OMX.LB.SOURCE.AREC",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&astream->asrc_info.al_comp, OMX_DirOutput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	astream->asrc_info.aout = &astream->asrc_info.al_comp.port_info[index];

	/* init audio enc component */
	ret = component_init(&astream->aenc_info.al_comp, "OMX.LB.ENCODER.AUDIO",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&astream->aenc_info.al_comp, OMX_DirInput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	astream->aenc_info.ain = &astream->aenc_info.al_comp.port_info[index];
	index = component_get_port_index(&astream->aenc_info.al_comp, OMX_DirOutput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	astream->aenc_info.aout = &astream->aenc_info.al_comp.port_info[index];
	OSCL_LOGE("aenc buffer number:%d", astream->aenc_info.aout->nbuffer);

	/* init audio encoder splitter component */
	ret = component_init(&astream->aenc_splt.al_comp, "OMX.LB.SPLITTER.BASE",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	astream->aenc_splt.in = &astream->aenc_splt.al_comp.port_info[0];

	/* init aenc splitter port definition */
	omx_port.nPortIndex = astream->aenc_info.aout->index;
	omx_port.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(astream->aenc_info.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &omx_port);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	_asplt_prepare(&astream->aenc_splt, &omx_port);

	/* init audio source splitter component */
	ret = component_init(&astream->asrc_splt.al_comp, "OMX.LB.SPLITTER.BASE",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	astream->asrc_splt.in = &astream->asrc_splt.al_comp.port_info[0];

	/* init asrc splitter port definition */
	omx_port.nPortIndex = astream->asrc_info.aout->index;
	omx_port.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(astream->asrc_info.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &omx_port);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	_asplt_prepare(&astream->asrc_splt, &omx_port);

	astream->state = AS_STATE_INIT;

EXIT:
	pthread_mutex_unlock(&astream->lock);
	if (ret != 0) {
		OSCL_LOGE("astream creat fail:%x", ret);
		audio_stream_release(astream);
	}
	OSCL_TRACE("astream:%p", astream);
	return astream;
}

/**
 * audio_stream_release - release resource recorder used.
 *
 * @hdl: recorder handle.
 */
void audio_stream_release(astream_handle hdl)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	int ret = 0;

	OSCL_TRACE("astream:%p", hdl);
	if (astream == NULL)
		return;

	/* only deinit astream when all audio handle released */
	pthread_mutex_lock(&astream->lock);
	if (astream->ref <= 0) {
		astream->ref = 0;
		OSCL_LOGE("astream already released!");
		pthread_mutex_unlock(&astream->lock);
		return;
	}
	if (astream->ref > 1) {
		astream->ref--;
		OSCL_TRACE("astream->ref:%d", astream->ref);
		pthread_mutex_unlock(&astream->lock);
		return;
	}
	ret = _astream_reset(hdl);
	oscl_param_check_exit(ret == 0, -1, NULL);

	component_deinit(&astream->aenc_splt.al_comp);
	component_deinit(&astream->aenc_info.al_comp);
	component_deinit(&astream->asrc_splt.al_comp);
	component_deinit(&astream->asrc_info.al_comp);
	astream->ref = 0;
	astream->state = AS_STATE_ERR;

EXIT:
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p astream->state:%d", hdl, astream->state);
}

/**
 * audio_stream_inc_refcnt - increase ref count of audio stream.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_inc_refcnt(astream_handle hdl)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	int ret = 0;

	OSCL_TRACE("start");
	oscl_param_check(astream != NULL, -1, NULL);
	pthread_mutex_lock(&astream->lock);
	if (astream->state == AS_STATE_ERR)
		ret = -1;
	else
		astream->ref++;
	pthread_mutex_unlock(&astream->lock);

	return ret;
}


/**
 * audio_stream_start - start audio stream.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_start(astream_handle hdl)
{
	int ret = 0;
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream != NULL, -1, NULL);
	oscl_param_check(astream->state != AS_STATE_ERR, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	ret = _astream_start(astream);
	if (ret != 0) {
		OSCL_LOGE("start astream failed, try stop!");
		_astream_reset(hdl);
	}
	pthread_mutex_unlock(&astream->lock);

	OSCL_TRACE("hdl:%p", hdl);
	return ret;
}

/**
 * audio_stream_stop - stop audio stream.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_stop(astream_handle hdl)
{
	int ret = 0;
	audio_stream_t *astream = (audio_stream_t *)hdl;
	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream != NULL, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	ret = _astream_reset(astream);
	pthread_mutex_unlock(&astream->lock);
	OSCL_TRACE("hdl:%p astream->state:%d", hdl, astream->state);
	return ret;
}

/**
 * audio_stream_start_enc - start audio encode.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
port_info_t *audio_stream_start_enc(astream_handle hdl)
{
	int ret = 0;
	audio_stream_t *astream = (audio_stream_t *)hdl;
	port_info_t *port = NULL;

	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream != NULL, NULL, NULL);
	oscl_param_check(astream != AS_STATE_ERR, NULL, NULL);

	pthread_mutex_lock(&astream->lock);
	if (astream->state == AS_STATE_INIT)
		ret = _astream_start(astream);
	if (astream->state == AS_STATE_START)
		ret = _astream_start_enc(astream);
	if (ret == 0)
		port = _splt_get_outport(&astream->aenc_splt);
	pthread_mutex_unlock(&astream->lock);

	OSCL_TRACE("astream:%p astream->state:%d", astream, astream->state);
	return port;
}

/**
 * audio_stream_stop_enc - stop audio encode.
 *
 * @hdl: audio stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_stop_enc(astream_handle hdl, port_info_t *enc_port)
{
	int ret = 0;
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("hdl:%p", hdl);
	oscl_param_check(astream != NULL, -1, NULL);
	oscl_param_check(astream != AS_STATE_ERR, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	if (enc_port)
		_splt_put_outport(&astream->aenc_splt, enc_port);
	ret = _astream_stop_enc(astream);
	pthread_mutex_unlock(&astream->lock);

	return ret;
}

/**
 * audio_stream_getport - get a free port from astream.
 *
 * @hdl: audio stream handle
 *
 * @type: pcm port or encoded port to get
 *
 * Returns port handle on success, NULL otherwise..
 */
port_info_t *audio_stream_getport(astream_handle hdl, as_port_type_e type)
{
	port_info_t *port = NULL;
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("==");
	oscl_param_check(astream != NULL, NULL, NULL);

	pthread_mutex_lock(&astream->lock);
	if (type == AS_PORT_CODEC)
		port = _splt_get_outport(&astream->aenc_splt);
	if (type == AS_PORT_PCM)
		port = _splt_get_outport(&astream->asrc_splt);
	pthread_mutex_unlock(&astream->lock);
	OSCL_LOGI("get port:%p", port);
	return port;
}

/**
 * audio_stream_putport - free a port get from astream.
 *
 * @hdl: audio stream handle
 *
 * @port: port to be freed
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_putport(astream_handle hdl, port_info_t *port)
{
	audio_stream_t *astream = (audio_stream_t *)hdl;
	int ret = -1;

	OSCL_TRACE("==");
	oscl_param_check(astream != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	if (port->comp_info == &astream->aenc_splt.al_comp)
		ret = _splt_put_outport(&astream->aenc_splt, port);
	if (port->comp_info == &astream->asrc_splt.al_comp)
		ret = _splt_put_outport(&astream->asrc_splt, port);
	pthread_mutex_unlock(&astream->lock);

	OSCL_TRACE("==ret:%d", ret);
	return ret;
}

/**
 * audio_stream_enable_port - enable a port get from astream.
 *
 * @hdl: audio stream handle
 *
 * @port: port to be enabled
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_enable_port(void *hdl, port_info_t *port)
{
	int ret = -1;
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("==");
	oscl_param_check(astream != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	if (port->comp_info == &astream->aenc_splt.al_comp)
		ret = _splt_enable_port(&astream->aenc_splt, port);
	if (port->comp_info == &astream->asrc_splt.al_comp)
		ret = _splt_enable_port(&astream->asrc_splt, port);
	pthread_mutex_unlock(&astream->lock);

	OSCL_TRACE("==");
	return ret;
}

/**
 * audio_stream_disable_port - disable a port get from astream.
 *
 * @hdl: audio stream handle
 *
 * @port: port to be disabled
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int audio_stream_disable_port(void *hdl, port_info_t *port)
{
	int ret = -1;
	audio_stream_t *astream = (audio_stream_t *)hdl;

	OSCL_TRACE("==");
	oscl_param_check(astream != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	pthread_mutex_lock(&astream->lock);
	if (port->comp_info == &astream->aenc_splt.al_comp)
		ret = _splt_disable_port(&astream->aenc_splt, port);
	if (port->comp_info == &astream->asrc_splt.al_comp)
		ret = _splt_disable_port(&astream->asrc_splt, port);
	pthread_mutex_unlock(&astream->lock);

	OSCL_TRACE("==");
	return ret;
}

