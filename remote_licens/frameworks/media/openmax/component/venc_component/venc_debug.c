#define DBG_LEVEL		DBG_VERBOSE
#define LOG_TAG			"venc_dbg"

#include "oscl.h"
#include "venc_component.h"
#include "lombo_enc_common.h"

#define DBG_PATH "/data/"

#ifdef SAVE_YUV_PICTURE
void _save_yuv_pic(lb_omx_component_t *component, int port_index,
	OMX_IN OMX_BUFFERHEADERTYPE *header, int *offset)
{
	static int yuv_frame_cnt;
	static int yuv_file_cnt;
	FILE *yuv_flip;
	unsigned char *omx_vir_addr = header->pBuffer;

	yuv_frame_cnt++;
	if (yuv_frame_cnt <= 5 || yuv_frame_cnt % 100 == 0) {
		char file_name[100] = "\0";
		int w = component->port[port_index].port_param.format.video.nFrameWidth;
		int h = component->port[port_index].port_param.format.video.nFrameHeight;
		sprintf(file_name, DBG_PATH"/%s_%d_%d.yuv",
			component->name, port_index, yuv_file_cnt % 100);
		yuv_file_cnt++;
		yuv_flip = fopen(file_name, "wb+");
		fwrite(omx_vir_addr + offset[0], 1, w * h, yuv_flip);
		fwrite(omx_vir_addr + offset[1], 1, w * h / 2, yuv_flip);
		OSCL_LOGE("w-h:%d-%d, omx_vir_addr : %p -- %d -- %d -- %d\n",
			w, h, omx_vir_addr, offset[0], offset[1], offset[2]);
		fclose(yuv_flip);
	}
}
#else
void _save_yuv_pic(lb_omx_component_t *component, int port_index,
	OMX_IN OMX_BUFFERHEADERTYPE *header, int *offset)
{
}
#endif

#ifdef SAVE_RAW_STREAM
FILE *test_flip;
static int raw_cnt_num = 150;
static int file_num;

void save_raw_stream(lb_omx_component_t *component, int port_index,
	unsigned char *pdata, int len)
{
	char file_name[100] = "\0";

	if (test_flip == NULL && raw_cnt_num > 0) {
		sprintf(file_name, DBG_PATH"/%s_%d_%d.raw",
			component->name, port_index, file_num);
		test_flip = fopen(file_name, "wb+");
	}
	if (test_flip) {
		fwrite(pdata, 1, len, test_flip);
		raw_cnt_num--;
		if (raw_cnt_num <= 0) {
			fclose(test_flip);
			test_flip = 0;
			file_num++;
		}
	}

}

FILE *test_yuv;
static int yuv_raw_cnt = 150;

void save_yuv_stream(lb_omx_component_t *component, int port_index,
	unsigned char *pdata, int len)
{
#if 0
	char file_name[100] = "\0";

	if (test_yuv == NULL && yuv_raw_cnt > 0) {
		sprintf(file_name, DBG_PATH"/%s_%d_%d.yuv",
			component->name, port_index, 0);
		test_yuv = fopen(file_name, "wb+");
	}
	if (test_yuv) {
		fwrite(pdata, 1, len, test_yuv);
		yuv_raw_cnt--;
		if (yuv_raw_cnt <= 0) {
			fclose(test_yuv);
			test_yuv = 0;
		}
	}
#endif
}
#else
void save_raw_stream(lb_omx_component_t *component, int port_index,
	unsigned char *pdata, int len)
{
}
void save_yuv_stream(lb_omx_component_t *component, int port_index,
	unsigned char *pdata, int len)
{
}
#endif

