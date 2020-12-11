/*
 * file_http_download.c
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

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/prctl.h>

#define LOG_TAG "file_http_download"
#include <log/log.h>

#include <curl/curl.h>
#include <curl/mprintf.h>

#include "file_http_download.h"

static void *download_thread(void *arg);

#define FILE_HTTP_DEFAULE_LEN 128

struct http_download_s {
	char _url[FILE_HTTP_DEFAULE_LEN];
	char _storage_path[FILE_HTTP_DEFAULE_LEN];
	char _file_name[FILE_HTTP_DEFAULE_LEN];

	char _out_file_name[FILE_HTTP_DEFAULE_LEN];
	char _out_file_name_tmp[FILE_HTTP_DEFAULE_LEN];
	long local_len;
	FILE *fp;

	int running; /* 0: not running  1: running */
	int exit; 	/* 0 not exit  1: exit */

	int auto_free_res; /* finish download auto release resources */
	void (*finish_cb)(void *, int);
	void *data;
	int size;

	pthread_mutex_t _lock;
	pthread_cond_t _cond;
	pthread_t _thrd;

	struct download_handler_s handler;
};

static size_t download_write(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	FILE *fp = (FILE*)userdata;
	size_t written = fwrite(ptr, size, nmemb, fp);
	return written;
}

static int http_download_release(void *handler)
{
	struct http_download_s *hh = (struct http_download_s *)handler;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	pthread_mutex_destroy(&hh->_lock);
	pthread_cond_destroy(&hh->_cond);

	hh->handler.hh = NULL;

	if (hh->data) {
		free(hh->data);
		hh->data = NULL;
	}

	free(hh);
	hh = NULL;
	return 0;
}

static int http_download_exit(void *handler)
{
	struct http_download_s *hh = (struct http_download_s *)handler;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	pthread_mutex_lock(&hh->_lock);
	hh->running = 0;
	hh->exit = 1;
	pthread_cond_signal(&hh->_cond);
	pthread_mutex_unlock(&hh->_lock);

	if (!hh->auto_free_res) {
		pthread_join(hh->_thrd, NULL);
		http_download_release(hh);
	}
	return 0;
}

static int progress_func(void *ptr, double total_download, double now_downloaded,
		double total_upLoad, double now_upLoaded)
{
	static int tmp = 0;
	long local_len = 0;

	struct http_download_s *hh = (struct http_download_s *)ptr;

	local_len = hh->local_len;
	if (total_download > 0) {
		tmp = (int)((now_downloaded + (double)local_len) /
			(total_download + (double)local_len) * 100);
	}

	//fprintf(stderr, "Downing: %0d%%\r", tmp);
	hh->handler.progress = tmp;
	pthread_mutex_lock(&hh->_lock);
	if (!hh->running) {
		pthread_mutex_unlock(&hh->_lock);
		return -1;
	}
	pthread_mutex_unlock(&hh->_lock);

	return 0;
}

static long get_local_filelen(const char* file_name)
{
	FILE* fp = fopen(file_name, "rb");
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		long local_len = ftell(fp);
		fclose(fp);
		return local_len;
	}
	return 0;
}

static double get_download_filelen(char *url)
{
	double download_filelen = 0;

	CURL *_curl = curl_easy_init();
	if (!_curl) {
		return -1;
	}
	curl_easy_setopt(_curl, CURLOPT_URL, url);
	curl_easy_setopt(_curl, CURLOPT_HEADER, 1); //need header
	curl_easy_setopt(_curl, CURLOPT_NOBODY, 1); //without body
	if (curl_easy_perform(_curl) == CURLE_OK) {
		curl_easy_getinfo(_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &download_filelen);
	} else {
		download_filelen = -1;
	}
	curl_easy_cleanup(_curl);

	return download_filelen;
}

static bool http_downLoad(struct http_download_s *hh)
{
	// check already download tmp
	if (access(hh->_out_file_name, F_OK) == 0) {
		long local_file_len = get_local_filelen(hh->_out_file_name);
		long download_filelen = (long)get_download_filelen(hh->_url);
		ALOGD("[%s][%d] local_file_len = %ld download_filelen %ld\n",
				__func__, __LINE__, local_file_len, download_filelen);
		if (local_file_len == download_filelen) {
			return true;
		}
	}

	hh->local_len = get_local_filelen(hh->_out_file_name_tmp);

	hh->fp = fopen(hh->_out_file_name_tmp, "a+b");
	if (!hh->fp) {
		return false;
	}
	fseek(hh->fp, 0, SEEK_END);

	// start download
	CURL* _curl = curl_easy_init();
	if (!_curl) {
		return 0;
	}

	CURLcode res;
	curl_easy_setopt(_curl, CURLOPT_URL, hh->_url);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, download_write);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, hh->fp);
	curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, progress_func);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, hh);
	curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, 5L);

	curl_easy_setopt(_curl, CURLOPT_HEADER, 0L);
	curl_easy_setopt(_curl, CURLOPT_NOBODY, 0L);
	curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
	ALOGD("[%s][%d] local_len = %ld\n", __func__, __LINE__, hh->local_len);
	curl_easy_setopt(_curl, CURLOPT_RESUME_FROM, hh->local_len);

#if 1
	/* connect time out */
	curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 10);
	/* recv data time out */
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 10);
#endif
	res = curl_easy_perform(_curl);

	curl_easy_cleanup(_curl);
	if (res != 0) {
		ALOGE("[%s][%d] curl_easy_perform failed ! res = %d\n", __func__, __LINE__, res);
		fclose(hh->fp);
		return false;
	}

	/* success */
	fclose(hh->fp);
	rename(hh->_out_file_name_tmp, hh->_out_file_name);
	return true;
}

static int _set_url(void *handler, char *url, char *storage_path, char *file_name)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	if (!url || !storage_path || !file_name) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	memcpy(hh->_url, url, sizeof(hh->_url));
	memcpy(hh->_storage_path, storage_path, sizeof(hh->_storage_path));
	memcpy(hh->_file_name, file_name, sizeof(hh->_file_name));
	sprintf(hh->_out_file_name, "%s/%s", hh->_storage_path, hh->_file_name);
	sprintf(hh->_out_file_name_tmp, "%s.tmp", hh->_out_file_name);

	ALOGD("_url: %s\n", hh->_url);
	ALOGD("_storage_path: %s\n", hh->_storage_path);
	ALOGD("_file_name: %s\n", hh->_file_name);
	ALOGD("_out_file_name: %s\n", hh->_out_file_name);
	ALOGD("_out_file_name_tmp: %s\n", hh->_out_file_name_tmp);

	return 0;
}

static int _set_finish_cb(void *handler, void (*finish_cb)(void *, int), void *data, int size)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	if (data && size > 0) {
		hh->data = malloc(sizeof(char) * size);
		if (!hh->data) {
			ALOGE("malloc cb data failed, errno: %d (%s)\n",
				errno, strerror(errno));
			return -1;
		}
		memcpy(hh->data, data, size);
		hh->size = size;
	}

	if(finish_cb)
		hh->finish_cb = finish_cb;

	return 0;
}

static int _set_auto_free_res(void *handler)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh->auto_free_res = 1;

	return 0;
}

static int _start(void *handler)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh->running = 1;
	if (pthread_create(&hh->_thrd, NULL,
				download_thread, (void *)hh)) {
		ALOGE("create down_thread failed, "
				"errno: %d (%s)", errno, strerror(errno));
		return -1;
	}

	if (hh->auto_free_res) {
		pthread_detach(hh->_thrd);
	}

	return 0;
}

static int _pause(void *handler)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	pthread_mutex_lock(&hh->_lock);
	hh->running = 0;
	pthread_cond_signal(&hh->_cond);
	pthread_mutex_unlock(&hh->_lock);
	return 0;
}

static int _resume(void *handler)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	pthread_mutex_lock(&hh->_lock);
	hh->running = 1;
	pthread_cond_signal(&hh->_cond);
	pthread_mutex_unlock(&hh->_lock);
	return 0;

}

static int _stop(void *handler)
{
	struct http_download_s *hh = NULL;
	struct download_handler_s *dh = NULL;
	ALOGD("[%s][%d] enter\n", __func__, __LINE__);

	dh = (struct download_handler_s *)handler;
	if (!dh) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	hh = dh->hh;
	if (!hh ) {
		ALOGE("[%s][%d] err\n", __func__, __LINE__);
		return -1;
	}

	http_download_exit(hh);
	return 0;
}

static void *download_thread(void *arg)
{
	struct http_download_s *hh = (struct http_download_s *)arg;
	bool ret = false;
	int err_cnt = 0;
	int last_len = 0;

	prctl(PR_SET_NAME, "download_thr");

	while (1) {
		pthread_mutex_lock(&hh->_lock);
		if (!hh->running && !hh->exit)
			pthread_cond_wait(&hh->_cond, &hh->_lock);
		pthread_mutex_unlock(&hh->_lock);

		if (err_cnt < 3) {
			ret = http_downLoad(hh);
			if (true == ret) {
				hh->exit = 1;
			} else if(last_len == hh->local_len){
				err_cnt++;
				ALOGE("[%s][%d] err_cnt=%d\n", __func__, __LINE__, err_cnt);
			} else {
				err_cnt = 0;
				ALOGD("[%s][%d] reset err_cnt=%d\n", __func__, __LINE__, err_cnt);
			}
			last_len = hh->local_len;
		} else if (hh->handler.loading) {
			ALOGW("[%s][%d] err_cnt=%d\n", __func__, __LINE__, err_cnt);
			hh->handler.loading = 0;
		}

		if(hh->exit) {
			break;
		}
		sleep(2);
	}

	if (ret && hh->finish_cb) {
		hh->finish_cb(hh->data, hh->size);
	}

	sleep(2);

	if (hh->auto_free_res) {
		http_download_release(hh);
	}

	return NULL;
}

struct download_handler_s *http_download_create(void)
{
	struct http_download_s *hh = NULL;

	hh = (struct http_download_s *)malloc(sizeof(struct http_download_s));
	if (!hh) {
		ALOGE("malloc http_download_s failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto exit;
	}

	memset(hh, 0, sizeof(struct http_download_s));

	if (pthread_mutex_init(&hh->_lock, NULL)) {
		ALOGE("init lock failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto exit;
	}

	if (pthread_cond_init(&hh->_cond, NULL)) {
		ALOGE("init cond failed, errno: %d (%s)\n",
				errno, strerror(errno));
		goto exit;
	}

	hh->handler.hh = hh;
	hh->handler.set_url	= _set_url;
	hh->handler.start	= _start;
	hh->handler.pause	= _pause;
	hh->handler.resume	= _resume;
	hh->handler.stop	= _stop;
	hh->handler.loading = 1;

	hh->handler.set_finish_cb = _set_finish_cb;
	hh->handler.set_auto_free_res = _set_auto_free_res;

	return &hh->handler;

exit:
	if (hh) {
		free(hh);
		hh = NULL;
	}
	return NULL;
}

#if 0
void test_finish_cb(void *data, int size)
{
	ALOGD("finishfinishfinishfinishfinishfinishfinishfinishfinishfinishfinish");
	return;
}

int main(void)
{
#if 1
	int a = 15;
	while(--a) {
		struct download_handler_s *http_download = http_download_create();
		http_download->set_url(http_download, "http://192.168.19.156:8080/test.pkg","/data","my-test-pkg");
		http_download->set_auto_free_res(http_download);
		http_download->set_finish_cb(http_download, test_finish_cb, NULL, 0);
		http_download->start(http_download);
		usleep(100*1000);
		//http_download->stop(http_download);
		if (a == 1) {
			a = 15;
		}
	}
#endif
#if 0
	struct download_handler_s *http_download = http_download_create();

	http_download->set_url(http_download, "http://192.168.19.156:8080/test.pkg","/data","my-test-pkg");
	http_download->set_finish_cb(http_download, test_finish_cb, NULL, 0);

	char c = 0;

	while((c = getchar())) {
		ALOGD("downloading :%d\n", http_download->progress);
		switch (c)
		{
		case 's':
			http_download->start(http_download);
			break;
		case 'p':
			http_download->pause(http_download);
			break;
		case 'r':
			http_download->resume(http_download);
			break;
		case 'o':
			http_download->stop(http_download);
			break;
		case 'b':
			http_download->set_auto_free_res(http_download);
			break;
		}
	}
#endif
}
#endif
