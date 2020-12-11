/*
 * Copyright (C) 2019 lombo Tech CO.,LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.lombo.app.faceid;

public class RegisterInfo {
  private static final String PERSON_REG_OK = "成功";		/* OK */
  private static final String PERSON_REG_REFUSE = "人脸被拒绝";		/* Face is refused */
  private static final String PERSON_INFO_DATA_ERROR = "人脸数据有误";		/* Personnel information data error */
  private static final String PERSON_INFO_NO_DETECT_FACE = "没有检测到人脸";	/* Unable to detect face */
  private static final String PERSON_INFO_FACE_FEATURE_FAIL = "人脸特征值有误";	/* Face feature value extratcion failed */
  private static final String PERSON_INFO_MULTIPLE_FACE = "检测到多个人脸";	/* Checked multiple faces */
  private static final String PERSON_INFO_FACE_SIZE_ERROR = "检查面部尺寸错误";	/* Checked face size */
  private static final String PERSON_INFO_IMAGE_DECODE_FAIL = "人脸图像解码失败";	/* Image decoding failed */
  private static final String PERSON_INFO_PIC_COPY_ERROR = "图像复制错误";	/* Picture error */
  private static final String PERSON_INFO_PIC_NO_HAVE = "没有图像";	/* No picture */
  private static final String PERSON_INFO_INSERT_DB_ERROR = "写入数据库错误";	/* Personnel information failed to write to the database */

  private static final String PERSON_INFO_FACE_QUALITY_TOO_LOW = "人脸像素过低"; /* Face quality is too low */
  private static final String PERSON_INFO_FACE_SIMILARITY_TOO_HIGH = "脸部相似度过高"; /* Face similarity is too high */
  private static final String PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT = "超过一个注册限制"; /* More than a single registration limit */

  private static final String PERSON_INFO_IMAGE_FORMAT_ERROR = "图像格式有误";	/* Image format does not meet the requirements */
  private static final String PERSON_INFO_INTERNAL_ERROR = "应用内部错误";	/* App internal error */
  private static final String PERSON_REGISTRATION_ERROR = "重复注册错误";	/* Repeated registration error */
  private static final String PERSON_MIS_RECOGNIZED = "有多个人脸";	/* Recognized as someone else */
  private static final String PERSON_REG_COMPARE_RESULT_OK = "比较结果正确";	/* The result of the comparison is OK */
  private static final String PERSON_REG_COMPARE_RESULT_FAIL = "比较结果错误";	/* The result of the comparison is FAIL */

  private static final String[] REGISTER_INFO = {
          PERSON_REG_OK,
          PERSON_REG_REFUSE,		/* Face is refused */
          PERSON_INFO_DATA_ERROR,		/* Personnel information data error */
          PERSON_INFO_NO_DETECT_FACE,	/* Unable to detect face */
          PERSON_INFO_FACE_FEATURE_FAIL,	/* Face feature value extratcion failed */
          PERSON_INFO_MULTIPLE_FACE,	/* Checked multiple faces */
          PERSON_INFO_FACE_SIZE_ERROR,	/* Checked face size */
          PERSON_INFO_IMAGE_DECODE_FAIL,	/* Image decoding failed */
          PERSON_INFO_PIC_COPY_ERROR,	/* Picture error */
          PERSON_INFO_PIC_NO_HAVE,	/* No picture */
          PERSON_INFO_INSERT_DB_ERROR,	/* Personnel information failed to write to the database */

          PERSON_INFO_FACE_QUALITY_TOO_LOW, /* Face quality is too low */
          PERSON_INFO_FACE_SIMILARITY_TOO_HIGH, /* Face similarity is too high */
          PERSON_INFO_MORE_THAN_SINGLE_REG_LIMIT, /* More than a single registration limit */

          PERSON_INFO_IMAGE_FORMAT_ERROR,	/* Image format does not meet the requirements */
          PERSON_INFO_INTERNAL_ERROR,	/* App internal error */
          PERSON_REGISTRATION_ERROR,	/* Repeated registration error */
          PERSON_MIS_RECOGNIZED,	/* Recognized as someone else */
          PERSON_REG_COMPARE_RESULT_OK,	/* The result of the comparison is OK */
          PERSON_REG_COMPARE_RESULT_FAIL,	/* The result of the comparison is FAIL */
  };

  public static String getRegisterInfo(int result) {
    return REGISTER_INFO[result];
  }

  private static final String PERSON_UNREG_OK = "注销成功";		/* OK */
  private static final String PERSON_HAVE_NOT = "没有该用户";    /*  */
  private static final String PERSON_UNREG_FAIL = "注销失败";

  private static final String[] UNREGISTER_INFO = {
          PERSON_UNREG_OK,
          PERSON_HAVE_NOT,
          PERSON_UNREG_FAIL
  };

  public static String getUnregisterInfo(int result) {
    return UNREGISTER_INFO[result];
  }

}
