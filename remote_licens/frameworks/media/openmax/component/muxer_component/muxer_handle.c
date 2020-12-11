/*
 * mp4_drv.c - Standard functionality for the media encodec component.
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
#define LOG_TAG			"muxer_common"

#include "oscl.h"
#include "base_component.h"
#include "muxer_com.h"
#include "av_media_type.h"
#include "muxer_common.h"
#include "audio_config.h"
#include "muxer_handle.h"
#include "muxer_private.h"
#include "muxer_collision.h"
#include "oscl_cache_file.h"

#define FHD_FILE_CACHE_SIZE     (512 * 1024)
#define FHD_FILE_CACHE_NUM      (5)
#define OTHER_FILE_CACHE_SIZE   (256 * 1024)
#define OTHER_FILE_CACHE_NUM    (5)

typedef enum Muxer_Handle_Staus {
	MUXER_INIT = 0,
	MUXER_MUXERING,
} Muxer_Handle_Staus;
static muxer_hdl_t *_get_muxer_handle(muxer_hdl_mgr_t *mgr)
{
	int i = 0;
	muxer_hdl_t *muxer = NULL;

	oscl_param_check(mgr != NULL, NULL, NULL);
	pthread_mutex_lock(&mgr->lock);
	for (i = 0; i < mgr->num; i++) {
		if (mgr->muxer_hdl[i].muxer_status == MUXER_INIT) {
			muxer = &(mgr->muxer_hdl[i]);
			muxer->muxer_status = MUXER_MUXERING;
			break;
		}
	}
	pthread_mutex_unlock(&mgr->lock);
	return muxer;
}

static int _put_muxer_handle(muxer_hdl_mgr_t *mgr, muxer_hdl_t *muxer)
{
	oscl_param_check(mgr != NULL, -1, NULL);
	oscl_param_check(muxer != NULL, -1, NULL);
	pthread_mutex_lock(&mgr->lock);
	muxer->muxer_status = MUXER_INIT;
	pthread_mutex_unlock(&mgr->lock);
	return 0;
}

static int _muxer_handle_deinit(muxer_hdl_t *muxer_handle)
{
	int ret = 0;

	oscl_param_check(muxer_handle != NULL, -1, NULL);
	if (muxer_handle->muxer) {
		ret = muxer_deinit(muxer_handle->muxer);
		muxer_handle->muxer = NULL;
	}
	if (muxer_handle->filename) {
		oscl_free(muxer_handle->filename);
		muxer_handle->filename = NULL;
	}
	if (muxer_handle->fp != FNULL) {
		oscl_cfile_close(muxer_handle->fp);
		muxer_handle->fp = FNULL;
	}

	muxer_handle->start_pts[AUDIO_INDEX] = -1;
	muxer_handle->start_pts[VIDEO_INDEX] = -1;
	muxer_handle->cur_time[AUDIO_INDEX] = -1;
	muxer_handle->cur_time[VIDEO_INDEX] = -1;

	return ret;
}

static int _muxer_handle_init(muxer_hdl_t *muxer_handle,
	muxer_stream_cfg_t *cfg, av_muxer_type_e id, char *filename)
{
	int i = 0;
	int ret = OMX_ErrorNone;
	format_muxer *muxer = NULL;
	int size_ex;
	int cache_num;
	int cache_size;
	AVIOFILE fp = FNULL;

	oscl_param_check(muxer_handle != NULL, -1, NULL);
	oscl_param_check(cfg != NULL, -1, NULL);
	oscl_param_check(id != AV_MUXER_TYPE_INVALID, -1, NULL);
	if (cfg->is_ready[VIDEO_INDEX] == 0 && cfg->exist[VIDEO_INDEX]) {
		OSCL_LOGE("video pps is not set before muxing");
		ret = -1;
		goto EXIT;
	}

	if (id != AV_MUXER_TYPE_RAW) {
		fp = oscl_cfile_open(filename, FWRMODE);
		if (fp == FNULL) {
			OSCL_LOGE("open:%s failed", filename);
			ret = -1;
			goto EXIT;
		}
		cache_num = OTHER_FILE_CACHE_NUM;
		cache_size = OTHER_FILE_CACHE_SIZE;
		if (cfg->para[VIDEO_INDEX].height >= 1920
			|| cfg->para[VIDEO_INDEX].width >= 1920) {
			cache_num = FHD_FILE_CACHE_NUM;
			cache_size = FHD_FILE_CACHE_SIZE;
		}
		oscl_cfile_alloc_cache(fp, cache_num, cache_size);
		muxer_handle->filename = oscl_strdup(filename);
	}

	muxer_handle->fp = fp;
	muxer = oscl_zalloc(sizeof(format_muxer));
	oscl_param_check(muxer != NULL, -1, NULL);
	muxer_handle->muxer = muxer;
	/* set muxer para */
	for (i = 0; i < cfg->nstream; i++) {
		size_ex = cfg->para[i].extradata_size + AV_INPUT_BUFFER_PADDING_SIZE;
		memcpy(&(muxer->para[i]), &(cfg->para[i]), sizeof(AV_CodecParameters));
		if (cfg->para[i].extradata != NULL) {
			muxer->para[i].extradata = oscl_zalloc(size_ex);
			memcpy(muxer->para[i].extradata,
				cfg->para[i].extradata,
				muxer->para[i].extradata_size);
		}
	}

	/* muxer init */
	ret = muxer_init(muxer, id, fp);
	muxer_handle->start_pts[AUDIO_INDEX] = -1;
	muxer_handle->start_pts[VIDEO_INDEX] = -1;
	muxer_handle->cur_time[AUDIO_INDEX] = -1;
	muxer_handle->cur_time[VIDEO_INDEX] = -1;

EXIT:
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("muxer init failed!");
		_muxer_handle_deinit(muxer_handle);
	}
	return ret;
}

static int _muxer_audio_buffer(muxer_hdl_t *muxer_handle, OMX_BUFFERHEADERTYPE *abuf)
{
	int ret = 0;
	int index;
	packet_t pin = { 0 };

	oscl_param_check(muxer_handle != NULL, -1, NULL);
	oscl_param_check(muxer_handle->muxer != NULL, -1, NULL);

	index = abuf->nInputPortIndex;
	pin.data = abuf->pBuffer + abuf->nOffset;
	pin.stream_index = index;
	pin.size = abuf->nFilledLen;
	if (muxer_handle->start_pts[index] < 0) {
		OSCL_LOGE("muxer_handle:%p, p[%d] start pts %lld",
			muxer_handle, index, abuf->nTimeStamp);
		muxer_handle->start_pts[index] = abuf->nTimeStamp;
	}
	pin.pts = abuf->nTimeStamp - muxer_handle->start_pts[index];
	pin.dts = pin.pts;
	if (pin.dts < 0) {
		OSCL_LOGW("pin.dts %lld - %lld - %lld\n", (OMX_S64)pin.dts,
			abuf->nTimeStamp, muxer_handle->start_pts[index]);
	}
	pin.timestamp = pin.pts * 1000 / muxer_handle->muxer->para[index].sample_rate;
	muxer_handle->cur_time[index] = pin.timestamp;

	ret = muxer_write_packet(muxer_handle->muxer, &pin);
	if (ret < 0)
		OSCL_LOGE("err ret :%d\n", ret);

	return ret;
}


static int _muxer_video_buffer(muxer_hdl_t *muxer_handle, OMX_BUFFERHEADERTYPE *vbuf)
{
	int ret = 0;
	int index;
	packet_t pin = { 0 };
	muxer_vpacket_t *venc_packet;

	index = vbuf->nInputPortIndex;
	venc_packet = (muxer_vpacket_t *)vbuf->pBuffer;
	oscl_param_check(venc_packet != NULL, -1, NULL);
	oscl_param_check(muxer_handle != NULL, -1, NULL);
	oscl_param_check(muxer_handle->muxer != NULL, -1, NULL);

	pin.data = venc_packet->vir_addr;
	pin.stream_index = index;
	pin.size = venc_packet->size;
	if (muxer_handle->start_pts[index] < 0) {
		OSCL_LOGE("muxer_handle:%p, p[%d] start pts %lld",
			muxer_handle, index, vbuf->nTimeStamp);
		muxer_handle->start_pts[index] = vbuf->nTimeStamp;
	}
	pin.timestamp = vbuf->nTimeStamp - muxer_handle->start_pts[index];
	muxer_handle->cur_time[index] = pin.timestamp;
	pin.pts = pin.timestamp * muxer_handle->muxer->para[index].sample_rate / 1000;
	pin.dts = pin.pts;
	if (pin.dts < 0) {
		OSCL_LOGI("pin.dts %lld - %lld - %lld\n", (OMX_S64)pin.dts,
			vbuf->nTimeStamp, muxer_handle->start_pts[index]);
	}

	if (vbuf->nFlags & OMX_BUFFERFLAG_SYNCFRAME)
		pin.flags |= AV_PKT_FLAG_KEY;

	ret = muxer_write_packet(muxer_handle->muxer, &pin);
	if (ret < 0)
		OSCL_LOGE("err ret :%d\n", ret);
	return ret;

}

static int _stop_muxer(muxer_hdl_t *muxer_handle)
{
	int ret = 0;
	lb_omx_component_t *component = (lb_omx_component_t *)(muxer_handle->component);
	MuxerContext *context = component->component_private;

	if (muxer_handle->fp != FNULL) {
		ret = muxer_write_trailer(muxer_handle->muxer);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("muxer_write_trailer error\n");
		oscl_cfile_close(muxer_handle->fp);
		muxer_handle->fp = FNULL;
		if (component && component->callbacks.EventHandler) {
			((component->callbacks.EventHandler))(&component->base_comp,
				component->callback_data, OMX_Eventfileclosed,
				0, 0, muxer_handle->filename);
		}
	}
	ret = _muxer_handle_deinit(muxer_handle);
	if (ret != OMX_ErrorNone)
		OSCL_LOGE("muxer_deinit error\n");
	_put_muxer_handle(&context->mux_hdl_mgr, muxer_handle);

	return ret;
}

static void *_stop_muxer_thread(void *parm)
{
	prctl(PR_SET_NAME, "stop_muxer_thread");
	pthread_detach(pthread_self());
	_stop_muxer((muxer_hdl_t *)parm);
	return NULL;
}

static muxer_hdl_t *_start_muxer_handle_rawdata(muxer_hdl_mgr_t *mgr,
	muxer_stream_cfg_t *cfg, external_stream_writer_t *writer)
{
	int ret = OMX_ErrorNone;
	muxer_hdl_t *muxer_handle = NULL;

	oscl_param_check(mgr != NULL, NULL, NULL);
	oscl_param_check(cfg != NULL, NULL, NULL);
	oscl_param_check(writer != NULL, NULL, NULL);
	oscl_param_check(writer->write != NULL, NULL, NULL);

	muxer_handle = _get_muxer_handle(mgr);
	oscl_param_check(muxer_handle != NULL, NULL, NULL);
	ret = _muxer_handle_init(muxer_handle, cfg, AV_MUXER_TYPE_RAW, NULL);
	oscl_param_check_exit(ret == OMX_ErrorNone, -1, NULL);
	ret = muxer_ctrl(muxer_handle->muxer, MUXER_CMD_SET_STREAM_CB, writer);
	OSCL_LOGE("======start muxer handle:%p=====", muxer_handle);

EXIT:
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("init muxer failed!");
		_muxer_handle_deinit(muxer_handle);
		_put_muxer_handle(mgr, muxer_handle);
		muxer_handle = NULL;
	}
	return muxer_handle;
}

static muxer_hdl_t *_start_muxer_handle_file(muxer_hdl_mgr_t *mgr,
	muxer_stream_cfg_t *cfg, av_muxer_type_e type, char *filename)
{
	int ret = OMX_ErrorNone;
	muxer_hdl_t *muxer_handle = NULL;

	oscl_param_check(mgr != NULL, NULL, NULL);
	oscl_param_check(cfg != NULL, NULL, NULL);
	if (filename == NULL)
		goto EXIT;

	muxer_handle = _get_muxer_handle(mgr);
	oscl_param_check(muxer_handle != NULL, NULL, NULL);

	ret = _muxer_handle_init(muxer_handle, cfg, type, filename);
	oscl_param_check_exit(ret == OMX_ErrorNone, -1, NULL);

	ret = muxer_write_header(muxer_handle->muxer);
	if (ret != OMX_ErrorNone)
		OSCL_LOGE("write_header error:%d", ret);

	OSCL_LOGE("======start muxer handle:%s, ret:%d=====", filename, ret);
EXIT:
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("init muxer failed!");
		_muxer_handle_deinit(muxer_handle);
		_put_muxer_handle(mgr, muxer_handle);
		muxer_handle = NULL;
	}
	return muxer_handle;
}

muxer_hdl_t *start_muxer_handle(muxer_hdl_mgr_t *mgr, muxer_stream_cfg_t *cfg,
	av_muxer_type_e type, char *filename, external_stream_writer_t *writer)
{
	muxer_hdl_t *muxer = NULL;

	oscl_param_check(mgr != NULL, NULL, NULL);
	oscl_param_check(cfg != NULL, NULL, NULL);
	if (type == AV_MUXER_TYPE_RAW)
		muxer = _start_muxer_handle_rawdata(mgr, cfg, writer);
	else
		muxer = _start_muxer_handle_file(mgr, cfg, type, filename);

	return muxer;
}

int stop_muxer_handle(muxer_hdl_t *muxer_handle)
{
	int ret;
	pthread_t file_close_thread;
	pthread_attr_t thread_attr;
#ifdef __EOS__
	struct sched_param shed_param = {0};
#endif
	oscl_param_check(muxer_handle != NULL, -1, NULL);
	oscl_param_check(muxer_handle->muxer_status == MUXER_MUXERING, -1, NULL);

	OSCL_LOGE("== %s time start:%lld, stop:%lld, total:%lld",
		muxer_handle->filename,
		muxer_handle_get_startpts(muxer_handle, VIDEO_INDEX),
		muxer_handle->cur_time[VIDEO_INDEX],
		muxer_handle_get_time(muxer_handle));

	pthread_attr_init(&thread_attr);
#ifdef __EOS__
	shed_param.sched_priority = LB_FILE_CLOSE_PRIO;
	pthread_attr_setstacksize(&thread_attr, 0x1000);
	pthread_attr_setschedparam(&thread_attr, &shed_param);
#endif
	ret = pthread_create(&file_close_thread, &thread_attr,
			_stop_muxer_thread, muxer_handle);
	if (ret != 0) {
		OSCL_LOGE("creat _stop_muxer_thread error : %d\n", ret);
		ret = _stop_muxer(muxer_handle);
	}
	return ret;
}

int muxer_handle_mux_buffer(muxer_hdl_t *muxer_handle,
	OMX_BUFFERHEADERTYPE *inbuffer)
{
	int ret = 0;
	if (muxer_handle == NULL || inbuffer == NULL)
		return 0;
	if (inbuffer->nInputPortIndex == AUDIO_INDEX)
		ret = _muxer_audio_buffer(muxer_handle, inbuffer);
	if (inbuffer->nInputPortIndex == VIDEO_INDEX)
		ret = _muxer_video_buffer(muxer_handle, inbuffer);
	return ret;
}

OMX_TICKS muxer_handle_get_time(muxer_hdl_t *muxer_handle)
{
	OMX_TICKS ret = 0;

	oscl_param_check(muxer_handle != NULL, 0, NULL);

	if (muxer_handle->cur_time[VIDEO_INDEX] >= muxer_handle->cur_time[AUDIO_INDEX])
		ret = muxer_handle->cur_time[VIDEO_INDEX];
	else
		ret = muxer_handle->cur_time[AUDIO_INDEX];

	if (ret < 0)
		ret = 0;
	return ret;
}

OMX_TICKS muxer_handle_get_startpts(muxer_hdl_t *muxer_handle, int index)
{
	oscl_param_check(muxer_handle != NULL, -1, NULL);
	oscl_param_check(index == VIDEO_INDEX || index == AUDIO_INDEX, -1, NULL);

	return muxer_handle->start_pts[index];
}

static int muxer_hdl_manger_init(muxer_hdl_mgr_t *mgr, void *component)
{
	int i;
#if 0
	int ret;
	pthread_attr_t thread_attr;
#endif
	muxer_hdl_t *muxer_handle;
#ifdef __EOS__
	struct sched_param shed_param = {0};
#endif

	mgr->num = MUXER_NUM;
	pthread_mutex_init(&mgr->lock, NULL);
	for (i = 0; i < MUXER_NUM; i++) {
		muxer_handle = &(mgr->muxer_hdl[i]);
		memset(muxer_handle, 0, sizeof(muxer_hdl_t));
		muxer_handle->filename = NULL;
		muxer_handle->fp = FNULL;
		muxer_handle->component = component;
	}
	return 0;
}

static int muxer_hdl_manger_deinit(muxer_hdl_mgr_t *mgr)
{
	int i;
	muxer_hdl_t *muxer_handle;
	for (i = 0; i < mgr->num; i++) {
		muxer_handle = &(mgr->muxer_hdl[i]);
		if (muxer_handle->muxer_status != MUXER_INIT)
			OSCL_LOGE("muxer is busy, need debug\n");

		_muxer_handle_deinit(muxer_handle);
	}
	pthread_mutex_destroy(&mgr->lock);
	return 0;
}

int mux_context_wait_idle(muxer_hdl_mgr_t *mgr)
{
	OMX_S32 i = 0;

	for (i = 0; i < mgr->num; i++) {
		while (!(mgr->muxer_hdl[i].muxer_status == MUXER_INIT)) {
			OSCL_LOGI("wait muxer handle to be idle, stat[%d]:%d\n",
					i, mgr->muxer_hdl[i].muxer_status);
			oscl_mdelay(10);
		}
	}
	return 0;
}

MuxerContext *mux_context_init(lb_omx_component_t *component)
{
	MuxerContext *context = NULL;
	context = oscl_zalloc(sizeof(MuxerContext));
	oscl_param_check((context != NULL), NULL, NULL);
	pthread_mutex_init(&context->context_mutex, NULL);
	context->lb_comp = component;
	context->ref_time[AUDIO_INDEX] = LLONG_MAX;
	context->ref_time[VIDEO_INDEX] = LLONG_MAX;
	context->id = AV_MUXER_TYPE_MOV;
	context->stream_cfg.para[0].codec_type = AV_MEDIA_TYPE_INVALID;
	context->stream_cfg.para[1].codec_type = AV_MEDIA_TYPE_INVALID;
	context->stream_cfg.nstream = 2;
	muxer_hdl_manger_init(&context->mux_hdl_mgr, component);

	return context;
}

int mux_context_deinit(MuxerContext *context)
{
	int i = 0;

	if (context->colli_info) {
		colli_deinit(context->colli_info);
		context->colli_info = NULL;
	}
	muxer_hdl_manger_deinit(&context->mux_hdl_mgr);
	mux_context_free_combined_packet(context);
	if (context->filename != NULL) {
		oscl_free(context->filename);
		context->filename = NULL;
	}
	for (i = 0; i < context->stream_cfg.nstream; i++) {
		if (context->stream_cfg.para[i].extradata) {
			oscl_free(context->stream_cfg.para[i].extradata);
			context->stream_cfg.para[i].extradata = NULL;
			context->stream_cfg.para[i].extradata_size = 0;
		}
	}
	pthread_mutex_destroy(&context->context_mutex);
	if (NULL != context)
		oscl_free(context);
	return 0;
}

void mux_context_free_combined_packet(MuxerContext *context)
{
	if (context->combine_packet.vir_addr != NULL)
		oscl_free(context->combine_packet.vir_addr);
	if (context->combine_header)
		oscl_free(context->combine_header);
	memset(&context->combine_packet, 0, sizeof(muxer_vpacket_t));
	context->combine_header = NULL;
	context->rebuilding_flag = 0;
}

int mux_context_start_normal(MuxerContext *context)
{
	oscl_param_check(context != NULL, -1, NULL);
	pthread_mutex_lock(&context->context_mutex);
	if ((context->ctrl_flag | CONTEXT_STOP_MASK) == 0)
		OSCL_LOGE("muxer not stopped when starting(0x%x)", context->ctrl_flag);
	context->ctrl_flag &= (~CONTEXT_STOP_MASK);
	context->ctrl_flag |= CONTEXT_FILENAME_MASK;
	pthread_mutex_unlock(&context->context_mutex);
	return 0;
}

int mux_context_stop_normal(MuxerContext *context)
{
	oscl_param_check(context != NULL, -1, NULL);
	pthread_mutex_lock(&context->context_mutex);
	if (context->ctrl_flag)
		OSCL_LOGE("err! ctrl_flag = 0x%x", context->ctrl_flag);
	if (context->mux_hdl == NULL || (context->ctrl_flag | CONTEXT_STOP_MASK))
		OSCL_LOGI("muxer is already stopped(%p - %x)",
			context->mux_hdl, context->ctrl_flag);
	context->ctrl_flag |= CONTEXT_STOP_MASK;
	pthread_mutex_unlock(&context->context_mutex);
	return 0;
}

OMX_ERRORTYPE mux_context_return_buffer(MuxerContext *context,
	OMX_BUFFERHEADERTYPE *buffer)
{
	OMX_U32 port_idx;
	base_port_t *port;
	lb_omx_component_t *component;
	int ret = 0;

	oscl_param_check((context != NULL) && (buffer != NULL),
		OMX_ErrorBadParameter, NULL);

	component = (lb_omx_component_t *)context->lb_comp;
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);

	port_idx = buffer->nInputPortIndex;
	oscl_param_check((port_idx == VIDEO_INDEX) || (port_idx == AUDIO_INDEX),
		OMX_ErrorBadParameter, NULL);
	port = &component->port[port_idx];
	buffer->nFilledLen = 0;
	buffer->nTimeStamp = 0;
	buffer->nFlags = 0;
	if (buffer == context->combine_header)
		mux_context_free_combined_packet(context);
	else
		ret = port->return_buffer(port, buffer);
	return ret;
}

void log_muxer_handle(muxer_hdl_t *hdl)
{

	if (hdl->muxer_status != MUXER_INIT && hdl->muxer_status != MUXER_MUXERING) {
		OSCL_PRINT("%p: err state:%d\n", hdl, hdl->muxer_status);
		return;
	}
	OSCL_PRINT("muxer_hdl_t %p: state:%d, %p\n",
		hdl, hdl->muxer_status, hdl->component);
	if (hdl->muxer_status == MUXER_INIT)
		return;
	OSCL_PRINT("muxer:%p, fp:%d, file:%p\n", hdl->muxer, (int)hdl->fp, hdl->filename);
	if (hdl->filename)
		OSCL_PRINT("file name:%s\n", hdl->filename);
	OSCL_PRINT("startpts:%lld, %lld\n", hdl->start_pts[0], hdl->start_pts[1]);
	OSCL_PRINT("curtime:%lld, %lld\n", hdl->cur_time[0], hdl->cur_time[1]);
}

void log_muxer_context(MuxerContext *context)
{
	int lock = 0;
	int i;

	if (context == NULL)
		return;

	if (pthread_mutex_trylock(&context->context_mutex) == 0) {
		lock = 1;
		OSCL_PRINT("context_mutex is free\n");
	} else
		OSCL_PRINT("context_mutex is busy\n");

	OSCL_PRINT("context:%p comp:%p\n", context, context->lb_comp);
	for (i = 0; i <  context->mux_hdl_mgr.num; i++)
		log_muxer_handle(&context->mux_hdl_mgr.muxer_hdl[i]);
	OSCL_PRINT("reftime:%lld-%lld\n",
		(long long)context->ref_time[0], (long long)context->ref_time[1]);
	OSCL_PRINT("inbuf time:%lld-%lld\n",
		(long long)context->intime[0], (long long)context->intime[1]);
	OSCL_PRINT("last update ref time:%lld\n", (long long)context->sync_time);

	if (context->colli_info)
		colli_print_dbg(context->colli_info);

	if (lock)
		pthread_mutex_unlock(&context->context_mutex);

}

