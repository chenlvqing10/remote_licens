#ifndef __SDEC_COMMON_H__
#define __SDEC_COMMON_H__

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "oscl.h"
#include "sdecode_com.h"
#include <omx_vendor_lb.h>

#define SDEC_RAW_BUFFER_SIZE (128 * 1024)

typedef struct sdec_private {
	void *decpara;
	sub_dec_t psub;
	AV_CodecParameters *para;
	void *dl_hdl;
	omx_sub_info_t info;
	OMX_BOOL open_error;
	void *extradata;
	OMX_S32 extrasize;
} sdec_private_t;

sdec_private_t *sdec_init(void);
OMX_S32 sdec_deinit(sdec_private_t *sub_private);
OMX_S32 sdec_frame(sdec_private_t *sub_private,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf);
OMX_S32 sdec_return_frame(sdec_private_t *sub_private, av_subtitle_t *frame);
OMX_S32 sdec_open(sdec_private_t *sub_private);
OMX_S32 sdec_close(sdec_private_t *sub_private);
OMX_S32 sdec_flush(sdec_private_t *sub_private);

#endif /* __SDEC_COMMON_H__ */
