#ifndef _NET_API_H_
#define _NET_API_H_

#include <vector>
#include <VX/vx.h>
#include <vx_ext_amd.h>
#include "./img_type.h"
/*	
 *	前处理算子支持YUV420、YVU420格式的输入图像，iuput_image
 *	结构体封装了传递给前处理算子的输入图像数据及相关参数。
 *	width：		图像宽度
 *	height：	图像高度
 *  stride1：	y通道stride
 *  stride2：	uv通道stride
 *  fmt：		图像格式
 *  handle1：	y通道数据指针
 *  handle2：	uv通道数据指针
 *  crop_x:		对输入图像进行crop的窗口起点x坐标
 *  crop_y：	对输入图像进行crop的窗口起点y坐标
 *  crop_w：	对输入图像进行crop的窗口宽度
 *  crop_h：	对输入图像进行crop的窗口高度
 */
typedef struct input_image_t {
	int width;
	int height;
	int stride1;
	int stride2;
	img_fmt fmt;
	vx_size handle1;
	vx_size handle2;
	int crop_x;
	int crop_y;
	int crop_w;
	int crop_h;
}input_image;

/*
 * 	人脸对齐参数，包括输入yuv图像和相关参数，由AI
 *	硬件完成人脸对齐和图像数据转换后，输入给识别网络
 * 	in_fmt：		输入的人脸图像格式
 * 	out_fmt:		对齐后的人脸图像格式
 * 	in_w：			输入图像宽度
 * 	in_h：			输入图像高度
 * 	in_stride_1:	输入图像y通道stride
 * 	in_stride_2：	输入图像uv通道stride
 * 	crop_x：		对输入图像进行crop的窗口起点x坐标
 * 	crop_y：		对输入图像进行crop的窗口起点y坐标
 * 	crop_w：		对输入图像进行crop的窗口宽度
 * 	crop_h：		对输入图像进行crop的窗口高度
 * 	flag：			设置为1
 * 	in_handle1：	输入图像y通道数据指针
 * 	in_handle2：	输入图像uv通道数据指针
 * 	m[6]:			人脸对齐矩阵
 */
typedef struct warp_input_t {
	img_fmt in_fmt;
	img_fmt out_fmt;
	int in_w;
	int in_h;
	int in_stride_1;
	int in_stride_2;
	int crop_x; 
	int crop_y;
	int crop_w;
	int crop_h;
	int flag;
	vx_size in_handle1;
	vx_size in_handle2;
	double m[6];
}warp_input;

/*
 * 	人脸检测网络RNet和ONet的输入yuv图像数据及参数
 * 	batch： 			滑动窗口数量，最多64个
 * 	in_fmt：			输入图像格式
 * 	img_width：			输入图像宽度
 * 	img_height: 		输入图像高度
 * 	img_stride1：		输入图像y通道stride
 * 	img_stride2：		输入图像uv通道stride
 * 	img_handle1：		输入图像y通道数据指针
 * 	img_handle2：		输入图像uv通道数据指针
 * 	out_fmt：			由AI硬件转换后的图像格式，RNet和ONet在转换后的图像数据上计算
 * 	roi_start_x[64]：	滑动窗口x坐标
 * 	roi_start_y[64]：	滑动窗口y坐标
 * 	roi_width[64]：		滑动窗口宽度
 * 	roi_height[64]：	滑动窗口高度
 */
typedef struct rsz_input_t {
	int batch;
	img_fmt in_fmt;
	int img_width;
	int img_height;
	int img_stride1;
	int img_stride2;
	int img_handle1;
	int img_handle2;
	img_fmt out_fmt;
	int roi_start_x[64];
	int roi_start_y[64];
	int roi_width[64];
	int roi_height[64];
}rsz_input;

typedef enum input_type_e {
	WARP,
	RSZ,
	TENSOR,
	TENSOR_PYRAMID,
}input_type;

typedef struct hmb_param_t {
	int threshold;
}hmb_param;

typedef enum param_type_e {
	HMB,
}param_type;

typedef struct encrypt_param_t{
	int encrypt;
}encrypt_param;

typedef struct output_blob_t {
	std::string name;
	vx_tensor tensor;
}output_blob;

typedef struct blob_t {
	int node_id;
	std::string name;
	std::vector<int> dims;
	vx_tensor tensor;
}blob;

#ifdef  __cplusplus
extern "C" {
#endif

//net
VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL ConstructNetGraph(
	vx_context context,
	vx_graph graph,
	int width,
	int height,
	int channel,
	int batch,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs,
	int hold_intermediate_blob,
	std::vector<output_blob> & out_blob);

VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL SetNetInput(
	vx_graph graph,
	input_type type,
	vx_size input);

VX_API_ENTRY vx_graph VX_API_CALL CreateNetGraph(
	vx_context context,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs
	);

VX_API_ENTRY vx_graph VX_API_CALL CreateNetDumpBlobGraph(
	vx_context context,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs,
	int blob_idx
	);

VX_API_ENTRY vx_status VX_API_CALL GetNetInputBlobs(
	vx_uint32 * net_topo,
	std::vector<blob> & input_blobs
	);

VX_API_ENTRY vx_status VX_API_CALL SetNetInputBlobsTensor(
	vx_graph graph,
	std::vector<blob> & input_blobs);

VX_API_ENTRY vx_status VX_API_CALL GetNetOutputBlobsTensor(
	vx_graph graph,
	std::vector<std::string> & name,
	std::vector<vx_tensor> & tensor);

//p-net
VX_API_ENTRY vx_graph VX_API_CALL CreatePNetGraph(
	vx_context context,
	int width,
	int height,
	int channel,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs
	);

VX_API_ENTRY vx_status VX_API_CALL SetNetParams(
	vx_graph graph,
	param_type type,
	vx_size params);

//ro-net
VX_API_ENTRY vx_graph VX_API_CALL CreateRONetGraph(
	vx_context context,
	int batch,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs
	);

VX_API_ENTRY vx_graph VX_API_CALL CreateRONetDumpBlobGraph(
	vx_context context,
	int batch,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs,
	int blob_idx
	);

//lstm
VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL ConstructLstmNetGraph(
	vx_context context,
	vx_graph graph,
	int width,
	int height,
	int channel,
	int batch,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs,
	int hold_intermediate_blob,
	std::vector<output_blob> & out_blob);

VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL SetLstmNetInput(
	vx_graph graph,
	vx_uint32 * net_topo,
	vx_tensor data_input,
	vx_tensor hidden_state,
	vx_tensor cell_state,
	vx_bool inverse);

VX_API_ENTRY vx_graph VX_API_CALL CreateLstmUnrollGraph(
	vx_context context,
	int seq_len,
	vx_uint32 * net_topo,
	vx_int8 * net_blobs
	);

VX_API_ENTRY vx_status VX_API_CALL SetLstmInOutTensor(
	vx_graph graph,
	vx_uint32 * net_topo,
	vx_bool input_inv,
	std::vector<blob> & input_blobs,
	vx_bool output_inv,
	vx_tensor output
	);

//vision
VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL ConstructTensorPyramidGraph(
	vx_context context,
	vx_graph graph,
	vx_int32 width,
	vx_int32 height,
	img_fmt input_fmt,
	vx_float32 global_scale,
	vx_float32 scale,
	vx_int32 level,
	vx_tensor_pyramid_t * tensor_out);

VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL ConstructImageConvertGraph(
	vx_context context,
	vx_graph graph,
	vx_int32 input_w,
	vx_int32 input_h,
	img_fmt input_fmt,
	vx_int32 output_w,
	vx_int32 output_h,
	vx_tensor * tensor_out);

VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL SetTensorPyramidInput(
	vx_graph graph,
	input_image * img);

VX_API_DEPRECATED VX_API_ENTRY vx_status VX_API_CALL SetImageConvertInput(
	vx_graph graph,
	input_image * img);

VX_API_ENTRY vx_size VX_API_CALL AllocatePyramidTensorsDeviceMemory(
	vx_context context,
	int width,
	int height,
	int channel,
	int batch,
	int level,
	vx_float32 global_scale,
	vx_float32 level_scale);

VX_API_ENTRY std::vector<vx_tensor> VX_API_CALL CreatePyramidTensorsFromHandle(
	vx_context context,
	int width,
	int height,
	int channel,
	int batch,
	int level,
	vx_float32 global_scale,
	vx_float32 level_scale,
	vx_size handle);

VX_API_ENTRY vx_status VX_API_CALL ImagePyramidDeviceProcess(
	vx_context context,
	input_image * pyr_img,
	std::vector<vx_tensor> & tensors);

VX_API_ENTRY vx_size VX_API_CALL AllocateTensorDeviceMemory(
	vx_context context,
	int width,
	int height,
	int channel,
	int batch);

VX_API_ENTRY vx_tensor VX_API_CALL CreateTensorFromHandle(
	vx_context context,
	int width,
	int height,
	int channel,
	int batch,
	vx_size handle);

VX_API_ENTRY vx_status VX_API_CALL ImageConvertDeviceProcess(
	vx_context context,
	input_image * image,
	vx_tensor tensor);

VX_API_ENTRY vx_status VX_API_CALL ImagePaddingDeviceProcess(
	vx_context context,
	vx_tensor input_tensor,
	vx_tensor output_tensor,
	int ins_len,
	vx_uint8 * pins);

VX_API_ENTRY vx_status VX_API_CALL FaceWarpDeviceProcess(
	vx_context context,
	warp_input * warp,
	vx_tensor tensor);

//utility
VX_API_ENTRY vx_status VX_API_CALL LoadNetModel(
	const char * model_file,
	encrypt_param * encryption_param,
	vx_int8 ** model);

VX_API_ENTRY vx_status VX_API_CALL LoadNetModelFromMem(
	vx_int8 * model_data,
	int model_len,
	encrypt_param * encryption_param,
	vx_int8 ** model);
	
VX_API_ENTRY vx_status VX_API_CALL UnLoadNetModel(vx_int8 * model);

/*
 * 	AI硬件输出数据的矩阵乘法：Mat1 X Mat2 = Mat3
 * 	context： 				设备上下文
 * 	row1：					Mat1行数
 * 	col1：					Mat1列数
 * 	fl1: 					Mat1数据量化位宽
 * 	mat1_phys_addr：		Mat1数据物理地址
 * 	row2：					Mat2行数
 * 	col2：					Mat2列数
 * 	fl2：					Mat2数据量化位宽
 * 	mat2_virt_addr：		Mat2数据虚拟地址
 * 	row3：					Mat3行数
 * 	col3：					Mat3列数
 * 	fl3：					Mat3数据量化位宽
 * 	mat3_phys_addr：		Mat3数据物理地址
 *
 *  使用方法：
 *  定义上，矩阵Amk =	|a11, a12, ..., a1k|
 * 					   |a21, a22, ..., a2k|
 * 					   |..., ..., ..., ...|
 * 					   |am1, am2, ..., amk|,
 * 		    矩阵Bkn = 	|b11, b12, ..., b1n|
 *		  			  	|b21, b22, ..., b2n|
 *		  			  	|..., ..., ..., ...|
 *					  	|bk1, bk2, ..., bkn|，
 *  矩阵乘法Cmn = Amk X Bkn =  |c11, c12, ..., c1n|
 * 							  |c21, c22, ..., c2n|
 * 							  |..., ..., ..., ...|
 * 							  |cm1, ..., ..., cmn|，
 *  其中，cij = ai1*b1j + ai2*b2j + ... + aik*bkj。
 *
 *  此函数中，将矩阵B转置，使其数据按列优先排列在内存中，
 *  Tnk = Trans(Bkn) =  |b11, b21, ..., bk1|
 * 						|b12, b22, ..., bk2|
 * 						|..., ..., ..., ...|
 * 						|b1n, b2n, ..., bkn|，
 *  定义上的矩阵乘法Cmn = Amk X Bkn 转化为Cmn = Amk X Tnk。
 *  调用此函数计算矩阵乘法时，
 *  传入的第一个矩阵为Amk，其行列数：row1 = m、col1 = k，
 *  传入的第二个矩阵为nk，其行列数：row2 = n、col2 = k。
 *  结果矩阵为Cmn，其行列数：row3 = m、col3 = n。
 *
 * 	使用限制：
 * 	同时满足：
 * 	col1 == col2、
 * 	row3 == row1、
 *  col3 == row2、
 * 	col1 % 32 == 0、
 * 	row2 % 32 == 0。
 */
VX_API_ENTRY vx_status VX_API_CALL MatMulHw(
	vx_context context,
	int row1, int col1, int fl1, vx_size mat1_phys_addr,
	int row2, int col2, int fl2, vx_size mat2_virt_addr,
	int row3, int col3, int fl3, vx_size mat3_phys_addr
	);

/*
 * 向量点乘
 */
VX_API_ENTRY vx_int32 VX_API_CALL VectorDotMul(
	int length,
	vx_int8 * vec1,
	vx_int8 * vec2);

/*
 * RGB24图像数据转换为EZ定义的RGB16图像数据格式，
 * RGB24排列方式：rgb00，rgb01，..., rgb0m, rgb10, ..., rgb1m, ..., rgbn0, ..., rgbnm
 * EZ RGB16排列方式：rgb00，rgb10，...，rgb40，0，rgb50，rgb60，...，rgb90，0，...，rgbn0，000...，rgb01，...
 * 转换规则：RGB24列数据放到EZ RGB16的行；每放5个像素后插入一个字节0；EZ RGB16的行字节数如果不是32的倍数，在行末尾插入0补齐到32的倍数。
 * srcw：RGB24图像宽
 * srch：RGB24图像高
 * src：RGB24图像数据
 * row：EZ RGB16图像数据总行数
 * col：EZ RGB16图像数据一行的字节数
 * dst：EZ RGB16图像数据
 */
VX_API_ENTRY vx_status VX_API_CALL RGB24CvtEZRGB16(
	int srcw,
	int srch,
	vx_uint8 * src,
	int * row,
	int * col,
	vx_uint8 ** dst);

#ifdef  __cplusplus
}
#endif

#endif
