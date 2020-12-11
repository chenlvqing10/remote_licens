#ifndef _YUV_RETANGLE_DRAW_H_
#define _YUV_RETANGLE_DRAW_H_

typedef enum {
	ENC_YUV420P = 0, /* for input mode */
	ENC_YVU420P,
	ENC_YUV420SP,
	ENC_YVU420SP,
	ENC_YUYV422,
	ENC_UYVY422,
	ENC_YVYU422,
	ENC_VYUY422,
	ENC_YUV422P,
	ENC_YVU422P,
	ENC_YUV422SP,
	ENC_YVU422SP,
	ENC_ARGB8888 = 100, /* for blending */
	ENC_RGBA8888,
	ENC_ABGR8888,
	ENC_BGRA8888,
} enc_input_mode_e;

typedef struct draw_dot {
	unsigned int x;
	unsigned int y;
} yuv_dot_t;

typedef struct draw_retangle {
	yuv_dot_t start_pos;
	yuv_dot_t end_pos;
} draw_retangle_t;

typedef struct yuv_retangle_draw {
	unsigned char *buffer_y;
	unsigned char *buffer_uv;
	unsigned char *buffer_v; /* used by planar*/
	enc_input_mode_e yuv_mode;
	unsigned int width_stride;
	unsigned int height_aligned;
	draw_retangle_t retangle;
} yuv_draw_t;

int yuv_retangle_draw(yuv_draw_t *rtd);
int yuv_retangle_draw_by_color(yuv_draw_t *rtd, int rgb_color);

#endif
