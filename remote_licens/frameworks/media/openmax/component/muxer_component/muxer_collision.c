/*
 * oscl_cache_file.c - file api used by lombo media framework.
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
#define DBG_LEVEL		DBG_WARNING
#define LOG_TAG		"collision"

#include "oscl.h"
#include "base_component.h"
#include "muxer_com.h"
#include "muxer_common.h"
#include "lombo_cycle_buf.h"
#include "video_recorder.h"
#include "muxer_handle.h"
#include "muxer_private.h"
#include "muxer_collision.h"

#define COLLI_CACHE_HEADER 0
#define COLLI_CACHE_DATA 1

#define VIDEO_CACHE_MODE COLLI_CACHE_DATA

#define DEFAULT_COLLISION_TIME 5
#define MAX_MEDIA_BUFFER_PER_SEC 100
#define COLLI_AUDIO_BUF_SIZE (32 * 1024)
#define COLLI_VIDEO_BUF_SIZE (2*1024*1024)

#define HEADER_IS_KEY_FRAME(context, buf_header) \
	(!context->stream_cfg.exist[VIDEO_INDEX] || \
		    (buf_header->nFlags & OMX_BUFFERFLAG_SYNCFRAME))

#define COLLI_IS_KEY_FRAME(context, colli_frame) \
	(!context->stream_cfg.exist[VIDEO_INDEX] || \
				(colli_frame->flag & OMX_BUFFERFLAG_SYNCFRAME))

#define IS_VIDEO_EXIST(colli_info) \
	(colli_info->muxer_context->stream_cfg.exist[VIDEO_INDEX])

typedef struct colli_cache_data {
	int len;
	unsigned char *buf;
	OMX_TICKS timestamp;
	int flag;
} colli_cache_data_t;

typedef struct colli_buffer {
	/* collision/emergency file saved */
	int port_index;
	int cache_mode;
	OMX_TICKS timestamp;
	int flag;
	union {
		colli_cache_data_t data_buf;
		OMX_BUFFERHEADERTYPE *buf_header;
	} mbuf;
} colli_buffer_t;

typedef enum colli_cmd {
	COLLI_CMD_NULL = 0,
	COLLI_CMD_START,
	COLLI_CMD_STOP,
} colli_cmd_e;

typedef struct colli_info {
	pthread_mutex_t lock;
	MuxerContext *muxer_context;
	muxer_hdl_t *muxer_handle;
	int start_normal_mux;

	/* cache buffer and queue info */
	int c_total; /*ms*/
	int64_t c_start;
	int64_t c_end;
	oscl_queue_t q_colli;
	int nvideo;
	int naudio;
	/* cycle buffer, to save data */
	int nbuffer;
	colli_buffer_t *media_buffer;
	oscl_queue_t q_free_buf;
	void *cycbuf;


	/* file info */
	colli_cmd_e cmd;
	colli_muxfile_cfg_t f_cfg;
	int64_t stop_stamp;
	int64_t muxering_stamp;
	pthread_t muxer_cache_thread;
} colli_info_t;

#if 0
static int _colli_queue_log_keybuf(colli_info_t *_info)
{
	oscl_message_t *message = NULL;
	colli_buffer_t *tmp_buffer;
	int i = 0;
	int len = 0;

	oscl_param_check((_info != NULL), OMX_ErrorBadParameter, NULL);

	do {
		message = oscl_queue_query_next(&_info->q_colli, NULL);
		if ((message == NULL) || (message->data == NULL))
			break;
		tmp_buffer = message->data;
		len += tmp_buffer->mbuf.data_buf.len;
		if (COLLI_IS_KEY_FRAME(_info->muxer_context, tmp_buffer)) {
			OSCL_LOGE("key %d, time: %lld, len:%d",
				i++, tmp_buffer->timestamp,
				tmp_buffer->mbuf.data_buf.len);
		}
	} while (tmp_buffer);
	OSCL_LOGE("%p: v_start:%lld, end:%lld, total:%d, len:%d",
		_info, _info->c_start, _info->c_end,
		_info->c_total, len);

	return 0;
}
#endif

static int __omxbuf_to_data(colli_info_t *_info,
	colli_buffer_t *cbuf, OMX_BUFFERHEADERTYPE *omx_buf)
{
	int ret = 0;
	muxer_vpacket_t *venc_packet;
	unsigned char *data_addr;
	int data_size;

	oscl_param_check(cbuf != NULL, -1, NULL);
	oscl_param_check(omx_buf != NULL, -1, NULL);

	data_addr = omx_buf->pBuffer;
	data_size = omx_buf->nFilledLen;

	memset(cbuf, 0, sizeof(colli_buffer_t));
	cbuf->port_index = omx_buf->nInputPortIndex;
	cbuf->timestamp = omx_buf->nTimeStamp;
	cbuf->flag = omx_buf->nFlags;

	cbuf->cache_mode = COLLI_CACHE_DATA;
	if (omx_buf->nInputPortIndex == VIDEO_INDEX) {
		cbuf->cache_mode = VIDEO_CACHE_MODE;
		venc_packet = (muxer_vpacket_t *)omx_buf->pBuffer;
		data_addr = venc_packet->vir_addr;
		data_size = venc_packet->size;
	}

	if (cbuf->cache_mode == COLLI_CACHE_HEADER) {
		cbuf->mbuf.buf_header = omx_buf;
		goto EXIT;
	}

	cbuf->mbuf.data_buf.len = data_size;
	cbuf->mbuf.data_buf.buf = cycle_buffer_malloc(_info->cycbuf, data_size);
	if (cbuf->mbuf.data_buf.buf == NULL) {
		OSCL_LOGW("cache full:%d-%d, malloc size:%d!",
			_info->naudio, _info->nvideo, data_size);
		cycle_buffer_log_freesize(_info->cycbuf);
	}
	oscl_param_check_exit(cbuf->mbuf.data_buf.buf != NULL, -1, NULL);
	memcpy(cbuf->mbuf.data_buf.buf, data_addr, data_size);
EXIT:
	return ret;
}

static int __data_to_omxbuf(OMX_BUFFERHEADERTYPE *omx_buf,
	muxer_vpacket_t *venc_packet, colli_buffer_t *cbuf)
{
	oscl_param_check(cbuf != NULL, -1, NULL);
	oscl_param_check(omx_buf != NULL, -1, NULL);
	oscl_param_check(venc_packet != NULL, -1, NULL);

	memset(omx_buf, 0, sizeof(OMX_BUFFERHEADERTYPE));
	omx_buf->nInputPortIndex = cbuf->port_index;
	omx_buf->nTimeStamp = cbuf->timestamp;
	omx_buf->nFlags = cbuf->flag;
	if (omx_buf->nInputPortIndex == VIDEO_INDEX) {
		omx_buf->pBuffer = (void *)venc_packet;
		omx_buf->nAllocLen = sizeof(muxer_vpacket_t);
		omx_buf->nFilledLen = omx_buf->nAllocLen;
		memset(venc_packet, 0, sizeof(muxer_vpacket_t));
		venc_packet->vir_addr = cbuf->mbuf.data_buf.buf;
		venc_packet->size = cbuf->mbuf.data_buf.len;
	} else {
		omx_buf->nFilledLen = cbuf->mbuf.data_buf.len;
		omx_buf->nAllocLen = cbuf->mbuf.data_buf.len;
		omx_buf->pBuffer = cbuf->mbuf.data_buf.buf;
	}

	return 0;
}

static void *_colli_queue_drop_onebuf(colli_info_t *_info)
{
	colli_buffer_t *tmp_buffer = NULL;
	oscl_message_t *message = NULL;

	pthread_mutex_lock(&_info->lock);
	message = oscl_queue_query_next(&_info->q_colli, NULL);
	if ((message != NULL) && (message->data != NULL)) {
		tmp_buffer = message->data;
		if (_info->cmd != COLLI_CMD_NULL) {
			if (_info->muxering_stamp < 0 ||
				_info->muxering_stamp == tmp_buffer->timestamp) {
				OSCL_LOGI("try to drop muxing frame:%lld-%lld, %lld",
					(long long)_info->muxering_stamp,
					(long long)tmp_buffer->timestamp,
					(long long)_info->c_end);
				tmp_buffer = NULL;
			}
		}
		if (tmp_buffer)
			tmp_buffer = oscl_queue_dequeue(&_info->q_colli);
	}
	pthread_mutex_unlock(&_info->lock);

	if (tmp_buffer == NULL)
		goto EXIT;

	if (tmp_buffer->cache_mode == COLLI_CACHE_DATA)
		cycle_buffer_free(_info->cycbuf, tmp_buffer->mbuf.data_buf.buf);
	else {
		mux_context_return_buffer(_info->muxer_context,
			tmp_buffer->mbuf.buf_header);
	}

	if (tmp_buffer->port_index == VIDEO_INDEX)
		_info->nvideo--;
	else
		_info->naudio--;

	oscl_queue_queue(&_info->q_free_buf, tmp_buffer);

EXIT:
	return tmp_buffer;
}


static OMX_ERRORTYPE _colli_queue_update_time(colli_info_t *_info,
	OMX_TICKS time_stamp)
{
	int ret = OMX_ErrorNone;
	oscl_message_t *message = NULL;
	colli_buffer_t *tmp_buffer;
	OMX_TICKS tmp_time;

	oscl_param_check((_info != NULL), OMX_ErrorBadParameter, NULL);

	/* cur frame is video keyframe, and colli queue len > c_total,
	 * dequeue frames untill next key frame in colli queue*/
	do {
		message = oscl_queue_query_next(&_info->q_colli, NULL);
		if ((message == NULL) || (message->data == NULL))
			break;
		tmp_buffer = message->data;
		if (COLLI_IS_KEY_FRAME(_info->muxer_context, tmp_buffer)) {
			tmp_time = tmp_buffer->timestamp;
			if ((time_stamp - tmp_time <= _info->c_total)) {
				_info->c_start = tmp_time;
				OSCL_LOGI("v_start:%lld, end:%lld, total:%d",
					(long long)_info->c_start,
					(long long)_info->c_end,
					_info->c_total);
				break;
			}
		}
		tmp_buffer = _colli_queue_drop_onebuf(_info);
		if (tmp_buffer == NULL)
			oscl_mdelay(30);
	} while (tmp_buffer);

	if (oscl_queue_get_num(&_info->q_colli) == 0)
		OSCL_LOGW("collision queue is null after update time!");

	return ret;
}

static int _colli_queue_buf(colli_info_t *_info,
	OMX_BUFFERHEADERTYPE *buffer)
{
	int ret = OMX_ErrorNone;
	colli_buffer_t *cbuf = NULL;
	OMX_TICKS startpts = -1;
	OMX_TICKS curtime = -1;

	if (_info->c_start < 0 && !HEADER_IS_KEY_FRAME(_info->muxer_context, buffer)) {
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}
	if (buffer->nFilledLen == 0) {
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}

	do {
		if (cbuf == NULL)
			cbuf = oscl_queue_dequeue(&_info->q_free_buf);
		ret = __omxbuf_to_data(_info, cbuf, buffer);
		if (ret) {
			OSCL_LOGW("data buffer full, %lld, %lld, %lld. nbuf:%d %d",
				(long long)_info->c_start, (long long)_info->c_end,
				(long long)_info->muxering_stamp,
				_info->nvideo, _info->naudio);
			if (_info->muxering_stamp != -1 &&
				_info->c_start > _info->muxering_stamp)
				OSCL_LOGE("ERR!! starttime > muxingtime!");
			_colli_queue_drop_onebuf(_info);
			_colli_queue_update_time(_info, _info->c_end);
		}
	} while (ret != 0);

	pthread_mutex_lock(&_info->lock);
	oscl_queue_queue(&_info->q_colli, cbuf);

	if (COLLI_IS_KEY_FRAME(_info->muxer_context, cbuf)) {
		_info->c_end = buffer->nTimeStamp;
		if (_info->c_start < 0)
			_info->c_start = buffer->nTimeStamp;
	}

	if (cbuf->port_index == VIDEO_INDEX)
		_info->nvideo++;
	else
		_info->naudio++;

	if (_info->muxer_handle == NULL) {
		pthread_mutex_unlock(&_info->lock);
		goto EXIT;
	}
	if (cbuf->port_index == VIDEO_INDEX || !IS_VIDEO_EXIST(_info)) {
		startpts = muxer_handle_get_startpts(_info->muxer_handle,
			cbuf->port_index);
		if (startpts <= 0) {
			pthread_mutex_unlock(&_info->lock);
			goto EXIT;
		}
		curtime = cbuf->timestamp - startpts;
		if (cbuf->port_index == AUDIO_INDEX)
			curtime = AUDPTS_TO_TIME(_info->muxer_context, curtime);
		if (_info->start_normal_mux == 0 &&
			curtime >= _info->f_cfg.total_time - 500) {
			OSCL_LOGI("%p start normal muxer:%lld, %lld, %d", _info,
				(long long)startpts, (long long)cbuf->timestamp,
				_info->f_cfg.total_time);
			_info->start_normal_mux = 1;
			mux_context_start_normal(_info->muxer_context);
		}
		if (_info->stop_stamp < 0) {
			if (COLLI_IS_KEY_FRAME(_info->muxer_context, cbuf) &&
				curtime >= _info->f_cfg.total_time)
				_info->stop_stamp = cbuf->timestamp;
			if (curtime >= _info->f_cfg.total_time + 2000)
				_info->stop_stamp = cbuf->timestamp;
		}

	}
	pthread_mutex_unlock(&_info->lock);
EXIT:
	if (ret < 0 && cbuf)
		oscl_queue_queue(&_info->q_free_buf, cbuf);
	if (ret < 0 || cbuf->cache_mode == COLLI_CACHE_DATA)
		ret = mux_context_return_buffer(_info->muxer_context, buffer);
	return ret;
}

static void *_colli_muxer_thread(void *param)
{
	colli_info_t *_info = param;
	MuxerContext *context = NULL;
	oscl_message_t *message = NULL;
	oscl_message_t *next_message = NULL;
	muxer_hdl_t *muxer_handle = NULL;
	colli_buffer_t *cbuf;
	int start_flag = 0;
	OMX_BUFFERHEADERTYPE tmp_omx_buf;
	OMX_BUFFERHEADERTYPE *omx_buf = NULL;
	muxer_vpacket_t venc_packet;
	int64_t occur_pts;
	int ret = 0;
	int empty = 0;

	prctl(PR_SET_NAME, "colli_muxer");
	context = _info->muxer_context;

	if (IS_VIDEO_EXIST(_info)) {
		while (context->stream_cfg.is_ready[VIDEO_INDEX] == 0
			&& _info->cmd != COLLI_CMD_STOP) {
			OSCL_LOGE("video pps is not set, waiting");
			oscl_mdelay(100);
		}
	}
	muxer_handle = start_muxer_handle(&context->mux_hdl_mgr, &context->stream_cfg,
		_info->f_cfg.type, _info->f_cfg.filename, &_info->f_cfg.ext_writer);
	if (_info->f_cfg.filename) {
		oscl_free(_info->f_cfg.filename);
		_info->f_cfg.filename = NULL;
	}
	oscl_param_check_exit((muxer_handle != NULL), -1, NULL);
	occur_pts = _info->c_end;
	_info->muxer_handle = muxer_handle;

	while (1) {
		if (_info->cmd == COLLI_CMD_STOP) {
			OSCL_LOGE("get colli cmd %d in muxing colli cache", _info->cmd);
			break;
		}
		next_message = oscl_queue_query_next(&_info->q_colli, message);
		if ((next_message == NULL) || (next_message->data == NULL)) {
			if (empty == 0) {
				OSCL_LOGE("colli %p mux all buffers", _info);
				empty = 1;
			}
			oscl_mdelay(10);
			continue;
		}
		message = next_message;
		cbuf = message->data;
		pthread_mutex_lock(&_info->lock);
		_info->muxering_stamp = cbuf->timestamp;

		if (start_flag == 0 &&
			cbuf->timestamp >= _info->c_end - _info->f_cfg.cache_time) {
			if (COLLI_IS_KEY_FRAME(context, cbuf)) {
				start_flag = 1;
				OSCL_LOGE("colli %p start mux:%lld, %lld", _info,
					(long long)cbuf->timestamp,
					(long long)_info->c_end);
			}
		}
		pthread_mutex_unlock(&_info->lock);
		if (start_flag == 0)
			continue;

		if (_info->stop_stamp == cbuf->timestamp) {
			OMX_TICKS s_pts;
			s_pts = muxer_handle_get_startpts(muxer_handle, cbuf->port_index);
			OSCL_LOGE("==%p %s time start:%lld, stop:%lld, total:%lld",
				_info, muxer_handle->filename,
				(long long)s_pts,
				(long long)cbuf->timestamp,
				(long long)muxer_handle_get_time(muxer_handle));

			_info->stop_stamp = -1;
			break;
		}
		if (occur_pts == cbuf->timestamp)
			OSCL_LOGE("%p finish cache:%lld", _info, (long long)occur_pts);

		omx_buf = NULL;
		if (cbuf->cache_mode == COLLI_CACHE_DATA) {
			ret = __data_to_omxbuf(&tmp_omx_buf, &venc_packet, cbuf);
			if (ret == 0)
				omx_buf = &tmp_omx_buf;
		} else
			omx_buf = cbuf->mbuf.buf_header;
		if (omx_buf)
			muxer_handle_mux_buffer(muxer_handle, omx_buf);
		if (cbuf->port_index == VIDEO_INDEX || !IS_VIDEO_EXIST(_info))
			oscl_mdelay(10);
	}
	_info->muxering_stamp = -1;
	stop_muxer_handle(muxer_handle);
EXIT:
	if (_info->start_normal_mux == 0 && ret != 0) {
		OSCL_LOGE("%p start normal muxer", _info);
		_info->start_normal_mux = 1;
		mux_context_start_normal(_info->muxer_context);
	}
	pthread_mutex_lock(&_info->lock);
	_info->muxer_handle = NULL;
	_info->cmd = COLLI_CMD_NULL;
	pthread_mutex_unlock(&_info->lock);
	OSCL_LOGE("exit _colli_muxer_thread");
	return NULL;
}

OMX_ERRORTYPE colli_flush(void *colli_info)
{
	colli_info_t *_info = colli_info;
	colli_buffer_t *tmp_buffer = NULL;

	OSCL_TRACE("%p", _info);
	oscl_param_check(_info != NULL, OMX_ErrorBadParameter, NULL);
	if (_info->cmd != COLLI_CMD_NULL) {
		OSCL_LOGE("err! flush queue when collison is running");
		colli_stop(colli_info);
	}

	OSCL_LOGE("==");
	do {
		tmp_buffer = _colli_queue_drop_onebuf(_info);
	} while (tmp_buffer);
	OSCL_LOGE("==");
	pthread_mutex_lock(&_info->lock);
	_info->c_start = -1;
	_info->c_end = -1;
	pthread_mutex_unlock(&_info->lock);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE colli_cache_buffer(void *colli_info, OMX_BUFFERHEADERTYPE *buffer)
{
	int ret = OMX_ErrorNone;
	colli_info_t *_info = colli_info;

	OSCL_TRACE("%p:%p", colli_info, buffer);

	oscl_param_check((colli_info != NULL) && (buffer != NULL),
		OMX_ErrorBadParameter, NULL);

	/* collision queue will start with a video key buffer */
	ret = _colli_queue_buf(_info, buffer);
	if (ret != OMX_ErrorNone)
		goto EXIT;
	if (_info->c_end - _info->c_start >= _info->c_total)
		ret = _colli_queue_update_time(_info, _info->c_end);

EXIT:
	return ret;
}

int colli_stop(void *colli_info)
{
	colli_info_t *_info = colli_info;

	oscl_param_check((colli_info != NULL), OMX_ErrorBadParameter, NULL);
	pthread_mutex_lock(&_info->lock);
	if (_info->cmd != COLLI_CMD_NULL)
		OSCL_LOGE("receive stop cmd while cmd:%d unfinished.", _info->cmd);
	_info->cmd = COLLI_CMD_STOP;
	pthread_mutex_unlock(&_info->lock);
	if (_info->muxer_cache_thread) {
		pthread_join(_info->muxer_cache_thread, NULL);
		_info->muxer_cache_thread = 0;
		OSCL_LOGE("cmd:%d.", _info->cmd);
	}
	pthread_mutex_lock(&_info->lock);
	_info->cmd = COLLI_CMD_NULL;
	pthread_mutex_unlock(&_info->lock);

	return OMX_ErrorNone;
}

int colli_start(void *colli_info, colli_muxfile_cfg_t *colli_para)
{
	colli_info_t *_info = colli_info;
	int ret = -1;

	oscl_param_check((colli_info != NULL), OMX_ErrorBadParameter, NULL);
	oscl_param_check((colli_para != NULL), OMX_ErrorBadParameter, NULL);
	oscl_param_check((colli_para->filename || colli_para->ext_writer.write),
		OMX_ErrorBadParameter, NULL);
	oscl_param_check((colli_para->total_time > 0), OMX_ErrorBadParameter, NULL);
	oscl_param_check((colli_para->cache_time >= 0), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&_info->lock);
	if (colli_para->restart == 0 && _info->cmd == COLLI_CMD_START) {
		OSCL_LOGE("colli is in recording, ignore new file!");
		goto EXIT;
	}
	if (_info->muxer_cache_thread) {
		pthread_mutex_unlock(&_info->lock);
		colli_stop(colli_info);
		pthread_mutex_lock(&_info->lock);
	}
	if (_info->f_cfg.filename) {
		OSCL_LOGE("colli filename is not null, check it!");
		oscl_free(_info->f_cfg.filename);
		_info->f_cfg.filename = NULL;
	}
	OSCL_LOGE("%p stop normal muxer", _info);
	mux_context_stop_normal(_info->muxer_context);
	memcpy(&_info->f_cfg, colli_para, sizeof(*colli_para));
	if (colli_para->filename)
		_info->f_cfg.filename = oscl_strdup(colli_para->filename);
	_info->muxering_stamp = -1;
	_info->cmd = COLLI_CMD_START;
	_info->start_normal_mux = 0;
	_info->stop_stamp = -1;
	ret = pthread_create(&_info->muxer_cache_thread,
			NULL,
			_colli_muxer_thread,
			_info);
	if (ret != 0) {
		OSCL_LOGE("pthread_create muxer_cache_thread failed!");
		_info->muxer_cache_thread = 0;
		oscl_free(_info->f_cfg.filename);
		_info->cmd = COLLI_CMD_NULL;
		memset(&_info->f_cfg, 0, sizeof(*colli_para));
		mux_context_start_normal(_info->muxer_context);
	}
EXIT:
	pthread_mutex_unlock(&_info->lock);
	return ret;
}


void colli_deinit(void *colli_info)
{
	colli_info_t *_info = colli_info;
	if (_info == NULL)
		return;

	colli_stop(_info);
	colli_flush(_info);
	oscl_queue_deinit(&_info->q_colli);
	oscl_queue_deinit(&_info->q_free_buf);
	pthread_mutex_destroy(&_info->lock);
	if (_info->cycbuf)
		cycle_buffer_destory(_info->cycbuf);
	if (_info->media_buffer)
		oscl_free(_info->media_buffer);
	memset(_info, 0, sizeof(colli_info_t));
	oscl_free(_info);
}

void *colli_init(MuxerContext *context, long c_total)
{
	colli_info_t *info;
	int i;
	int ret = 0;
	long long cycle_buf_size = 0;

	info = oscl_zalloc(sizeof(colli_info_t));
	oscl_param_check(info != NULL, NULL, NULL);
	info->muxer_context = context;
	info->muxering_stamp = -1;
	info->c_total = (int)c_total * 1000;
	if (info->c_total <= 0)
		info->c_total = DEFAULT_COLLISION_TIME;
	info->c_total += 500;
	info->c_start = -1;
	info->c_end = -1;
	oscl_queue_init(&info->q_free_buf);
	oscl_queue_init(&info->q_colli);
	pthread_mutex_init(&info->lock, NULL);

	info->nbuffer = (info->c_total + 1000) * MAX_MEDIA_BUFFER_PER_SEC / 1000;
	info->media_buffer = oscl_zalloc(sizeof(colli_buffer_t) * info->nbuffer);
	oscl_param_check_exit(info->media_buffer != NULL, -1, NULL);
	for (i = 0; i < info->nbuffer; i++)
		oscl_queue_queue(&info->q_free_buf, &info->media_buffer[i]);

	cycle_buf_size = COLLI_AUDIO_BUF_SIZE;
	if (VIDEO_CACHE_MODE == COLLI_CACHE_DATA)
		cycle_buf_size += COLLI_VIDEO_BUF_SIZE;
	cycle_buf_size = cycle_buf_size * info->c_total / 1000;
	cycle_buf_size += COLLI_VIDEO_BUF_SIZE;
	info->cycbuf = cycle_buffer_creat_with_flag("colli", cycle_buf_size, 1, MEM_VIRT);
	oscl_param_check_exit(info->cycbuf != NULL, -1, NULL);
	cycle_buffer_set_block(info->cycbuf, 0);

EXIT:
	if (ret != 0) {
		colli_deinit(info);
		info = NULL;
	}
	return info;
}

void colli_print_dbg(void *colli_info)
{
	colli_info_t *_info = colli_info;
	int lock = 0;

	if (_info == NULL)
		return;

	if (pthread_mutex_trylock(&_info->lock) == 0) {
		lock = 1;
		OSCL_PRINT("flush_mutex is free\n");
	} else
		OSCL_PRINT("flush_mutex is busy\n");

	OSCL_PRINT("cmd:%d, context:%p, handle:%p, start normal:%d\n",
		_info->cmd, _info->muxer_context, _info->muxer_handle,
		_info->start_normal_mux);
	OSCL_PRINT("muxing stamp:%lld, stop stamp:%lld\n",
		(long long)_info->muxering_stamp, (long long)_info->stop_stamp);
	OSCL_PRINT("c_total:%d\n", _info->c_total);
	OSCL_PRINT("file_cache_time:%d, file_total_time:%d\n",
		_info->f_cfg.cache_time, _info->f_cfg.total_time);

	OSCL_PRINT("collision start:%lld end:%lld\n",
		(long long)_info->c_start, (long long)_info->c_end);
	OSCL_PRINT("video buf:%d audio buf:%d\n", _info->nvideo, _info->naudio);
	OSCL_PRINT("nbuffer:%d q_colli num:%d, q_free_buf:%d\n", _info->nbuffer,
		oscl_queue_get_num(&_info->q_colli),
		oscl_queue_get_num(&_info->q_free_buf));
	cycle_buffer_log_freesize(_info->cycbuf);

	if (lock)
		pthread_mutex_unlock(&_info->lock);

}

