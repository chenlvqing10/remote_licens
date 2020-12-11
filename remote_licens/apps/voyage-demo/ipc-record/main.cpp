/*
 * main.cpp
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
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "rtsp_client.h"

#define MAX_CAMERA_NUMBER 8

class camera_info {
public :
	char *ip;
	char *user;
	char *pwd;

	char url[256];
	int frame_rate;
};
int duration;

camera_info cam_info[MAX_CAMERA_NUMBER];
char path[128];

char *getCmdOption(char **begin, char **end, const std::string &option)
{
	char **itr = std::find(begin, end, option);
	if (itr != end && ++itr != end) {
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
	return std::find(begin, end, option) != end;
}

void *recording_thread(void *arg)
{
	camera_info *cam = (camera_info *)arg;
//	int ret = rtsp_record_by_ipc_info(duration, path, cam->ip, cam->user, cam->pwd);

	int ret = rtsp_record_by_url(duration, cam->frame_rate, path, cam->url);
	if (ret != 0)
		printf("%s %d ret is %d\n", __func__, __LINE__, ret);

	return NULL;
}

int main(int argc, char **argv)
{
	int ret = 0;
	int camera_number = 0;
	pthread_t camera_recording[MAX_CAMERA_NUMBER];
	int times = 1;

	if (!(cmdOptionExists(argv, argv + argc, "-cIp")
			&& cmdOptionExists(argv, argv + argc, "-cUsr")
			&& cmdOptionExists(argv, argv + argc, "-cPwd"))) {
		std::cout
				<< "usage: ./ipc-record -cIp [<camera-ip>:<port>] -cUsr <cam-id> -cPwd <cam-pwd> "
						"[-cIp<n> [<camera-ip>:<port>] -cUsr<n> <cam-id> -cPwd<n> <cam-pwd>]"
						"[-cPath <storage-path>] [-cTimes <number>] [-cDuration <time>]\n";
		return -1;
	}

	cam_info[0].ip = getCmdOption(argv, argv + argc, "-cIp");
	cam_info[0].user = getCmdOption(argv, argv + argc, "-cUsr");
	cam_info[0].pwd = getCmdOption(argv, argv + argc, "-cPwd");

	ret = rtsp_is_ipc_enable(cam_info[0].ip, cam_info[0].user, cam_info[0].pwd);

	if (ret != 0) {
		printf("%s %d ret is %d\n", __func__, __LINE__, ret);
		return -1;
	}

	rtsp_get_stream_uri_framerate(cam_info[0].ip, cam_info[0].user,
			cam_info[0].pwd, cam_info[0].url, &cam_info[0].frame_rate);
	camera_number++;

	if (cmdOptionExists(argv, argv + argc, "-cPath")) {
		char *temp = getCmdOption(argv, argv + argc, "-cPath");
		strcpy(path, temp);
	} else
		sprintf(path, "/mnt/usb");

	if (cmdOptionExists(argv, argv + argc, "-cTimes")) {
		char *temp = getCmdOption(argv, argv + argc, "-cTimes");
		times = atoi(temp);
	} else
		times = 1;

	if (cmdOptionExists(argv, argv + argc, "-cDuration")) {
		char *temp = getCmdOption(argv, argv + argc, "-cDuration");
		duration = atoi(temp);
	} else
		duration = 15;

	for (int i = 1; i < MAX_CAMERA_NUMBER; i++) {
		if (cmdOptionExists(argv, argv + argc, "-cIp" + std::to_string(i))
				&& cmdOptionExists(argv, argv + argc, "-cUsr" + std::to_string(i))
				&& cmdOptionExists(argv, argv + argc,
						"-cPwd" + std::to_string(i))) {

			cam_info[camera_number].ip = getCmdOption(argv, argv + argc,
					"-cIp" + std::to_string(i));
			cam_info[camera_number].user = getCmdOption(argv, argv + argc,
					"-cUsr" + std::to_string(i));
			cam_info[camera_number].pwd = getCmdOption(argv, argv + argc,
					"-cPwd" + std::to_string(i));
			rtsp_get_stream_uri_framerate(cam_info[camera_number].ip,
					cam_info[camera_number].user, cam_info[camera_number].pwd,
					cam_info[camera_number].url,
					&cam_info[camera_number].frame_rate);
			camera_number++;
		}
	}

	for (int j = 0; j < times; j++) {
		for (int i = 0; i < camera_number; i++)
			pthread_create(&camera_recording[i], NULL, recording_thread,
					(void*) &cam_info[i]);

		for (int i = 0; i < camera_number; i++)
			pthread_join(camera_recording[i], NULL);
	}

	return ret;
}
