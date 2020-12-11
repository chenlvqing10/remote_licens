/*
 * recorder_take_picture.c - Standard functionality for take picture.
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
#define DBG_LEVEL         DBG_WARNING
#include <oscl.h>
#include <base_component.h>
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "video_stream.h"
#include "vsrc_camera.h"
#include "video_recorder.h"
#include "recorder_private.h"
#include "omx_vendor_lb.h"
#include "watermark/watermark.h"
#include "lombo_jpegenc_plugin.h"

#define JPEG_INFO_SIZE  (8*1024)
#define JPEG_STREAM_BUF_SIZE (800*1024)
#define MAX_PIC_ENC 3

typedef struct pic_msg {
	frame_t *frame;
	jpeg_enc_packet_t packet;
	char *file_name;
	void *watermark_private;
	win_rect_t enc_rect;
	int offset[3];
} pic_msg_t;

typedef struct pic_private {
	sem_t frame_sem;
	sem_t pic_taken_sem;
	pthread_mutex_t lock;
	oscl_queue_t queue;
	int num_finished;
	int total_encoding;
	int thread_id;
	pthread_t enc_thread;
	int stop_flag;
	pic_msg_t pic[MAX_PIC_ENC];
	void *so_lib_handle;
	void *(*encode_init)(void);
	JPEGENC_STATUS(*encode_deinit)(void *);
	JPEGENC_STATUS(*encode_frame)(void *jpeg_enc_handle,
				jpeg_enc_capbuf_t *jpeg_enc_capbuf,
				jpeg_enc_packet_t *jpeg_enc_packet,
				jpeg_enc_parm_t *jpeg_enc_parm);
} pic_private_t;

static port_info_t *_vstream_getport(void *handle, vdisp_rotate_mode_e mode)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_getport(camera, mode);
}

static int _vstream_putport(void *handle, port_info_t *port)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_putport(camera, port);
}

static int _vstream_enable_port(void *handle, port_info_t *port)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_enable_port(camera, port);
}

static int _vstream_disable_port(void *handle, port_info_t *port)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_disable_port(camera, port);
}

#ifdef OMX_DYNAMIC_LOADING
#include <dlfcn.h>

static int _lib_load(pic_private_t *pic_private)
{
	int ret = 0;
	pic_private->so_lib_handle = dlopen("libie_jpg.so", RTLD_LAZY);
	OSCL_LOGI("open lib :%s, hdle:%p", "libie_jpg.so", pic_private->so_lib_handle);
	if (pic_private->so_lib_handle == NULL) {
		OSCL_LOGE("Failed to open lib:libie_jpg.so, %s", dlerror());
		ret = -1;
		goto EXIT;
	}

	pic_private->encode_deinit = dlsym(pic_private->so_lib_handle, "jpeg_enc_deinit");
	pic_private->encode_frame = dlsym(pic_private->so_lib_handle, "jpeg_enc_frame");
	pic_private->encode_init = dlsym(pic_private->so_lib_handle, "jpeg_enc_init");
	if (pic_private->encode_deinit == NULL ||
		pic_private->encode_frame == NULL ||
		pic_private->encode_init == NULL) {
		dlclose(pic_private->so_lib_handle);
		OSCL_LOGE("libie_jpg.so dlsym fail:%s, %p-%p-%p", dlerror(),
			pic_private->encode_init,
			pic_private->encode_frame,
			pic_private->encode_deinit);
		pic_private->so_lib_handle = NULL;
		pic_private->encode_init = NULL;
		pic_private->encode_frame = NULL;
		pic_private->encode_deinit = NULL;
		ret = -1;
		goto EXIT;
	}
	OSCL_LOGI("libie_jpg.so dlsym %p-%p-%p",
		pic_private->encode_init,
		pic_private->encode_frame,
		pic_private->encode_deinit);
EXIT:
	return ret;
}
static int _lib_unload(pic_private_t *pic_private)
{
	int ret = 0;
	if (pic_private->so_lib_handle) {
		dlclose(pic_private->so_lib_handle);
		pic_private->so_lib_handle = NULL;
		pic_private->encode_init = NULL;
		pic_private->encode_frame = NULL;
		pic_private->encode_deinit = NULL;
	}
	return ret;
}
#else
static int _lib_load(pic_private_t *pic_private)
{
	pic_private->so_lib_handle = NULL;
	pic_private->encode_deinit = jpeg_enc_deinit;
	pic_private->encode_frame = jpeg_enc_frame;
	pic_private->encode_init = jpeg_enc_init;
}
static int _lib_unload(pic_private_t *pic_private)
{
	pic_private->so_lib_handle = NULL;
	pic_private->encode_init = NULL;
	pic_private->encode_frame = NULL;
	pic_private->encode_deinit = NULL;
}
#endif


static int OmxFormat_to_Enc(int OmxFormat)
{
	int EncodeFormat = -1;

	if (OmxFormat == OMX_COLOR_FormatYUV420Planar)
		EncodeFormat = ENC_YUV420P;
	else if (OmxFormat == OMX_COLOR_FormatYUV420SemiPlanar)
		EncodeFormat = ENC_YUV420SP;

	if (EncodeFormat == -1)
		OSCL_LOGE("not support the OmxFormat\n");

	return EncodeFormat;
}
static int OmxFormat_to_Blending(int OmxFormat)
{
	int EncodeFormat = -1;

	if (OmxFormat == OMX_COLOR_Format32bitARGB8888)
		EncodeFormat = ENC_ARGB8888;
	else if (OmxFormat == OMX_COLOR_Format32bitBGRA8888)
		EncodeFormat = ENC_BGRA8888;
	else if (OmxFormat == OMX_COLOR_Format32BitRGBA8888)
		EncodeFormat = ENC_RGBA8888;

	if (EncodeFormat == -1)
		OSCL_LOGE("not support the OmxFormat blending\n");
	return EncodeFormat;
}

static int __set_enc_area(pic_msg_t *pic, jpeg_enc_parm_t *encode_parm)
{
	int src_height;
	int src_width;
	pic->offset[0] = 0;
	pic->offset[1] = 0;
	pic->offset[2] = 0;

	src_height = pic->frame->info.video.height;
	src_width = pic->frame->info.video.width;
	if (pic->enc_rect.width > src_width || pic->enc_rect.width <= 0)
		pic->enc_rect.width = src_width;
	if (pic->enc_rect.height > src_height || pic->enc_rect.height <= 0)
		pic->enc_rect.height = src_height;
	encode_parm->in_width = src_width;
	encode_parm->in_height = src_height;
	encode_parm->in_stride = (encode_parm->in_width + 15) / 16 * 16;
	encode_parm->out_width = pic->enc_rect.width;
	encode_parm->out_height = pic->enc_rect.height;
	encode_parm->input_mode = OmxFormat_to_Enc(pic->frame->info.video.color_fmt);
	encode_parm->quality = 70;

	if (pic->enc_rect.width != src_width && pic->enc_rect.height != src_height) {
		pic->offset[0] = src_width * pic->enc_rect.y;
		pic->offset[0] += pic->enc_rect.x;
		if (encode_parm->input_mode == ENC_YUV420SP) {
			pic->offset[1] = src_width * pic->enc_rect.y / 2;
			pic->offset[1] += pic->enc_rect.x;
		}
		if (encode_parm->input_mode == ENC_YUV420P) {
			pic->offset[1] = src_width * pic->enc_rect.y / 4;
			pic->offset[1] += pic->enc_rect.x / 2;
			pic->offset[2] = pic->offset[1];
		}
		encode_parm->in_width = pic->enc_rect.width;
		encode_parm->in_height = pic->enc_rect.height;
		encode_parm->out_width = pic->enc_rect.width;
		encode_parm->out_height = pic->enc_rect.height;
	}
	return 0;
}
int jpeg_encode_frame(pic_private_t *pic_private, pic_msg_t *pic)
{
	FILE *flip = NULL;
	jpeg_enc_capbuf_t capbuf;
	jpeg_enc_parm_t encode_parm;
	void *handle;
	int ret = -1;
	jpeg_enc_packet_t *pic_packet;
	int nmark;
	numeral_output_t *mark;
	watermark_picture_t *wartermark_pic;
	int i;

	oscl_param_check(pic != NULL, -1, NULL);
	oscl_param_check(pic->frame != NULL, -1, NULL);
	pic_packet = &pic->packet;
	oscl_param_check(pic_packet->vir_addr != NULL, -1, NULL);
	oscl_param_check(pic_private->encode_deinit != NULL, -1, NULL);
	oscl_param_check(pic_private->encode_frame != NULL, -1, NULL);
	oscl_param_check(pic_private->encode_init != NULL, -1, NULL);

	__set_enc_area(pic, &encode_parm);
	memset(&capbuf, 0, sizeof(jpeg_enc_capbuf_t));
	capbuf.vir_addr[0] = pic->frame->info.video.addr[0] + pic->offset[0];
	capbuf.vir_addr[1] = pic->frame->info.video.addr[1] + pic->offset[1];
	capbuf.vir_addr[2] = pic->frame->info.video.addr[2] + pic->offset[2];

	capbuf.phy_addr[0] = oscl_virt_to_phys(pic->frame->info.video.addr[0]);
	if (pic->frame->info.video.addr[1] != NULL)
		capbuf.phy_addr[1] = oscl_virt_to_phys(pic->frame->info.video.addr[1]);
	if (pic->frame->info.video.addr[2] != NULL)
		capbuf.phy_addr[2] = oscl_virt_to_phys(pic->frame->info.video.addr[2]);
	capbuf.phy_addr[0] += pic->offset[0];
	capbuf.phy_addr[1] += pic->offset[1];
	capbuf.phy_addr[2] += pic->offset[2];

	handle = pic_private->encode_init();
	if (handle == NULL) {
		OSCL_LOGE("open jpeg encoder failed!");
		goto EXIT;
	}

	nmark = watermark_get_markarray(pic->watermark_private, &mark);
	if (nmark != 0 && mark != NULL) {
		if (nmark > MAX_JPG_BLENDING_NUM)
			nmark = MAX_JPG_BLENDING_NUM;
		OSCL_LOGI("set watermark to jpeg encoder!");
	}

	encode_parm.wm.watermark_picture_num = nmark;
	wartermark_pic = encode_parm.wm.watermark_pictures;
	for (i = 0; i < nmark; i++) {
		if (mark[i].numeral_picture.picture_size != 0) {
			wartermark_pic[i].blending_enable = 1;
			wartermark_pic[i].colorspace =
					OmxFormat_to_Blending(mark[i].colorspace);
			wartermark_pic[i].blending_x_pos =
					mark[i].start_x_pos;
			wartermark_pic[i].blending_y_pos =
					mark[i].start_y_pos;
			wartermark_pic[i].blending_width =
					mark[i].numeral_picture.width;
			wartermark_pic[i].blending_height =
					mark[i].numeral_picture.height;
			wartermark_pic[i].blending_stride =
					mark[i].numeral_picture.stride;
			wartermark_pic[i].phy_addr =
					mark[i].numeral_picture.phy_addr;
		} else{
			wartermark_pic[i].blending_enable = 0;
		}
	}

	ret = pic_private->encode_frame(handle, &capbuf, &pic->packet, &encode_parm);
	watermark_put_markarray(pic->watermark_private, mark);
	pic_private->encode_deinit(handle);
	if (ret < 0)
		OSCL_LOGE("jpeg encode frame err:%d!", ret);

	if (pic->file_name)
		flip = fopen(pic->file_name, "wb");
	if (flip == NULL) {
		OSCL_LOGE("open out file err!\n");
		goto EXIT;
	}
	ret = fwrite(pic->packet.vir_addr, 1, pic->packet.stream_length, flip);
	oscl_param_check_exit(ret == pic->packet.stream_length, -1, NULL);
	ret = 0;

EXIT:
	if (flip)
		fclose(flip);
	return ret;
}

void *picture_encoder_task(void *param)
{
	pic_private_t *pic_private = param;
	pic_msg_t *pic;

	prctl(PR_SET_NAME, "picture_encoder_task");
	while (pic_private->stop_flag == 0) {
		sem_wait(&pic_private->frame_sem);
		pic = oscl_queue_dequeue(&pic_private->queue);
		if (pic == NULL)
			continue;
		jpeg_encode_frame(pic_private, pic);
		app_empty_buffer_done(pic->frame);
		OSCL_LOGI("take pic,sem_post");
		sem_post(&pic_private->pic_taken_sem);
	}
	sem_post(&pic_private->pic_taken_sem);
	return NULL;
}

pic_msg_t *_get_msg(pic_private_t *pic_private)
{
	int i;
	pic_msg_t *msg = NULL;

	if (pic_private == NULL)
		return NULL;
	for (i = 0; i < MAX_PIC_ENC; i++)
		if (pic_private->pic[i].frame == NULL)
			break;
	if (i != MAX_PIC_ENC)
		msg = &pic_private->pic[i];
	return msg;
}


static int _pic_get_buffer(void *eplayer, frame_t *frame)
{
	pic_msg_t *pic = NULL;
	int ret = 0;
	video_recorder_t *video_rec = eplayer;
	pic_private_t *pic_private = video_rec->pic_private;

	pthread_mutex_lock(&pic_private->lock);
	if (pic_private->num_finished >= pic_private->total_encoding) {
		pthread_mutex_unlock(&pic_private->lock);
		ret = app_empty_buffer_done(frame);
		goto EXIT;
	}
	pic = _get_msg(pic_private);
	if (pic) {
		OSCL_LOGI("%p:%p", pic, frame);
		pic->frame = frame;
		oscl_queue_queue(&pic_private->queue, pic);
		pic_private->num_finished++;
		sem_post(&pic_private->frame_sem);
	}
	pthread_mutex_unlock(&pic_private->lock);
EXIT:
	return ret;
}

void _deinit_msg(pic_msg_t *pic)
{

	if (pic->packet.vir_addr)
		oscl_free_align(pic->packet.vir_addr);
	if (pic->file_name)
		oscl_free(pic->file_name);
	memset(pic, 0, sizeof(pic_msg_t));
}

int _init_msg(pic_msg_t *pic, char *file, video_recorder_t *video_rec)
{
	int ret = 0;
	void *watermark_private = video_rec->watermark_private;
	pic->frame = NULL;
	pic->file_name = oscl_strdup(file);
	oscl_param_check_exit(pic->file_name, -1, NULL);

	pic->packet.buf_size = JPEG_STREAM_BUF_SIZE;
	pic->packet.vir_addr = oscl_malloc_align(JPEG_STREAM_BUF_SIZE, 4096);
	oscl_param_check_exit(pic->packet.vir_addr, -1, NULL);
	pic->watermark_private = watermark_private;
	memcpy(&pic->enc_rect, &video_rec->venc_info.venc_rect, sizeof(win_rect_t));
EXIT:
	if (ret)
		_deinit_msg(pic);
	return ret;
}


int take_pic_start(video_recorder_t *video_rec, char *file)
{
	int ret = 0;
	app_frame_cb_t cb;
	pic_private_t *pic_private;

	OSCL_TRACE("start");
	oscl_param_check_exit(video_rec, -1, NULL);
	oscl_param_check_exit(video_rec->pic_private, -1, NULL);
	pic_private = video_rec->pic_private;

	/* If already get source port, there is taking-picture task unfinished, just
	 * return failed. Otherwise get a camera port.
	*/
	pthread_mutex_lock(&pic_private->lock);
	if (video_rec->src_pic != NULL || video_rec->vstream == NULL) {
		ret = -1;
		pthread_mutex_unlock(&pic_private->lock);
		goto EXIT;
	}
	video_rec->src_pic = _vstream_getport(video_rec->vstream,
		video_rec->venc_info.rotate);
	pthread_mutex_unlock(&pic_private->lock);
	oscl_param_check_exit(video_rec->src_pic, -1, NULL);

	/* initial pic message info */
	_init_msg(&pic_private->pic[0], file, video_rec);
	pic_private->total_encoding = 1;

	/* setup raw data callback to camera port and enable it. */
	cb.app_data = video_rec;
	cb.buf_handle = _pic_get_buffer;
	cb.type = VS_VIDEO_RAW_FRAME;
	ret = untunnel_setup_cb(video_rec->src_pic, &cb);
	if (ret == 0) {
		ret = _vstream_enable_port(video_rec->vstream, video_rec->src_pic);
		/* wait for take picture finished and then unset and free camera port */
		sem_wait(&pic_private->pic_taken_sem);
		OSCL_LOGI("take picture finished");
		_vstream_disable_port(video_rec->vstream, video_rec->src_pic);
	}
	untunnel_unset_cb(video_rec->src_pic);
	_vstream_putport(video_rec->vstream, video_rec->src_pic);

	_deinit_msg(&pic_private->pic[0]);
	pic_private->total_encoding = 0;
	pic_private->num_finished = 0;

	pthread_mutex_lock(&pic_private->lock);
	video_rec->src_pic = NULL;
	pthread_mutex_unlock(&pic_private->lock);
	ret = 0;

EXIT:
	return ret;
}

int take_pic_init(video_recorder_t *video_rec)
{
	pic_private_t *pic_private;
	pthread_attr_t thread_attr;
	struct sched_param shed_param = {0};

	oscl_param_check(video_rec, -1, NULL);
	oscl_param_check(video_rec->pic_private == NULL, -1, NULL);

	pic_private = oscl_zalloc(sizeof(pic_private_t));
	oscl_param_check(pic_private, -1, NULL);
	sem_init(&pic_private->frame_sem, 0, 0);
	sem_init(&pic_private->pic_taken_sem, 0, 0);
	pthread_mutex_init(&pic_private->lock, NULL);
	oscl_queue_init(&pic_private->queue);
	video_rec->pic_private = pic_private;
	_lib_load(pic_private);

	pthread_attr_init(&thread_attr);
	pthread_attr_setstacksize(&thread_attr, 0x2000);
	shed_param.sched_priority = 15;
	pthread_attr_setschedparam(&thread_attr, &shed_param);
	pic_private->thread_id = pthread_create(&pic_private->enc_thread, &thread_attr,
			picture_encoder_task, pic_private);
	if (pic_private->thread_id < 0)
		return OMX_ErrorInsufficientResources;

	return 0;
}

void take_pic_deinit(video_recorder_t *video_rec)
{
	pic_private_t *pic_private;

	if (video_rec == NULL || video_rec->pic_private == NULL)
		return;
	pic_private = video_rec->pic_private;

	while (video_rec->src_pic != NULL) {
		OSCL_LOGE("taking picture while exit take-pic thread, waiting");
		oscl_mdelay(10);
	}

	if (pic_private->thread_id == 0) {
		pic_private->stop_flag = 1;
		sem_post(&pic_private->frame_sem);
		pthread_join(pic_private->enc_thread, NULL);
		pic_private->thread_id = -1;
	}
	video_rec->pic_private = NULL;
	sem_destroy(&pic_private->frame_sem);
	sem_destroy(&pic_private->pic_taken_sem);
	pthread_mutex_destroy(&pic_private->lock);
	oscl_queue_deinit(&pic_private->queue);
	_lib_unload(pic_private);
	oscl_free(pic_private);
}

