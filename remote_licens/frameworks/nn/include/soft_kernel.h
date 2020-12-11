#ifndef __SOFT_KERNEL_H__
#define __SOFT_KERNEL_H__

typedef struct {
	int type;
} GeneralParam;

typedef struct {
	GeneralParam head;
	int iw, ih, ic; //total ic
	int ic_start, ic_width;
	int ow, oh, oc; //total oc
	int oc_start,oc_width;
	int group;
	int kw, kh, pw, ph, sw, sh, dw, dh;
} ConvParam;

enum DataType {
	INT8 = 1,
	INT16 = 2,
	FLOAT32 = 4,
};

typedef struct {
	int bw_in, bw_out, bw_para;
	int fl_in, fl_out, fl_para;	
	DataType type;
} QuantParam;

typedef int (*SOFT_CONV_FUNC)(ConvParam* pConvParam, QuantParam* pQuantParam, void* bottom_addr, void* top_addr, void* weight_addr, void* bias_addr);

extern SOFT_CONV_FUNC sw_convolution_func;

#endif
