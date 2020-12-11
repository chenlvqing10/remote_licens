/*
 * temp_measure_device.c
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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOG_TAG "temp_measure_device"
#include <log/log.h>

#include "face_svc_conf.h"

#define UART_DEVICE "/dev/ttySLB1"

struct temp_s {
	float temp_max1;
	float temp_max2;
	float temp_max3;
	float temp_min;
	float temp_mean;
};

struct temp_device_s {
	int fd;
	struct termios oldtio;
	struct termios newtio;
	struct temp_s temp;
};

struct temp_device_s *device;

int temp_device_init(void)
{
	device = (struct temp_device_s *)malloc(sizeof(struct temp_device_s));
	if (!device) {
		ALOGE("malloc face_svc_t failed, errno: %d (%s)\n",
			errno, strerror(errno));
	}

	//-----------打开uart设备文件------------------
	device->fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
	if (device->fd < 0) {
		ALOGE("Open %s failed\n", UART_DEVICE);
		return -1;
	} else {
		ALOGE("Open %s successfully\n", UART_DEVICE);
	}

	//-----------设置操作参数-----------------------
	tcgetattr(device->fd, &device->oldtio);//获取当前操作模式参数
	memset(&device->newtio, 0, sizeof(struct termios));

	//波特率=230400 数据位=8 使能数据接收
	device->newtio.c_cflag = B230400 | CS8 | CLOCAL | CREAD | CSTOPB;
	device->newtio.c_iflag = IGNPAR;

	tcflush(device->fd, TCIFLUSH);//清空输入缓冲区和输出缓冲区
	tcsetattr(device->fd, TCSANOW, &device->newtio);//设置新的操作参数

	return 0;
}


float temp_device_get(void)
{
	int count, i;
	char cmd_buf[] = { 0xAA, 0x01, 0x04, 0x00, 0x06, 0x10, 0x05, 0x00, 0xBB};
	char read_buf[819];

	if(!device)
		return -1;

	memset(&device->temp, 0 , sizeof(struct temp_s));

	//printf("input: %s, len = %d\n", cmd_buf, strlen(cmd_buf));
	//------------向urat发送数据-------------------

	if (face_svc_conf->svc.temp_measure_debug) {
		for (i = 0; i < 9; i++)
			ALOGV("%#X ", cmd_buf[i]);
	}

	count = write(device->fd, cmd_buf, 9);
	if (count != 9) {
		ALOGE("send failed\n");
		return -1;
	}

	usleep(500000);

	memset(read_buf, 0, sizeof(read_buf));
	count = read(device->fd, read_buf, sizeof(read_buf));
	if (count > 0) {
		for (i = 0; i < count; i++);

		device->temp.temp_max1 = read_buf[7] << 8 | read_buf[6];
		device->temp.temp_max2 = read_buf[9] << 8 | read_buf[8];
		device->temp.temp_max3 = read_buf[11] << 8 | read_buf[10];
		device->temp.temp_min	= read_buf[13] << 8 | read_buf[12];
		device->temp.temp_mean = read_buf[15] << 8 | read_buf[14];

		if (face_svc_conf->svc.temp_measure_debug) {
			ALOGV("temp->temp_max1 = %f\n", device->temp.temp_max1 * 0.01);
			ALOGV("temp->temp_max2 = %f\n", device->temp.temp_max2 * 0.01);
			ALOGV("temp->temp_max3 = %f\n", device->temp.temp_max3 * 0.01);
			ALOGV("temp->temp_min	= %f\n", device->temp.temp_min  * 0.01);
			ALOGV("temp->temp_mean = %f\n", device->temp.temp_mean * 0.01);
		}
	} else {
		ALOGE(" read failed\n");
		return -1;
	}
	return (device->temp.temp_mean * 0.01);
}

int temp_device_deinit(void)
{
	if(!device)
		return -1;
	close(device->fd);
	tcsetattr(device->fd, TCSANOW, &device->oldtio); //恢复原先的设置
	free(device);
	return 0;
}

