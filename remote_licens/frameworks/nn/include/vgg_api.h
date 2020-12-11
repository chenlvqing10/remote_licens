#ifndef vggAPI_H
#define vggAPI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	int x;
	int y;
	int width;
	int height;
} vgg_rect_t;


typedef struct {
	int width;
	int height;
} vgg_size_t;


typedef struct {
	void *y;
	void *uv;
	vgg_size_t size;
} vgg_imgyuv_t;


typedef struct {
	vgg_imgyuv_t yuv;
	vgg_rect_t   dt_roi;
	vgg_size_t   target_sz;
} vgg_dt_in_t;


typedef struct vgg_fn_s {
	int (*vgg_recv_buffer)(void *hdl, void *param, void *virAddr, int *result);
} vgg_fn_t;

vgg_fn_t *vgg_open(int w, int h);
void vgg_close(void *hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
