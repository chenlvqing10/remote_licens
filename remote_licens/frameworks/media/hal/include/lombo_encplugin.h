#ifndef LOMBO_ENCPLUGIN_H
#define LOMBO_ENCPLUGIN_H

#include "lombo_enc_common.h"

#define VENC_MAX_BLENDING_PICTURES 21
#define VENC_MIN_STREAM_BUF_SIZE (2*1024)
#define VENC_MAX_ROI_REGION 64


typedef enum {
	INVALID,
	VENC_INIT_PACKET,
	VENC_I_FRAME,
	VENC_P_FRAME,
	VENC_B_FRAME,
} venc_frame_type_e;

typedef enum {
	VENC_FRAME_END_STATUS_INIT,
	VENC_FRAME_END,
	VENC_FRAME_NOT_END
} venc_frame_end_status_e;

typedef enum {
	VENC_SET_BLENDING_PICTURE,
	VENC_DISABLE_BLENDING,
	VENC_FORCE_INTRA,
	VENC_SET_BLOCKING,
	VENC_FLUSH,
	VENC_SET_ROI_REGION
} venc_cmd_e;


typedef enum {
	VENC_GOPTYPE_ALL_I = 1,
	VENC_GOPTYPE_IP = 2,
	VENC_GOPTYPE_IBBBP = 5,
} venc_gop_type_e;

typedef enum {
	VENC_MIRDIR_NONE,
	VENC_MIRDIR_VER,
	VENC_MIRDIR_HOR,
	VENC_MIRDIR_HOR_VER,
} venc_mirrordirection_e;


typedef enum {
	VENC_ROTATION_NONE,
	VENC_ROTATION_LEFT_90,
	VENC_ROTATION_LEFT_180,
	VENC_ROTATION_LEFT_270,
} venc_rotation_e;

typedef enum {
	VENC_DEQUEBUFFER_TIMEOUT = -2,
	VENC_DEQUEBUFFER_OTHERERR = -1,
	VENC_DEQUEBUFFER_OK = 0,
} venc_dequebufferstatus_e;


typedef struct {
	int buf_num;
	int max_buf_num;
} venc_info_t;


typedef struct {
	unsigned int crop_x;
	unsigned int crop_y;
	unsigned int crop_width;
	unsigned int crop_height;
} venc_crop_t;


typedef struct {
	unsigned int left;
	unsigned int top;
	unsigned int right;
	unsigned int bottom;
} venc_rect_t;


typedef struct {
	venc_rect_t roi_rect[VENC_MAX_ROI_REGION];
	int roi_level[VENC_MAX_ROI_REGION];
	int roi_region_number;
} venc_roi_region_t;


typedef struct {
	int in_width; /* the input video width */
	int in_height;
	int l_stride; /* the input video width aligned*/
	int out_width;
	int out_height;
	int bitrate; /* the output raw data bitrate */
	int slice_mode; /* 0:sigle 1:muti */
	int input_mode; /* ENC_INPUT_MODE type */
	int idr_period;
	int frame_rate;
	int quality; /* support for mjpeg */
} vc_init_param_t;


typedef struct {
	int in_width; /* the input video width */
	int in_height;
	int l_stride;
	int bitrate; /* the output raw data bitrate */
	int input_mode; /* ENC_INPUT_MODE type */
	int idr_period;
	int frame_rate;
	int slice_mode;
	int slice_num;
	int rotation;
	int mirrordirection;
	int gop_type;
	venc_crop_t rect;
} vpu_init_param_t;


typedef struct {
	union {
		vc_init_param_t vc_init_parm;
		vpu_init_param_t vpu_init_parm;
	};
} venc_parm_t;


typedef struct {
	unsigned char *src_vir_addr[3];
	unsigned long src_phy_addr[3];
	long long time_stamp;
} venc_capbuf_t;


typedef struct {
	unsigned char *vir_addr[3];
	unsigned long phy_addr[3];
} venc_encoded_frame_t;


typedef struct {
	unsigned char *vir_addr[2];
	unsigned long phy_addr[2];
	int size[2];
	venc_frame_type_e pic_type;
	long long time_stamp;
	venc_frame_end_status_e frame_end_flag;
} venc_packet_t;

#ifdef __LINUX__

#elif defined __EOS__
	void *h264_enc_open(venc_parm_t *enc_parm, int buf_size);
	int h264_queue_buf(void *handle, venc_packet_t *p_frame);
	int h264_enc_frame(void *handle, venc_capbuf_t *capbuf_s);
	int h264_dequeue_buf(void *handle, venc_packet_t *p_frame, int wait_time);
	int h264_enc_close(void *handle);
	int h264_enc_ops(void *handle, venc_cmd_e cmd, void *arg);
#else

#endif

typedef struct {
	int (*prob)(venc_parm_t *enc_parm, venc_info_t *info);
	void *(*open)(venc_parm_t *enc_parm, int buf_size);
	int (*queue_buf)(void *handle, venc_packet_t *p_frame);
	int (*encode_frame)(void *handle, venc_capbuf_t *capbuf_s,
		venc_encoded_frame_t *frame_encoded);
	int (*dequeue_buf)(void *handle, venc_packet_t *p_frame, int wait_time);
	int (*close)(void *handle);
	int (*ex_ops)(void *handle, venc_cmd_e cmd, void *arg);
} venc_plugin_t;

#endif
