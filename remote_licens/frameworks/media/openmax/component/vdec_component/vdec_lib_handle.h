#ifndef __VDEC_LIB_HANDLE_H__
#define __VDEC_LIB_HANDLE_H__

#include <av_media_type.h>

void *open_vdec_lib(av_codec_type_e id, int *is_vdc);
void close_vdec_lib(void *lib_hdl);
int video_codec_supported(av_codec_type_e id);

static inline int vdecoder_check(void)
{
	return access("/dev/vdc", F_OK);
}

#endif /* __VDEC_LIB_HANDLE_H__ */
