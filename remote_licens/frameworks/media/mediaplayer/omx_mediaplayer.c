#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "omx_mediaplayer"

#include <oscl.h>
#include <base_component.h>
#include <omx_vendor_lb.h>
#include <lombo_dec_common.h>
#include <lb_omx_core.h>
#include <lb_omx_as_string.h>
#include "vrender_component.h"
#include "srender_component.h"
#include "clock_component.h"
#include "rotate_component.h"
#include "demuxer_component.h"
#include "omx_mediaplayer.h"
#include "oscl_audio.h"
#include "omx_api.h"
#include <demuxer_plugin.h>

typedef enum {
	OMXMP_SUB_STATUS_CLOSED = 0,
	OMXMP_SUB_STATUS_PAUSE,
	OMXMP_SUB_STATUS_OPEND,
} player_sub_status_e;
typedef enum {
	SUB_ERR_NONE,
	SUB_ERR_URL_OPENED,
	SUB_ERR_URL_NOT_ADD,
	SUB_ERR_OPEN_FAILED,
	SUB_ERR_SEEK_FAILED,
	SUB_ERR_MAX
} player_sub_err_e;

typedef enum player_state {
	OMXMP_STATE_IDLE = 0,
	OMXMP_STATE_INIT,
	OMXMP_STATE_PREPARED,
	OMXMP_STATE_STARTED,
	OMXMP_STATE_PAUSED,
	OMXMP_STATE_STOP,
	OMXMP_STATE_COMPLETED,
	OMXMP_STATE_ERROR,
	OMXMP_STATE_END
} player_state_t;

typedef enum player_msg_type {
	OMXMP_MSG_COMPLETE = 0,
	OMXMP_MSG_PREPARED,
	OMXMP_MSG_SEEK_COMPLETE,
	OMXMP_MSG_SUB_ERROR,
	OMXMP_MSG_ERROR,
	OMXMP_MSG_AUDIO_CHANGE,
	OMXMP_MSG_VIDEO_CHANGE,
	OMXMP_MSG_STREAM_EOS,
	OMXMP_MSG_PORT_SETTINGS_CHANGE,
} player_msg_type_e;

typedef struct demuxer_info {
	comp_info_t al_comp;
	port_info_t *aout;
	port_info_t *vout;
	port_info_t *clk;
	omx_mediainfo_t *mediainfo;
	exsub_info_t *exsubinfo;
} demuxer_info_t;

typedef struct adec_info {
	comp_info_t al_comp;
	port_info_t *ain;
	port_info_t *aout;
	omx_audio_info_t ainfo;
} adecoder_info_t;

typedef struct vdec_info {
	comp_info_t al_comp;
	port_info_t *vin;
	port_info_t *vout;
} vdecoder_info_t;

typedef struct sdec_info {
	comp_info_t al_comp;
	port_info_t *sin;
	port_info_t *sout;
} sdec_info_t;

typedef struct arender_info {
	comp_info_t al_comp;
	port_info_t *ain;
	port_info_t *clk;
	omx_audio_info_t *ainfo;
} arender_info_t;

typedef struct vrender_info {
	comp_info_t al_comp;
	port_info_t *vin;
	port_info_t *clk;
} vrender_info_t;

typedef struct srender_info {
	comp_info_t al_comp;
	port_info_t *sin;
	port_info_t *clk;
	port_info_t *sout;
} srender_info_t;

typedef struct clock_info {
	comp_info_t al_comp;
	port_info_t *audio;
	port_info_t *video;
	port_info_t *dmx;
} clock_info_t;

typedef struct rotate_info {
	comp_info_t al_comp;
	port_info_t *vin;
	port_info_t *vout;
} rotate_info_t;

typedef struct _subtitle_info {
	sdec_info_t sdec;
	srender_info_t srender;
	port_info_t *clk_sout;
	port_info_t *dmx_sout;
	OMX_BOOL has_subtitle;
	OMX_BOOL error_flag;
	OMX_S32 index;
} subtitle_info_t;
typedef struct omx_mediaplayer {
	demuxer_info_t dmx;
	adecoder_info_t adec;
	vdecoder_info_t vdec;
	arender_info_t arender;
	vrender_info_t vrender;
	rotate_info_t rotate;
	clock_info_t clksrc;
	subtitle_info_t sub[DMX_SUBTITLE_STREAM_NUM];

	mp_callback_ops_t *callback;

	OMX_U32 bit_rate;
	OMX_U32 channels;	/* Number of channels (e.g. 2 for stereo) */
	OMX_U32 bit_per_sample;	/* Bit per sample */
	OMX_U32 sample_rate;	/* Sampling rate of the source data.
				   Use 0 for variable or unknown sampling rate. */
	OMX_AUDIO_CODINGTYPE audio_codingtype;

	OMX_U32 frame_width;
	OMX_U32 frame_height;
	OMX_U32 frame_stride;
	OMX_U32 frame_sliceheight;
	OMX_U32 rotate_width;
	OMX_U32 rotate_height;
	OMX_U32 rotate_stride;
	OMX_U32 rotate_sliceheight;
	OMX_VIDEO_CODINGTYPE video_codingtype;

	vdisp_para_t disp_para; /* display params */
	OMX_CONFIG_ROTATIONTYPE rot_mode;
	int rot_enable;

	/* for msg queue */
	sem_t msg_sem;
	oscl_queue_t msg_queue;
	OMX_S32 msg_handler_thread_id;
	pthread_t msg_handler_thread;

	sem_t sem_event;
	sem_t sem_eos;
	pthread_mutex_t lock;

	int status;
	int play_rate;
	char *data_url;
	long duration; /* duration of the media stream, in msecs */
	long cur_pos; /* current position of the media stream, in msecs */
	long seek_pos; /* the position to seek */
	OMX_BOOL isloop; /* indicate if it's loop play */
	OMX_BOOL has_audio;
	OMX_BOOL has_video;
	OMX_BOOL is_seeking;
	OMX_BOOL seek_in_pause;
	OMX_BOOL audio_eos;
	OMX_BOOL video_eos;
	OMX_BOOL reach_eos;

	OMX_U32 vdec_out_buf_alignment;

	dmx_stream_handle_t *stream_handle;
	OMX_BOOL video_header_decoded;
	OMX_U32 play_index;

	omxmp_win_layer_e layer;
	scale_mode_e scale_mode;

	/* todo :... */
} omx_mediaplayer_t;


/**
 * subtitle_stream_return_frame - return subtitle frame get by subtitle callback.
 *
 * @frame: frame tobe returned
 *
 * Returns 0 on success, -EERROR otherwise..
 */

int omxmp_subtitle_return_frame(frame_t *frame)
{
	return app_empty_buffer_done(frame);
}

static int sub_buffer_handle(void *handle, frame_t *frame)
{
	subtitle_frame_info_t *sub = NULL;

	if ((NULL == handle) || (NULL == frame)) {
		OSCL_LOGE("para is error: %p, %p", handle, frame);
		return -1;
	}
	sub = &frame->info.subtitle;
	if (sub) {
		OSCL_LOGE("sub->start_time %lld",
			(long long)sub->comm_info.start_disp_time);
		OSCL_LOGE("sub->end_time %lld",
			(long long)sub->comm_info.end_disp_time);
		if (sub->data.pdata)
			OSCL_LOGE("sub->event.pdata %s", sub->data.pdata);
	}
	omxmp_subtitle_return_frame(frame);

	return 0;
}

int omxmp_subtitle_render_set_cb(subtitle_info_t *psub, app_frame_cb_t *cb)
{
	int ret = 0;

	oscl_param_check((psub != NULL), OMX_ErrorBadParameter, NULL);

	if (psub->srender.sout && psub->srender.sout->cb_async.buf_handle != NULL) {
		untunnel_unset_cb(psub->srender.sout);
		psub->srender.sout->cb_async.buf_handle = NULL;
		psub->srender.sout->cb_async.app_data = NULL;
	}

	if (cb && cb->buf_handle)
		untunnel_setup_cb(psub->srender.sout, cb);
	return ret;
}

int omxmp_send_msg(omx_mediaplayer_t *player, player_msg_type_e cmd,
	OMX_U32 param1, OMX_U32 param2, void *msg_data)
{
	int ret = 0;
	oscl_message_t message;

	message.comp = player;
	message.msg_type = cmd;
	message.para1 = param1;
	message.para2 = param2;
	message.data = msg_data;
	ret = oscl_message_put(&player->msg_queue, &message);
	if (ret)
		return OMX_ErrorInsufficientResources;
	sem_post(&player->msg_sem);

	return ret;
}

static int omxmp_stream_eos(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone, i = 0;
	subtitle_info_t *psub = NULL;
	oscl_param_check(player != NULL, -1, "player null\n");

	pthread_mutex_lock(&player->lock);
	if (player->status != OMXMP_STATE_STARTED
		&& player->status != OMXMP_STATE_PAUSED
		&& player->status != OMXMP_STATE_PREPARED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGW("eos reach in state %d", player->status);
		return ret;
	}

	/* if it's loop, then seek to start & play from beginning */
	if (player->isloop) {
		player->audio_eos = OMX_FALSE;
		player->video_eos = OMX_FALSE;
		player->reach_eos = OMX_FALSE;
		pthread_mutex_unlock(&player->lock);
		ret = omxmp_seek_to(player, 0);
		return ret;
	}

	if (!player->reach_eos) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGW("reach eos 0");
		return 0;
	}

	ret = component_setstate(&player->dmx.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to idle err!\n");

	/* set other component to idle */
	if (player->has_audio) {
		ret = component_setstate(&player->arender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set arender to idle err!\n");
		ret = component_setstate(&player->adec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to idle err!\n");
	}
	if (player->has_video) {
		ret = component_setstate(&player->vrender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set vrender to idle err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp, OMX_StateIdle);
			oscl_param_check_exit(ret == 0, -1, "set rotate to idle err!\n");
		}
		ret = component_setstate(&player->vdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set videodec to idle err!\n");
	}


	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (!psub->has_subtitle)
			continue;
		ret = component_setstate(&psub->sdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set subdec to idle err!\n");
		ret = component_setstate(&psub->srender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set srender to idle err!\n");
	}

	ret = component_setstate(&player->dmx.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to idle err!\n");
	/* set clock component to IDLE */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");

	player->status = OMXMP_STATE_COMPLETED;
	pthread_mutex_unlock(&player->lock);

	/* call back to notify completion */
	player->audio_eos = OMX_FALSE;
	player->video_eos = OMX_FALSE;
	player->reach_eos = OMX_FALSE;
	if (player->callback && player->callback->on_completion)
		player->callback->on_completion(player);
	OSCL_LOGI("=====stream eos end==========\n");
	return 0;
EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

static OMX_ERRORTYPE omxmp_event_handler(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_EVENTTYPE eEvent,
	OMX_U32 Data1,
	OMX_U32 Data2,
	OMX_PTR pEventData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	OMX_S32 i = 0;
	subtitle_info_t *psub = NULL;

	oscl_param_check((hComponent != NULL && pAppData != NULL),
		OMX_ErrorBadParameter, NULL);

	comp_info_t *al_comp = pAppData;
	omx_mediaplayer_t *player = al_comp->priv_data;
	component = get_lb_component(hComponent);

	if (eEvent == OMX_EventCmdComplete) {
		sem_post(al_comp->sem_cmd);
		OSCL_LOGI("%s %s %s complete",
			component->name,
			omx_cmd_as_string(Data1),
			(Data1 == OMX_CommandStateSet) ? omx_state_as_string(Data2) : "");
	} else if (eEvent == OMX_EventError) {
		OSCL_LOGW("%s %s error(%x)", component->name,
			omx_cmd_as_string(Data2), Data1);
		if (Data2 == OMX_CommandStateSet ||
			Data2 == OMX_CommandFlush ||
			Data2 == OMX_CommandPortDisable ||
			Data2 == OMX_CommandPortEnable ||
			Data2 == OMX_CommandMarkBuffer)
			sem_post(al_comp->sem_cmd);
		else if (Data1 == OMX_ErrorDmxReadSeek) {
			omxmp_send_msg(player, OMXMP_MSG_SEEK_COMPLETE,
				OMX_ErrorDmxReadSeek, 0, NULL);
		} else if (Data1 == OMX_ErrorDmxReadPacket) {
			player->video_eos = OMX_TRUE;
			player->audio_eos = OMX_TRUE;
			player->reach_eos = OMX_TRUE;
			omxmp_send_msg(player, OMXMP_MSG_STREAM_EOS, 0, 0, NULL);
		} else if (Data1 == OMX_ErrorUnsupportedVideo) {
			player->video_eos = OMX_TRUE;
		} else {
			if (hComponent == player->vdec.al_comp.cmp_hdl) {
				omxmp_send_msg(player, OMXMP_MSG_ERROR,
					OMXMP_ERR_VIDEO_DEC, 0, NULL);
			} else if (hComponent == player->dmx.al_comp.cmp_hdl) {
				omxmp_send_msg(player, OMXMP_MSG_ERROR,
					OMXMP_ERR_DEMUXER, 0, NULL);
			}
			for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
				psub = &player->sub[i];
				if (hComponent == psub->sdec.al_comp.cmp_hdl) {
					OSCL_LOGE("hComponent: %p", hComponent);
					omxmp_send_msg(player, OMXMP_MSG_SUB_ERROR,
						0, 0, hComponent);
				}
			}
		}
	} else if (eEvent == OMX_EventBufferFlag) {
		if ((int)Data2 == OMX_BUFFERFLAG_EOS) {
			OSCL_LOGW("%s buffer eos", component->name);
			if (player->arender.al_comp.cmp_hdl == hComponent)
				player->audio_eos = OMX_TRUE;
			else if (player->vrender.al_comp.cmp_hdl == hComponent) {
				player->video_eos = OMX_TRUE;
			}
			if (player->audio_eos && player->video_eos)
				player->reach_eos = OMX_TRUE;
			else if (player->audio_eos && !player->has_video)
				player->reach_eos = OMX_TRUE;
			else if (player->video_eos && !player->has_audio)
				player->reach_eos = OMX_TRUE;
			if (player->reach_eos)
				omxmp_send_msg(player, OMXMP_MSG_STREAM_EOS, 0, 0, NULL);
		}
	} else if (eEvent == OMX_EventPortSettingsChanged) {
		OSCL_LOGI("%s-%d port setting changed", component->name, Data1);
		omxmp_send_msg(player, OMXMP_MSG_PORT_SETTINGS_CHANGE,
			Data1, Data2, hComponent);
		/* todo: we should get the new port definiton params here
		 * and init the auido & video decoder component here */
	} else if (eEvent == OMX_EventPortFormatDetected) {
		if (Data1 == OMX_IndexParamAudioPortFormat) {
			OSCL_LOGI("audio detected");
			player->has_audio = OMX_TRUE;
			player->dmx.mediainfo = pEventData;
		} else if (Data1 == OMX_IndexParamVideoPortFormat) {
			OSCL_LOGI("video detected");
			player->has_video = OMX_TRUE;
			player->dmx.mediainfo = pEventData;
		} else if (Data1 == OMX_IndexParamSubtitlePortFormat) {
			OSCL_LOGI("subtitle detected");
			if (Data2 == SUB_STYLE_EXTERNAL)
				player->dmx.exsubinfo = pEventData;
		} else {
			OSCL_LOGW("unkown format %x\n", Data1);
		}
	} else if (eEvent == OMX_EventOutputRendered && player->is_seeking) {
		if (player->has_audio && !player->has_video &&
			hComponent == player->arender.al_comp.cmp_hdl)
			omxmp_send_msg(player, OMXMP_MSG_SEEK_COMPLETE, 0, 0, NULL);
		if (player->has_video && hComponent == player->vrender.al_comp.cmp_hdl)
			omxmp_send_msg(player, OMXMP_MSG_SEEK_COMPLETE, 0, 0, NULL);
	} else if (eEvent == OMX_EventAudioParamChanged) {
		OSCL_LOGI("audio format change");
		player->arender.ainfo = pEventData;
		omxmp_send_msg(player, OMXMP_MSG_AUDIO_CHANGE, 0, 0, NULL);
	} else if (eEvent == OMX_EventVideoParamChanged) {
		OSCL_LOGI("video format change");
	}

	return ret;
}

static OMX_CALLBACKTYPE omxmp_component_callbacks = {
	.EventHandler = omxmp_event_handler,
	.EmptyBufferDone = untunnel_empty_buffer_done,
	.FillBufferDone = untunnel_fill_buffer_done,
};

static int config_audiodec_cmp(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE params;
	omx_audio_info_t ainfo;
	omx_mediainfo_t *omx_mi = player->dmx.mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;
	dmx_ainfo_t *ai = &mi->ainfo[omx_mi->cur_audio_idx];

	ainfo.codec_id = ai->codec_id;
	ainfo.bit_rate = ai->bit_rate;
	ainfo.sample_rate = ai->sample_rate;
	ainfo.channels = ai->channels;
	ainfo.channel_layout = ai->channel_layout;
	ainfo.bitspersample = ai->bits_per_coded_sample;
	ainfo.frame_size = ai->frame_size;
	ainfo.block_align = ai->block_align;
	ainfo.extradata = ai->extradata;
	ainfo.extradata_size = ai->extradata_size;
	ret = OMX_SetParameter(player->adec.al_comp.cmp_hdl,
			omx_index_media_audio_info,
			&ainfo);
	oscl_param_check(ret == OMX_ErrorNone, -1, "audio params err\n");

	/* get audio outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = 1;
	ret = OMX_GetParameter(player->adec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");

	/* update audio outport definiton */
	params.format.audio.eEncoding = player->audio_codingtype;
	ret = OMX_SetParameter(player->adec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");

	ret = untunnel_setup_ports(player->adec.aout, player->arender.ain);
	oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports error");

	return ret;
}

static int config_videodec_cmp(omx_mediaplayer_t *player)
{
	OMX_PARAM_PORTDEFINITIONTYPE params;
	omx_video_info_t vinfo;
	omx_mediainfo_t *omx_mi = player->dmx.mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;
	dmx_vinfo_t *vi = &mi->vinfo[omx_mi->cur_video_idx];
	int ret = OMX_ErrorNone;

	/* get vdec outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = player->vdec.vout->index;
	ret = OMX_GetParameter(player->vdec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");

	player->vdec_out_buf_alignment = params.nBufferAlignment;
	/* update video outport definiton */
	params.format.video.eCompressionFormat = player->video_codingtype;
	params.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	ret = OMX_SetParameter(player->vdec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");

	vinfo.width = vi->width;
	vinfo.height = vi->height;
	vinfo.codec_id = vi->codec_id;
	vinfo.extradata = vi->extradata;
	vinfo.extrasize = vi->extradata_size;
	ret = OMX_SetParameter(player->vdec.al_comp.cmp_hdl,
			omx_index_media_video_info,
			&vinfo);
	oscl_param_check(ret == OMX_ErrorNone, -1, "set media info error");

	return ret;
}

static int config_arender_cmp(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_AUDIO_PARAM_PCMMODETYPE audio_params;
	OMX_BOOL avsync;

	/* get audio outport definition */
	memset(&audio_params, 0, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
	ret = OMX_GetParameter(player->adec.al_comp.cmp_hdl,
			OMX_IndexParamAudioPcm, &audio_params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET audio params err\n");
	OSCL_LOGI("channels %d, bit per sample %d, sample rate %d\n",
		audio_params.nChannels, audio_params.nBitPerSample,
		audio_params.nSamplingRate);

	ret = OMX_SetParameter(player->arender.al_comp.cmp_hdl,
			OMX_IndexParamAudioPcm, &audio_params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");
	avsync = OMX_TRUE;
	ret = OMX_SetParameter(player->arender.al_comp.cmp_hdl,
			omx_index_media_avsync, &avsync);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET avsync err\n");

	return ret;
}

static int config_vrender_cmp(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE params;
	OMX_BOOL avsync;

	/* setup display mode */
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_vrender_mode, &player->disp_para);
	oscl_param_check(ret == OMX_ErrorNone, ret, NULL);

	/* set scale mode */
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_vrender_scale_mode, &player->scale_mode);
	oscl_param_check(ret == OMX_ErrorNone, ret, "set scale mode err\n");

	/* set rotate */
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			OMX_IndexConfigCommonRotate, &player->rot_mode);
	oscl_param_check(ret == OMX_ErrorNone, ret, "set rotation err\n");

	/* get video inport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = 0;
	ret = OMX_GetParameter(player->vrender.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");

	/* update video inport definiton */
	params.nBufferAlignment = player->vdec_out_buf_alignment;
	params.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	if (player->rot_enable) {
		params.format.video.nFrameWidth = player->rotate_width;
		params.format.video.nFrameHeight = player->rotate_height;
		params.format.video.nStride = player->rotate_stride;
		params.format.video.nSliceHeight = player->rotate_sliceheight;
	} else {
		params.format.video.nFrameWidth = player->frame_width;
		params.format.video.nFrameHeight = player->frame_height;
		params.format.video.nStride = player->frame_stride;
		params.format.video.nSliceHeight = player->frame_sliceheight;
	}
	OSCL_LOGI("vrender: size(%dx%d), stride(%dx%d)",
		params.format.video.nFrameWidth,
		params.format.video.nFrameHeight,
		params.format.video.nStride,
		params.format.video.nSliceHeight);

	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");

	/* set avsync */
	avsync = OMX_TRUE;
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_media_avsync, &avsync);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET avsync err\n");

	return ret;
}

static int config_rotate_cmp(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE params;

	/* get rotate inport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = player->rotate.vout->index;
	ret = OMX_GetParameter(player->rotate.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");

	/* update rotate inport definiton */
	params.nBufferAlignment = 1024;
	params.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	params.format.video.nFrameWidth = player->frame_width;
	params.format.video.nFrameHeight = player->frame_height;
	params.format.video.nStride = player->frame_stride;
	params.format.video.nSliceHeight = player->frame_sliceheight;
	ret = OMX_SetParameter(player->rotate.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");

	/* set rotate angle */
	ret = OMX_SetParameter(player->rotate.al_comp.cmp_hdl,
			OMX_IndexConfigCommonRotate, &player->rot_mode);
	oscl_param_check(ret == OMX_ErrorNone, ret, "set rotation err\n");

	/* get params after set rotate angle */
	ret = OMX_GetParameter(player->rotate.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");
	player->rotate_width = params.format.video.nFrameWidth;
	player->rotate_height = params.format.video.nFrameHeight;
	player->rotate_stride = params.format.video.nStride;
	player->rotate_sliceheight = params.format.video.nSliceHeight;
	OSCL_LOGI("rot:%d %d %d %d", params.format.video.nFrameWidth,
		params.format.video.nFrameHeight,
		params.format.video.nStride,
		params.format.video.nSliceHeight);

	ret = untunnel_setup_ports(player->rotate.vout, player->vrender.vin);
	oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports error");

	return ret;
}

static int config_demuxer_cmp(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;

	if (player->has_audio) {
		ret = untunnel_setup_ports(player->dmx.aout, player->adec.ain);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, "");
	}

	if (player->has_video) {
		ret = untunnel_setup_ports(player->dmx.vout, player->vdec.vin);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, "");
	}

EXIT:
	return ret;
}

static int config_clock_cmp(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone, i = 0;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE  refclk;
	subtitle_info_t *psub = NULL;

	/* set ref clk */
	refclk.nSize = sizeof(refclk);
	refclk.eClock = OMX_TIME_RefClockAudio;
	ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeActiveRefClock, &refclk);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("set activ ref clock error!\n");
		return -1;
	}

	/* setup clk port tunnel */
	ret = OMX_SetupTunnel(player->clksrc.al_comp.cmp_hdl, CLOCK_PORT_VIDEO,
			player->vrender.al_comp.cmp_hdl, player->vrender.clk->index);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("setup vrender to clock port tunnel error!");
		return -1;
	}
	ret = OMX_SetupTunnel(player->clksrc.al_comp.cmp_hdl, CLOCK_PORT_AUDIO,
			player->arender.al_comp.cmp_hdl, player->arender.clk->index);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("arender to setup clock port tunnel error!");
		return -1;
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		ret = OMX_SetupTunnel(player->clksrc.al_comp.cmp_hdl,
			(CLOCK_PORT_SUB + i),
			psub->srender.al_comp.cmp_hdl, psub->srender.clk->index);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("setup clock port tunnel error!\n");
			return -1;
		}
	}

	ret = OMX_SetupTunnel(player->clksrc.al_comp.cmp_hdl, CLOCK_PORT_DEMUXER,
			player->dmx.al_comp.cmp_hdl, player->dmx.clk->index);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("setup dmx to clock port tunnel error!");
		return -1;
	}
	return ret;
}

static int config_subtitle_render_cmp(omx_mediaplayer_t *player,
	subtitle_info_t *psub)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	OMX_BOOL avsync;
	app_frame_cb_t cb;
	/* get subtitle inport definition */
	memset(&port_def, 0, sizeof(port_def));
	port_def.nPortIndex = SRENDER_OUTPUT_PORT_INDEX;
	port_def.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(psub->srender.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("get params error!\n");
		return ret;
	}

	port_def.nBufferCountActual = INPUT_BUF_SIZE;
	port_def.bBuffersContiguous = 1;
	port_def.eDomain = OMX_PortDomainSubtitle;
	port_def.nBufferSize = SRENDER_INPUT_DEFAULT_SIZE;
	port_def.nBufferAlignment = 1024;
	ret = OMX_SetParameter(psub->srender.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	if (ret) {
		OSCL_LOGE("set port params error!");
		return ret;
	}
	/* set avsync */
	avsync = OMX_TRUE;
	ret = OMX_SetParameter(psub->srender.al_comp.cmp_hdl,
		omx_index_media_avsync, &avsync);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET avsync err\n");
	/* set subtitle app callback */
	cb.app_data = player;
	if (player->callback->on_get_sub_frame)
		cb.buf_handle = player->callback->on_get_sub_frame;
	else
		cb.buf_handle = sub_buffer_handle;
	cb.type = VS_SUBTITLE_FRAME;
	ret = omxmp_subtitle_render_set_cb(psub, &cb);
	return ret;
}

static int config_subtitle_dec_cmp(omx_mediaplayer_t *player,
	subtitle_info_t *psub, int index)
{
	int ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE params;
	omx_sub_info_t sinfo;
	omx_mediainfo_t *omx_mi = player->dmx.mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;
	exsub_info_t *exsub = player->dmx.exsubinfo;
	dmx_subinfo_t *si = NULL;

	if (index < omx_mi->mediainfo.substream_num) {
		si = &mi->subinfo[index];
		sinfo.sub_type = 0;
	} else if (index < (omx_mi->mediainfo.substream_num + exsub->stream_num)) {
		si = &exsub->stream[index - omx_mi->mediainfo.substream_num];
		sinfo.sub_type = 1;
	} else {
		OSCL_LOGE("open sub index is error");
		return -1;
	}
	OSCL_LOGE("sdec cmp: %d, 0x%x", index, si->codec_id);
	sinfo.index = index;
	sinfo.codec_id = si->codec_id;
	sinfo.extradata = si->extradata;
	sinfo.extrasize = si->extradata_size;
	ret = OMX_SetParameter(psub->sdec.al_comp.cmp_hdl,
			omx_index_media_subtitle_info,
			&sinfo);
	oscl_param_check(ret == OMX_ErrorNone, -1, "sub decodec params err\n");

	/* get subtitle outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = 1;
	ret = OMX_GetParameter(psub->sdec.al_comp.cmp_hdl,
		OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, ret, "get port definition err\n");
	/* update subtitle outport definition */
	ret = OMX_SetParameter(psub->sdec.al_comp.cmp_hdl,
		OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, ret, "SET port definition err\n");
	ret = untunnel_setup_ports(psub->sdec.sout, psub->srender.sin);
	oscl_param_check(ret == OMX_ErrorNone, ret, "untunnel_setup_ports error");

	return ret;
}

static int get_mediainfo(omx_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE params;

	/* get audio outport params */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = 0;
	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");
	player->audio_codingtype = params.format.audio.eEncoding;
	OSCL_LOGI("audio coding type %d\n", player->audio_codingtype);

	/* get video outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = 1;
	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");
	player->video_codingtype = params.format.video.eCompressionFormat;
	player->frame_width = params.format.video.nFrameWidth;
	player->frame_height = params.format.video.nFrameHeight;
	player->frame_stride = (player->frame_width + 15) & 0xFFFFFFF0;
#if 0
	/* add one more mblock to avoid hw bug */
	player->frame_sliceheight = (player->frame_height + 31) & 0xFFFFFFF0;
#else
	player->frame_sliceheight = (player->frame_height + 15) & 0xFFFFFFF0;
#endif
	OSCL_LOGI("video codingtype %d, width %d, height %d, stride %d, sliceheight %d\n",
		player->video_codingtype, player->frame_width, player->frame_height,
		player->frame_stride, player->frame_sliceheight);

	/* get duration */
	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_duration, &player->duration);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get duration err\n");
	OSCL_LOGI("media duration %ld\n", player->duration);

	if (!player->has_audio && !player->has_video)
		ret = -1;

	return ret;
}

static int config_video_ports(omx_mediaplayer_t *player)
{
	OMX_PARAM_PORTDEFINITIONTYPE params;
	int ret = OMX_ErrorNone;

	if (player->status != OMXMP_STATE_STARTED) {
		OSCL_LOGE("error state %d", player->status);
		return -1;
	}

	ret = component_setstate(&player->vdec.al_comp, OMX_StatePause);
	oscl_param_check(ret == 0, -1, "set vdec pause err!");

	ret = component_sendcom(&player->vdec.al_comp, OMX_CommandPortDisable,
		player->vdec.vout->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "disable vdec outport err");
	if (player->rot_enable) {
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortDisable,
			player->rotate.vin->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "disable rotate inport err");
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortDisable,
			player->rotate.vout->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "disable rotate outport err");
	}
	ret = component_sendcom(&player->vrender.al_comp, OMX_CommandPortDisable,
		player->vrender.vin->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "disable vrender inport err");

	/* get vdec outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = player->vdec.vout->index;
	ret = OMX_GetParameter(player->vdec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err");
	player->frame_width = params.format.video.nFrameWidth;
	player->frame_height = params.format.video.nFrameHeight;
	player->frame_stride = params.format.video.nStride;
	player->frame_sliceheight = params.format.video.nSliceHeight;
	player->vdec_out_buf_alignment = params.nBufferAlignment;

	if (player->rot_enable) {
		ret = untunnel_setup_ports(player->vdec.vout, player->rotate.vin);
		oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports err");
		ret = config_rotate_cmp(player);
		oscl_param_check(ret == 0, -1, "config rotate err!");
	} else {
		ret = untunnel_setup_ports(player->vdec.vout, player->vrender.vin);
		oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports err");
	}

	ret = config_vrender_cmp(player);
	oscl_param_check(ret == 0, -1, "config vrender err!");

	ret = component_sendcom(&player->vrender.al_comp, OMX_CommandPortEnable,
		player->vrender.vin->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "enable vrender inport err");
	if (player->rot_enable) {
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortEnable,
			player->rotate.vout->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "enable rotate outport err");
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortEnable,
			player->rotate.vin->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "enable rotate inport err");
	}
	ret = component_sendcom(&player->vdec.al_comp, OMX_CommandPortEnable,
		player->vdec.vout->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "disable vdec outport err");

	ret = component_setstate(&player->vrender.al_comp, OMX_StateIdle);
	oscl_param_check(ret == 0, -1, "set vrender to idle err!\n");
	if (player->rot_enable) {
		ret = component_setstate(&player->rotate.al_comp, OMX_StateIdle);
		oscl_param_check(ret == 0, -1, "set rotate to idle err!\n");
	}

	ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
	oscl_param_check(ret == 0, -1, "set vrender execute err!");
	if (player->rot_enable) {
		ret = component_setstate(&player->rotate.al_comp, OMX_StateExecuting);
		oscl_param_check(ret == 0, -1, "set rotate execute err!");
	}
	ret = component_setstate(&player->vdec.al_comp, OMX_StateExecuting);
	oscl_param_check(ret == 0, -1, "set vdec execute err!");

	/*set window layer*/
	switch (player->layer) {
	case OMXMP_LAYER_TOP:
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_top, NULL);
		break;
	case OMXMP_LAYER_BOTTOM:
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_bottom, NULL);
		break;
	default:
		OSCL_LOGE("no window layer will be set");
	}

	return ret;
}

static int reset_video_ports(omx_mediaplayer_t *player)
{
	OMX_PARAM_PORTDEFINITIONTYPE params;
	int ret = OMX_ErrorNone;

	ret = component_setstate(&player->vdec.al_comp, OMX_StatePause);
	oscl_param_check(ret == 0, -1, "set vdec pause err!");
	if (player->rot_enable) {
		ret = component_setstate(&player->rotate.al_comp, OMX_StatePause);
		oscl_param_check(ret == 0, -1, "set rotate pause err!");
	}
	ret = component_setstate(&player->vrender.al_comp, OMX_StatePause);
	oscl_param_check(ret == 0, -1, "set vrender pause err!");

	ret = component_sendcom(&player->vdec.al_comp, OMX_CommandPortDisable,
		player->vdec.vout->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "disable vdec outport err");
	if (player->rot_enable) {
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortDisable,
			player->rotate.vin->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "disable rotate inport err");
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortDisable,
			player->rotate.vout->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "disable rotate outport err");
	}
	ret = component_sendcom(&player->vrender.al_comp, OMX_CommandPortDisable,
		player->vrender.vin->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "disable vrender inport err");

	if (player->rot_enable) {
		untunnel_unset_ports(player->vdec.vout, player->rotate.vin);
		ret = untunnel_setup_ports(player->vdec.vout, player->rotate.vin);
		oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports err");
	} else {
		untunnel_setup_ports(player->vdec.vout, player->vrender.vin);
		ret = untunnel_setup_ports(player->vdec.vout, player->vrender.vin);
		oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports err");
	}

	/* get video outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = player->vdec.vout->index;
	ret = OMX_GetParameter(player->vdec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err");

	if (player->frame_width != params.format.video.nFrameWidth ||
		player->frame_height != params.format.video.nFrameHeight ||
		player->frame_stride != params.format.video.nStride ||
		player->frame_sliceheight != params.format.video.nSliceHeight) {
		OSCL_LOGI("video size changed: old[%dx%d], new[%dx%d]",
			player->frame_width,
			player->frame_height,
			params.format.video.nFrameWidth,
			params.format.video.nFrameHeight);
		player->frame_width = params.format.video.nFrameWidth;
		player->frame_height = params.format.video.nFrameHeight;
		player->frame_stride = params.format.video.nStride;
		player->frame_sliceheight = params.format.video.nSliceHeight;
		if (player->rot_enable) {
			untunnel_unset_ports(player->rotate.vout, player->vrender.vin);
			ret = config_rotate_cmp(player);
			oscl_param_check(ret == 0, -1, "config rotate err!");
		}

		/* get video inport definition */
		memset(&params, 0, sizeof(params));
		params.nPortIndex = player->vrender.vin->index;
		ret = OMX_GetParameter(player->vrender.al_comp.cmp_hdl,
				OMX_IndexParamPortDefinition, &params);
		oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err");

		/* update video inport definiton */
		if (player->rot_enable) {
			params.format.video.nFrameWidth = player->rotate_width;
			params.format.video.nFrameHeight = player->rotate_height;
			params.format.video.nStride = player->rotate_stride;
			params.format.video.nSliceHeight = player->rotate_sliceheight;
		} else {
			params.format.video.nFrameWidth = player->frame_width;
			params.format.video.nFrameHeight = player->frame_height;
			params.format.video.nStride = player->frame_stride;
			params.format.video.nSliceHeight = player->frame_sliceheight;
		}
		OSCL_LOGI("vrender: size(%dx%d), stride(%dx%d)",
			params.format.video.nFrameWidth,
			params.format.video.nFrameHeight,
			params.format.video.nStride,
			params.format.video.nSliceHeight);

		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				OMX_IndexParamPortDefinition, &params);
		oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err");
	}

	ret = component_sendcom(&player->vrender.al_comp, OMX_CommandPortEnable,
		player->vrender.vin->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "enable vrender inport err");
	if (player->rot_enable) {
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortEnable,
			player->rotate.vout->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "enable rotate outport err");
		ret = component_sendcom(&player->rotate.al_comp, OMX_CommandPortEnable,
			player->rotate.vin->index, NULL);
		oscl_param_check(ret == OMX_ErrorNone, -1, "enable rotate inport err");
	}
	ret = component_sendcom(&player->vdec.al_comp, OMX_CommandPortEnable,
		player->vdec.vout->index, NULL);
	oscl_param_check(ret == OMX_ErrorNone, -1, "disable vdec outport err");

	ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
	oscl_param_check(ret == 0, -1, "set vrender execute err!");
	if (player->rot_enable) {
		ret = component_setstate(&player->rotate.al_comp, OMX_StateExecuting);
		oscl_param_check(ret == 0, -1, "set rotate execute err!");
	}
	ret = component_setstate(&player->vdec.al_comp, OMX_StateExecuting);
	oscl_param_check(ret == 0, -1, "set vdec execute err!");

	return ret;
}

static int _pause(omx_mediaplayer_t *player)
{
	int ret = 0;
	subtitle_info_t *psub = NULL;
	OMX_S32 i = 0;

	ret = component_setstate(&player->dmx.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to pause err!\n");
	if (player->has_audio) {
		ret = component_setstate(&player->adec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to pause err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set arender to pause err!\n");
	}
	if (player->has_video) {
		ret = component_setstate(&player->vdec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set videodec to pause err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp, OMX_StatePause);
			oscl_param_check_exit(ret == 0, -1, "set rotate to pause err!\n");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set vrender to pause err!\n");
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (!psub->has_subtitle)
			continue;
		ret = component_setstate(&psub->sdec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set sdec to pause err!\n");
		ret = component_setstate(&psub->srender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set srender to pause err!\n");
	}

	ret = component_setstate(&player->clksrc.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "set clock to pause err!\n");

	return 0;

EXIT:
	return -1;
}

static int _stop(omx_mediaplayer_t *player)
{
	int ret = 0;
	subtitle_info_t *psub = NULL;
	OMX_S32 i = 0;

	if (player->status != OMXMP_STATE_STARTED &&
		player->status != OMXMP_STATE_PREPARED &&
		player->status != OMXMP_STATE_PAUSED &&
		player->status != OMXMP_STATE_COMPLETED &&
		player->status != OMXMP_STATE_ERROR) {
		OSCL_LOGI("stop in state %d\n", player->status);
		return 0;
	}

	if (player->status == OMXMP_STATE_COMPLETED) {
		OSCL_LOGW("already idle, goto set loaded\n");
		goto set_to_loaded;
	}

	/* pause the demuxer component */
	ret = _pause(player);
	oscl_param_check_exit(ret == 0, -1, "pause err!");

	/* set other component to idle */
	if (player->has_audio) {
		ret = component_setstate(&player->arender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set arender to idle err!\n");
		ret = component_setstate(&player->adec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to idle err!\n");
	}
	if (player->has_video) {
		ret = component_setstate(&player->vrender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set vrender to idle err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp, OMX_StateIdle);
			oscl_param_check_exit(ret == 0, -1, "set rotate to idle err!\n");
		}
		ret = component_setstate(&player->vdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set videodec to idle err!\n");
	}

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (!psub->has_subtitle)
			continue;
		ret = component_setstate(&psub->sdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set sdec to pause err!\n");
		ret = component_setstate(&psub->srender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set srender to idle err!\n");
	}

	ret = component_setstate(&player->dmx.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to idle err!\n");
	/* set clk to idle, as supplier, set it to idle before
	 * the other client */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");

#if 0
	/* flush ports */
	if (player->has_audio) {
		ret = component_sendcom(&player->arender.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush arender inport err!\n");
		ret = component_sendcom(&player->adec.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush adec inport err!\n");
	}
	if (player->has_video) {
		ret = component_sendcom(&player->vrender.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush vrender inport err!\n");
		if (player->rot_enable) {
			ret = component_sendcom(&player->rotate.al_comp,
					OMX_CommandFlush, 0, NULL);
			oscl_param_check_exit(ret == 0, -1, "flush rotate inport err!\n");
		}
		ret = component_sendcom(&player->vdec.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush vdec inport err!\n");
	}
	ret = component_sendcom(&player->dmx.al_comp, OMX_CommandFlush, -1, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush demuxer inport err!\n");
#endif

set_to_loaded:
	/* set clk to loaded */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateLoaded);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");
	if (player->has_video)
		untunnel_unset_ports(player->dmx.vout, player->vdec.vin);
	if (player->has_audio)
		untunnel_unset_ports(player->dmx.aout, player->adec.ain);

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (psub->has_subtitle || (psub->error_flag == OMX_TRUE))
			untunnel_unset_ports(psub->dmx_sout, psub->sdec.sin);
	}

	ret = component_setstate(&player->dmx.al_comp, OMX_StateLoaded);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to loaded err!\n");
	/* set audio component to loaded & free the buffer */
	if (player->has_audio) {
		untunnel_unset_ports(player->adec.aout, player->arender.ain);
		ret = component_setstate(&player->adec.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to loaded err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, -1, "set arender to loaded err!\n");

	}
	/* set video component to loaded & free the buffer */
	if (player->has_video) {
		if (player->rot_enable) {
			untunnel_unset_ports(player->vdec.vout, player->rotate.vin);
			ret = component_setstate(&player->vdec.al_comp, OMX_StateLoaded);
			oscl_param_check_exit(ret == 0, -1, "set vdec to loaded err!");
			untunnel_unset_ports(player->rotate.vout, player->vrender.vin);
			ret = component_setstate(&player->rotate.al_comp,
				OMX_StateLoaded);
			oscl_param_check_exit(ret == 0, -1, "set rotate to loaded err!");
		} else {
			untunnel_unset_ports(player->vdec.vout, player->vrender.vin);
			ret = component_setstate(&player->vdec.al_comp, OMX_StateLoaded);
			oscl_param_check_exit(ret == 0, -1, "set vdec to loaded err!");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, -1, "set vrender to loaded err!\n");

	}
	/* set subtitle component to loaded & free the buffer */
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (psub->has_subtitle || (psub->error_flag == OMX_TRUE)) {
			untunnel_unset_ports(psub->sdec.sout, psub->srender.sin);
			ret = component_setstate(&psub->sdec.al_comp, OMX_StateLoaded);
			oscl_param_check_exit(ret == 0, -1,
				"set subdec to loaded err!\n");
			untunnel_unset_cb(psub->srender.sout);
			ret = component_setstate(&psub->srender.al_comp, OMX_StateLoaded);
			oscl_param_check_exit(ret == 0, -1,
				"set srender to loaded err!\n");
		}
	}

	return 0;

EXIT:
	return -1;
}

static void *omxmp_message_thread(void *param)
{
	oscl_message_t message;
	int ret, i = 0;
	subtitle_info_t *psub = NULL;
	omx_mediaplayer_t *player = param;

	prctl(PR_SET_NAME, "omxmp_message_thread");
	while (1) {
		if (player->status == OMXMP_STATE_END) {
			OSCL_LOGI("exit from msg thread!\n");
			break;
		}
		/* wait for a msg */
		sem_wait(&player->msg_sem);
		/* dequeue a msg */
		ret = oscl_message_get(&player->msg_queue, &message);
		if (ret) {
			OSCL_LOGE("get msg failed!\n");
			continue;
		}

		/* hanle the msg */
		switch (message.msg_type) {
		case OMXMP_MSG_COMPLETE:
			player->audio_eos = OMX_FALSE;
			player->video_eos = OMX_FALSE;
			player->reach_eos = OMX_FALSE;
			if (player->callback && player->callback->on_completion)
				player->callback->on_completion(player);
			break;
		case OMXMP_MSG_PREPARED:
			if (player->callback && player->callback->on_prepared)
				player->callback->on_prepared(player);
			break;
		case OMXMP_MSG_SEEK_COMPLETE:
			if (player->seek_in_pause) {
				_pause(player);
				player->seek_in_pause = OMX_FALSE;
			}
			pthread_mutex_lock(&player->lock);
			player->is_seeking = OMX_FALSE;
			pthread_mutex_unlock(&player->lock);
			if (message.para1 == OMX_ErrorDmxReadSeek) {
				OMX_TIME_CONFIG_CLOCKSTATETYPE clk_state;
				OSCL_LOGW("seek error");
				/* set clock to wait for start */
				clk_state.nSize = sizeof(clk_state);
				clk_state.nWaitMask = 0;
				clk_state.eState = OMX_TIME_ClockStateRunning;
				ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
						OMX_IndexConfigTimeClockState,
						&clk_state);
				if (ret != OMX_ErrorNone)
					OSCL_LOGE("set clock to wait for start err!");
				if (player->callback && player->callback->on_error)
					player->callback->on_error(player, message.para1);
				break;
			}
			if (player->callback && player->callback->on_seek_complete)
				player->callback->on_seek_complete(player);
			break;
		case OMXMP_MSG_ERROR:
			pthread_mutex_lock(&player->lock);
			player->status = OMXMP_STATE_ERROR;
			pthread_mutex_unlock(&player->lock);
			if (player->callback && player->callback->on_error)
				player->callback->on_error(player, message.para1);
			break;
		case OMXMP_MSG_SUB_ERROR:
			for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
				psub = &player->sub[i];
				if (message.data == psub->sdec.al_comp.cmp_hdl)
					break;
			}
			if (i < DMX_SUBTITLE_STREAM_NUM) {
				ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
					omx_index_media_disable_sub,
					&psub->index);
				psub->has_subtitle = OMX_FALSE;
				psub->error_flag = OMX_TRUE;
			} else
				OSCL_LOGE("handle of srender component is err: %p",
					message.data);
			break;
		case OMXMP_MSG_STREAM_EOS:
			omxmp_stream_eos(player);
			break;
		case OMXMP_MSG_AUDIO_CHANGE: {
			int ret;
			OMX_AUDIO_PARAM_PCMMODETYPE audio_params;
			memset(&audio_params, 0, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
			audio_params.nChannels = player->arender.ainfo->channels;
			audio_params.nBitPerSample =
				player->arender.ainfo->bitspersample;
			audio_params.nSamplingRate = player->arender.ainfo->sample_rate;
			ret = OMX_SetConfig(player->arender.al_comp.cmp_hdl,
					OMX_IndexParamAudioPcm, &audio_params);
			if (ret != 0) {
				OSCL_LOGE("set audio err");
				break;
			}
			break;
		}
		case OMXMP_MSG_PORT_SETTINGS_CHANGE: {
			OMX_COMPONENTTYPE *cmp_hdl = message.data;
			int port_inx = message.para1;
			if (cmp_hdl == player->vdec.al_comp.cmp_hdl &&
				port_inx == player->vdec.vout->index) {
				OMX_U32 dec_idx = message.para2;
				OSCL_LOGI("vdec out port settings changed");
				pthread_mutex_lock(&player->lock);
				if (dec_idx != player->play_index) {
					pthread_mutex_unlock(&player->lock);
					OSCL_LOGW("play file changed");
					break;
				}
				if (player->video_header_decoded) {
					OSCL_LOGW("port settings changed while playing");
					ret = reset_video_ports(player);
					if (ret != 0) {
						pthread_mutex_unlock(&player->lock);
						OSCL_LOGE("reset videodec err!");
						break;
					}
				} else {
					ret = config_video_ports(player);
					if (ret != 0) {
						pthread_mutex_unlock(&player->lock);
						OSCL_LOGE("config videodec err!");
						break;
					}
					player->video_header_decoded = OMX_TRUE;
				}
				pthread_mutex_unlock(&player->lock);
			}
			break;
		}
		default:
			OSCL_LOGE("msg type %d not supported!\n", message.msg_type);
			break;
		}
	}

	OSCL_LOGD("omxmp message thread exit.");
	return NULL;
}

int omxmp_subtitle_enable(void *handle, int index)
{
	player_sub_status_e sub_status = OMXMP_SUB_STATUS_PAUSE;
	omx_mediaplayer_t *player = handle;
	subtitle_info_t *psub = NULL;
	OMX_S32 idx = index, i = 0;
	OMX_S32 ret = -1;
	OMX_S32 open_idx = -1;

	pthread_mutex_lock(&player->lock);
	oscl_param_check_exit(player != NULL, ret, "player handle is NULL\n");

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (!psub->has_subtitle)
			break;
	}
	if (i > DMX_SUBTITLE_STREAM_NUM) {
		OSCL_LOGE("subtitle has been open, please close first");
		goto EXIT;
	}

	/* open new subtitle*/
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_enable_sub, &idx);
	oscl_param_check_exit(ret == 0, ret, "open sub failed");
	open_idx = idx;

	/*get dmx subtitle out port index by subtitle index*/
	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_get_dmx_sub_outport, &idx);
	oscl_param_check_exit(ret == 0, ret, "get outport index failed!\n");

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		OSCL_LOGE("i: %d, idx: %d, dmx_sout_idx: %d", i, idx,
			psub->dmx_sout->index);
		if (psub->dmx_sout->index == idx)
			break;
	}

	if (i >= DMX_SUBTITLE_STREAM_NUM) {
		OSCL_LOGE("sub index and port index are error");
		idx = index;
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_disable_sub, &idx);
		oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");
		goto EXIT;
	}

	ret = component_sendcom(&player->dmx.al_comp, OMX_CommandPortDisable,
		psub->dmx_sout->index, NULL);
	oscl_param_check_exit(ret == 0, ret, "disable dmx sub port failed!\n");

	ret = untunnel_setup_ports(psub->dmx_sout, psub->sdec.sin);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret,
		"link dmx sout to dec sin err");

	ret = component_sendcom(&player->dmx.al_comp, OMX_CommandPortEnable,
		psub->dmx_sout->index, NULL);
	oscl_param_check_exit(ret == 0, ret, "enable dmx sub port failed!\n");

	ret = component_sendcom(&player->clksrc.al_comp, OMX_CommandPortEnable,
		psub->clk_sout->index, NULL);
	oscl_param_check_exit(ret == 0, ret, "enable clock sub port failed!\n");

	/* loaded to idle */
	ret = config_subtitle_dec_cmp(player, psub, index);
	oscl_param_check_exit(ret == 0, ret, "config subdec err!\n");
	ret = component_setstate(&psub->sdec.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, ret, "set subdec to idle err!\n");

	ret = config_subtitle_render_cmp(player, psub);
	oscl_param_check_exit(ret == 0, ret, "config srender err!\n");
	ret = component_setstate(&psub->srender.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, ret, "set srender to idle err!\n");

	/* idle to executing */
	ret = component_setstate(&psub->sdec.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, ret, "set subdec to execute err!\n");
	ret = component_setstate(&psub->srender.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, ret, "set srender to execute err!");

	if (!psub->error_flag) {
		psub->has_subtitle = OMX_TRUE;
		psub->index = index;

		OSCL_LOGI("subtitle pipe is ready");
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			OMX_index_media_sub_Position, NULL);
		oscl_param_check(ret == 0, ret, "seek subtitle latest pos failed!\n");

		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sub_set_status_idx, &psub->index);
		oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");

		sub_status = OMXMP_SUB_STATUS_OPEND;
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sw_sub_status, &sub_status);
		oscl_param_check(ret == 0, ret, "resume subtitle failed!\n");
	} else {
		/* excuting to pause */
		ret = component_setstate(&psub->sdec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, ret, "set sdec to pause err!\n");
		ret = component_setstate(&psub->srender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, ret, "set srender to pause err!\n");

		ret = component_setstate(&psub->sdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, ret, "set subdec to idle err!\n");
		/* pause to idle */
		ret = component_setstate(&psub->srender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, ret, "set srender to idle err!\n");

#if 0
		ret = component_sendcom(&psub->srender.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush srender inport err!\n");
		ret = component_sendcom(&psub->sdec.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush sdec inport err!\n");
#endif
		/*unlink dmx sout to sdec sin port*/
		ret = component_sendcom(&player->dmx.al_comp, OMX_CommandPortDisable,
			psub->dmx_sout->index, NULL);
		oscl_param_check_exit(ret == 0, ret, "disable dmx sout failed!\n");
		untunnel_unset_ports(psub->dmx_sout, psub->sdec.sin);

		/* idle to loaded */
		untunnel_unset_ports(psub->sdec.sout, psub->srender.sin);
		ret = component_setstate(&psub->sdec.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, ret, "set subdec to loaded err!\n");

		ret = component_sendcom(&player->clksrc.al_comp, OMX_CommandPortDisable,
			psub->clk_sout->index, NULL);
		oscl_param_check_exit(ret == 0, ret, "disable clock sub port failed!\n");

		untunnel_unset_cb(psub->srender.sout);
		ret = component_setstate(&psub->srender.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, ret, "set srender to loaded err!\n");


		idx = index;
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_disable_sub, &idx);
		oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");

		/*untunnel_unset_ports(player->dmx.sout, player->sdec.sin);*/
		psub->has_subtitle = OMX_FALSE;
		psub->index = -1;
		psub->error_flag = OMX_FALSE;
		ret = -1;
		OSCL_LOGE("open subtitle failed since get invalid component resource");
	}
	pthread_mutex_unlock(&player->lock);
	return ret;
EXIT:
	if (open_idx >= 0)
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_disable_sub, &open_idx);
	pthread_mutex_unlock(&player->lock);
	return ret;

}

int omxmp_subtitle_disable(void *handle, int index)
{
	OMX_S32 ret = OMX_ErrorNone;
	omx_mediaplayer_t *player = handle;
	player_sub_status_e sub_status = OMXMP_SUB_STATUS_PAUSE;
	OMX_S32 idx = index, i = 0;
	subtitle_info_t *psub = NULL;

	pthread_mutex_lock(&player->lock);

	oscl_param_check_exit(player != NULL, ret, "player handle is NULL\n");

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if ((psub->index == idx) && (!psub->has_subtitle)) {
			OSCL_LOGE("subtitle has been closed");
			goto EXIT;
		}
	}
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_sub_set_status_idx, &idx);
	oscl_param_check_exit(ret == 0, ret, "set subtitle index failed!\n");

	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_sw_sub_status, &sub_status);
	oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");

	/*get dmx subtitle out port index by subtitle index*/
	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_get_dmx_sub_outport, &idx);
	if (ret) {
		OSCL_LOGE("get outport index failed!\n");
		idx = index;
		sub_status = OMXMP_SUB_STATUS_OPEND;
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sub_set_status_idx, &idx);
		oscl_param_check_exit(ret == 0, ret, "set subtitle index failed!\n");

		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sw_sub_status, &sub_status);
		oscl_param_check_exit(ret == 0, ret, "open subtitle failed!\n");
		goto EXIT;
	}
	oscl_param_check_exit(ret == 0, ret, "get outport index failed!\n");

	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (psub->dmx_sout->index == idx)
			break;
	}

	if (i > DMX_SUBTITLE_STREAM_NUM) {
		OSCL_LOGE("sub index and port index are error");
		idx = index;
		sub_status = OMXMP_SUB_STATUS_OPEND;
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sub_set_status_idx, &idx);
		oscl_param_check_exit(ret == 0, ret, "set subtitle index failed!\n");

		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sw_sub_status, &sub_status);
		oscl_param_check_exit(ret == 0, ret, "open subtitle failed!\n");
		goto EXIT;
	}

	/* excuting to pause */
	ret = component_setstate(&psub->sdec.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, ret, "set sdec to pause err!\n");
	ret = component_setstate(&psub->srender.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, ret, "set srender to pause err!\n");

	ret = component_setstate(&psub->sdec.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, ret, "set subdec to idle err!\n");
	/* pause to idle */
	ret = component_setstate(&psub->srender.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, ret, "set srender to idle err!\n");

	/*unlink dmx sout to sdec sin port*/
	ret = component_sendcom(&player->dmx.al_comp, OMX_CommandPortDisable,
		psub->dmx_sout->index, NULL);
	oscl_param_check_exit(ret == 0, ret, "disable dmx sout failed!\n");
	untunnel_unset_ports(psub->dmx_sout, psub->sdec.sin);

	/* idle to loaded */
	untunnel_unset_ports(psub->sdec.sout, psub->srender.sin);
	ret = component_setstate(&psub->sdec.al_comp, OMX_StateLoaded);
	oscl_param_check_exit(ret == 0, ret, "set subdec to loaded err!\n");

	ret = component_sendcom(&player->clksrc.al_comp, OMX_CommandPortDisable,
		psub->clk_sout->index, NULL);
	oscl_param_check_exit(ret == 0, ret, "disable clock sub port failed!\n");

	untunnel_unset_cb(psub->srender.sout);
	ret = component_setstate(&psub->srender.al_comp, OMX_StateLoaded);
	oscl_param_check_exit(ret == 0, ret, "set srender to loaded err!\n");

	idx = index;
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_disable_sub, &idx);
	oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");

	psub->has_subtitle = OMX_FALSE;
	psub->index = -1;
	psub->error_flag = OMX_FALSE;

EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;

}

int omxmp_subtitle_get_info(void *handle, omxmp_sub_info_t *sub_info)
{
	omx_mediaplayer_t *player = handle;
	omx_mediainfo_t mi;
	exsub_info_t exsi;
	omxmp_sub_stream_t *sstream;
	OMX_S32 ret = -1;
	OMX_S32 i, j;

	oscl_param_check(player != NULL, -1, "handle null");
	oscl_param_check(sub_info != NULL, -1, "sub_info null");

	if (player->status != OMXMP_STATE_PREPARED &&
		player->status != OMXMP_STATE_STARTED &&
		player->status != OMXMP_STATE_PAUSED) {
		OSCL_LOGW("error state %d\n", player->status);
		return -1;
	}

	memset(&mi, 0x00, sizeof(omx_mediainfo_t));
	memset(&exsi, 0x00, sizeof(exsub_info_t));

	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
				omx_index_media_info, &mi);
	oscl_param_check(ret == 0, -1, "dmx get embeded subtitle err");


	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
				omx_index_media_get_exsub_info, &exsi);
	oscl_param_check(ret == 0, -1, "dmx get external subtitle err");


	/*embeded subtitle information*/
	sub_info->stream_num = mi.mediainfo.substream_num;
	if (sub_info->stream_num > DMX_MAX_SUB_STREAM)
		sub_info->stream_num = DMX_MAX_SUB_STREAM;
	for (i = 0; i < sub_info->stream_num; i++) {
		sstream = &sub_info->sub_stream[i];
		sstream->stream_idx = i;
		if (sstream->name) {
			free(sstream->name);
			sstream->name = NULL;
		}
		if (mi.mediainfo.subinfo[i].text)
			sstream->name = oscl_strdup(mi.mediainfo.subinfo[i].text);
		OSCL_LOGI("em: sstream[%d]->name: %s, codecID: 0x%x", i, sstream->name,
			mi.mediainfo.subinfo[i].codec_id);
	}

	/* external subtitle information*/
	if (exsi.stream_num > DMX_MAX_SUB_STREAM)
		sub_info->stream_num += DMX_MAX_SUB_STREAM;
	else
		sub_info->stream_num += exsi.stream_num;
	for (j = 0; j < (sub_info->stream_num - i); j++) {
		sstream = &sub_info->sub_stream[i + j];
		sstream->stream_idx = i + j;
		if (sstream->name) {
			free(sstream->name);
			sstream->name = NULL;
		}
		if (exsi.stream[j].text)
			sstream->name = oscl_strdup(exsi.stream[j].text);
		OSCL_LOGI("ex: sstream[%d]->name: %s, codecID: 0x%x", i + j,
			sstream->name, exsi.stream[j].codec_id);
	}

	return 0;
}

int omxmp_subtitle_add_external_source(void *handle, const char *url)
{
	OMX_S32 ret = -1;
	omx_mediaplayer_t *player = handle;

	oscl_param_check(player != NULL, ret, "player handle is NULL\n");
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_media_add_exsub_url, (char *)url);
	switch (ret) {
	case -1:
		return OMXMP_ERR_SUB_FILE_IS_NOT_EXIST;
	case -2:
		return OMXMP_ERR_SUB_FILE_NUM_BIGGER;
	case -3:
		return OMXMP_ERR_SUB_DEC_NOT_SUPPORTED;
	case -4:
		return OMXMP_ERR_SUB_FILE_IS_EXIST;
	default:
		break;
	}
	return ret;
}

void *omxmp_create(mp_callback_ops_t *cb_ops)
{
	omx_mediaplayer_t *player = NULL;
	int ret = OMX_ErrorNone;
	pthread_attr_t msg_thread_attr;
	struct sched_param shed_param = {0};
	int index;

	oscl_param_check(cb_ops != NULL, NULL, "call back ops null\n");

	player = oscl_zalloc(sizeof(*player));
	if (player == NULL) {
		OSCL_LOGE("alloc omx_mediaplayer_t error!\n");
		return NULL;
	}

	pthread_mutex_init(&player->lock, NULL);
	pthread_mutex_lock(&player->lock);

	player->callback = cb_ops;
	player->play_rate = 1; /* default rate is normal */

	ret = component_init(&player->dmx.al_comp,
			"OMX.LB.SOURCE.DEMUXER",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&player->dmx.al_comp, OMX_DirOutput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->dmx.aout = &player->dmx.al_comp.port_info[index];
	index = component_get_port_index(&player->dmx.al_comp, OMX_DirOutput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->dmx.vout = &player->dmx.al_comp.port_info[index];
	index = component_get_port_index(&player->dmx.al_comp, OMX_DirInput,
			OMX_PortDomainOther);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->dmx.clk = &player->dmx.al_comp.port_info[index];
	player->dmx.al_comp.priv_data = player;

	ret = component_init(&player->adec.al_comp,
			"OMX.LB.SOURCE.ADEC",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&player->adec.al_comp, OMX_DirInput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->adec.ain = &player->adec.al_comp.port_info[index];
	index = component_get_port_index(&player->adec.al_comp, OMX_DirOutput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->adec.aout = &player->adec.al_comp.port_info[index];
	player->adec.al_comp.priv_data = player;

	ret = component_init(&player->vdec.al_comp,
			"OMX.LB.VIDEO.DEC",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&player->vdec.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->vdec.vin = &player->vdec.al_comp.port_info[index];
	index = component_get_port_index(&player->vdec.al_comp, OMX_DirOutput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->vdec.vout = &player->vdec.al_comp.port_info[index];
	player->vdec.al_comp.priv_data = player;

	ret = component_init(&player->arender.al_comp,
			"OMX.LB.SINK.ARENDER",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&player->arender.al_comp, OMX_DirInput,
			OMX_PortDomainAudio);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->arender.ain = &player->arender.al_comp.port_info[index];
	index = component_get_port_index(&player->arender.al_comp, OMX_DirInput,
			OMX_PortDomainOther);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->arender.clk = &player->arender.al_comp.port_info[index];
	player->arender.al_comp.priv_data = player;

	ret = component_init(&player->vrender.al_comp,
			"OMX.LB.SINK.VRENDER",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&player->vrender.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->vrender.vin = &player->vrender.al_comp.port_info[index];
	index = component_get_port_index(&player->vrender.al_comp, OMX_DirInput,
			OMX_PortDomainOther);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->vrender.clk = &player->vrender.al_comp.port_info[index];
	player->vrender.al_comp.priv_data = player;

	ret = component_init(&player->rotate.al_comp,
			"OMX.LB.VIDEO.ROT",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&player->rotate.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->rotate.vin = &player->rotate.al_comp.port_info[index];
	index = component_get_port_index(&player->rotate.al_comp, OMX_DirOutput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	player->rotate.vout = &player->rotate.al_comp.port_info[index];
	player->rotate.al_comp.priv_data = player;

	ret = component_init(&player->clksrc.al_comp,
			"OMX.LB.SOURCE.CLOCK",
			&omxmp_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	player->clksrc.audio = &player->clksrc.al_comp.port_info[CLOCK_PORT_AUDIO];
	player->clksrc.video = &player->clksrc.al_comp.port_info[CLOCK_PORT_VIDEO];
	player->clksrc.dmx = &player->clksrc.al_comp.port_info[CLOCK_PORT_DEMUXER];
	player->clksrc.al_comp.priv_data = player;

	/*create subtitle component*/
	for (int i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		ret = component_init(&player->sub[i].sdec.al_comp,
				"OMX.LB.SINK.SDEC",
				&omxmp_component_callbacks);
		oscl_param_check_exit(ret == 0, ret, NULL);

		index = component_get_port_index(&player->sub[i].sdec.al_comp,
			OMX_DirInput, OMX_PortDomainSubtitle);
		oscl_param_check_exit(index >= 0, -1, NULL);
		player->sub[i].sdec.sin = &player->sub[i].sdec.al_comp.port_info[index];

		index = component_get_port_index(&player->sub[i].sdec.al_comp,
			OMX_DirOutput, OMX_PortDomainSubtitle);
		oscl_param_check_exit(index >= 0, -1, NULL);
		player->sub[i].sdec.sout = &player->sub[i].sdec.al_comp.port_info[index];
		player->sub[i].sdec.al_comp.priv_data = player;

		ret = component_init(&player->sub[i].srender.al_comp,
				"OMX.LB.SINK.SRENDER",
				&omxmp_component_callbacks);
		oscl_param_check_exit(ret == 0, ret, NULL);

		index = component_get_port_index(&player->sub[i].srender.al_comp,
			OMX_DirInput, OMX_PortDomainSubtitle);
		oscl_param_check_exit(index >= 0, -1, NULL);
		player->sub[i].srender.sin =
			&player->sub[i].srender.al_comp.port_info[index];

		index = component_get_port_index(&player->sub[i].srender.al_comp,
			OMX_DirOutput, OMX_PortDomainSubtitle);
		oscl_param_check_exit(index >= 0, -1, NULL);
		player->sub[i].srender.sout =
			&player->sub[i].srender.al_comp.port_info[index];

		index = component_get_port_index(&player->sub[i].srender.al_comp,
			OMX_DirInput, OMX_PortDomainOther);
		oscl_param_check_exit(index >= 0, -1, NULL);
		player->sub[i].srender.clk =
			&player->sub[i].srender.al_comp.port_info[index];

		player->sub[i].srender.al_comp.priv_data = player;

		index = component_get_port_index(&player->dmx.al_comp, OMX_DirOutput,
					OMX_PortDomainSubtitle);
		oscl_param_check_exit(index >= 0, -1, NULL);
		player->sub[i].dmx_sout = &player->dmx.al_comp.port_info[index + i];

		player->sub[i].clk_sout =
			&player->clksrc.al_comp.port_info[CLOCK_PORT_SUB + i];

	}

	sem_init(&player->sem_event, 0, 0);
	sem_init(&player->sem_eos, 0, 0);

	/* config clock component */
	ret = config_clock_cmp(player);
	oscl_param_check_exit(ret == 0, -1, "config clock cmp err!\n");

	/* create the msg thread */
	oscl_queue_init(&player->msg_queue);
	sem_init(&player->msg_sem, 0, 0);
	pthread_attr_init(&msg_thread_attr);
	pthread_attr_setstacksize(&msg_thread_attr, 0x2000);
	shed_param.sched_priority = 15;
	pthread_attr_setschedparam(&msg_thread_attr, &shed_param);
	player->msg_handler_thread_id = pthread_create(
			&player->msg_handler_thread, &msg_thread_attr,
			omxmp_message_thread, player);

	/* change state to idle */
	player->status = OMXMP_STATE_IDLE;
	player->layer = OMXMP_LAYER_NONE;
	pthread_mutex_unlock(&player->lock);

EXIT:
	if (ret != OMX_ErrorNone) {
		pthread_mutex_unlock(&player->lock);
		omxmp_release(player);
		player = NULL;
	}
	return player;
}

void omxmp_release(void *handle)
{
	omx_mediaplayer_t *player = NULL;
	OMX_S32 i = 0;

	if (handle == NULL) {
		OSCL_LOGE("handle null\n");
		return;
	}

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);

	_stop(player);

	/* quit the msg thread */
	player->status = OMXMP_STATE_END;
	pthread_mutex_unlock(&player->lock);
	if (player->msg_handler_thread_id == 0) {
		oscl_queue_flush(&player->msg_queue);
		sem_post(&player->msg_sem);
		pthread_join(player->msg_handler_thread, NULL);
		player->msg_handler_thread_id = -1;
	}
	pthread_mutex_lock(&player->lock);

	component_deinit(&player->clksrc.al_comp);
	component_deinit(&player->rotate.al_comp);
	component_deinit(&player->vrender.al_comp);
	component_deinit(&player->arender.al_comp);
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		component_deinit(&player->sub[i].srender.al_comp);
		component_deinit(&player->sub[i].sdec.al_comp);
	}

	component_deinit(&player->vdec.al_comp);
	component_deinit(&player->adec.al_comp);
	component_deinit(&player->dmx.al_comp);

	/* destroy sem */
	sem_destroy(&player->sem_event);
	sem_destroy(&player->sem_eos);
	sem_destroy(&player->msg_sem);

	/* free the url */
	if (player->data_url) {
		oscl_free(player->data_url);
		player->data_url = NULL;
	}

	oscl_queue_deinit(&player->msg_queue);

	pthread_mutex_unlock(&player->lock);
	pthread_mutex_destroy(&player->lock);
	/* free the player */
	oscl_free(player);

	/* for memory observation */
	/* osal_dump(); */
	OSCL_LOGI("====== player release =======\n");
}

int omxmp_set_data_source(void *handle, const char *url)
{
	int ret = OMX_ErrorNone;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null");
	oscl_param_check(url != NULL, -1, "url null");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	if (player->status != OMXMP_STATE_IDLE) {
		OSCL_LOGE("error state %d\n", player->status);
		pthread_mutex_unlock(&player->lock);
		return -1;
	}

	if (player->data_url) {
		oscl_free(player->data_url);
		player->data_url = NULL;
	}
	player->data_url = oscl_strdup(url);
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_vendor_input_filename, player->data_url);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, "set input file err!");

	player->play_index++;
	ret = OMX_SetParameter(player->vdec.al_comp.cmp_hdl,
			omx_index_media_vdec_id, &player->play_index);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, "set vdec_id err!");

	player->status = OMXMP_STATE_INIT;
	pthread_mutex_unlock(&player->lock);
	OSCL_LOGI("set data url %s\n", player->data_url);
	return 0;

EXIT:
	pthread_mutex_unlock(&player->lock);
	omxmp_send_msg(player, OMXMP_MSG_ERROR, OMXMP_ERR_FUNC, 0, NULL);
	return ret;
}

int omxmp_prepare(void *handle)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	if (player->status != OMXMP_STATE_INIT &&
		player->status != OMXMP_STATE_STOP) {
		OSCL_LOGE("error state %d\n", player->status);
		pthread_mutex_unlock(&player->lock);
		return -1;
	}

	/* clock component set to idle, tunnel port in clk cmp act as supllier,
	 * so set to idle prior to the client */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");

	/* set demxer to idle */
	ret = component_setstate(&player->dmx.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to idle err!\n");

	/* config demuxer, alloc buffer */
	ret = config_demuxer_cmp(player);
	oscl_param_check_exit(ret == 0, -1, "config demuxer err!\n");

#if 0
	/* set demuxer to execute to get media info */
	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to executing err!\n");
#endif

	/* get mediainfo from demuxer */
	ret = get_mediainfo(player);
	oscl_param_check_exit(ret == 0, -1, "get_mediainfo err!\n");

	/* config audio component & set to idle */
	if (player->has_audio) {
		ret = config_audiodec_cmp(player);
		oscl_param_check_exit(ret == 0, -1, "config audiodec err!\n");
		ret = config_arender_cmp(player);
		oscl_param_check_exit(ret == 0, -1, "config arender err!\n");
		ret = component_setstate(&player->adec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to idle err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set arender to idle err!\n");
	}
	/* config video component & set to idle */
	if (player->has_video) {
		ret = config_videodec_cmp(player);
		oscl_param_check_exit(ret == 0, -1, "config videodec err!\n");

		ret = component_setstate(&player->vdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set videodec to idle err!\n");
	}

	player->audio_eos = OMX_FALSE;
	player->video_eos = OMX_FALSE;
	player->reach_eos = OMX_FALSE;
	player->is_seeking = OMX_FALSE;
	player->seek_pos  = 0;
	player->video_header_decoded = OMX_FALSE;

	/* change state to prepared */
	player->status = OMXMP_STATE_PREPARED;
	pthread_mutex_unlock(&player->lock);

	ret = omxmp_send_msg(player, OMXMP_MSG_PREPARED, 0, 0, NULL);
	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	omxmp_send_msg(player, OMXMP_MSG_ERROR, OMXMP_ERR_FUNC, 0, NULL);
	return ret;
}

int omxmp_start(void *handle)
{
	int ret = 0, i = 0;
	omx_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_CLOCKSTATETYPE clk_state;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE refclk;
	subtitle_info_t *psub = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);

	if (player->status != OMXMP_STATE_PREPARED &&
		player->status != OMXMP_STATE_PAUSED &&
		player->status != OMXMP_STATE_COMPLETED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

#if 0
	player->audio_eos = OMX_FALSE;
	player->video_eos = OMX_FALSE;
	player->reach_eos = OMX_FALSE;
	player->is_seeking = OMX_FALSE;
	player->seek_pos  = 0;
#endif

	/* set ref clk */
	refclk.nSize = sizeof(refclk);
	if (player->has_audio)
		refclk.eClock = OMX_TIME_RefClockAudio;
	else
		refclk.eClock = OMX_TIME_RefClockNone;
	ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeActiveRefClock, &refclk);
	oscl_param_check_exit(ret == 0, -1, "config refclk err!");

	/* set clock to executing */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set clock to execute err!\n");
	if (player->status == OMXMP_STATE_PREPARED ||
		player->status == OMXMP_STATE_COMPLETED) {
		/* set clock state to ClockStateWaitingForStartTime */
		clk_state.nSize = sizeof(clk_state);
		clk_state.nWaitMask = 0;
		if (player->has_audio)
			clk_state.nWaitMask |= OMX_CLOCKPORT0;
		if (player->has_video)
			clk_state.nWaitMask |= OMX_CLOCKPORT1;
		clk_state.eState = OMX_TIME_ClockStateWaitingForStartTime;
		ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
				OMX_IndexConfigTimeClockState, &clk_state);
		oscl_param_check_exit(ret == 0, -1, "set clock to state err!\n");
	}

	/* set component to executing */
	if (player->has_audio) {
		OMX_BOOL drop_data = OMX_FALSE;
		ret = OMX_SetParameter(player->arender.al_comp.cmp_hdl,
				omx_index_media_drop_data, &drop_data);
		oscl_param_check_exit(ret == 0, -1, "set arender drop data err!");

		ret = component_setstate(&player->arender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set arender to execute err!\n");
		ret = component_setstate(&player->adec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to execute err!\n");
	}
	if (player->has_video) {
		if (player->video_header_decoded) {
			ret = component_setstate(&player->vrender.al_comp,
				OMX_StateExecuting);
			oscl_param_check_exit(ret == 0, -1,
				"set vrender to execute err!");
			if (player->rot_enable) {
				ret = component_setstate(&player->rotate.al_comp,
						OMX_StateExecuting);
				oscl_param_check_exit(ret == 0, -1,
					"set rotate to execute err!");
			}
		}
		ret = component_setstate(&player->vdec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set videodec to execute err!\n");
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (psub->has_subtitle) {
			ret = component_setstate(&psub->sdec.al_comp, OMX_StateExecuting);
			oscl_param_check_exit(ret == 0, -1, "set sdec to pause err!\n");
			ret = component_setstate(&psub->srender.al_comp,
				OMX_StateExecuting);
			oscl_param_check_exit(ret == 0, -1,
				"set srender to pause err!\n");
		}
	}

	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to executing err!\n");
	OSCL_LOGI("-------start end--------\n");

	player->status = OMXMP_STATE_STARTED;

	pthread_mutex_unlock(&player->lock);

	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	omxmp_send_msg(player, OMXMP_MSG_ERROR, OMXMP_ERR_FUNC, 0, NULL);
	return ret;
}

int omxmp_pause(void *handle)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);

	if (player->status != OMXMP_STATE_STARTED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}
	if (player->has_video && !player->video_header_decoded) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGE("video header not decode yet");
		return -1;
	}

	ret = _pause(player);
	oscl_param_check_exit(ret == 0, -1, "pause err!");

	player->status = OMXMP_STATE_PAUSED;
	OSCL_LOGW("------------pause end-------\n");
	pthread_mutex_unlock(&player->lock);

	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	omxmp_send_msg(player, OMXMP_MSG_ERROR, OMXMP_ERR_FUNC, 0, NULL);
	return ret;
}

int omxmp_stop(void *handle)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	ret = _stop(player);
	oscl_param_check_exit(ret == 0, -1, "stop err!\n");
	player->status = OMXMP_STATE_STOP;

	oscl_queue_flush(&player->msg_queue);
	pthread_mutex_unlock(&player->lock);
#if 0
	rt_uint32_t mem_total, mem_used, mem_max_used;
	rt_memory_info(&mem_total, &mem_used, &mem_max_used);
	LOG_W("total memory %d, use %d, max_used %d",
		mem_total, mem_used, mem_max_used);
#endif
	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	omxmp_send_msg(player, OMXMP_MSG_ERROR, OMXMP_ERR_FUNC, 0, NULL);
	return ret;
}

int omxmp_reset(void *handle)
{
	int ret = 0, i = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);

	_stop(player);

	if (player->data_url) {
		oscl_free(player->data_url);
		player->data_url = NULL;
	}
	player->has_audio = OMX_FALSE;
	player->has_video = OMX_FALSE;
	player->audio_eos = OMX_FALSE;
	player->video_eos = OMX_FALSE;
	player->reach_eos = OMX_FALSE;
	player->isloop = OMX_FALSE;
	player->status = OMXMP_STATE_IDLE;
	player->dmx.mediainfo = NULL;
	player->video_header_decoded = OMX_FALSE;
	oscl_queue_flush(&player->msg_queue);
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		player->sub[i].error_flag = OMX_FALSE;
		player->sub[i].has_subtitle = OMX_FALSE;
	}
	pthread_mutex_unlock(&player->lock);

	return ret;
}

int omxmp_seek_to(void *handle, long msec)
{
	int ret = 0, i = 0;
	omx_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE       timestamp;
	OMX_TIME_CONFIG_CLOCKSTATETYPE      clk_state;
	subtitle_info_t *psub = NULL;
	player_sub_status_e sub_status = OMXMP_SUB_STATUS_PAUSE;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	if (msec < 0 || msec > player->duration) {
		OSCL_LOGE("invalid seek position %ld\n", msec);
		return -1;
	}
	if (player->is_seeking) {
		OSCL_LOGE("seeking is not complete!\n");
		return -1;
	}
	if (player->reach_eos) {
		OSCL_LOGE("have eos message, don't seek now!\n");
		return -1;
	}

	pthread_mutex_lock(&player->lock);

	if (player->has_video && !player->video_header_decoded) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGE("video header not decode yet, don't seek now!\n");
		return -1;
	}

	if (player->status != OMXMP_STATE_PAUSED &&
		player->status != OMXMP_STATE_STARTED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGW("error state %d\n", player->status);
		return -1;
	}
	if (player->dmx.mediainfo->mediainfo.seek_enable == 0) {
		OSCL_LOGE("seek is not support!");
		pthread_mutex_unlock(&player->lock);
		return -1;
	}

	player->is_seeking = OMX_TRUE;
	player->seek_pos = msec;

	timestamp.nSize = sizeof(timestamp);
	timestamp.nPortIndex = 0;
	timestamp.nTimestamp = msec + player->dmx.mediainfo->start_time / 1000;

	if (player->status == OMXMP_STATE_PAUSED) {
		player->seek_in_pause = OMX_TRUE;
		OSCL_LOGD("seek in pause state\n");
	} else {
		for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
			psub = &player->sub[i];
			if (!psub->has_subtitle)
				continue;
			ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
				omx_index_media_sub_set_status_idx, &psub->index);
			oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");

			ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
				omx_index_media_sw_sub_status, &sub_status);
			oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");
		}
		ret = _pause(player);
		oscl_param_check_exit(ret == 0, -1, "pause err!");
	}

	/* flush ports */
	if (player->has_audio) {
		ret = component_sendcom(&player->arender.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush arender inport err!\n");
		ret = component_sendcom(&player->adec.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush adec inport err!\n");
	}
	if (player->has_video) {
		ret = component_sendcom(&player->vrender.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush vrender inport err!\n");
		if (player->rot_enable) {
			ret = component_sendcom(&player->rotate.al_comp,
					OMX_CommandFlush, 0, NULL);
			oscl_param_check_exit(ret == 0, -1, "flush rotate inport err!\n");
		}
		ret = component_sendcom(&player->vdec.al_comp,
				OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush vdec inport err!\n");
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (!psub->has_subtitle)
			continue;
		ret = component_sendcom(&psub->srender.al_comp,
			OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush srender inport err!\n");
		ret = component_sendcom(&psub->sdec.al_comp,
			OMX_CommandFlush, 0, NULL);
		oscl_param_check_exit(ret == 0, -1, "flush sdec inport err!\n");
	}

	ret = component_sendcom(&player->dmx.al_comp, OMX_CommandFlush, -1, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush demuxer inport err!\n");

	/* stop the clock */
	clk_state.nSize = sizeof(clk_state);
	clk_state.eState = OMX_TIME_ClockStateStopped;
	ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeClockState, &clk_state);
	oscl_param_check_exit(ret == 0, -1, "set clock stop err!\n");

	/* set clock to wait for start */
	clk_state.nSize = sizeof(clk_state);
	clk_state.nWaitMask = 0;
	if (player->has_audio)
		clk_state.nWaitMask |= OMX_CLOCKPORT0;
	if (player->has_video)
		clk_state.nWaitMask |= OMX_CLOCKPORT1;
	clk_state.eState = OMX_TIME_ClockStateWaitingForStartTime;
	ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeClockState, &clk_state);
	oscl_param_check_exit(ret == 0, -1, "set clock to wait for start err!\n");

	/* set the seek position to demuxer */
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			OMX_IndexConfigTimePosition, &timestamp);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to seek pos err!\n");

	if (player->has_audio && player->seek_in_pause) {
		OMX_BOOL drop_data = OMX_TRUE;
		/* don't sink audio data in pause state */
		ret = OMX_SetParameter(player->arender.al_comp.cmp_hdl,
				omx_index_media_drop_data, &drop_data);
		oscl_param_check_exit(ret == 0, -1, "set arender drop data err!");
		OSCL_LOGI("drop data %d", drop_data);
	}

	ret = component_setstate(&player->clksrc.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set clock to executing err!\n");

	if (player->has_video) {
		ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set vrender to executing err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp,
					OMX_StateExecuting);
			oscl_param_check_exit(ret == 0, -1,
				"set rotate to executing err!\n");
		}
		ret = component_setstate(&player->vdec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set vdec to executing err!\n");
	}
	if (player->has_audio) {
		ret = component_setstate(&player->arender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set arender to executing err!\n");
		ret = component_setstate(&player->adec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set adec to executing err!\n");
	}
	for (i = 0; i < DMX_SUBTITLE_STREAM_NUM; i++) {
		psub = &player->sub[i];
		if (!psub->has_subtitle)
			continue;
		oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");
		ret = component_setstate(&psub->sdec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set sdec to executing err!\n");
		ret = component_setstate(&psub->srender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set srender to executing err!\n");
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sub_set_status_idx, &psub->index);
		oscl_param_check_exit(ret == 0, ret, "pause subtitle failed!\n");
		sub_status = OMXMP_SUB_STATUS_OPEND;
		ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_sw_sub_status, &sub_status);
	}
	/* start the demuxer component & queue buffer */
	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to executing err!\n");

	pthread_mutex_unlock(&player->lock);

	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	omxmp_send_msg(player, OMXMP_MSG_ERROR, OMXMP_ERR_FUNC, 0, NULL);
	return ret;
}

int omxmp_get_state(void *handle)
{
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	return player->status;
}

int omxmp_is_playing(void *handle)
{
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	if (player->status == OMXMP_STATE_STARTED)
		return 1;
	else
		return 0;
}

long omxmp_get_current_position(void *handle)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE timestamp;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	if (player->status != OMXMP_STATE_STARTED &&
		player->status != OMXMP_STATE_PAUSED) {
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

	if (player->is_seeking)
		return player->seek_pos;

	timestamp.nPortIndex = CLOCK_PORT_AUDIO;
	timestamp.nSize = sizeof(timestamp);
	ret = OMX_GetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeCurrentMediaTime, &timestamp);
	oscl_param_check(ret == 0, -1, "get cur mediatime err!\n");

	if (timestamp.nTimestamp < player->dmx.mediainfo->start_time) {
		OSCL_LOGW("start_time[%lld] > timestamp[%lld]",
			player->dmx.mediainfo->start_time, timestamp.nTimestamp);
		timestamp.nTimestamp = 0;
	} else {
		timestamp.nTimestamp -= player->dmx.mediainfo->start_time;
	}

	return timestamp.nTimestamp / 1000;
}

long omxmp_get_duration(void *handle)
{
	omx_mediaplayer_t *player = NULL;
	oscl_param_check(handle != NULL, -1, "handle null\n");
	player = (omx_mediaplayer_t *)handle;
	return player->duration;
}

void omxmp_set_loop(void *handle, int loop)
{
	omx_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("handle null\n");
		return;
	}

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	if (loop)
		player->isloop = OMX_TRUE;
	else
		player->isloop = OMX_FALSE;
	pthread_mutex_unlock(&player->lock);
}

int omxmp_set_playback_rate(void *handle, int rate)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_SCALETYPE config_scale;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	player->play_rate = rate;
	config_scale.nSize = sizeof(config_scale);
	config_scale.xScale = rate << 16;
	ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeScale, &config_scale);
	oscl_param_check_exit(ret == 0, -1, "set clock scale err!\n");

	pthread_mutex_unlock(&player->lock);
	return ret;
EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int omxmp_set_playback_volume(void *handle, int volume)
{
	int ret;
	OMX_AUDIO_CONFIG_VOLUMETYPE config_v;
	omx_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("hanel null\n");
		return -1;
	}
	player = (omx_mediaplayer_t *)handle;

	config_v.nSize = sizeof(config_v);
	config_v.sVolume.nValue = volume;
	ret = OMX_SetConfig(player->arender.al_comp.cmp_hdl,
			OMX_IndexConfigAudioVolume, &config_v);
	if (ret != 0)
		OSCL_LOGE("set volume err!!");

	return ret;
}

int omxmp_get_playback_volume(void *handle)
{
	int ret;
	OMX_AUDIO_CONFIG_VOLUMETYPE config_v;
	omx_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("hanel null\n");
		return -1;
	}
	player = (omx_mediaplayer_t *)handle;

	memset(&config_v, 0, sizeof(config_v));
	ret = OMX_GetConfig(player->arender.al_comp.cmp_hdl,
			OMX_IndexConfigAudioVolume, &config_v);
	if (ret != 0) {
		OSCL_LOGE("get volume err!!");
		return -1;
	}

	return config_v.sVolume.nValue;
}

int omxmp_set_scaling_mode(void *handle, omx_disp_mode_e mode)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	switch (mode) {
	case OMXMP_WINDOW_ORIGINAL:
		player->disp_para.mode = VDISP_WINDOW_ORIGINAL;
		break;
	case OMXMP_WINDOW_FULL_SCREEN_VIDEO_RATIO:
		player->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		break;
	case OMXMP_WINDOW_FULL_SCREEN_SCREEN_RATIO:
		player->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO;
		break;
	case OMXMP_WINDOW_4R3MODE:
		player->disp_para.mode = VDISP_WINDOW_4R3MODE;
		break;
	case OMXMP_WINDOW_16R9MODE:
		player->disp_para.mode = VDISP_WINDOW_16R9MODE;
		break;
	case OMXMP_WINDOW_CUTEDGE:
		player->disp_para.mode = VDISP_WINDOW_CUTEDGE;
	default:
		OSCL_LOGE("error scaling mode %d\n", mode);
		player->disp_para.mode = VDISP_WINDOW_ORIGINAL;
		break;
	}

	/* setup display mode */
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_vrender_mode, &player->disp_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, -1, "set display mode err\n");

	pthread_mutex_unlock(&player->lock);
	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int omxmp_set_scale_mode(void *handle, omxmp_scale_mode_e mode)
{
	omx_mediaplayer_t *player = NULL;
	scale_mode_e scale_mode;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	if (mode == OMXMP_SCALE_MODE_ONLINE) {
		scale_mode = SCALE_MODE_ONLINE;
	} else if (mode == OMXMP_SCALE_MODE_OFFLINE) {
		scale_mode = SCALE_MODE_OFFLINE;
	} else {
		OSCL_LOGE("unknown scale mode %d!", mode);
		return -1;
	}

	player->scale_mode = scale_mode;

	return 0;
}

int omxmp_set_window(void *handle, omxmp_win_t *win)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	player->disp_para.win_rect.left = win->left;
	player->disp_para.win_rect.top  = win->top;
	player->disp_para.win_rect.width = win->width;
	player->disp_para.win_rect.height = win->height;
	/* setup display window */
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_vrender_mode, &player->disp_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, -1, "set display window err\n");

	pthread_mutex_unlock(&player->lock);
	return ret;
EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	return ret;
}

static vdisp_rotate_mode_e _omxmprot_to_disprot(omxmp_rotate_e mode)
{
	switch (mode) {
	case OMXMP_ROTATE_NONE:
		return VDISP_ROTATE_NONE;
	case OMXMP_ROTATE_90:
		return VDISP_ROTATE_90;
	case OMXMP_ROTATE_180:
		return VDISP_ROTATE_180;
	case OMXMP_ROTATE_270:
		return VDISP_ROTATE_270;
	default:
		OSCL_LOGW("mode %d not supported, set to default", mode);
		return VDISP_ROTATE_NONE;
	}
}

int omxmp_set_rotation(void *handle, omxmp_rotate_e rot_mode)
{
	omx_mediaplayer_t *player = (omx_mediaplayer_t *)handle;
	vdisp_rotate_mode_e disp_rot;
	int ret = 0;

	oscl_param_check(player != NULL, -1, "handle null\n");

	if (!player->rot_enable) {
		OSCL_LOGW("rotation was disabled!");
		return 0;
	}

	disp_rot = _omxmprot_to_disprot(rot_mode);

	pthread_mutex_lock(&player->lock);
	if (disp_rot == player->rot_mode.nRotation) {
		OSCL_LOGW("not need to set rotation!");
		goto EXIT;
	}
	player->rot_mode.nRotation = disp_rot;

	if (!player->has_video)
		goto EXIT;
	if (player->status != OMXMP_STATE_PAUSED &&
		player->status != OMXMP_STATE_PREPARED &&
		player->status != OMXMP_STATE_STARTED) {
		goto EXIT;
	}
	if (!player->video_header_decoded) {
		OSCL_LOGW("video header not decode yet!");
		goto EXIT;
	}

	/* pause the rotate component */
	ret = component_setstate(&player->rotate.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to pause err!\n");
	/* pause the vrender component */
	ret = component_setstate(&player->vrender.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "set vrender to pause err!\n");

	ret = component_sendcom(&player->rotate.al_comp, OMX_CommandFlush, 1, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush rotate  outport err!\n");
	ret = component_sendcom(&player->vrender.al_comp, OMX_CommandFlush, -1, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush rotate  outport err!\n");

	omx_size_t rot_size;
	/* get rotate inport definition */
	memset(&rot_size, 0, sizeof(rot_size));
	/* set rotate angle */
	ret = OMX_SetParameter(player->rotate.al_comp.cmp_hdl,
			OMX_IndexConfigCommonRotate, &player->rot_mode);
	oscl_param_check(ret == OMX_ErrorNone, ret, "set rotation err\n");

	/* get params after set rotate angle */
	ret = OMX_GetParameter(player->rotate.al_comp.cmp_hdl,
			omx_index_lombo_video_size, &rot_size);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get rotation err\n");
	player->rotate_width = rot_size.width;
	player->rotate_height = rot_size.height;
	player->rotate_stride = rot_size.stride;
	player->rotate_sliceheight = rot_size.slice_height;
	OSCL_LOGI("rot:%d %d %d %d",
		rot_size.width,
		rot_size.height,
		rot_size.stride,
		rot_size.slice_height);

	/* set rotate */
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			OMX_IndexConfigCommonRotate, &player->rot_mode);
	oscl_param_check(ret == OMX_ErrorNone, ret, "set rotation err\n");
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_lombo_video_size, &rot_size);

	/* start the vrender component */
	ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set vrender to executing err!\n");
	/* start the rotate component */
	ret = component_setstate(&player->rotate.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set rotate to executing err!\n");

EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int omxmp_enable_rotation(void *handle, int enable)
{
	omx_mediaplayer_t *player = (omx_mediaplayer_t *)handle;

	oscl_param_check(player != NULL, -1, "handle null\n");

	if (player->status != OMXMP_STATE_IDLE) {
		OSCL_LOGE("error state %d", player->status);
		return -1;
	}

	OSCL_LOGI("%s rotation", enable ? "enable" : "disable");
	player->rot_enable = enable;

	return 0;
}

/* 0: layer top, 1: layer bottom */
int omxmp_set_win_layer(void *handle, omxmp_win_layer_e layer)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;
	if (!player->video_header_decoded) {
		player->layer = layer;
		return ret;
	}

	pthread_mutex_lock(&player->lock);
	switch (layer) {
	case OMXMP_LAYER_TOP:
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_top, NULL);
		break;
	case OMXMP_LAYER_BOTTOM:
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_bottom, NULL);
		break;
	default:
		OSCL_LOGW("error layer %d, set to bottom as default!\n", layer);
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_bottom, NULL);
		break;
	}
	if (0 == ret)
		player->layer = layer;
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int omxmp_disable_display(void *handle)
{
	int ret = 0;
	omx_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (omx_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_vrender_disable_display, NULL);
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int omxmp_get_audio_info(void *handle, omxmp_audio_info_t *audio_info)
{
	omx_mediaplayer_t *player = handle;
	omx_mediainfo_t omx_mi;
	audio_stream_info_t *astream;
	int i;
	int ret = 0;

	oscl_param_check(player != NULL, -1, "handle null");
	oscl_param_check(player->has_audio, -1, "no audio stream");

	pthread_mutex_lock(&player->lock);
	if (player->status != OMXMP_STATE_PREPARED &&
		player->status != OMXMP_STATE_STARTED &&
		player->status != OMXMP_STATE_PAUSED) {
		OSCL_LOGW("error state %d\n", player->status);
		ret = -1;
		goto EXIT;
	}

	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_info, &omx_mi);
	oscl_param_check_exit(ret == 0, -1, "dmx get mediainfo err");

	audio_info->audio_stream_num = omx_mi.mediainfo.astream_num;
	audio_info->cur_stream = omx_mi.cur_audio_idx;
	audio_info->supported = omx_mi.audio_supported_flag;
	for (i = 0; i < audio_info->audio_stream_num; i++) {
		astream = &audio_info->stream_info[i];
		astream->bits_per_sample =
			omx_mi.mediainfo.ainfo[i].bits_per_coded_sample;
		astream->channels = omx_mi.mediainfo.ainfo[i].channels;
		astream->samplerate = omx_mi.mediainfo.ainfo[i].sample_rate;
		if (astream->text) {
			free(astream->text);
			astream->text = NULL;
		}
		if (omx_mi.mediainfo.ainfo[i].text)
			astream->text = oscl_strdup(omx_mi.mediainfo.ainfo[i].text);
	}

EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int omxmp_set_audio_track(void *handle, int track)
{
	omx_mediaplayer_t *player = handle;
	omx_mediainfo_t omx_mi;
	int ret = 0;

	oscl_param_check(player != NULL, -1, "handle null");
	oscl_param_check(player->has_audio, -1, "no audio stream");

	pthread_mutex_lock(&player->lock);

	if (player->status != OMXMP_STATE_PREPARED &&
		player->status != OMXMP_STATE_STARTED &&
		player->status != OMXMP_STATE_PAUSED) {
		OSCL_LOGW("error state %d\n", player->status);
		ret = -1;
		goto EXIT;
	}

	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_info, &omx_mi);
	oscl_param_check_exit(ret == 0, -1, "dmx get mediainfo err");
	if (track < 0 || track >= omx_mi.mediainfo.astream_num) {
		OSCL_LOGW("invalid track value %d(only %d audio track)!",
			track, omx_mi.mediainfo.astream_num);
		ret = -1;
		goto EXIT;
	}
	if (track == omx_mi.cur_audio_idx) {
		OSCL_LOGW("switch track %d equal to current!", track);
		goto EXIT;
	}
	if (!(omx_mi.audio_supported_flag & 1 << track)) {
		OSCL_LOGW("audio track %d is not supported", track);
		ret = -1;
		goto EXIT;
	}

	ret = component_setstate(&player->dmx.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "pause adec comp err!");
	ret = component_setstate(&player->adec.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "pause adec comp err!");
	ret = component_setstate(&player->arender.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "pause arender comp err!");

	ret = component_sendcom(&player->adec.al_comp, OMX_CommandFlush, 0, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush adec comp err!");

	ret = component_sendcom(&player->arender.al_comp, OMX_CommandFlush, 0, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush arender comp err!");

	omx_audio_info_t ainfo;
	dmx_ainfo_t *ai = &omx_mi.mediainfo.ainfo[track];
	ainfo.codec_id = ai->codec_id;
	ainfo.bit_rate = ai->bit_rate;
	ainfo.sample_rate = ai->sample_rate;
	ainfo.channels = ai->channels;
	ainfo.channel_layout = ai->channel_layout;
	ainfo.bitspersample = ai->bits_per_coded_sample;
	ainfo.frame_size = ai->frame_size;
	ainfo.block_align = ai->block_align;
	ainfo.extradata = ai->extradata;
	ainfo.extradata_size = ai->extradata_size;
	ret = OMX_SetParameter(player->adec.al_comp.cmp_hdl,
			omx_index_media_audio_info,
			&ainfo);
	oscl_param_check_exit(ret == OMX_ErrorNone, -1, "audio params err");

	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_audio_track, &track);
	oscl_param_check_exit(ret == 0, -1, "set audio track err!");

	ret = component_setstate(&player->arender.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "execute arender comp err!");
	ret = component_setstate(&player->adec.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "execute adec comp err!");
	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "execute dmx comp err!");

EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

static av_codec_type_e _convert_video_codec(int id)
{
	switch (id) {
	case VIDEO_CODEC_H264:
		return AV_CODEC_TYPE_H264;
	default:
		break;
	}
	return AV_CODEC_TYPE_INVALID;
}

static av_codec_type_e _convert_audio_codec(int id)
{
	switch (id) {
	case AUDIO_CODEC_PCM:
		return AV_CODEC_TYPE_PCM_S16LE;
	case AUDIO_CODEC_AAC:
		return AV_CODEC_TYPE_AAC;
	case AUDIO_CODEC_G729:
		return AV_CODEC_TYPE_G729;
	default:
		break;
	}
	return AV_CODEC_TYPE_INVALID;
}

int omxmp_set_stream(void *handle, avstream_info_t *stream_info)
{
	omx_mediaplayer_t *player = handle;
	omx_stream_info_t si;
	int ret;

	oscl_param_check(player != NULL, -1, "handle null");

	if (player->status != OMXMP_STATE_IDLE && player->status != OMXMP_STATE_INIT) {
		OSCL_LOGE("error state %d", player->status);
		return -1;
	}

	memset(&si, 0, sizeof(omx_stream_info_t));
	if (stream_info->muxer_type == MUXER_TYPE_RAW) {
		si.muxer_type = AV_MUXER_TYPE_RAW;
		if (stream_info->vstream_num == 0 && stream_info->astream_num == 0) {
			OSCL_LOGE("no stream defined!");
			return -1;
		}
		if (stream_info->astream_num > 0) {
			si.astream_num = stream_info->astream_num;
			si.audio_info.bitspersample =
				stream_info->astream_info.bits_per_sample;
			si.audio_info.channels = stream_info->astream_info.channels;
			si.audio_info.sample_rate = stream_info->astream_info.samplerate;
			si.audio_info.codec_id =
				_convert_audio_codec(stream_info->astream_info.codec);
			if (si.audio_info.codec_id == AV_CODEC_TYPE_INVALID) {
				OSCL_LOGE("invalid audio codec %d!",
					stream_info->astream_info.codec);
				return -1;
			}
		}
		if (stream_info->vstream_num > 0) {
			si.vstream_num = stream_info->vstream_num;
			si.video_info.width = stream_info->vstream_info.width;
			si.video_info.height = stream_info->vstream_info.height;
			si.video_info.codec_id =
				_convert_video_codec(stream_info->vstream_info.codec);
			if (si.video_info.codec_id == AV_CODEC_TYPE_INVALID) {
				OSCL_LOGE("invalid video codec %d!",
					stream_info->vstream_info.codec);
				return -1;
			}
		}
	} else if (stream_info->muxer_type == MUXER_TYPE_TS) {
		si.muxer_type = AV_MUXER_TYPE_TS;
	} else {
		OSCL_LOGE("unknown muxer type %d!", stream_info->muxer_type);
		return -1;
	}

	pthread_mutex_lock(&player->lock);
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl, omx_index_stream_info, &si);
	oscl_param_check_exit(ret == 0, -1, "set stream info err!");
	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
		omx_index_stream_handle, &player->stream_handle);
	oscl_param_check_exit(ret == 0, -1, "get external stream handle err!");
	pthread_mutex_unlock(&player->lock);
	return 0;

EXIT:
	player->status = OMXMP_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	return -1;
}

int omxmp_fill_stream(void *handle, avstream_t *stream)
{
	omx_mediaplayer_t *player = handle;
	av_media_type_e type;
	int ret;

	oscl_param_check(player != NULL, -1, "handle null");
	oscl_param_check(player->stream_handle != NULL, -1, "must set_stream first!");

	if (stream->type == STREAM_TYPE_VIDEO)
		type = AV_MEDIA_TYPE_VIDEO;
	else
		type = AV_MEDIA_TYPE_AUDIO;
	ret = player->stream_handle->fill_data(player->stream_handle->priv, type,
		stream->data, stream->data_size, stream->timestamp);
	if (ret != 0) {
		OSCL_LOGE("stream fill_data err!");
		return -1;
	}

	return 0;
}

int omxmp_set_io_interface(void *handle, omxmp_io_interface_t *intf)
{
	omx_mediaplayer_t *player;
	omx_io_interface_t omx_intf;
	int ret = 0;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = handle;

	if (player->status != OMXMP_STATE_INIT) {
		OSCL_LOGE("error state %d", player->status);
		return -1;
	}

	omx_intf.open = intf->open;
	omx_intf.close = intf->close;
	omx_intf.read = intf->read;
	omx_intf.lseek = intf->lseek;

	pthread_mutex_lock(&player->lock);
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_io_interface, &omx_intf);
	pthread_mutex_unlock(&player->lock);

	return ret;
}

