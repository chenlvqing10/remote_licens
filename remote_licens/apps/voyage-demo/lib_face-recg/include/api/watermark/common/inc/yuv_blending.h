#ifndef _YUV_BLENDING_H_
#define _YUV_BLENDING_H_

enum blendings_colorspace {
	ARGB8888,
	RGBA8888,
	ABGR8888,
	BGRA8888,
};

typedef struct yuv_picture {
	unsigned int width_stride;
	unsigned int height_aligned;
	unsigned char *buffer_y;
	unsigned char *buffer_uv;
} yuv_picture_t;

enum blending_text_format {
	unknow_text,    /* not text, adjust nothing*/
	white_text,     /* text is white text, will adjust depend on background */
	black_text      /* text is black text, will adjust depend on background */
};

typedef struct yuv_blending_struct {
	struct yuv_picture source;
	unsigned char *blending_data;   /* blending picture data addr */
	unsigned int colorspace;        /* blending colorspace */
	unsigned int blending_x_pos;    /* blending start pos x */
	unsigned int blending_y_pos;    /* blending start pos y */
	unsigned int blending_width;    /* blending output width pixcel */
	unsigned int blending_height;   /* blending output height pixcel */
	unsigned int blending_stride;   /* blending picture stride (byte) */
	enum blending_text_format text_color; /* adjust for text color */
} yuv_blending_t;

void blending_operation(yuv_blending_t *yuv_blending);

#endif
