#ifndef LOMBO_DECPLUGIN_H
#define LOMBO_DECPLUGIN_H


typedef struct {
	unsigned char *vir_addr;
	unsigned long phy_addr;
	unsigned int type;
	unsigned int len;
	long long ts;
	unsigned int flags;
} vdec_packet_t;


typedef struct {
	unsigned int crop_x;
	unsigned int crop_y;
	unsigned int crop_width;
	unsigned int crop_height;
} vdec_crop_t;

typedef enum {
	ERROR_HANDLE_NORMAL,
	ERROR_HANDLE_PIC_COPY_REF,
} vdec_error_handle_e;


typedef enum {
	VDEC_YUV_SEMI420,
	VDEC_YUV_PLAN420,
} vdec_outputformat_e;

typedef struct {
	int codec_id;
	unsigned int src_width;
	unsigned int src_height;
	unsigned int pp_out_width;
	unsigned int pp_out_height;
	unsigned char *extra_data;
	unsigned int extradata_len;
	vdec_error_handle_e error_handling_mode;
	vdec_outputformat_e output_mode;
} vdc_init_param_t;

typedef struct {
	int codec_id;
	unsigned int src_width;
	unsigned int src_height;
} hvc_init_param_t;

typedef struct {
	union {
		vdc_init_param_t vdc_init_parm;
		hvc_init_param_t hvc_init_parm;
	};
} vdec_init_param_t;

typedef struct {
	int pic_width;
	int pic_height;
	int buf_num;
	int buf_size;
	int max_buf_num;
	vdec_crop_t crop;
} vdec_info_t;

typedef struct {
	unsigned char *vir_addr;
	unsigned long phy_addr;
	int buf_size;
	int total_buf_num;
} vdec_alloc_buf_t;

typedef struct {
	unsigned char *vir_addr;
	unsigned long phy_addr;
	unsigned int offset[3];
	unsigned int planenum;
	vdec_crop_t crop;
	unsigned int width_stride;
	unsigned int height_aligned;
	unsigned int output_mode;
	long long time_stamp;
	int flags;
} vdec_frame_t;

typedef enum {
	VDEC_ERRORS = -2000,
	VDEC_TIMEOUT = -255,
	VDEC_STREAM_NOT_SUPPORTED = -8,
	VDEC_HDRS_NOT_RDY = -6,
	VDEC_NOT_INITIALIZED = -3,
	VDEC_STRM_ERROR = -2,
	VDEC_PARAM_ERROR = -1,
	VDEC_OK = 0,
	VDEC_HEAD_INFO = 4,
	VDEC_BUF_NOT_ENOUGH = 9,
	VDEC_DISCARD_PACKET = 0xFFF0,
} vdec_ret_e;

typedef enum {
	VDEQBUF_ENOBUFS = -355,
	VDEQBUF_TIMEOUT = -255,
	VDEQBUF_ERRORS = -1,
	VDEQBUF_OK = 0,
} vdeqbuf_ret_e;

typedef enum {
	VDEC_CMD_PP_SET_OUTPUT_WIDTH,
	VDEC_CMD_PP_SET_OUTPUT_HEIGHT,
	VDEC_CMD_FLUSH,
	VDEC_CMD_LIVING,
	VDEC_CMD_EXIT_DEQUEUE_BUF,
	VDEC_CMD_GET_VALID_BUF_NUM,
} vdec_cmd_e;

typedef struct {
	void *(*open)(vdec_init_param_t *init_param);
	int (*get_info)(void *handle, vdec_info_t *buf_info);
	int (*register_buf)(void *handle, vdec_alloc_buf_t *mem_buf);
	int (*queue_buf)(void *handle, vdec_frame_t *p_frame);
	int (*decode_frame)(void *handle, vdec_packet_t *src_packet);
	int (*dequeue_buf)(void *handle, vdec_frame_t *p_frame, int timeout_ms);
	int (*close)(void *handle);
	int (*ex_ops)(void *handle, vdec_cmd_e cmd, void *parm);
} vdec_plugin_t;


#endif /* LOMBO_DECPLUGIN_H */
