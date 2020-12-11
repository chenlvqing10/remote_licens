/*
 * rgb2nir_core.cpp - code for rotate component.
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

//#pragma warning(disable:4996)
#include <random>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <vector>
#include <string.h>
#include <dirent.h>
#include <sstream>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#define LOG_TAG "rgb2nir_core"
#include <log/log.h>

using namespace std;
using namespace cv;

#include "rgb2nir_util.hpp"
#include "rgb2nir_core.hpp"

double mean(vector<double> list)
{
  double sum = 0;
  for (unsigned int i = 0; i < list.size(); i++) {
    sum += list[i];
  }
  return sum / list.size();
}

int cal_ord_map(string data_path, Size& boardSize, Mat& x_map, Mat& y_map, double vec_grids_per_face, int flip_en)
{
  ALOGV("enter %s", __func__);

  int img_num = 100;
  long long start_time = 0;
  long long end_time = 0;

  vector<string> imgs_lst_nir;
  vector<string> imgs_lst_rgb;

  int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
  std::default_random_engine random_engine;
  random_engine.seed(time(0));
  std::uniform_real_distribution<double> uniform(0.85, 1.2);

  getFiles2(data_path, imgs_lst_nir, "nir");
  for (unsigned int i = 0; i < imgs_lst_nir.size(); i++)
    ALOGV("nir img: %s", imgs_lst_nir[i].c_str());

  getFiles2(data_path, imgs_lst_rgb, "rgb");
  for (unsigned int i = 0; i < imgs_lst_rgb.size(); i++)
    ALOGV("rgb img: %s", imgs_lst_rgb[i].c_str());

  if (imgs_lst_nir.size() != imgs_lst_rgb.size() && imgs_lst_rgb.size() > 0) {
    ALOGE("imgs not equal nir: %d rgb: %d", imgs_lst_nir.size(), imgs_lst_rgb.size());
    return -1;
  }

  img_num = std::min(int(imgs_lst_rgb.size()), img_num);
  vector<double> vec_xin, vec_yin, vec_win, vec_hin, vec_x2in, vec_y2in, vec_w2in, vec_h2in;
  vector<double> vec_xyin, vec_xhin, vec_xwin, vec_yhin, vec_ywin, vec_hwin;
  vector<double> vec_xout, vec_yout, vec_xxout, vec_yxout, vec_wxout, vec_hxout, vec_xyout, vec_wyout, vec_hyout, vec_yyout;

  for (int i = 0; i < img_num; i++) {
    Mat img_rgb = imread(imgs_lst_rgb[i]);
    Mat img_nir = imread(imgs_lst_nir[i]);

    if (flip_en) {
      cv::flip(img_rgb, img_rgb, 1);
      cv::flip(img_nir, img_nir, 1);
    }

    Mat gray_rgb, gray_nir;
    cvtColor(img_rgb, gray_rgb, COLOR_BGR2GRAY);
    cvtColor(img_nir, gray_nir, COLOR_BGR2GRAY);
    int found = 0;
    vector<Point2f> pointBuf_rgb, pointBuf_nir;

    start_time = gettime_us();
    found = findChessboardCorners(gray_rgb, boardSize, pointBuf_rgb, chessBoardFlags);
    end_time = gettime_us();
    ALOGE("%s findChessboardCorners found %d img %s us: %lld pointBuf_rgb.size() = %d\n",
        __func__, found, imgs_lst_rgb[i].c_str(), end_time - start_time, pointBuf_rgb.size());
    if (found == 0) {
        ALOGE("%s findChessboardCorners err", __func__);
        continue;
    }

    start_time = gettime_us();
    found = findChessboardCorners(gray_nir, boardSize, pointBuf_nir, chessBoardFlags);
    end_time = gettime_us();
    ALOGE("%s findChessboardCorners found %d img %s us: %lld pointBuf_nir.size() = %d\n",
        __func__, found, imgs_lst_nir[i].c_str(), end_time - start_time, pointBuf_nir.size());
    if (found == 0) {
        ALOGE("%s findChessboardCorners err", __func__);
        continue;
    }

    Point2f p0 = pointBuf_rgb[0];
    Point2f p1 = pointBuf_rgb[pointBuf_rgb.size() - 1];
    double h = abs(p0.y - p1.y) / (boardSize.height-1)*vec_grids_per_face;// / 8 * 5.5;
    //double w = abs(p0.x - p1.x);
    double w = h*0.88;

    for (unsigned int i = 0; i < pointBuf_rgb.size(); i++) {
      double x_in = pointBuf_rgb[i].x;
      double y_in = pointBuf_rgb[i].y;
      double x_out = pointBuf_nir[i].x;
      double y_out = pointBuf_nir[i].y;

      double h_in = h*uniform(random_engine);
      double w_in = w*uniform(random_engine);
      double scale = 1.0; //(h*w);

      vec_hin.push_back(h_in / scale);
      vec_win.push_back(w_in / scale);
      vec_h2in.push_back(h_in*h_in / scale);
      vec_w2in.push_back(w_in*w_in / scale);
      vec_hwin.push_back(h_in*w_in / scale);

      vec_xin.push_back(x_in / scale);
      vec_yin.push_back(y_in / scale);
      vec_x2in.push_back(x_in*x_in / scale);
      vec_y2in.push_back(y_in*y_in / scale);

      vec_xyin.push_back(x_in*y_in / scale);
      vec_xhin.push_back(x_in*h_in / scale);
      vec_xwin.push_back(x_in*w_in / scale);
      vec_yhin.push_back(y_in*h_in / scale);
      vec_ywin.push_back(y_in*w_in / scale);

      vec_xout.push_back(x_out / scale);
      vec_yout.push_back(y_out / scale);
      vec_xxout.push_back(x_in*x_out / scale);
      vec_yxout.push_back(y_in*x_out / scale);
      vec_wxout.push_back(w_in*x_out / scale);
      vec_hxout.push_back(h_in*x_out / scale);
      vec_yyout.push_back(y_in*y_out / scale);
      vec_xyout.push_back(x_in*y_out / scale);
      vec_wyout.push_back(w_in*y_out / scale);
      vec_hyout.push_back(h_in*y_out / scale);
    }
  }

  double m_xin = mean(vec_xin), m_yin = mean(vec_yin), m_win = mean(vec_win), m_hin = mean(vec_hin), m_x2in = mean(vec_x2in), m_y2in = mean(vec_y2in), m_w2in = mean(vec_w2in), m_h2in = mean(vec_h2in);
  double m_xyin = mean(vec_xyin), m_xhin = mean(vec_xhin), m_xwin = mean(vec_xwin), m_yhin = mean(vec_yhin), m_ywin = mean(vec_ywin), m_hwin = mean(vec_hwin);
  double m_xxout = mean(vec_xxout), m_yxout = mean(vec_yxout), m_xout = mean(vec_xout), m_wxout = mean(vec_wxout), m_hxout = mean(vec_hxout);
  double m_yyout = mean(vec_yyout), m_xyout = mean(vec_xyout), m_yout = mean(vec_yout), m_wyout = mean(vec_wyout), m_hyout = mean(vec_hyout);

  Mat ir(5, 5, CV_64FC1), dst_x(5, 1, CV_64FC1), dst_y(5, 1, CV_64FC1);
  ir.at<double>(0, 0) = m_x2in, ir.at<double>(0, 1) = m_xyin, ir.at<double>(0, 2) = m_xin, ir.at<double>(0, 3) = m_xwin, ir.at<double>(0, 4) = m_xhin;
  ir.at<double>(1, 0) = m_xyin, ir.at<double>(1, 1) = m_y2in, ir.at<double>(1, 2) = m_yin, ir.at<double>(1, 3) = m_ywin, ir.at<double>(1, 4) = m_yhin;
  ir.at<double>(2, 0) = m_xin, ir.at<double>(2, 1) = m_yin, ir.at<double>(2, 2) = 1, ir.at<double>(2, 3) = m_win, ir.at<double>(2, 4) = m_hin;
  ir.at<double>(3, 0) = m_xwin, ir.at<double>(3, 1) = m_ywin, ir.at<double>(3, 2) = m_win, ir.at<double>(3, 3) = m_w2in, ir.at<double>(3, 4) = m_hwin;
  ir.at<double>(4, 0) = m_xhin, ir.at<double>(4, 1) = m_yhin, ir.at<double>(4, 2) = m_hin, ir.at<double>(4, 3) = m_hwin, ir.at<double>(4, 4) = m_h2in;

  dst_x.at<double>(0) = m_xxout, dst_x.at<double>(1) = m_yxout, dst_x.at<double>(2) = m_xout, dst_x.at<double>(3) = m_wxout, dst_x.at<double>(4) = m_hxout;
  dst_y.at<double>(0) = m_xyout, dst_y.at<double>(1) = m_yyout, dst_y.at<double>(2) = m_yout, dst_y.at<double>(3) = m_wyout, dst_y.at<double>(4) = m_hyout;
  x_map = ir.inv()*dst_x;
  y_map = ir.inv()*dst_y;

  return 0;
}

int modify_ord_map(string data_path, double lr, int lr_cnt_thr, Size& boardSize, Mat& x_map, Mat& y_map, double vec_grids_per_face, int flip_en)
{
    ALOGV("enter %s", __func__);

    int img_num = 100;
    long long start_time = 0;
    long long end_time = 0;

    vector<string> imgs_lst_nir;
    vector<string> imgs_lst_rgb;

    int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
    std::default_random_engine random_engine;
    random_engine.seed(time(0));
    std::uniform_real_distribution<double> uniform(0.85, 1.2);

    getFiles2(data_path, imgs_lst_nir, "nir");
    for (unsigned int i = 0; i < imgs_lst_nir.size(); i++)
        ALOGV("nir img: %s", imgs_lst_nir[i].c_str());

    getFiles2(data_path, imgs_lst_rgb, "rgb");
    for (unsigned int i = 0; i < imgs_lst_rgb.size(); i++)
        ALOGV("rgb img: %s", imgs_lst_rgb[i].c_str());

    if (imgs_lst_nir.size() != imgs_lst_rgb.size() && imgs_lst_rgb.size() > 0) {
        ALOGE("imgs not equal nir: %d rgb: %d", imgs_lst_nir.size(), imgs_lst_rgb.size());
        return -1;
    }

    img_num = std::min(int(imgs_lst_rgb.size()), img_num);
    vector<double> vec_xmap0, vec_xmap1, vec_xmap2, vec_xmap3, vec_xmap4;
    vector<double> vec_ymap0, vec_ymap1, vec_ymap2, vec_ymap3, vec_ymap4;
    vector<double> vec_x_in, vec_y_in, vec_w_in, vec_h_in, vec_x_out, vec_y_out;

    for (int i = 0; i < img_num; i++) {
        Mat img_rgb = imread(imgs_lst_rgb[i]);
        Mat img_nir = imread(imgs_lst_nir[i]);

        if (flip_en) {
            cv::flip(img_rgb, img_rgb, 1);
            cv::flip(img_nir, img_nir, 1);
        }

        Mat gray_rgb, gray_nir;
        cvtColor(img_rgb, gray_rgb, COLOR_BGR2GRAY);
        cvtColor(img_nir, gray_nir, COLOR_BGR2GRAY);
        int found = 0;
        vector<Point2f> pointBuf_rgb, pointBuf_nir;

        start_time = gettime_us();
        found = findChessboardCorners(gray_rgb, boardSize, pointBuf_rgb, chessBoardFlags);
        end_time = gettime_us();
        ALOGE("%s findChessboardCorners found %d img %s us: %lld pointBuf_rgb.size() = %d\n",
            __func__, found, imgs_lst_rgb[i].c_str(), end_time - start_time, pointBuf_rgb.size());
        if (found == 0) {
            ALOGE("%s findChessboardCorners err", __func__);
            return -1;
        }

        start_time = gettime_us();
        found = findChessboardCorners(gray_nir, boardSize, pointBuf_nir, chessBoardFlags);
        end_time = gettime_us();
        ALOGE("%s findChessboardCorners found %d img %s us: %lld pointBuf_nir.size() = %d\n",
            __func__, found, imgs_lst_nir[i].c_str(), end_time - start_time, pointBuf_nir.size());
        if (found == 0) {
            ALOGE("%s findChessboardCorners err", __func__);
            return -1;
        }

        Point2f p0 = pointBuf_rgb[0];
        Point2f p1 = pointBuf_rgb[pointBuf_rgb.size() - 1];
        double h = abs(p0.y - p1.y) / (boardSize.height - 1)*vec_grids_per_face;
        double w = h*0.88;

        for (unsigned int i = 0; i < pointBuf_rgb.size(); i++) {
            double x_in = pointBuf_rgb[i].x;
            double y_in = pointBuf_rgb[i].y;
            double h_in = h*uniform(random_engine);
            double w_in = w*uniform(random_engine);
            double x_out_map = x_map.at<double>(0)*x_in + x_map.at<double>(1)*y_in + x_map.at<double>(2) + x_map.at<double>(3)*w_in + x_map.at<double>(4)*h_in;
            double y_out_map = y_map.at<double>(0)*x_in + y_map.at<double>(1)*y_in + y_map.at<double>(2) + y_map.at<double>(3)*w_in + y_map.at<double>(4)*h_in;
            double x_out = pointBuf_nir[i].x;
            double y_out = pointBuf_nir[i].y;
            double del_x_out = x_out_map - x_out;
            double del_y_out = y_out_map - y_out;

            vec_x_in.push_back(x_in);
            vec_y_in.push_back(y_in);
            vec_w_in.push_back(w_in);
            vec_h_in.push_back(h_in);
            vec_x_out.push_back(x_out);
            vec_y_out.push_back(y_out);

            vec_xmap0.push_back(del_x_out / x_in);
            vec_xmap1.push_back(del_x_out / y_in);
            vec_xmap2.push_back(del_x_out);
            vec_xmap3.push_back(del_x_out / w_in);
            vec_xmap4.push_back(del_x_out / h_in);

            vec_ymap0.push_back(del_y_out / x_in);
            vec_ymap1.push_back(del_y_out / y_in);
            vec_ymap2.push_back(del_y_out);
            vec_ymap3.push_back(del_y_out / w_in);
            vec_ymap4.push_back(del_y_out / h_in);
        }
    }

    Mat x_map_t, y_map_t;
    double loss_old = 0, loss_new = 0;

    x_map.copyTo(x_map_t);
    y_map.copyTo(y_map_t);
    int lr_cnt = 0;

    start_time = gettime_us();
    while (((loss_new <= loss_old) || (loss_old == 0)) && (lr_cnt<lr_cnt_thr)) {
        loss_old = loss_new;
        loss_new = 0;
        x_map_t.copyTo(x_map);
        y_map_t.copyTo(y_map);
        for (unsigned int i = 0; i < vec_x_in.size(); i++) {
            double x_in = vec_x_in[i];
            double y_in = vec_y_in[i];
            double h_in = vec_h_in[i];
            double w_in = vec_w_in[i];
            double x_out_map = x_map_t.at<double>(0)*x_in + x_map_t.at<double>(1)*y_in + x_map_t.at<double>(2) + x_map_t.at<double>(3)*w_in + x_map_t.at<double>(4)*h_in;
            double y_out_map = y_map_t.at<double>(0)*x_in + y_map_t.at<double>(1)*y_in + y_map_t.at<double>(2) + y_map_t.at<double>(3)*w_in + y_map_t.at<double>(4)*h_in;
            double x_out = vec_x_out[i];
            double y_out = vec_y_out[i];
            double del_x_out = x_out_map - x_out;
            double del_y_out = y_out_map - y_out;

            loss_new += del_x_out*del_x_out + del_y_out*del_y_out;
        }

        if (loss_new <= loss_old || loss_old == 0) {
            x_map_t.at<double>(0) = x_map.at<double>(0) - lr*mean(vec_xmap0);
            x_map_t.at<double>(1) = x_map.at<double>(1) - lr*mean(vec_xmap1);
            x_map_t.at<double>(2) = x_map.at<double>(2) - lr*mean(vec_xmap2);
            x_map_t.at<double>(3) = x_map.at<double>(3) - lr*mean(vec_xmap3);
            x_map_t.at<double>(4) = x_map.at<double>(4) - lr*mean(vec_xmap4);

            y_map_t.at<double>(0) = y_map.at<double>(0) - lr*mean(vec_ymap0);
            y_map_t.at<double>(1) = y_map.at<double>(1) - lr*mean(vec_ymap1);
            y_map_t.at<double>(2) = y_map.at<double>(2) - lr*mean(vec_ymap2);
            y_map_t.at<double>(3) = y_map.at<double>(3) - lr*mean(vec_ymap3);
            y_map_t.at<double>(4) = y_map.at<double>(4) - lr*mean(vec_ymap4);
            lr_cnt++;
        }
    }
    end_time = gettime_us();
    ALOGE("%s learn us: %lld\n", __func__, end_time - start_time);

    return 0;
}
