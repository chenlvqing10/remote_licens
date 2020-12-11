#ifndef NV12_RESIZE_H_
#define NV12_RESIZE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "lombo_jpegdec_plugin.h"
typedef unsigned char       mmBool;
typedef unsigned char       mmUchar;
typedef unsigned char       mmUint8;
typedef unsigned char       mmByte;
typedef unsigned short      mmUint16;
typedef unsigned int        mmUint32;
typedef unsigned long       mmUint64;
typedef signed char         mmInt8;
typedef char                mmChar;
typedef signed short        mmInt16;
typedef signed int          mmInt32;
typedef signed long         mmLong;
typedef signed int          mmHandle;
typedef float               mmFloat;
typedef double              mmDouble;
typedef int                 HObj;
typedef HObj                HFile;
typedef int                 HDir;
typedef void                *mmMutexHandle;
typedef struct  _fstat {
mmInt32 fileSize;
} VE_FileAttribute;

typedef struct {
mmInt32   second;
mmInt32   millisecond;
} tsVE_Time;

typedef struct {
mmInt32   year;
mmInt32   month;
mmInt32   day;
mmInt32   hour;
mmInt32   minute;
mmInt32   second;
} TmDateTime;

/*----------------------------------------------------------------------------
    Define : TRUE/FALSE for boolean operations
----------------------------------------------------------------------------*/

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL        0
#endif

/* This structure defines the format of an image */
typedef struct {
mmInt32                 uWidth;
mmInt32                 uHeight;
mmInt32                 uStride;
jpeg_output_format_e    eFormat;
mmByte                  *imgPtr;
mmByte                  *clrPtr;
mmByte                  *clrPtr2;
mmInt32                 uOffset;
} structConvImage;

typedef struct IC_crop_struct {
mmUint32 x;             /* x pos of rectangle                              */
mmUint32 y;             /* y pos of rectangle                              */
mmUint32 uWidth;        /* dx of rectangle                                 */
mmUint32 uHeight;       /* dy of rectangle                                 */
} IC_rect_type;

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
mmBool
VT_resizeFrame_Video_opt2_lp
(
structConvImage *i_img_ptr,        /* Points to the input image           */
structConvImage *o_img_ptr,        /* Points to the output image          */
IC_rect_type *cropout,          /* how much to resize to in final image */
mmUint16 dummy                         /* Transparent pixel value              */
);

#ifdef __cplusplus
}
#endif

#endif
