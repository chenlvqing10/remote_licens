/*
 * rgb2nir.cpp - code for rotate component.
 *
 * Copyright (C) 2019-2020, LomboTech Co.Ltd.
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

#include <random>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <vector>
#include <sstream>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#define LOG_TAG "RGB2NIR"
#include <log/log.h>

using namespace std;
using namespace cv;

#include "rgb2nir_core.hpp"

extern "C" {

#include "rgb2nir.h"

int get_modify_ord_map_params(get_mod_ord_map_t *get_mod_ord_map)
{
  int ret = -1;
  if(!get_mod_ord_map) {
    ALOGE("%s get_mod_ord_map error", __func__);
    return -1;
  }

  ALOGI("mod_path = %s", get_mod_ord_map->mod_path);
  ALOGI("mod_grid_face_scale = %lf", get_mod_ord_map->mod_grid_face_scale);
  ALOGI("mod_width = %d", get_mod_ord_map->mod_width);
  ALOGI("mod_height = %d", get_mod_ord_map->mod_height);
  ALOGI("lr = %lf", get_mod_ord_map->lr);
  ALOGI("lr_cnt_thr = %d", get_mod_ord_map->lr_cnt_thr);
  ALOGI("flip = %d", get_mod_ord_map->flip);

  int i = 0;
  Mat x_map(5, 1, CV_64FC1), y_map(5, 1, CV_64FC1);

  ALOGI("ord_map");
  ALOGI("x_map:");
  for (i = 0; i < 5; i++) {
    x_map.at<double>(i) = get_mod_ord_map->ord_map->x_map[i];
    ALOGI("%lf\t", x_map.at<double>(i));
  }

  ALOGI("y_map:");
  for (i = 0; i < 5; i++) {
    y_map.at<double>(i) = get_mod_ord_map->ord_map->y_map[i];
    ALOGI("%lf\t", y_map.at<double>(i));
  }

  Size boardSize(get_mod_ord_map->mod_width, get_mod_ord_map->mod_height);

  ret = modify_ord_map(get_mod_ord_map->mod_path,
    get_mod_ord_map->lr,
    get_mod_ord_map->lr_cnt_thr,
    boardSize,
    x_map,
    y_map,
    get_mod_ord_map->mod_grid_face_scale,
    get_mod_ord_map->flip);
  if (ret) {
      ALOGE("%s modify_ord_map error", __func__);
      return -1;
  }

  ALOGI("mod_map");
  ALOGI("x_map:");
  for (i = 0; i < 5; i++) {
    get_mod_ord_map->mod_map->x_map[i] = x_map.at<double>(i);
    ALOGI("%lf\t", x_map.at<double>(i));
  }
  ALOGI("y_map:");
  for (i = 0; i < 5; i++) {
    get_mod_ord_map->mod_map->y_map[i] = y_map.at<double>(i);
    ALOGI("%lf\t", y_map.at<double>(i));
  }
  return 0;
}

int get_ord_map_params(get_ord_map_t *get_ord_map)
{
  int ret = -1;
  if(!get_ord_map) {
    ALOGE("%s get_ord_map error", __func__);
    return -1;
  }

  ALOGI("ord_path = %s", get_ord_map->ord_path);
  ALOGI("ord_grid_face_scale = %lf", get_ord_map->ord_grid_face_scale);
  ALOGI("ord_width = %d", get_ord_map->ord_width);
  ALOGI("ord_hight = %d", get_ord_map->ord_hight);
  ALOGI("flip = %d", get_ord_map->flip);

  int i = 0;
  Mat x_map(5, 1, CV_64FC1), y_map(5, 1, CV_64FC1);

  Size boardSize(get_ord_map->ord_width, get_ord_map->ord_hight);

  ret = cal_ord_map(get_ord_map->ord_path,
    boardSize,
    x_map,
    y_map,
    get_ord_map->ord_grid_face_scale,
    get_ord_map->flip);
  if (ret) {
      ALOGE("%s cal_ord_map error", __func__);
      return -1;
  }

  ALOGI("ord_map");
  ALOGI("x_map:");
  for (i = 0; i < 5; i++) {
    get_ord_map->ord_map->x_map[i] = x_map.at<double>(i);
    ALOGI("%lf\t", x_map.at<double>(i));
  }
  ALOGI("y_map:");
  for (i = 0; i < 5; i++) {
    get_ord_map->ord_map->y_map[i] = y_map.at<double>(i);
    ALOGI("%lf\t", y_map.at<double>(i));
  }
   return 0;
}

int get_map_params(get_map_t *get_map)
{
  int ret = -1;
  if(!get_map) {
    ALOGE("%s get_map error", __func__);
    return -1;
  }

  ALOGI("ord_path = %s", get_map->ord_path);
  ALOGI("ord_grid_face_scale = %lf", get_map->ord_grid_face_scale);
  ALOGI("ord_width = %d", get_map->ord_width);
  ALOGI("ord_hight = %d", get_map->ord_hight);
  ALOGI("mod_path = %s", get_map->mod_path);
  ALOGI("mod_grid_face_scale = %lf", get_map->mod_grid_face_scale);
  ALOGI("mod_width = %d", get_map->mod_width);
  ALOGI("mod_height = %d", get_map->mod_height);
  ALOGI("lr = %lf", get_map->lr);
  ALOGI("lr_cnt_thr = %d", get_map->lr_cnt_thr);
  ALOGI("flip = %d", get_map->flip);

  int i = 0;
  Mat x_map(5, 1, CV_64FC1), y_map(5, 1, CV_64FC1);

  Size ord_boardSize(get_map->ord_width, get_map->ord_hight);
  Size mod_boardSize(get_map->mod_width, get_map->mod_height);

  ret = cal_ord_map(get_map->ord_path,
    ord_boardSize,
    x_map,
    y_map,
    get_map->ord_grid_face_scale,
    get_map->flip);
  if (ret) {
      ALOGE("%s cal_ord_map error", __func__);
      return -1;
  }

  ALOGI("ord_map");
  ALOGI("x_map:");
  for (i = 0; i < 5; i++) {
    ALOGI("%lf\t", x_map.at<double>(i));
  }
  ALOGI("y_map:");
  for (i = 0; i < 5; i++) {
    ALOGI("%lf\t", y_map.at<double>(i));
  }

 ret = modify_ord_map(get_map->mod_path,
    get_map->lr,
    get_map->lr_cnt_thr,
    mod_boardSize,
    x_map,
    y_map,
    get_map->mod_grid_face_scale,
    get_map->flip);
  if (ret) {
    ALOGE("%s modify_ord_map error", __func__);
    return -1;
  }

  ALOGI("mod_map");
  ALOGI("x_map:");
  for (i = 0; i < 5; i++) {
    get_map->mod_map->x_map[i] = x_map.at<double>(i);
    ALOGI("%lf\t", x_map.at<double>(i));
  }
  ALOGI("y_map:");
  for (i = 0; i < 5; i++) {
    get_map->mod_map->y_map[i] = y_map.at<double>(i);
    ALOGI("%lf\t", y_map.at<double>(i));
  }
  return 0;
}

}
