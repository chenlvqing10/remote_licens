/*
 * camera_record.c
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <stdio.h>
#include <log/log.h>

#include "camera_record.h"
#include "file_ops.h"
#include "video_recorder.h"
#include "face_svc_media.h"
#include "face.h"

#define VIDEO_NAME_FORMAT	"/data/%s_%d%02d%02d_%02d%02d%02d.mp4"

typedef struct {
	void *camera_record_rgb_hdl;
	char camera_record_rgb_file[128];
	void *camera_record_nir_hdl;
	char camera_record_nir_file[128];
} CAMERA_RECORD_INFO;

static CAMERA_RECORD_INFO camera_record_info;

static void generate_filename(char *buf, size_t buflen, const char *format,
		const char *prefix)
{
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	snprintf(buf, buflen, format, prefix, p->tm_year + 1900, p->tm_mon + 1,
			p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

int camera_record_start(void)
{
	return camera_record_start_by_params(VIDEO_ENCODER_H264,
			REC_OUTPUT_FORMAT_MP4, 1);
}

int camera_record_start_by_params(int video_enc_format, int output_format,
		int nir_enable)
{
	void *rgb_hdl = face_rgb_hdl(get_face_handle());
	void *nir_hdl = face_nir_hdl(get_face_handle());
	int ret = 0;
	face_camera_cfg_t rgb_cfg;
	face_camera_cfg_t nir_cfg;

	ALOGD("camera_record_start");
	if (rgb_hdl) {
		if (!camera_record_info.camera_record_rgb_hdl) {
			camera_record_info.camera_record_rgb_hdl = video_rec_creat();
			enc_para_t para;

			ret = video_rec_set_video_stream(
					camera_record_info.camera_record_rgb_hdl, rgb_hdl);

			if (ret < 0) {
				ALOGE("video_rec_set_video_stream faild\n");
				goto end;
			}

			ret = face_get_camera_config(get_face_handle(),
				FACE_CAMERA_RGB,&rgb_cfg);
			if (ret < 0) {
				ALOGE("rgb face_get_camera_config faild\n");
				goto end;
			}

			para.width = rgb_cfg.width;
			para.height = rgb_cfg.height;
			para.bitrate = 4 * 1000 * 1000;
			para.fmt = output_format;
			para.rotate = 0;
			para.slice_mode = 0;

			ret = video_rec_set_para(camera_record_info.camera_record_rgb_hdl,
					&para);
			if (ret < 0) {
				ALOGE("video_rec_set_para faild\n");
				goto end;
			}
		}

		generate_filename(camera_record_info.camera_record_rgb_file, 128,
		VIDEO_NAME_FORMAT, "rgb");
		ret = video_rec_set_output_file(
				camera_record_info.camera_record_rgb_hdl,
				camera_record_info.camera_record_rgb_file);
		if (ret < 0) {
			ALOGE("video_rec_set_video_stream faild\n");
			goto end;
		}

		ret = video_rec_start(camera_record_info.camera_record_rgb_hdl);
		if (ret < 0) {
			ALOGE("video_rec_start error\n");
			goto end;
		}
	}

	if (nir_hdl && nir_enable) {
		if (!camera_record_info.camera_record_nir_hdl) {
			camera_record_info.camera_record_nir_hdl = video_rec_creat();
			enc_para_t para;

			ret = video_rec_set_video_stream(
					camera_record_info.camera_record_nir_hdl, nir_hdl);

			if (ret < 0) {
				ALOGE("video_rec_set_video_stream faild\n");
				goto end;
			}

			ret = face_get_camera_config(get_face_handle(),
				FACE_CAMERA_NIR, &nir_cfg);
			if (ret < 0) {
				ALOGE("nir face_get_camera_config faild\n");
				goto end;
			}

			para.width = nir_cfg.width;
			para.height = nir_cfg.height;
			para.bitrate = 4 * 1000 * 1000;
			para.fmt = output_format;
			para.rotate = 0;
			para.slice_mode = 0;

			ret = video_rec_set_para(camera_record_info.camera_record_nir_hdl,
					&para);
			if (ret < 0) {
				ALOGE("video_rec_set_para faild\n");
				goto end;
			}
		}

		generate_filename(camera_record_info.camera_record_nir_file, 128,
		VIDEO_NAME_FORMAT, "nir");
		ret = video_rec_set_output_file(
				camera_record_info.camera_record_nir_hdl,
				camera_record_info.camera_record_nir_file);
		if (ret < 0) {
			ALOGE("video_rec_set_video_stream faild\n");
			goto end;
		}

		ret = video_rec_start(camera_record_info.camera_record_nir_hdl);
		if (ret < 0) {
			ALOGE("video_rec_start error\n");
			goto end;
		}
	}

	end: return ret;
}

int camera_record_stop(void)
{
	if (camera_record_info.camera_record_rgb_hdl) {
		if (video_rec_stop(camera_record_info.camera_record_rgb_hdl))
			return -1;
	}

	if (camera_record_info.camera_record_nir_hdl) {
		if (video_rec_stop(camera_record_info.camera_record_nir_hdl))
			return -1;
	}

	return 0;
}

int camera_record_remove_files(void)
{
	file_remove(camera_record_info.camera_record_rgb_file);

	file_remove(camera_record_info.camera_record_nir_file);

	return 0;
}

int camera_record_get_rgb_status()
{
	int status = 0;

	if (camera_record_info.camera_record_rgb_hdl)
		video_rec_get_status(camera_record_info.camera_record_rgb_hdl, &status);

	return status;
}

int camera_record_get_nir_status()
{
	int status = 0;

	if (camera_record_info.camera_record_nir_hdl)
		video_rec_get_status(camera_record_info.camera_record_nir_hdl, &status);

	return status;
}

char *camera_record_get_rgb_file_path()
{
	return camera_record_info.camera_record_rgb_file;
}

char *camera_record_get_nir_file_path()
{
	return camera_record_info.camera_record_nir_file;
}
