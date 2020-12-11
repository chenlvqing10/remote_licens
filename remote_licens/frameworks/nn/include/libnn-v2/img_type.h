#ifndef __IMG_TYPE_H
#define __IMG_TYPE_H
typedef enum  image_format_t
{
	RGB = 0x00,
	RGB_WHC = 0x01,
	YUV420 = 0x02,
	YVU420 = 0x03,
	GRAY = 0x04,
	GRAY_WHC = 0x05
}img_fmt;


#endif