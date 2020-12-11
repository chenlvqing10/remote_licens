/*
 * person.c - Public interface for personnel management
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "sqlite3.h"
#include "db_punch_record.h"
#include "db_person_info.h"

#define LOG_TAG "MANAGEMENT"
#include "log/log.h"

#include "lombo_malloc.h"
#include "elang.h"
#include "jpeg_dec.h"
#include "face_svc.h"
#include "jpeg_enc.h"
#include "cut.h"
#include "face.h"
#include "time_conv.h"
#include "file_ops.h"
#include "config.h"
#include "face.h"
#include "person.h"
#include "flip.h"
#include "recg_img.h"

person_reg_error_info_t st_reg_info[] = {
	{PERSON_REG_OK,				"PASS"},
	{PERSON_REG_REFUSE,			"REFUSE"},
	{PERSON_INFO_DATA_ERROR,		"人员信息错误"},
	{PERSON_INFO_NO_DETECT_FACE,		"无法检测到人脸"},
	{PERSON_INFO_FACE_FEATURE_FAIL,		"提取特征值失败"},
	{PERSON_INFO_MULTIPLE_FACE,		"检测到多个人脸"},
	{PERSON_INFO_FACE_SIZE_ERROR,		"人脸大小不符合要求"},
	{PERSON_INFO_IMAGE_DECODE_FAIL,		"图片解码出错"},
	{PERSON_INFO_PIC_COPY_ERROR,		"图片拷贝失败"},
	{PERSON_INFO_PIC_NO_HAVE,		"无图片"},
	{PERSON_INFO_INSERT_DB_ERROR,		"数据库注册失败"},
	{PERSON_INFO_FACE_QUALITY_TOO_LOW,	"人脸质量过低"},
	{PERSON_INFO_FACE_SIMILARITY_TOO_HIGH,	"人脸相似度过高"},
	{PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT, "超过单次注册上限"},
	{PERSON_INFO_IMAGE_FORMAT_ERROR,	"图片格式不符合要求"},
	{PERSON_INFO_INTERNAL_ERROR,		"程序内部错误"},
	{PERSON_REGISTRATION_ERROR,		"人员重复注册"},
	{PERSON_MIS_RECOGNIZED,			"误识别"},
	{PERSON_REG_COMPARE_RESULT_OK,		"OK"},
	{PERSON_REG_COMPARE_RESULT_FAIL,	"FAIL"}
};

#define REGISTER_PIC_SAVE_PATH		"/data/face_register"

#define JPEG_FAST_DEC

/**
 * person_is_already_register Check if this person is already registered
 * @p_name The name of the person who needs to be checked
 * @return 0-if this person is already registered,
 *	   -1-This person is not registered
 */
int person_is_already_register(char *p_name)
{
	ee_queue_t db_head;
	int ret = 0;
	int is_reg = -1;

	ALOGD("%s: enter\n", __func__);

	/*query register info by name */
	ret = db_person_info_query_begin(p_name, 0, 1, &db_head);
	if (ret != 0) {
		db_person_info_query_end(&db_head);
		is_reg = 0;
	}

	return is_reg;
}

/**
 * person_reg_fail_reason - Returns the information corresponding to the
 *	error code
 * @code Failed error code when importing personnel registration information
 * @fail_reason_str Fail reason string
 * @fail_reason_str_max_len Maximum length of the failure information string
 *
 * When there is a problem with one person's registration information, in order
 * to prompt the user, write this information to another excel.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_reg_fail_reason(e_person_reg_code code, char *fail_reason_str,
	unsigned int fail_reason_str_max_len)
{
	int i = 0;
	int reg_error_mode_count = 0;

	assert(fail_reason_str != NULL);

	reg_error_mode_count = (sizeof(st_reg_info) / sizeof(person_reg_error_info_t));
	for (i = 0; i < reg_error_mode_count; i++) {
		if (st_reg_info[i].error_code == code) {
			snprintf(fail_reason_str, fail_reason_str_max_len, "%s",
				st_reg_info[i].error_msg);
			break;
		}
	}

	return 0;
}

/**
 * person_add_to_db - Write personnel information to the database.
 * @p_person_info A point to 'person_t'
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_add_to_db(person_t *p_person_info)
{
	int ret = 0;
	int sql_code;
	person_info_t user_a;

	assert(p_person_info != NULL);

	/* Call to add a personnel interface */

	/* Employee mumber(E.g SW200) */
	memcpy(user_a.employee_number, p_person_info->employee_number,
		sizeof(user_a.employee_number));

	/* Name */
	memcpy(user_a.name, p_person_info->name, sizeof(user_a.name));

	/* Gender(E.g "Man/Women/..." */
	memcpy(user_a.gender, p_person_info->gender, sizeof(user_a.gender));

	/* Nationa */
	memcpy(user_a.nationa, p_person_info->nationa, sizeof(user_a.nationa));

	/* ID card number */
	memcpy(user_a.id_card_number, p_person_info->id_card_number,
		sizeof(user_a.id_card_number));

	/* Telephone number */
	memcpy(user_a.mobile, p_person_info->mobile, sizeof(user_a.mobile));


	/* Department name(E.g "Software Lab001") */
	memcpy(user_a.department_name, p_person_info->department_name,
		sizeof(user_a.department_name));

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 */
	user_a.access_right = p_person_info->access_right;

	/* When the permission is temporary access permission mode 1,
	 * Allowed start time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	user_a.temporary_access_start_time = p_person_info->temporary_access_start_time;

	/* When the permission is temporary access permission mode 1,
	 * Allowed end time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	user_a.temporary_access_end_time = p_person_info->temporary_access_end_time;

	/* Allowed number of passes
	 * When the permission is temporary access permission mode 2
	 */
	user_a.temporary_access_times = p_person_info->temporary_access_times;

	/* Access card number.
	 * Effective when the punch mode is "Card reader"
	 */
	memcpy(user_a.access_card_number, p_person_info->access_card_number,
		sizeof(user_a.access_card_number));

	/* Remarks: Used to fill in the description information */
	memcpy(user_a.remarks, p_person_info->remarks, sizeof(user_a.remarks));

	/* Register photo name.(Including jpg suffix) */
	memcpy(user_a.regist_photo_name, p_person_info->regist_photo_name,
		sizeof(user_a.regist_photo_name));

	/* Face feature value. Used to store facial feature values */
	memcpy(user_a.face_features, p_person_info->face_features,
		sizeof(user_a.face_features));

	/* face key points */
	memcpy(user_a.face_key_points, p_person_info->face_key_points,
		sizeof(user_a.face_key_points));

	/* norm */
	user_a.face_norm[0] = p_person_info->face_norm[0];
	user_a.face_norm[1] = p_person_info->face_norm[1];

	/* Insert into database */
	sql_code = db_person_info_insert(&user_a);
	if (SQLITE_OK == sql_code)
		ret = 0;
	else
		ret = -1;


	return ret;
}

/**
 * person_reg_proc – Processing person registration interface.
 * @p_person_info IN, Person registration information
 * @reg_img IN, Image information for preson registration
 * @err_code OUT, Error code returned when person registration fails
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_reg_proc(person_t *p_person_info, img_info_t *reg_img,
	char *detail_info, int detail_info_size, person_reg_param_t *reg_param,
	e_person_reg_code *err_code, int h_flip)
{
	struct recg_img_intput input;
	struct recg_img_output output;
	struct input_image_info jpeg_input;
	struct output_image_info jpeg_output;
	struct flip_input flip;
	face_frame_t frame;
	struct cut_input cut_in;
	struct cut_output cut_out;
	face_info_t *info = NULL;
	DIR *dir;
	char save_path[64], pic_path[64], date[16], name[32];
	int ret, face_num, face_max, face_min, face_width, face_height;

	assert((p_person_info != NULL) && (reg_img != NULL) && (err_code != NULL));

	*err_code = PERSON_REG_OK;
	memset(detail_info, 0, detail_info_size);

	ALOGD("register photo path:%s", reg_img->pic_path);

	/* check file */
	ret = is_file_exist(reg_img->pic_path);
	if (ret) {
		ALOGE("%s not found", reg_img->pic_path);
		*err_code = PERSON_INFO_PIC_NO_HAVE;
	}

	input.file = reg_img->pic_path;
	input.stride_width = reg_param->jpeg_width_stride;
	input.stride_height = reg_param->jpeg_height_stride;

    	/* Check whether the image size meets the specification */
	ret = recg_img_size_check(&input, &output);
	if (ret) {
		printf("check image size failed\n");
		return -1;
	}

	ret = recg_img_dec_begin(&input, &output);
	if (ret) {
		ALOGE("image dec failed");
		*err_code = PERSON_INFO_IMAGE_DECODE_FAIL;
		return -1;
	}

	if (h_flip) {
		ALOGD("flip image horizontally");
		flip.addr[0] = output.virt_addr[0];
		flip.addr[1] = output.virt_addr[1];
		flip.addr[2] = output.virt_addr[2];
		flip.size[0] = output.size[0];
		flip.size[1] = output.size[1];
		flip.size[2] = output.size[2];
		flip.width = output.stride_width;
		flip.height = output.stride_height;
		flip.format = FLIP_COLOR_NV12;
		ret = yuv_flip(&flip);
		if (ret) {
			ALOGE("flip failed");
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			goto free_res;
		}

		lombo_cache_flush_vir(output.virt_addr[0]);
	}

	info = malloc(sizeof(face_info_t));
	if (!info) {
		ALOGE("malloc face_info_t failed");
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.virt_addr[0];
	frame.viraddr[1] = output.virt_addr[1];
	frame.viraddr[2] = output.virt_addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];
	face_num = face_recg_block(get_face_handle(), &frame, 1, info);
	if (face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	/* check face num */
	if (face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		*err_code = PERSON_INFO_NO_DETECT_FACE;
		goto free_res;
	} else if (face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, face_num);
		*err_code = PERSON_INFO_MULTIPLE_FACE;
		goto free_res;
	}

	/* check face size */
	if (reg_param->check_face_size) {
		face_max = reg_param->face_max;
		face_min = reg_param->face_min;
		face_width = info->rect.rect[2] - info->rect.rect[0];
		face_height = info->rect.rect[3] - info->rect.rect[1];
		if (face_width > face_max || face_height > face_max) {
			ALOGE("face size(%dx%d) > face max(%d)",
				face_width, face_height, face_max);
			*err_code = PERSON_INFO_FACE_SIZE_ERROR;
			snprintf(detail_info, 64, "%dx%d", face_width, face_height);
			goto free_res;
		}
		if (face_width < face_min || face_height < face_min) {
			ALOGE("face size(%dx%d) < face min(%d)",
				face_width, face_height, face_min);
			*err_code = PERSON_INFO_FACE_SIZE_ERROR;
			snprintf(detail_info, detail_info_size, "%dx%d",
					face_width, face_height);
			goto free_res;
		}
	}

	/* check face quality */
	if (info->rect.quality_score < reg_param->quality) {
		ALOGE("%s: face quality(%d) is too low\n", __func__,
						info->rect.quality_score);
		*err_code = PERSON_INFO_FACE_QUALITY_TOO_LOW;
		goto free_res;
	}

	ret = time_get_cur_date(date);
	if (ret) {
		ALOGE("get date failed\n");
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	if (reg_param->save_pic_enable) {
		snprintf(save_path, sizeof(save_path), "%s/%s", REGISTER_PIC_SAVE_PATH,
				date);
		/* check directory */
		dir = opendir(save_path);
		if (dir == NULL) {
			ret = mk_mul_dirs(save_path);
			if (ret != 0) {
				ALOGE("Create multiple folder: %s fail\n", save_path);
				return -1;
			}
		} else
			closedir(dir);
	}

	/* cut the face */
	cut_in.addr[0] = frame.viraddr[0];
	cut_in.addr[1] = frame.viraddr[1];
	cut_in.addr[2] = frame.viraddr[2];
	cut_in.size[0] = frame.size[0];
	cut_in.size[1] = frame.size[1];
	cut_in.size[2] = frame.size[2];
	cut_in.src_width = frame.width;
	cut_in.src_height = frame.height;
	cut_in.format = IMG_COLOR_NV12;
	cut_in.dest_x = info->rect.rect[0];
	cut_in.dest_y = info->rect.rect[1];
	/*16 byte alignment for jpeg enc hardware */
	cut_in.dest_width = info->rect.rect[2] - info->rect.rect[0];
	cut_in.dest_width = (cut_in.dest_width + 15) & (~0x0f);
	cut_in.dest_height = info->rect.rect[3] - info->rect.rect[1];
	cut_in.dest_height = (cut_in.dest_height + 15) & (~0x0f);
	/* avoid crossing the boundary */
	if (cut_in.dest_x + cut_in.dest_width >= cut_in.src_width)
		cut_in.dest_width -= 16;
	if (cut_in.dest_y + cut_in.dest_height >= cut_in.src_height)
		cut_in.dest_height -= 16;
	/*call function to cutout*/
	ret = yuv_cut_start(&cut_in, &cut_out);
	if (ret) {
		ALOGE("%s: cut failed\n", __func__);
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	memset(p_person_info->regist_photo_name, 0,
				sizeof(p_person_info->regist_photo_name));

	if (reg_param->save_pic_enable) {
		snprintf(pic_path, sizeof(pic_path), "%s/%s-small.jpg", save_path,
				p_person_info->name);
		ALOGD("face pic save path:%s", pic_path);
		jpeg_input.addr[0] = cut_out.addr[0];
		jpeg_input.addr[1] = cut_out.addr[1];
		jpeg_input.addr[2] = cut_out.addr[2];
		jpeg_input.size[0] = cut_out.size[0];
		jpeg_input.size[1] = cut_out.size[1];
		jpeg_input.size[2] = cut_out.size[2];
		jpeg_input.width = cut_out.width;
		jpeg_input.height = cut_out.height;
		jpeg_input.format = IMG_COLOR_NV12;
		jpeg_input.qualify = 99;
		jpeg_input.file = pic_path;
		ret = jpeg_enc_file(&jpeg_input, &jpeg_output);
		yuv_cut_end(&cut_out);
		if (ret) {
			ALOGE("jpeg enc small image failed\n");
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			goto free_res;
		}

		snprintf(pic_path, sizeof(pic_path), "%s/%s-large.jpg", save_path,
				p_person_info->name);
		ALOGD("large pic save path:%s", pic_path);
		ret = file_copy(reg_img->pic_path, pic_path);
		if (ret) {
			ALOGE("copy file failed\n");
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			goto free_res;
		}

		/* Update registration image file path.
		 * Attention: regist_photo_name is a path without photo name.
		 */
		snprintf(p_person_info->regist_photo_name,
				sizeof(p_person_info->regist_photo_name),
				"%s", save_path);
	}
	/* save feature, feature norm, face key points */
	memcpy(p_person_info->face_features, info->feat.values,
					sizeof(info->feat.values));
	p_person_info->face_norm[0] = info->feat.norm[0];
	p_person_info->face_norm[1] = info->feat.norm[1];
	snprintf(p_person_info->face_key_points,
			sizeof(p_person_info->face_key_points),
			"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
			info->rect.kpts[0], info->rect.kpts[1], info->rect.kpts[2],
			info->rect.kpts[3], info->rect.kpts[4], info->rect.kpts[5],
			info->rect.kpts[6], info->rect.kpts[7], info->rect.kpts[8],
			info->rect.kpts[9]);

	/* get recognition threshold */
	if (info->feat.score >= reg_param->threshold) {
		ALOGE("%s: face simularity(%f) is too high\n", __func__,
						info->feat.score);
		*err_code = PERSON_INFO_FACE_SIMILARITY_TOO_HIGH;
		ret = face_db_get_name(info->feat.max_id, name, sizeof(name));
		if (ret) {
			ALOGE("%s: get name failed", __func__);
			snprintf(detail_info, detail_info_size, "%s(%f)",
					"get_name_failed", info->feat.score);
		} else
			snprintf(detail_info, detail_info_size, "%s(%f)", name,
					info->feat.score);

		/* delete large and small photo if simularity is too high
		* in batch picture regitration proccess
		*/
		if (reg_param->batch_reg && reg_param->save_pic_enable) {
			snprintf(pic_path, sizeof(pic_path), "%s/%s-small.jpg", save_path,
					p_person_info->name);
			file_remove(pic_path);
			snprintf(pic_path, sizeof(pic_path), "%s/%s-large.jpg", save_path,
					p_person_info->name);
			file_remove(pic_path);
		}

		goto free_res;
	}

	/** If the registration is successful:
	 * Write the personnel information to the database.
	 */
	ret = person_add_to_db(p_person_info);
	if (ret != 0) {
		*err_code = PERSON_INFO_INSERT_DB_ERROR;
		goto free_res;
	}

	if (reg_param->refresh_db_each_time) {
		ret = face_db_update();
		if (ret) {
			ALOGE("%s: update db failed", __func__);
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			goto free_res;
		}
	}

free_res:
	if (info) {
		free(info);
		info = NULL;
	}
	recg_img_dec_end(&output);
	if (*err_code != PERSON_REG_OK)
		return -1;

	return 0;
}

/**
 * person_recognition_proc – Processing person recognition interface.
 * @p_person_info IN, Person registration information
 * @reg_img IN, Image information for preson registration
 * @err_code OUT, Error code returned when person registration fails
 * @recg_name OUT, When the error type is "false recognition", the name of the person who
 *	was misidentified is stored.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_recognition_proc(person_t *p_person_info, img_info_t *reg_img,
	e_person_reg_code *err_code, char *detail_info, int detail_size,
	person_recg_param_t *recg_param)
{
	struct recg_img_intput input;
	struct recg_img_output output;
	face_frame_t frame;
	face_info_t *info = NULL;
	int ret, face_num;
	char name[64];

	assert((p_person_info != NULL) && (reg_img != NULL)
		&& (err_code != NULL) && recg_param != NULL);

	*err_code = PERSON_REG_OK;

	ALOGD("recognition photo path:%s", reg_img->pic_path);
	input.file = reg_img->pic_path;
	input.stride_width = recg_param->jpeg_width_stride;
	input.stride_height = recg_param->jpeg_height_stride;
	ret = recg_img_dec_begin(&input, &output);
	if (ret) {
		ALOGE("image dec failed");
		*err_code = PERSON_INFO_IMAGE_DECODE_FAIL;
		return -1;
	}

	info = malloc(sizeof(face_info_t));
	if (!info) {
		ALOGE("malloc face_info_t failed");
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	/* step1: detect face and extract face feature */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.virt_addr[0];
	frame.viraddr[1] = output.virt_addr[1];
	frame.viraddr[2] = output.virt_addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];
	face_num = face_recg_block(get_face_handle(), &frame, 1, info);
	if (face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	if (face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		*err_code = PERSON_INFO_NO_DETECT_FACE;
		goto free_res;
	} else if (face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, face_num);
		*err_code = PERSON_INFO_MULTIPLE_FACE;
		goto free_res;
	}

	/* fix me: get recognition threshold */
	if (info->feat.score >= recg_param->threshold) {
		ALOGD("%s: pass(%f)\n", __func__, info->feat.score);
		ret = face_db_get_name(info->feat.max_id, name, sizeof(name));
		if (ret) {
			ALOGE("get name from db failed, max_id:%d", info->feat.max_id);
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			goto free_res;
		}
		if (strncmp(name, p_person_info->name, sizeof(name))) {
			/* mis-recognition occurs,
			* recognised name is not equal to name in excel table
			*/
			*err_code = PERSON_MIS_RECOGNIZED;
			snprintf(detail_info, 64, "%s(%f)", name, info->feat.score);
			goto free_res;
		} else {
			snprintf(detail_info, 64, "%f", info->feat.score);
			*err_code = PERSON_REG_OK;
		}
	} else {
		ALOGD("%s: refuse(%f)\n", __func__, info->feat.score);
		snprintf(detail_info, 64, "%f", info->feat.score);
		*err_code = PERSON_REG_REFUSE;
	}

free_res:
	if (info) {
		free(info);
		info = NULL;
	}
	recg_img_dec_end(&output);
	if (*err_code != PERSON_REG_OK && *err_code != PERSON_REG_REFUSE)
		return -1;

	return 0;
}

/**
 * person_get_feats_from_data – Get features from jpeg data.
 * @jpeg_data IN, JPEG data.
 * @len IN, The length of the jpeg picture.
 * @check_size IN, If check the size of the face or not.
 * @feats OUT, Save the feature data of the face.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_get_feats_from_data(char *jpeg_data, int len, int check_size, char *feats)
{
	struct jpeg_dec_output	output;
	face_frame_t		frame;
	int			ret = 0;
	int			face_num = 0;
	face_info_t		*info = NULL;
	int			face_max;
	int			face_min;
	int			face_width;
	int			face_height;
	int			w_stride = 0;
	int			h_stride = 0;
#ifdef CHECK_SIMULARITY
	float			threshold = 0.0;
#endif
	if ((NULL == jpeg_data) || (NULL == feats))
		return PERSON_REG_REFUSE;

	w_stride = config_get_int("register", "jpeg_w_stride", 800);
	h_stride = config_get_int("register", "jpeg_h_stride", 1024);

	ret = jpeg_dec_data(jpeg_data, len, w_stride, h_stride, JPEG_SCALE_AUTO, &output);
	if (ret) {
		ALOGE("jpeg dec failed");
		return PERSON_INFO_IMAGE_DECODE_FAIL;
	}

	info = (face_info_t *)malloc(sizeof(face_info_t));
	if (!info) {
		ALOGE("malloc face_info_t failed");
		return PERSON_INFO_INTERNAL_ERROR;
	}

	/* get face info */
	frame.width = output.stride_width;
	frame.height = output.stride_height;
	frame.viraddr[0] = output.addr[0];
	frame.viraddr[1] = output.addr[1];
	frame.viraddr[2] = output.addr[2];
	frame.phyaddr[0] = output.phy_addr[0];
	frame.phyaddr[1] = output.phy_addr[1];
	frame.phyaddr[2] = output.phy_addr[2];
	frame.size[0] = output.size[0];
	frame.size[1] = output.size[1];
	frame.size[2] = output.size[2];
	face_num = face_recg_block(get_face_handle(), &frame, 1, info);
	if (face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		if (info) {
			free(info);
			info = NULL;
		}
		jpeg_dec_end(&output);
		return PERSON_INFO_INTERNAL_ERROR;
	}

	/* check face num */
	if (face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		if (info) {
			free(info);
			info = NULL;
		}
		jpeg_dec_end(&output);
		return PERSON_INFO_NO_DETECT_FACE;
	} else if (face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, face_num);
		if (info) {
			free(info);
			info = NULL;
		}
		jpeg_dec_end(&output);
		return PERSON_INFO_MULTIPLE_FACE;
	}

	/* check face size */
	if (check_size != 0) {
		face_max = config_get_int("register", "face_max", 512);
		face_min = config_get_int("register", "face_min", 128);
		face_width = info->rect.rect[2] - info->rect.rect[0];
		face_height = info->rect.rect[3] - info->rect.rect[1];
		if (face_width > face_max || face_height > face_max) {
			ALOGE("face size(%dx%d) > face max(%d)",
				face_width, face_height, face_max);
			if (info) {
				free(info);
				info = NULL;
			}
			jpeg_dec_end(&output);
			return PERSON_INFO_FACE_SIZE_ERROR;
		}
		if (face_width < face_min || face_height < face_min) {
			ALOGE("face size(%dx%d) < face min(%d)",
				face_width, face_height, face_min);
			if (info) {
				free(info);
				info = NULL;
			}
			jpeg_dec_end(&output);
			return PERSON_INFO_FACE_SIZE_ERROR;
		}
	}

	/* check face quality */
	if (info->rect.quality_score < 0) {
		ALOGE("%s: face quality(%d) is too low\n", __func__,
						info->rect.quality_score);
		if (info) {
			free(info);
			info = NULL;
		}
		jpeg_dec_end(&output);
		return PERSON_INFO_FACE_QUALITY_TOO_LOW;
	}
#ifdef CHECK_SIMULARITY
	threshold = config_get_double("register", "reg_threshold", 0.5f);
	/* fix me: get recognition threshold */
	if (info->feat.score >= threshold) {
		ALOGE("%s: face simularity(%f) is too high\n", __func__,
						info->feat.score);
		if (info) {
			free(info);
			info = NULL;
		}
		jpeg_dec_end(&output);
		return PERSON_INFO_FACE_SIMILARITY_TOO_HIGH;
	}
#endif
	/* save feature, feature norm, face key points */
	memcpy(feats, info->feat.values, sizeof(info->feat.values));
	memcpy(&feats[FACE_FEAT_SIZE], info->rect.kpts, 10 * sizeof(float));
	memcpy(&feats[FACE_FEAT_SIZE + 10 * sizeof(float)],
		&info->feat.norm, sizeof(float) * 2);

	if (info) {
		free(info);
		info = NULL;
	}
	jpeg_dec_end(&output);
	return PERSON_REG_OK;
}

