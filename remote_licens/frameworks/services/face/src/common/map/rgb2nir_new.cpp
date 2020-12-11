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

#define LOG_TAG "rgb2nir_new"
#include <log/log.h>

extern "C" {
#include <string.h>
#include <dirent.h>
#include "face_svc.h"
#include "file_ops.h"
#include "face_svc_conf.h"
}

#include "rgb2nir_new.h"
#include "rgb2nir_util.hpp"

#define RGB2NIR_NEW_DEBUG 1

using namespace cv;
using namespace std;

static int r2n_get_point_from(struct cal_img_t *cal_img);
static int r2n_add_rgb_point(struct img_point_t *img_point);
static int r2n_add_nir_point(struct img_point_t *img_point);
static int r2n_take_ord_pic(void);
static int r2n_take_mod_pic(int x, int y, int w, int h);
static int r2n_cal_ord_map(struct map_t *map);
static int r2n_cal_mod_map(struct map_t *map);
static int r2n_cal_reset_point(void);

struct cal_iface_t default_iface = {
	.rgb_point = NULL,
	.nir_point = NULL,
	.get_point_from = r2n_get_point_from,
	.add_rgb_point = r2n_add_rgb_point,
	.add_nir_point = r2n_add_nir_point,
	.take_ord_pic = r2n_take_ord_pic,
	.take_mod_pic= r2n_take_mod_pic,
	.cal_ord_map= r2n_cal_ord_map,
	.cal_mod_map= r2n_cal_mod_map,
	.cal_reset_point = r2n_cal_reset_point,
};

struct cal_iface_t *cal_iface = &default_iface;

static void r2n_create_map_pararm(char *path, map_t *map)
{
	FILE *ini = NULL;

	ini = fopen(path, "wb");
	if (!ini) {
		printf("fopen map_params fail\n");
		return;
	}

	fprintf(ini,
			"[map]\n"
			"x_map_0=%lf\n"
			"x_map_1=%lf\n"
			"x_map_2=%lf\n"
			"x_map_3=%lf\n"
			"x_map_4=%lf\n"
			"y_map_0=%lf\n"
			"y_map_1=%lf\n"
			"y_map_2=%lf\n"
			"y_map_3=%lf\n"
			"y_map_4=%lf\n",
			map->x_map[0],
			map->x_map[1],
			map->x_map[2],
			map->x_map[3],
			map->x_map[4],
			map->y_map[0],
			map->y_map[1],
			map->y_map[2],
			map->y_map[3],
			map->y_map[4]
				);

			fclose(ini);
			return;
}

double mean(vector<double> list, int bit=0)
{
	double sum = 0;
	for (unsigned int i = 0; i < list.size(); i++)
	{
		sum += list[i];
	}

	double rd = pow(10, bit);
	return round(sum / list.size()*rd) / rd;
}

void r2n_cal_map(vector<vector<Point2f>> vec_pointBuf_rgb, vector<vector<Point2f>> vec_pointBuf_nir, vector<double> vec_grids_per_face, Size boardSize, Mat& x_map, Mat& y_map, int mode)
{
	std::default_random_engine random_engine;
	random_engine.seed(time(0));
	std::uniform_real_distribution<double> uniform(0.85, 1.2);
	vector<Point2f> pointBuf_rgb, pointBuf_nir;
	int round_bit = 4;
	if (mode == 0)
	{
		vector<double> vec_xin, vec_yin, vec_win, vec_hin, vec_x2in, vec_y2in, vec_w2in, vec_h2in;
		vector<double> vec_xyin, vec_xhin, vec_xwin, vec_yhin, vec_ywin, vec_hwin;
		vector<double> vec_xout, vec_yout, vec_xxout, vec_yxout, vec_wxout, vec_hxout, vec_xyout, vec_wyout, vec_hyout, vec_yyout;

		for (unsigned int i = 0; i < vec_pointBuf_rgb.size(); i++)
		{
			pointBuf_rgb = vec_pointBuf_rgb[i];
			pointBuf_nir = vec_pointBuf_nir[i];
			Point2f p0 = pointBuf_rgb[0];
			Point2f p1 = pointBuf_rgb[pointBuf_rgb.size() - 1];
			double h = abs(p0.y - p1.y) / boardSize.height*vec_grids_per_face[i];
			double w = h*0.88;

			ALOGD_IF(RGB2NIR_NEW_DEBUG, "p0.y %lf, p1.y  %lf, h %d, v %lf\n", p0.y , p1.y, boardSize.height , vec_grids_per_face[0]);
			for (unsigned int i = 0; i < pointBuf_rgb.size(); i++)
			{
				double x_in = pointBuf_rgb[i].x;
				double y_in = pointBuf_rgb[i].y;
				double x_out = pointBuf_nir[i].x;
				double y_out = pointBuf_nir[i].y;

				ALOGD_IF(RGB2NIR_NEW_DEBUG, "x:%lf  y:%lf\n", x_in, y_in);
				double hu = uniform(random_engine);
				double h_in = h*hu;
				double wu = uniform(random_engine);
				double w_in = w*wu;
				double scale = 1.0; //(h*w);
				ALOGD_IF(RGB2NIR_NEW_DEBUG, "h %lf, hu %lf, w %lf, wu %lf\n", h, hu, w, wu);
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

		double m_xin = mean(vec_xin, round_bit), m_yin = mean(vec_yin, round_bit), m_win = mean(vec_win, round_bit), m_hin = mean(vec_hin, round_bit), m_x2in = mean(vec_x2in, round_bit), m_y2in = mean(vec_y2in, round_bit), m_w2in = mean(vec_w2in, round_bit), m_h2in = mean(vec_h2in, round_bit);
		double m_xyin = mean(vec_xyin, round_bit), m_xhin = mean(vec_xhin, round_bit), m_xwin = mean(vec_xwin, round_bit), m_yhin = mean(vec_yhin, round_bit), m_ywin = mean(vec_ywin, round_bit), m_hwin = mean(vec_hwin, round_bit);
		double m_xxout = mean(vec_xxout, round_bit), m_yxout = mean(vec_yxout, round_bit), m_xout = mean(vec_xout, round_bit), m_wxout = mean(vec_wxout, round_bit), m_hxout = mean(vec_hxout, round_bit);
		double m_yyout = mean(vec_yyout, round_bit), m_xyout = mean(vec_xyout, round_bit), m_yout = mean(vec_yout, round_bit), m_wyout = mean(vec_wyout, round_bit), m_hyout = mean(vec_hyout, round_bit);

		Mat ir(5, 5, CV_64FC1), dst_x(5, 1, CV_64FC1), dst_y(5, 1, CV_64FC1);
		ir.at<double>(0, 0) = m_x2in, ir.at<double>(0, 1) = m_xyin, ir.at<double>(0, 2) = m_xin, ir.at<double>(0, 3) = m_xwin, ir.at<double>(0, 4) = m_xhin;
		ir.at<double>(1, 0) = m_xyin, ir.at<double>(1, 1) = m_y2in, ir.at<double>(1, 2) = m_yin, ir.at<double>(1, 3) = m_ywin, ir.at<double>(1, 4) = m_yhin;
		ir.at<double>(2, 0) = m_xin, ir.at<double>(2, 1) = m_yin, ir.at<double>(2, 2) = 1, ir.at<double>(2, 3) = m_win, ir.at<double>(2, 4) = m_hin;
		ir.at<double>(3, 0) = m_xwin, ir.at<double>(3, 1) = m_ywin, ir.at<double>(3, 2) = m_win, ir.at<double>(3, 3) = m_w2in, ir.at<double>(3, 4) = m_hwin;
		ir.at<double>(4, 0) = m_xhin, ir.at<double>(4, 1) = m_yhin, ir.at<double>(4, 2) = m_hin, ir.at<double>(4, 3) = m_hwin, ir.at<double>(4, 4) = m_h2in;

		dst_x.at<double>(0) = m_xxout, dst_x.at<double>(1) = m_yxout, dst_x.at<double>(2) = m_xout, dst_x.at<double>(3) = m_wxout, dst_x.at<double>(4) = m_hxout;
		dst_y.at<double>(0) = m_xyout, dst_y.at<double>(1) = m_yyout, dst_y.at<double>(2) = m_yout, dst_y.at<double>(3) = m_wyout, dst_y.at<double>(4) = m_hyout;

		#if RGB2NIR_NEW_DEBUG
		for (int i = 0; i < ir.rows; i++)
		{
			for (int j = 0; j < ir.cols; j++)
				ALOGD("%lf\t", ir.at<double>(i, j));
			ALOGD("\n");
		}
		#endif

		ir = ir.inv();

		#if RGB2NIR_NEW_DEBUG
		for (int i = 0; i < ir.rows; i++)
		{
			for (int j = 0; j < ir.cols; j++)
				ALOGD("%lf\t", ir.at<double>(i, j));
			ALOGD("\n");
		}
		#endif

		x_map = ir*dst_x;
		y_map = ir*dst_y;

		vec_xin.clear();
		std::vector<double>().swap(vec_xin);
		vec_yin.clear();
		std::vector<double>().swap(vec_yin);
		vec_win.clear();
		std::vector<double>().swap(vec_win);
		vec_hin.clear();
		std::vector<double>().swap(vec_hin);
		vec_y2in.clear();
		std::vector<double>().swap(vec_y2in);
		vec_w2in.clear();
		std::vector<double>().swap(vec_w2in);
		vec_h2in.clear();
		std::vector<double>().swap(vec_h2in);

		vec_xyin.clear();
		std::vector<double>().swap(vec_xyin);
		vec_xhin.clear();
		std::vector<double>().swap(vec_xhin);
		vec_xwin.clear();
		std::vector<double>().swap(vec_xwin);
		vec_yhin.clear();
		std::vector<double>().swap(vec_yhin);
		vec_ywin.clear();
		std::vector<double>().swap(vec_ywin);
		vec_hwin.clear();
		std::vector<double>().swap(vec_hwin);

		vec_xout.clear();
		std::vector<double>().swap(vec_xout);
		vec_yout.clear();
		std::vector<double>().swap(vec_yout);
		vec_xxout.clear();
		std::vector<double>().swap(vec_xxout);
		vec_yxout.clear();
		std::vector<double>().swap(vec_yxout);
		vec_wxout.clear();
		std::vector<double>().swap(vec_wxout);
		vec_hxout.clear();
		std::vector<double>().swap(vec_hxout);
		vec_xyout.clear();
		std::vector<double>().swap(vec_xyout);
		vec_wyout.clear();
		std::vector<double>().swap(vec_wyout);
		vec_hyout.clear();
		std::vector<double>().swap(vec_hyout);
		vec_yyout.clear();
		std::vector<double>().swap(vec_yyout);
	}
	else if (mode == 1) {
		int lr_cnt = 0;
		int lr_down_cnt = 0;
		int lr_down_cnt_thr = 5;
		double lr = 0.05;
		int lr_cnt_thr = 100;

		vector<double> vec_xmap0, vec_xmap1, vec_xmap2, vec_xmap3, vec_xmap4;
		vector<double> vec_ymap0, vec_ymap1, vec_ymap2, vec_ymap3, vec_ymap4;
		vector<double> vec_x_in, vec_y_in, vec_w_in, vec_h_in, vec_x_out, vec_y_out;

		Mat x_map_t, y_map_t, x_map_min, y_map_min;
		double loss_old = DBL_MAX, loss_new = DBL_MAX;

		x_map.copyTo(x_map_t);
		y_map.copyTo(y_map_t);
		x_map.copyTo(x_map_min);
		y_map.copyTo(y_map_min);

		while (lr_cnt<lr_cnt_thr)
		{
			if (loss_new >= loss_old)
			{
				lr_down_cnt++;
				if (lr_down_cnt > lr_down_cnt_thr)
				{
					lr = lr / 10;
					lr_down_cnt = 0;
				}
			}
			else {
				x_map_t.copyTo(x_map_min);
				y_map_t.copyTo(y_map_min);
			}

			loss_old = loss_new;
			loss_new = 0;
			int num = 0;
			x_map_t.copyTo(x_map);
			y_map_t.copyTo(y_map);

			for (unsigned int i = 0; i < vec_pointBuf_rgb.size(); i++)
			{
				pointBuf_rgb = vec_pointBuf_rgb[i];
				pointBuf_nir = vec_pointBuf_nir[i];
				Point2f p0 = pointBuf_rgb[0];
				Point2f p1 = pointBuf_rgb[pointBuf_rgb.size() - 1];
				double h = abs(p0.y - p1.y) / (boardSize.height - 1)*vec_grids_per_face[i];
				double w = h*0.88;

				for (unsigned int i = 0; i < pointBuf_rgb.size(); i++)
				{
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
				num += pointBuf_rgb.size();
			}

			x_map_t.at<double>(0) = x_map.at<double>(0) - lr*mean(vec_xmap0, round_bit);
			x_map_t.at<double>(1) = x_map.at<double>(1) - lr*mean(vec_xmap1, round_bit);
			x_map_t.at<double>(2) = x_map.at<double>(2) - lr*mean(vec_xmap2, round_bit);
			x_map_t.at<double>(3) = x_map.at<double>(3) - lr*mean(vec_xmap3, round_bit);
			x_map_t.at<double>(4) = x_map.at<double>(4) - lr*mean(vec_xmap4, round_bit);

			y_map_t.at<double>(0) = y_map.at<double>(0) - lr*mean(vec_ymap0, round_bit);
			y_map_t.at<double>(1) = y_map.at<double>(1) - lr*mean(vec_ymap1, round_bit);
			y_map_t.at<double>(2) = y_map.at<double>(2) - lr*mean(vec_ymap2, round_bit);
			y_map_t.at<double>(3) = y_map.at<double>(3) - lr*mean(vec_ymap3, round_bit);
			y_map_t.at<double>(4) = y_map.at<double>(4) - lr*mean(vec_ymap4, round_bit);

			for (unsigned int i = 0; i < vec_x_in.size(); i++)
			{
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

			vec_xmap0.clear();
			std::vector<double>().swap(vec_xmap0);
			vec_xmap1.clear();
			std::vector<double>().swap(vec_xmap1);
			vec_xmap2.clear();
			std::vector<double>().swap(vec_xmap2);
			vec_xmap3.clear();
			std::vector<double>().swap(vec_xmap3);
			vec_xmap4.clear();
			std::vector<double>().swap(vec_xmap4);

			vec_ymap0.clear();
			std::vector<double>().swap(vec_ymap0);
			vec_ymap1.clear();
			std::vector<double>().swap(vec_ymap1);
			vec_ymap2.clear();
			std::vector<double>().swap(vec_ymap2);
			vec_ymap3.clear();
			std::vector<double>().swap(vec_ymap3);
			vec_ymap4.clear();
			std::vector<double>().swap(vec_ymap4);

			vec_x_in.clear();
			std::vector<double>().swap(vec_x_in);
			vec_y_in.clear();
			std::vector<double>().swap(vec_y_in);
			vec_w_in.clear();
			std::vector<double>().swap(vec_w_in);
			vec_h_in.clear();
			std::vector<double>().swap(vec_h_in);
			vec_x_out.clear();
			std::vector<double>().swap(vec_x_out);
			vec_y_out.clear();
			std::vector<double>().swap(vec_y_out);

			if (lr_cnt%5==0)
				ALOGE("iter:%d, L2 loss: %lf, lr: %lf\n", lr_cnt, loss_new/num, lr);

			lr_cnt++;
		}

		x_map_min.copyTo(x_map);
		y_map_min.copyTo(y_map);
	}

	//cal lost
	double loss = 0;
	int num = 0;
	for (unsigned int i = 0; i < vec_pointBuf_rgb.size(); i++)
	{
		pointBuf_rgb = vec_pointBuf_rgb[i];
		pointBuf_nir = vec_pointBuf_nir[i];
		Point2f p0 = pointBuf_rgb[0];
		Point2f p1 = pointBuf_rgb[pointBuf_rgb.size() - 1];
		double h = abs(p0.y - p1.y) / (boardSize.height - 1)*vec_grids_per_face[i];
		double w = h*0.88;

		for (unsigned int i = 0; i < pointBuf_rgb.size(); i++)
		{
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

			loss += del_x_out*del_x_out + del_y_out*del_y_out;
		}
		num += pointBuf_rgb.size();
	}

	ALOGD("x_map:\n");
	for (int i = 0; i < 5; i++)
		ALOGD("%lf\t", x_map.at<double>(i));
	ALOGD("\n");

	ALOGD("y_map:\n");
	for (int i = 0; i < 5; i++)
		ALOGD("%lf\t", y_map.at<double>(i));
	ALOGD("\n");
	ALOGD("L2 loss: %lf\n", loss/num);
}

static int r2n_get_point_from(struct cal_img_t *cal_img)
{
	int found = 0;
	int size = 0;
	long long start_time = 0;
	long long end_time = 0;
	char circle_path[128] = {0};

	std::string img = cal_img->path;
	cv::Size boardSize(cal_img->row, cal_img->col);
	int flags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;
	cv::Mat bgr, gray;
	std::vector<cv::Point2f> pointBuf;

	bgr = cv::imread(img);
	if (bgr.empty()) {
		ALOGE("%s empty", __func__);
		goto error;
	}

	cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);

	if (cal_img->x || cal_img->y || cal_img->w || cal_img->h) {
		cv::Rect rect(cal_img->x, cal_img->y, cal_img->w, cal_img->h);
		cv::Mat mask(bgr.rows, bgr.cols, CV_8UC1, cv::Scalar(0));
		mask(rect) = cv::Scalar(1);
		gray = gray.mul(mask) + (Mat(bgr.rows, bgr.cols, CV_8UC1, Scalar(1)) - mask) * 255;
		cv::imwrite("/data/test.jpg", gray);
	}

	start_time = gettime_us();
	found = cv::findChessboardCorners(gray, boardSize, pointBuf, flags);
	end_time = gettime_us();
	size = pointBuf.size();
	ALOGE("%s findChessboardCorners found %d img %s us: %lld size = %d\n",
			__func__, found, img.c_str(), end_time - start_time, size);
	if ((found == 0) || (size != cal_img->row * cal_img->col)) {
		ALOGE("%s findChessboardCorners err", __func__);
		goto error;
	}

	cornerSubPix( gray, pointBuf, boardSize, Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));

	#if RGB2NIR_NEW_DEBUG
	for (unsigned int i = 0; i < pointBuf.size(); i++)
	{
		double x_in = pointBuf[i].x;
		double y_in = pointBuf[i].y;

		circle(bgr, CvPoint(x_in, y_in), 1, CvScalar(0, 0, 255));
	}

	sprintf(circle_path, "/data/cir_%lld.jpg", end_time);
	imwrite(circle_path, bgr);

	for (unsigned int i = 0; i < pointBuf.size(); i++) {
		ALOGE("x: %lf y: %lf\n", pointBuf[i].x, pointBuf[i].y);
	}
	ALOGE("\n");
	ALOGE("\n");
	#endif

	cal_img->img_point = (struct img_point_t *)malloc(sizeof(struct img_point_t));
	if (!cal_img->img_point) {
		ALOGE("malloc img_point failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto error;
	}
	memset(cal_img->img_point, 0, sizeof(struct img_point_t));
	cal_img->img_point->count = size;
	cal_img->img_point->point = (struct point_t *)malloc(sizeof(struct point_t) * size);
	if (!cal_img->img_point->point) {
		ALOGE("malloc point failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto error;
	}
	memset(cal_img->img_point->point, 0, sizeof(struct point_t) * size);
	for (unsigned int i = 0; i < pointBuf.size(); i++) {
		struct point_t * point = &cal_img->img_point->point[i];
		point->x = pointBuf[i].x;
		point->y = pointBuf[i].y;
	}

	pointBuf.clear();
	std::vector<cv::Point2f>().swap(pointBuf);

	return 0;

error:
	if (cal_img->img_point) {

		ALOGE("malloc img_point failed, errno: %d (%s)\n",
				errno, strerror(errno));

		if (cal_img->img_point->point) {
			free(cal_img->img_point->point);
			cal_img->img_point->point = NULL;
		}
		free(cal_img->img_point);
		cal_img->img_point = NULL;
	}
	return -1;
}

static int r2n_add_rgb_point(struct img_point_t *img_point)
{
	struct img_point_t *tmp = NULL;

	if(cal_iface->rgb_point == NULL) {
		cal_iface->rgb_point = img_point;
	} else {
		tmp = cal_iface->rgb_point;
		cal_iface->rgb_point = img_point;
		img_point->next = tmp;
	}
	return 0;
}

static int r2n_add_nir_point(struct img_point_t *img_point)
{
	struct img_point_t *tmp = NULL;

	if(cal_iface->nir_point == NULL) {
		cal_iface->nir_point = img_point;
	} else {
		tmp = cal_iface->nir_point;
		cal_iface->nir_point = img_point;
		img_point->next = tmp;
	}
	return 0;
}

static int r2n_take_ord_pic(void)
{
	int ret = -1;
	char rgb[256] = {0};
	char nir[256] = {0};
	long long cur_time = gettime_us();
	struct cal_img_t cal_rgb_img = {0};
	struct cal_img_t cal_nir_img = {0};

	char ord_path[] = "/data/ord_img";

	if (is_dir_exist(ord_path))
		mk_mul_dirs(ord_path);

	sprintf(rgb, "/data/ord_img/rgb-%lld.jpg", cur_time);
	sprintf(nir, "/data/ord_img/nir-%lld.jpg", cur_time);

	ret = face_take_picture_file(FACE_CAMERA_RGB, rgb);
	if (ret) {
		goto exit;
	}
	ret = face_take_picture_file(FACE_CAMERA_NIR, nir);
	if (ret) {
		goto exit;
	}

	/* rgb */
	cal_rgb_img.path = rgb;
	cal_rgb_img.row = face_svc_conf->svc.map_ord_width;
	cal_rgb_img.col = face_svc_conf->svc.map_ord_height;
	cal_rgb_img.ratio = face_svc_conf->svc.map_ord_grid_face;
	ret = r2n_get_point_from(&cal_rgb_img);
	if (ret) {
		goto exit;
	}

	/* nir */
	cal_nir_img.path = nir;
	cal_nir_img.row = face_svc_conf->svc.map_ord_width;
	cal_nir_img.col = face_svc_conf->svc.map_ord_height;
	cal_nir_img.ratio = face_svc_conf->svc.map_ord_grid_face;
	ret = r2n_get_point_from(&cal_nir_img);
	if (ret) {
		goto exit;
	}

	r2n_add_rgb_point(cal_rgb_img.img_point);
	r2n_add_nir_point(cal_nir_img.img_point);

	return 0;

exit:

	if(cal_rgb_img.img_point) {
		if(cal_rgb_img.img_point->point) {
			free(cal_rgb_img.img_point->point);
			cal_rgb_img.img_point->point = NULL;
		}
		free(cal_rgb_img.img_point);
		cal_rgb_img.img_point = NULL;
	}
	if(cal_nir_img.img_point) {
		if(cal_nir_img.img_point->point) {
			free(cal_nir_img.img_point->point);
			cal_nir_img.img_point->point = NULL;
		}
		free(cal_nir_img.img_point);
		cal_nir_img.img_point = NULL;
	}

	return ret;
}

static int r2n_take_mod_pic(int x, int y, int w, int h)
{
	int ret = -1;
	char rgb[256] = {0};
	char nir[256] = {0};
	long long cur_time = gettime_us();
	struct cal_img_t cal_rgb_img = {0};
	struct cal_img_t cal_nir_img = {0};

	char mod_path[] = "/data/mod_img";

	if (is_dir_exist(mod_path))
		mk_mul_dirs(mod_path);

	sprintf(rgb, "/data/mod_img/rgb-%lld.jpg", cur_time);
	sprintf(nir, "/data/mod_img/nir-%lld.jpg", cur_time);

	ret = face_take_picture_file(FACE_CAMERA_RGB, rgb);
	if (ret) {
		goto exit;
	}
	ret = face_take_picture_file(FACE_CAMERA_NIR, nir);
	if (ret) {
		goto exit;
	}

	/* rgb */
	cal_rgb_img.path = rgb;
	cal_rgb_img.row = face_svc_conf->svc.map_mod_width;
	cal_rgb_img.col = face_svc_conf->svc.map_mod_height;
	cal_rgb_img.ratio = face_svc_conf->svc.map_mod_grid_face;
	cal_rgb_img.x = x;
	cal_rgb_img.y = y;
	cal_rgb_img.w = w;
	cal_rgb_img.h = h;
	ret = r2n_get_point_from(&cal_rgb_img);
	if (ret) {
		goto exit;
	}

	/* nir */
	cal_nir_img.path = nir;
	cal_nir_img.row = face_svc_conf->svc.map_mod_width;
	cal_nir_img.col = face_svc_conf->svc.map_mod_height;
	cal_nir_img.ratio = face_svc_conf->svc.map_mod_grid_face;
	ret = r2n_get_point_from(&cal_nir_img);
	if (ret) {
		goto exit;
	}

	r2n_add_rgb_point(cal_rgb_img.img_point);
	r2n_add_nir_point(cal_nir_img.img_point);

	return 0;

exit:
	if(cal_rgb_img.img_point) {
		if(cal_rgb_img.img_point->point) {
			free(cal_rgb_img.img_point->point);
			cal_rgb_img.img_point->point = NULL;
		}
		free(cal_rgb_img.img_point);
		cal_rgb_img.img_point = NULL;
	}
	if(cal_nir_img.img_point) {
		if(cal_nir_img.img_point->point) {
			free(cal_nir_img.img_point->point);
			cal_nir_img.img_point->point = NULL;
		}
		free(cal_nir_img.img_point);
		cal_nir_img.img_point = NULL;
	}
	return ret;
}

static int r2n_cal_ord_map(struct map_t *map)
{
	int i=0;
	vector<vector<Point2f>> vec_pointBuf_rgb, vec_pointBuf_nir;
	vector<Point2f> pointBuf_rgb, pointBuf_nir;
	Point2f point_rgb, point_nir;
	vector<double> vec_grids_per_face;
	Size boardSize = { face_svc_conf->svc.map_mod_width, face_svc_conf->svc.map_mod_height }; //w, h
	struct img_point_t *rgb_point = NULL;
	struct img_point_t *nir_point = NULL;

	if (!cal_iface->rgb_point || !cal_iface->nir_point) {
		ALOGE(" rgb_point is null or nir_point is null");
		return -1;
	}

	for (rgb_point = cal_iface->rgb_point; rgb_point != NULL; rgb_point = rgb_point->next) {
		vec_grids_per_face.push_back(face_svc_conf->svc.map_ord_grid_face);
		for (i = 0; i < rgb_point->count; i++) {
			point_rgb.x = rgb_point->point[i].x;
			point_rgb.y = rgb_point->point[i].y;
			pointBuf_rgb.push_back(point_rgb);
		}
		ALOGD("pointBuf_rgb.size: %d \n", pointBuf_rgb.size());
		vec_pointBuf_rgb.push_back(pointBuf_rgb);
		pointBuf_rgb.clear();
		std::vector<Point2f>().swap(pointBuf_rgb);
	}

	for (nir_point = cal_iface->nir_point; nir_point != NULL; nir_point = nir_point->next) {
		for (i = 0; i < nir_point->count; i++) {
			point_nir.x = nir_point->point[i].x;
			point_nir.y = nir_point->point[i].y;
			pointBuf_nir.push_back(point_nir);
		}
		ALOGD("pointBuf_nir.size: %d \n", pointBuf_nir.size());
		vec_pointBuf_nir.push_back(pointBuf_nir);
		pointBuf_nir.clear();
		std::vector<Point2f>().swap(pointBuf_nir);
	}

	ALOGD("vec_grids_per_face.size: %d \n", vec_grids_per_face.size());
	ALOGD("vec_pointBuf_rgb.size: %d \n", vec_pointBuf_rgb.size());
	ALOGD("vec_pointBuf_nir.size: %d \n", vec_pointBuf_nir.size());

	Mat x_map(5, 1, CV_64FC1), y_map(5, 1, CV_64FC1);
	r2n_cal_map(vec_pointBuf_rgb, vec_pointBuf_nir, vec_grids_per_face, boardSize, x_map, y_map, 0);

	ALOGI("mod_map");
	ALOGI("x_map:");
	for (i = 0; i < 5; i++) {
		map->x_map[i] = x_map.at<double>(i);
		ALOGI("%lf\t", x_map.at<double>(i));
	}
	ALOGI("y_map:");
	for (i = 0; i < 5; i++) {
		map->y_map[i] = y_map.at<double>(i);
		ALOGI("%lf\t", y_map.at<double>(i));
	}

	char ord_path[] = "/data/ord_map_params";
	r2n_create_map_pararm(ord_path, map);
	return 0;
}

typedef struct {
	union {int x; int col; };
	union {int y; int row; };
	union {int width; int cols; };
	union {int height; int rows; };
} IRECTC;

#define CLIPMIN(v, mn) {		\
	if ((v) < (mn))			\
	(v) = (mn);		\
}

#define CLIPMAX(v, mx) {		\
	if ((v) > (mx))			\
	(v) = (mx);		\
}

static void rect_rgb_to_nir2(Point2f &p0, double w, double h, IRECTC *out, struct map_t *map)
{
	int i = 0;
	double x_map[5] = {0};
	double y_map[5] = {0};
	int r_x0, r_y0, r_x1, r_y1, r_w, r_h, n_x0, n_y0, n_x1, n_y1;

	for (i = 0; i < 5; i++) {
		x_map[i] = map->x_map[i];
		y_map[i] = map->y_map[i];
	}

	r_x0 = p0.x;
	r_y0 = p0.y;
	r_x1 = p0.x + w;
	r_y1 = p0.y + h;
	r_w = w;
	r_h = h;

	n_x0 = x_map[0] * r_x0 + x_map[1] * r_y0 + x_map[2] + x_map[3] * r_w +
		x_map[4] * r_h;
	n_y0 = y_map[0] * r_x0 + y_map[1] * r_y0 + y_map[2] + y_map[3] * r_w +
		y_map[4] * r_h;

	n_x1 = x_map[0] * r_x1 + x_map[1] * r_y1 + x_map[2] + x_map[3] * r_w +
		x_map[4] * r_h;
	n_y1 = y_map[0] * r_x1 + y_map[1] * r_y1 + y_map[2] + y_map[3] * r_w +
		y_map[4] * r_h;

	out->x = n_x0;
	out->y = n_y0;

	out->width = n_x1 - n_x0;
	out->height = n_y1 - n_y0;

	CLIPMIN(out->y, 0);
	CLIPMAX(out->height, face_svc_conf->svc.nir_buffer_height - out->y);
	CLIPMIN(out->x, 0);
	CLIPMAX(out->width, face_svc_conf->svc.nir_buffer_width - out->x);
}

static int r2n_cal_mod_map(struct map_t *map)
{
	int i=0;
	vector<vector<Point2f>> vec_pointBuf_rgb, vec_pointBuf_nir;
	vector<Point2f> pointBuf_rgb, pointBuf_nir;
	Point2f point_rgb, point_nir;
	vector<double> vec_grids_per_face;

	vector<Point2f> verify_pointBuf_rgb, verify_pointBuf_nir;
	Size boardSize = { face_svc_conf->svc.map_mod_width, face_svc_conf->svc.map_mod_height }; //w, h

	struct img_point_t *rgb_point = NULL;
	struct img_point_t *nir_point = NULL;

	if (!cal_iface->rgb_point || !cal_iface->nir_point) {
		ALOGE(" rgb_point is null or nir_point is null");
		return -1;
	}

	for (rgb_point = cal_iface->rgb_point; rgb_point != NULL; rgb_point = rgb_point->next) {
		vec_grids_per_face.push_back(face_svc_conf->svc.map_mod_grid_face);
		for (i = 0; i < rgb_point->count; i++) {
			point_rgb.x = rgb_point->point[i].x;
			point_rgb.y = rgb_point->point[i].y;
			pointBuf_rgb.push_back(point_rgb);
		}
		ALOGD("pointBuf_rgb.size: %d \n", pointBuf_rgb.size());
		vec_pointBuf_rgb.push_back(pointBuf_rgb);
		pointBuf_rgb.clear();
		std::vector<Point2f>().swap(pointBuf_rgb);
	}

	for (nir_point = cal_iface->nir_point; nir_point != NULL; nir_point = nir_point->next) {
		for (i = 0; i < nir_point->count; i++) {
			point_nir.x = nir_point->point[i].x;
			point_nir.y = nir_point->point[i].y;
			pointBuf_nir.push_back(point_nir);
		}
		ALOGD("pointBuf_nir.size: %d \n", pointBuf_nir.size());
		vec_pointBuf_nir.push_back(pointBuf_nir);
		pointBuf_nir.clear();
		std::vector<Point2f>().swap(pointBuf_nir);
	}

	ALOGD("vec_grids_per_face.size: %d \n", vec_grids_per_face.size());
	ALOGD("vec_pointBuf_rgb.size: %d \n", vec_pointBuf_rgb.size());
	ALOGD("vec_pointBuf_nir.size: %d \n", vec_pointBuf_nir.size());

	Mat x_map(5, 1, CV_64FC1), y_map(5, 1, CV_64FC1);
	ALOGI("ord_map");
	ALOGI("x_map:");
	for (i = 0; i < 5; i++) {
		x_map.at<double>(i) = map->x_map[i];
		ALOGI("%lf\t", x_map.at<double>(i));
	}
	ALOGI("y_map:");
	for (i = 0; i < 5; i++) {
		y_map.at<double>(i) = map->y_map[i];
		ALOGI("%lf\t", y_map.at<double>(i));
	}
	r2n_cal_map(vec_pointBuf_rgb, vec_pointBuf_nir, vec_grids_per_face, boardSize, x_map, y_map, 0);

	ALOGI("mod_map");
	ALOGI("x_map:");
	for (i = 0; i < 5; i++) {
		map->x_map[i] = x_map.at<double>(i);
		ALOGI("%lf\t", x_map.at<double>(i));
	}
	ALOGI("y_map:");
	for (i = 0; i < 5; i++) {
		map->y_map[i] = y_map.at<double>(i);
		ALOGI("%lf\t", y_map.at<double>(i));
	}

	pointBuf_rgb = vec_pointBuf_rgb[0];
	Point2f rgb_p0 = pointBuf_rgb[0];
	Point2f rgb_p1 = pointBuf_rgb[pointBuf_rgb.size() - 1];
	double rgb_h = abs(rgb_p0.y - rgb_p1.y) / (boardSize.height - 1)*vec_grids_per_face[0];
	double rgb_w = rgb_h*0.88;
	IRECTC out;
	ALOGI("pointBuf_rgb.size(): %d\n", pointBuf_rgb.size());
	ALOGI("rgb_p0.x: %lf\n", rgb_p0.x);
	ALOGI("rgb_p0.y: %lf\n", rgb_p0.y);
	ALOGI("rgb_p1.x: %lf\n", rgb_p1.x);
	ALOGI("rgb_p1.y: %lf\n", rgb_p1.y);

	rect_rgb_to_nir2(rgb_p0, rgb_w, rgb_h, &out, map);

	pointBuf_nir = vec_pointBuf_nir[0];
	Point2f nir_p0 = pointBuf_nir[0];
	Point2f nir_p1 = pointBuf_nir[pointBuf_nir.size() - 1];
	double nir_h = abs(nir_p0.y - nir_p1.y) / (boardSize.height - 1)*vec_grids_per_face[0];
	double nir_w = nir_h*0.88;
	ALOGI("pointBuf_nir.size(): %d\n", pointBuf_nir.size());
	ALOGI("nir_p0.x: %lf\n", nir_p0.x);
	ALOGI("nir_p0.y: %lf\n", nir_p0.y);
	ALOGI("nir_p1.x: %lf\n", nir_p1.x);
	ALOGI("nir_p1.y: %lf\n", nir_p1.y);

	ALOGI("out %d, %d, %d, %d\n", out.x, out.y, out.width, out.height);
	ALOGI("nir %lf, %lf, %lf, %lf\n", nir_p0.x, nir_p0.y, nir_w, nir_h);

	/* expand 0.6 times */
	int dx = (int)(out.width * 0.3f);
	int dy = (int)(out.height * 0.3f);
	out.x = out.x - dx;
	out.y = out.y - dy;
	out.width = out.width + 2 * dx;
	out.height = out.height + 2 * dy;
	ALOGI("0.6 * out %d, %d, %d, %d\n", out.x, out.y, out.width, out.height);

	if (out.x > nir_p0.x || out.y > nir_p0.y || out.width < nir_w || out.height < nir_h) {
		ALOGE("0.6 * out err\n");
		return -1;
	}

	char mod_path[] = "/data/mod_map_params";
	r2n_create_map_pararm(mod_path, map);
	return 0;
}

static int r2n_cal_reset_point(void)
{
	struct img_point_t *tmp = NULL;
	struct img_point_t *rgb_point = NULL;
	for (rgb_point = cal_iface->rgb_point; rgb_point != NULL; rgb_point = tmp) {
		tmp = rgb_point->next;
		free(rgb_point->point);
		free(rgb_point);
	}
	cal_iface->rgb_point = NULL;

	struct img_point_t *nir_point = NULL;
	for (nir_point = cal_iface->nir_point; nir_point != NULL; nir_point = tmp) {
		tmp = nir_point->next;
		free(nir_point->point);
		free(nir_point);
	}
	cal_iface->nir_point = NULL;
	return 0;
}

int r2n_test_pic(void)
{
#if 1
	int ret = -1;
	char rgb[256] = {0};
	char nir[256] = {0};
	struct cal_img_t cal_rgb_img = {0};
	struct cal_img_t cal_nir_img = {0};
	struct map_t map = {0};
	sprintf(rgb, "/data/mod_img/rgb-1594957397372766.jpg");
	sprintf(nir, "/data/mod_img/nir-1594957397372766.jpg");

	/* rgb */
	cal_rgb_img.path = rgb;
	cal_rgb_img.row = face_svc_conf->svc.map_mod_width;
	cal_rgb_img.col = face_svc_conf->svc.map_mod_height;
	cal_rgb_img.ratio = face_svc_conf->svc.map_mod_grid_face;

	ret = r2n_get_point_from(&cal_rgb_img);
	if (ret) {
		goto exit;
	}

	/* nir */
	cal_nir_img.path = nir;
	cal_nir_img.row = face_svc_conf->svc.map_mod_width;
	cal_nir_img.col = face_svc_conf->svc.map_mod_height;
	cal_nir_img.ratio = face_svc_conf->svc.map_mod_grid_face;
	ret = r2n_get_point_from(&cal_nir_img);
	if (ret) {
		goto exit;
	}

	r2n_add_rgb_point(cal_rgb_img.img_point);
	r2n_add_nir_point(cal_nir_img.img_point);


	sprintf(rgb, "/data/mod_img/rgb-1594957405744209.jpg");
	sprintf(nir, "/data/mod_img/nir-1594957405744209.jpg");

	/* rgb */
	cal_rgb_img.path = rgb;
	cal_rgb_img.row = face_svc_conf->svc.map_mod_width;
	cal_rgb_img.col = face_svc_conf->svc.map_mod_height;
	cal_rgb_img.ratio = face_svc_conf->svc.map_mod_grid_face;

	ret = r2n_get_point_from(&cal_rgb_img);
	if (ret) {
		goto exit;
	}

	/* nir */
	cal_nir_img.path = nir;
	cal_nir_img.row = face_svc_conf->svc.map_mod_width;
	cal_nir_img.col = face_svc_conf->svc.map_mod_height;
	cal_nir_img.ratio = face_svc_conf->svc.map_mod_grid_face;
	ret = r2n_get_point_from(&cal_nir_img);
	if (ret) {
		goto exit;
	}

	r2n_add_rgb_point(cal_rgb_img.img_point);
	r2n_add_nir_point(cal_nir_img.img_point);

#endif

#if 0
	vector<Point2f> pointBuf_rgb;
	vector<Point2f> pointBuf_nir;
	Point2f pointBuf[54];
	vector<vector<Point2f>> vec_pointBuf_rgb, vec_pointBuf_nir;
	vector<double> vec_grids_per_face;
	vec_grids_per_face.push_back(face_svc_conf->svc.map_ord_grid_face);
	vec_grids_per_face.push_back(face_svc_conf->svc.map_ord_grid_face);
	Size boardSize = { face_svc_conf->svc.map_mod_width, face_svc_conf->svc.map_mod_height }; //w, h

	pointBuf[0].x = 567.731567;
	pointBuf[0].y = 240.099945;
	pointBuf[1].x = 595.650513;
	pointBuf[1].y = 240.428436;
	pointBuf[2].x = 624.382080;
	pointBuf[2].y = 240.719696;
	pointBuf[3].x = 652.549866;
	pointBuf[3].y = 241.154922;
	pointBuf[4].x = 681.459290;
	pointBuf[4].y = 241.474045;
	pointBuf[5].x = 710.277222;
	pointBuf[5].y = 241.729095;
	pointBuf[6].x = 567.989502;
	pointBuf[6].y = 268.359589;
	pointBuf[7].x = 596.139709;
	pointBuf[7].y = 268.612213;
	pointBuf[8].x = 624.450867;
	pointBuf[8].y = 269.363586;
	pointBuf[9].x = 652.535278;
	pointBuf[9].y = 269.712494;
	pointBuf[10].x = 681.084106;
	pointBuf[10].y = 270.008667;
	pointBuf[11].x = 709.484741;
	pointBuf[11].y = 270.277588;
	pointBuf[12].x = 568.532593;
	pointBuf[12].y = 297.056549;
	pointBuf[13].x = 596.229797;
	pointBuf[13].y = 297.289703;
	pointBuf[14].x = 623.995422;
	pointBuf[14].y = 297.751923;
	pointBuf[15].x = 652.490417;
	pointBuf[15].y = 298.404022;
	pointBuf[16].x = 680.510986;
	pointBuf[16].y = 298.567719;
	pointBuf[17].x = 709.458984;
	pointBuf[17].y = 299.442078;
	pointBuf[18].x = 568.435974;
	pointBuf[18].y = 324.965942;
	pointBuf[19].x = 595.761047;
	pointBuf[19].y = 325.388580;
	pointBuf[20].x = 623.918884;
	pointBuf[20].y = 325.036407;
	pointBuf[21].x = 652.320557;
	pointBuf[21].y = 326.360779;
	pointBuf[22].x = 680.903198;
	pointBuf[22].y = 326.489685;
	pointBuf[23].x = 708.499451;
	pointBuf[23].y = 327.603882;
	pointBuf[24].x = 568.773926;
	pointBuf[24].y = 353.081238;
	pointBuf[25].x = 596.133179;
	pointBuf[25].y = 353.349548;
	pointBuf[26].x = 623.579529;
	pointBuf[26].y = 354.057678;
	pointBuf[27].x = 652.389587;
	pointBuf[27].y = 354.343414;
	pointBuf[28].x = 679.895081;
	pointBuf[28].y = 355.219391;
	pointBuf[29].x = 708.792664;
	pointBuf[29].y = 355.611115;
	pointBuf[30].x = 568.727478;
	pointBuf[30].y = 379.816498;
	pointBuf[31].x = 595.803345;
	pointBuf[31].y = 380.641296;
	pointBuf[32].x = 623.867920;
	pointBuf[32].y = 381.048584;
	pointBuf[33].x = 651.892700;
	pointBuf[33].y = 382.026459;
	pointBuf[34].x = 680.437622;
	pointBuf[34].y = 382.928314;
	pointBuf[35].x = 708.306458;
	pointBuf[35].y = 383.690918;
	pointBuf[36].x = 568.337891;
	pointBuf[36].y = 407.354645;
	pointBuf[37].x = 596.571716;
	pointBuf[37].y = 407.767242;
	pointBuf[38].x = 624.319031;
	pointBuf[38].y = 409.016724;
	pointBuf[39].x = 652.204651;
	pointBuf[39].y = 409.644470;
	pointBuf[40].x = 679.547729;
	pointBuf[40].y = 410.768585;
	pointBuf[41].x = 708.552429;
	pointBuf[41].y = 411.585571;
	pointBuf[42].x = 569.600891;
	pointBuf[42].y = 434.098877;
	pointBuf[43].x = 596.386902;
	pointBuf[43].y = 435.549133;
	pointBuf[44].x = 623.890137;
	pointBuf[44].y = 436.265411;
	pointBuf[45].x = 651.520447;
	pointBuf[45].y = 436.625946;
	pointBuf[46].x = 680.291687;
	pointBuf[46].y = 438.083557;
	pointBuf[47].x = 707.520447;
	pointBuf[47].y = 438.851013;
	pointBuf[48].x = 569.289795;
	pointBuf[48].y = 461.545715;
	pointBuf[49].x = 596.341919;
	pointBuf[49].y = 462.363190;
	pointBuf[50].x = 623.655151;
	pointBuf[50].y = 463.188141;
	pointBuf[51].x = 651.818604;
	pointBuf[51].y = 464.251617;
	pointBuf[52].x = 679.325745;
	pointBuf[52].y = 464.533112;
	pointBuf[53].x = 708.001099;
	pointBuf[53].y = 466.256714;
	pointBuf_rgb.clear();
	std::vector<Point2f>().swap(pointBuf_rgb);
	for(int i = 0; i< 54; i++) {
		pointBuf_rgb.push_back(pointBuf[i]);
	}
	vec_pointBuf_rgb.push_back(pointBuf_rgb);

	pointBuf[0].x = 533.655701;
	pointBuf[0].y = 173.335876;
	pointBuf[1].x = 579.270203;
	pointBuf[1].y = 173.197479;
	pointBuf[2].x = 624.503296;
	pointBuf[2].y = 173.025711;
	pointBuf[3].x = 670.505066;
	pointBuf[3].y = 172.630554;
	pointBuf[4].x = 716.132141;
	pointBuf[4].y = 172.364258;
	pointBuf[5].x = 762.549377;
	pointBuf[5].y = 172.529251;
	pointBuf[6].x = 534.422302;
	pointBuf[6].y = 219.416534;
	pointBuf[7].x = 579.374939;
	pointBuf[7].y = 219.057877;
	pointBuf[8].x = 624.861816;
	pointBuf[8].y = 218.965866;
	pointBuf[9].x = 670.446838;
	pointBuf[9].y = 218.796310;
	pointBuf[10].x = 716.285461;
	pointBuf[10].y = 218.591309;
	pointBuf[11].x = 762.176575;
	pointBuf[11].y = 218.639114;
	pointBuf[12].x = 534.539490;
	pointBuf[12].y = 264.708710;
	pointBuf[13].x = 579.574402;
	pointBuf[13].y = 264.564667;
	pointBuf[14].x = 624.790833;
	pointBuf[14].y = 264.603027;
	pointBuf[15].x = 670.188660;
	pointBuf[15].y = 264.698456;
	pointBuf[16].x = 715.865356;
	pointBuf[16].y = 264.517242;
	pointBuf[17].x = 762.023071;
	pointBuf[17].y = 264.967072;
	pointBuf[18].x = 534.773987;
	pointBuf[18].y = 309.843506;
	pointBuf[19].x = 579.586548;
	pointBuf[19].y = 309.735229;
	pointBuf[20].x = 625.272095;
	pointBuf[20].y = 309.660522;
	pointBuf[21].x = 669.850525;
	pointBuf[21].y = 310.177429;
	pointBuf[22].x = 716.054993;
	pointBuf[22].y = 310.699768;
	pointBuf[23].x = 761.739807;
	pointBuf[23].y = 310.528290;
	pointBuf[24].x = 534.161255;
	pointBuf[24].y = 355.277649;
	pointBuf[25].x = 580.087402;
	pointBuf[25].y = 355.211304;
	pointBuf[26].x = 624.552490;
	pointBuf[26].y = 355.165314;
	pointBuf[27].x = 670.760742;
	pointBuf[27].y = 355.508148;
	pointBuf[28].x = 715.272705;
	pointBuf[28].y = 355.798492;
	pointBuf[29].x = 761.683899;
	pointBuf[29].y = 356.306122;
	pointBuf[30].x = 535.310303;
	pointBuf[30].y = 399.730164;
	pointBuf[31].x = 579.457703;
	pointBuf[31].y = 400.571320;
	pointBuf[32].x = 625.397095;
	pointBuf[32].y = 400.046783;
	pointBuf[33].x = 669.690796;
	pointBuf[33].y = 400.463501;
	pointBuf[34].x = 716.243713;
	pointBuf[34].y = 401.264038;
	pointBuf[35].x = 761.024780;
	pointBuf[35].y = 401.571869;
	pointBuf[36].x = 534.234314;
	pointBuf[36].y = 445.463165;
	pointBuf[37].x = 579.910828;
	pointBuf[37].y = 445.635742;
	pointBuf[38].x = 624.666992;
	pointBuf[38].y = 445.506927;
	pointBuf[39].x = 670.912292;
	pointBuf[39].y = 446.016235;
	pointBuf[40].x = 715.478577;
	pointBuf[40].y = 446.466827;
	pointBuf[41].x = 761.842041;
	pointBuf[41].y = 446.689178;
	pointBuf[42].x = 534.439331;
	pointBuf[42].y = 491.828583;
	pointBuf[43].x = 579.363770;
	pointBuf[43].y = 489.645386;
	pointBuf[44].x = 625.837891;
	pointBuf[44].y = 490.275330;
	pointBuf[45].x = 670.009888;
	pointBuf[45].y = 490.620117;
	pointBuf[46].x = 716.353210;
	pointBuf[46].y = 491.499939;
	pointBuf[47].x = 760.531799;
	pointBuf[47].y = 491.994690;
	pointBuf[48].x = 534.479736;
	pointBuf[48].y = 534.441406;
	pointBuf[49].x = 580.609497;
	pointBuf[49].y = 534.306091;
	pointBuf[50].x = 623.703003;
	pointBuf[50].y = 535.486938;
	pointBuf[51].x = 671.497986;
	pointBuf[51].y = 535.838318;
	pointBuf[52].x = 715.349609;
	pointBuf[52].y = 536.190857;
	pointBuf[53].x = 762.237976;
	pointBuf[53].y = 537.008789;
	pointBuf_rgb.clear();
	std::vector<Point2f>().swap(pointBuf_rgb);
	for(int i = 0; i< 54; i++) {
		pointBuf_rgb.push_back(pointBuf[i]);
	}
	vec_pointBuf_rgb.push_back(pointBuf_rgb);

	pointBuf[0].x = 318.305573;
	pointBuf[0].y = 277.998840;
	pointBuf[1].x = 337.135620;
	pointBuf[1].y = 278.465363;
	pointBuf[2].x = 355.028564;
	pointBuf[2].y = 278.969940;
	pointBuf[3].x = 373.452240;
	pointBuf[3].y = 279.188782;
	pointBuf[4].x = 391.483093;
	pointBuf[4].y = 279.397095;
	pointBuf[5].x = 410.638824;
	pointBuf[5].y = 279.671692;
	pointBuf[6].x = 318.408356;
	pointBuf[6].y = 297.223511;
	pointBuf[7].x = 337.163788;
	pointBuf[7].y = 297.438324;
	pointBuf[8].x = 355.221283;
	pointBuf[8].y = 297.557037;
	pointBuf[9].x = 373.435425;
	pointBuf[9].y = 297.745636;
	pointBuf[10].x = 391.840118;
	pointBuf[10].y = 298.055786;
	pointBuf[11].x = 410.442322;
	pointBuf[11].y = 298.385742;
	pointBuf[12].x = 319.139099;
	pointBuf[12].y = 315.249908;
	pointBuf[13].x = 337.218231;
	pointBuf[13].y = 315.509918;
	pointBuf[14].x = 355.077423;
	pointBuf[14].y = 315.777679;
	pointBuf[15].x = 373.457367;
	pointBuf[15].y = 316.174042;
	pointBuf[16].x = 391.728699;
	pointBuf[16].y = 316.352356;
	pointBuf[17].x = 410.220612;
	pointBuf[17].y = 317.114532;
	pointBuf[18].x = 318.872589;
	pointBuf[18].y = 333.400391;
	pointBuf[19].x = 337.369873;
	pointBuf[19].y = 333.593994;
	pointBuf[20].x = 355.282013;
	pointBuf[20].y = 334.219452;
	pointBuf[21].x = 373.319794;
	pointBuf[21].y = 334.118042;
	pointBuf[22].x = 391.541809;
	pointBuf[22].y = 335.396637;
	pointBuf[23].x = 410.138153;
	pointBuf[23].y = 335.373291;
	pointBuf[24].x = 319.509491;
	pointBuf[24].y = 351.391449;
	pointBuf[25].x = 337.007355;
	pointBuf[25].y = 352.220490;
	pointBuf[26].x = 355.305328;
	pointBuf[26].y = 352.298096;
	pointBuf[27].x = 373.418793;
	pointBuf[27].y = 353.046997;
	pointBuf[28].x = 391.552338;
	pointBuf[28].y = 353.228760;
	pointBuf[29].x = 409.764740;
	pointBuf[29].y = 353.798767;
	pointBuf[30].x = 319.352112;
	pointBuf[30].y = 369.536835;
	pointBuf[31].x = 337.173401;
	pointBuf[31].y = 370.043488;
	pointBuf[32].x = 355.399139;
	pointBuf[32].y = 370.334381;
	pointBuf[33].x = 373.356354;
	pointBuf[33].y = 370.587921;
	pointBuf[34].x = 391.486389;
	pointBuf[34].y = 371.410126;
	pointBuf[35].x = 409.715881;
	pointBuf[35].y = 371.862457;
	pointBuf[36].x = 319.433563;
	pointBuf[36].y = 387.317230;
	pointBuf[37].x = 337.338104;
	pointBuf[37].y = 387.587708;
	pointBuf[38].x = 355.413208;
	pointBuf[38].y = 388.304565;
	pointBuf[39].x = 373.151093;
	pointBuf[39].y = 388.745758;
	pointBuf[40].x = 391.354370;
	pointBuf[40].y = 389.343872;
	pointBuf[41].x = 409.566589;
	pointBuf[41].y = 389.618744;
	pointBuf[42].x = 319.753326;
	pointBuf[42].y = 404.689911;
	pointBuf[43].x = 337.414398;
	pointBuf[43].y = 405.278900;
	pointBuf[44].x = 355.421814;
	pointBuf[44].y = 405.989288;
	pointBuf[45].x = 373.013702;
	pointBuf[45].y = 406.312286;
	pointBuf[46].x = 391.461395;
	pointBuf[46].y = 407.448669;
	pointBuf[47].x = 409.530762;
	pointBuf[47].y = 407.877869;
	pointBuf[48].x = 319.576508;
	pointBuf[48].y = 422.514252;
	pointBuf[49].x = 337.713104;
	pointBuf[49].y = 422.859436;
	pointBuf[50].x = 355.381134;
	pointBuf[50].y = 423.467224;
	pointBuf[51].x = 373.343079;
	pointBuf[51].y = 424.080688;
	pointBuf[52].x = 391.423828;
	pointBuf[52].y = 423.823578;
	pointBuf[53].x = 409.398956;
	pointBuf[53].y = 425.566406;
	pointBuf_nir.clear();
	std::vector<Point2f>().swap(pointBuf_nir);
	for(int i = 0; i< 54; i++) {
		pointBuf_nir.push_back(pointBuf[i]);
	}
	vec_pointBuf_nir.push_back(pointBuf_nir);

	pointBuf[0].x = 290.068817;
	pointBuf[0].y = 235.421646;
	pointBuf[1].x = 319.426819;
	pointBuf[1].y = 235.279816;
	pointBuf[2].x = 348.808563;
	pointBuf[2].y = 235.446823;
	pointBuf[3].x = 378.546570;
	pointBuf[3].y = 235.048767;
	pointBuf[4].x = 408.059326;
	pointBuf[4].y = 235.079300;
	pointBuf[5].x = 438.127747;
	pointBuf[5].y = 234.716690;
	pointBuf[6].x = 290.396149;
	pointBuf[6].y = 265.186615;
	pointBuf[7].x = 319.726563;
	pointBuf[7].y = 265.252594;
	pointBuf[8].x = 349.140533;
	pointBuf[8].y = 265.070282;
	pointBuf[9].x = 378.625061;
	pointBuf[9].y = 265.178864;
	pointBuf[10].x = 408.239899;
	pointBuf[10].y = 265.256866;
	pointBuf[11].x = 437.893829;
	pointBuf[11].y = 265.280914;
	pointBuf[12].x = 290.589661;
	pointBuf[12].y = 294.605072;
	pointBuf[13].x = 319.502655;
	pointBuf[13].y = 295.314270;
	pointBuf[14].x = 348.975128;
	pointBuf[14].y = 294.486450;
	pointBuf[15].x = 378.571930;
	pointBuf[15].y = 294.537018;
	pointBuf[16].x = 407.548798;
	pointBuf[16].y = 295.294891;
	pointBuf[17].x = 437.484711;
	pointBuf[17].y = 294.691223;
	pointBuf[18].x = 290.720001;
	pointBuf[18].y = 323.979980;
	pointBuf[19].x = 319.706757;
	pointBuf[19].y = 324.157867;
	pointBuf[20].x = 349.200043;
	pointBuf[20].y = 324.331482;
	pointBuf[21].x = 378.506836;
	pointBuf[21].y = 324.475494;
	pointBuf[22].x = 407.852844;
	pointBuf[22].y = 324.456879;
	pointBuf[23].x = 437.505615;
	pointBuf[23].y = 324.583740;
	pointBuf[24].x = 290.798523;
	pointBuf[24].y = 353.282318;
	pointBuf[25].x = 319.808746;
	pointBuf[25].y = 353.700775;
	pointBuf[26].x = 349.076508;
	pointBuf[26].y = 353.665314;
	pointBuf[27].x = 378.460052;
	pointBuf[27].y = 353.804047;
	pointBuf[28].x = 407.782043;
	pointBuf[28].y = 353.960388;
	pointBuf[29].x = 437.440918;
	pointBuf[29].y = 354.018097;
	pointBuf[30].x = 290.807129;
	pointBuf[30].y = 382.748047;
	pointBuf[31].x = 319.560608;
	pointBuf[31].y = 383.442322;
	pointBuf[32].x = 349.172272;
	pointBuf[32].y = 382.952820;
	pointBuf[33].x = 378.508392;
	pointBuf[33].y = 383.081299;
	pointBuf[34].x = 407.558685;
	pointBuf[34].y = 383.503571;
	pointBuf[35].x = 437.287262;
	pointBuf[35].y = 383.623077;
	pointBuf[36].x = 291.089874;
	pointBuf[36].y = 412.039154;
	pointBuf[37].x = 319.962677;
	pointBuf[37].y = 412.040649;
	pointBuf[38].x = 349.147858;
	pointBuf[38].y = 412.172760;
	pointBuf[39].x = 378.490814;
	pointBuf[39].y = 412.484222;
	pointBuf[40].x = 407.773621;
	pointBuf[40].y = 412.806427;
	pointBuf[41].x = 437.088501;
	pointBuf[41].y = 413.140839;
	pointBuf[42].x = 291.198273;
	pointBuf[42].y = 441.206207;
	pointBuf[43].x = 320.316895;
	pointBuf[43].y = 441.337952;
	pointBuf[44].x = 349.105652;
	pointBuf[44].y = 441.630707;
	pointBuf[45].x = 378.482574;
	pointBuf[45].y = 441.809814;
	pointBuf[46].x = 407.593842;
	pointBuf[46].y = 442.143341;
	pointBuf[47].x = 437.121460;
	pointBuf[47].y = 442.387085;
	pointBuf[48].x = 291.199188;
	pointBuf[48].y = 470.114349;
	pointBuf[49].x = 320.267365;
	pointBuf[49].y = 470.284882;
	pointBuf[50].x = 349.179962;
	pointBuf[50].y = 470.421356;
	pointBuf[51].x = 378.376648;
	pointBuf[51].y = 470.743347;
	pointBuf[52].x = 407.524719;
	pointBuf[52].y = 471.473450;
	pointBuf[53].x = 437.156525;
	pointBuf[53].y = 471.634644;
	pointBuf_nir.clear();
	std::vector<Point2f>().swap(pointBuf_nir);
	for(int i = 0; i< 54; i++) {
		pointBuf_nir.push_back(pointBuf[i]);
	}
	vec_pointBuf_nir.push_back(pointBuf_nir);

	Mat x_map(5, 1, CV_64FC1), y_map(5, 1, CV_64FC1);
	ALOGI("ord_map");
	ALOGI("x_map:");
	for (int i = 0; i < 5; i++) {
		ALOGI("%lf\t", x_map.at<double>(i));
	}
	ALOGI("y_map:");
	for (int i = 0; i < 5; i++) {
		ALOGI("%lf\t", y_map.at<double>(i));
	}
	r2n_cal_map(vec_pointBuf_rgb, vec_pointBuf_nir, vec_grids_per_face, boardSize, x_map, y_map, 0);

	ALOGI("mod_map");
	ALOGI("x_map:");
	for (int i = 0; i < 5; i++) {
		ALOGI("%lf\t", x_map.at<double>(i));
	}
	ALOGI("y_map:");
	for (int i = 0; i < 5; i++) {
		ALOGI("%lf\t", y_map.at<double>(i));
	}

	return 0;
#endif

#if 1
	cal_iface->cal_mod_map(&map);
exit:
	if(cal_rgb_img.img_point) {
		if(cal_rgb_img.img_point->point) {
			free(cal_rgb_img.img_point->point);
			cal_rgb_img.img_point->point = NULL;
		}
		free(cal_rgb_img.img_point);
		cal_rgb_img.img_point = NULL;
	}
	if(cal_nir_img.img_point) {
		if(cal_nir_img.img_point->point) {
			free(cal_nir_img.img_point->point);
			cal_nir_img.img_point->point = NULL;
		}
		free(cal_nir_img.img_point);
		cal_nir_img.img_point = NULL;
	}
	return ret;
#endif
}

