/*
 * uito_tp_test.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>

int fd;
int fdw;

typedef struct qnode {
	char data[256];
	struct qnode *next;
}qnode,*queueptr;

typedef struct {
	queueptr front,rear;
}link_queue;
link_queue *queue;

static link_queue *creat_queue(void)
{
	link_queue *q1 = (link_queue *)malloc(sizeof(link_queue));
	if(q1 == NULL){
		printf("Creat empty queue failed\n");
	}
	q1->front = q1->rear = NULL;

	return q1;
}

int empty_link_queue(link_queue *q)
{
	if(q == NULL)
		printf("Empty link queue error!\n");

	return (q->front == NULL ? 1 : 0);
}

void enqueue(link_queue *q, char value[])
{
	unsigned int i = 0;
	queueptr new = (queueptr)malloc(sizeof(qnode));

	for (i = 0; i <= 255; i++)
		new->data[i] = value[i];

	new->next = NULL;

	if (empty_link_queue(q))
		q->front = q->rear = new;
	else {
		q->rear->next = new;
		q->rear = new;
	}

	return ;
}

void dequeue(link_queue *q)
{
	queueptr remove = (queueptr)malloc(sizeof(qnode));
	if (q == NULL) {
		printf("dequeue error\n");
		return ;
	}
	if (empty_link_queue(q)) {
		printf("queue is empty\n");
		return ;
	}
	remove = q->front;
	q->front = remove->next;

	if (q->front == NULL)
		q->rear = NULL;
	free(remove);
	return ;
}

static void *read_thread(void *args)
{
	int count, ret, timeout = 50;
	char buf[2];
	char tx_buf[256];
	struct pollfd fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLIN;

	while (1) {
		ret = poll(fds, 1, timeout);
		if (ret == 0)
			continue;
		else if (ret == -1)
			break;
		else {
			memset(buf, 0, sizeof(buf));
			count = read(fd, buf, 1);
			if (count > 0) {
				if (buf[0] != '\n') {
					strcat(tx_buf,buf);
				} else if (buf[0] == '\n') {
					enqueue(queue,tx_buf);
					memset(tx_buf, 0, sizeof(tx_buf));
				}
			}
		}
	}

	return NULL;
}

static void *write_thread(void *args)
{
	int retw= 0;

	while (1) {
		if (!empty_link_queue(queue)) {
			retw = write(fdw, queue->front->data, 256);
			dequeue(queue);
		}
		else
			usleep(2000);
		if (retw < 0)
			printf("write to kernel failed\n");
	}

	return NULL;
}

void help_info(void)
{
	printf("Usage: uarttest <uartid> <baudrate>\n");
	printf("    uartid: uart id, from 0 start\n");
	printf("    baudrate: uart baud rate\n");
}

int main(int argc, char *argv[])
{
	struct termios oldtio, newtio;
	char uart_name[64];
	pthread_t read_thread_id;
	pthread_t write_thread_id;
	int uart_idx;
	int speed;

	queue = creat_queue();

	if (argc < 3) {
		help_info();
		return -1;
	}

	uart_idx = atoi(argv[1]);
	speed = atoi(argv[2]);

	printf("target uart id: %d, baud rate: %d\n", uart_idx, speed);

	sprintf(uart_name, "/dev/ttySLB%d", uart_idx);
	/* open uart device file */
	fd = open(uart_name, O_RDWR | O_NOCTTY);
	fdw = open("/sys/class/tp_coordinate/coordinate/value", O_WRONLY);
	if (fd < 0) {
		printf("Open %s failed\n", uart_name);
		return -1;
	}
	else
		printf("Open %s successfully\n", uart_name);
	if (fdw < 0){
		printf("Open coordinate failed\n");
		return -1;
	}
	else
		printf("Open coordinate successfully\n");
	/* Set operating parameters */
	tcgetattr(fd, &oldtio);
	memset(&newtio, 0, sizeof(newtio));

	/* Set the baund rate */
	switch (speed) {
	case 2400:
		newtio.c_cflag = B2400;
		break;
	case 4800:
		newtio.c_cflag = B4800;
		break;
	case 9600:
		newtio.c_cflag = B9600;
		break;
	case 19200:
		newtio.c_cflag = B19200;
		break;
	case 38400:
		newtio.c_cflag = B38400;
		break;
	case 57600:
		newtio.c_cflag = B57600;
		break;
	case 115200:
		newtio.c_cflag = B115200;
		break;
	case 230400:
		newtio.c_cflag = B230400;
		break;
	case 460800:
		newtio.c_cflag = B460800;
		break;
	case 500000:
		newtio.c_cflag = B500000;
		break;
	case 576000:
		newtio.c_cflag = B576000;
		break;
	case 921600:
		newtio.c_cflag = B921600;
		break;
	case 1000000:
		newtio.c_cflag = B1000000;
		break;
	case 1152000:
		newtio.c_cflag = B1152000;
		break;
	case 1500000:
		newtio.c_cflag = B1500000;
		break;
	case 2000000:
		newtio.c_cflag = B2000000;
		break;
	case 2500000:
		newtio.c_cflag = B2500000;
		break;
	case 3000000:
		newtio.c_cflag = B3000000;
		break;
	case 3500000:
		newtio.c_cflag = B3500000;
		break;
	case 4000000:
		newtio.c_cflag = B4000000;
		break;
	default:
		newtio.c_cflag = B115200;
		break;
	}

	/* Data bit = 8, enable data recepation */
	newtio.c_cflag |= CS8 | CLOCAL | CREAD | CSTOPB | CRTSCTS;
	newtio.c_iflag = IGNPAR;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	pthread_create(&read_thread_id, NULL, read_thread, NULL);
	pthread_create(&write_thread_id, NULL, write_thread, NULL);

	while (1)
		usleep(100000);

	close(fd);
	close(fdw);

	return 0;
}
