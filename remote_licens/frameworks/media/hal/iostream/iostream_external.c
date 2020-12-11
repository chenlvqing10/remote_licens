#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "iostream_external"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>

#include "oscl.h"
#include "ring_buffer_mgr.h"
#include "iostream_external.h"
#include "iostream_priv.h"
#include "iostream.h"


typedef struct ext_stream_priv {
	void *rbm_hdl;
	pthread_mutex_t wlock;
	pthread_cond_t wcond;
	pthread_mutex_t rlock;
	pthread_cond_t rcond;
	int blocking;
} ext_stream_priv_t;

ssize_t iostream_ext_write(void *s, uint8_t *buf, size_t size)
{
	OSCL_LOGW("write is not support!");
	return -1;
}

ssize_t iostream_ext_read(void *s, uint8_t *buf, size_t size)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	ext_stream_priv_t *exts = (ext_stream_priv_t *)s;
	ssize_t remain = size;
	uint8_t *p = buf;
	ssize_t ret;

	OSCL_LOGD("read %d bytes", size);

	while (remain > 0) {
		ret = rbm_read(exts->rbm_hdl, p, remain);
		if (ret < 0) {
			OSCL_LOGE("read stream data err!");
			return -1;
		}
		if (exts->blocking == 0)
			break;
		remain -= ret;
		p += ret;
		if (remain > 0) {
			pthread_mutex_lock(&exts->wlock);
			pthread_cond_wait(&exts->wcond, &exts->wlock);
			pthread_mutex_unlock(&exts->wlock);
		}
	}

	pthread_mutex_lock(&exts->rlock);
	pthread_cond_signal(&exts->rcond);
	pthread_mutex_unlock(&exts->rlock);

	OSCL_LOGD("read %d bytes", size - remain);

	return size - remain;
}

int64_t iostream_ext_seek(void *s, int64_t offset, int whence)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	ext_stream_priv_t *exts = (ext_stream_priv_t *)s;
	OSCL_LOGD("seek to %lld", offset);
	return rbm_seek(exts->rbm_hdl, offset, whence);
}

int64_t iostream_ext_tell(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	ext_stream_priv_t *exts = (ext_stream_priv_t *)s;
	return rbm_tell(exts->rbm_hdl);
}

int64_t iostream_ext_fsize(void *s)
{
	OSCL_LOGD("fsize is not support!");
	return -1;
}

long iostream_ext_ctrl(void *s, int cmd, void *arg)
{
	if (s == NULL) {
		OSCL_LOGE("invalid parameter(s=%p, param=%p)!", s, arg);
		return -1;
	}
	ext_stream_priv_t *exts = (ext_stream_priv_t *)s;

	switch (cmd) {
	case IOSTREAM_CMD_FILL_DATA: {
		IOStreamData_t *iosData = (IOStreamData_t *)arg;
		ssize_t remain = iosData->size;
		uint8_t *p = iosData->data;
		ssize_t ret;

		if (iosData == NULL) {
			OSCL_LOGE("invalid parameter(iosData=NULL)!");
			return -1;
		}
		OSCL_LOGD("fill %d bytes", iosData->size);
		while (remain > 0) {
			ret = rbm_write(exts->rbm_hdl, p, remain);
			if (ret < 0) {
				OSCL_LOGE("write stream data err!");
				return -1;
			}
			remain -= ret;
			p += ret;
			if (remain > 0) {
				OSCL_LOGD("remain %d bytes, wait", remain);
				pthread_mutex_lock(&exts->rlock);
				pthread_cond_wait(&exts->rcond, &exts->rlock);
				pthread_mutex_unlock(&exts->rlock);
			}
		}
		pthread_mutex_lock(&exts->wlock);
		pthread_cond_signal(&exts->wcond);
		pthread_mutex_unlock(&exts->wlock);
		OSCL_LOGD("fill %d bytes success", iosData->size);
		return 0;
	}
	case IOSTREAM_CMD_BLOCKING: {
		exts->blocking = (int)arg;
		pthread_mutex_lock(&exts->wlock);
		pthread_cond_signal(&exts->wcond);
		pthread_mutex_unlock(&exts->wlock);
		return 0;
	}
	default:
		OSCL_LOGW("unknown cmd %d!", cmd);
		return -1;
	}

	return 0;
}

void *iostream_ext_create(void *param)
{
	external_stream_para_t *buf_info = (external_stream_para_t *)param;
	ext_stream_priv_t *exts;

	exts = (ext_stream_priv_t *)oscl_malloc(sizeof(ext_stream_priv_t));
	if (exts == NULL) {
		OSCL_LOGE("alloc external priv error!");
		return NULL;
	}

	pthread_mutex_init(&exts->wlock, NULL);
	pthread_cond_init(&exts->wcond, NULL);

	pthread_mutex_init(&exts->rlock, NULL);
	pthread_cond_init(&exts->rcond, NULL);

	exts->rbm_hdl = rbm_create(buf_info->size, buf_info->align);
	if (exts->rbm_hdl == NULL) {
		OSCL_LOGE("alloc rbm_hdl error!");
		oscl_free(exts);
		return NULL;
	}
	exts->blocking = 1;

	return exts;
}

void iostream_ext_destroy(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return;
	}

	ext_stream_priv_t *exts = (ext_stream_priv_t *)s;

	pthread_mutex_lock(&exts->rlock);
	pthread_cond_signal(&exts->rcond);
	pthread_mutex_unlock(&exts->rlock);

	pthread_mutex_lock(&exts->wlock);
	pthread_cond_signal(&exts->wcond);
	pthread_mutex_unlock(&exts->wlock);

	pthread_cond_destroy(&exts->rcond);
	pthread_mutex_destroy(&exts->rlock);

	pthread_cond_destroy(&exts->wcond);
	pthread_mutex_destroy(&exts->wlock);

	rbm_destroy(exts->rbm_hdl);
	oscl_free(exts);
}

