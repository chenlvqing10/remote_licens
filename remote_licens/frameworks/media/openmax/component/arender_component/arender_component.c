#define DBG_LEVEL              DBG_INFO
#define LOG_TAG                "arend_component"

#include <oscl.h>
#include <base_component.h>
#include <alsa/asoundlib.h>
#include "arender_component.h"
#include "clock_component.h"
#include "clock_port.h"
#include "audio_gain.h"

/* #define SAVE_PCM_FILE */

#define HW_OFFSET_THRESH	400000
#define DEFAULT_PERIOD_CNT	4
#define DEFAULT_PERIOD_SIZE	1024
#define DEFAULT_BUFFER_SIZE	4096
#define DEFAULT_VOLUME		87

typedef struct arender_ctx {
	snd_pcm_t	*pcm_dev; /* pcm device */
	void		*audio_gain;
	OMX_U32		volume;
	OMX_U32		channels; /* audio steam channels */
	OMX_U32		samplefmt; /* bits per sample */
	OMX_U32		samplerate; /* sample rate of audio sream */
	OMX_U32		bytes_perframe;
	pthread_mutex_t lock;
#ifdef SAVE_PCM_FILE
	FILE *outfile;
#endif
	OMX_TIME_CLOCKSTATE clk_state; /* clk state */
	OMX_S32             xscale;    /* scale of the clk */
	OMX_S64             audio_hw_offset;
	OMX_BOOL            avsync;
	OMX_BOOL            fmt_changed;

	OMX_TICKS           pre_buf_ts;
	OMX_TICKS           cal_next_ts;
	OMX_BOOL            sw_refclk;

	OMX_BOOL clkbuf_thd_run;
	pthread_t clkbuf_thd;

	OMX_BOOL drop_data;
} arender_ctx_t;

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

static OMX_ERRORTYPE audio_render_init(lb_omx_component_t *component)
{
	OMX_S32 ret = 0;
	arender_ctx_t *audio_ctx = NULL;
	snd_pcm_hw_params_t *hw;

	audio_ctx = component->component_private;
	pthread_mutex_lock(&audio_ctx->lock);
	audio_ctx->audio_gain = audio_gain_open(audio_ctx->channels,
			audio_ctx->samplefmt);
	if (audio_ctx->audio_gain == NULL) {
		OSCL_LOGE("open gain err\n");
		goto GAIN_OPEN_ERR;
	}
	audio_gain_set_volume(audio_ctx->audio_gain, audio_ctx->volume);
	ret = snd_pcm_open(&audio_ctx->pcm_dev, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (ret < 0) {
		OSCL_LOGE("open pcm err\n");
		goto PCM_OPEN_ERR;
	}
	snd_pcm_hw_params_malloc(&hw);
	snd_pcm_hw_params_any(audio_ctx->pcm_dev, hw);
	snd_pcm_hw_params_set_access(audio_ctx->pcm_dev,
		hw, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(audio_ctx->pcm_dev,
		hw, to_pcm_fmt(audio_ctx->samplefmt));
	snd_pcm_hw_params_set_channels(audio_ctx->pcm_dev, hw, audio_ctx->channels);
	snd_pcm_hw_params_set_rate(audio_ctx->pcm_dev, hw, audio_ctx->samplerate, 0);
	snd_pcm_hw_params_set_period_size(audio_ctx->pcm_dev, hw, DEFAULT_PERIOD_SIZE, 0);
	snd_pcm_hw_params_set_buffer_size(audio_ctx->pcm_dev, hw, DEFAULT_BUFFER_SIZE);
	if (snd_pcm_hw_params(audio_ctx->pcm_dev, hw) < 0) {
		OSCL_LOGE("snd_pcm_hw_params error\n");
		goto PCM_HW_PARAMS_ERR;
	}
	snd_pcm_hw_params_free(hw);
	pthread_mutex_unlock(&audio_ctx->lock);
#ifdef SAVE_PCM_FILE
	audio_ctx->outfile = fopen("aacout.pcm", "wb");
	if (audio_ctx->outfile == NULL)
		OSCL_LOGE("open out file err!\n");
#endif
	OSCL_LOGI("audio receive init!");
	return ret;

PCM_HW_PARAMS_ERR:
	snd_pcm_close(audio_ctx->pcm_dev);
	audio_ctx->pcm_dev = NULL;
PCM_OPEN_ERR:
	audio_gain_close(audio_ctx->audio_gain);
	audio_ctx->audio_gain = NULL;
GAIN_OPEN_ERR:
	pthread_mutex_unlock(&audio_ctx->lock);
	return OMX_ErrorResourcesLost;
}

static void audio_render_exit(lb_omx_component_t *component)
{
	arender_ctx_t *audio_ctx = component->component_private;

	if (audio_ctx->pcm_dev == NULL)
		return;
	pthread_mutex_lock(&audio_ctx->lock);
	snd_pcm_close(audio_ctx->pcm_dev);
	audio_ctx->pcm_dev = NULL;
	audio_gain_close(audio_ctx->audio_gain);
	audio_ctx->audio_gain = NULL;
	pthread_mutex_unlock(&audio_ctx->lock);
#ifdef SAVE_PCM_FILE
	fclose(audio_ctx->outfile);
#endif
}

static void *clkbuf_manager(void *param)
{
	lb_omx_component_t *comp = param;
	arender_ctx_t *priv = comp->component_private;
	base_port_t *clkport = &comp->port[ARENDER_CLOCK_PORT];
	OMX_BUFFERHEADERTYPE *clk_buffer = NULL;

	OSCL_LOGD("running");
	while (priv->clkbuf_thd_run) {
		sem_wait(&clkport->buffer_sem);
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		if (clk_buffer == NULL)
			continue;

		OMX_TIME_MEDIATIMETYPE *mediatime;
		mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
		if (mediatime->eUpdateType == OMX_TIME_UpdateScaleChanged) {
			priv->xscale = mediatime->xScale;
			OSCL_LOGI("got scale changed scale %d\n", priv->xscale);
		} else if (mediatime->eUpdateType == OMX_TIME_UpdateClockStateChanged) {
			priv->clk_state = mediatime->eState;
			OSCL_LOGI("clock state change %d\n", priv->clk_state);
		}
		clkport->return_buffer(clkport, clk_buffer);
	}

	OSCL_LOGD("exit");
	return NULL;
}

static int clk_buf_init(lb_omx_component_t *comp)
{
	arender_ctx_t *priv = comp->component_private;
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
	arender_ctx_t *priv = comp->component_private;
	base_port_t *clkport = &comp->port[ARENDER_CLOCK_PORT];

	OSCL_LOGD("joining clkbuf_thd");
	priv->clkbuf_thd_run = OMX_FALSE;
	sem_post(&clkport->buffer_sem);
	pthread_join(priv->clkbuf_thd, NULL);
	OSCL_LOGD("clkbuf_thd joined");
}

static OMX_BOOL clock_handle_frame(lb_omx_component_t *component,
	OMX_BUFFERHEADERTYPE *inbuffer)
{
	base_port_t *clkport = NULL;
	arender_ctx_t *audio_ctx = NULL;
	OMX_HANDLETYPE                        clk_cmp = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE         client_timestamp;
	OMX_ERRORTYPE                         err;

	audio_ctx = component->component_private;
	clkport = &component->port[ARENDER_CLOCK_PORT];
	clk_cmp = clkport->tunneled_component;

	/* if first timestamp is received, then notify clk component */
	if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {
		OSCL_LOGI("audio first timestamp received %lld\n", inbuffer->nTimeStamp);
		client_timestamp.nSize = sizeof(client_timestamp);
		client_timestamp.nPortIndex = clkport->tunnel_ports;
		client_timestamp.nTimestamp =
			inbuffer->nTimeStamp - audio_ctx->audio_hw_offset;
		if (client_timestamp.nTimestamp < 0)
			client_timestamp.nTimestamp = inbuffer->nTimeStamp;
		err = OMX_SetConfig(clk_cmp,
			OMX_IndexConfigTimeClientStartTime, &client_timestamp);
		if (err != OMX_ErrorNone)
			OSCL_LOGE("set client start time error!\n");

		/* wait for state change notification */
		int i = 0;
		while (audio_ctx->clk_state != OMX_TIME_ClockStateRunning) {
			if (i++ > 1000) {
				OSCL_LOGW("wait clock running failed!");
				break;
			}
			oscl_mdelay(1);
		}

#ifdef __EOS__
		pcm_flush(audio_ctx->pcm_dev, PCM_OUT);
#endif

		/* first frame, send it anyway */
		return OMX_TRUE;
	}

	/* do not send the data to sink and return back,
	* if the clock is not running */
	if (audio_ctx->clk_state != OMX_TIME_ClockStateRunning
		&& !(inbuffer->nFlags & OMX_BUFFERFLAG_EOS)) {
		OSCL_LOGD("clock is not running now, drop this frame!\n");
		inbuffer->nFilledLen = 0;
		return OMX_FALSE;
	}

	OSCL_LOGD("buffer ts %lld, hw_offset=%lld", inbuffer->nTimeStamp,
		audio_ctx->audio_hw_offset);
	/* update mediatime base */
	client_timestamp.nPortIndex = clkport->tunnel_ports;

	OMX_TICKS ts;
	if (inbuffer->nTimeStamp != audio_ctx->pre_buf_ts) {
		ts = inbuffer->nTimeStamp;
		audio_ctx->pre_buf_ts = inbuffer->nTimeStamp;
		audio_ctx->cal_next_ts = inbuffer->nTimeStamp +
			(OMX_TICKS)inbuffer->nFilledLen * 1000000 * 8 /
			(audio_ctx->samplerate * audio_ctx->channels *
			audio_ctx->samplefmt);
	} else {
		ts = audio_ctx->cal_next_ts;
		audio_ctx->cal_next_ts += (OMX_TICKS)inbuffer->nFilledLen * 1000000 * 8 /
			(audio_ctx->samplerate * audio_ctx->channels *
			audio_ctx->samplefmt);
	}
	client_timestamp.nTimestamp = ts - audio_ctx->audio_hw_offset;

	if (inbuffer->nFlags & OMX_BUFFERFLAG_EOS)
		client_timestamp.nTimestamp = 0xffffffff;
	/* OSCL_LOGI("timestamp received %lld\n", inbuffer->nTimeStamp); */
	OSCL_LOGD("config current audio timestamp %lld", client_timestamp.nTimestamp);
	err = OMX_SetConfig(clk_cmp,
		OMX_IndexConfigTimeCurrentAudioReference,
		&client_timestamp);
	if (err != OMX_ErrorNone)
		OSCL_LOGE("set audio ref time error!\n");

	return OMX_TRUE;
}

void arender_buffer_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	int ret;
	arender_ctx_t *audio_ctx = NULL;
	lb_omx_component_t *component = NULL;

	component = get_lb_component(stand_com);
	audio_ctx = component->component_private;
	if (inbuffer == NULL || inbuffer->pBuffer == NULL) {
		OSCL_LOGE("inbuffer:%p, nFilledLen:%d, pBuffer:%p!!",
			inbuffer, inbuffer->nFilledLen, inbuffer->pBuffer);
		OSCL_LOGE("err param!");
		return;
	}
	OSCL_LOGD("fill len %d, offset %d, bytes_perframe=%d",
		inbuffer->nFilledLen, inbuffer->nOffset, audio_ctx->bytes_perframe);

	if (inbuffer->nFilledLen == 0) {
		OSCL_LOGW("nFilledLen = 0, return!\n");
		return;
	}
	if (inbuffer->nOffset > inbuffer->nFilledLen) {
		OSCL_LOGE("param error! offset[%d]>filledlen[%d]",
			inbuffer->nOffset, inbuffer->nFilledLen);
		inbuffer->nOffset = 0;
		inbuffer->nFilledLen = 0;
		return;
	}
#ifdef SAVE_PCM_FILE
	if (inbuffer->nOffset == 0)
		fwrite(inbuffer->pBuffer, 1, inbuffer->nFilledLen, audio_ctx->outfile);
#endif
	if (audio_ctx->pcm_dev == NULL || audio_ctx->fmt_changed) {
		if (audio_ctx->pcm_dev != NULL) {
			OSCL_LOGI("closed pcm dev first");
			audio_render_exit(component);
		}
		ret = audio_render_init(component);
		if (OMX_ErrorNone != ret) {
			((component->callbacks.EventHandler)) (&component->base_comp,
			component->callback_data, OMX_EventError, ret, -1, NULL);
			return;
		}
		audio_ctx->fmt_changed = OMX_FALSE;
		audio_ctx->pre_buf_ts = 0;
		audio_ctx->cal_next_ts = 0;
	}

	OMX_U8 *p = inbuffer->pBuffer + inbuffer->nOffset;
	int remain = inbuffer->nFilledLen - inbuffer->nOffset;
	int frames = remain / audio_ctx->bytes_perframe;
	if (frames <= 0) {
		OSCL_LOGE("inbuf data len error!(filllen=%d, ofs=%d, sampleBytes=%d)",
			inbuffer->nFilledLen, inbuffer->nOffset,
			audio_ctx->bytes_perframe);
		inbuffer->nOffset = inbuffer->nFilledLen;
		return;
	}
	pthread_mutex_lock(&audio_ctx->lock);
	if (audio_ctx->pcm_dev == NULL) {
		pthread_mutex_unlock(&audio_ctx->lock);
		OSCL_LOGE("pcm dev is closed\n");
		return;
	}
	audio_gain_apply(audio_ctx->audio_gain, p, p, remain);
	ret = snd_pcm_writei(audio_ctx->pcm_dev, p, frames);
	if (ret == -EPIPE) {
		int res;
		OSCL_LOGW("snd_pcm_writei err EPIPE, underrun occurred");
		res = snd_pcm_prepare(audio_ctx->pcm_dev);
		if (res < 0)
			OSCL_LOGE("snd_pcm_prepare err(%d, %s)", res, snd_strerror(res));
		pthread_mutex_unlock(&audio_ctx->lock);
		inbuffer->nOffset = 0;
		inbuffer->nFilledLen = 0;
		return;
	} else if (ret < 0) {
		pthread_mutex_unlock(&audio_ctx->lock);
		OSCL_LOGE("write to pcm err(%d, %s)", ret, snd_strerror(ret));
		return;
	} else if (ret != frames)
		OSCL_LOGW("write to pcm %d!=%d", ret, frames);
	pthread_mutex_unlock(&audio_ctx->lock);
	inbuffer->nOffset += ret * audio_ctx->bytes_perframe;
}

void *arender_buffer_manager(void *param)
{
	lb_omx_component_t *component = NULL;
	OMX_COMPONENTTYPE *stand_comp = (OMX_COMPONENTTYPE *)param;
	base_port_t *inport = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;
	arender_ctx_t *audio_ctx = NULL;
	OMX_S64 wait_buf_time = oscl_get_msec();

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(stand_comp);
	audio_ctx = component->component_private;
	inport = &component->port[BASE_INPUT_PORT];

	while (1) {
		OMX_STATETYPE state = component->state;
		if (state != OMX_StateIdle
			&& state != OMX_StateExecuting
			&& state != OMX_StatePause) {
			OSCL_LOGD("comp-state=%d, state=%d, exit",
				component->state, state);
			break;
		}

		/*Wait till the ports are being flushed*/
		pthread_mutex_lock(&component->flush_mutex);
		while (inport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			if (inbuffer != NULL) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
				OSCL_LOGI("retrun buffer while flushing port");
			}
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			OSCL_LOGW("%s flush in buffer manager thread\n",
				get_component_name(component));
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
			oscl_queue_get_num(&inport->buffer_queue), inbuffer);
		if (inbuffer == NULL) {
			inbuffer = oscl_queue_dequeue(&inport->buffer_queue);
			if (inbuffer == NULL) {
				OMX_S64 cur_time = oscl_get_msec();
				int ret;
				if (cur_time - wait_buf_time > 1000) {
					OSCL_LOGW("wait buffer for over %lld ms",
						cur_time - wait_buf_time);
					wait_buf_time = cur_time;
				}
				ret = oscl_sem_timedwait_ms(
					component->buf_mgnt_sem,
					1000);
				if (ret == -1) {
					if (errno == ETIMEDOUT) {
						OSCL_LOGE("wait timeout!");
					} else {
						OSCL_LOGE("wait error(%s)!",
							strerror(errno));
					}
				}
				continue;
			}
			wait_buf_time = oscl_get_msec();
		}

		OSCL_LOGD("buffer size=%d, offset=%d, ts=%lld",
			inbuffer->nFilledLen, inbuffer->nOffset, inbuffer->nTimeStamp);
		if (audio_ctx->avsync &&
			!clock_handle_frame(component, inbuffer)) {
			inbuffer->nOffset = 0;
			inbuffer->nFilledLen = 0;
			inbuffer->nFlags = 0;
			inport->return_buffer(inport, inbuffer);
			inbuffer = NULL;
			OSCL_LOGD("skip one aduio frame\n");
			continue;
		}

		if (component->mark.hMarkTargetComponent) {
			inbuffer->hMarkTargetComponent
				= component->mark.hMarkTargetComponent;
			inbuffer->pMarkData
				= component->mark.pMarkData;
			component->mark.hMarkTargetComponent = NULL;
			component->mark.pMarkData = NULL;
		}

		OSCL_LOGD("component->state:%x,inbuffer->nFilledLen:%d",
			component->state,
			inbuffer->nFilledLen);
		/* only when we're at executing state, we handle the buffer */
		if (component->state == OMX_StateExecuting) {
			if (component->buf_handle && !audio_ctx->drop_data)
				(*(component->buf_handle))(stand_comp, inbuffer, NULL);
		}

		if (audio_ctx->drop_data) {
			inbuffer->nOffset = 0;
			inbuffer->nFilledLen = 0;
		}

		OSCL_LOGD("state:%x,inbuf=%p(nFilledLen=%d,nOffset=%d)",
			component->state, inbuffer,
			inbuffer->nFilledLen, inbuffer->nOffset);

		if (inbuffer->hMarkTargetComponent == stand_comp) {
			(*(component->callbacks.EventHandler))
			(stand_comp,
				component->callback_data,
				OMX_EventMark,
				0,
				0,
				inbuffer->pMarkData);
		}
		if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {
			OSCL_LOGI("start ts %lld", inbuffer->nTimeStamp);
			inbuffer->nFlags &= (~OMX_BUFFERFLAG_STARTTIME);
			(*(component->callbacks.EventHandler))
			(stand_comp, component->callback_data,
				OMX_EventOutputRendered, 0, 0, NULL);
			OSCL_LOGD("out put render\n");
		}
		/* if we have an eos flag, then notify app from eventHandler */
		if (inbuffer->nFlags & OMX_BUFFERFLAG_EOS &&
			inbuffer->nFilledLen == inbuffer->nOffset) {
			OSCL_LOGI("EOS ts %lld", inbuffer->nTimeStamp);
			(*(component->callbacks.EventHandler))
			(stand_comp,
				component->callback_data,
				OMX_EventBufferFlag,
				inport->port_param.nPortIndex,
				OMX_BUFFERFLAG_EOS,
				NULL);
			component->eos_flag = OMX_TRUE;
		}
		/* if we successfully fill the buffer, then return it */
		if (inbuffer->nFilledLen == inbuffer->nOffset
			|| (inbuffer->nFlags & OMX_BUFFERFLAG_EOS)) {
			OSCL_LOGD("inport return_buffer");
			inbuffer->nOffset = 0;
			inbuffer->nFilledLen = 0;
			inbuffer->nFlags = 0;
			inport->return_buffer(inport, inbuffer);
			inbuffer = NULL;
		}
	}
	if ((inbuffer != NULL) && (!PORT_IS_SUPPLIER(inport))) {
		OSCL_LOGI("outport return_buffer before exit");
		inport->return_buffer(inport, inbuffer);
	}
	oscl_queue_flush(&inport->buffer_queue);
	OSCL_LOGI("exit from arender_buffer_manager\n");
	OSCL_LOGW("exit, inport buffer left %d\n",
		inport->buffer_queue.count);
	OSCL_TRACE(" %p\n", param);

	return NULL;

}

OMX_ERRORTYPE arender_set_config(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE index,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	arender_ctx_t *audio_ctx = NULL;
	OMX_AUDIO_PARAM_PCMMODETYPE *audio_params = NULL;
	OMX_S64 offset;

	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, index);
	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, "invalid param!");
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (index) {
	case OMX_IndexParamAudioPcm: {
		audio_params = (OMX_AUDIO_PARAM_PCMMODETYPE *)paramData;
		if (audio_params->nChannels == 0
			|| audio_params->nBitPerSample == 0
			|| audio_params->nSamplingRate == 0)
			break;
		if (audio_ctx->channels != audio_params->nChannels ||
			audio_ctx->samplefmt != audio_params->nBitPerSample ||
			audio_ctx->samplerate != audio_params->nSamplingRate) {
			OSCL_LOGW("audio fmt change");
			audio_ctx->fmt_changed = OMX_TRUE;
			audio_ctx->channels = audio_params->nChannels;
			audio_ctx->samplefmt = audio_params->nBitPerSample;
			audio_ctx->samplerate = audio_params->nSamplingRate;
			audio_ctx->bytes_perframe =
				audio_ctx->samplefmt * audio_ctx->channels / 8;
			offset = DEFAULT_PERIOD_CNT * DEFAULT_PERIOD_SIZE * 1e6 /
				audio_ctx->samplerate;
			/* use 2/3 of the hw_offset */
			audio_ctx->audio_hw_offset = offset * 2 / 3;
			if (audio_ctx->audio_hw_offset > HW_OFFSET_THRESH)
				audio_ctx->audio_hw_offset = HW_OFFSET_THRESH;
			OSCL_LOGI("sr=%d, ch=%d, bits=%d, frame bytes %d, hw_offset %lld",
				audio_ctx->samplerate,
				audio_ctx->channels,
				audio_ctx->samplefmt,
				audio_ctx->bytes_perframe,
				audio_ctx->audio_hw_offset);
		}
		break;
	}
	case OMX_IndexConfigAudioVolume: {
		OMX_AUDIO_CONFIG_VOLUMETYPE *volume =
			(OMX_AUDIO_CONFIG_VOLUMETYPE *)paramData;
		audio_ctx->volume = volume->sVolume.nValue;
		if (audio_ctx->audio_gain)
			audio_gain_set_volume(audio_ctx->audio_gain,
				audio_ctx->volume);
		break;
	}
	default:
		OSCL_LOGI("default set config index %d", index);
		ret = base_set_config(hComp, index, paramData);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE arender_get_config(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE index,
	OMX_INOUT OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	arender_ctx_t *audio_ctx = NULL;

	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, index);
	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, "invalid param!");
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (index) {
	case OMX_IndexConfigAudioVolume: {
		OMX_AUDIO_CONFIG_VOLUMETYPE *volume =
			(OMX_AUDIO_CONFIG_VOLUMETYPE *)paramData;
		volume->sVolume.nValue = audio_ctx->volume;
		break;
	}
	default:
		OSCL_LOGI("default set config index %d", index);
		ret = base_get_config(hComp, index, paramData);
		break;
	}

	return ret;
}

OMX_ERRORTYPE arender_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	arender_ctx_t *audio_ctx = NULL;
	OMX_AUDIO_PARAM_PCMMODETYPE *audio_params = NULL;
	OMX_S64 offset;

	audio_params = (OMX_AUDIO_PARAM_PCMMODETYPE *)paramData;
	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, paramIndex);
	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_ctx = component->component_private;

	switch (paramIndex) {
	case OMX_IndexParamAudioPcm:
		if (audio_params->nChannels == 0
			|| audio_params->nBitPerSample == 0
			|| audio_params->nSamplingRate == 0)
			break;
		audio_ctx->channels = audio_params->nChannels;
		audio_ctx->samplefmt = audio_params->nBitPerSample;
		audio_ctx->samplerate = audio_params->nSamplingRate;
		audio_ctx->bytes_perframe =
			audio_ctx->samplefmt * audio_ctx->channels / 8;
		offset = DEFAULT_PERIOD_CNT * DEFAULT_PERIOD_SIZE * 1e6 /
			audio_ctx->samplerate;
		/* use 2/3 of the hw_offset */
		audio_ctx->audio_hw_offset = offset * 2 / 3;
		if (audio_ctx->audio_hw_offset > HW_OFFSET_THRESH)
			audio_ctx->audio_hw_offset = HW_OFFSET_THRESH;
		OSCL_LOGI("samplerate=%d, ch=%d, bits=%d, frame bytes %d, hw_offset %lld",
			audio_ctx->samplerate,
			audio_ctx->channels,
			audio_ctx->samplefmt,
			audio_ctx->bytes_perframe,
			audio_ctx->audio_hw_offset);
		break;
	case omx_index_media_avsync:
		audio_ctx->avsync = *((OMX_BOOL *)paramData);
		OSCL_LOGI("avsync %d\n", audio_ctx->avsync);
		break;
	case omx_index_media_drop_data:
		audio_ctx->drop_data = *((OMX_BOOL *)paramData);
		OSCL_LOGI("drop data %d", audio_ctx->drop_data);
		break;
	default:
		OSCL_LOGI("default base set params index %d\n", paramIndex);
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE arender_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	OMX_STATETYPE pre_state;

	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));

#if 0
	if (dest_state == OMX_StateExecuting &&
		(component->state == OMX_StateIdle ||
		component->state == OMX_StatePause)) {
		arender_ctx_t *audio_ctx = component->component_private;
		if (audio_ctx->pcm_dev != NULL) {
			OSCL_LOGW("Device not closed while entering StateIdle");
			audio_render_exit(component);
		}
		ret = audio_render_init(component);
		if (OMX_ErrorNone != ret)
			return ret;
	}
#endif

	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);
	if (dest_state == OMX_StatePause && pre_state == OMX_StateExecuting) {
		OSCL_LOGI("audio receiver exit\n");
		audio_render_exit(component);
	}
	if (dest_state == OMX_StateIdle &&
		(pre_state == OMX_StateExecuting || pre_state == OMX_StatePause)) {
		OSCL_LOGI("audio receiver exit\n");
		audio_render_exit(component);
	}

	return ret;

}


OMX_ERRORTYPE arender_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *base_cmp = (OMX_COMPONENTTYPE *)hComponent;
	lb_omx_component_t *component = NULL;
	arender_ctx_t *audio_ctx = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	OSCL_TRACE("base_cmp_handle:%p\n", hComponent);
	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = (lb_omx_component_t *)base_cmp->pComponentPrivate;
	audio_ctx = (arender_ctx_t *)component->component_private;

	clk_buf_deinit(component);

	component->port[BASE_INPUT_PORT].deinit(
		&component->port[BASE_INPUT_PORT]);
	clock_port_deinit(&component->port[ARENDER_CLOCK_PORT]);
	pthread_mutex_destroy(&audio_ctx->lock);
	oscl_free(audio_ctx);
	ret = base_component_deinit(hComponent);

	return ret;
}

OMX_ERRORTYPE arender_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	arender_ctx_t *audio_ctx = NULL;
	OMX_S64 offset;

	audio_ctx = oscl_zalloc(sizeof(arender_ctx_t));
	if (!audio_ctx) {
		OSCL_LOGE("malloc arender_ctx_t error!\n");
		return OMX_ErrorInsufficientResources;
	}
	pthread_mutex_init(&audio_ctx->lock, NULL);
	audio_ctx->volume = DEFAULT_VOLUME;
	audio_ctx->channels = 2;
	audio_ctx->samplefmt = 16;
	audio_ctx->samplerate = 8000;
	audio_ctx->bytes_perframe =
		audio_ctx->samplefmt * audio_ctx->channels / 8;
	offset = DEFAULT_PERIOD_CNT * DEFAULT_PERIOD_SIZE * 1e6 /
		audio_ctx->samplerate;
	/* use 2/3 of the hw_offset */
	audio_ctx->audio_hw_offset = offset * 2 / 3;
	if (audio_ctx->audio_hw_offset > HW_OFFSET_THRESH)
		audio_ctx->audio_hw_offset = HW_OFFSET_THRESH;

	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_component_init error!\n");
		goto error1;
	}
	cmp_handle->name = "OMX.LB.SINK.ARENDER";
	cmp_handle->component_private = audio_ctx;
	cmp_handle->buf_manager = arender_buffer_manager;
	cmp_handle->buf_handle = arender_buffer_handle;
	cmp_handle->base_comp.ComponentDeInit = arender_component_deinit;
	cmp_handle->base_comp.SetParameter = arender_set_parameter;
	cmp_handle->base_comp.SetConfig = arender_set_config;
	cmp_handle->base_comp.GetConfig = arender_get_config;
	cmp_handle->do_state_set = arender_set_state;
	cmp_handle->num_ports = 2;
	ret = base_port_init(cmp_handle, &cmp_handle->port[BASE_INPUT_PORT],
			BASE_INPUT_PORT,
			OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_port_init error!\n");
		goto error2;
	}
	ret = clock_port_init(cmp_handle, &cmp_handle->port[ARENDER_CLOCK_PORT],
		ARENDER_CLOCK_PORT, OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("clock_port_init error!\n");
		goto error3;
	}

	clk_buf_init(cmp_handle);

	return ret;

error3:
	base_port_deinit(&cmp_handle->port[BASE_INPUT_PORT]);
error2:
	base_component_deinit(cmp_handle);
error1:
	if (audio_ctx != NULL)
		oscl_free(audio_ctx);
	OSCL_TRACE("%d arec:%p\n", ret, cmp_handle);

	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = arender_component_init;
#endif

