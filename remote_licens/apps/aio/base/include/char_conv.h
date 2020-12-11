/*
 * char_conv.h - Convert UTF-8 characters and GB2312 characters to each other.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _CHAR_CONV_H
#define _CHAR_CONV_H

/**
 * utf8_to_gb2312 - Convert characters from UTF-8 encoding to GB2312.
 * @inbuf: IN, Pointer to the input string.
 * @inbuf: IN, Input string length.
 * @outbuf: OUT, Output string pointer after conversion.
 * @outlen OUT, Output string length after conversion.
 *
 * Returns 0 if success, otherwise not 0.
 */
int utf8_to_gb2312(char *inbuf, int inlen, char *outbuf, int outlen);

/**
 * gb2312_to_utf8 - Convert characters from GB2312 encoding to UTF-8.
 * @inbuf: IN, Pointer to the input string.
 * @inbuf: IN, Input string length.
 * @outbuf: OUT, Output string pointer after conversion.
 * @outlen OUT, Output string length after conversion.
 *
 * Returns 0 if success, otherwise not 0.
 */
int gb2312_to_utf8(char *inbuf, int inlen, char *outbuf, int outlen);

#endif  /* _CHAR_CONV_H */

