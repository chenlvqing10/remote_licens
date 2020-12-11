#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "vdecoder"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <oscl.h>
#include <av_media_type.h>
#include <lombo_decplugin.h>
#include "vdecoder.h"
#include "vdec_lib_handle.h"

/* #define DEBUG_VDECODER */

/* #define DBG_VDEC_SAVE_PKT	"/mnt/usb1/test/packet/vdec_pkt.dat" */
/* #define DBG_VDEC_SAVE_YUV	"/mnt/usb1/test/yuv/vdec_yuv" */
#define DBG_SAVE_YUV_START_TS	(1LL*(-1))
#define DBG_SAVE_YUV_END_TS	(7200LL*1000*1000)

typedef void *(*get_plugin)(void);

typedef struct video_decoder {
	av_codec_type_e id;
	vdec_plugin_t *plugin;
	void *lib_hdl;
	void *codec;
	int ref_bufnum;
	int reg_bufnum;
	int req_bufnum;
	int hold_bufnum;
	int buf_size;
	int header_decoded;
	int drop_flags;
	pthread_cond_t cond;
	pthread_mutex_t lock;
	pthread_mutex_t deq_lock;

	int eos_reached;
	int deq_err_cnt;
	int need_reset;

#ifdef DEBUG_VDECODER
	int64_t pre_ts;
#endif
#ifdef DBG_VDEC_SAVE_PKT
	int pktfd;
#endif
#ifdef DBG_VDEC_SAVE_YUV
	int yuvfd;
	int frame_size;
#endif
} video_decoder_t;

#if 0
static void _save_data(char *filename, uint8_t *data, size_t size)
{
	int fd;

	fd = open(filename, O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		OSCL_LOGE("open file %s error(%s)!", filename, strerror(errno));
	} else {
		int wlen = write(fd, data, size);
		if (wlen != size)
			OSCL_LOGE("write file %s error(%s)!", filename, strerror(errno));
		close(fd);
	}
}

static void _save_pkt_data(char *path, vdec_packet_t *pkt, int pkt_cnt)
{
	struct stat st;
	char filename[256];
	int64_t curTime = oscl_get_usec();

	if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
		mkdir(path, 0777);

	snprintf(filename, 256, "%s/pkt_%04d_%d_%lld_%08x_%lld.h264", path, pkt_cnt,
		pkt->len, pkt->ts, pkt->flags, curTime);
	_save_data(filename, pkt->vir_addr, pkt->len);
}

static void _save_raw_data(char *path, vdec_frame_t *frame)
{
	struct stat st;
	char filename[256];
	int64_t curTime = oscl_get_usec();
	int len = frame->width_stride * frame->height_aligned * 3 / 2;

	if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
		mkdir(path, 0777);

	snprintf(filename, 256, "%s/frame_%lld_%d_%08x_%lld.yuv", path,
		frame->time_stamp , len, frame->flags, curTime);
	_save_data(filename, frame->vir_addr, len);
}
#endif

void *vdecoder_open(av_codec_type_e id, vdec_init_param_t *param, int *is_vdc)
{
	video_decoder_t *priv;
	get_plugin plugin_func;
	void *lib_hdl;

	lib_hdl = open_vdec_lib(id, is_vdc);
	if (lib_hdl == NULL)
		return NULL;

	plugin_func = (get_plugin)dlsym(lib_hdl, "get_plugin_info");
	if (plugin_func == NULL) {
		OSCL_LOGE("get plugin info error!");
		goto ERR_0;
	}

	priv = (video_decoder_t *)oscl_malloc(sizeof(video_decoder_t));
	if (priv == NULL) {
		OSCL_LOGE("alloc vdecoder error!");
		goto ERR_0;
	}
	memset(priv, 0, sizeof(video_decoder_t));
	priv->id = id;
	priv->lib_hdl = lib_hdl;

	priv->plugin = plugin_func();
	if (priv->plugin == NULL) {
		OSCL_LOGE("get plugin method error!");
		goto ERR_1;
	}

	OSCL_LOGI("codec %x, size(%dx%d), extra=%p, extra_size=%d, output_mode=%d",
		param->vdc_init_parm.codec_id,
		param->vdc_init_parm.src_width,
		param->vdc_init_parm.src_height,
		param->vdc_init_parm.extra_data,
		param->vdc_init_parm.extradata_len,
		param->vdc_init_parm.output_mode);
	priv->codec = priv->plugin->open(param);
	if (priv->codec == NULL) {
		OSCL_LOGE("open codec error!");
		goto ERR_1;
	}

	pthread_cond_init(&priv->cond, NULL);
	pthread_mutex_init(&priv->lock, NULL);
	pthread_mutex_init(&priv->deq_lock, NULL);

	priv->need_reset = 1;

#ifdef DBG_VDEC_SAVE_PKT
	priv->pktfd = open(DBG_VDEC_SAVE_PKT, O_RDWR | O_CREAT, 0666);
	if (priv->pktfd < 0)
		OSCL_LOGE("open packet file error!");
#endif
#ifdef DBG_VDEC_SAVE_YUV
	int i;
	char fname[256];
	for (i = 0; i < 512; i++) {
		snprintf(fname, 256, "%s_%03d_%dx%d.yuv",
			DBG_VDEC_SAVE_YUV, i,
			param->vdc_init_parm.src_width,
			param->vdc_init_parm.src_height);
		if (access(fname, F_OK) < 0)
			break;
	}
	OSCL_LOGI("open yuv file %s", fname);
	priv->yuvfd = open(fname, O_RDWR | O_CREAT, 0666);
	if (priv->yuvfd < 0)
		OSCL_LOGE("open %s error!", fname);
	priv->frame_size = 0;
#endif

	return priv;

ERR_1:
	free(priv);
ERR_0:
	close_vdec_lib(lib_hdl);
	return NULL;
}

int vdecoder_close(void *handle)
{
	video_decoder_t *priv = (video_decoder_t *)handle;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	pthread_cond_destroy(&priv->cond);
	pthread_mutex_destroy(&priv->lock);
	pthread_mutex_destroy(&priv->deq_lock);
#ifdef DBG_VDEC_SAVE_PKT
	close(priv->pktfd);
#endif
#ifdef DBG_VDEC_SAVE_YUV
	if (priv->yuvfd >= 0)
		close(priv->yuvfd);
#endif
	priv->plugin->close(priv->codec);
	priv->codec = NULL;

	close_vdec_lib(priv->lib_hdl);
	free(priv);

	return 0;
}

int vdecoder_get_info(void *handle, vdec_info_t *info)
{
	video_decoder_t *priv = (video_decoder_t *)handle;
	int ret;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	ret = priv->plugin->get_info(priv->codec, info);
	if (ret != 0) {
		OSCL_LOGE("get_info error(%d)!", ret);
		return ret;
	}
	priv->ref_bufnum = info->buf_num;
	info->buf_num += 5;
	if (info->buf_num > info->max_buf_num)
		info->buf_num = info->max_buf_num;
	priv->req_bufnum = info->buf_num;
	priv->reg_bufnum = 0;
	priv->header_decoded = 1;
	priv->buf_size = info->buf_size;
#ifdef DBG_VDEC_SAVE_YUV
	priv->frame_size = info->pic_width * info->pic_height * 3 / 2;
#endif

	OSCL_LOGI("pic[%dx%d], crop[%d %d %d %d], bufsize[%d], refnum[%d], bufnum[%d]",
		info->pic_width,
		info->pic_height,
		info->crop.crop_x,
		info->crop.crop_y,
		info->crop.crop_width,
		info->crop.crop_height,
		info->buf_size,
		priv->ref_bufnum,
		info->buf_num);

	if (info->pic_width <= 0
		|| info->pic_height <= 0
		|| info->crop.crop_width <= 0
		|| info->crop.crop_height <= 0
		|| info->buf_size <= 0
		|| info->buf_num <= 0) {
		OSCL_LOGE("get_info param error!");
		return -1;
	}

	return 0;
}

int vdecoder_decode_frame(void *handle, vdec_packet_t *pkt)
{
	video_decoder_t *priv = (video_decoder_t *)handle;
	int ret;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	if (!priv->header_decoded)
		goto DECODE_PKT;

	pthread_mutex_lock(&priv->lock);
	while (priv->reg_bufnum < priv->req_bufnum
		/* || priv->hold_bufnum < priv->ref_bufnum */
		) {
		ret = oscl_cond_timedwait_s(&priv->cond, &priv->lock, 1);
		pthread_mutex_unlock(&priv->lock);
		if (ETIMEDOUT == ret) {
			OSCL_LOGW("wait buf timeout, total %d, ref %d, reg %d, hold %d",
				priv->req_bufnum, priv->ref_bufnum,
				priv->reg_bufnum, priv->hold_bufnum);
			return VDEC_BUF_NOT_ENOUGH;
		}
		pthread_mutex_lock(&priv->lock);
	}
	pthread_mutex_unlock(&priv->lock);

DECODE_PKT:
#ifdef DBG_VDEC_SAVE_PKT
	write(priv->pktfd, pkt->vir_addr, pkt->len);
#endif
	if (pkt->flags & AV_PACKET_FLAG_RESET) {
		OSCL_LOGI("start ts %lld, size %u, flags 0x%x",
			pkt->ts, pkt->len, pkt->flags);
		ret = priv->plugin->ex_ops(priv->codec, VDEC_CMD_FLUSH, NULL);
		if (ret != 0)
			OSCL_LOGE("VDEC_CMD_FLUSH error[%d]", ret);
		priv->eos_reached = 0;

#ifdef DEBUG_VDECODER
		priv->pre_ts = 0;
#endif
	}
	if (pkt->flags & AV_PACKET_FLAG_EOS)
		OSCL_LOGI("EOS ts %lld, size %u", pkt->ts, pkt->len);

	if (priv->drop_flags & AV_PACKET_FLAG_RESET) {
		pkt->flags |= AV_PACKET_FLAG_RESET;
		priv->drop_flags = 0;
	}

	OSCL_LOGD("hold %d, packet decoding: size=%d,ts=%lld,vir=%p,flag=%08x",
		priv->hold_bufnum, pkt->len, pkt->ts, pkt->vir_addr, pkt->flags);
	ret = priv->plugin->decode_frame(priv->codec, pkt);
	OSCL_LOGD("packet decoded: ret=%d", ret);
	if (ret != VDEC_OK) {
		if (ret < 0)
			OSCL_LOGE("error(%d)", ret);
		if (pkt->flags & AV_PACKET_FLAG_RESET)
			priv->drop_flags |= AV_PACKET_FLAG_RESET;
	}
	if (pkt->flags & AV_PACKET_FLAG_EOS)
		priv->eos_reached = 1;

	return ret;
}

int vdecoder_queue_buf(void *handle, vdec_frame_t *frame)
{
	video_decoder_t *priv = (video_decoder_t *)handle;
	int ret;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	if (priv->reg_bufnum < priv->req_bufnum) {
		vdec_alloc_buf_t buf;
		buf.buf_size = priv->buf_size;
		buf.total_buf_num = priv->req_bufnum;
		buf.vir_addr  = frame->vir_addr;
		buf.phy_addr = frame->phy_addr;
		ret = priv->plugin->register_buf(priv->codec, &buf);
		if (ret != 0) {
			OSCL_LOGE("register_buf error(%d)", ret);
			return ret;
		}
		priv->reg_bufnum++;
		OSCL_LOGI("reg buf %p", buf.vir_addr);
	} else {
		ret = priv->plugin->queue_buf(priv->codec, frame);
		if (ret != 0) {
			OSCL_LOGE("queue_buf error(%d)", ret);
			return ret;
		}
	}

	pthread_mutex_lock(&priv->lock);
	priv->hold_bufnum++;
	pthread_cond_signal(&priv->cond);
	OSCL_LOGD("total %d, hold %d, vir %p, phy %lu, ts=%lld, w=%d, h=%d",
		priv->req_bufnum, priv->hold_bufnum,
		frame->vir_addr, frame->phy_addr, frame->time_stamp,
		frame->width_stride, frame->height_aligned);
	pthread_mutex_unlock(&priv->lock);

	return 0;
}

int vdecoder_dequeue_buf(void *handle, vdec_frame_t *frame, int timeout_ms)
{
	video_decoder_t *priv = (video_decoder_t *)handle;
	int ret;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	OSCL_LOGD("start");
	pthread_mutex_lock(&priv->deq_lock);
	ret = priv->plugin->dequeue_buf(priv->codec, frame, timeout_ms);
	pthread_mutex_unlock(&priv->deq_lock);
	if (ret != VDEQBUF_OK) {
		OSCL_LOGW("dequeue_buf err[%d]:total %d, hold %d, eos_reached %d",
			ret, priv->req_bufnum, priv->hold_bufnum, priv->eos_reached);
		if (priv->eos_reached == 1 && priv->deq_err_cnt > 0) {
			OSCL_LOGW("dequeue_buf err while eos reached!");
			frame->flags |= AV_PACKET_FLAG_EOS;
			priv->eos_reached = 2;
		}
		priv->deq_err_cnt++;
		return ret;
	}
	if (frame->vir_addr == NULL) {
		OSCL_LOGE("vir_addr=NULL!");
		return -1;
	}
	if (frame->flags & AV_PACKET_FLAG_RESET)
		frame->flags &= ~AV_PACKET_FLAG_RESET;
	if (frame->flags & AV_PACKET_FLAG_EOS)
		priv->eos_reached = 2;
	priv->deq_err_cnt = 0;

	if (priv->need_reset) {
		frame->flags |= AV_PACKET_FLAG_RESET;
		priv->need_reset = 0;
	}

	pthread_mutex_lock(&priv->lock);
	priv->hold_bufnum--;
	OSCL_LOGD("total %d, hold %d, vir %p, phy %lu, ts=%lld, w=%d, h=%d",
		priv->req_bufnum, priv->hold_bufnum,
		frame->vir_addr, frame->phy_addr, frame->time_stamp,
		frame->width_stride, frame->height_aligned);
	pthread_mutex_unlock(&priv->lock);

#ifdef DEBUG_VDECODER
	if (priv->pre_ts > frame->time_stamp) {
		OSCL_LOGE("timestamp error! pre[%lld] > cur[%lld]",
			priv->pre_ts, frame->time_stamp);
	}
	priv->pre_ts = frame->time_stamp;
#endif
#ifdef DBG_VDEC_SAVE_YUV
	if (frame->time_stamp > DBG_SAVE_YUV_START_TS
		&& frame->time_stamp < DBG_SAVE_YUV_END_TS
		&& ret == VDEQBUF_OK && frame->vir_addr != NULL && priv->yuvfd >= 0) {
		int wlen = write(priv->yuvfd, frame->vir_addr, priv->frame_size);
		if (wlen != priv->frame_size) {
			OSCL_LOGE("write file %s error(%s)!",
				DBG_VDEC_SAVE_YUV, strerror(errno));
		}
	}
#endif
	return VDEQBUF_OK;
}

static inline int clear_valid_frame(video_decoder_t *priv)
{
	vdec_frame_t frame;
	int i;
	int ret;

	for (i = 0; i < priv->req_bufnum; i++) {
		ret = priv->plugin->dequeue_buf(priv->codec, &frame, 0);
		if (ret == VDEQBUF_ENOBUFS) {
			OSCL_LOGD("finish");
			ret = 0;
			break;
		} else if (ret != VDEQBUF_OK) {
			OSCL_LOGE("dequeue_buf error(%d)", ret);
			break;
		}
		ret = priv->plugin->queue_buf(priv->codec, &frame);
		if (ret != 0) {
			OSCL_LOGE("queue_buf error(%d)", ret);
			break;
		}
	}

	return ret;
}

int vdecoder_clear(void *handle)
{
	video_decoder_t *priv = (video_decoder_t *)handle;
	int valid_buf_num = 0;
	int ret;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	ret = priv->plugin->ex_ops(priv->codec, VDEC_CMD_EXIT_DEQUEUE_BUF,
		&valid_buf_num);
	if (ret != 0) {
		OSCL_LOGE("VDEC_CMD_EXIT_DEQUEUE_BUF error[%d]", ret);
		return ret;
	}

	pthread_mutex_lock(&priv->deq_lock);

	ret = priv->plugin->ex_ops(priv->codec, VDEC_CMD_FLUSH, NULL);
	if (ret != 0) {
		OSCL_LOGE("VDEC_CMD_FLUSH error[%d]", ret);
		goto ERR_EXIT;
	}
	ret = clear_valid_frame(priv);
	if (ret != 0)
		goto ERR_EXIT;
	priv->need_reset = 1;
	pthread_mutex_unlock(&priv->deq_lock);

	return 0;

ERR_EXIT:
	pthread_mutex_unlock(&priv->deq_lock);
	return ret;
}

int vdecoder_ctrl(void *handle, vdec_cmd_e cmd, void *param)
{
	video_decoder_t *priv = (video_decoder_t *)handle;
	int ret;

	if (priv == NULL || priv->codec == NULL) {
		OSCL_LOGE("error! priv=%p!", priv);
		return -1;
	}

	ret = priv->plugin->ex_ops(priv->codec, cmd, param);
	if (ret != 0) {
		OSCL_LOGE("ex_ops error[%d]", ret);
		return -1;
	}

	return 0;
}

