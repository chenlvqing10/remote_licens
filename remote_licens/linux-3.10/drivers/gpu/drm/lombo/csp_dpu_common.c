/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __DPU_COMMON_C__
#define __DPU_COMMON_C__

#include "csp_dpu_common.h"

unsigned int dpu_format_get_planes(unsigned int fmt)
{
	unsigned int planes = 1;

	switch (fmt) {
	case DPU_FMT_YUV444SP:
	case DPU_FMT_YVU444SP:
	case DPU_FMT_YUV422SP:
	case DPU_FMT_YVU422SP:
	case DPU_FMT_YUV420SP:
	case DPU_FMT_YVU420SP:
		planes = 2;
		break;
	case DPU_FMT_YUV444P:
	case DPU_FMT_YUV422P:
	case DPU_FMT_YUV420P:
	case DPU_FMT_YUV411P:
	case DPU_FMT_YUV410P:
		planes = 3;
		break;
	default:
		planes = 1;
	}

	return planes;
}

unsigned int dpu_format_get_bits_per_pixel(unsigned int fmt, unsigned int plane)
{
	unsigned int bits_per_pixel = 32;

	if (plane == 0) {
		switch (fmt) {
		case DPU_FMT_ARGB8888:
		case DPU_FMT_ABGR8888:
		case DPU_FMT_BGRA8888:
		case DPU_FMT_RGBA8888:
		case DPU_FMT_AYUV:
		case DPU_FMT_VUYA:
			bits_per_pixel = 32;
			break;
		case DPU_FMT_RGB888:
		case DPU_FMT_BGR888:
		case DPU_FMT_RGB888_R:
		case DPU_FMT_BGR888_R:
			bits_per_pixel = 24;
			break;
		case DPU_FMT_ARGB4444:
		case DPU_FMT_ABGR4444:
		case DPU_FMT_BGRA4444:
		case DPU_FMT_RGBA4444:
		case DPU_FMT_ARGB1555:
		case DPU_FMT_ABGR1555:
		case DPU_FMT_BGRA5551:
		case DPU_FMT_RGBA5551:
		case DPU_FMT_RGB565:
		case DPU_FMT_BGR565:
		case DPU_FMT_RGB565_R:
		case DPU_FMT_BGR565_R:
		case DPU_FMT_YUYV:
		case DPU_FMT_YVYU:
		case DPU_FMT_UYVY:
		case DPU_FMT_VYUY:
			bits_per_pixel = 16;
			break;
		case DPU_FMT_YUV444SP:
		case DPU_FMT_YVU444SP:
		case DPU_FMT_YUV422SP:
		case DPU_FMT_YVU422SP:
		case DPU_FMT_YUV420SP:
		case DPU_FMT_YVU420SP:
		case DPU_FMT_YUV444P:
		case DPU_FMT_YUV422P:
		case DPU_FMT_YUV420P:
		case DPU_FMT_YUV411P:
		case DPU_FMT_YUV410P:
		case DPU_FMT_8BPP:
			bits_per_pixel = 8;
			break;
		case DPU_FMT_4BPP:
			bits_per_pixel = 4;
			break;
		case DPU_FMT_2BPP:
			bits_per_pixel = 2;
			break;
		case DPU_FMT_1BPP:
			bits_per_pixel = 1;
			break;
		default:
			bits_per_pixel = 32;
		}
	} else if (plane == 1) {
		switch (fmt) {
		case DPU_FMT_YUV444SP:
		case DPU_FMT_YVU444SP:
		case DPU_FMT_YUV422SP:
		case DPU_FMT_YVU422SP:
		case DPU_FMT_YUV420SP:
		case DPU_FMT_YVU420SP:
			bits_per_pixel = 16;
			break;
		case DPU_FMT_YUV444P:
		case DPU_FMT_YUV422P:
		case DPU_FMT_YUV420P:
		case DPU_FMT_YUV411P:
		case DPU_FMT_YUV410P:
			bits_per_pixel = 8;
			break;
		default:
			bits_per_pixel = 0;
		}
	} else if (plane == 2) {
		switch (fmt) {
		case DPU_FMT_YUV444P:
		case DPU_FMT_YUV422P:
		case DPU_FMT_YUV420P:
		case DPU_FMT_YUV411P:
		case DPU_FMT_YUV410P:
			bits_per_pixel = 8;
			break;
		default:
			bits_per_pixel = 0;
		}
	}

	return bits_per_pixel;
}

unsigned int dpu_format_get_bytes_per_pixel(
	unsigned int fmt, unsigned int component)
{
	return dpu_format_get_bits_per_pixel(fmt, component) / 8;
}

/**
 * dpu_get_hsr - get horizontal sample rate
 *
 * ret - 1: the same with y component;
 *        2: 1/2 of y component;
 *        4: 1/4 of y component
 */
int dpu_format_get_hsr(unsigned int fmt)
{
	int hsr = 1;

	switch (fmt) {
	case DPU_FMT_YUYV:
	case DPU_FMT_YVYU:
	case DPU_FMT_UYVY:
	case DPU_FMT_VYUY:
	case DPU_FMT_YUV422SP:
	case DPU_FMT_YVU422SP:
	case DPU_FMT_YUV422P:
	case DPU_FMT_YUV420SP:
	case DPU_FMT_YVU420SP:
	case DPU_FMT_YUV420P:
		hsr = 2;
		break;
	case DPU_FMT_YUV411P:
	case DPU_FMT_YUV410P:
		hsr = 4;
		break;
	default:
		hsr = 1;
	}

	return hsr;
}

/**
 * dpu_get_vsr - get vertical sample rate
 *
 * ret - 1: the same with y component;
 *        2: 1/2 of y component;
 *        4: 1/4 of y component.
 */
int dpu_format_get_vsr(unsigned int fmt)
{
	int vsr = 1;

	switch (fmt) {
	case DPU_FMT_YUV420SP:
	case DPU_FMT_YVU420SP:
	case DPU_FMT_YUV420P:
		vsr = 2;
		break;
	case DPU_FMT_YUV410P:
		vsr = 4;
		break;
	default:
		vsr = 1;
	}

	return vsr;
}

bool dpu_format_has_lut(unsigned int fmt)
{
	bool has_lut = false;

	switch (fmt) {
	case DPU_FMT_1BPP:
	case DPU_FMT_2BPP:
	case DPU_FMT_4BPP:
	case DPU_FMT_8BPP:
		has_lut = true;
		break;
	default:
		has_lut = false;
	}

	return has_lut;
}

#endif /* __DPU_COMMON_C__ */
