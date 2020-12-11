/*
 * watermark_retangle.c
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "watermark_api.h"

int watermark_face_retangle_draw(face_t *face, frame_t *pframe,
		int face_video_width, int face_video_height,
		enc_input_mode_e input_mode)
{
	return watermark_face_retangle_draw_by_color(face, pframe, face_video_width,
			face_video_height, input_mode, 0xFF0000);
}

int watermark_face_retangle_draw_by_color(face_t *face, frame_t *pframe,
		int face_video_width, int face_video_height,
		enc_input_mode_e input_mode, int rgb_color)
{
	yuv_draw_t rtd;

	int x_start = pframe->info.video.width * face->rect_smooth[0]
			/ face_video_width;
	int y_start = pframe->info.video.height * face->rect_smooth[1]
			/ face_video_height;
	int x_end = pframe->info.video.width * face->rect_smooth[2]
			/ face_video_width;
	int y_end = pframe->info.video.height * face->rect_smooth[3]
			/ face_video_height;

	rtd.width_stride = pframe->info.video.width;
	rtd.height_aligned = pframe->info.video.height;
	rtd.yuv_mode = input_mode;
	rtd.retangle.start_pos.x = x_start;
	rtd.retangle.start_pos.y = y_start;
	rtd.retangle.end_pos.x = x_end;
	rtd.retangle.end_pos.y = y_end;
	rtd.buffer_y = pframe->info.video.addr[0];
	rtd.buffer_uv = pframe->info.video.addr[1];

#if 0
	ALOGD("rtd.retangle.start_pos.x: %d\n", rtd.retangle.start_pos.x);
	ALOGD("rtd.retangle.start_pos.y: %d\n", rtd.retangle.start_pos.y);
	ALOGD("rtd.retangle.end_pos.x: %d\n", rtd.retangle.end_pos.x);
	ALOGD("rtd.retangle.end_pos.y: %d\n", rtd.retangle.end_pos.y);
#endif
	return yuv_retangle_draw_by_color(&rtd, rgb_color);
}
