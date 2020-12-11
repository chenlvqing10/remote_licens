/*
 * rs.c - Interface to operate serial port
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

#include <sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include<termios.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>

#include "rs.h"

#define LOG_TAG "hal-rs"
#include <log/log.h>

#define MAX_INFO_LEN (1024)
#define MAX_BUF_LEN (512)

#define SLEEP_TIME (1000)

/* serial port thread structure */
typedef struct _rs_thread {
	int handle; /* handle of serial port */
	cb_trans_data_notify cb; /* callback of serial port receive data */
	void *context; /* object of context */
	int thread_running; /* thread is running */
	pthread_t rs_thread; /* thread id */
	struct termios oldtio; /* serial port parameter */
	int is_first; /* is first */
} rs_thread_t;

/**
 * open_tty - open tty device
 * @tty_name: tty device name
 * @flag: flag of open mode
 *
 * Returns handle of device.
 */
int open_tty(const char *tty_name, int flag)
{
	int handle = -1;

	if (NULL == tty_name) {
		ALOGE("%s[%d] tty_name is NULL", __func__, __LINE__);
		return -1;
	}

	handle = open(tty_name, flag);
	if (handle != -1) {
		if (fcntl(handle, F_SETFL, 0) < 0)
			ALOGE("%s[%d] fcntl failed!", __func__, __LINE__);

		if (isatty(STDIN_FILENO) == 0)
			ALOGE("%s[%d] device %s standard input is not a terminal device!",
					__func__, __LINE__, tty_name);
	}

	return handle;
}

/**
 * close_tty - close tty device
 * @handle: handle of device name
 * @oldtio: parameter of device
 *
 * Returns 0 if success, otherwise -1.
 */
int close_tty(int handle, struct termios *oldtio)
{
	if (handle == -1 || !oldtio) {
		ALOGE("%s[%d] oldtio is NULL or handle == -1!",
				__func__, __LINE__);
		return -1;
	}

	if ((tcsetattr(handle, TCSANOW, oldtio)) != 0)
		ALOGE("%s[%d] rs(%d) set error, errno %d\n",
				__func__, __LINE__, handle, errno);

	if (close(handle) < 0)
		ALOGE("%s[%d] close file error = %d\n",
				__func__, __LINE__, errno);

	return 0;
}

/**
 * set_opt - set parameter of serial port
 * @fd: handle of device name
 * @speed: boud rate
 * @bits: data bits
 * @even: even/odd bit
 * @stop: stop bit
 * @oldtio: parameter of device
 *
 * Returns 0 if success, otherwise -1.
 */
static int set_opt(int fd,
	int speed,
	int bits,
	char even,
	int stop,
	struct termios *oldtio)
{
	struct termios newtio;

	if (fd == -1) {
		ALOGE("%s[%d] fd == -1", __func__, __LINE__);
		return -1;
	}

	if (oldtio && (tcgetattr(fd, oldtio) != 0)) {
		ALOGE("%s[%d] SetupSerial (%d) errno %d",
				__func__, __LINE__, fd, errno);
		return -1;
	}

	memset(&newtio, 0, sizeof(struct termios));
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch (bits) {
	case 7:
		newtio.c_cflag |= CS7;
		break;

	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch (even) {
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;

	case 'E':
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;

	case 'N':
		newtio.c_cflag &= ~PARENB;
		break;
	}

	newtio.c_cflag &= ~CRTSCTS;

	switch (speed) {
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;

	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;

	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;

	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;

	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}

	if (stop == 1)
		newtio.c_cflag &= ~CSTOPB;
	else if (stop == 2)
		newtio.c_cflag |= CSTOPB;

	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_oflag &= ~OPOST;

	newtio.c_cc[VTIME] = 150;
	newtio.c_cc[VMIN] = 0;
	tcflush(fd, TCIFLUSH);

	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
		ALOGE("%s[%d] rs(%d) set error, errno %d",
				__func__, __LINE__, fd, errno);
		return -1;
	}

	return 0;
}

/**
 * rs_recv_thread: serial port receive data thread
 * @param: object of device
 * @data: data of send
 * &data_len: length of data
 *
 * Returns 0 if success, otherwise -1.
 */
void *rs_recv_thread(void *param)
{
	char buffer[MAX_BUF_LEN];
	int ret_len;
	rs_thread_t *thread;
	prctl(PR_SET_NAME, "rs_recv");

	thread = (rs_thread_t *)param;
	if (NULL == thread) {
		ALOGE("%s[%d] param is NULL", __func__, __LINE__);
		return NULL;
	}

	while (thread->thread_running) {
#if 1
		ret_len = -1;
		memset(buffer, 0, sizeof(buffer));
		if (thread->handle != -1)
			ret_len = read(thread->handle, buffer, sizeof(buffer));

		if (ret_len <= 0)
			usleep(SLEEP_TIME);
		else {
			ALOGE("%s[%d] recv data(%d): %s",
					__func__, __LINE__, ret_len, buffer);
			if (thread->cb)
				thread->cb(buffer, ret_len, thread->context);
		}
#endif
	}

	return NULL;
}

/**
 * rs_open - open serial port
 * @dev_name: device name
 * @open_mode: open mode(1/2/3:read only/write only/read and write)
 * @speed: boud rate
 * @bits: data bits
 * @even: even/odd bit
 * @stop: stop bit
 * @_trans_data_notify: a pointer of callback
 * @context: object of context
 *
 * Returns object of device.
 */
void *rs_open(const char *dev_name, int open_mode,
				int speed, int bits, char even, int stop,
				cb_trans_data_notify _trans_data_notify,
				void *context)
{
	int mode = 0;
	rs_thread_t *thread;

	if (NULL == dev_name) {
		ALOGE("%s[%d] dev_name is NULL", __func__, __LINE__);
		return NULL;
	}

	thread = (rs_thread_t *)malloc(sizeof(rs_thread_t));
	if (NULL == thread) {
		ALOGE("%s[%d] malloc(sizeof(rs_thread_t)) failure",
				__func__, __LINE__);
		return NULL;
	}

	memset(thread, 0, sizeof(rs_thread_t));
	thread->is_first = 1;
	thread->cb = _trans_data_notify;
	thread->context = context;
	switch (open_mode) {
	case 1:
		mode = O_RDONLY | O_NOCTTY | O_NDELAY;
		thread->thread_running = 1;
		break;

	case 2:
		mode = O_WRONLY | O_NOCTTY | O_NDELAY;
		break;

	case 3:
	default:
		mode = O_RDWR | O_NOCTTY | O_NDELAY;
		thread->thread_running = 1;
		break;
	}

	thread->handle = open_tty(dev_name, mode);
	if (thread->handle == -1) {
		ALOGE("%s[%d] open_tty(%s) failure, errno %d",
				__func__, __LINE__, dev_name, errno);
		free(thread);
		return NULL;
	}

	if (set_opt(thread->handle,
			speed, bits, even, stop, &(thread->oldtio)) < 0) {
		ALOGE("%s[%d] set_opt(%d %d %c %d) failure, errno %d",
				__func__, __LINE__, speed, bits, even, stop, errno);
		free(thread);
		return NULL;
	}

	if (1 == thread->thread_running) {
		pthread_create(&thread->rs_thread, 0,
				rs_recv_thread, (void *)thread);
	}

	return thread;
}

/**
 * rs_close - close serial port
 * @handle: object of device
 *
 * Returns 0 if success, otherwise -1.
 */
int rs_close(void *handle)
{
	rs_thread_t *thread;

	thread = (rs_thread_t *)handle;
	if (thread) {
		if (thread->thread_running) {
			thread->thread_running = 0;
			pthread_join(thread->rs_thread, NULL);
		}
		close_tty(thread->handle, &(thread->oldtio));
		free(thread);
		thread = NULL;

		return 0;
	}

	return -1;
}

/**
 * rs_send_data: send data to serial port
 * @handle: object of device
 * @data: data of send
 * &data_len: length of data
 *
 * Returns 0 if success, otherwise -1.
 */
int rs_send_data(void *handle, const char *data, int data_len)
{
	int ret_len;
	rs_thread_t *thread;

	thread = (rs_thread_t *)handle;
	if ((NULL == thread) || (NULL == data) || (0 == data_len)) {
		ALOGE("%s[%d] handle is NULL or data is NULL or data_len is 0",
				__func__, __LINE__);
		return -1;
	}

	if (0 > thread->handle) {
		ALOGE("%s[%d] thread->handle(%d) < 0",
				__func__, __LINE__, thread->handle);
		return -1;
	}

	ret_len = write(thread->handle, data, data_len);
	if (ret_len != data_len) {
		ALOGE("%s[%d] thread->handle(%d) send len %d should %d errno %d",
				__func__, __LINE__,
				thread->handle, ret_len,
				data_len, errno);
		return -1;
	}

	ALOGE("%s[%d] thread->handle(%d) send len %d",
			__func__, __LINE__, thread->handle, ret_len);

	return 0;
}

