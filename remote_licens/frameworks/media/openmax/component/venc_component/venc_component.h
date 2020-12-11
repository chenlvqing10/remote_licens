#ifndef __VIDEO_ENCODE_COMPOMEMT_H__
#define __VIDEO_ENCODE_COMPOMEMT_H__

#include "base_component.h"
#include "lombo_encplugin.h"
#include "venc_platform.h"

#define INPUT_PORT 0
#define OUTPUT_PORT 1
#define H264_INFO_SIZE 1024

/* #define FILL_BUFFER_INTERVAL_DEBUG */
#define INTERVAL_TIME 100

typedef enum OMX_VIDEO_ENCODER_ROLE {
	UNSUPPORT_ROLE_TYPE = 0,
	H264_ROLE_TYPE,
	JPEG_ROLE_TYPE,
	H265_ROLE_TYPE,
} OMX_VIDEO_ENCODER_ROLE;

typedef struct wm_data {
	int index; /* blending index */
	watermark_picture_t wm_pic;
} wm_data_t;

typedef enum {
	BUF_NOT_INITED = 0,
	BUF_OWN_BY_COMP,
	BUF_OWN_BY_CLIENT,
	BUF_OWN_BY_ENCODER,
} encoder_buf_state_e;

typedef enum {
	ENC_OUTMODE_INVALID = 0,
	ENC_OUTMODE_PACKET,
	ENC_OUTMODE_BYTEBUF,
} encoder_outbuf_mode_e;

typedef enum {
	ENC_INMODE_INVALID = 0,
	ENC_INMODE_BYTEBUF_VIR,
	ENC_INMODE_BYTEBUF_PHY,
	ENC_INMODE_ANW_BUF,
	ENC_INMODE_NATIVE_HANDLE_BUF,
} encoder_inbuf_mode_e;

typedef struct venc_outbuf_info {
	int index;
	encoder_buf_state_e state;
	OMX_BUFFERHEADERTYPE *header;
	venc_packet_t enc_packt;
} venc_outbuf_info_t;

typedef struct {
	encoder_outbuf_mode_e outbuf_mode;
	int outbuf_cnt;
	venc_outbuf_info_t *outbuf_infos;
	unsigned char *tmpbuf_for_cycle;
	int tmpbuf_size;
	int tmpbuf_index;
} venc_outbuf_sum_t;

typedef struct venc_inbuf_info {
	int index;
	encoder_buf_state_e state;
	OMX_BUFFERHEADERTYPE *header;
	unsigned char *vir_addr;
	unsigned long phy_addr;
	venc_capbuf_t capbuf;
} venc_inbuf_info_t;

typedef struct {
	encoder_inbuf_mode_e inbuf_mode;
	int inbuf_cnt;
	venc_inbuf_info_t *inbuf_infos;
	unsigned char *vir_cap_buf;
	unsigned long phy_cap_buf;
	int cap_size;
} venc_inbuf_sum_t;

typedef struct {
	int role_type;
	int in_width; /* the input video width */
	int in_height;
	int out_width;
	int out_height;
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
	int quality; /* support for mjpeg */
} vcodec_cfg_t;

typedef struct venc_component_private {
	pthread_mutex_t lock;
	void *lb_component;
	int init_codec_flag;
	vcodec_cfg_t encode_parm;
	void *codec_handle;
	void *lib_handle;
	unsigned int cap_offsets[3];
	unsigned int cap_plan_num;
	venc_plugin_t *video_encode_info;
	unsigned int is_encoding_flag; /* marked omx is encoding a frame */
	watermark_t wartermark;
	pthread_mutex_t encode_mutex;
	OMX_S64 ref_time;
#ifdef FILL_BUFFER_INTERVAL_DEBUG
	int pre_time;
	int encode_num;
	int pre_num;
#endif
	venc_inbuf_sum_t inbuf_sum;
	venc_outbuf_sum_t outbuf_sum;
} venc_component_private_t;

void al_vc_core_init();
OMX_ERRORTYPE video_encode_port_init(lb_omx_component_t *component,
	base_port_t *base_port, OMX_U32 index, OMX_U32 dir_type);
int codec_init(venc_component_private_t *ctx);
int codec_deinit(venc_component_private_t *ctx);
int codec_dequeue_buffer(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *out_header, int timeout);
int codec_queue_buffer(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *out_header);
int codec_encode_frame(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *inbuffer);

int anw_buf_to_capbuf(venc_component_private_t *ctx,
	OMX_BUFFERHEADERTYPE *inbuf,
	venc_capbuf_t *cap_buf);
int outbuf_to_muxpack(venc_component_private_t *ctx,
	venc_outbuf_info_t *pbuf_info);
void save_raw_stream(lb_omx_component_t *component, int port_index,
	unsigned char *pdata, int len);
void save_yuv_stream(lb_omx_component_t *component, int port_index,
	unsigned char *pdata, int len);

#endif
