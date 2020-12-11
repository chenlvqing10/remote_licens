#define DBG_LEVEL		DBG_VERBOSE
#define LOG_TAG			"code_opts"

#include <dlfcn.h>
#include "oscl.h"
#include "venc_component.h"
#include "lombo_al_lib.h"
#include "lombo_enc_common.h"

#define VENCODER_BUFFER_SIZE (2*1024)
#define MJPEG_BUFFER_SIZE (5*1024)


static long __get_port_header_index(base_port_t *port,
	OMX_BUFFERHEADERTYPE *buf_header)
{
	int index = -1;
	unsigned int i;

	oscl_param_check(port != NULL, -1, NULL);
	oscl_param_check(buf_header != NULL, -1, NULL);
	for (i = 0; i < port->num_assigned; i++) {
		if (port->header[i] == buf_header) {
			index = i;
			break;
		}
	}
	return index;
}

static int __init_inbuf_info(venc_component_private_t *ctx,
	venc_inbuf_info_t *info, OMX_BUFFERHEADERTYPE *inbuf)
{
	int ret = 0;
	encoder_inbuf_mode_e inbuf_mode;
	venc_capbuf_t *pcap_buf;

	inbuf_mode = ctx->inbuf_sum.inbuf_mode;
	pcap_buf = &info->capbuf;
	switch (inbuf_mode) {
	case ENC_INMODE_ANW_BUF:
		ret = anw_buf_to_capbuf(ctx, inbuf, pcap_buf);
		break;
	case ENC_INMODE_NATIVE_HANDLE_BUF:
		OSCL_LOGE("to be support mode:ENC_INMODE_NATIVE_HANDLE_BUF");
		ret = -1;
		break;
	case ENC_INMODE_BYTEBUF_PHY:
		pcap_buf->src_phy_addr[0] = oscl_virt_to_phys((void *)inbuf->pBuffer);
		pcap_buf->src_vir_addr[0] = inbuf->pBuffer;
		break;
	case ENC_INMODE_BYTEBUF_VIR:
		pcap_buf->src_vir_addr[0] = oscl_malloc_align(inbuf->nAllocLen, 1024);
		if (pcap_buf->src_vir_addr[0]) {
			pcap_buf->src_phy_addr[0] =
				oscl_virt_to_phys(pcap_buf->src_vir_addr[0]);
		} else
			ret = -1;
		break;
	default:
		ret = -1;
		break;
	}

	if (ret == 0) {
		info->header = inbuf;
		info->vir_addr = pcap_buf->src_vir_addr[0];
		info->phy_addr = pcap_buf->src_phy_addr[0];
		pcap_buf->src_vir_addr[0] = info->vir_addr + ctx->cap_offsets[0];
		pcap_buf->src_vir_addr[1] = info->vir_addr + ctx->cap_offsets[1];
		pcap_buf->src_vir_addr[2] = info->vir_addr + ctx->cap_offsets[2];
		pcap_buf->src_phy_addr[0] = info->phy_addr;
		pcap_buf->src_phy_addr[1] = info->phy_addr + ctx->cap_offsets[1];
		pcap_buf->src_phy_addr[2] = info->phy_addr + ctx->cap_offsets[2];
	}
	return ret;
}

static int __add_inbuf_info(venc_component_private_t *ctx, base_port_t *port,
	OMX_BUFFERHEADERTYPE *header)
{
	int initflag = 0;
	long index;
	int inbuf_cnt;
	venc_inbuf_info_t *pinfo;

	inbuf_cnt = ctx->inbuf_sum.inbuf_cnt;
	if (ctx->inbuf_sum.inbuf_infos == NULL) {
		inbuf_cnt = port->port_param.nBufferCountActual;
		ctx->inbuf_sum.inbuf_cnt = inbuf_cnt;
		ctx->inbuf_sum.inbuf_infos =
			oscl_zalloc((sizeof(venc_inbuf_info_t)) * inbuf_cnt);
		initflag = 1;
	}
	if (ctx->inbuf_sum.inbuf_infos == NULL) {
		OSCL_LOGE("alloc inbuf_infos err(inbuf_cnt:%d)!!", inbuf_cnt);
		goto EXIT;
	}
	index = __get_port_header_index(port, header);
	if (index >= 0 && index < inbuf_cnt) {
		header->pInputPortPrivate = (void *)(index + 1);
		pinfo = &ctx->inbuf_sum.inbuf_infos[index];
		if (pinfo->header) {
			OSCL_LOGE("inbuf info:%ld reinit.(%p-%p)", index,
				ctx->inbuf_sum.inbuf_infos[index].header, header);
			memset(pinfo, 0, sizeof(venc_inbuf_info_t));
		}
		pinfo->index = index;
		__init_inbuf_info(ctx, pinfo, header);
		if (pinfo->phy_addr == 0 && initflag == 1 &&
			ctx->inbuf_sum.inbuf_mode == ENC_INMODE_BYTEBUF_PHY) {
			ctx->inbuf_sum.inbuf_mode = ENC_INMODE_BYTEBUF_VIR;
			__init_inbuf_info(ctx, pinfo, header);
		}
	}
EXIT:
	return 0;
}

static venc_inbuf_info_t *__get_inbuf_info(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *in_header)
{
	venc_inbuf_info_t *pinbuf_info = NULL;
	long index = -1;

	oscl_param_check(ctx != NULL, NULL, NULL);
	oscl_param_check(ctx->inbuf_sum.inbuf_infos != NULL, NULL, NULL);
	oscl_param_check(in_header != NULL, NULL, NULL);

	index = (long)in_header->pInputPortPrivate;
	if ((index <= 0) || (index > ctx->inbuf_sum.inbuf_cnt)) {
		OSCL_LOGE("err param. %ld-(%p-%p)",
			index, in_header->pInputPortPrivate, in_header);
		goto EXIT;
	}
	pinbuf_info = &ctx->inbuf_sum.inbuf_infos[index - 1];
	if (pinbuf_info->header != in_header) {
		OSCL_LOGE("can not find header in bufinfo. %ld-(%p-%p)",
			index, pinbuf_info->header, in_header);
		pinbuf_info = NULL;
	}

EXIT:
	return pinbuf_info;

}
static venc_inbuf_info_t *__get_inbuf_info_by_addr(venc_component_private_t *ctx,
	void *addr)
{
	int i;
	venc_inbuf_info_t *pinbuf_info = NULL;

	oscl_param_check(ctx != NULL, NULL, NULL);
	oscl_param_check(ctx->inbuf_sum.inbuf_infos != NULL, NULL, NULL);
	oscl_param_check(addr != NULL, NULL, NULL);

	for (i = 0; i < ctx->inbuf_sum.inbuf_cnt; i++) {
		if (addr == ctx->inbuf_sum.inbuf_infos[i].vir_addr) {
			pinbuf_info = &ctx->inbuf_sum.inbuf_infos[i];
			break;
		}
	}

	return pinbuf_info;

}

static int __add_outbuf_info(venc_component_private_t *ctx, base_port_t *port,
	OMX_BUFFERHEADERTYPE *header)
{
	long index;
	int outbuf_cnt;

	outbuf_cnt = ctx->outbuf_sum.outbuf_cnt;
	if (ctx->outbuf_sum.outbuf_infos == NULL) {
		outbuf_cnt = port->port_param.nBufferCountActual;
		ctx->outbuf_sum.outbuf_cnt = outbuf_cnt;
		ctx->outbuf_sum.outbuf_infos =
			oscl_zalloc((sizeof(venc_outbuf_info_t)) * outbuf_cnt);
	}
	if (ctx->outbuf_sum.outbuf_infos == NULL) {
		OSCL_LOGE("alloc outbuf_infos err(outbuf_cnt:%d)!!", outbuf_cnt);
		goto EXIT;
	}
	index = __get_port_header_index(port, header);
	if (index >= 0 && index < outbuf_cnt) {
		ctx->outbuf_sum.outbuf_infos[index].index = index;
		if (ctx->outbuf_sum.outbuf_infos[index].header) {
			OSCL_LOGE("outbuf info:%ld reinit.(%p-%p)", index,
				ctx->outbuf_sum.outbuf_infos[index].header, header);
		}
		ctx->outbuf_sum.outbuf_infos[index].header = header;
		header->pOutputPortPrivate = (void *)(index + 1);
		OSCL_LOGI("====index:%ld, header:%p,info:%p",
			index, header, &ctx->outbuf_sum.outbuf_infos[index]);
	}
EXIT:
	return 0;
}

static venc_outbuf_info_t *__get_outbuf_info(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *out_header)
{
	venc_outbuf_info_t *poutbuf_info = NULL;
	long index = -1;

	oscl_param_check(ctx != NULL, NULL, NULL);
	oscl_param_check(ctx->outbuf_sum.outbuf_infos != NULL, NULL, NULL);
	oscl_param_check(out_header != NULL, NULL, NULL);

	index = (long)out_header->pOutputPortPrivate;
	if ((index <= 0) || (index > ctx->outbuf_sum.outbuf_cnt)) {
		OSCL_LOGE("err param. %ld-(%p-%p)",
			index, out_header->pInputPortPrivate, out_header);
		goto EXIT;
	}
	poutbuf_info = &ctx->outbuf_sum.outbuf_infos[index - 1];
	if (poutbuf_info->header != out_header) {
		OSCL_LOGE("can not find header in bufinfo. %ld-(%p-%p)",
			index, poutbuf_info->header, out_header);
		poutbuf_info = NULL;
	}

EXIT:
	return poutbuf_info;

}

static int __check_encode_parm(vcodec_cfg_t *parm)
{
	if ((parm->in_width <= 0) || (parm->in_height <= 0) || (parm->l_stride <= 0)) {
		OSCL_LOGE("no input width/height\n");
		return -1;
	}

	if ((parm->out_width <= 0) || (parm->out_height <= 0)) {
		OSCL_LOGE("no ouput width/height\n");
		return -1;
	}

	if (parm->bitrate <= 0) {
		OSCL_LOGW("no bitrate, use defalut 2M bitrate\n");
		parm->bitrate = 2 * 1024 * 1024;
	}

	parm->slice_mode = 0;

	if ((parm->input_mode != ENC_YUV420P) && (parm->input_mode != ENC_YUV420SP))
		OSCL_LOGW("other input mode, need attention\n");

	if (parm->frame_rate == 0) {
		parm->frame_rate = 30;
		OSCL_LOGW("set frame_rate to 30\n");
	}
	if (parm->idr_period == 0) {
		parm->idr_period = parm->frame_rate;
		OSCL_LOGW("set idr_period to %d\n", parm->idr_period);
	}
	return 0;
}

static void __log_codec_init_para(venc_component_private_t *ctx)
{
	OSCL_LOGE("enc_parm role[%d], inputmode[%d],slice[%d-%d]",
		ctx->encode_parm.role_type,
		ctx->encode_parm.input_mode,
		ctx->encode_parm.slice_mode,
		ctx->encode_parm.slice_num);
	OSCL_LOGE("enc_parm rotation[%d], mirrordirection[%d],gop_type[%d], quality[%d]",
		ctx->encode_parm.rotation,
		ctx->encode_parm.mirrordirection,
		ctx->encode_parm.gop_type,
		ctx->encode_parm.quality);
	OSCL_LOGE("enc_parm: insize[%dx%d], outsize[%dx%d], bitrate[%d], framerate[%d]",
		ctx->encode_parm.in_width,
		ctx->encode_parm.in_height,
		ctx->encode_parm.out_width,
		ctx->encode_parm.out_height,
		ctx->encode_parm.bitrate,
		ctx->encode_parm.frame_rate);
	OSCL_LOGE("enc_parm: l_stride[%d], idr_period[%d]",
		ctx->encode_parm.l_stride,
		ctx->encode_parm.idr_period);
	OSCL_LOGE("enc_parm: rect[%d,%d,%d,%d]",
		ctx->encode_parm.rect.crop_x,
		ctx->encode_parm.rect.crop_y,
		ctx->encode_parm.rect.crop_width,
		ctx->encode_parm.rect.crop_height);
	OSCL_LOGE("plan_num %d, offsets : %d - %d - %d\n",
		ctx->cap_plan_num,
		ctx->cap_offsets[0],
		ctx->cap_offsets[1],
		ctx->cap_offsets[2]);
}

static void __return_frame(venc_component_private_t *ctx,
	unsigned char *vir_addr)
{
	base_port_t *inport;
	lb_omx_component_t *component;
	venc_inbuf_info_t *pencoded_buf_info;

	component = ctx->lb_component;
	inport = &component->port[BASE_INPUT_PORT];
	pthread_mutex_lock(&ctx->lock);
	pencoded_buf_info = __get_inbuf_info_by_addr(ctx, vir_addr);
	if (pencoded_buf_info) {
		pencoded_buf_info->state = BUF_OWN_BY_CLIENT;
		inport->return_buffer(inport, pencoded_buf_info->header);
	}
	pthread_mutex_unlock(&ctx->lock);
}

static int __update_for_croprect(venc_component_private_t *ctx)
{
	int offset[3];
	vcodec_cfg_t *pcfg;

	offset[0] = 0;
	offset[1] = 0;
	offset[2] = 0;
	pcfg = &ctx->encode_parm;
	if (pcfg->rect.crop_width != 0 && pcfg->rect.crop_height != 0) {
		offset[0] = pcfg->in_width * pcfg->rect.crop_y;
		offset[0] += pcfg->rect.crop_x;
		if (pcfg->input_mode == ENC_YUV420SP) {
			offset[1] = pcfg->in_width * pcfg->rect.crop_y / 2;
			offset[1] += pcfg->rect.crop_x;
		}
		if (pcfg->input_mode == ENC_YUV420P) {
			offset[1] = pcfg->in_width * pcfg->rect.crop_y / 4;
			offset[1] += pcfg->rect.crop_x / 2;
			offset[2] = offset[1];
		}
		pcfg->in_width = pcfg->rect.crop_width;
		pcfg->in_height = pcfg->rect.crop_height;
		ctx->cap_offsets[0] += offset[0];
		ctx->cap_offsets[1] += offset[1];
		ctx->cap_offsets[2] += offset[2];
	}
	return 0;
}

int __cfg_to_initpara(venc_parm_t *init_para, venc_component_private_t *ctx)
{
	int ret = 0;
	vcodec_cfg_t *pcfg;
	vc_init_param_t *vc_init_parm;
	vpu_init_param_t *vpu_init_parm;

	pcfg = &ctx->encode_parm;
	vc_init_parm = &init_para->vc_init_parm;
	vpu_init_parm = &init_para->vpu_init_parm;
	switch (pcfg->role_type) {
	case H264_ROLE_TYPE:
	case JPEG_ROLE_TYPE:
		__update_for_croprect(ctx);
		vc_init_parm->in_height = pcfg->in_height;
		vc_init_parm->in_width = pcfg->in_width;
		vc_init_parm->l_stride = pcfg->l_stride;
		vc_init_parm->out_height = pcfg->out_height;
		vc_init_parm->out_width = pcfg->out_width;
		vc_init_parm->bitrate = pcfg->bitrate;
		vc_init_parm->slice_mode = pcfg->slice_mode;
		vc_init_parm->input_mode = pcfg->input_mode;
		vc_init_parm->idr_period = pcfg->idr_period;
		vc_init_parm->frame_rate = pcfg->frame_rate;
		vc_init_parm->quality = pcfg->quality;
		break;
	case H265_ROLE_TYPE:
		vpu_init_parm->in_height = pcfg->in_height;
		vpu_init_parm->in_width = pcfg->in_width;
		vpu_init_parm->l_stride = pcfg->l_stride;
		vpu_init_parm->bitrate = pcfg->bitrate;
		vpu_init_parm->input_mode = pcfg->input_mode;
		vpu_init_parm->idr_period = pcfg->idr_period;
		vpu_init_parm->frame_rate = pcfg->frame_rate;
		vpu_init_parm->slice_mode = pcfg->slice_mode;
		vpu_init_parm->slice_num = pcfg->slice_num;
		vpu_init_parm->rotation = pcfg->rotation;
		vpu_init_parm->mirrordirection = pcfg->mirrordirection;
		vpu_init_parm->gop_type = pcfg->gop_type;
		vpu_init_parm->gop_type = VENC_GOPTYPE_IBBBP;
		memcpy(&vpu_init_parm->rect, &pcfg->rect, sizeof(venc_crop_t));
		break;
	default:
		ret = -1;
		break;
	}
	__log_codec_init_para(ctx);
	return ret;
}

int codec_init(venc_component_private_t *ctx)
{
	lb_omx_component_t *component = NULL;
	vcodec_cfg_t *pcfg;
	unsigned int plan_size[3];
	venc_parm_t init_param;

	int ret = 0;
	int buffer_size = VENCODER_BUFFER_SIZE;

	component = (lb_omx_component_t *)ctx->lb_component;
	pcfg = &ctx->encode_parm;
	ret = __check_encode_parm(pcfg);
	if (ret != 0) {
		OSCL_LOGE("encode parm error\n");
		ret = OMX_ErrorBadParameter;
		return ret;
	}

	if ((int)pcfg->rect.crop_width > pcfg->in_width || pcfg->rect.crop_width == 0)
		pcfg->rect.crop_width = pcfg->in_width;
	if ((int)pcfg->rect.crop_height > pcfg->in_height || pcfg->rect.crop_height == 0)
		pcfg->rect.crop_height = pcfg->in_height;

	if (ctx->cap_plan_num > 1 && ctx->cap_offsets[1] == 0) {
		memset(plan_size, 0, sizeof(plan_size));
		calc_frame_size(
			component->port[INPUT_PORT].port_param.format.video.eColorFormat,
			pcfg->in_width, pcfg->in_height,
			plan_size,
			component->port[INPUT_PORT].port_param.nBufferAlignment);
		ctx->cap_offsets[0] = 0;
		ctx->cap_offsets[1] = plan_size[0];
		ctx->cap_offsets[2] = plan_size[0] + plan_size[1];
	}

	ret = __cfg_to_initpara(&init_param, ctx);
	oscl_param_check_exit(ret == 0, -1, NULL);

	OSCL_LOGI("venc open start..");
	if (ctx->encode_parm.role_type == JPEG_ROLE_TYPE)
		buffer_size = MJPEG_BUFFER_SIZE;
	if (ctx->codec_handle == NULL)
		ctx->codec_handle = ctx->video_encode_info->open
			(&init_param, buffer_size);
	oscl_param_check_exit(ctx->codec_handle != NULL, -1, NULL);
	OSCL_LOGI("venc open end..");
EXIT:
	return ret;
}

int codec_deinit(venc_component_private_t *ctx)
{
	if (ctx->codec_handle != NULL) {
		OSCL_LOGI("venc close start..");
		ctx->video_encode_info->close(ctx->codec_handle);
		ctx->codec_handle = NULL;
		OSCL_LOGI("venc close end..");
	}

	return 0;
}


int codec_dequeue_buffer(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *out_header, int timeout)
{
	int ret = -1;
	venc_outbuf_info_t *pbuf_info;
	venc_packet_t *enc_packet;

	oscl_param_check((ctx != NULL) && out_header != NULL, -1, NULL);

	pthread_mutex_lock(&ctx->lock);
	out_header->nFlags = 0;
	out_header->nFilledLen = 0;
	pbuf_info = __get_outbuf_info(ctx, out_header);
	oscl_param_check_exit((pbuf_info != NULL), -1, NULL);

	enc_packet = &pbuf_info->enc_packt;
	memset(enc_packet, 0, sizeof(venc_packet_t));
	ret = ctx->video_encode_info->dequeue_buf(ctx->codec_handle, enc_packet, timeout);
	if (ret != 0)
		goto EXIT;
	if (enc_packet->vir_addr[0] == NULL)
		goto EXIT;

	/* the picture type of the frame */
	if (enc_packet->pic_type == VENC_I_FRAME)
		out_header->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
	if (enc_packet->pic_type == VENC_INIT_PACKET)
		out_header->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
	if (enc_packet->frame_end_flag)
		out_header->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
	out_header->nTimeStamp = enc_packet->time_stamp;
	if (ctx->outbuf_sum.outbuf_mode == ENC_OUTMODE_PACKET)
		outbuf_to_muxpack(ctx, pbuf_info);
	else {
		out_header->nFilledLen = enc_packet->size[0];
		if (out_header->nFilledLen > out_header->nAllocLen) {
			OSCL_LOGE("paket size > outbuffer size. bug to be fixed");
			out_header->nFilledLen = out_header->nAllocLen;
		}
		memcpy(out_header->pBuffer, enc_packet->vir_addr[0],
			out_header->nFilledLen);
		out_header->nFilledLen += enc_packet->size[1];
		if (out_header->nFilledLen > out_header->nAllocLen) {
			out_header->nFilledLen -= enc_packet->size[1];
		} else {
			memcpy(out_header->pBuffer + enc_packet->size[0],
				enc_packet->vir_addr[1], enc_packet->size[1]);
		}
		OSCL_LOGI("====packet:%p, vir_addr:%p(%p) size:%d", enc_packet,
			out_header->pBuffer, enc_packet->vir_addr[0],
			out_header->nFilledLen);
		save_raw_stream(ctx->lb_component, OUTPUT_PORT,
			out_header->pBuffer, out_header->nFilledLen);
	}
	if (pbuf_info->state != BUF_OWN_BY_ENCODER)
		OSCL_LOGE("warn: buffer is not owned by encoder.(%d)", pbuf_info->state);
	pbuf_info->state = BUF_OWN_BY_CLIENT;
EXIT:
	pthread_mutex_unlock(&ctx->lock);
	return ret;
}

int codec_queue_buffer(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *out_header)
{
	int ret = -1;
	venc_packet_t *packet;
	lb_omx_component_t *component;
	venc_outbuf_info_t *pbuf_info;

	oscl_param_check((ctx != NULL) && out_header != NULL, -1, NULL);
	component = (lb_omx_component_t *)ctx->lb_component;

	if (out_header->pOutputPortPrivate == NULL) {
		pthread_mutex_lock(&ctx->lock);
		__add_outbuf_info(ctx,
			&component->port[out_header->nOutputPortIndex], out_header);
		pthread_mutex_unlock(&ctx->lock);
	}
	pthread_mutex_lock(&ctx->lock);
	pbuf_info = __get_outbuf_info(ctx, out_header);
	oscl_param_check_exit((pbuf_info != NULL), -1, NULL);

	if (pbuf_info->state != BUF_OWN_BY_CLIENT && pbuf_info->state != BUF_NOT_INITED)
		OSCL_LOGE("warn: buffer is not owned by comp.(%d)", pbuf_info->state);

	if (pbuf_info->index == ctx->outbuf_sum.tmpbuf_index)
		ctx->outbuf_sum.tmpbuf_index = 0;

	packet = &pbuf_info->enc_packt;
	if (packet->vir_addr[0] != NULL) {
		pbuf_info->state = BUF_OWN_BY_ENCODER;
		ret = ctx->video_encode_info->queue_buf(ctx->codec_handle, packet);
		if (ret)
			OSCL_LOGE("return packet to venc codec failed!");
	} else if (pbuf_info->state == BUF_NOT_INITED)
		pbuf_info->state = BUF_OWN_BY_ENCODER;
	else {
		pbuf_info->state = BUF_OWN_BY_COMP;
		OSCL_LOGE("return packet with a null addr!");
	}
	memset(packet, 0, sizeof(venc_packet_t));
	ret = 0;
EXIT:
	pthread_mutex_unlock(&ctx->lock);
	return ret;
}

int codec_encode_frame(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *inbuf)
{
	venc_capbuf_t *pcap_buf;
	int ret = 0;
	lb_omx_component_t *component;
	venc_inbuf_info_t *pbuf_info;
	venc_encoded_frame_t encoded_frame;

	oscl_param_check((ctx != NULL) && inbuf != NULL, -1, NULL);
	component = (lb_omx_component_t *)ctx->lb_component;

	save_yuv_stream(ctx->lb_component, inbuf->nInputPortIndex,
		inbuf->pBuffer, inbuf->nFilledLen);
	pthread_mutex_lock(&ctx->lock);
	if (inbuf->pInputPortPrivate == NULL) {
		__add_inbuf_info(ctx,
			&component->port[inbuf->nInputPortIndex], inbuf);
	}
	pbuf_info = __get_inbuf_info(ctx, inbuf);
	pthread_mutex_unlock(&ctx->lock);
	oscl_param_check_exit((pbuf_info != NULL), -1, NULL);
	pbuf_info->state = BUF_OWN_BY_COMP;

	pcap_buf = &pbuf_info->capbuf;
	if (pcap_buf->src_phy_addr[0] != 0) {
		if (ctx->inbuf_sum.inbuf_mode == ENC_INMODE_BYTEBUF_VIR)
			memcpy(pbuf_info->vir_addr, inbuf->pBuffer, inbuf->nFilledLen);
		pcap_buf->time_stamp = inbuf->nTimeStamp;
		encoded_frame.vir_addr[0] = NULL;
		encoded_frame.phy_addr[0] = 0;
		pbuf_info->state = BUF_OWN_BY_ENCODER;
		pthread_mutex_lock(&ctx->encode_mutex);
		ret = ctx->video_encode_info->encode_frame(
				ctx->codec_handle, pcap_buf, &encoded_frame);
		pthread_mutex_unlock(&ctx->encode_mutex);
		if (ret != 0)
			OSCL_LOGE("h264_enc_frame error\n");
		__return_frame(ctx, encoded_frame.vir_addr[0]);
	}
EXIT:
	return ret;
}

