#ifndef __SDECODE_COM_H__
#define __SDECODE_COM_H__

#include "audio_common.h"
#include "demuxer_plugin.h"
#include "subdec.h"
typedef enum __SUBTITLE_DEC_RESULT {
	SDEC_UNDERFLOW = -5, /* data underflow */
	SDEC_FLUSH = -4,   /* flush decode lib */
	SDEC_ERRINIT = -3, /* subtitle init error */
	SDEC_BSEND = -2,   /* subtitle bitstream decode end */
	SDEC_ERROR = -1,   /* frame failed */

	SDEC_SUCCESS = 0,   /* decode success. */

} subtitle_dec_result_t;

/* sub decode plugin */
typedef struct tag_sub_dec {
	OMX_S32 (*init)(void **avctx);
	OMX_S32 (*decode)(void *avctx, av_subtitle_t *data,
			dmx_packet_t *pkt, OMX_S32 sub_type);
	OMX_S32 (*return_frame)(void *avctx, av_subtitle_t *frame);
	OMX_S32 (*flush)(void *avctx);
	OMX_S32 (*close)(void *avctx);
} sub_dec_t;

#endif /* __SDECODE_COM_H__ */

