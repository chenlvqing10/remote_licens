/*
 * adec_lib_handle.c - demo code for adec.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "adec_lib_handle"

#include <dlfcn.h>
#include <oscl.h>

#include "av_media_type.h"

typedef struct adec_sux_t {
	char *suffix;
	av_codec_type_e id;
} adec_sux_t;

static const  adec_sux_t adec_uri[] = {
	/* audio probe */
	{"aac", AV_CODEC_TYPE_AAC},
	{"aac", AV_CODEC_TYPE_AAC_LATM},
	{"amr", AV_CODEC_TYPE_AMR_NB},
	{"amr", AV_CODEC_TYPE_AMR_WB},
	{"ape", AV_CODEC_TYPE_APE},
	{"ac3", AV_CODEC_TYPE_AC3},
	{"ac3", AV_CODEC_TYPE_EAC3},
	{"ac3", AV_CODEC_TYPE_TRUEHD},
	{"atrac", AV_CODEC_TYPE_ATRAC1},
	{"atrac", AV_CODEC_TYPE_ATRAC3},
	{"atrac", AV_CODEC_TYPE_ATRAC3P},
	{"cook", AV_CODEC_TYPE_COOK},
	{"dts", AV_CODEC_TYPE_DTS},
	{"dts", AV_CODEC_TYPE_DTSHD},
	{"flac", AV_CODEC_TYPE_FLAC},
	{"g729", AV_CODEC_TYPE_G729},
	{"mp3", AV_CODEC_TYPE_MP3},
	{"vorbis", AV_CODEC_TYPE_VORBIS},
	{"mp3", AV_CODEC_TYPE_MP2},
	{"mp3", AV_CODEC_TYPE_MP1},
	{"ra144", AV_CODEC_TYPE_RA_144},
	{"ra288", AV_CODEC_TYPE_RA_288},
	{"sipr", AV_CODEC_TYPE_SIPR},
	{"wav", AV_CODEC_TYPE_PCM_S16LE},
	{"wav", AV_CODEC_TYPE_PCM_S16BE},
	{"wav", AV_CODEC_TYPE_PCM_U16LE},
	{"wav", AV_CODEC_TYPE_PCM_U16BE},
	{"wav", AV_CODEC_TYPE_PCM_S8},
	{"wav", AV_CODEC_TYPE_PCM_U8},
	{"wav", AV_CODEC_TYPE_PCM_MULAW},
	{"wav", AV_CODEC_TYPE_PCM_ALAW},
	{"wav", AV_CODEC_TYPE_PCM_S32LE},
	{"wav", AV_CODEC_TYPE_PCM_S32BE},
	{"wav", AV_CODEC_TYPE_PCM_U32LE},
	{"wav", AV_CODEC_TYPE_PCM_U32BE},
	{"wav", AV_CODEC_TYPE_PCM_S24LE},
	{"wav", AV_CODEC_TYPE_PCM_S24BE},
	{"wav", AV_CODEC_TYPE_PCM_U24LE},
	{"wav", AV_CODEC_TYPE_PCM_U24BE},
	{"wav", AV_CODEC_TYPE_PCM_F64LE},
	{"wav", AV_CODEC_TYPE_PCM_F32LE},
	{"wav", AV_CODEC_TYPE_PCM_BLURAY},
	{"wav", AV_CODEC_TYPE_PCM_DVD},
	{"wav", AV_CODEC_TYPE_ADPCM_IMA_WAV},
	{"wma", AV_CODEC_TYPE_WMAV1},
	{"wma", AV_CODEC_TYPE_WMAV2},
	{"wma", AV_CODEC_TYPE_WMAPRO},
	{"wma", AV_CODEC_TYPE_WMALOSSLESS},
	{NULL, AV_CODEC_TYPE_INVALID}
};

void *open_adec_lib(av_codec_type_e id)
{
	char libname[64] = "lib";
	void *dl_hdl = NULL;
	int i = 0;

	while (adec_uri[i].id != AV_CODEC_TYPE_INVALID) {
		if (adec_uri[i].id == id)
			break;
		i++;
	}
	if (adec_uri[i].id == AV_CODEC_TYPE_INVALID) {
		OSCL_LOGE("audio codec %x not support.", id);
		return NULL;
	}
	strcat(libname, adec_uri[i].suffix);
	strcat(libname, "dec.so");

	OSCL_LOGD("opening lib %s", libname);
	dl_hdl = dlopen(libname, RTLD_LAZY);
	if (dl_hdl == NULL) {
		OSCL_LOGE("audio codec %x: open %s lib error(%s)!",
			id, adec_uri[i].suffix, dlerror());
		return NULL;
	}
	OSCL_LOGI("audio codec %x: open %s lib success", id, adec_uri[i].suffix);
	return dl_hdl;
}

void close_adec_lib(void *dl_hdl)
{
	if (dl_hdl == NULL) {
		OSCL_LOGE("lib_hdl=NULL!");
		return;
	}
	dlclose(dl_hdl);
}

int audio_codec_supported(av_codec_type_e id)
{
	void *hdl = open_adec_lib(id);
	if (hdl == NULL)
		return 0;
	close_adec_lib(hdl);
	return 1;
}
