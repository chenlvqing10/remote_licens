/*
 * person_register.c - Define interface for personnel registration..
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

#include "person_register.h"

#include <stdio.h>
#include <string.h>
#include <log/log.h>
#include <dirent.h>
#include <stdlib.h>

#include "face_svc.h"
#include "face.h"
#include "lombo_malloc.h"
#include "jpeg_enc.h"
#include "person.h"
#include "cut.h"
#include "time_conv.h"
#include "file_ops.h"
#include "elang.h"
#include "config.h"
#include "db_person_info.h"

typedef struct tag_management_reg {
	char large_pic_path[128];
	char small_pic_path[128];
	person_t person_info;
	face_frame_t *face_frame;
	int graph_reg_flag;
	int reg_running;
	pthread_mutex_t mutex_lock;
	pthread_cond_t cond;
} management_reg_t;

management_reg_t *reg_handle;

#define MEDIA_SUCCESS 0
#define ETYPE_MALLOC_FAILED -1
#define ETYPE_NOT_DET_FACE -2
#define ETYPE_POINTER_IS_NULL -3
#define ETYPE_JPG_ENC_FAILED -4
#define ETYPE_JPG_CUTOUT_FAILED -5
#define ETYPE_FACE_DET_FAILED -6
#define ETYPE_WAIT_TIMEROUT -7
#define ETYPE_CB_IS_NULL -8
#define ETYPE_JPG_DEC_FAILED -9
#define ETYPE_DET_MUTIPLE_FACE -10
#define ETYPE_DET_FACE_QUALITY_LOW -11
#define ETYPE_DET_FACE_SIMULATE	-12

#define PIC_PATH "/data/recg_tmp"
#define JPG_INFO_SIZE	(8*1024)
#define JPG_OUT_SIZE	(4*1024*1024)

#define PRESET_FACE_PICTURE_WIDTH   250
#define PRESET_FACE_PICTURE_HEIGHT  300

static e_person_reg_code result_code;

/**
 * return frame buffer of camera
 * @param NULL
 * @return
 */
static int app_management_reg_return_buff(face_frame_t *frame)
{
	int ret = 0;
	void *svc_handle = get_face_handle();
	ALOGD("%s: frame: %p; svc_handle: %p", __func__, frame, svc_handle);
	if (frame) {
		ret = face_return_frame(svc_handle, frame);
		if (ret) {
			ALOGE("%s: face return failed\n", __func__);
			frame = NULL;
			return -1;
		}
		frame = NULL;
	}
	return ret;
}

/**
 * compress small picture of jpg farmat
 * @param NULL
 * @return 0-success,other-failed
 */
static int app_management_reg_big_jpg_enc(management_reg_t *p_reg_handle)
{
	int ret = 0;
	time_t t = 0;
	/*the struct of large jpg graph infomation*/
	struct output_image_info large_jpg_img_info;
	/*the struct of yuv graph infomation*/
	struct input_image_info large_yuv_img_info;
	struct cut_input tmp_input;
	struct cut_output tmp_output;
	person_reg_param_t reg_param;
	DIR *dir;

	/*init parameters*/
	memset(&large_yuv_img_info, 0x00, sizeof(large_yuv_img_info));
	memset(&large_jpg_img_info, 0x00, sizeof(large_jpg_img_info));
	memset(&tmp_input, 0x00, sizeof(tmp_input));
	memset(&tmp_output, 0x00, sizeof(tmp_output));
	memset(&reg_param, 0x00, sizeof(person_reg_param_t));

	app_management_reg_get_config(&reg_param);

	if ((NULL == p_reg_handle->face_frame) || (NULL == p_reg_handle)) {
		ALOGE("%s: yuv data pointer is NULL %p--%p\n", __func__,
			p_reg_handle->face_frame, p_reg_handle);
		app_management_reg_return_buff(p_reg_handle->face_frame);
		p_reg_handle->face_frame = NULL;
		return ETYPE_POINTER_IS_NULL;
	}

	/*init cut image info, camera size 800*1408, vedio preview size 800*1024*/
	tmp_input.addr[0] = p_reg_handle->face_frame->viraddr[0];
	tmp_input.addr[1] = p_reg_handle->face_frame->viraddr[1];
	tmp_input.size[0] = p_reg_handle->face_frame->size[0];
	tmp_input.size[1] = p_reg_handle->face_frame->size[1];
	tmp_input.src_width = reg_param.jpeg_width_stride;
	tmp_input.src_height = reg_param.jpeg_height_stride;
	tmp_input.format = IMG_COLOR_NV12;
	tmp_input.dest_x = 0;
	tmp_input.dest_y = 0;
	tmp_input.dest_width = reg_param.jpeg_width_stride;
	tmp_input.dest_height = reg_param.jpeg_height_stride;
	/*call function to cutout*/
	ret = yuv_cut_start(&tmp_input, &tmp_output);
	if (ret) {
		ALOGE("%s: jpg cut failed\n", __func__);
		ret = ETYPE_JPG_CUTOUT_FAILED;
		goto error_exit;
	}
	ALOGD("tmp_output.width = %d\n", tmp_output.width);
	ALOGD("tmp_output.height = %d\n", tmp_output.height);
	ALOGD("tmp_output.format = %d\n", tmp_output.format);
	ALOGD("tmp_output.addr[0] = %p\n", tmp_output.addr[0]);
	ALOGD("tmp_output.addr[1] = %p\n", tmp_output.addr[1]);
	ALOGD("tmp_output.size[0] = %d\n", tmp_output.size[0]);
	ALOGD("tmp_output.size[1] = %d\n", tmp_output.size[1]);

#if 0
	large_yuv_img_info.buf = p_reg_handle->face_frame->viraddr;
	large_yuv_img_info.width = p_reg_handle->face_frame->width;
	large_yuv_img_info.height = p_reg_handle->face_frame->height;
#else
	large_yuv_img_info.addr[0] = tmp_output.addr[0];
	large_yuv_img_info.addr[1] = tmp_output.addr[1];
	large_yuv_img_info.size[0] = tmp_output.size[0];
	large_yuv_img_info.size[1] = tmp_output.size[1];
	large_yuv_img_info.width = tmp_output.width;
	large_yuv_img_info.height = tmp_output.height;
#endif
	large_yuv_img_info.format = IMG_COLOR_NV12;
	large_yuv_img_info.qualify = 99;
	if (large_yuv_img_info.jpeg_info_buf == NULL)
		large_yuv_img_info.jpeg_info_buf = (unsigned char *)malloc(JPG_INFO_SIZE);
	if (large_yuv_img_info.jpeg_out_buf == NULL)
		large_yuv_img_info.jpeg_out_buf = (unsigned char *)malloc(JPG_OUT_SIZE);
	ALOGD("big jpg malloc: %p, %p\n", large_yuv_img_info.jpeg_info_buf,
		large_yuv_img_info.jpeg_out_buf);
	ret = jpeg_enc_buffer(&large_yuv_img_info, &large_jpg_img_info);
	if (ret) {
		ALOGE("%s: big jpg enc failed\n", __func__);
		yuv_cut_end(&tmp_output);
		ret = ETYPE_JPG_ENC_FAILED;
		goto error_exit;
	}
	yuv_cut_end(&tmp_output);
	/*get time stamp*/
	time_get_current_sec(&t);

	/*organize path*/
	memset(p_reg_handle->large_pic_path, 0, 128);
	snprintf(p_reg_handle->large_pic_path,
		sizeof(p_reg_handle->large_pic_path),
		"%s/%ld-large%s", PIC_PATH, t, ".jpg");
	ALOGD("the big picture addr is: %s\n", p_reg_handle->large_pic_path);

	/*whether the dir is exist, if not the create*/
	dir = opendir(p_reg_handle->large_pic_path);
	if (dir == NULL) {
		ret = mk_mul_dirs(PIC_PATH);

		if (ret != 0) {
			ALOGE("Create multiple folder: %s fail\n",
				p_reg_handle->large_pic_path);
			goto error_exit;
		}
	} else
		closedir(dir);

	ALOGD("the big picture size is: %d\n", large_jpg_img_info.size);
	/* write file to tmp folder */
	ret = file_write(p_reg_handle->large_pic_path,
		(char *)large_jpg_img_info.buf,
		large_jpg_img_info.size);
	if (ret) {
		ALOGE("%s: write file failed\n", __func__);
		goto error_exit;
	}

	/*free memery*/
	if (large_yuv_img_info.jpeg_info_buf)
		free(large_yuv_img_info.jpeg_info_buf);
	if (large_yuv_img_info.jpeg_out_buf)
		free(large_yuv_img_info.jpeg_out_buf);

	return ret;

error_exit:
	/*return buff*/
	app_management_reg_return_buff(p_reg_handle->face_frame);
	p_reg_handle->face_frame = NULL;
	/*free memery*/
	if (large_yuv_img_info.jpeg_info_buf)
		free(large_yuv_img_info.jpeg_info_buf);
	if (large_yuv_img_info.jpeg_out_buf)
		free(large_yuv_img_info.jpeg_out_buf);

	return ret;
}

/**
 * enlarge face box coordinate
 * @param p_in_box_info the pointer of the src coordinate
 * @param p_out_box_info the pointer of the dest coordinate
 * @param w the preset width of picture
 * @param h the preset height of picture
 * @param screen_w the width of screen
 * @param screen_h the height of screen
 * @return 0-success,other-failed
 */
static int app_management_reg_face_box_cor_enlarge(face_rect_t *p_in_box_info,
	face_rect_t *p_out_box_info, int w, int h, int screen_w, int screen_h,
	management_reg_t *p_reg_handle)
{
	int tmp_w = 0;
	int tmp_h = 0;
	int diff_w = 0;
	int diff_h = 0;
	if ((NULL == p_in_box_info) || (NULL == p_out_box_info) ||
		(NULL == p_reg_handle)) {

		printf("%s: parameter is NULL", __func__);
		return ETYPE_POINTER_IS_NULL;
	}

	/*fix value*/
	p_out_box_info->score = p_in_box_info->score;
	p_out_box_info->id = p_in_box_info->id;
	memcpy(&p_out_box_info->kpts[0], &p_in_box_info->kpts[0],
		(sizeof(p_in_box_info->kpts)));
	p_out_box_info->quality_score = p_in_box_info->quality_score;

	/*calc face box width and height*/
	tmp_w =  p_in_box_info->rect[2] - p_in_box_info->rect[0];
	tmp_h =  p_in_box_info->rect[3] - p_in_box_info->rect[1];

	/*compare width and height,then enlarge*/
	if ((tmp_w <= w) && (tmp_h <= h)) {
		/*calc diff value*/
		diff_w = w - tmp_w;
		diff_h = h - tmp_h;
		ALOGD("box coordinate enlarge0:dif_w-%d; diff_h-%d\n", diff_w, diff_h);
		/*enlarge width*/
		p_out_box_info->rect[0] = p_in_box_info->rect[0];
		p_out_box_info->rect[1] = p_in_box_info->rect[1];
		p_out_box_info->rect[2] = p_in_box_info->rect[2];
		p_out_box_info->rect[3] = p_in_box_info->rect[3];
	} else {
		/*calc diff value*/
		diff_h = (tmp_w * h - tmp_h * w)/w;/* tmp_w/(tmp_h+diff_h) == w/h */
		diff_w = (tmp_h * w - tmp_w * h)/h;/* (tmp_w+diff_w)/tmp_h == w/h */
		ALOGD("box coordinate enlarge1:dif_w-%d; diff_h-%d\n", diff_w, diff_h);
		if (diff_h > 0) {
			/*assign width*/
			p_out_box_info->rect[0] = p_in_box_info->rect[0];
			p_out_box_info->rect[2] = p_in_box_info->rect[2];
			/*enlarge height*/
			p_out_box_info->rect[1] = p_in_box_info->rect[1] - (diff_h/2);
			p_out_box_info->rect[3] = p_in_box_info->rect[3] + (diff_h/2);
		} else {
			/*enlarge width*/
			p_out_box_info->rect[0] = p_in_box_info->rect[0] - (diff_w/2);
			p_out_box_info->rect[2] = p_in_box_info->rect[2] + (diff_w/2);
			p_out_box_info->rect[1] = p_in_box_info->rect[1];
			p_out_box_info->rect[3] = p_in_box_info->rect[3];
		}
	}

	/*parameter check*/
	if (p_out_box_info->rect[0] < 0)
		p_out_box_info->rect[0] = 0;
	if (p_out_box_info->rect[1] < 0)
		p_out_box_info->rect[1] = 0;
	if (p_out_box_info->rect[2] > screen_w)
		p_out_box_info->rect[2] = screen_w;
	if (p_out_box_info->rect[3] > screen_h)
		p_out_box_info->rect[3] = screen_h;

	return MEDIA_SUCCESS;
}

/**
 * cutout and compress picture
 * @param p_yuv_img_info-the pointer of picture which format is yuv
 * @return 0-success,other-failed
 */
static int app_management_reg_cutout(face_rect_t *p_box, management_reg_t *p_reg_handle)
{
	int ret = 0;
	time_t t = 0;
	struct cut_input tmp_input;
	struct cut_output tmp_output;
	/*the struct of jpg graph infomation*/
	struct output_image_info small_jpg_img_info;
	/*the struct of yuv graph infomation*/
	struct input_image_info small_yuv_img_info;
	person_reg_param_t reg_param;
	DIR *dir;

	memset(&reg_param, 0x0, sizeof(person_reg_param_t));

	app_management_reg_get_config(&reg_param);

	/*check parameter*/
	if (!p_box) {
		ALOGE("%s: parameter pointer is NULL", __func__);
		return -1;
	}
	/*init cut image info*/
	tmp_input.addr[0] = p_reg_handle->face_frame->viraddr[0];
	tmp_input.addr[1] = p_reg_handle->face_frame->viraddr[1];
	tmp_input.size[0] = p_reg_handle->face_frame->size[0];
	tmp_input.size[1] = p_reg_handle->face_frame->size[1];
	tmp_input.src_width = reg_param.jpeg_width_stride;
	tmp_input.src_height = reg_param.jpeg_height_stride;
	tmp_input.format = IMG_COLOR_NV12;
	tmp_input.dest_x = p_box->rect[0];
	/*x coordinate even alignment*/
	/*tmp_input.dest_x = (tmp_input.dest_x + 1) & (~0x01);*/
	tmp_input.dest_y = p_box->rect[1];
	/*16 byte alignment for jpeg enc hardware */
	tmp_input.dest_width = p_box->rect[2] - p_box->rect[0];
	tmp_input.dest_width = (tmp_input.dest_width + 15) & (~0x0f);
	/* avoid crossing the boundary */
	if (tmp_input.dest_x + tmp_input.dest_width >= tmp_input.src_width)
		tmp_input.dest_width -= 16;
	/*16 byte alignment for jpeg enc hardware */
	tmp_input.dest_height = p_box->rect[3] - p_box->rect[1];
	tmp_input.dest_height = (tmp_input.dest_height + 15) & (~0x0f);
	/* cross the boundary */
	if (tmp_input.dest_y + tmp_input.dest_height >= tmp_input.src_height)
		tmp_input.dest_height -= 16;
	ALOGD("face_box_info.rect[0] = %d\n", p_box->rect[0]);
	ALOGD("face_box_info.rect[1] = %d\n", p_box->rect[1]);
	ALOGD("face_box_info.rect[2] = %d\n", p_box->rect[2]);
	ALOGD("face_box_info.rect[3] = %d\n", p_box->rect[3]);
	ALOGD("tmp_input.addr[0] = %p\n", tmp_input.addr[0]);
	ALOGD("tmp_input.addr[1] = %p\n", tmp_input.addr[1]);
	ALOGD("tmp_input.size[0] = %d\n", tmp_input.size[0]);
	ALOGD("tmp_input.size[1] = %d\n", tmp_input.size[1]);

	/*call function to cutout*/
	ret = yuv_cut_start(&tmp_input, &tmp_output);
	if (ret) {
		ALOGE("%s: jpg cut failed\n", __func__);
		ret = ETYPE_JPG_CUTOUT_FAILED;
		goto cutout_error_exit;
	}
	ALOGD("tmp_output.width = %d\n", tmp_output.width);
	ALOGD("tmp_output.height = %d\n", tmp_output.height);
	ALOGD("tmp_output.format = %d\n", tmp_output.format);
	ALOGD("tmp_output.addr[0] = %p\n", tmp_output.addr[0]);
	ALOGD("tmp_output.addr[1] = %p\n", tmp_output.addr[1]);
	ALOGD("tmp_output.size[0] = %d\n", tmp_output.size[0]);
	ALOGD("tmp_output.size[1] = %d\n", tmp_output.size[1]);

	/*init compress image info*/
	memset(&small_yuv_img_info, 0x00, sizeof(small_yuv_img_info));
	memset(&small_jpg_img_info, 0x00, sizeof(small_jpg_img_info));
	small_yuv_img_info.addr[0] = tmp_output.addr[0];
	small_yuv_img_info.addr[1] = tmp_output.addr[1];
	small_yuv_img_info.size[0] = tmp_output.size[0];
	small_yuv_img_info.size[1] = tmp_output.size[1];
	small_yuv_img_info.width = tmp_output.width;
	small_yuv_img_info.height = tmp_output.height;
	small_yuv_img_info.format = tmp_output.format;
	small_yuv_img_info.qualify = 99;
	if (small_yuv_img_info.jpeg_info_buf == NULL)
		small_yuv_img_info.jpeg_info_buf = (unsigned char *)malloc(JPG_INFO_SIZE);
	if (small_yuv_img_info.jpeg_out_buf == NULL)
		small_yuv_img_info.jpeg_out_buf = (unsigned char *)malloc(JPG_OUT_SIZE);
	/*compress jpg picture*/
	ret = jpeg_enc_buffer(&small_yuv_img_info, &small_jpg_img_info);
	if (ret) {
		ALOGE("%s: jpg enc failed\n", __func__);
		yuv_cut_end(&tmp_output);
		ret = ETYPE_JPG_ENC_FAILED;
	}
	yuv_cut_end(&tmp_output);
	ALOGD("the small picture addr is: %p\n", &small_jpg_img_info);

	/*get time stamp*/
	time_get_current_sec(&t);

	/*organize path*/
	memset(p_reg_handle->small_pic_path, 0, 128);
	snprintf(p_reg_handle->small_pic_path,
		sizeof(p_reg_handle->small_pic_path),
		"%s/%ld-small%s", PIC_PATH, t, ".jpg");
	ALOGD("the small picture addr is: %s\n", p_reg_handle->small_pic_path);

	/*whether the dir is exist, if not the create*/
	dir = opendir(p_reg_handle->small_pic_path);
	if (dir == NULL) {
		ret = mk_mul_dirs(PIC_PATH);
		if (ret != 0) {
			ALOGE("Create multiple folder: %s fail\n",
				p_reg_handle->small_pic_path);
			goto cutout_error_exit;
		}
	} else
		closedir(dir);

	/* write file to tmp folder */
	ret = file_write(p_reg_handle->small_pic_path,
		(char *)small_jpg_img_info.buf,
		small_jpg_img_info.size);
	if (ret) {
		ALOGE("app_reg_handle_jpg_enc:the callback fun is NULL\n");
		goto cutout_error_exit;
	}

cutout_error_exit:

	if (small_yuv_img_info.jpeg_info_buf)
		free(small_yuv_img_info.jpeg_info_buf);
	if (small_yuv_img_info.jpeg_out_buf)
		free(small_yuv_img_info.jpeg_out_buf);

	return ret;
}

/**
 * recognition of face in picture
 * @param w the preset width of picture
 * @param h the preset height of picture
 * @param screen_w the width of screen
 * @param screen_h the height of screen
 * @return 0-success,other-failed
 */
static int app_management_reg_face_recg(int w, int h, int screen_w,
	int screen_h, management_reg_t *p_reg_handle)
{
	int ret = MEDIA_SUCCESS;
	face_info_t *api_feat = NULL;
	face_rect_t face_box_info;
	void *svc_handle  = get_face_handle();

	ALOGD("%s: enter\n", __func__);
	if (NULL == p_reg_handle->face_frame) {
		ALOGE("%s: yuv data pointer is NULL\n", __func__);
		ret = ETYPE_POINTER_IS_NULL;
		goto recg_error_exit;
	}


	/*call face recg check fun, to verify the legality of pictures*/
	api_feat = malloc(sizeof(face_info_t));
	if (!api_feat) {
		ALOGE("%s: alloc face_api_feat_t failed\n", __func__);
		ret = ETYPE_MALLOC_FAILED;
		goto recg_error_exit;
	}
	/*recognition face*/
	ret = face_recg_block(svc_handle, p_reg_handle->face_frame, 1, api_feat);
	if (ret <= 0) {
		ALOGE("%s: not detect face\n", __func__);
		ret = ETYPE_NOT_DET_FACE;
		goto recg_error_exit;
	} else if (ret > 1) {
		ALOGE("%s: detect mutiple faces\n", __func__);
		ret = ETYPE_DET_MUTIPLE_FACE;
		goto recg_error_exit;
	}
	ALOGD("%s: recg end", __func__);

	/*enlarge coordinate*/
	ret = app_management_reg_face_box_cor_enlarge(&api_feat->rect,
		&face_box_info, w, h, screen_w, screen_h, p_reg_handle);
	if (ret) {
		ALOGE("%s: box coordinate enlarge failed\n", __func__);
		ret = -1;
		goto recg_error_exit;
	}
	/*cutout small pic*/
	ret = app_management_reg_cutout(&face_box_info, p_reg_handle);
	if (ret) {
		ALOGE("%s: cutout failed\n", __func__);
		goto recg_error_exit;
	}

recg_error_exit:
	if (api_feat)
		free(api_feat);
	/*photo face frame is malloc by face server, so return buff*/
	if (p_reg_handle->graph_reg_flag == 0) {
		app_management_reg_return_buff(p_reg_handle->face_frame);
		p_reg_handle->face_frame = NULL;
	} else {
		/*graph face frame is malloc by app, so free here*/
		if (p_reg_handle->face_frame) {
			free(p_reg_handle->face_frame);
			p_reg_handle->face_frame = 0;
		}
	}
	return ret;
}

void app_management_reg_get_config(void *param)
{
	person_reg_param_t *reg_param = (person_reg_param_t *)param;

	if (reg_param == NULL)
		return;

	reg_param->check_face_size = config_get_int("register", "check_face_size",
			0);
	reg_param->face_max = config_get_int("register", "face_max", 512);
	reg_param->face_min = config_get_int("register", "face_min", 128);
	reg_param->quality = config_get_int("register", "reg_quality", 0);
	reg_param->threshold = config_get_double("register", "reg_threshold",
			0.5f);
	reg_param->refresh_db_each_time = config_get_int("register",
			"refresh_db_each_time", 0);
	reg_param->jpeg_width_stride = config_get_int("register", "jpeg_w_stride",
			800);
	reg_param->jpeg_height_stride = config_get_int("register",
			"jpeg_h_stride", 1024);

	reg_param->save_pic_enable = config_get_int("register", "save_pic_enable",
			1);
}

static int app_management_reg_save_reg_info(management_reg_t *p_reg_handle)
{
	img_info_t reg_img;
	e_person_reg_code err_code;
	person_reg_param_t reg_param;
	char detail_info[64];
	int ret = 0;

	if (NULL == p_reg_handle) {
		ALOGE("%s: reg_handle is NULL", __func__);
		return -1;
	}

	ee_queue_t head;

	ret = db_person_info_query_begin_by_name(p_reg_handle->person_info.name,
		&head);
	db_person_info_query_end(&head);
	if (ret <= 0) {

		snprintf(reg_img.pic_path, sizeof(reg_img.pic_path), "%s",
				p_reg_handle->large_pic_path);
		snprintf(reg_img.face_path, sizeof(reg_img.face_path), "%s",
				p_reg_handle->small_pic_path);
		ALOGD("%s: reg_img.pic_path: %s\n", __func__, reg_img.pic_path);
		ALOGD("%s: reg_img.face_path: %s\n", __func__, reg_img.face_path);

		app_management_reg_get_config(&reg_param);

		reg_param.batch_reg = 0;
		ret = person_reg_proc(&p_reg_handle->person_info, &reg_img,
				detail_info, sizeof(detail_info), &reg_param, &err_code, 0);

	} else
		err_code = PERSON_REGISTRATION_ERROR;

	if (ret) {
		char test_reason_str[64] = {0};

		ret = err_code;
		person_reg_fail_reason(ret, test_reason_str,
			sizeof(test_reason_str));

		printf("person register failed; %s\n", test_reason_str);
	} else {
		ret = face_db_update();
		if (ret)
			printf("%s\n", elang_str("TEXT_PERSON_INFO_INSERT_DB_ERROR"));
		else
			printf("%s \n", elang_str("TEXT_PERSONAL_ADD_SUCCESS"));
	}
    result_code = ret;
	if (!p_reg_handle->graph_reg_flag)
		remove(p_reg_handle->large_pic_path);
	remove(p_reg_handle->small_pic_path);

	p_reg_handle->graph_reg_flag = 0;

	return ret;
}

static void app_manamement_photo_start_face_recg(management_reg_t *reg_handle)
{
	int ret = 0;
	char tmp_buff[128] = { 0 };
	person_reg_param_t reg_param;

	memset(&reg_param, 0x0, sizeof(person_reg_param_t));

	app_management_reg_get_config(&reg_param);

	if (reg_handle == NULL) {
		printf("error %s: reg handle is NULL, face detect\n", __func__);
		return;
	}
	ALOGD("call api_reg_face_recg\n");

	ret = app_management_reg_face_recg(PRESET_FACE_PICTURE_WIDTH, PRESET_FACE_PICTURE_HEIGHT,
		reg_param.jpeg_width_stride, reg_param.jpeg_height_stride, reg_handle);
	if (ret) {
		ALOGE("%s: error code: %d\n", __func__, ret);
		switch (ret) {
		case ETYPE_DET_MUTIPLE_FACE:
			strcpy(tmp_buff, elang_str("TEXT_ERROR_DET_MUTIPLE_FACES_TIP"));
			result_code = PERSON_INFO_MULTIPLE_FACE;
			break;

		case ETYPE_DET_FACE_QUALITY_LOW:
			strcpy(tmp_buff, elang_str("TEXT_ERROR_FACE_LOW_QUALITY"));
			result_code = PERSON_INFO_FACE_QUALITY_TOO_LOW;
			break;
		case ETYPE_NOT_DET_FACE:
			strcpy(tmp_buff, elang_str("TEXT_ERROR_NOT_DET_FACE"));
			result_code = PERSON_INFO_NO_DETECT_FACE;
			break;

		default:
			strcpy(tmp_buff, elang_str("TEXT_ERROR_FACE_RECG_FAILED"));
			break;
		}
		printf("%s : %s \n", elang_str("TEXT_ERROR_TIP"), tmp_buff);
		/*face recognition failed, and delete big picture*/
		ALOGD("%d: delete big picture: %s\n", __LINE__,
				reg_handle->large_pic_path);
		remove(reg_handle->large_pic_path);
	} else {
		app_management_reg_save_reg_info(reg_handle);
	}

	pthread_mutex_lock(&reg_handle->mutex_lock);
	pthread_cond_signal(&reg_handle->cond);
	pthread_mutex_unlock(&reg_handle->mutex_lock);
}

static int app_manamement_reg_photo_comleted(face_frame_t *frame)
{
	int ret = 0;

	reg_handle->face_frame = frame;
	reg_handle->graph_reg_flag = 0;
	ret = app_management_reg_big_jpg_enc(reg_handle);

	if (!ret)
		app_manamement_photo_start_face_recg(reg_handle);

	return ret;
}

static int app_management_reg_image_handle_cb(face_frame_t *frame)
{
	int ret = 0;

	ret = app_manamement_reg_photo_comleted(frame);

	if (ret)
		app_management_reg_return_buff(frame);

	return ret;
}

static int reg_take_picture()
{
	int ret = 0;
	void *svc_handle = get_face_handle();

	if (NULL == svc_handle) {
		ALOGE("%s: svc_handle is NULL\n", __func__);
		return ret;
	}

	ret = face_take_picture(svc_handle, FACE_CAMERA_RGB,
			app_management_reg_image_handle_cb);
	if (ret)
		ALOGE("%s: face rgb take picture failed\n", __func__);
	return ret;
}

static void *reg_thread(void *arg)
{
	reg_handle->reg_running = 1;
	reg_take_picture();
	reg_handle->reg_running = 0;
	return NULL;
}

int app_management_reg_start(const char *name)
{
	pthread_t reg_thread_thd;

	if (reg_handle != NULL && reg_handle->reg_running == 1)
		return -1;

	app_management_reg_stop();

	reg_handle = (management_reg_t *) malloc(sizeof(management_reg_t));
	memset(reg_handle, 0, sizeof(management_reg_t));

	if (name == NULL) {
		time_t t = 0;

		/*get time stamp*/
		time_get_current_sec(&t);

		snprintf(reg_handle->person_info.name,
				sizeof(reg_handle->person_info.name), "%ld-test", t);
	} else {
		snprintf(reg_handle->person_info.name,
				sizeof(reg_handle->person_info.name), "%s", name);
	}

	if (pthread_mutex_init(&reg_handle->mutex_lock, NULL)) {
		return -1;
	}
	if (pthread_cond_init(&reg_handle->cond, NULL)) {
		return -1;
	}

	pthread_create(&reg_thread_thd, NULL, reg_thread, NULL);

	return 0;
}

void app_management_reg_stop()
{
	if (reg_handle) {
		free(reg_handle);
		reg_handle = NULL;
	}
}

int app_management_reg_by_jpg_and_name(const char *path, const char *name)
{
	/* Check if the image exists */
	img_info_t reg_img;
	person_reg_param_t reg_param;
	person_t p_person_info;
	char detail_info[256];
	e_person_reg_code err_code;
	int ret = 0;

	if (access(path, F_OK) != 0)
		return PERSON_INFO_PIC_NO_HAVE;

	/** Step 1: Call the personnel registration interface to register. */
	memset(&reg_img, 0, sizeof(img_info_t));
	snprintf(reg_img.pic_path, sizeof(reg_img.pic_path), "%s", path);

	reg_param.check_face_size = config_get_int("register", "check_face_size",
		0);
	reg_param.face_max = config_get_int("register", "face_max", 512);
	reg_param.face_min = config_get_int("register", "face_min", 128);
	reg_param.quality = config_get_int("register", "reg_quality", 0);
	reg_param.threshold = config_get_double("register", "reg_threshold",
		0.5f);
	reg_param.jpeg_width_stride = config_get_int("register", "jpeg_w_stride",
		800);
	reg_param.jpeg_height_stride = config_get_int("register", "jpeg_h_stride",
		1024);

	reg_param.batch_reg = 0;
	sprintf(p_person_info.name, "%s", name);
	ret = person_reg_proc(&p_person_info, &reg_img, detail_info,
		sizeof(detail_info), &reg_param, &err_code, 0);

	return ret;
}

/* get reg result */
int app_management_get_reg_result(void)
{
	pthread_mutex_lock(&reg_handle->mutex_lock);
	pthread_cond_wait(&reg_handle->cond, &reg_handle->mutex_lock);
	pthread_mutex_unlock(&reg_handle->mutex_lock);
	return (int)result_code;
}

