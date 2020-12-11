#ifndef _LIVING_H_
#define _LIVING_H_

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************/
#ifndef _IRECTC_
#define _IRECTC_
typedef struct {
		union {int x; int col; };
		union {int y; int row; };
		union {int width; int cols; };
		union {int height; int rows; };
} IRECTC;
#endif
/***************************************/
#ifndef _IMGSIZEC_
#define _IMGSIZEC_
typedef struct {
		union {int cols; int width; };
		union {int rows; int height; };
} SIZEC;
#endif
/***************************************/
#ifndef _IMGYUVC_
#define _IMGYUVC_
typedef struct {
		SIZEC size;		/* size of y vector */
		unsigned char *yP;	/* pointer of y vector */
		unsigned char *uvP;	/* pointer of uv vector  */
} IMGYUVC;
#endif
/***************************************/
typedef struct {
		SIZEC  yuvSize;		/* rgb info */
		SIZEC  nirSize;		/* nir info */
} LivingIniIn;

typedef struct {
		char  *version;		/* version */
} LivingIniOut;
/***************************************/
typedef struct {
		IMGYUVC  imgYuv;	/* rgb image */
		IMGYUVC  imgNir;	/* nir image */

		int      yuvFaceLocExist; /* whether yuvFaceLoc exists in rgb image*/
		IRECTC   yuvFaceLoc;		/* face location in rgb image */

		int      nirFaceLocExist; /* whether yuvFaceLoc exists in nir image */
		IRECTC   nirFaceLoc;		/* face location in nir image */
} LivingFrameIn;

/*
* label 1:living 0:unsure -1:not living
*/
typedef struct {
		int label;
} LivingFrameOut;

void CreatLiving(LivingIniOut *livingIniOut, LivingIniIn *livingIniIn);
void ImportLiving(LivingFrameOut *livingFrameOut, LivingFrameIn *livingFrameIn);
void DelteteLiving(void);

#ifdef __cplusplus
}
#endif

#endif
