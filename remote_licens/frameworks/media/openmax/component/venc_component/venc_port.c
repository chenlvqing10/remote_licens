#define LOG_TAG	"venc_port"

#include "venc_component.h"
#include "lombo_encplugin.h"

#define STREAM_BUF_NUM 5
#define STREAM_BUF_SIZE (800*1024)
#define STREAM_BUF_ALIGN (64)

OMX_ERRORTYPE encoder_inport_flush_buffer(base_port_t *inport)
{
	int i;
	lb_omx_component_t *component;
	venc_component_private_t *ctx;

	base_port_flush_buffer(inport);

	component = (lb_omx_component_t *)inport->component;
	ctx = (venc_component_private_t *)component->component_private;
	pthread_mutex_lock(&ctx->lock);
	for (i = 0; i < ctx->inbuf_sum.inbuf_cnt; i++) {
		if (ctx && ctx->inbuf_sum.inbuf_infos &&
			(ctx->inbuf_sum.inbuf_infos[i].state == BUF_OWN_BY_COMP ||
			ctx->inbuf_sum.inbuf_infos[i].state == BUF_OWN_BY_ENCODER)) {
				ctx->inbuf_sum.inbuf_infos[i].state = BUF_OWN_BY_CLIENT;
				if (ctx->inbuf_sum.inbuf_infos[i].header == NULL)
					continue;
				inport->return_buffer(inport,
					ctx->inbuf_sum.inbuf_infos[i].header);
				OSCL_LOGE("%d", ctx->inbuf_sum.inbuf_infos[i].index);
		}
	}
	pthread_mutex_unlock(&ctx->lock);
	return 0;
}

OMX_ERRORTYPE encoder_inport_recive_buffer(base_port_t *port,
	OMX_BUFFERHEADERTYPE *buffer)
{
	lb_omx_component_t *component;
	venc_component_private_t *vec_private;

	OSCL_TRACE("%p(%s, %d)", port, get_port_name(port), port->port_param.nPortIndex);
	component = (lb_omx_component_t *)port->component;
	vec_private = (venc_component_private_t *)component->component_private;

	if (buffer->nTimeStamp < vec_private->ref_time) {
		OSCL_LOGI("port:%d drop frame, time:%lld:%lld", buffer->nInputPortIndex,
			  buffer->nTimeStamp, vec_private->ref_time);
		return -1;
	}

	return base_port_recive_buffer(port, buffer);
}


OMX_ERRORTYPE video_encode_port_init(lb_omx_component_t *component,
	base_port_t *base_port,
	OMX_U32 index,
	OMX_U32 dir_type)
{
	OMX_PARAM_PORTDEFINITIONTYPE *pparam;
	venc_component_private_t *ctx;

	OSCL_TRACE(" %p %p %d %d\n", component, base_port, index, dir_type);

	oscl_param_check((component != NULL) && (base_port != NULL),
		OMX_ErrorBadParameter, NULL);
	ctx = (venc_component_private_t *)(component->component_private);

	base_port_init(component, base_port, index, dir_type);

	pparam = &base_port->port_param;
	pparam->eDomain = OMX_PortDomainVideo;
	if (dir_type == OMX_DirOutput) {
		pparam->nBufferCountActual = STREAM_BUF_NUM;
		if (ctx->outbuf_sum.outbuf_mode == ENC_OUTMODE_PACKET)
			pparam->nBufferSize = MUXER_PACKET_SIZE;
		else
			pparam->nBufferSize = STREAM_BUF_SIZE;
		pparam->format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
		pparam->format.video.xFramerate = 30;
	} else {
		pparam->format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
		pparam->format.video.xFramerate = 30;
		base_port->recive_buffer = encoder_inport_recive_buffer;
		base_port->flush_buffer = encoder_inport_flush_buffer;
	}
	return OMX_ErrorNone;
}

