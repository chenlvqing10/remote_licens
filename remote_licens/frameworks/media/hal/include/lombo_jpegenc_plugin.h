#ifndef LOMBO_JPEGENC_PLUGIN_H
#define LOMBO_JPEGENC_PLUGIN_H

#include "lombo_enc_common.h"
#define MAX_JPG_BLENDING_NUM  4

typedef enum JPEG_ENC_STATUS {
	JPEG_ENC_ERROR = -1,
	JPEG_ENC_SUCCESS,
	JPEG_ENC_BUF_FULL,
} JPEGENC_STATUS;


typedef struct {
	int in_width;
	int in_height;
	int in_stride;
	int out_width;
	int out_height;
	int quality;
	enc_input_mode_e input_mode;
	watermark_t wm;
} jpeg_enc_parm_t;



typedef struct {
	unsigned char *vir_addr[3];
	unsigned long phy_addr[3];
} jpeg_enc_capbuf_t;



typedef struct {
	unsigned char *vir_addr;
	unsigned int buf_size;
	unsigned int stream_length;
} jpeg_enc_packet_t;


typedef struct {
	void *(*encode_init)(void);
	JPEGENC_STATUS(*encode_deinit)(void *jpeg_enc_handle);
	JPEGENC_STATUS(*encode_frame)(void *jpeg_enc_handle,
					jpeg_enc_capbuf_t *jpeg_enc_capbuf,
					jpeg_enc_packet_t *jpeg_enc_packet,
					jpeg_enc_parm_t *jpeg_enc_parm);
} jpegenc_plugin_t;

void *jpeg_enc_init(void);
JPEGENC_STATUS	jpeg_enc_deinit(void *jpeg_enc_handle);
JPEGENC_STATUS	jpeg_enc_frame(void *jpeg_enc_handle,
					jpeg_enc_capbuf_t *jpeg_enc_capbuf,
					jpeg_enc_packet_t *jpeg_enc_packet,
					jpeg_enc_parm_t *jpeg_enc_parm);


#endif/* LOMBO_JPEGENC_PLUGIN_H */
