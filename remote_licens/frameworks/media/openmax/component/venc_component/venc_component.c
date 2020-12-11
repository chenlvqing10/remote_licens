#define DBG_LEVEL		DBG_WARNING
#define LOG_TAG			"video_encoder_component"

#include "oscl.h"
#include "venc_component.h"
#include "lombo_al_lib.h"
#include "lombo_enc_common.h"
#include "lombo_encplugin.h"
#ifdef __ANDROID__
#include <nativebase/nativebase.h>
#include <hardware/gralloc.h>
#include "android_hw_api.h"
#include "lb_omx_index.h"
#else
#endif
#define BLENDING_BUF_ALIGN 4096

#ifdef __linux__
	#define DYNAMIC_LOADING
#endif

#ifdef DYNAMIC_LOADING
#include <dlfcn.h>
static int __load_symb(venc_component_private_t *vec_private)
{
	int ret = 0;
	venc_plugin_t *(*func_get_plugin_info)(void);
	char *shared_lib;

	if (vec_private->lib_handle != NULL) {
		OSCL_LOGE("symble not unload while load symbol");
		dlclose(vec_private->lib_handle);
		vec_private->lib_handle = NULL;
	}
	if (vec_private->encode_parm.role_type == H264_ROLE_TYPE)
		shared_lib = "vce_h264.so";
	else if (vec_private->encode_parm.role_type == H265_ROLE_TYPE)
		shared_lib = "vce_h265.so";
	else if (vec_private->encode_parm.role_type == JPEG_ROLE_TYPE)
		shared_lib = "vce_mjpeg.so";
	else {
		OSCL_LOGE("unsupport role(%d), set to default(h264)",
			vec_private->encode_parm.role_type);
		vec_private->encode_parm.role_type = H264_ROLE_TYPE;
		shared_lib = "vce_h264.so";
	}

	if (vec_private->lib_handle == NULL) {
		vec_private->lib_handle = dlopen(shared_lib, RTLD_LAZY);
		oscl_param_check_exit(vec_private->lib_handle != NULL, -1, NULL);

		func_get_plugin_info = dlsym(vec_private->lib_handle, "get_plugin_info");
		oscl_param_check_exit(func_get_plugin_info != NULL, -1, NULL);

		vec_private->video_encode_info =
				(venc_plugin_t *)func_get_plugin_info();
		oscl_param_check_exit(vec_private->video_encode_info != NULL, -1, NULL);
	}
EXIT:
	return ret;
}

static void __unload_symb(venc_component_private_t *vec_private)
{
	if (vec_private->lib_handle) {
		dlclose(vec_private->lib_handle);
		vec_private->lib_handle = NULL;
	}
	vec_private->video_encode_info = NULL;
}
void al_vc_core_init(void)
{
	return;
}
#else
venc_plugin_t venc_h264_plugin = {
	.open = h264_enc_open,
	.queue_buf = h264_queue_buf,
	.encode_frame = h264_enc_frame,
	.dequeue_buf = h264_dequeue_buf,
	.close = h264_enc_close,
	.ex_ops = h264_enc_ops,
};
venc_plugin_t venc_mjpeg_plugin = {
	.open = mjpeg_enc_open,
	.queue_buf = mjpeg_queue_buf,
	.encode_frame = mjpeg_enc_frame,
	.dequeue_buf = mjpeg_dequeue_buf,
	.close = mjpeg_enc_close,
	.ex_ops = mjpeg_enc_ops,
};
static int __load_symb(venc_component_private_t *vec_private)
{
	vec_private->video_encode_info = &venc_h264_plugin;
	if (vec_private->encode_parm.role_type == JPEG_ROLE_TYPE)
		vec_private->video_encode_info = &venc_mjpeg_plugin;
	return 0;
}

static void __unload_symb(venc_component_private_t *vec_private)
{
	vec_private->video_encode_info = NULL;
}
#endif

static int OmxFormat_to_EncodeFormat(int OmxFormat)
{
	int EncodeFormat = -1;

	if (OmxFormat == OMX_COLOR_FormatYUV420Planar)
		EncodeFormat = ENC_YUV420P;
	else if (OmxFormat == OMX_COLOR_FormatYUV420SemiPlanar)
		EncodeFormat = ENC_YUV420SP;

	if (EncodeFormat == -1)
		OSCL_LOGE("not support the OmxFormat:%d\n", OmxFormat);

	return EncodeFormat;
}

static int OmxFormat_to_BlendingFormat(int OmxFormat)
{
	int EncodeFormat = -1;

	if (OmxFormat == OMX_COLOR_Format32bitARGB8888)
		EncodeFormat = ENC_ARGB8888;
	else if (OmxFormat == OMX_COLOR_Format32bitBGRA8888)
		EncodeFormat = ENC_BGRA8888;
	else if (OmxFormat == OMX_COLOR_Format32BitRGBA8888)
		EncodeFormat = ENC_RGBA8888;

	if (EncodeFormat == -1)
		OSCL_LOGE("not support the OmxFormat blending\n");
	return EncodeFormat;
}

static int write_watermark(venc_component_private_t *vec_private,
					wm_data_t *wm_data)
{
	watermark_picture_t *wm_pic;
	int index, align_width, align_height;
	int pic_size = 0, bpp = 0;
	int ret = 0;
	int new_wm = 0;
	unsigned char *blending_buf = NULL;

	oscl_param_check_exit(vec_private != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check_exit(vec_private->codec_handle != NULL,
		OMX_ErrorBadParameter, NULL);
	oscl_param_check_exit(wm_data != NULL, OMX_ErrorBadParameter, NULL);
	index = wm_data->index;
	oscl_param_check_exit((index >= 0), OMX_ErrorBadParameter, NULL);
	oscl_param_check_exit((index < MAX_ENC_BLENDING_NUM),
				OMX_ErrorBadParameter, NULL);

	wm_pic = &vec_private->wartermark.watermark_pictures[index];
	new_wm = vec_private->wartermark.watermark_picture_num;
	blending_buf = wm_pic->vir_addr;

	if ((wm_data->wm_pic.colorspace == OMX_COLOR_Format32bitARGB8888) ||
		(wm_data->wm_pic.colorspace == OMX_COLOR_Format32bitBGRA8888) ||
		(wm_data->wm_pic.colorspace == OMX_COLOR_Format32BitRGBA8888))
		bpp = 4;
	else {
		OSCL_LOGE("Not support watermark pic format.");
		return OMX_ErrorBadParameter;
	}
	align_width = (wm_data->wm_pic.blending_width + 15) & 0xFFFFFFF0;
	align_height = (wm_data->wm_pic.blending_height + 15) & 0xFFFFFFF0;
	pic_size = align_width * align_height * bpp;
	if (!blending_buf)
		new_wm++;
	if (blending_buf &&
		((wm_pic->blending_width != wm_data->wm_pic.blending_width) ||
		(wm_pic->blending_height != wm_data->wm_pic.blending_height))) {
			oscl_free_align(blending_buf);
			wm_pic->vir_addr = NULL;
			blending_buf = NULL;
	}
	if (!blending_buf) {
		if (pic_size)
			blending_buf = oscl_malloc_align(pic_size,
							BLENDING_BUF_ALIGN);
		if (!blending_buf) {
			OSCL_LOGE("Malloc fail. %d", pic_size);
			return OMX_ErrorBadParameter;
		}
	}

	memcpy(blending_buf, wm_data->wm_pic.vir_addr, pic_size);
	memcpy(wm_pic, &wm_data->wm_pic, sizeof(watermark_picture_t));
	wm_pic->blending_enable = 1;
	wm_pic->colorspace = OmxFormat_to_BlendingFormat(wm_data->wm_pic.colorspace);
	wm_pic->vir_addr = blending_buf;
	wm_pic->phy_addr = oscl_virt_to_phys(blending_buf);
	vec_private->wartermark.watermark_picture_num = new_wm;

	OSCL_LOGI("watermark_picture_num:%d, index:%d\n", new_wm, index);
	ret = vec_private->video_encode_info->ex_ops(vec_private->codec_handle,
		VENC_SET_BLENDING_PICTURE, &vec_private->wartermark);
EXIT:
	if (ret != 0)
		OSCL_LOGE("set blending picture error\n");
	return ret;

}

static int disable_watermark(venc_component_private_t *vec_private, int index)
{
	int ret = 0;
	watermark_picture_t *wm_pic = NULL;

	oscl_param_check_exit(vec_private != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check_exit(vec_private->codec_handle != NULL,
		OMX_ErrorBadParameter, NULL);
	oscl_param_check_exit((index >= 0), OMX_ErrorBadParameter, NULL);
	oscl_param_check_exit((index < MAX_ENC_BLENDING_NUM),
					OMX_ErrorBadParameter, NULL);

	wm_pic = &vec_private->wartermark.watermark_pictures[index];
	wm_pic->blending_enable = 0;
	ret = vec_private->video_encode_info->ex_ops(vec_private->codec_handle,
		VENC_SET_BLENDING_PICTURE, &vec_private->wartermark);

EXIT:
	if (ret != 0)
		OSCL_LOGE("set blending picture error\n");
	return ret;

}

static void free_inbuf_info(int mode, venc_inbuf_info_t *info)
{
	if (NULL == info)
		return;
	if (info->state == BUF_OWN_BY_COMP || info->state == BUF_OWN_BY_ENCODER)
		OSCL_LOGE("err state(%d) while free.", info->state);
	if (mode == ENC_INMODE_BYTEBUF_VIR) {
		oscl_free_align(info->vir_addr);
		info->vir_addr = NULL;
		info->phy_addr = 0;
	}
}
static void free_inbuf_sum(venc_component_private_t *ctx)
{
	int i;
	lb_omx_component_t *component;
	base_port_t *inport;
	pthread_mutex_lock(&ctx->lock);
	component = ctx->lb_component;
	inport = &component->port[INPUT_PORT];
	for (i = 0; i < (int)inport->num_assigned; i++) {
		if (inport->header[i] != NULL)
			inport->header[i]->pInputPortPrivate = NULL;
	}
	for (i = 0; i < ctx->inbuf_sum.inbuf_cnt; i++) {
		if (ctx->inbuf_sum.inbuf_infos)
			free_inbuf_info(ctx->inbuf_sum.inbuf_mode,
				&ctx->inbuf_sum.inbuf_infos[i]);
	}
	if (ctx->inbuf_sum.inbuf_infos)
		oscl_free(ctx->inbuf_sum.inbuf_infos);
	ctx->inbuf_sum.inbuf_infos = NULL;
	oscl_free_align(ctx->inbuf_sum.vir_cap_buf);
	ctx->inbuf_sum.vir_cap_buf = NULL;
	ctx->inbuf_sum.phy_cap_buf = 0;
	ctx->inbuf_sum.cap_size = 0;
	pthread_mutex_unlock(&ctx->lock);
}

static void free_outbuf_info(int mode, venc_outbuf_info_t *info)
{
}
static void free_outbuf_sum(venc_component_private_t *ctx)
{
	int i;
	lb_omx_component_t *component;
	base_port_t *outport;
	pthread_mutex_lock(&ctx->lock);
	component = ctx->lb_component;
	outport = &component->port[OUTPUT_PORT];
	for (i = 0; i < (int)outport->num_assigned; i++) {
		if (outport->header[i] != NULL)
			outport->header[i]->pOutputPortPrivate = NULL;
	}
	for (i = 0; i < ctx->outbuf_sum.outbuf_cnt; i++) {
		free_outbuf_info(ctx->outbuf_sum.outbuf_mode,
			&ctx->outbuf_sum.outbuf_infos[i]);
	}
	oscl_free(ctx->outbuf_sum.outbuf_infos);
	ctx->outbuf_sum.outbuf_infos = NULL;
	oscl_free(ctx->outbuf_sum.tmpbuf_for_cycle);
	ctx->outbuf_sum.tmpbuf_for_cycle = NULL;
	ctx->outbuf_sum.tmpbuf_index = 0;
	ctx->outbuf_sum.tmpbuf_size = 0;
	pthread_mutex_unlock(&ctx->lock);
}

static int OmxFormat_to_plan_num(int OmxFormat)
{
	int plan_num = -1;

	if (OmxFormat == OMX_COLOR_FormatYUV420Planar)
		plan_num = 3;
	else if (OmxFormat == OMX_COLOR_FormatYUV420SemiPlanar)
		plan_num = 2;
	else
		plan_num = -1;

	if (plan_num == -1)
		OSCL_LOGE("not support the OmxFormat\n");

	return plan_num;
}

static int update_encode_param(OMX_PARAM_PORTDEFINITIONTYPE *port_def,
	vcodec_cfg_t *encode_parm, venc_component_private_t *vec_private)
{
	int ret = 0;
	oscl_param_check(port_def != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(encode_parm != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(vec_private != NULL, OMX_ErrorBadParameter, NULL);

	if (port_def->nPortIndex == INPUT_PORT) {
		encode_parm->in_width = port_def->format.video.nFrameWidth;
		encode_parm->in_height = port_def->format.video.nFrameHeight;
		encode_parm->l_stride = port_def->format.video.nStride;
		encode_parm->input_mode = OmxFormat_to_EncodeFormat
					(port_def->format.video.eColorFormat);
		vec_private->cap_plan_num = OmxFormat_to_plan_num
					(port_def->format.video.eColorFormat);
		OSCL_LOGI("frame_rate:%d:%d", port_def->format.video.xFramerate,
			encode_parm->frame_rate);
		encode_parm->frame_rate = port_def->format.video.xFramerate;
		if (encode_parm->frame_rate > (1<<16))
			encode_parm->frame_rate = encode_parm->frame_rate >> 16;
		else if (encode_parm->frame_rate > 1000)
			encode_parm->frame_rate /= 1000;
		OSCL_LOGI("frame_rate:%d:%d", port_def->format.video.xFramerate,
			encode_parm->frame_rate);
	} else if (port_def->nPortIndex == OUTPUT_PORT) {
		encode_parm->out_width = port_def->format.video.nFrameWidth;
		encode_parm->out_height = port_def->format.video.nFrameHeight;
		encode_parm->bitrate = port_def->format.video.nBitrate;
		OSCL_LOGI("bitrate:%d", port_def->format.video.nBitrate);
		OSCL_LOGI("frame_rate:%d:%d", port_def->format.video.xFramerate,
			encode_parm->frame_rate);
	} else
		ret = -1;
	return ret;
}


OMX_ERRORTYPE video_encoder_set_config(OMX_IN OMX_HANDLETYPE hcomp,
		OMX_IN OMX_INDEXTYPE cfg_index,
		OMX_IN OMX_PTR cfg_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;
	OMX_TIME_CONFIG_TIMESTAMPTYPE *time;

	oscl_param_check(hcomp != NULL, -1, NULL);
	oscl_param_check(cfg_data != NULL, -1, NULL);

	component = get_lb_component(hcomp);
	oscl_param_check(component != NULL, -1, NULL);
	vec_private = (venc_component_private_t *)(component->component_private);
	oscl_param_check(vec_private != NULL, -1, NULL);

	switch ((OMX_U32)cfg_index) {
	case omx_index_lombo_config_cur_time:
		time = cfg_data;
		vec_private->ref_time = time->nTimestamp;
		break;
	default:
		ret = base_set_config(hcomp, cfg_index, cfg_data);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE video_encoder_get_parameter(OMX_IN OMX_HANDLETYPE     hComp,
	OMX_IN OMX_INDEXTYPE    paramIndex,
	OMX_INOUT OMX_PTR       paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;
	vcodec_cfg_t *encode_parm = NULL;
	unsigned int *parm;

	component = get_lb_component(hComp);
	vec_private = (venc_component_private_t *)(component->component_private);
	if (vec_private == NULL) {
		OSCL_LOGE("vec_private is NULL\n");
		ret = OMX_ErrorBadParameter;
		return ret;
	}
	encode_parm = &(vec_private->encode_parm);

	switch ((OMX_U32)paramIndex) {
	case omx_index_lombo_venc_framerate:
		parm = (unsigned int *)paramData;
		*parm = encode_parm->frame_rate;
		break;
	case omx_index_lombo_venc_voprefresh:
		parm = (unsigned int *)paramData;
		*parm = encode_parm->idr_period;
		break;
	default:
		ret = base_get_parameter(hComp, paramIndex, paramData);
		break;
	}
	return ret;
}

OMX_ERRORTYPE video_encoder_set_parameter(OMX_IN OMX_HANDLETYPE  hComp,
		OMX_IN OMX_INDEXTYPE  paramIndex,
		OMX_IN OMX_PTR        paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;
	OMX_PARAM_PORTDEFINITIONTYPE *port_def;
	OMX_PARAM_COMPONENTROLETYPE *pRole = NULL;
	vcodec_cfg_t *encode_parm;
	long index;

	component = get_lb_component(hComp);
	vec_private = (venc_component_private_t *)(component->component_private);
	if (vec_private == NULL) {
		OSCL_LOGE("vec_private is NULL\n");
		ret = OMX_ErrorBadParameter;
		return ret;
	}
	encode_parm = &(vec_private->encode_parm);
	if (encode_parm == NULL) {
		OSCL_LOGE("encode_parm5 is NULL\n");
		ret = OMX_ErrorBadParameter;
		return ret;
	}

	switch ((OMX_U32)paramIndex) {
	case OMX_IndexParamPortDefinition:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("base set port definition error\n");
			return ret;
		}
		port_def = (OMX_PARAM_PORTDEFINITIONTYPE *)paramData;
		oscl_param_check(port_def->nPortIndex < component->num_ports,
			OMX_ErrorBadParameter, NULL);

		if (port_def->nPortIndex == INPUT_PORT) {
			encode_parm->in_width = port_def->format.video.nFrameWidth;
			encode_parm->in_height = port_def->format.video.nFrameHeight;
			encode_parm->l_stride = port_def->format.video.nStride;
			encode_parm->input_mode = OmxFormat_to_EncodeFormat
						(port_def->format.video.eColorFormat);
			vec_private->cap_plan_num = OmxFormat_to_plan_num
						(port_def->format.video.eColorFormat);
			encode_parm->frame_rate = port_def->format.video.xFramerate;
			if (encode_parm->frame_rate > 1000)
				encode_parm->frame_rate /= 1000;
		} else if (port_def->nPortIndex == OUTPUT_PORT) {
			encode_parm->out_width = port_def->format.video.nFrameWidth;
			encode_parm->out_height = port_def->format.video.nFrameHeight;
			encode_parm->bitrate = port_def->format.video.nBitrate;
			OSCL_LOGI("bitrate:%x", port_def->format.video.nBitrate);
		}
		pthread_mutex_lock(&vec_private->lock);
		update_encode_param(port_def, encode_parm, vec_private);
		pthread_mutex_unlock(&vec_private->lock);
		break;

	case omx_index_lombo_venc_bitrate:
		encode_parm->bitrate = *((int *)paramData);
		break;
	case omx_index_lombo_venc_mjpeg_quality:
		encode_parm->quality = *((int *)paramData);
		break;
	case omx_index_lombo_venc_rect: {
		win_rect_t *crop_rect;
		crop_rect = (win_rect_t *)paramData;
		encode_parm->rect.crop_x = crop_rect->x;
		encode_parm->rect.crop_y = crop_rect->y;
		encode_parm->rect.crop_width = crop_rect->width;
		encode_parm->rect.crop_height = crop_rect->height;
		break;
	}
	case omx_index_lombo_venc_slicemode:
		encode_parm->slice_mode = *((int *)paramData);
		break;
	case omx_index_lombo_venc_voprefresh:
		encode_parm->idr_period = *((int *)paramData);
		break;
	case OMX_IndexParamStandardComponentRole: {
		int role_type = UNSUPPORT_ROLE_TYPE;
		OMX_VIDEO_CODINGTYPE compress = OMX_VIDEO_CodingUnused;
		pRole = (OMX_PARAM_COMPONENTROLETYPE *)paramData;
		port_def = &component->port[OUTPUT_PORT].port_param;
		if (strcmp((char *)(pRole->cRole), "video_encode.h264") == 0 ||
			strcmp((char *)(pRole->cRole), "video_encode.avc") == 0) {
			role_type = H264_ROLE_TYPE;
			compress = OMX_VIDEO_CodingAVC;
		} else if (strcmp((char *)(pRole->cRole), "video_encode.jpeg") == 0) {
			role_type = JPEG_ROLE_TYPE;
			compress = OMX_VIDEO_CodingMJPEG;
		} else if (strcmp((char *)(pRole->cRole), "video_decoder.hevc") == 0) {
			role_type = H265_ROLE_TYPE;
			compress = OMX_VIDEO_CodingHEVC;
		} else {
			OSCL_LOGE("no support role %s\n", pRole->cRole);
			ret = OMX_ErrorBadParameter;
		}
		if (role_type != vec_private->encode_parm.role_type) {
			vec_private->encode_parm.role_type = role_type;
			__load_symb(vec_private);
			OSCL_LOGI("venc init start..");
			al_vc_core_init();
			OSCL_LOGI("venc init end..");
			port_def->format.video.eCompressionFormat = compress;
		}
		break;
	}
	case omx_index_lombo_capture_plan_offsets:
		if (paramData == NULL) {
			OSCL_LOGE("paramData is NULL\n");
			ret = OMX_ErrorBadParameter;
			return ret;
		}
		if (vec_private->codec_handle != NULL) {
			OSCL_LOGE("err state! codec already opened\n");
			ret = OMX_ErrorBadParameter;
			return ret;
		}
		memcpy(vec_private->cap_offsets, paramData, sizeof(int)*3);
		OSCL_LOGI("offsets : %d - %d - %d\n",
			vec_private->cap_offsets[0], vec_private->cap_offsets[1],
				vec_private->cap_offsets[2]);
		break;
	case omx_index_lombo_blending_picture_indexs:
		pthread_mutex_lock(&vec_private->encode_mutex);
		OSCL_LOGI("omx_index_lombo_blending_picture_indexs");
		ret = write_watermark(vec_private, paramData);
		pthread_mutex_unlock(&vec_private->encode_mutex);
		break;
	case omx_index_lombo_disable_blending_picture:
		OSCL_LOGI("omx_index_lombo_disable_blending_picture");
		pthread_mutex_lock(&vec_private->encode_mutex);
		index = (long)paramData;
		ret = disable_watermark(vec_private, index);
		pthread_mutex_unlock(&vec_private->encode_mutex);
		break;
	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}
	return ret;
}

OMX_ERRORTYPE video_encoder_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;
	component = get_lb_component(hComp);
	vec_private = (venc_component_private_t *)component->component_private;
	OMX_STATETYPE old_state = component->state;

	OSCL_LOGI("video_encoder_set_state state:%d->%d", old_state, dest_state);
	ret = base_component_set_state(hComp, dest_state);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base component set state error\n");
		return ret;
	}
	if ((component->state == OMX_StateIdle) && (old_state == OMX_StateLoaded)) {
		if (vec_private->init_codec_flag == 0) {
			ret = codec_init(vec_private);
			if (ret != 0) {
				OSCL_LOGE("encodec_init error\n");
				ret = OMX_ErrorDynamicResourcesUnavailable;
				return ret;
			}
			vec_private->init_codec_flag = 1;
		} else {
			OSCL_LOGE("encode lib has been opened, need debug\n");
		}
	} else if ((component->state == OMX_StateLoaded)
			&& (old_state == OMX_StateIdle)) {
		if (vec_private->init_codec_flag == 1) {
			while (vec_private->is_encoding_flag == 1) {
				OSCL_LOGE("wait for encoding end\n");
				oscl_mdelay(200);
			}
			ret = codec_deinit(vec_private);
			if (ret != 0) {
				OSCL_LOGE("encode deinit error\n");
				return ret;
			}
			vec_private->init_codec_flag = 0;
			__unload_symb(vec_private);
			__load_symb(vec_private);
		} else
			OSCL_LOGE("The lib has not be inited\n");
		free_outbuf_sum(vec_private);
		free_inbuf_sum(vec_private);
	} else if ((component->state == OMX_StateIdle)
			&& (old_state == OMX_StateExecuting)) {
		while (vec_private->is_encoding_flag == 1) {
			OSCL_LOGE("wait encode frame end\n");
			oscl_mdelay(100);
		}
		vec_private->ref_time = LLONG_MAX;
	}
	return ret;
}

OMX_ERRORTYPE video_encoder_fill_this_buffer(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_BUFFERHEADERTYPE *header)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(header != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	vec_private = (venc_component_private_t *)(component->component_private);

#ifdef FILL_BUFFER_INTERVAL_DEBUG
	vec_private->encode_num++;
	int time = oscl_get_msec();
	if ((time - vec_private->pre_time) > INTERVAL_TIME) {
		OSCL_LOGE("can not get a empty buffer : %d  %d\n",
			time, vec_private->pre_time);
		/* OSCL_LOGE("can not get a empty buffer3 : %d  %d\n",
			vec_private->pre_num, vec_private->encode_num); */
	}
	vec_private->pre_time = time;
	vec_private->pre_num = vec_private->encode_num;
#endif

#ifdef __EOS__
	rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, header->pBuffer, header->nAllocLen);
#endif
	codec_queue_buffer(vec_private, header);

	header->nFilledLen = 0;
	header->nTimeStamp = 0;
	ret = base_fill_this_buffer(hComp, header);

	return ret;
}


void video_encoder_buffer_handle(OMX_HANDLETYPE cmp_handle,
		OMX_BUFFERHEADERTYPE *inbuffer,
		OMX_BUFFERHEADERTYPE *outbuffer)
{
	lb_omx_component_t *component = NULL;
	venc_component_private_t *vec_private = NULL;

	component = (lb_omx_component_t *)cmp_handle;
	vec_private = (venc_component_private_t *)(component->component_private);
	if (inbuffer)
		codec_encode_frame(vec_private, inbuffer);
	if (outbuffer)
		codec_dequeue_buffer(vec_private, outbuffer, 0);
	return;
}

void *video_encoder_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;
	base_port_t *inport;
	base_port_t *outport;
	OMX_BUFFERHEADERTYPE *outbuffer = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;
	int codec_deq_timeout = 0;

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(param);
	inport = &component->port[BASE_INPUT_PORT];
	outport = &component->port[BASE_OUTPUT_PORT];

	vec_private = (venc_component_private_t *)
					(component->component_private);
	/* checks if the component is in a state able to receive buffers */
	while (component->state == OMX_StateIdle
		|| component->state == OMX_StateExecuting
		|| component->state == OMX_StatePause) {

		pthread_mutex_lock(&component->flush_mutex);
		while (inport->is_flushed || outport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			if (inbuffer && inport->is_flushed) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
			}
			if (outbuffer && outport->is_flushed) {
				outport->return_buffer(outport, outbuffer);
				outbuffer = NULL;
			}

			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			pthread_mutex_lock(&component->flush_mutex);
		}
		pthread_mutex_unlock(&component->flush_mutex);
		if (component->state != OMX_StateExecuting) {
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		if (inbuffer == NULL) {
			inbuffer = oscl_queue_dequeue(&inport->buffer_queue);
			if (inbuffer)
				oscl_sem_trywait(component->buf_mgnt_sem);
		}

		if (outbuffer == NULL) {
			codec_deq_timeout = 0;
			outbuffer = oscl_queue_dequeue(&outport->buffer_queue);
			if (outbuffer) {
				oscl_sem_trywait(component->buf_mgnt_sem);
				outbuffer->nFlags = 0;
				outbuffer->nFilledLen = 0;
			}
		}
		if (inbuffer == NULL && (outbuffer == NULL || codec_deq_timeout != 0)) {
			oscl_sem_wait(component->buf_mgnt_sem);
			continue;
		}

		vec_private->is_encoding_flag = 1;
		if (component->state == OMX_StateExecuting) {
			if (component->buf_handle) {
				((component->buf_handle))(component, inbuffer, outbuffer);
				inbuffer = NULL;
			}
		}

		if (outbuffer && outbuffer->nFilledLen != 0) {
			outport->return_buffer(outport, outbuffer);
			outbuffer = NULL;
			codec_deq_timeout = 0;
		} else
			codec_deq_timeout = -1;
		vec_private->is_encoding_flag = 0;
	}
	OSCL_LOGE("exit from buffer_manager: %p, %p\n", outbuffer, inbuffer);
	OSCL_TRACE(" %p\n", param);
	pthread_exit(NULL);
	return NULL;
}

OMX_ERRORTYPE video_encoder_component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
	lb_omx_component_t *component;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	venc_component_private_t *vec_private;
	int i = 0;

	component = get_lb_component(hComp);
	vec_private = (venc_component_private_t *)(component->component_private);

	if (vec_private->lib_handle || vec_private->video_encode_info)
		__unload_symb(vec_private);
	pthread_mutex_destroy(&vec_private->encode_mutex);
	pthread_mutex_destroy(&vec_private->lock);
	for (i = 0; i < (int)vec_private->wartermark.watermark_picture_num; i++) {
		if (vec_private->wartermark.watermark_pictures[i].vir_addr != NULL) {
			oscl_free_align(
				vec_private->wartermark.watermark_pictures[i].vir_addr);
			vec_private->wartermark.watermark_pictures[i].vir_addr = NULL;
		}
	}

	if (component->component_private != NULL) {
		oscl_free(component->component_private);
		component->component_private = NULL;
	}
	base_port_deinit(&component->port[INPUT_PORT]);
	base_port_deinit(&component->port[OUTPUT_PORT]);
	ret = base_component_deinit(hComp);
	return ret;
}


OMX_ERRORTYPE video_encoder_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	venc_component_private_t *vec_private = NULL;
	OMX_COMPONENTTYPE *base_cmp = &(cmp_handle->base_comp);

	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE(" base init component error\n");
		return ret;
	}

	vec_private = oscl_malloc(sizeof(venc_component_private_t));
	if (vec_private == NULL) {
		OSCL_LOGE("malloc vec_private error\n");
		ret = OMX_ErrorInsufficientResources;
		goto err_init_exit;
	}
	memset(vec_private, 0, sizeof(venc_component_private_t));
	pthread_mutex_init(&vec_private->lock, NULL);
	vec_private->lb_component = cmp_handle;
	vec_private->outbuf_sum.outbuf_mode = ENC_OUTMODE_PACKET;
	vec_private->inbuf_sum.inbuf_mode = ENC_INMODE_BYTEBUF_PHY;
	vec_private->ref_time = LLONG_MAX;

	pthread_mutex_init(&vec_private->encode_mutex, NULL);

	cmp_handle->name = "OMX.LB.VIDEO.ENCODECOMPONENT";
	cmp_handle->component_private = (OMX_PTR)vec_private;
	cmp_handle->buf_manager = video_encoder_buffer_manager;
	cmp_handle->buf_handle = video_encoder_buffer_handle;
	cmp_handle->do_state_set = video_encoder_set_state;
	cmp_handle->num_ports = 2;

	video_encode_port_init(cmp_handle, &cmp_handle->port[INPUT_PORT], INPUT_PORT,
		OMX_DirInput);
	video_encode_port_init(cmp_handle, &cmp_handle->port[OUTPUT_PORT], OUTPUT_PORT,
		OMX_DirOutput);

	base_cmp->SetConfig	      = video_encoder_set_config;
	base_cmp->SetParameter        = video_encoder_set_parameter;
	base_cmp->GetParameter        = video_encoder_get_parameter;
	base_cmp->FillThisBuffer      = video_encoder_fill_this_buffer;
	base_cmp->ComponentDeInit     = video_encoder_component_deinit;

	pthread_attr_setstacksize(&cmp_handle->buf_thread_attr, 0x2000);

	return ret;

err_init_exit:

	if (vec_private != NULL) {
		oscl_free(vec_private);
		vec_private = NULL;
	}
	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = video_encoder_component_init;
#endif

