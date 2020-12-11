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

/*#include "sqlite3.h"*/
/*#include "db_punch_record.h"*/
/*#include "db_person_info.h"*/

#define LOG_TAG "MANAGEMENT"
#include "log/log.h"

#include "clang.h"
#include "lombo_malloc.h"
#include "jpeg_dec.h"
#include "face_svc.h"
#include "jpeg_enc.h"
#include "cut.h"
#include "face.h"
#include "time_conv.h"
#include "file_ops.h"
/*#include "config.h"*/
#include "face.h"
#include "person.h"
#include "flip.h"
#include "db_all.h"

/* error info list */
person_reg_error_info_t st_reg_info[] = {
	{PERSON_REG_OK,				"TEXT_PERSON_REG_OK"},
	{PERSON_REG_REFUSE,			"TEXT_PERSON_REG_REFUSE"},
	{PERSON_INFO_DATA_ERROR,		"TEXT_PERSON_INFO_DATA_ERROR"},
	{PERSON_INFO_PIC_NO_HAVE,		"TEXT_PERSON_INFO_PIC_NO_HAVE"},
	{PERSON_INFO_NO_DETECT_FACE,		"TEXT_PERSON_INFO_NO_DETECT_FACE"},
	{PERSON_INFO_FACE_FEATURE_FAIL,		"TEXT_PERSON_INFO_FACE_FEATURE_FAIL"},
	{PERSON_INFO_PIC_SIZE_LARGER,	"TEXT_PERSON_INFO_IMAGE_DECODE_FAIL"},
	{PERSON_INFO_MULTIPLE_FACE,		"TEXT_PERSON_INFO_MULTIPLE_FACE"},
	{PERSON_INFO_FACE_SIZE_LARGER,		"TEXT_PERSON_INFO_FACE_SIZE_ERROR"},
	{PERSON_INFO_FACE_SIZE_SMALLER,		"TEXT_PERSON_INFO_FACE_SIZE_ERROR"},
	{PERSON_INFO_IMAGE_DECODE_FAIL,		"TEXT_PERSON_INFO_IMAGE_DECODE_FAIL"},
	{PERSON_INFO_PIC_COPY_ERROR,		"TEXT_PERSON_INFO_PIC_COPY_ERROR"},
	{PERSON_INFO_INSERT_DB_ERROR,		"TEXT_PERSON_INFO_INSERT_DB_ERROR"},
	{PERSON_INFO_FACE_QUALITY_TOO_LOW,	"TEXT_PERSON_INFO_FACE_QUALITY_TOO_LOW"},
	{PERSON_INFO_FACE_SIMILARITY_TOO_HIGH,
					"TEXT_PERSON_INFO_FACE_SIMILARITY_TOO_HIGH"},
	{PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT,
					"TEXT_PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT"},
	{PERSON_INFO_IMAGE_FORMAT_ERROR,	"TEXT_PERSON_INFO_IMAGE_FORMAT_ERROR"},
	{PERSON_INFO_INTERNAL_ERROR,		"TEXT_PERSON_INFO_INTERNAL_ERROR"},
	{PERSON_REGISTRATION_ERROR,		"TEXT_PERSON_REGISTRATION_ERROR"},
	{PERSON_MIS_RECOGNIZED,			"TEXT_PERSON_MIS_RECOGNIZED"},
	{PERSON_REG_COMPARE_RESULT_OK,		"TEXT_OK"},
	{PERSON_REG_COMPARE_RESULT_FAIL,	"TEXT_FAIL"}
};

#define REGISTER_PIC_SAVE_PATH		"/data/face_register"

/*
#define JPEG_FAST_DEC

static void *jpg_hdl;
*/

/**
 * person_is_already_register Check if this person is already registered
 * @p_name The name of the person who needs to be checked
 *
 * @return 0-if this person is not registered,
 *	   other-This person is already registered
 */
int person_is_already_register(char *p_name)
{
	#if 0
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
	#endif
	return 0;
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

	reg_error_mode_count = (sizeof(st_reg_info) /
		sizeof(person_reg_error_info_t));
	for (i = 0; i < reg_error_mode_count; i++) {
		if (st_reg_info[i].error_code == code) {
			snprintf(fail_reason_str, fail_reason_str_max_len, "%s",
				clang_str(st_reg_info[i].error_msg));
			break;
		}
	}

	return 0;
}

/**
 * ui_user_free - free queue of query person info.
 * @head: A point to 'ee_queue_t'
 *
 * Returns 0 if success, otherwise not 0.
 */
static int ui_user_free(ee_queue_t *head)
{
	if (NULL == head) {
		ALOGE("%s:head is null\n", __func__);
		return -1;
	}

	ee_queue_t *h = head, *pos, *n;
	websocket_person_info_t *p = NULL;
	ee_queue_for_each_safe(pos, n, h) {
		p = ee_queue_data(pos, websocket_person_info_t, queue);
		free(p);
	}
	ee_queue_init(h);

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
	int ret = -1;
	websocket_person_info_t *person_info = NULL;
	ee_queue_t *h = NULL;
	ee_queue_t head;
	h = &head;

	if (NULL == p_person_info) {
		ALOGE("p_info is null\n");
		return ret;
	}

	person_info = calloc(sizeof(websocket_person_info_t), 1);
	if (person_info == NULL) {
		ret = -2;
		return ret;
	}

	strncpy(person_info->user_id, p_person_info->employee_number,
			sizeof(p_person_info->employee_number));
	strncpy(person_info->name, p_person_info->name,
			sizeof(p_person_info->name));
	memcpy(person_info->face_feature, p_person_info->face_features,
			FACE_FEAT_SIZE);
	memcpy(person_info->key_points, p_person_info->face_key_points,
			sizeof(person_info->key_points));
	person_info->face_norm[0] = p_person_info->face_norm[0];
	person_info->face_norm[1] = p_person_info->face_norm[1];
	memcpy(person_info->pic_large, p_person_info->regist_photo_name,
			sizeof(person_info->pic_large));
	memcpy(person_info->access_card_number,
		p_person_info->access_card_number,
		sizeof(person_info->access_card_number));

	ee_queue_init(h);
	ee_queue_insert_tail(h, &(person_info->queue));
	ret = db_websocket_person_info_insert(h);
	ui_user_free(h);

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
	int ret;
	DIR *dir;
	face_info_t info;
	face_image_file_t image_file;
	face_feat_opt_t opt;
	char save_path[64], date[16], name[32];

	assert((p_person_info != NULL) &&
		(reg_img != NULL) && (err_code != NULL) &&
		(reg_param != NULL) && (detail_info != NULL));
	ret = db_ui_person_info_get_count(TYPE_PERSON) +
		db_ui_person_info_get_count(TYPE_VISITOR);
	if (MAX_FEAT_CNT < ret) {
		ALOGE("The count of person in the db is out of limit[%d]\n",
			MAX_FEAT_CNT);
		*err_code = PERSON_INFO_INSERT_DB_ERROR;
		return -1;
	}
	ret = time_get_cur_date(date);
	if (ret) {
		ALOGE("get date failed\n");
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		return -1;
	}

	snprintf(save_path, sizeof(save_path),
			"%s/%s", REGISTER_PIC_SAVE_PATH, date);

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

	*err_code = PERSON_REG_OK;
	memset(detail_info, 0, detail_info_size);

	memset(&opt, 0, sizeof(face_feat_opt_t));
	memset(&image_file, 0, sizeof(face_image_file_t));
	strcpy(image_file.image_path, reg_img->pic_path);
	image_file.hflip = h_flip;
	image_file.save_face = 0;
#if 0
	image_file.save_face = 1;
	snprintf(image_file.save_face_path,
			sizeof(image_file.save_face_path),
			"%s/%s-small.jpg",
			save_path,
			p_person_info->employee_number);
#endif
	ret = face_get_feats_from_image_file(get_face_handle(), &image_file, &opt);
	if (ret != 0) {
		ALOGE("face_get_feats_from_image_file() ret %d", ret);
		*err_code = ret;
		goto free_res;
	}

	memcpy(&info, &opt.info, sizeof(face_info_t));
	/* Update registration image file path.
	* Attention: regist_photo_name is a path without photo name.
	*/
	memset(p_person_info->regist_photo_name, 0,
				sizeof(p_person_info->regist_photo_name));

	snprintf(p_person_info->regist_photo_name,
		sizeof(p_person_info->regist_photo_name),
		"%s/%s-large.jpg",
		save_path, p_person_info->employee_number);

	ret = file_copy(reg_img->pic_path, p_person_info->regist_photo_name);
	if (ret) {
		ALOGE("copy file failed\n");
		*err_code = PERSON_INFO_INTERNAL_ERROR;
		goto free_res;
	}

	/* save feature, feature norm, face key points */
	memcpy(p_person_info->face_features, info.feat.values,
					sizeof(info.feat.values));
	p_person_info->face_norm[0] = info.feat.norm[0];
	p_person_info->face_norm[1] = info.feat.norm[1];
#if 0
	snprintf(p_person_info->face_key_points,
			sizeof(p_person_info->face_key_points),
			"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
			info.rect.kpts[0], info.rect.kpts[1], info.rect.kpts[2],
			info.rect.kpts[3], info.rect.kpts[4], info.rect.kpts[5],
			info.rect.kpts[6], info.rect.kpts[7], info.rect.kpts[8],
			info.rect.kpts[9]);
#else
	memcpy(p_person_info->face_key_points, info.rect.kpts,
			sizeof(info.rect.kpts));
#endif

	/* get recognition threshold */
	if (info.feat.score >= reg_param->threshold) {
		ALOGE("%s: face simularity(%f) is too high\n", __func__,
						info.feat.score);
		*err_code = PERSON_INFO_FACE_SIMILARITY_TOO_HIGH;
		file_remove(p_person_info->regist_photo_name);
		ret = face_db_get_name(info.feat.max_id, name, sizeof(name));
		if (ret) {
			ALOGE("%s: get name failed", __func__);
			snprintf(detail_info, detail_info_size, "%s(%f)",
					"get_name_failed", info.feat.score);
		} else
			snprintf(detail_info, detail_info_size, "%s(%f)", name,
					info.feat.score);
		goto free_res;
	}

	/** If the registration is successful:
	 * Write the personnel information to the database.
	 */
	ret = person_add_to_db(p_person_info);
	if (ret != 0) {
		*err_code = PERSON_INFO_INSERT_DB_ERROR;
		file_remove(p_person_info->regist_photo_name);
		goto free_res;
	}

	if (reg_param->refresh_db_each_time) {
		ret = face_db_update();
		if (ret) {
			ALOGE("%s: update db failed", __func__);
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			file_remove(p_person_info->regist_photo_name);
			goto free_res;
		}
	}

free_res:
	return 0;
}

#if 0
/**
 * person_get_feats_from_data – Get features from jpeg data.
 * @jpeg_data IN, JPEG data.
 * @len IN, The length of the jpeg picture.
 * @check_size IN, If check the size of the face or not.
 * @feats OUT, Save the feature data of the face.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_get_feats_from_data(char *jpeg_data, int len,
		int check_size, char *feats)
{
	struct jpeg_dec_output	output;
	face_frame_t	frame;
	int			ret = 0;
	int			face_num = 0;
	face_info_t	*info = NULL;
	int			face_max;
	int			face_min;
	int			face_width;
	int			face_height;
	int			w_stride = 0;
	int			h_stride = 0;
#ifdef CHECK_SIMULARITY
	float		threshold = 0.0;
#endif

	if ((NULL == jpeg_data) || (NULL == feats))
		return PERSON_REG_REFUSE;

#if 0
	w_stride = config_get_int("register", "jpeg_w_stride", 800);
	h_stride = config_get_int("register", "jpeg_h_stride", 1024);
#endif
	w_stride = 800;
	h_stride = 1024;

#ifdef JPEG_FAST_DEC
	if (jpg_hdl == NULL) {
		jpg_hdl = jpeg_fast_dec_init();
		if (jpg_hdl == NULL) {
			ALOGE("jpeg fast dec init failed");
			return PERSON_INFO_IMAGE_DECODE_FAIL;
		}
	}
	ret = jpeg_fast_dec_data(jpeg_data, len, w_stride, h_stride,
					JPEG_SCALE_AUTO, &output, jpg_hdl);
#else
	ret = jpeg_dec_data(jpeg_data, len, w_stride, h_stride,
					JPEG_SCALE_AUTO, &output);
#endif
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
#ifdef JPEG_FAST_DEC
		jpeg_fast_dec_end(&output);
#else
		jpeg_dec_end(&output);
#endif
		return PERSON_INFO_INTERNAL_ERROR;
	}

	/* check face num */
	if (face_num == 0) {
		ALOGE("%s: no face is detected\n", __func__);
		if (info) {
			free(info);
			info = NULL;
		}
#ifdef JPEG_FAST_DEC
		jpeg_fast_dec_end(&output);
#else
		jpeg_dec_end(&output);
#endif
		return PERSON_INFO_NO_DETECT_FACE;
	} else if (face_num > 1) {
		ALOGE("%s: more than one face(%d) are detected\n", __func__, face_num);
		if (info) {
			free(info);
			info = NULL;
		}
#ifdef JPEG_FAST_DEC
		jpeg_fast_dec_end(&output);
#else
		jpeg_dec_end(&output);
#endif
		return PERSON_INFO_MULTIPLE_FACE;
	}

	/* check face size */
	if (check_size != 0) {
#if 0
		face_max = config_get_int("register", "face_max", 512);
		face_min = config_get_int("register", "face_min", 128);
#endif
		face_max = 512;
		face_min = 128;
		face_width = info->rect.rect[2] - info->rect.rect[0];
		face_height = info->rect.rect[3] - info->rect.rect[1];
		if (face_width > face_max || face_height > face_max) {
			ALOGE("face size(%dx%d) > face max(%d)",
				face_width, face_height, face_max);
			if (info) {
				free(info);
				info = NULL;
			}
#ifdef JPEG_FAST_DEC
			jpeg_fast_dec_end(&output);
#else
			jpeg_dec_end(&output);
#endif
			return PERSON_INFO_FACE_SIZE_LARGER;
		}
		if (face_width < face_min || face_height < face_min) {
			ALOGE("face size(%dx%d) < face min(%d)",
				face_width, face_height, face_min);
			if (info) {
				free(info);
				info = NULL;
			}
#ifdef JPEG_FAST_DEC
			jpeg_fast_dec_end(&output);
#else
			jpeg_dec_end(&output);
#endif

			return PERSON_INFO_FACE_SIZE_SMALLER;
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
#ifdef JPEG_FAST_DEC
		jpeg_fast_dec_end(&output);
#else
		jpeg_dec_end(&output);
#endif
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
#ifdef JPEG_FAST_DEC
		jpeg_fast_dec_end(&output);
#else
		jpeg_dec_end(&output);
#endif
		return PERSON_INFO_FACE_SIMILARITY_TOO_HIGH;
	}
#endif

	/* save feature, feature norm, face key points */
	memcpy(feats, info->feat.values, sizeof(info->feat.values));
	memcpy(&feats[FACE_FEAT_SIZE], info->rect.kpts, 10 * sizeof(float));
	memcpy(&feats[FACE_FEAT_SIZE + 10 * sizeof(float)],
		&info->feat.norm, sizeof(float));

	if (info) {
		free(info);
		info = NULL;
	}
#ifdef JPEG_FAST_DEC
	jpeg_fast_dec_end(&output);
#else
	jpeg_dec_end(&output);
#endif

	return PERSON_REG_OK;
}

/**
 * person_get_feats_from_frame – Get features from frame data.
 * @jpeg_data IN, frame data.
 * @feats OUT, Save the feature data of the face.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_get_feats_from_frame(face_frame_t *frame_data, char *feats)
{
	face_info_t		*info = NULL;
	int			face_num = 0;
	int			face_max;
	int			face_min;
	int			face_width;
	int			face_height;
#ifdef CHECK_SIMULARITY
	float			threshold = 0.0;
#endif

	if (NULL == feats) {
		ALOGE("input parameter invalid");
		return PERSON_INFO_INTERNAL_ERROR;
	}

	info = (face_info_t *)malloc(sizeof(face_info_t));
	if (!info) {
		ALOGE("malloc face_info_t failed");
		return PERSON_INFO_INTERNAL_ERROR;
	}

	face_num = face_recg_block(get_face_handle(), frame_data, 1, info);
	if (face_num < 0) {
		ALOGE("%s: face recg block failed\n", __func__);
		if (info) {
			free(info);
			info = NULL;
		}
		return PERSON_INFO_INTERNAL_ERROR;
	}
#if 0
	face_max = config_get_int("register", "face_max", 512);
	face_min = config_get_int("register", "face_min", 128);
#endif
	face_max = 512;
	face_min = 128;
	face_width = info->rect.rect[2] - info->rect.rect[0];
	face_height = info->rect.rect[3] - info->rect.rect[1];
	if (face_width > face_max || face_height > face_max) {
		ALOGE("face size(%dx%d) > face max(%d)",
			face_width, face_height, face_max);
		if (info) {
			free(info);
			info = NULL;
		}
		return PERSON_INFO_FACE_SIZE_LARGER;
	}
	if (face_width < face_min || face_height < face_min) {
		ALOGE("face size(%dx%d) < face min(%d)",
			face_width, face_height, face_min);
		if (info) {
			free(info);
			info = NULL;
		}
		return PERSON_INFO_FACE_SIZE_SMALLER;
	}

	/* check face quality */
	if (info->rect.quality_score < 0) {
		ALOGE("%s: face quality(%d) is too low\n", __func__,
						info->rect.quality_score);
		if (info) {
			free(info);
			info = NULL;
		}
		return PERSON_INFO_FACE_QUALITY_TOO_LOW;
	}
#if CHECK_SIMULARITY
	threshold = config_get_double("register", "reg_threshold", 0.5f);
	/* fix me: get recognition threshold */
	if (info->feat.score >= threshold) {
		ALOGE("%s: face simularity(%f) is too high\n", __func__,
						info->feat.score);
		if (info) {
			free(info);
			info = NULL;
		}
		return PERSON_INFO_FACE_SIMILARITY_TOO_HIGH;
	}
#endif
	/* save feature, feature norm, face key points */
	memcpy(feats, info->feat.values, sizeof(info->feat.values));
	memcpy(&feats[FACE_FEAT_SIZE], info->rect.kpts, 10 * sizeof(float));
	memcpy(&feats[FACE_FEAT_SIZE + 10 * sizeof(float)],
		&info->feat.norm, sizeof(float));

	if (info) {
		free(info);
		info = NULL;
	}

	return PERSON_REG_OK;
}
#endif

/**
 * person_recognition_proc – Processing person recognition interface.
 * @p_person_info IN, Person registration information
 * @reg_img IN, Image information for preson registration
 * @err_code OUT, Error code returned when person registration fails
 * @recg_name OUT, When the error type is "false recognition", the name
 * of the person who was misidentified is stored.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_recognition_proc(person_t *p_person_info, img_info_t *reg_img,
	e_person_reg_code *err_code, char *detail_info, int detail_size,
	person_recg_param_t *recg_param)
{
	int ret;
	char name[64];
	face_image_file_t image_file;
	face_info_t info;
	face_feat_opt_t opt;

	assert((p_person_info != NULL) && (reg_img != NULL)
		&& (err_code != NULL) && recg_param != NULL);

	*err_code = PERSON_REG_OK;
	memset(detail_info, 0, detail_size);

	ALOGD("recognition photo path:%s", reg_img->pic_path);
	memset(&image_file, 0, sizeof(face_image_file_t));
	memset(&opt, 0, sizeof(face_feat_opt_t));
	strcpy(image_file.image_path, reg_img->pic_path);
	image_file.hflip = 1;
	image_file.save_face = 0;
	ret = face_get_feats_from_image_file(get_face_handle(), &image_file, &opt);
	ALOGE("%s: face_get_feats_from_image_file ret = %d\n", __func__, ret);
	if (ret != 0) {
		ALOGE("face_get_feats_from_image_file() ret %d", ret);
		*err_code = ret;
		return -1;
	}

	memcpy(&info, &opt.info, sizeof(face_info_t));

	/* fix me: get recognition threshold */
	if (info.feat.score >= recg_param->threshold) {
		ALOGD("%s: pass(%f)\n", __func__, info.feat.score);
		ret = face_db_get_name(info.feat.max_id, name, sizeof(name));
		if (ret) {
			ALOGE("get name from db failed, max_id:%d", info.feat.max_id);
			*err_code = PERSON_INFO_INTERNAL_ERROR;
			goto free_res;
		}
		if (strncmp(name, p_person_info->name, sizeof(name))) {
			/* mis-recognition occurs,
			* recognised name is not equal to name in excel table
			*/
			*err_code = PERSON_MIS_RECOGNIZED;
			snprintf(detail_info, 64, "%s(%f)", name, info.feat.score);
			goto free_res;
		} else {
			snprintf(detail_info, 64, "%f", info.feat.score);
			*err_code = PERSON_REG_OK;
		}
	} else {
		ALOGD("%s: refuse(%f)\n", __func__, info.feat.score);
		snprintf(detail_info, 64, "%f", info.feat.score);
		*err_code = PERSON_REG_REFUSE;
	}

free_res:
	if (*err_code != PERSON_REG_OK && *err_code != PERSON_REG_REFUSE)
		return -1;

	return 0;
}

