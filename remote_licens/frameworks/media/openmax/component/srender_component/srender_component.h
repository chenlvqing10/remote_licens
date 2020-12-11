#ifndef __SRENDER_COMPONENT_H__
#define __SRENDER_COMPONENT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define INPUT_BUF_SIZE	6
#define SRENDER_INPUT_DEFAULT_SIZE (1024)

#define SRENDER_INPUT_PORT_INDEX 0
#define SRENDER_OUTPUT_PORT_INDEX 1
#define SRENDER_CLOCK_PORT_INDEX 2

extern OMX_ERRORTYPE srender_component_deinit(OMX_IN OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE srender_component_init(lb_omx_component_t *cmp_handle);

#ifdef __cplusplus
}
#endif
#endif /* __SRENDER_COMPONENT_H__ */
