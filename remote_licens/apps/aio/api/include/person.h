/*
 * person.h - Public interface for personnel management
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

#ifndef _PERSON_H
#define _PERSON_H

#include "face_svc.h"
#include "common_person_info.h"

/* Registered person's small picture suffix */
#define SMALL_PICTURE_SUFFIX	"small.jpg"

/* Registered person's large picture suffix */
#define LARGE_PICTURE_SUFFIX	"large.jpg"

/* person info */
typedef struct tag_person {
	/* Employee mumber(E.g SW200) */
	char employee_number[32];

	/* Name */
	char name[32];

	/* Gender(E.g "Man/Women/..." */
	char gender[8];

	/* Nationa */
	char nationa[32];

	/* ID card number */
	char id_card_number[32];

	/* Telephone number */
	char mobile[20];

	/* Department name(E.g "Software Lab001") */
	char department_name[128];

	/* Access right
	 * Options:
	 *	0: Free access
	 *	1: Temporary access mode 1
	 *	2: Temporary access mode 2
	 */
	int access_right;

	/* When the permission is temporary access permission mode 1,
	 * Allowed start time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	long temporary_access_start_time;

	/* When the permission is temporary access permission mode 1,
	 * Allowed end time
	 * The time display format is "yyyy/mm/dd hh:mm:ss"
	 */
	long temporary_access_end_time;

	/* Allowed number of passes
	 * When the permission is temporary access permission mode 2
	 */
	int temporary_access_times;

	/* Access card number.
	 * Effective when the punch mode is "Card reader"
	 */
	char access_card_number[32];

	/* Remarks: Used to fill in the description information */
	char remarks[128];

	/* Register photo name.(Including jpg suffix) */
	char regist_photo_name[128];

	/* Face feature value. Used to store facial feature values */
	unsigned char face_features[FACE_FEATURES_DATA_LENGTH];

	/* face key points */
	char face_key_points[256];

	/* norm */
	double face_norm[2];
} person_t;

/* Image information for preson registration */
typedef struct tag_img_info {
	/* The path to store the large picture registered by the person */
	char pic_path[128];

	/* The path of the small picture of the face registered by the person */
	char face_path[128];
} img_info_t;

/** Failed error code when importing personnel registration information */
typedef enum {
	PERSON_REG_OK = 0,		/* OK */
	PERSON_REG_REFUSE,		/* Face is refused */
	PERSON_INFO_PIC_NO_HAVE,	/* No picture */
	PERSON_INFO_IMAGE_DECODE_FAIL,	/* Image decoding failed */
	PERSON_INFO_DATA_ERROR,		/* Personnel information data error */
	PERSON_INFO_NO_DETECT_FACE,	/* Unable to detect face */
	PERSON_INFO_MULTIPLE_FACE,	/* Checked multiple faces */
	PERSON_INFO_PIC_SIZE_LARGER,	/*  size to too larger */
	PERSON_INFO_FACE_SIZE_LARGER,	/* Checked face size larger */
	PERSON_INFO_FACE_SIZE_SMALLER,	/* Checked face size smaller */
	PERSON_INFO_FACE_QUALITY_TOO_LOW, /* Face quality is too low */
	PERSON_INFO_FACE_FEATURE_FAIL,	/* Face feature value extratcion failed */

	PERSON_INFO_PIC_COPY_ERROR,	/* Picture error */

	PERSON_INFO_INSERT_DB_ERROR,	/* Personnel information failed to
					 * write to the database
					 */
	PERSON_INFO_FACE_SIMILARITY_TOO_HIGH, /* Face similarity is too high */
	PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT, /* More than a single registration
						 * limit
						 */
	PERSON_INFO_IMAGE_FORMAT_ERROR, /* Image format does not
				meet the requirements */
	PERSON_INFO_INTERNAL_ERROR,	/* App internal error */
	PERSON_REGISTRATION_ERROR,	/* Repeated registration error */
	PERSON_MIS_RECOGNIZED,	/* Recognized as someone else */
	PERSON_REG_COMPARE_RESULT_OK,	/* The result of the comparison is OK */
	PERSON_REG_COMPARE_RESULT_FAIL,	/* The result of the comparison is FAIL */
} e_person_reg_code;

typedef struct tag_person_reg_error_info_t {
	e_person_reg_code error_code;
	const char *error_msg; /* The index string correspoonding to elang.el */
} person_reg_error_info_t;

typedef struct person_reg_param {
	int batch_reg;		/* batch picture registration */
	int check_face_size;	/* check face size */
	int face_max;		/* maximum size of face */
	int face_min;		/* minimum size of face */
	float threshold;	/* threshold of simularity */
	int quality;		/* quality of face */
	int refresh_db_each_time;
				/* update database for each successful registration,
				mainly used for debug */
	int jpeg_width_stride;	/* width stride size of jpeg dec */
	int jpeg_height_stride;	/* height stride size of jpeg dec */
} person_reg_param_t;

typedef struct person_recg_param {
	float threshold;	/* threshold of simularity */
	int jpeg_width_stride;	/* width stride size of jpeg dec */
	int jpeg_height_stride;	/* height stride size of jpeg dec */
} person_recg_param_t;

/**
 * person_is_already_register Check if this person is already registered
 * @p_name The name of the person who needs to be checked
 * @return 0-if this person is not registered,
 *	   other-This person is already registered
 */
int person_is_already_register(char *p_name);

/**
 * person_add_to_db - Write personnel information to the database.
 * @p_person_info A point to 'person_t'
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_add_to_db(person_t *p_person_info);

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
	unsigned int fail_reason_str_max_len);

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
	e_person_reg_code * err_code, int h_flip);

#if 0
/**
 * person_get_feats_from_data – Get face features from
 * face picture (jpeg) data.
 * @jpeg_data IN, jpeg data include face.
 * @len IN, The length of the jpeg data.
 * @check_size IN, If check the size of the face or not.
 * @feats OUT, Save the feature data of the face.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_get_feats_from_data(
			char *jpeg_data, int len, int check_size, char *feats);

/**
 * person_get_feats_from_frame – Get features from frame data.
 * @jpeg_data IN, frame data.
 * @feats OUT, Save the feature data of the face.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_get_feats_from_frame(face_frame_t *frame_data, char *feats);
#endif

/**
 * person_recognition_proc – Processing person recognition interface.
 * @p_person_info IN, Person registration information
 * @reg_img IN, Image information for preson registration
 * @err_code OUT, Error code returned when person registration fails
 * @recg_name OUT, When the error type is "false recognition",
 * the name of the person who was misidentified is stored.
 *
 * Returns 0 if success, otherwise not 0.
 */
int person_recognition_proc(person_t *p_person_info, img_info_t *reg_img,
	e_person_reg_code *err_code, char *detail_info, int detail_size,
	person_recg_param_t *recg_param);

#endif /* _PERSON_H */

