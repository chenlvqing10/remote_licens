/*
 * watermark_retangle.h
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
#ifndef __WATERMARK_RETANGLE_H__
#define __WATERMARK_RETANGLE_H__

#include "face.h"
#include "media_common.h"
#include "yuv_retangle_draw.h"

int watermark_face_retangle_draw(face_t *face, frame_t *pframe,
		int face_video_width, int face_video_height,
		enc_input_mode_e input_mode);

int watermark_face_retangle_draw_by_color(face_t *face, frame_t *pframe,
		int face_video_width, int face_video_height,
		enc_input_mode_e input_mode, int rgb_color);

int watermark_init_source(void *rec);
int watermark_set_time_hour_minute(void *rec, int number, int x, int y);
int watermark_set_time_h_m_s(void *rec, int number, int x, int y);

#endif
