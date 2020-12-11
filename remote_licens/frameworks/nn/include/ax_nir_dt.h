#ifndef _AX_NIR_DT_H
#define _AX_NIR_DT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _AX_RECT_T
#define _AX_RECT_T
typedef struct
{
	int x;
	int y;
	int width;
	int height;
} ax_rect_t;
#endif

#ifndef _AX_SIZE_T
#define _AX_SIZE_T
typedef struct
{
	int width;
	int height;
} ax_size_t;
#endif

#ifndef _AX_IMG_YUV_T
#define _AX_IMG_YUV_T
typedef struct
{
	void *y;
	void *uv;
	void *y_phy;
	void *uv_phy;
	ax_size_t size;
} ax_imgyuv_t;
#endif

typedef struct
{
	ax_imgyuv_t yuv;
	ax_rect_t   dt_roi;
	ax_size_t   target_sz;
	float       enlarge_x, enlarge_y;
	int         detect_whole_image;/*0 - detect roi, 1 - detect whole image*/
	int         set_gray;
} nirdt_in_t;


typedef struct
{
	ax_rect_t roi;
	float     score;
	int       pts[10];
} axdt_result_t;

typedef struct
{
	axdt_result_t *p;
	int           size;
	int           capacity;
} nirdt_out_t;

/*
create nir face detector handle
*/
void *nirdt_create(ax_size_t img_size);
/*
detect nir faces
*/
void nirdt_detect(void *dt_hdl, nirdt_in_t *dt_in, nirdt_out_t **dt_out);
/*
release detector
*/
void nirdt_release(void *dt_hdl);

#ifdef __cplusplus
}
#endif

/*
	//Demo

	void *nir_hdl = nirdt_create();
	nirdt_in_t dt_in;
	dt_in.yuv.y = (void*)0x88888888;
	dt_in.yuv.uv = (void*)0x88888888;
	dt_in.yuv.size.width = 800;
	dt_in.yuv.size.height = 1024;

	dt_in.dt_roi.x = 88;
	dt_in.dt_roi.y = 88;
	dt_in.dt_roi.width = 88;
	dt_in.dt_roi.height = 88;

	//face size in estimation
	dt_in.target_sz.width = 60;
	dt_in.target_sz.height = 60;

	nirdt_out_t *result;
	nirdt_detect(nir_hdl, &dt_in, &result);

	for (int i = 0; i < result->size; i++)
	{
		axdt_result_t *faces = &result->p[i];
		printf("%d, %d, %d, %d, %4.5f\n", faces->roi.x, faces->roi.y, faces->roi.width, faces->roi.height, faces->score);
	}
*/

#endif