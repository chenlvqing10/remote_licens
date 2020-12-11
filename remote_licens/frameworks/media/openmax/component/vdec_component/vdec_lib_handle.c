#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "vdec_lib_handle"

#include <dlfcn.h>
#include <oscl.h>
#include <av_media_type.h>
#include "vdec_lib_handle.h"

typedef struct vdec_codec_lib {
	av_codec_type_e id;
	char *suffix;
} vdec_codec_lib_t;

static const vdec_codec_lib_t g_codec_lib[] = {
	{AV_CODEC_TYPE_H264, "h264"},
	{AV_CODEC_TYPE_MPEG2VIDEO, "mpeg"},
	{AV_CODEC_TYPE_MPEG1VIDEO, "mpeg"},
	/* {AV_CODEC_TYPE_MJPEG, "mjpeg"}, */
	{AV_CODEC_TYPE_VC1, "vc1"},
	{AV_CODEC_TYPE_WMV3, "vc1"},
	{AV_CODEC_TYPE_MPEG4, "mpeg4"},
	{AV_CODEC_TYPE_H263, "mpeg4"},
	{AV_CODEC_TYPE_FLV1, "mpeg4"},
	{AV_CODEC_TYPE_MSMPEG4V3, "mpeg4"},
	{AV_CODEC_TYPE_VP6F, "vp6"},
	{AV_CODEC_TYPE_RV30, "rv34"},
	{AV_CODEC_TYPE_RV40, "rv34"},
	{AV_CODEC_TYPE_INVALID, NULL}
};

void *open_vdec_lib(av_codec_type_e id, int *is_vdc)
{
	char libname[64];
	void *lib_hdl = NULL;
	char *suffix = NULL;
	int vdc;
	int i;

	for (i = 0; g_codec_lib[i].suffix != NULL; i++) {
		if (g_codec_lib[i].id == id) {
			suffix = g_codec_lib[i].suffix;
			break;
		}
	}
	if (suffix == NULL) {
		OSCL_LOGE("video codec %x not support!", id);
		return NULL;
	}

	if (vdecoder_check() == 0) {
		OSCL_LOGI("vdc");
		vdc = 1;
	} else {
		OSCL_LOGI("vdec");
		vdc = 0;
	}

	while (1) {
		libname[0] = 0;
		if (vdc)
			strcat(libname, "vd_");
		else
			strcat(libname, "vdec_");
		strcat(libname, suffix);
		strcat(libname, ".so");

		OSCL_LOGD("opening lib %s", libname);
		lib_hdl = dlopen(libname, RTLD_LAZY);
		if (lib_hdl != NULL) {
			break;
		} else {
			OSCL_LOGD("load_library %s error", libname);
			if (vdc)
				vdc = 0;
			else
				break;
		}
	}
	if (lib_hdl == NULL) {
		OSCL_LOGE("video codec %x: open %s lib error!", id, libname);
		return NULL;
	}
	*is_vdc = vdc;
	OSCL_LOGI("video codec %x: open %s success", id, libname);

	return lib_hdl;
}

void close_vdec_lib(void *lib_hdl)
{
	if (lib_hdl == NULL) {
		OSCL_LOGE("lib_hdl=NULL!");
		return;
	}
	dlclose(lib_hdl);
}

int video_codec_supported(av_codec_type_e id)
{
	 int is_vdc;
	void *hdl = open_vdec_lib(id, &is_vdc);
	if (hdl == NULL)
		return 0;
	close_vdec_lib(hdl);
	return 1;
}
