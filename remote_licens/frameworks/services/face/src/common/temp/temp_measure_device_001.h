/*
 * temp_measure_device_001.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __TEMP_MEASURE_DEVICE_001__
#define __TEMP_MEASURE_DEVICE_001__

typedef struct temp_stats_s
{
	float tmax;
	float tmin;
	float tavg;
	float ext;
	float amt;
	unsigned short *stats_32x32;
} temp_stats_t;

float temp_measure(temp_stats_t *stats, int num);
void temp_measure_config(float offset, float env_slop, float env_standard);

int temp_device_001_init(void);
int temp_device_001_deinit(void);
float temp_device_001_get(void);
int temp_device_001_set_measure_frames(int _measure_frames);
int temp_device_001_reset_measure_frame();

//demo
//int main()
//{
//	temp_stats_t temp_s[8] = {0};
//	temp_measure(temp_s, 8);
//	return 0;
//}
// 每隔125ms读取一次温度统计值，一定要等够时间读取,建议测量数量大于等于8帧。


#endif //__TEMP_MEASURE_DEVICE_001__

