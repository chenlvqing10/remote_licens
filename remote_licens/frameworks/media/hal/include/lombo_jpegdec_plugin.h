#ifndef LOMBO_JPEGDEC_PLUGIN_H
#define LOMBO_JPEGDEC_PLUGIN_H


typedef enum JPEG_OUTPUT_FORMAT {
	JPEG_YV12 = 0,
	JPEG_YV21,
	JPEG_NV12,
	JPEG_NV21
} jpeg_output_format_e;



typedef enum PICTURE_TYPE {
	JPEG_ERROR = -1,
	JPEG_NORMAL,
	JPEG_CMYK,
	JPEG_PROGRESSIVE
} picture_type_e;


typedef struct {
	unsigned char *vir_addr;
	unsigned int stream_len;
} jpeg_dec_packet_t;




typedef struct {
	unsigned char *vir_addr;
	unsigned long phy_addr;
	unsigned int offset[3];
	unsigned int planenum;
} jpeg_dec_output_buf_t;



typedef struct {
	picture_type_e picture_type;
	unsigned int width_stride;
	unsigned int height_aligned;
	unsigned int crop_width;
	unsigned int crop_height;
	unsigned int rotation_degree;
} jpeg_dec_header_info_t;



typedef struct {
	jpeg_output_format_e output_format;
	unsigned int down_scale_ratio;
	unsigned int rotation_degree;
	unsigned int width_stride;
	unsigned int height_aligned;
	unsigned int crop_x;
	unsigned int crop_y;
	unsigned int crop_width;
	unsigned int crop_height;
} jpeg_dec_parm_t;



typedef struct {
	void* (*decode_init)(void);
	int (*decode_deinit)(void *handle);
	int (*read_header)(void *jpeg_dec_handle,
					jpeg_dec_packet_t *jpeg_dec_packet,
					jpeg_dec_header_info_t *jpeg_dec_info);
	int (*decode_frame)(void *jpeg_dec_handle,
					jpeg_dec_packet_t *jpeg_dec_packet,
					jpeg_dec_output_buf_t *jpeg_dec_output_buf,
					jpeg_dec_parm_t *jpeg_dec_parm);
} jpegdec_plugin_t;


void *jpeg_dec_init(void);
int jpeg_dec_deinit(void *jpeg_dec_handle);
int jpeg_dec_read_header(void *jpeg_dec_handle, jpeg_dec_packet_t *jpeg_dec_packet,
						jpeg_dec_header_info_t *jpeg_dec_info);
int jpeg_dec_frame(void *jpeg_dec_handle,
				jpeg_dec_packet_t *jpeg_dec_packet,
				jpeg_dec_output_buf_t *jpeg_dec_output_buf,
				jpeg_dec_parm_t *jpeg_dec_parm);


#endif /*LOMBO_JPEGDEC_PLUGIN_H*/
