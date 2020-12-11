#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "nn_self_test"

#include <oscl.h>
#include <base_component.h>
#include <omx_vendor_lb.h>
#include <lombo_dec_common.h>
#include <lb_omx_core.h>
#include <lb_omx_as_string.h>
#include "vrender_component.h"
#include "clock_component.h"
#include "rotate_component.h"
#include "face_svc_dec.h"
#include "face_svc_main.h"

#include "oscl_audio.h"
#include "omx_api.h"
#include <demuxer_plugin.h>



typedef enum player_state {
	FACE_SVC_DEC_STATE_IDLE = 0,
	FACE_SVC_DEC_STATE_INIT,
	FACE_SVC_DEC_STATE_PREPARED,
	FACE_SVC_DEC_STATE_STARTED,
	FACE_SVC_DEC_STATE_PAUSED,
	FACE_SVC_DEC_STATE_STOP,
	FACE_SVC_DEC_STATE_COMPLETED,
	FACE_SVC_DEC_STATE_ERROR,
	FACE_SVC_DEC_STATE_END
} player_state_t;


typedef enum player_msg_type {
	FACE_SVC_DEC_MSG_COMPLETE = 0,
	FACE_SVC_DEC_MSG_PREPARED,
	FACE_SVC_DEC_MSG_SEEK_COMPLETE,
	FACE_SVC_DEC_MSG_ERROR,
	FACE_SVC_DEC_MSG_AUDIO_CHANGE,
	FACE_SVC_DEC_MSG_VIDEO_CHANGE,
	FACE_SVC_DEC_MSG_STREAM_EOS
} player_msg_type_e;

typedef struct demuxer_info {
	comp_info_t al_comp;
	port_info_t *aout;
	port_info_t *vout;
	port_info_t *clk;
	omx_mediainfo_t *mediainfo;
} demuxer_info_t;

typedef struct adec_info {
	comp_info_t al_comp;
	port_info_t *ain;
	port_info_t *aout;
	omx_audio_info_t ainfo;
} adec_info_t;

typedef struct vdec_info {
	comp_info_t al_comp;
	port_info_t *vin;
	port_info_t *vout;
} vdec_info_t;

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

typedef struct face_svc_dec_mediaplayer {
	demuxer_info_t dmx;
	adec_info_t adec;
	vdec_info_t vdec;
	arender_info_t arender;
	vrender_info_t vrender;
	rotate_info_t rotate;
	clock_info_t clksrc;

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

	OMX_U32 vdec_out_buf_alignment;
	/* todo :... */
} face_svc_dec_mediaplayer_t;

int face_svc_dec_send_msg(face_svc_dec_mediaplayer_t *player,
	player_msg_type_e cmd, OMX_U32 param1, void *msg_data)
{
	int ret = 0;
	oscl_message_t message;

	message.comp = player;
	message.msg_type = cmd;
	message.para1 = param1;
	message.data = msg_data;
	ret = oscl_message_put(&player->msg_queue, &message);
	if (ret)
		return OMX_ErrorInsufficientResources;
	sem_post(&player->msg_sem);

	return ret;
}

static int face_svc_dec_stream_eos(face_svc_dec_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	oscl_param_check(player != NULL, -1, "player null\n");

	if (player->status == FACE_SVC_DEC_STATE_COMPLETED) {
		OSCL_LOGE("already in complete state\n");
		return ret;
	}

	/* if it's loop, then seek to start & play from beginning */
	if (player->isloop) {
		ret = face_svc_dec_seek_to(player, 0);
		return ret;
	}

	pthread_mutex_lock(&player->lock);
	/* set clock component to IDLE */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");
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
	ret = component_setstate(&player->dmx.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to idle err!\n");

	player->status = FACE_SVC_DEC_STATE_COMPLETED;
	pthread_mutex_unlock(&player->lock);

	/* call back to notify completion */
	ret = face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_COMPLETE, 0, NULL);
	OSCL_LOGI("=====stream eos end==========\n");
	return ret;
EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

static OMX_ERRORTYPE face_svc_dec_event_handler(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_EVENTTYPE eEvent,
	OMX_U32 Data1,
	OMX_U32 Data2,
	OMX_PTR pEventData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;

	oscl_param_check((hComponent != NULL && pAppData != NULL),
		OMX_ErrorBadParameter, NULL);

	comp_info_t *al_comp = pAppData;
	face_svc_dec_mediaplayer_t *player = al_comp->priv_data;
	component = get_lb_component(hComponent);

	if (eEvent == OMX_EventCmdComplete) {
		OSCL_LOGI("complete cmd :%s, component %s",
			omx_cmd_as_string(Data1), component->name);
		if (Data1 == OMX_CommandStateSet)
			OSCL_LOGI("set state complete:%s, component %s",
				omx_state_as_string(Data2), component->name);
		sem_post(al_comp->sem_cmd);
	} else if (eEvent == OMX_EventError) {
		OSCL_LOGW("command err:%s, %x", omx_cmd_as_string(Data2), Data1);
		if (Data2 == OMX_CommandStateSet ||
			Data2 == OMX_CommandFlush ||
			Data2 == OMX_CommandPortDisable ||
			Data2 == OMX_CommandPortEnable ||
			Data2 == OMX_CommandMarkBuffer)
			sem_post(al_comp->sem_cmd);
		else {
			if (hComponent == player->vdec.al_comp.cmp_hdl) {
				face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR,
					FACE_SVC_DEC_ERR_VIDEO_DEC, NULL);
			} else if (hComponent == player->dmx.al_comp.cmp_hdl) {
				face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR,
					FACE_SVC_DEC_ERR_DEMUXER, NULL);
			}
		}
	} else if (eEvent == OMX_EventBufferFlag) {
		if ((int)Data2 == OMX_BUFFERFLAG_EOS) {
			OSCL_LOGW("buffer eos, cmp %s\n", component->name);
			if (player->arender.al_comp.cmp_hdl == hComponent)
				player->audio_eos = OMX_TRUE;
			//else if (player->vrender.al_comp.cmp_hdl == hComponent) {
			else if(player->vdec.al_comp.cmp_hdl == hComponent){
				player->video_eos = OMX_TRUE;
				player->audio_eos = OMX_TRUE;
			}
			
			
			if (player->audio_eos && player->video_eos)
				face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_STREAM_EOS,
					0, NULL);
			if (player->has_audio && !player->has_video &&
				player->audio_eos)
				face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_STREAM_EOS,
					0, NULL);
		}
	} else if (eEvent == OMX_EventPortSettingsChanged) {
		OSCL_LOGI("port setting changed, cmp %s, index %d\n",
			component->name, Data2);
		/* todo: we should get the new port definiton params here
		 * and init the auido & video decoder component here */
	} else if (eEvent == OMX_EventPortFormatDetected) {
		if (Data1 == OMX_IndexParamAudioPortFormat) {
			OSCL_LOGI("audio detected, but we don't handle it");
			player->has_audio = OMX_TRUE;
			player->dmx.mediainfo = pEventData;
		} else if (Data1 == OMX_IndexParamVideoPortFormat) {
			OSCL_LOGI("video detected");
			player->has_video = OMX_TRUE;
			player->dmx.mediainfo = pEventData;
		} else {
			OSCL_LOGW("unkown format %x\n", Data1);
		}
	} else if (eEvent == OMX_EventOutputRendered && player->is_seeking) {
		if (player->has_audio && !player->has_video &&
			hComponent == player->arender.al_comp.cmp_hdl)
			face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_SEEK_COMPLETE, 0, NULL);
		if (player->has_video && hComponent == player->vrender.al_comp.cmp_hdl)
			face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_SEEK_COMPLETE, 0, NULL);
	} else if (eEvent == OMX_EventAudioParamChanged) {
		OSCL_LOGI("audio format change");
		player->arender.ainfo = pEventData;
		face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_AUDIO_CHANGE, 0, NULL);
	} else if (eEvent == OMX_EventVideoParamChanged) {
		OSCL_LOGI("video format change");
	}

	return ret;
}

static OMX_CALLBACKTYPE face_svc_dec_component_callbacks = {
	.EventHandler = face_svc_dec_event_handler,
	.EmptyBufferDone = untunnel_empty_buffer_done,
	.FillBufferDone = untunnel_fill_buffer_done,
};





static int config_audiodec_cmp(face_svc_dec_mediaplayer_t *player)
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


 static int face_dec_frame_cb_handle(void *p, frame_t *in_frame)
 {
	 face_frame_t *f_frame = NULL;
	 frame_t *frame = NULL;
	 struct cb_priv *priv;
	 int ret;
 
	 priv = (struct cb_priv *)p;
	 if (priv->cb) {
		 f_frame = malloc(sizeof(face_frame_t));
		 if (!f_frame)
			 goto exit;
		 frame = malloc(sizeof(frame_t));
		 if (!frame)
			 goto exit;
		 memcpy(frame, in_frame, sizeof(frame_t));
		 memset(f_frame, 0, sizeof(face_frame_t));
		 f_frame->width = frame->info.video.width;
		 f_frame->height = frame->info.video.height;
		 f_frame->size[0] = frame->info.video.size[0];
		 f_frame->size[1] = frame->info.video.size[1];
		 f_frame->size[2] = frame->info.video.size[2];
		 f_frame->viraddr[0] = frame->info.video.addr[0];
		 f_frame->viraddr[1] = frame->info.video.addr[1];
		 f_frame->viraddr[2] = frame->info.video.addr[2];
		 f_frame->phyaddr[0] = lombo_vir_to_phy(f_frame->viraddr[0]);
		 f_frame->phyaddr[1] = lombo_vir_to_phy(f_frame->viraddr[1]);
		 f_frame->phyaddr[2] = 0; /* nv12 */
		 /* Fix device sync time, frame timestamp is err */
		 /* f_frame->time_stamp = frame->info.video.time_stamp; */
		 f_frame->time_stamp = get_systime_us();
		 f_frame->priv = frame;
		 ret = priv->cb(priv->p, f_frame);
		 if (ret == 0)
			 return 0;
	 }
 
 exit:
	 if (f_frame)
		 free(f_frame);
	 if (frame)
		 free(frame);
	 /* return frame immediately if cb == NULL or f_cb return failed */
	 ret = video_stream_return_frame(frame);
	 if (ret) {
		 ALOGE("return frame failed\n");
		 return -1;
	 }
 
	 return 0;
 }
 

 int face_svc_dec_rgb_frame_cb(void *p, face_frame_t *frame)
 {
	 face_svc_t *svc = (face_svc_t *)p;
	 int ret;
	 static unsigned int idx = 1;

	 frame_t *pframe = 0;
	 if (!frame || !p)
		 return -1;
	 
	 if (face_svc_conf->svc.dump_fps) {
		 static long long start = 0;
		 long long end = get_systime_us();
		 static int fps = 0;
 
		 fps++;
		 if (end - start > 1000000) {
			 ALOGI("rgb frame %d fps", fps);
			 start = get_systime_us();
			 fps = 0;
		 }
	 }
 
	 /* ALOGV("rgb cb, vir: 0x%x, width: %d, height: %d, ts: %lld\n", frame->viraddr,
			 frame->width, frame->height, frame->time_stamp); */
	pframe = (frame_t *)frame->priv;

	 if (idx++ % face_svc_conf->svc.det_step) {
		 //ALOGV("%s dropped frame: %d", __func__, idx);
		 face_media_return_frame(frame);
		 return 0;
	 }
 
	 /* for calculating time consuming of each algorithm module */
	 frame->type = FACE_FRAME_TYPE_RGB;
	 frame->det_begin = -1;
	 frame->det_end = -1;
	 frame->recg_check_cnt = 0;
	 frame->living_check_cnt = 0;
 
	 pthread_mutex_lock(&svc->rgb_frame_lock);
	 ret = en_queue(&svc->rgb_frame_queue, frame);
	 pthread_cond_signal(&svc->rgb_frame_cond);
	 pthread_mutex_unlock(&svc->rgb_frame_lock);
 
	 return ret;
 }


 
int face_svc_dec_nir_frame_cb(void *p, face_frame_t *frame)
{
	 face_svc_t *svc = (face_svc_t *)p;
	 int ret;
	 frame_t *pframe = 0;
	 if (!frame || !svc)
		 return -1;
 
	 if (face_svc_conf->svc.dump_fps) {
		 static long long start = 0;
		 long long end = get_systime_us();
		 static int fps = 0;
 
		 fps++;
		 if (end - start > CLOCKS_PER_SEC) {
			 ALOGI("nir frame %d fps", fps);
			 start = get_systime_us();
			 fps = 0;
		 }
	 }

	pframe = (frame_t *)frame->priv;

	 if ((!face_svc_conf->svc.living_check && !svc->nir_det)
				 || face_svc_conf->svc.living_check_mode) {
		 face_media_return_frame(frame);
		 return 0;
	 }
 
	 /* ALOGV("nir cb, vir: 0x%x, width: %d, height: %d, ts: %lld\n", frame->viraddr,
			 frame->width, frame->height, frame->time_stamp); */
 
	 /* for calculating time consuming of each algorithm module */
	 frame->type = FACE_FRAME_TYPE_NIR;
	 frame->det_begin = -1;
	 frame->det_end = -1;
 
	 pthread_mutex_lock(&svc->nir_frame_lock);
	 ret = en_queue(&svc->nir_frame_queue, frame);
	 pthread_mutex_unlock(&svc->nir_frame_lock);
 
	 return ret;
}

 static struct cb_priv *_face_dec_set_cb(void *hdl, void *p,
							p_face_frame_cb f_cb)
{
	app_frame_cb_t cb;
	struct cb_priv *priv;
	int ret;

	if (!hdl || !f_cb)
		return NULL;

	priv = malloc(sizeof(struct cb_priv));
	if (!priv)
		return NULL;
	priv->cb = f_cb;
	priv->p = p;

	cb.type = VS_VIDEO_ROT_FRAME;
	cb.app_data = priv;
	
	cb.buf_handle = face_dec_frame_cb_handle;
	ALOGV("try to setup_cb now\n");
	
	ret = untunnel_setup_cb((port_info_t *)hdl, &cb);
	if (ret) {
		ALOGE("set cb failed\n");
		return NULL;
	}
	ALOGV("setup_cb done\n");

	return priv;
}

int face_svc_dec_set_cb(void *hdl, enum face_camera_id camera_id,
						void *p, p_face_frame_cb f_cb)
{
	face_svc_dec_mediaplayer_t *player = (face_svc_dec_mediaplayer_t *)hdl;
	struct cb_priv *priv;
	void *handle;
	
	

//	OMX_BUFFERHEADERTYPE *outport_buf;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	port_info_t *out_port;
	int ret;

	(void)camera_id;
	
	handle = (void *)player->vdec.vout;


	out_port = player->vdec.vout;

	/* set output config to input component: get output config */
	port_def.nPortIndex = out_port->index;
	port_def.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(((comp_info_t *)out_port->comp_info)->cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	oscl_param_check(ret == OMX_ErrorNone, ret, NULL);
	OSCL_LOGI("port_def %d, %d\n", port_def.nBufferCountActual, port_def.nBufferSize);
	OSCL_LOGI("out_port %d, %d\n", out_port->nbuffer, out_port->buf_size);
	out_port->nbuffer = port_def.nBufferCountActual;
	out_port->buf_size = port_def.nBufferSize;

	if (port_def.eDomain == OMX_PortDomainVideo)
		OSCL_LOGI("eColorFormat:%d, w-h(%d, %d), (%d, %d)",
			port_def.format.video.eColorFormat,
			port_def.format.video.nFrameWidth,
			port_def.format.video.nFrameHeight,
			port_def.format.video.nStride,
			port_def.format.video.nSliceHeight);
	
	priv = _face_dec_set_cb(handle, p, f_cb);
	if (priv == NULL)
		return -1;

	return 0;
}
						

int untunnel_set_filter(port_info_t *port, app_port_filter_t *filter);

static int dec_buf_handle (void *app_data, frame_t *pframe)
{
	video_frame_info_t *vinf = NULL;
	vinf = &(pframe->info.video);

	OSCL_LOGI("%-8llu %-8d %-8d %-8d: %-4ux%-4u %u\n",
		vinf->time_stamp,
		vinf->size[0],
		vinf->size[1],
		vinf->size[2],
		vinf->width,
		vinf->height,
		vinf->planar);
	//return APP_BUFFER_HANDLED;
	return APP_BUFFER_PASS_ON;
}

app_port_filter_t dec_filter = 
{
	.app_data = NULL,
	.buf_handle = dec_buf_handle,
};

static int config_videodec_cmp(face_svc_dec_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE params;
	omx_video_info_t vinfo;
	omx_mediainfo_t *omx_mi = player->dmx.mediainfo;
	dmx_mediainfo_t *mi = &omx_mi->mediainfo;
	dmx_vinfo_t *vi = &mi->vinfo[omx_mi->cur_video_idx];

	/* get video outport definition */
	memset(&params, 0, sizeof(params));
	params.nPortIndex = player->vdec.vout->index;
	ret = OMX_GetParameter(player->vdec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");

	/* update video outport definiton */
	params.format.video.eCompressionFormat = player->video_codingtype;
	params.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
#if 0
	params.format.video.nFrameWidth = player->frame_width;
	params.format.video.nFrameHeight = player->frame_height;
	params.format.video.nStride = player->frame_stride;
	params.format.video.nSliceHeight = player->frame_sliceheight;
#else
	OSCL_LOGI("params %d %d %d %d ",
		params.format.video.nFrameWidth,
		params.format.video.nFrameHeight,
		params.format.video.nStride,
		params.format.video.nSliceHeight);

	player->frame_width = params.format.video.nFrameWidth;
	player->frame_height = params.format.video.nFrameHeight;
	player->frame_stride = params.format.video.nStride;
	player->frame_sliceheight = params.format.video.nSliceHeight;
#endif
	player->vdec_out_buf_alignment = params.nBufferAlignment;
	ret = OMX_SetParameter(player->vdec.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &params);
	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");

	OSCL_LOGI("vi %d %d %d %d ",
		vi->width,
		vi->height,
		vi->codec_id,
		vi->extradata_size);

	vinfo.width = vi->width;
	vinfo.height = vi->height;
	vinfo.codec_id = vi->codec_id;
	vinfo.extradata = vi->extradata;
	vinfo.extrasize = vi->extradata_size;

	ret = OMX_SetParameter(player->vdec.al_comp.cmp_hdl,
			omx_index_media_video_info,
			&vinfo);
	oscl_param_check(ret == OMX_ErrorNone, -1, "set media info error");

//	if (player->rot_enable) {
//		ret = untunnel_setup_ports(player->vdec.vout, player->rotate.vin);
//		oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports error");
//	} else {
//		ret = untunnel_setup_ports(player->vdec.vout, player->vrender.vin);
//		oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports error");
//	}
	//untunnel_set_filter(player->vdec.vout, &dec_filter);
	OSCL_LOGI("end");
	return ret;
}

static int config_arender_cmp(face_svc_dec_mediaplayer_t *player)
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

static int config_vrender_cmp(face_svc_dec_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
//	OMX_PARAM_PORTDEFINITIONTYPE params;
//	OMX_BOOL avsync;
//
//	/* setup display mode */
//	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
//			omx_index_vrender_mode, &player->disp_para);
//	oscl_param_check(ret == OMX_ErrorNone, ret, NULL);
//
//	/* set rotate */
//	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
//			OMX_IndexConfigCommonRotate, &player->rot_mode);
//	oscl_param_check(ret == OMX_ErrorNone, ret, "set rotation err\n");
//
//	/* get video inport definition */
//	memset(&params, 0, sizeof(params));
//	params.nPortIndex = 0;
//	ret = OMX_GetParameter(player->vrender.al_comp.cmp_hdl,
//			OMX_IndexParamPortDefinition, &params);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");
//
//	/* update video inport definiton */
//	params.nBufferAlignment = player->vdec_out_buf_alignment;
//	params.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
//	if (player->rot_enable) {
//		params.format.video.nFrameWidth = player->rotate_width;
//		params.format.video.nFrameHeight = player->rotate_height;
//		params.format.video.nStride = player->rotate_stride;
//		params.format.video.nSliceHeight = player->rotate_sliceheight;
//	} else {
//		params.format.video.nFrameWidth = player->frame_width;
//		params.format.video.nFrameHeight = player->frame_height;
//		params.format.video.nStride = player->frame_stride;
//		params.format.video.nSliceHeight = player->frame_sliceheight;
//	}
//	OSCL_LOGI("vrender: size(%dx%d), stride(%dx%d)",
//		params.format.video.nFrameWidth,
//		params.format.video.nFrameHeight,
//		params.format.video.nStride,
//		params.format.video.nSliceHeight);
//
//	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
//			OMX_IndexParamPortDefinition, &params);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");
//
//	/* set avsync */
//	avsync = OMX_TRUE;
//	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
//			omx_index_media_avsync, &avsync);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "SET avsync err\n");

	return ret;
}

static int config_rotate_cmp(face_svc_dec_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
//	OMX_PARAM_PORTDEFINITIONTYPE params;
//
//	/* get rotate inport definition */
//	memset(&params, 0, sizeof(params));
//	params.nPortIndex = player->rotate.vout->index;
//	ret = OMX_GetParameter(player->rotate.al_comp.cmp_hdl,
//			OMX_IndexParamPortDefinition, &params);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");
//
//	/* update rotate inport definiton */
//	params.nBufferAlignment = 1024;
//	params.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
//	params.format.video.nFrameWidth = player->frame_width;
//	params.format.video.nFrameHeight = player->frame_height;
//	params.format.video.nStride = player->frame_stride;
//	params.format.video.nSliceHeight = player->frame_sliceheight;
//	ret = OMX_SetParameter(player->rotate.al_comp.cmp_hdl,
//			OMX_IndexParamPortDefinition, &params);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "SET port definition err\n");
//
//	/* set rotate angle */
//	ret = OMX_SetParameter(player->rotate.al_comp.cmp_hdl,
//			OMX_IndexConfigCommonRotate, &player->rot_mode);
//	oscl_param_check(ret == OMX_ErrorNone, ret, "set rotation err\n");
//
//	/* get params after set rotate angle */
//	ret = OMX_GetParameter(player->rotate.al_comp.cmp_hdl,
//			OMX_IndexParamPortDefinition, &params);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "get port definition err\n");
//	player->rotate_width = params.format.video.nFrameWidth;
//	player->rotate_height = params.format.video.nFrameHeight;
//	player->rotate_stride = params.format.video.nStride;
//	player->rotate_sliceheight = params.format.video.nSliceHeight;
//	OSCL_LOGI("rot:%d %d %d %d", params.format.video.nFrameWidth,
//		params.format.video.nFrameHeight,
//		params.format.video.nStride,
//		params.format.video.nSliceHeight);
//
//	ret = untunnel_setup_ports(player->rotate.vout, player->vrender.vin);
//	oscl_param_check(ret == OMX_ErrorNone, -1, "untunnel_setup_ports error");

	return ret;
}

static int config_demuxer_cmp(face_svc_dec_mediaplayer_t *player)
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

static int config_clock_cmp(face_svc_dec_mediaplayer_t *player)
{
	int ret = OMX_ErrorNone;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE  refclk;

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
	ret = OMX_SetupTunnel(player->clksrc.al_comp.cmp_hdl, CLOCK_PORT_DEMUXER,
			player->dmx.al_comp.cmp_hdl, player->dmx.clk->index);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("setup dmx to clock port tunnel error!");
		return -1;
	}
	return ret;
}

static int get_mediainfo(face_svc_dec_mediaplayer_t *player)
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

void *face_svc_dec_message_thread(void *param)
{
	oscl_message_t message;
	int ret;
	face_svc_dec_mediaplayer_t *player = param;

	prctl(PR_SET_NAME, "face_svc_dec_message_thread");
	while (1) {
		if (player->status == FACE_SVC_DEC_STATE_END) {
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
		case FACE_SVC_DEC_MSG_COMPLETE:
			player->audio_eos = OMX_FALSE;
			player->video_eos = OMX_FALSE;
			if (player->callback && player->callback->on_completion)
				player->callback->on_completion(player);
			break;
		case FACE_SVC_DEC_MSG_PREPARED:
			if (player->callback && player->callback->on_prepared)
				player->callback->on_prepared(player);
			break;
		case FACE_SVC_DEC_MSG_SEEK_COMPLETE:
			if (player->seek_in_pause) {
				face_svc_dec_pause(player);
				player->seek_in_pause = OMX_FALSE;
			}
			pthread_mutex_lock(&player->lock);
			player->is_seeking = OMX_FALSE;
			pthread_mutex_unlock(&player->lock);
			if (player->callback && player->callback->on_seek_complete)
				player->callback->on_seek_complete(player);
			break;
		case FACE_SVC_DEC_MSG_ERROR:
			pthread_mutex_lock(&player->lock);
			player->status = FACE_SVC_DEC_STATE_ERROR;
			pthread_mutex_unlock(&player->lock);
			if (player->callback && player->callback->on_error)
				player->callback->on_error(player, message.para1);
			break;
		case FACE_SVC_DEC_MSG_STREAM_EOS:
			face_svc_dec_stream_eos(player);
			break;
		case FACE_SVC_DEC_MSG_AUDIO_CHANGE: {
			int ret;
			OMX_AUDIO_PARAM_PCMMODETYPE audio_params;
			memset(&audio_params, 0, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
			audio_params.nChannels = player->arender.ainfo->channels;
			audio_params.nBitPerSample =
				player->arender.ainfo->bitspersample;
			audio_params.nSamplingRate = player->arender.ainfo->sample_rate;
			ret = OMX_SetConfig(player->arender.al_comp.cmp_hdl,
					OMX_IndexParamAudioPcm, &audio_params);
			oscl_param_check(ret == OMX_ErrorNone, NULL, "set audio err");
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

void *face_svc_dec_create(mp_callback_ops_t *cb_ops)
{
	face_svc_dec_mediaplayer_t *player = NULL;
	int ret = OMX_ErrorNone;
	pthread_attr_t msg_thread_attr;
	struct sched_param shed_param = {0};
	int index;

	oscl_param_check(cb_ops != NULL, NULL, "call back ops null\n");

	player = oscl_zalloc(sizeof(*player));
	if (player == NULL) {
		OSCL_LOGE("alloc face_svc_dec_mediaplayer_t error!\n");
		return NULL;
	}

	pthread_mutex_init(&player->lock, NULL);
	pthread_mutex_lock(&player->lock);

	player->callback = cb_ops;
	player->play_rate = 1; /* default rate is normal */

	ret = component_init(&player->dmx.al_comp,
			"OMX.LB.SOURCE.DEMUXER",
			&face_svc_dec_component_callbacks);
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
			&face_svc_dec_component_callbacks);
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
			&face_svc_dec_component_callbacks);
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
			&face_svc_dec_component_callbacks);
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
			&face_svc_dec_component_callbacks);
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
			&face_svc_dec_component_callbacks);
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
			&face_svc_dec_component_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	player->clksrc.audio = &player->clksrc.al_comp.port_info[CLOCK_PORT_AUDIO];
	player->clksrc.video = &player->clksrc.al_comp.port_info[CLOCK_PORT_VIDEO];
	player->clksrc.dmx = &player->clksrc.al_comp.port_info[CLOCK_PORT_DEMUXER];
	player->clksrc.al_comp.priv_data = player;

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
			face_svc_dec_message_thread, player);

	/* change state to idle */
	player->status = FACE_SVC_DEC_STATE_IDLE;
	pthread_mutex_unlock(&player->lock);

EXIT:
	if (ret != OMX_ErrorNone) {
		pthread_mutex_unlock(&player->lock);
		face_svc_dec_release(player);
		player = NULL;
	}
	return player;
}

void face_svc_dec_release(void *handle)
{
	face_svc_dec_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("handle null\n");
		return;
	}

	player = (face_svc_dec_mediaplayer_t *)handle;

	if (player->status == FACE_SVC_DEC_STATE_PREPARED ||
		player->status == FACE_SVC_DEC_STATE_PAUSED ||
		player->status == FACE_SVC_DEC_STATE_STARTED ||
		player->status == FACE_SVC_DEC_STATE_COMPLETED) {
		OSCL_LOGW("call at a running state, so stop first\n");
		face_svc_dec_stop(player);
	}

	pthread_mutex_lock(&player->lock);
	/* quit the msg thread */
	player->status = FACE_SVC_DEC_STATE_END;
	if (player->msg_handler_thread_id == 0) {
		oscl_queue_flush(&player->msg_queue);
		sem_post(&player->msg_sem);
		pthread_join(player->msg_handler_thread, NULL);
		player->msg_handler_thread_id = -1;
	}

	component_deinit(&player->clksrc.al_comp);
	component_deinit(&player->rotate.al_comp);
	component_deinit(&player->vrender.al_comp);
	component_deinit(&player->arender.al_comp);
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

int face_svc_dec_set_data_source(void *handle, const char *url)
{
	int ret = OMX_ErrorNone;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null");

	player = (face_svc_dec_mediaplayer_t *)handle;
	if (player->status != FACE_SVC_DEC_STATE_IDLE) {
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

	pthread_mutex_lock(&player->lock);
	if (player->data_url) {
		oscl_free(player->data_url);
		player->data_url = NULL;
	}
	player->data_url = oscl_strdup(url);
	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_vendor_input_filename, player->data_url);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("set input file error!\n");
		pthread_mutex_unlock(&player->lock);
		face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR, FACE_SVC_DEC_ERR_FUNC, NULL);
		return ret;
	}
	player->status = FACE_SVC_DEC_STATE_INIT;
	pthread_mutex_unlock(&player->lock);
	OSCL_LOGI("set data url %s\n", player->data_url);
	return 0;
}

int face_svc_dec_prepare(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	if (player->status != FACE_SVC_DEC_STATE_INIT &&
		player->status != FACE_SVC_DEC_STATE_STOP) {
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

	pthread_mutex_lock(&player->lock);
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
		if (player->rot_enable) {
			ret = config_rotate_cmp(player);
			oscl_param_check_exit(ret == 0, -1, "config rotate err!\n");
		}
		ret = config_vrender_cmp(player);
		oscl_param_check_exit(ret == 0, -1, "config vrender err!\n");

		ret = component_setstate(&player->vdec.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set videodec to idle err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp, OMX_StateIdle);
			oscl_param_check_exit(ret == 0, -1, "set rotate to idle err!\n");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StateIdle);
		oscl_param_check_exit(ret == 0, -1, "set vrender to idle err!\n");
	}

	/* change state to prepared */
	player->status = FACE_SVC_DEC_STATE_PREPARED;
	pthread_mutex_unlock(&player->lock);

	ret = face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_PREPARED, 0, NULL);
	return ret;
EXIT:
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR, FACE_SVC_DEC_ERR_FUNC, NULL);
	return ret;
}

int face_svc_dec_start(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_CLOCKSTATETYPE clk_state;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE refclk;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;


	pthread_mutex_lock(&player->lock);

	if (player->status != FACE_SVC_DEC_STATE_PREPARED &&
		player->status != FACE_SVC_DEC_STATE_PAUSED &&
		player->status != FACE_SVC_DEC_STATE_COMPLETED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

	player->audio_eos = OMX_FALSE;
	player->video_eos = OMX_FALSE;
	player->is_seeking = OMX_FALSE;
	player->seek_pos  = 0;

	/* set ref clk */
	refclk.nSize = sizeof(refclk);
	if (player->has_audio)
		refclk.eClock = OMX_TIME_RefClockAudio;
	else
		refclk.eClock = OMX_TIME_RefClockNone;
	ret = OMX_SetConfig(player->clksrc.al_comp.cmp_hdl,
			OMX_IndexConfigTimeActiveRefClock, &refclk);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("set activ ref clock error!\n");
		return -1;
	}
	if (player->status == FACE_SVC_DEC_STATE_PREPARED ||
		player->status == FACE_SVC_DEC_STATE_COMPLETED) {
		/* set clock to executing */
		ret = component_setstate(&player->clksrc.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set clock to execute err!\n");
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
		ret = component_setstate(&player->adec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set audiodec to execute err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set arender to execute err!\n");
	}
	if (player->has_video) {
		ret = component_setstate(&player->vdec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set videodec to execute err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp,
					OMX_StateExecuting);
			oscl_param_check_exit(ret == 0, -1, "set rotate to execute err!");
		}
		//this will take all drm plane when in loop test mode, then cause process exit(1) drm_reserve_plane_index
//		ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
//		oscl_param_check_exit(ret == 0, -1, "set vrender to execute err!");
	}
	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to executing err!\n");
	OSCL_LOGI("-------start end--------\n");
	player->status = FACE_SVC_DEC_STATE_STARTED;

	pthread_mutex_unlock(&player->lock);

	return ret;
EXIT:
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR, FACE_SVC_DEC_ERR_FUNC, NULL);
	return ret;
}

int face_svc_dec_pause(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);

	if (player->status != FACE_SVC_DEC_STATE_STARTED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

	/* set other component to pause */
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

	player->status = FACE_SVC_DEC_STATE_PAUSED;
	OSCL_LOGW("------------pause end-------\n");
	pthread_mutex_unlock(&player->lock);

	return ret;
EXIT:
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR, FACE_SVC_DEC_ERR_FUNC, NULL);
	return ret;
}

int face_svc_dec_stop(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	if (player->status != FACE_SVC_DEC_STATE_STARTED &&
		player->status != FACE_SVC_DEC_STATE_PREPARED &&
		player->status != FACE_SVC_DEC_STATE_PAUSED &&
		player->status != FACE_SVC_DEC_STATE_COMPLETED &&
		player->status != FACE_SVC_DEC_STATE_ERROR) {
		OSCL_LOGE("error state %d\n", player->status);
		return -1;
	}

	pthread_mutex_lock(&player->lock);
	if (player->status == FACE_SVC_DEC_STATE_COMPLETED) {
		OSCL_LOGW("already idle, goto set loaded\n");
		goto set_to_loaded;
	}

	/* pause the demuxer component */
	ret = component_setstate(&player->dmx.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to pause err!\n");
	if (player->has_video) {
		ret = component_setstate(&player->vdec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set vdec to pause err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp, OMX_StatePause);
			oscl_param_check_exit(ret == 0, -1, "set rotate to pause err!\n");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set vrender to pause err!\n");
	}
	if (player->has_audio) {
		ret = component_setstate(&player->adec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set adec to pause err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set arender to pause err!\n");
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
	ret = component_sendcom(&player->dmx.al_comp, OMX_CommandFlush, -1, NULL);
	oscl_param_check_exit(ret == 0, -1, "flush demuxer inport err!\n");

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
	ret = component_setstate(&player->dmx.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to idle err!\n");
	/* set clk to idle, as supplier, set it to idle before
	 * the other client */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");

set_to_loaded:
	/* set clk to loaded */
	ret = component_setstate(&player->clksrc.al_comp, OMX_StateLoaded);
	oscl_param_check_exit(ret == 0, -1, "set clock to idle err!\n");
	if (player->has_video)
		untunnel_unset_ports(player->dmx.vout, player->vdec.vin);
	if (player->has_audio)
		untunnel_unset_ports(player->dmx.aout, player->adec.ain);
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
		untunnel_unset_ports(player->vdec.vout, player->vrender.vin);
		ret = component_setstate(&player->vdec.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, -1, "set videodec to loaded err!\n");
		if (player->rot_enable) {
			untunnel_unset_ports(player->rotate.vout, player->vrender.vin);
			ret = component_setstate(&player->rotate.al_comp,
					OMX_StateLoaded);
			oscl_param_check_exit(ret == 0, -1, "set rotate to loaded err!");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StateLoaded);
		oscl_param_check_exit(ret == 0, -1, "set vrender to loaded err!\n");

	}

	player->status = FACE_SVC_DEC_STATE_STOP;

	pthread_mutex_unlock(&player->lock);
#if 0
	rt_uint32_t mem_total, mem_used, mem_max_used;
	rt_memory_info(&mem_total, &mem_used, &mem_max_used);
	LOG_W("total memory %d, use %d, max_used %d",
		mem_total, mem_used, mem_max_used);
#endif
	return ret;
EXIT:
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR, FACE_SVC_DEC_ERR_FUNC, NULL);
	return ret;
}

int face_svc_dec_reset(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	if (player->status == FACE_SVC_DEC_STATE_STARTED ||
		player->status == FACE_SVC_DEC_STATE_PAUSED ||
		player->status == FACE_SVC_DEC_STATE_PREPARED ||
		player->status == FACE_SVC_DEC_STATE_COMPLETED ||
		player->status == FACE_SVC_DEC_STATE_ERROR) {
		OSCL_LOGI("call reset not in stop state, stop first\n");
		ret = face_svc_dec_stop(handle);
	}

	pthread_mutex_lock(&player->lock);
	if (player->data_url) {
		oscl_free(player->data_url);
		player->data_url = NULL;
	}
	player->has_audio = OMX_FALSE;
	player->has_video = OMX_FALSE;
	player->isloop = OMX_FALSE;
	player->status = FACE_SVC_DEC_STATE_IDLE;
	player->dmx.mediainfo = NULL;
	pthread_mutex_unlock(&player->lock);

	return ret;
}

int face_svc_dec_player_is_ready_for_next(void *handle)
{
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	return (player->status == FACE_SVC_DEC_STATE_IDLE) || ((player->status == FACE_SVC_DEC_STATE_IDLE));
}

int face_svc_dec_seek_to(void *handle, long msec)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE       timestamp;
	OMX_TIME_CONFIG_CLOCKSTATETYPE      clk_state;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;
	if (msec < 0 || msec > player->duration) {
		OSCL_LOGE("invalid seek position %ld\n", msec);
		return -1;
	}
	if (player->is_seeking) {
		OSCL_LOGE("seeking is not complete!\n");
		return -1;
	}
	if (player->audio_eos || player->video_eos) {
		OSCL_LOGE("have eos message, don't seek now!\n");
		return -1;
	}

	pthread_mutex_lock(&player->lock);

	if (player->status != FACE_SVC_DEC_STATE_PAUSED &&
		player->status != FACE_SVC_DEC_STATE_STARTED) {
		pthread_mutex_unlock(&player->lock);
		OSCL_LOGW("error state %d\n", player->status);
		return -1;
	}
	if (player->status == FACE_SVC_DEC_STATE_PAUSED) {
		face_svc_dec_start(player);
		player->seek_in_pause = OMX_TRUE;
		OSCL_LOGD("seek in pause state\n");
	}

	player->is_seeking = OMX_TRUE;
	player->seek_pos = msec;

	timestamp.nSize = sizeof(timestamp);
	timestamp.nPortIndex = 0;
	timestamp.nTimestamp = msec;

	/* pause the demuxer component */
	ret = component_setstate(&player->dmx.al_comp, OMX_StatePause);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to pause err!\n");
	if (player->has_video) {
		ret = component_setstate(&player->vdec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set vdec to pause err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp, OMX_StatePause);
			oscl_param_check_exit(ret == 0, -1, "set rotate to pause err!\n");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set vrender to pause err!\n");
	}
	if (player->has_audio) {
		ret = component_setstate(&player->adec.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set adec to pause err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StatePause);
		oscl_param_check_exit(ret == 0, -1, "set arender to pause err!\n");
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

	/* start the demuxer component & queue buffer */
	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set demuxer to executing err!\n");
	if (player->has_video) {
		ret = component_setstate(&player->vdec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set vdec to executing err!\n");
		if (player->rot_enable) {
			ret = component_setstate(&player->rotate.al_comp,
					OMX_StateExecuting);
			oscl_param_check_exit(ret == 0, -1,
				"set rotate to executing err!\n");
		}
		ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set vrender to executing err!\n");
	}
	if (player->has_audio) {
		ret = component_setstate(&player->adec.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set adec to executing err!\n");
		ret = component_setstate(&player->arender.al_comp, OMX_StateExecuting);
		oscl_param_check_exit(ret == 0, -1, "set arender to executing err!\n");
	}

	pthread_mutex_unlock(&player->lock);

	return ret;
EXIT:
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	face_svc_dec_send_msg(player, FACE_SVC_DEC_MSG_ERROR, FACE_SVC_DEC_ERR_FUNC, NULL);
	return ret;
}

int face_svc_dec_get_state(void *handle)
{
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	return player->status;
}

int face_svc_dec_is_playing(void *handle)
{
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	if (player->status == FACE_SVC_DEC_STATE_STARTED)
		return 1;
	else
		return 0;
}

long face_svc_dec_get_current_position(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE timestamp;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	if (player->status != FACE_SVC_DEC_STATE_STARTED &&
		player->status != FACE_SVC_DEC_STATE_PAUSED) {
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

	return timestamp.nTimestamp / 1000;
}

long face_svc_dec_get_duration(void *handle)
{
	face_svc_dec_mediaplayer_t *player = NULL;
	oscl_param_check(handle != NULL, -1, "handle null\n");
	player = (face_svc_dec_mediaplayer_t *)handle;
	return player->duration;
}

void face_svc_dec_set_loop(void *handle, int loop)
{
	face_svc_dec_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("handle null\n");
		return;
	}

	player = (face_svc_dec_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	if (loop)
		player->isloop = OMX_TRUE;
	else
		player->isloop = OMX_FALSE;
	pthread_mutex_unlock(&player->lock);
}

int face_svc_dec_set_playback_rate(void *handle, int rate)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;
	OMX_TIME_CONFIG_SCALETYPE config_scale;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

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

int face_svc_dec_set_playback_volume(void *handle, int volume)
{
	int ret;
	OMX_AUDIO_CONFIG_VOLUMETYPE config_v;
	face_svc_dec_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("hanel null\n");
		return -1;
	}
	player = (face_svc_dec_mediaplayer_t *)handle;

	config_v.nSize = sizeof(config_v);
	config_v.sVolume.nValue = volume;
	ret = OMX_SetConfig(player->arender.al_comp.cmp_hdl,
			OMX_IndexConfigAudioVolume, &config_v);
	if (ret != 0)
		OSCL_LOGE("set volume err!!");

	return ret;
}

int face_svc_dec_get_playback_volume(void *handle)
{
	int ret;
	OMX_AUDIO_CONFIG_VOLUMETYPE config_v;
	face_svc_dec_mediaplayer_t *player = NULL;

	if (handle == NULL) {
		OSCL_LOGE("hanel null\n");
		return -1;
	}
	player = (face_svc_dec_mediaplayer_t *)handle;

	memset(&config_v, 0, sizeof(config_v));
	ret = OMX_GetConfig(player->arender.al_comp.cmp_hdl,
			OMX_IndexConfigAudioVolume, &config_v);
	if (ret != 0)
		OSCL_LOGE("set volume err!!");

	return config_v.sVolume.nValue;
}

int face_svc_dec_set_scaling_mode(void *handle, omx_disp_mode_e mode)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	switch (mode) {
	case FACE_SVC_DEC_WINDOW_ORIGINAL:
		player->disp_para.mode = VDISP_WINDOW_ORIGINAL;
		break;
	case FACE_SVC_DEC_WINDOW_FULL_SCREEN_VIDEO_RATIO:
		player->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		break;
	case FACE_SVC_DEC_WINDOW_FULL_SCREEN_SCREEN_RATIO:
		player->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO;
		break;
	case FACE_SVC_DEC_WINDOW_4R3MODE:
		player->disp_para.mode = VDISP_WINDOW_4R3MODE;
		break;
	case FACE_SVC_DEC_WINDOW_16R9MODE:
		player->disp_para.mode = VDISP_WINDOW_16R9MODE;
		break;
	case FACE_SVC_DEC_WINDOW_CUTEDGE:
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
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int face_svc_dec_set_window(void *handle, face_svc_dec_win_t *win)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

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
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	return ret;
}

static vdisp_rotate_mode_e _omxmprot_to_disprot(face_svc_dec_rotate_e mode)
{
	switch (mode) {
	case FACE_SVC_DEC_ROTATE_NONE:
		return VDISP_ROTATE_NONE;
	case FACE_SVC_DEC_ROTATE_90:
		return VDISP_ROTATE_90;
	case FACE_SVC_DEC_ROTATE_180:
		return VDISP_ROTATE_180;
	case FACE_SVC_DEC_ROTATE_270:
		return VDISP_ROTATE_270;
	default:
		OSCL_LOGW("mode %d not supported, set to default", mode);
		return VDISP_ROTATE_NONE;
	}
}

int face_svc_dec_set_rotation(void *handle, face_svc_dec_rotate_e rot_mode)
{
	face_svc_dec_mediaplayer_t *player = (face_svc_dec_mediaplayer_t *)handle;
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
	if (player->status != FACE_SVC_DEC_STATE_PAUSED &&
		player->status != FACE_SVC_DEC_STATE_PREPARED &&
		player->status != FACE_SVC_DEC_STATE_STARTED) {
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

	/* start the arender component */
	ret = component_setstate(&player->vrender.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set vrender to executing err!\n");
	/* start the arender component */
	ret = component_setstate(&player->rotate.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "set rotate to executing err!\n");

EXIT:
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int face_svc_dec_enable_rotation(void *handle, int enable)
{
	face_svc_dec_mediaplayer_t *player = (face_svc_dec_mediaplayer_t *)handle;

	oscl_param_check(player != NULL, -1, "handle null\n");

	if (player->status != FACE_SVC_DEC_STATE_IDLE) {
		OSCL_LOGE("error state %d", player->status);
		return -1;
	}

	OSCL_LOGI("%s rotation", enable ? "enable" : "disable");
	player->rot_enable = enable;

	return 0;
}

/* 0: layer top, 1: layer bottom */
int face_svc_dec_set_win_layer(void *handle, face_svc_dec_win_layer_e layer)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	switch (layer) {
	case FACE_SVC_DEC_LAYER_TOP:
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_top, NULL);
		break;
	case FACE_SVC_DEC_LAYER_BOTTOM:
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_bottom, NULL);
		break;
	default:
		OSCL_LOGW("error layer %d, set to bottom as default!\n", layer);
		ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
				omx_index_vrender_win_bottom, NULL);
		break;
	}

	pthread_mutex_unlock(&player->lock);
	return ret;
}

int face_svc_dec_disable_display(void *handle)
{
	int ret = 0;
	face_svc_dec_mediaplayer_t *player = NULL;

	oscl_param_check(handle != NULL, -1, "handle null\n");

	player = (face_svc_dec_mediaplayer_t *)handle;

	pthread_mutex_lock(&player->lock);
	ret = OMX_SetParameter(player->vrender.al_comp.cmp_hdl,
			omx_index_vrender_disable_display, NULL);
	pthread_mutex_unlock(&player->lock);
	return ret;
}

int face_svc_dec_get_audio_info(void *handle, face_svc_dec_audio_info_t *audio_info)
{
	face_svc_dec_mediaplayer_t *player = handle;
	omx_mediainfo_t omx_mi;
	face_svc_dec_audio_stream_t *astream;
	int i;
	int ret = 0;

	oscl_param_check(player != NULL, -1, "handle null");

	if (player->status != FACE_SVC_DEC_STATE_PREPARED &&
		player->status != FACE_SVC_DEC_STATE_STARTED &&
		player->status != FACE_SVC_DEC_STATE_PAUSED) {
		OSCL_LOGW("error state %d\n", player->status);
		return -1;
	}

	if (!player->has_audio) {
		OSCL_LOGW("no audio stream");
		return -1;
	}

	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_info, &omx_mi);
	oscl_param_check(ret == 0, -1, "dmx get mediainfo err");

	audio_info->audio_stream_num = omx_mi.mediainfo.astream_num;
	audio_info->cur_stream = omx_mi.cur_audio_idx;
	audio_info->supported = omx_mi.audio_supported_flag;
	for (i = 0; i < audio_info->audio_stream_num; i++) {
		astream = &audio_info->audio_stream[i];
		astream->bits = omx_mi.mediainfo.ainfo[i].bits_per_coded_sample;
		astream->channels = omx_mi.mediainfo.ainfo[i].channels;
		astream->samplerate = omx_mi.mediainfo.ainfo[i].sample_rate;
	}

	return 0;
}

int face_svc_dec_set_audio_track(void *handle, int track)
{
	face_svc_dec_mediaplayer_t *player = handle;
	omx_mediainfo_t omx_mi;
	int ret = 0;

	oscl_param_check(player != NULL, -1, "handle null");

	if (player->status != FACE_SVC_DEC_STATE_PREPARED &&
		player->status != FACE_SVC_DEC_STATE_STARTED &&
		player->status != FACE_SVC_DEC_STATE_PAUSED) {
		OSCL_LOGW("error state %d\n", player->status);
		return -1;
	}

	if (!player->has_audio) {
		OSCL_LOGW("no audio stream");
		return -1;
	}

	pthread_mutex_lock(&player->lock);

	ret = OMX_GetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_info, &omx_mi);
	oscl_param_check(ret == 0, -1, "dmx get mediainfo err");
	if (track < 0 || track >= omx_mi.mediainfo.astream_num) {
		OSCL_LOGW("invalid track value %d(only %d audio track)!",
			track, omx_mi.mediainfo.astream_num);
		pthread_mutex_unlock(&player->lock);
		return -1;
	}
	if (track == omx_mi.cur_audio_idx) {
		OSCL_LOGW("switch track %d equal to current!", track);
		pthread_mutex_unlock(&player->lock);
		return 0;
	}
	if (!(omx_mi.audio_supported_flag & 1 << track)) {
		OSCL_LOGW("audio track %d is not supported", track);
		pthread_mutex_unlock(&player->lock);
		return -1;
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
	oscl_param_check(ret == OMX_ErrorNone, -1, "audio params err");

	ret = OMX_SetParameter(player->dmx.al_comp.cmp_hdl,
			omx_index_media_audio_track, &track);
	oscl_param_check_exit(ret == 0, -1, "set audio track err!");

	ret = component_setstate(&player->arender.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "execute arender comp err!");
	ret = component_setstate(&player->adec.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "execute adec comp err!");
	ret = component_setstate(&player->dmx.al_comp, OMX_StateExecuting);
	oscl_param_check_exit(ret == 0, -1, "execute dmx comp err!");

	pthread_mutex_unlock(&player->lock);
	return ret;

EXIT:
	player->status = FACE_SVC_DEC_STATE_ERROR;
	pthread_mutex_unlock(&player->lock);
	return ret;
}

