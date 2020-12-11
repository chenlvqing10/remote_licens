/*
 * lcd_tool_drm.h
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
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
#ifndef __LCD_TOOL_DRM_H_
#define __LCD_TOOL_DRM_H_

#define LCD_TOOL_OK	0

int lcd_tool_color_enhance_enable(void);
int lcd_tool_color_enhance_disable(void);

int lcd_tool_set_color_enhance_brightness(int val);
int lcd_tool_set_color_enhance_saturation(int val);
int lcd_tool_set_color_enhance_contrast(int val);

int lcd_tool_set_video_lti(int val);
int lcd_tool_set_video_peak(int val);

int lcd_tool_gamma_enable(void);
int lcd_tool_gamma_disable(void);
int lcd_tool_gamma_update(char *gamma_tbl, int tbl_size, bool enabled);

int lcd_tool_init(void);
int lcd_tool_exit(void);
#endif /* __LCD_TOOL_DRM_H_ */
