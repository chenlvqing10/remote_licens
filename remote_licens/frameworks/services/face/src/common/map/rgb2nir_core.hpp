/*
 * rgb2nir_core.hpp - code for rotate component.
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

#ifndef __RGB2NIR_CORE__
#define __RGB2NIR_CORE__

int cal_ord_map(string data_path, Size& boardSize, Mat& x_map, Mat& y_map, double grid_face_scale, int flip_en);
int modify_ord_map(string data_path, double lr, int lr_cnt_thr, Size& boardSize, Mat& x_map, Mat& y_map, double grid_face_scale, int flip_en);

#endif //__RGB2NIR_CORE__