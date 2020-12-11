/*
 * camera_record.h
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
int camera_record_start(void);
int camera_record_start_by_params(int video_enc_format, int output_format,
		int nir_enable);
int camera_record_stop(void);
int camera_record_remove_files(void);
int camera_record_get_rgb_status();
int camera_record_get_nir_status();
char *camera_record_get_rgb_file_path();
char *camera_record_get_nir_file_path();
