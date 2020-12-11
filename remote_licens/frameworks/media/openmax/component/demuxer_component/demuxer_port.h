#ifndef __DEMUXER_PORT_H__
#define __DEMUXER_PORT_H__

OMX_ERRORTYPE demuxer_port_init(lb_omx_component_t *component,
	base_port_t *base_port,
	OMX_U32 index,
	OMX_U32 dir_type);

#endif /* __DEMUXER_PORT_H__ */

