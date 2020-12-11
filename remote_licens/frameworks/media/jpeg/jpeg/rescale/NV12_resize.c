#include "NV12_resize.h"

#define LOG_NDEBUG 1
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0

#define LOG_TAG "NV12_resize"
#include "log/log.h"


static const mmUint8 bWeights[8][8][4] = {
	{{64, 0, 0, 0}, {56, 0, 0, 8}, {48, 0, 0, 16}, {40, 0, 0, 24},
	{32, 0, 0, 32}, {24, 0, 0, 40}, {16, 0, 0, 48}, { 8, 0, 0, 56} },

	{{56, 8, 0, 0}, {49, 7, 1, 7}, {42, 6, 2, 14}, {35, 5, 3, 21},
	{28, 4, 4, 28}, {21, 3, 5, 35}, {14, 2, 6, 42}, { 7, 1, 7, 49} },

	{{48, 16, 0, 0}, {42, 14, 2, 6}, {36, 12, 4, 12}, {30, 10, 6, 18},
	{24, 8, 8, 24}, {18, 6, 10, 30}, {12, 4, 12, 36}, { 6, 2, 14, 42} },

	{{40, 24, 0, 0}, {35, 21, 3, 5}, {30, 18, 6, 10}, {25, 15, 9, 15},
	{20, 12, 12, 20}, {15, 9, 15, 25}, {10, 6, 18, 30}, { 5, 3, 21, 35} },

	{{32, 32, 0, 0}, {28, 28, 4, 4}, {24, 24, 8, 8}, {20, 20, 12, 12},
	{16, 16, 16, 16}, {12, 12, 20, 20}, { 8, 8, 24, 24}, { 4, 4, 28, 28} },

	{{24, 40, 0, 0}, {21, 35, 5, 3}, {18, 30, 10, 6}, {15, 25, 15, 9},
	{12, 20, 20, 12}, {9, 15, 25, 15}, { 6, 10, 30, 18}, { 3, 5, 35, 21} },

	{{16, 48, 0, 0}, {14, 42, 6, 2}, {12, 36, 12, 4}, {10, 30, 18, 6},
	{8 , 24, 24, 8}, { 6, 18, 30, 10}, { 4, 12, 36, 12}, { 2, 6, 42, 14} },

	{{ 8, 56, 0, 0}, { 7, 49, 7, 1}, { 6, 42, 14, 2}, { 5, 35, 21, 3},
	{ 4, 28, 28, 4}, { 3, 21, 35, 5}, { 2, 14, 42, 6}, { 1, 7, 49, 7} }
};

/*==========================================================================
* Function Name  : VT_resizeFrame_Video_opt2_lp
*
* Description    : Resize a yuv frame.
*
* Input(s)       : input_img_ptr        -> Input Image Structure
*                : output_img_ptr       -> Output Image Structure
*                : cropout             -> crop structure
*
* Value Returned : mmBool               -> FALSE on error TRUE on success
* NOTE:
*            Not tested for crop funtionallity.
*            faster version.
============================================================================*/
mmBool VT_resizeFrame_Video_opt2_lp(
structConvImage *i_img_ptr,        /* Points to the input image           */
structConvImage *o_img_ptr,        /* Points to the output image          */
IC_rect_type *cropout,          /* how much to resize to in final image */
mmUint16 dummy                         /* Transparent pixel value              */
)
{
	ALOGV("VT_resizeFrame_Video_opt2_lp+");

	mmUint16 row, col;
	mmUint32 resizeFactorX;
	mmUint32 resizeFactorY;


	mmUint16 x, y;

	mmUchar *ptr8;
	mmUchar *ptr8Cb, *ptr8Cr;


	mmUint16 xf, yf;
	mmUchar *inImgPtrY;
	mmUchar *inImgPtrU;
	mmUchar *inImgPtrV;
	mmUint32 cox, coy, codx, cody;
	mmUint16 idx, idy;
	mmUint32 chroma_stride, unit;

	if (i_img_ptr->uWidth == o_img_ptr->uWidth) {
		if (i_img_ptr->uHeight == o_img_ptr->uHeight) {
			ALOGV("(i_img_ptr->uHeight == o_img_ptr->uHeight) are same\n");
			ALOGV("(i_img_ptr->width == %d" , i_img_ptr->uWidth);
			ALOGV("(i_img_ptr->uHeight == %d" , i_img_ptr->uHeight);
			ALOGV("(o_img_ptr->width == %d" , o_img_ptr->uWidth);
			ALOGV("(o_img_ptr->uHeight == %d" , o_img_ptr->uHeight);
		}
	}

	if (!i_img_ptr || !i_img_ptr->imgPtr ||
	!o_img_ptr || !o_img_ptr->imgPtr) {
		ALOGE("Image Point NULL");
		ALOGV("VT_resizeFrame_Video_opt2_lp-");
		return FALSE;
	}

	switch (i_img_ptr->eFormat) {
	case JPEG_NV12:
		inImgPtrY = (mmUchar *) i_img_ptr->imgPtr + i_img_ptr->uOffset;
		inImgPtrU = (mmUchar *) i_img_ptr->clrPtr + i_img_ptr->uOffset/2;
		inImgPtrV = (mmUchar *)inImgPtrU + 1;
		chroma_stride = i_img_ptr->uStride;
		unit = 2;
		break;
	case JPEG_NV21:
		inImgPtrY = (mmUchar *) i_img_ptr->imgPtr + i_img_ptr->uOffset;
		inImgPtrV = (mmUchar *) i_img_ptr->clrPtr + i_img_ptr->uOffset/2;
		inImgPtrU = (mmUchar *)inImgPtrV + 1;
		chroma_stride = i_img_ptr->uStride;
		unit = 2;
		break;
	case JPEG_YV12:
		inImgPtrY = (mmUchar *) i_img_ptr->imgPtr + i_img_ptr->uOffset;
		inImgPtrU = (mmUchar *) i_img_ptr->clrPtr + i_img_ptr->uOffset/2;
		inImgPtrV = (mmUchar *) i_img_ptr->clrPtr2 + i_img_ptr->uOffset/2;
		chroma_stride = i_img_ptr->uStride/2;
		unit = 1;
		break;
	case JPEG_YV21:
		inImgPtrY = (mmUchar *) i_img_ptr->imgPtr + i_img_ptr->uOffset;
		inImgPtrV = (mmUchar *) i_img_ptr->clrPtr + i_img_ptr->uOffset/2;
		inImgPtrU = (mmUchar *) i_img_ptr->clrPtr2 + i_img_ptr->uOffset/2;
		chroma_stride = i_img_ptr->uStride/2;
		unit = 1;
		break;
	default:
			ALOGE("not support this format\n");
			return FALSE;
	}

	if (cropout == NULL) {
		cox = 0;
		coy = 0;
		codx = o_img_ptr->uWidth;
		cody = o_img_ptr->uHeight;
	} else {
		cox = cropout->x;
		coy = cropout->y;
		codx = cropout->uWidth;
		cody = cropout->uHeight;
	}
		idx = i_img_ptr->uWidth;
		idy = i_img_ptr->uHeight;

	/* make sure valid input size */
	if (idx < 1 || idy < 1 || i_img_ptr->uStride < 1) {
		ALOGE("idx or idy less then 1 idx = %d idy = %d stride = %d",
		idx, idy, i_img_ptr->uStride);
		ALOGV("VT_resizeFrame_Video_opt2_lp-");
		return FALSE;
	}

	resizeFactorX = ((idx-1)<<9) / codx;
	resizeFactorY = ((idy-1)<<9) / cody;

	ptr8 = (mmUchar *)o_img_ptr->imgPtr + cox + coy*o_img_ptr->uWidth;

    /* for Y */
	for (row = 0; row < cody; row++) {
		mmUchar *pu8Yrow1 = NULL;
		mmUchar *pu8Yrow2 = NULL;
		y  = (mmUint16) ((mmUint32) (row*resizeFactorY) >> 9);
		yf = (mmUchar)  ((mmUint32)((row*resizeFactorY) >> 6) & 0x7);
		pu8Yrow1 = inImgPtrY + (y) * i_img_ptr->uStride;
		pu8Yrow2 = pu8Yrow1 + i_img_ptr->uStride;

		for (col = 0; col < codx; col++) {
			mmUchar in11, in12, in21, in22;
			mmUchar *pu8ptr1 = NULL;
			mmUchar *pu8ptr2 = NULL;
			mmUchar w;
			mmUint16 accum_1;

			x  = (mmUint16) ((mmUint32)  (col*resizeFactorX) >> 9);
			xf = (mmUchar)  ((mmUint32) ((col*resizeFactorX) >> 6) & 0x7);

			accum_1 =  0;

			pu8ptr1 = pu8Yrow1 + (x);
			pu8ptr2 = pu8Yrow2 + (x);

			/* A pixel */
			in11 = *(pu8ptr1);

			w = bWeights[xf][yf][0];
			accum_1 = (w * in11);

			/* B pixel */
			in12 = *(pu8ptr1+1);
			w = bWeights[xf][yf][1];
			accum_1 += (w * in12);

			/* C pixel */
			in21 = *(pu8ptr2);
			w = bWeights[xf][yf][3];
			accum_1 += (w * in21);

			/* D pixel */
			in22 = *(pu8ptr2+1);
			w = bWeights[xf][yf][2];
			accum_1 += (w * in22);

			/* divide by sum of the weights */
			accum_1 = (accum_1>>6);
			*ptr8 = (mmUchar)accum_1;

			ptr8++;
		}
		ptr8 = ptr8 + (o_img_ptr->uStride - codx);
	}
    /* for Y */

    /* for Cb-Cr */
	switch (o_img_ptr->eFormat) {
	case JPEG_NV12:
		ptr8Cb = (mmUchar *)o_img_ptr->clrPtr + cox + coy*o_img_ptr->uWidth;
		ptr8Cr = (mmUchar *)(ptr8Cb+1);
		break;
	case JPEG_NV21:
		ptr8Cr = (mmUchar *)o_img_ptr->clrPtr + cox + coy*o_img_ptr->uWidth;
		ptr8Cb = (mmUchar *)(ptr8Cr+1);
		break;
	case JPEG_YV12:
		ptr8Cb = (mmUchar *)o_img_ptr->clrPtr + cox + coy*o_img_ptr->uWidth;
		ptr8Cr = (mmUchar *)o_img_ptr->clrPtr2 + cox + coy*o_img_ptr->uWidth;
		break;
	case JPEG_YV21:
		ptr8Cr = (mmUchar *)o_img_ptr->clrPtr + cox + coy*o_img_ptr->uWidth;
		ptr8Cb = (mmUchar *)o_img_ptr->clrPtr2 + cox + coy*o_img_ptr->uWidth;
		break;
	default:
		ALOGE("not support this format\n");
		return FALSE;
	}

	for (row = 0; row < (((cody)>>1)); row++) {
		mmUchar *pu8Cbr1 = NULL;
		mmUchar *pu8Cbr2 = NULL;
		mmUchar *pu8Crr1 = NULL;
		mmUchar *pu8Crr2 = NULL;

		y  = (mmUint16) ((mmUint32) (row*resizeFactorY) >> 9);
		yf = (mmUchar)  ((mmUint32)((row*resizeFactorY) >> 6) & 0x7);

		pu8Cbr1 = inImgPtrU + (y) * chroma_stride;
		pu8Cbr2 = pu8Cbr1 + chroma_stride;
		pu8Crr1 = inImgPtrV + (y) * chroma_stride;
		pu8Crr2 = pu8Crr1 + chroma_stride;

		for (col = 0; col < (((codx)>>1)); col++) {
			mmUchar in11, in12, in21, in22;
			mmUchar *pu8Cbc1 = NULL;
			mmUchar *pu8Cbc2 = NULL;
			mmUchar *pu8Crc1 = NULL;
			mmUchar *pu8Crc2 = NULL;

			mmUchar w;
			mmUint16 accum_1Cb, accum_1Cr;

			x  = (mmUint16) ((mmUint32)  (col*resizeFactorX) >> 9);
			xf = (mmUchar)  ((mmUint32) ((col*resizeFactorX) >> 6) & 0x7);

			accum_1Cb = accum_1Cr =  0;

			pu8Cbc1 = pu8Cbr1 + (x*unit);
			pu8Cbc2 = pu8Cbr2 + (x*unit);
			pu8Crc1 = pu8Crr1 + (x*unit);
			pu8Crc2 = pu8Crr2 + (x*unit);

			/* A pixel */
			w = bWeights[xf][yf][0];

			in11 = *(pu8Cbc1);
			accum_1Cb = (w * in11);

			in11 = *(pu8Crc1);
			accum_1Cr = (w * in11);

			/* B pixel */
			w = bWeights[xf][yf][1];

			in12 = *(pu8Cbc1+2);
			accum_1Cb += (w * in12);

			in12 = *(pu8Crc1+2);
			accum_1Cr += (w * in12);

			/* C pixel */
			w = bWeights[xf][yf][3];

			in21 = *(pu8Cbc2);
			accum_1Cb += (w * in21);

			in21 = *(pu8Crc2);
			accum_1Cr += (w * in21);

			/* D pixel */
			w = bWeights[xf][yf][2];

			in22 = *(pu8Cbc2+2);
			accum_1Cb += (w * in22);

			in22 = *(pu8Crc2+2);
			accum_1Cr += (w * in22);

			/* divide by sum of the weights */
			accum_1Cb = (accum_1Cb>>6);
			*ptr8Cb = (mmUchar)accum_1Cb;


			accum_1Cr = (accum_1Cr >> 6);
			*ptr8Cr = (mmUchar)accum_1Cr;

			ptr8Cb += unit;
			ptr8Cr += unit;
		}
		ptr8Cb = ptr8Cb + (o_img_ptr->uStride - codx)*unit/2;
		ptr8Cr = ptr8Cr + (o_img_ptr->uStride - codx)*unit/2;
	}
    /* For Cb- Cr */

	ALOGV("success");
	ALOGV("VT_resizeFrame_Video_opt2_lp-");
	return TRUE;
}
