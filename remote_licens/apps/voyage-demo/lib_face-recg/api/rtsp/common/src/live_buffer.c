#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#define LOG_TAG "LIVE_BUFFER"
#include <log/log.h>

#include "live_buffer.h"

#define FIFO_VIDEO_BUF_SIZE	614400 /* 600KB */
#define FIFO_AUDIO_BUF_SIZE	10240 /* 10KB */
#define MAX_SFIFO_NUM	10

#if 0
#define CONFIG_COND_FREE 1
#define CONFIG_COND_ACTIVE 1
#endif

struct sfifo_des_s *my_sfifo_des[2] = {NULL};

struct sfifo_s* sfifo_get_free_buf(struct sfifo_des_s *sfifo_des_p)
{
	static long empty_count = 0;
	struct sfifo_s *sfifo = NULL;

	pthread_mutex_lock(&(sfifo_des_p->free_list.lock_mutex));
#ifdef CONFIG_COND_FREE
	while (sfifo_des_p->free_list.head == NULL) {
		pthread_cond_wait(&(sfifo_des_p->free_list.cond), &(sfifo_des_p->free_list.lock_mutex));
	}
#else
	if (sfifo_des_p->free_list.head == NULL) {
		if (empty_count++ % 120 == 0) {
			ALOGD("%s free list empty\n", __func__);
		}
		goto EXIT;
	}
#endif
	sfifo = sfifo_des_p->free_list.head;
	sfifo_des_p->free_list.head = sfifo->next;

EXIT:
	pthread_mutex_unlock(&(sfifo_des_p->free_list.lock_mutex));

	return sfifo;
}

int sfifo_put_free_buf(struct sfifo_s *sfifo, struct sfifo_des_s *sfifo_des_p)
{
#ifdef CONFIG_COND_FREE
	int send_cond = 0;
#endif

	pthread_mutex_lock(&(sfifo_des_p->free_list.lock_mutex));
	if (sfifo_des_p->free_list.head == NULL) {
		sfifo_des_p->free_list.head = sfifo;
		sfifo_des_p->free_list.tail = sfifo;
		sfifo_des_p->free_list.tail->next = NULL;
#ifdef CONFIG_COND_FREE
		send_cond = 1;
#endif
	} else {
		sfifo_des_p->free_list.tail->next = sfifo;
		sfifo_des_p->free_list.tail = sfifo;
		sfifo_des_p->free_list.tail->next = NULL;
	}
	pthread_mutex_unlock(&(sfifo_des_p->free_list.lock_mutex));
#ifdef CONFIG_COND_FREE
	if (send_cond) {
		pthread_cond_signal(&(sfifo_des_p->free_list.cond));
	}
#endif
	return 0;
}

struct sfifo_s* sfifo_get_active_buf(struct sfifo_des_s *sfifo_des_p)
{
	struct sfifo_s *sfifo = NULL;

	pthread_mutex_lock(&(sfifo_des_p->active_list.lock_mutex));
#ifdef CONFIG_COND_ACTIVE
	while (sfifo_des_p->active_list.head == NULL) {
		/* pthread_cond_timedwait(&(sfifo_des_p->active_list.cond), &(sfifo_des_p->active_list.lock_mutex), &outtime); */
		pthread_cond_wait(&(sfifo_des_p->active_list.cond), &(sfifo_des_p->active_list.lock_mutex));
	}
#else
	if (sfifo_des_p->active_list.head == NULL) {
		/* ALOGD("%s active list empty\n", __func__); */
		goto EXIT;
	}
#endif

	sfifo = sfifo_des_p->active_list.head;
	sfifo_des_p->active_list.head = sfifo->next;

#ifndef CONFIG_COND_ACTIVE
EXIT:
#endif
	pthread_mutex_unlock(&(sfifo_des_p->active_list.lock_mutex));

	return sfifo;
}

int sfifo_put_active_buf(struct sfifo_s *sfifo, struct sfifo_des_s *sfifo_des_p)
{
#ifdef CONFIG_COND_ACTIVE
	int send_cond = 0;
#endif
	pthread_mutex_lock(&(sfifo_des_p->active_list.lock_mutex));
	if (sfifo_des_p->active_list.head == NULL) {
		sfifo_des_p->active_list.head = sfifo;
		sfifo_des_p->active_list.tail = sfifo;
		sfifo_des_p->active_list.tail->next = NULL;
#ifdef CONFIG_COND_ACTIVE
		send_cond = 1;
#endif
	} else {
		sfifo_des_p->active_list.tail->next = sfifo;
		sfifo_des_p->active_list.tail = sfifo;
		sfifo_des_p->active_list.tail->next = NULL;
	}
	pthread_mutex_unlock(&(sfifo_des_p->active_list.lock_mutex));
#ifdef CONFIG_COND_ACTIVE
	if (send_cond) {
		pthread_cond_signal(&(sfifo_des_p->active_list.cond));
	}
#endif
	return 0;
}

int dump_sfifo_list(struct sfifo_list_des_s *list)
{
	struct sfifo_s *sfifo = NULL;
	sfifo = list->head;
	do {
		ALOGD("%s dump : %x\n", __func__, sfifo->buffer[0]);
		usleep(500 * 1000);
	} while (sfifo->next != NULL && (sfifo = sfifo->next));

	return 0;
}

struct sfifo_des_s *sfifo_init(int sfifo_num, int sfifo_buffer_size,
	int sfifo_active_max_num)
{
	int i = 0;
	struct sfifo_s *sfifo;

	struct sfifo_des_s *sfifo_des_p;
	sfifo_des_p = (struct sfifo_des_s *)malloc(sizeof(struct sfifo_des_s));

	sfifo_des_p->sfifos_num = sfifo_num;
	sfifo_des_p->sfifos_active_max_num = sfifo_active_max_num;

	sfifo_des_p->free_list.sfifo_num = 0;
	sfifo_des_p->free_list.head = NULL;
	sfifo_des_p->free_list.tail = NULL;
	pthread_mutex_init(&sfifo_des_p->free_list.lock_mutex, NULL);
	pthread_cond_init(&sfifo_des_p->free_list.cond, NULL);

	sfifo_des_p->active_list.sfifo_num = 0;
	sfifo_des_p->active_list.head = NULL;
	sfifo_des_p->active_list.tail = NULL;
	pthread_mutex_init(&sfifo_des_p->active_list.lock_mutex, NULL);
	pthread_cond_init(&sfifo_des_p->active_list.cond, NULL);

	for (i = 0; i < sfifo_num; i++) {
		sfifo = (struct sfifo_s *)malloc(sizeof(struct sfifo_s));
		sfifo->buffer = (unsigned char *)malloc(sfifo_buffer_size);
		ALOGD("%s sfifo_init: %p\n", __func__, sfifo->buffer);
		memset(sfifo->buffer, i, sfifo_buffer_size);
		sfifo->size = sfifo_buffer_size;
		sfifo->next = NULL;
		sfifo_put_free_buf(sfifo, sfifo_des_p);
	}

	return sfifo_des_p;
}

void sfifo_deinit()
{
	struct sfifo_s *sfifo = NULL;
	struct sfifo_s *sfifo_temp = NULL;

	sfifo = my_sfifo_des[0]->active_list.head;
	while (sfifo != NULL) {
		sfifo_temp = sfifo->next;
		free(sfifo);
		sfifo = sfifo_temp;
	}

	sfifo = my_sfifo_des[0]->free_list.head;
	while (sfifo != NULL) {
		sfifo_temp = sfifo->next;
		free(sfifo);
		sfifo = sfifo_temp;
	}

#ifdef RTSP_AUDIO
	sfifo = my_sfifo_des[1]->active_list.head;
	while (sfifo != NULL) {
		sfifo_temp = sfifo->next;
		free(sfifo);
		sfifo = sfifo_temp;
	}

	sfifo = my_sfifo_des[1]->free_list.head;
	while (sfifo != NULL) {
		sfifo_temp = sfifo->next;
		free(sfifo);
		sfifo = sfifo_temp;
	}
#endif
}

static int get_buffer_index_by_stream_type(stream_type_e type)
{
#ifdef RTSP_AUDIO
	if (type == STREAM_TYPE_AUDIO)
		return 1;
#endif
	return 0;
}

static int get_buffer_size_by_stream_type(stream_type_e type)
{
#ifdef RTSP_AUDIO
	if (type == STREAM_TYPE_AUDIO)
		return FIFO_AUDIO_BUF_SIZE;
#endif
	return FIFO_VIDEO_BUF_SIZE;
}


int save_data_to_buf(unsigned char *buffer, unsigned int size, stream_type_e type)
{
	struct sfifo_s *sfifo;
	int loop_count = 0;

	while (1) {

		loop_count++;

		sfifo = sfifo_get_free_buf(
			my_sfifo_des[get_buffer_index_by_stream_type(type)]);
		if (sfifo != NULL) {
			if (size < get_buffer_size_by_stream_type(type)) {
				memcpy(sfifo->buffer + 1, buffer, size);
				sfifo->size = size;
			} else {
				memcpy(sfifo->buffer + 1, buffer,
					get_buffer_size_by_stream_type(type) - 1);
				sfifo->size = get_buffer_size_by_stream_type(type) - 1;
			}
			sfifo->type = type;
			/* printf("+++++++++ put : %x\n", sfifo->buffer[0]); */
			sfifo_put_active_buf(sfifo,
				my_sfifo_des[get_buffer_index_by_stream_type(type)]);

			loop_count = 0;

			break;
		} else if (loop_count > 2) {
			loop_count = 0;
			ALOGD("%s get_free_buf failed\n", __func__);

			break;
		}
	}

	return 0;
}

int get_data_from_buf(unsigned char *buffer, unsigned int *size, stream_type_e type)
{
	struct sfifo_s *sfifo;
	int loop_count = 0;

	while (1) {
		loop_count++;

		sfifo = sfifo_get_active_buf(
			my_sfifo_des[get_buffer_index_by_stream_type(type)]);

		if (sfifo != NULL) {
			memcpy(buffer, sfifo->buffer + 1, sfifo->size);
			*size = sfifo->size;
			/* printf("---------- get %x\n", sfifo->buffer[0]); */
			sfifo_put_free_buf(sfifo,
				my_sfifo_des[get_buffer_index_by_stream_type(type)]);
			break;
		} else if (loop_count > MAX_SFIFO_NUM) {
			loop_count = 0;

			break;
		}
		/* ALOGD("%s get_active_buf continue...\n", __func__); */
	}

	return 0;
}

struct sfifo_des_s *video_buffer_init()
{
	my_sfifo_des[0] = sfifo_init(MAX_SFIFO_NUM, FIFO_VIDEO_BUF_SIZE, MAX_SFIFO_NUM);

#ifdef RTSP_AUDIO
	my_sfifo_des[1] = sfifo_init(MAX_SFIFO_NUM, FIFO_AUDIO_BUF_SIZE, MAX_SFIFO_NUM);
#endif

	return my_sfifo_des[0];
}

int video_buffer_deinit()
{
	sfifo_deinit();
	return 0;
}

