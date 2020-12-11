#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "demuxer_port"

#include <oscl.h>
#include <base_component.h>
#include "demuxer_component.h"

OMX_ERRORTYPE demuxer_port_allocate_buf(base_port_t *port,
	OMX_BUFFERHEADERTYPE **buf_header,
	OMX_PTR app_private,
	OMX_U32 size)
{
	int ret = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE *tmp_header;

	ret = base_port_allocate_buf(port, buf_header, app_private, size);
	if (ret == OMX_ErrorNone) {
		tmp_header = *buf_header;
		if (tmp_header->pBuffer)
			memset(tmp_header->pBuffer, 0, tmp_header->nAllocLen);
	}

	return ret;
}

OMX_ERRORTYPE demuxer_outport_flush_buffer(base_port_t *port)
{
	OMX_BUFFERHEADERTYPE *buf_head = NULL;

	OSCL_TRACE("%p(%s, %d)", port, get_port_name(port), port->port_param.nPortIndex);
	OSCL_LOGD("%s flush buffer:%p, port:%d\n", get_port_name(port), buf_head,
		port->port_param.nPortIndex);
	if (PORT_IS_SUPPLIER(port)) {
		while (oscl_queue_get_num(&port->buffer_queue) != port->num_assigned) {
			OSCL_LOGD("queue num:%d assigned:%d",
				oscl_queue_get_num(&port->buffer_queue),
				port->num_assigned);
			sem_wait(&port->buffer_sem);
		}
	} else {
		do {
			buf_head = oscl_queue_dequeue(&port->buffer_queue);
			OSCL_LOGD("(%s)flush buffer:%p\n", get_port_name(port), buf_head);
			if (buf_head != NULL)
				port->return_buffer(port, buf_head);
		} while (buf_head != NULL);
		OSCL_LOGD(" %p", port);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE demuxer_port_recive_buffer(base_port_t *port,
	OMX_BUFFERHEADERTYPE *buffer)
{
	OMX_U32 port_idx;
	lb_omx_component_t *component = (lb_omx_component_t *)port->component;

	OSCL_TRACE("%p:%p", port, buffer);
	port_idx = (port->port_param.eDir == OMX_DirInput) ? buffer->nInputPortIndex :
		buffer->nOutputPortIndex;
	oscl_param_check((port_idx == port->port_param.nPortIndex),
		OMX_ErrorBadPortIndex, NULL);
	oscl_param_check((OMX_StateInvalid != get_component_state(port)),
		OMX_ErrorInvalidState, NULL);
	if (!PORT_IS_SUPPLIER(port) && port->is_flushed) {
		OSCL_LOGW("recive buffers while flush port(none supplier)\n");
		OSCL_LOGW("%p(%s, %d)", port,
			get_port_name(port), port->port_param.nPortIndex);
	}

	/* notify the buffer management thread we have a fresh new buffer to manage*/
	oscl_queue_queue(&port->buffer_queue, buffer);
	if (PORT_IS_SUPPLIER(port) && (port->is_flushed))
		sem_post(&port->buffer_sem);
	sem_post(component->buf_mgnt_sem);
	OSCL_TRACE("%p:%p", port, buffer);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE demuxer_port_init(lb_omx_component_t *component,
	base_port_t *base_port,
	OMX_U32 index,
	OMX_U32 dir_type)
{
	OSCL_TRACE(" %p %p %d %d\n", component, base_port, index, dir_type);

	oscl_param_check((component != NULL) && (base_port != NULL),
		OMX_ErrorBadParameter, NULL);

	base_port_init(component, base_port, index, dir_type);

	base_port->flush_buffer = demuxer_outport_flush_buffer;
	base_port->allocate_buffer = demuxer_port_allocate_buf;
	base_port->recive_buffer = demuxer_port_recive_buffer;

	return OMX_ErrorNone;
}

