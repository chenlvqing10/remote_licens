/*
 * rotation_neon.c - Rotation neon module driver code for LomboTech
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
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

#ifndef __ROTATION_NEON_C__
#define __ROTATION_NEON_C__
/**
 * transpose_8bit_8row_neon - transpose 8row blocks for 8bit format data
 * @src: the buffer address of source image
 * @src_line_stride: the line stride of source image
 * @dst: the buffer address of destination image
 * @dst_line_stride: the line stride of destination image
 * @src_width: the horizontal resolution of source image
 *
 * No return.
 */
void transpose_8bit_8row_neon(const unsigned char *src,
			      int src_line_stride,
			      unsigned char *dst,
			      int dst_line_stride,
			      int src_width)
{
	const unsigned char *temp;

	asm volatile(
		/* if src_width < 8, process 4x8 block */
		"cmp         %5, #8                      \n"
		"blt         2f                          \n"

		"sub         %5, #8                     \n"

		/* 8x8 blocks */
		"1:                                      \n"

		/* read one block of data from src buffer */
		"mov         %0, %1                      \n"

		"vld1.8      {d0}, [%0], %2              \n"
		"vld1.8      {d1}, [%0], %2              \n"
		"vld1.8      {d2}, [%0], %2              \n"
		"vld1.8      {d3}, [%0], %2              \n"
		"vld1.8      {d4}, [%0], %2              \n"
		"vld1.8      {d5}, [%0], %2              \n"
		"vld1.8      {d6}, [%0], %2              \n"
		"vld1.8      {d7}, [%0]                  \n"

		/* transpose 8x8 blocks */
		"vtrn.8      d0, d1                      \n"
		"vtrn.8      d2, d3                      \n"
		"vtrn.8      d4, d5                      \n"
		"vtrn.8      d6, d7                      \n"

		"vtrn.16     d0, d2                      \n"
		"vtrn.16     d4, d6                      \n"
		"vtrn.16     d1, d3                      \n"
		"vtrn.16     d5, d7                      \n"

		"vtrn.32     d0, d4                      \n"
		"vtrn.32     d2, d6                      \n"
		"vtrn.32     d1, d5                      \n"
		"vtrn.32     d3, d7                      \n"

		/* write one block of data to the dst buffer */
		"mov         %0, %3                      \n"

		"vst1.8      {d0}, [%0], %4              \n"
		"vst1.8      {d1}, [%0], %4              \n"
		"vst1.8      {d2}, [%0], %4              \n"
		"vst1.8      {d3}, [%0], %4              \n"
		"vst1.8      {d4}, [%0], %4              \n"
		"vst1.8      {d5}, [%0], %4              \n"
		"vst1.8      {d6}, [%0], %4              \n"
		"vst1.8      {d7}, [%0]                  \n"

		/* src and dst step to the next block */
		"add         %1, #8                      \n"  /* src += 8 */
		"add         %3, %3, %4, lsl #3          \n"  /* dst += 8 * dst_line_stride */
		"subs        %5, #8                      \n"  /* src_width -= 8 */
		"bge         1b                          \n"  /* if src_width >= 8, continue 8x8 blocks process */

		"adds        %5, #8                      \n"
		"beq         5f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		"2:                                      \n"

		/* if src_width < 2, process 1x8 block */
		"cmp         %5, #2                      \n"
		"blt         4f                          \n"

		/* if src_width < 4, process 2x8 block */
		"cmp         %5, #4                      \n"
		"blt         3f                          \n"

		/* 4x8 block */
		"mov         %0, %1                      \n"
		"vld1.32     {d0[0]}, [%0], %2           \n"
		"vld1.32     {d1[0]}, [%0], %2           \n"
		"vld1.32     {d2[0]}, [%0], %2           \n"
		"vld1.32     {d3[0]}, [%0], %2           \n"
		"vld1.32     {d0[1]}, [%0], %2           \n"
		"vld1.32     {d1[1]}, [%0], %2           \n"
		"vld1.32     {d2[1]}, [%0], %2           \n"
		"vld1.32     {d3[1]}, [%0]               \n"

		"vtrn.8      d0, d1                       \n"
		"vtrn.8      d2, d3                       \n"

		"mov         %0, %3                      \n"

		"vtrn.16     d0, d2                      \n"
		"vtrn.16     d1, d3                      \n"  /* perf limit */

		"vst1.32     {d0}, [%0], %4              \n"
		"vst1.32     {d1}, [%0], %4              \n"
		"vst1.32     {d2}, [%0], %4              \n"
		"vst1.32     {d3}, [%0]                  \n"

		"add         %1, #4                      \n"  /* src += 4 */
		"add         %3, %3, %4, lsl #2          \n"  /* dst += 4 * dst_line_stride */
		"subs        %5,  #4                     \n"  /* src_width -= 4 */
		"beq         5f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		/* if src_width < 2, process 1x8 block */
		"cmp         %5, #2                      \n"
		"blt         4f                          \n"

		/* 2x8 block */
		"3:                                      \n"
		"mov         %0, %1                      \n"

		"vld2.8      {d0[0], d1[0]}, [%0], %2    \n"
		"vld2.8      {d0[1], d1[1]}, [%0], %2    \n"
		"vld2.8      {d0[2], d1[2]}, [%0], %2    \n"
		"vld2.8      {d0[3], d1[3]}, [%0], %2    \n"
		"vld2.8      {d0[4], d1[4]}, [%0], %2    \n"
		"vld2.8      {d0[5], d1[5]}, [%0], %2    \n"
		"vld2.8      {d0[6], d1[6]}, [%0], %2    \n"
		"vld2.8      {d0[7], d1[7]}, [%0]        \n"

		"mov         %0, %3                      \n"
		"vst1.64     {d0}, [%0], %4              \n"  /* perf limit */
		"vst1.64     {d1}, [%0]                  \n"

		"add         %1, #2                      \n"  /* src += 2 */
		"add         %3, %3, %4, lsl #1          \n"  /* dst += 2 * dst_line_stride */
		"subs        %5,  #2                     \n"  /* src_width -= 2 */
		"beq         5f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		/* 1x8 block */
		"4:                                      \n"
		"vld1.8      {d0[0]}, [%1], %2           \n"
		"vld1.8      {d0[1]}, [%1], %2           \n"
		"vld1.8      {d0[2]}, [%1], %2           \n"
		"vld1.8      {d0[3]}, [%1], %2           \n"
		"vld1.8      {d0[4]}, [%1], %2           \n"
		"vld1.8      {d0[5]}, [%1], %2           \n"
		"vld1.8      {d0[6]}, [%1], %2           \n"
		"vld1.8      {d0[7]}, [%1]               \n"

		"vst1.64     {d0}, [%3]                  \n"  /* perf limit */

		"5:                                      \n"  /* exit */

		: "=&r"(temp),            /* %0 */
		  "+r"(src),              /* %1 */
		  "+r"(src_line_stride),  /* %2 */
		  "+r"(dst),              /* %3 */
		  "+r"(dst_line_stride),  /* %4 */
		  "+r"(src_width)         /* %5 */
		:
		: "memory", "cc", "q0", "q1", "q2", "q3");
}

void transpose_16bit_8row_neon(const unsigned char *src,
			       int src_line_stride,
			       unsigned char *dst,
			       int dst_line_stride,
			       int src_width)
{
	const unsigned char *temp;

	asm volatile(
		/* if src_width < 8, process 4x8 block */
		"cmp         %5, #8                      \n"
		"blt         2f                          \n"

		"sub         %5, #8                      \n"

		/* 8x8 blocks */
		"1:                                      \n"

		/* read one block of data from src buffer */
		"mov         %0, %1                      \n"

		"vld1.16     {q0}, [%0], %2              \n"
		"vld1.16     {q1}, [%0], %2              \n"
		"vld1.16     {q2}, [%0], %2              \n"
		"vld1.16     {q3}, [%0], %2              \n"
		"vld1.16     {q4}, [%0], %2              \n"
		"vld1.16     {q5}, [%0], %2              \n"
		"vld1.16     {q6}, [%0], %2              \n"
		"vld1.16     {q7}, [%0]                  \n"

		/* transpose 8x8 blocks */
		"vtrn.16     q0, q1                      \n"
		"vtrn.16     q2, q3                      \n"
		"vtrn.16     q4, q5                      \n"
		"vtrn.16     q6, q7                      \n"

		"vtrn.32     q0, q2                      \n"
		"vtrn.32     q4, q6                      \n"
		"vtrn.32     q1, q3                      \n"
		"vtrn.32     q5, q7                      \n"

		"vswp.8      d1, d8                      \n"
		"vswp.8      d3, d10                     \n"
		"vswp.8      d5, d12                     \n"
		"vswp.8      d7, d14                     \n"

		/* write one block of data to the dst buffer */
		"mov         %0, %3                      \n"

		"vst1.16     {q0}, [%0], %4              \n"    /* vst1.64 ? */
		"vst1.16     {q1}, [%0], %4              \n"
		"vst1.16     {q2}, [%0], %4              \n"
		"vst1.16     {q3}, [%0], %4              \n"
		"vst1.16     {q4}, [%0], %4              \n"
		"vst1.16     {q5}, [%0], %4              \n"
		"vst1.16     {q6}, [%0], %4              \n"
		"vst1.16     {q7}, [%0]                  \n"

		/* src and dst step to the next block */
		"add         %1, #16                     \n"  /* src += 8 * 2 */
		"add         %3, %3, %4, lsl #3          \n"  /* dst += 8 * dst_line_stride */
		"subs        %5,  #8                     \n"  /* src_width -= 8 */
		"bge         1b                          \n"  /* if src_width >= 8, continue 8x8 blocks process */

		"adds        %5, #8                      \n"
		"beq         5f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		"2:                                      \n"
		/* if src_width < 2, process 1x8 block */
		"cmp         %5, #2                      \n"
		"blt         4f                          \n"

		/* if src_width < 4, process 2x8 block */
		"cmp         %5, #4                      \n"
		"blt         3f                          \n"

		/* 4x8 block */
		"mov         %0, %1                      \n"
		"vld1.16     {d0}, [%0], %2              \n"
		"vld1.16     {d2}, [%0], %2              \n"
		"vld1.16     {d4}, [%0], %2              \n"
		"vld1.16     {d6}, [%0], %2              \n"
		"vld1.16     {d1}, [%0], %2              \n"
		"vld1.16     {d3}, [%0], %2              \n"
		"vld1.16     {d5}, [%0], %2              \n"
		"vld1.16     {d7}, [%0]                  \n"

		"vtrn.16     q0, q1                      \n"
		"vtrn.16     q2, q3                      \n"

		"mov         %0, %3                      \n"

		"vtrn.32     q0, q2                      \n"
		"vtrn.32     q1, q3                      \n"  /* perf limit */

		"vst1.16     {q0}, [%0], %4              \n"
		"vst1.16     {q1}, [%0], %4              \n"
		"vst1.16     {q2}, [%0], %4              \n"
		"vst1.16     {q3}, [%0]                  \n"

		"add         %1, #8                      \n"  /* src += 4 * 2 */
		"add         %3, %3, %4, lsl #2          \n"  /* dst += 4 * dst_line_stride */
		"subs        %5, #4                      \n"  /* src_width -= 4 */
		"beq         5f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		/* if src_width < 2, process 1x8 block */
		"cmp         %5, #2                      \n"
		"blt         4f                          \n"

		/* 2x8 block */
		"3:                                      \n"
		"mov         %0, %1                      \n"

		"vld2.16     {d0[0], d2[0]}, [%0], %2    \n"  /* only d register may be indexed */
		"vld2.16     {d0[1], d2[1]}, [%0], %2    \n"
		"vld2.16     {d0[2], d2[2]}, [%0], %2    \n"
		"vld2.16     {d0[3], d2[3]}, [%0], %2    \n"
		"vld2.16     {d1[0], d3[0]}, [%0], %2    \n"
		"vld2.16     {d1[1], d3[1]}, [%0], %2    \n"
		"vld2.16     {d1[2], d3[2]}, [%0], %2    \n"
		"vld2.16     {d1[3], d3[3]}, [%0]        \n"

		"mov         %0, %3                      \n"
		"vst1.64     {q0}, [%0], %4              \n"  /* perf limit */
		"vst1.64     {q1}, [%0]                  \n"

		"add         %1, #4                      \n"  /* src += 2 * 2 */
		"add         %3, %3, %4, lsl #1          \n"  /* dst += 2 * dst_line_stride */
		"subs        %5, #2                      \n"  /* src_width -= 2 */
		"beq         5f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		/* 1x8 block */
		"4:                                      \n"
		"vld1.16     {d0[0]}, [%1], %2           \n"
		"vld1.16     {d0[1]}, [%1], %2           \n"
		"vld1.16     {d0[2]}, [%1], %2           \n"
		"vld1.16     {d0[3]}, [%1], %2           \n"
		"vld1.16     {d1[0]}, [%1], %2           \n"
		"vld1.16     {d1[1]}, [%1], %2           \n"
		"vld1.16     {d1[2]}, [%1], %2           \n"
		"vld1.16     {d1[3]}, [%1]               \n"

		"vst1.64     {q0}, [%3]                  \n"  /* perf limit */


		"5:                                      \n"  /* exit */

		: "=&r"(temp),            /* %0 */
		  "+r"(src),              /* %1 */
		  "+r"(src_line_stride),  /* %2 */
		  "+r"(dst),              /* %3 */
		  "+r"(dst_line_stride),  /* %4 */
		  "+r"(src_width)         /* %5 */
		:
		: "memory", "cc", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7");
}

void transpose_32bit_4row_neon(const unsigned char *src,
			       int src_line_stride,
			       unsigned char *dst,
			       int dst_line_stride,
			       int src_width)
{
	const unsigned char *temp;

	asm volatile(
		/* if src_width < 4, process 2x4 block */
		"cmp         %5, #4                      \n"
		"blt         2f                          \n"

		"sub         %5, #4                     \n"

		/* 4x4 blocks */
		"1:                                      \n"

		/* read one block of data from src buffer */
		"mov         %0, %1                      \n"

		"vld1.64     {q0}, [%0], %2              \n"
		"vld1.64     {q1}, [%0], %2              \n"
		"vld1.64     {q2}, [%0], %2              \n"
		"vld1.64     {q3}, [%0]                  \n"

		/* transpose 4x4 blocks */
		"vtrn.32     q0, q1                      \n"
		"vtrn.32     q2, q3                      \n"

		"vswp.8      d1, d4                      \n"
		"vswp.8      d3, d6                      \n"

		/* write one block of data to the dst buffer */
		"mov         %0, %3                      \n"

		"vst1.64     {q0}, [%0], %4              \n"    /* vst1.64 ? */
		"vst1.64     {q1}, [%0], %4              \n"
		"vst1.64     {q2}, [%0], %4              \n"
		"vst1.64     {q3}, [%0]                  \n"

		/* src and dst step to the next block */
		"add         %1, #16                     \n"  /* src += 4 * 4 */
		"add         %3, %3, %4, lsl #2          \n"  /* dst += 4 * dst_line_stride */
		"subs        %5, #4                      \n"  /* src_width -= 4 */
		"bge         1b                          \n"  /* if src_width >= 4, continue 8x8 blocks process */

		"adds        %5, #4                      \n"
		"beq         4f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */
		"2:                                      \n"

		/* if src_width < 2, process 1x4 block */
		"cmp         %5, #2                      \n"
		"blt         3f                          \n"

		/* 2x4 block */
		"mov         %0, %1                      \n"

		"vld2.32     {d0[0], d2[0]}, [%0], %2    \n"  /* only d register may be indexed */
		"vld2.32     {d0[1], d2[1]}, [%0], %2    \n"
		"vld2.32     {d1[0], d3[0]}, [%0], %2    \n"
		"vld2.32     {d1[1], d3[1]}, [%0]        \n"

		"mov         %0, %3                      \n"
		"vst1.32     {q0}, [%0], %4              \n"  /* perf limit, vst1.64? */
		"vst1.32     {q1}, [%0]                  \n"

		"add         %1, #8                      \n"  /* src += 2 * 4 */
		"add         %3, %3, %4, lsl #1          \n"  /* dst += 2 * dst_line_stride */
		"subs        %5, #2                      \n"  /* src_width -= 2 */
		"beq         4f                          \n"  /* if src_width eq 0, indicate all the blocks have been processed,
							       * just exit
							       */

		/* 1x8 block */
		"3:                                      \n"
		"vld1.32     {d0[0]}, [%1], %2           \n"
		"vld1.32     {d0[1]}, [%1], %2           \n"
		"vld1.32     {d1[0]}, [%1], %2           \n"
		"vld1.32     {d1[1]}, [%1]               \n"

		"vst1.64     {q0}, [%3]                  \n"  /* perf limit */


		"4:                                      \n"  /* exit */
		: "=&r"(temp),            /* %0 */
		  "+r"(src),              /* %1 */
		  "+r"(src_line_stride),  /* %2 */
		  "+r"(dst),              /* %3 */
		  "+r"(dst_line_stride),  /* %4 */
		  "+r"(src_width)         /* %5 */
		:
		: "memory", "cc", "q0", "q1", "q2", "q3");
}

/**
 * mirror_8bit_1row_neon - mirror 1row of image data for 8bit format
 * @src: the buffer address of source image
 * @dst: the buffer address of destination image
 * @width: the horizontal resolution of source image
 *
 * No return.
 */
void mirror_8bit_1row_neon(const unsigned char *src, unsigned char *dst, int width)
{
	asm volatile(
		"add        %0, %0, %2                   \n"  /* src move forward to the end of row */

		"cmp         %2, #16                     \n"
		"blt         2f                          \n"

		"sub        %0, #16                      \n"  /* src move backward to the start of last 16bytes */
		"mov        r3, #-16                     \n"

		"subs       %2, #16                      \n"
		/* 16bytes micro row */
		"1:                                      \n"
		"subs       %2, #16                      \n"
		"vld1.8     {q0}, [%0], r3               \n"  /* src -= 16 */
		"vrev64.8   q0, q0                       \n"
		"vst1.8     {d1}, [%1]!                  \n"  /* dst += 16 */
		"vst1.8     {d0}, [%1]!                  \n"
		"bge        1b                           \n"

		"add         %0, #16                     \n"
		"adds        %2, #16                     \n"
		"beq         6f                          \n"  /* if width eq 0,
							       * indicate all the micro row have been processed,
							       * just exit
							       */
		"2:                                      \n"
		/* if width < 2, process 1byte micro row */
		"cmp         %2, #2                      \n"
		"blt         5f                          \n"

		/* if width < 4, process 2bytes  micro row */
		"cmp         %5, #4                      \n"
		"blt         4f                          \n"

		/* if width < 8, process 4bytes  micro row */
		"cmp         %5, #8                      \n"
		"blt         3f                          \n"

		/* 8bytes micro row */
		"sub        %0, #8                       \n"  /* src move backward to the start of last 8bytes */
		"mov        r3, #-8                      \n"
		"subs       %2, #8                       \n"
		"vld1.8     {d0}, [%0]                   \n"
		"vrev64.8   d0, d0                       \n"
		"vst1.8     {d0}, [%1]!                  \n"  /* dst += 8 */
		"beq        6f                           \n"

		/* if width < 2, process 1byte micro row */
		"cmp         %2, #2                      \n"
		"blt         5f                          \n"

		/* if width < 4, process 2bytes  micro row */
		"cmp         %5, #4                      \n"
		"blt         4f                          \n"

		/* 4bytes micro row */
		"3:                                      \n"
		"sub        %0, #4                       \n"  /* src move backward to the start of last 4bytes */
		"mov        r3, #-4                      \n"
		"subs       %2, #4                       \n"
		"vld1.32    {d0[0]}, [%0]                \n"
		"vrev32.8   d0, d0                       \n"
		"vst1.32    {d0[0]}, [%1]!               \n"  /* dst += 4 */
		"beq        6f                           \n"

		/* if width < 2, process 1byte micro row */
		"cmp         %2, #2                      \n"
		"blt         5f                          \n"

		/* 2bytes micro row */
		"4:                                      \n"
		"sub        %0, #2                       \n"  /* src move backward to the start of last 2bytes */
		"mov        r3, #-2                      \n"
		"subs       %2, #2                       \n"
		"vld1.16    {d0[0]}, [%0]                \n"
		"vrev16.8   d0, d0                       \n"
		"vst1.16    {d0[0]}, [%1]!               \n"  /* dst += 2 */
		"beq        6f                           \n"

		/* 1bytes micro row */
		"5:                                      \n"
		"sub        %0, #1                       \n"  /* src move backward to the start of last 1bytes */
		"vld1.8     {d0[0]}, [%0]                \n"
		"vst1.8     {d0[0]}, [%1]!               \n"  /* dst += 1 */


		"6:                                      \n"

		: "+r"(src),   /* %0 */
		  "+r"(dst),   /* %1 */
		  "+r"(width)  /* %2 */
		:
		: "memory", "cc", "r3", "q0");
}

void mirror_16bit_1row_neon(const unsigned char *src, unsigned char *dst, int width)
{
	asm volatile(
		"add        %0, %0, %2, lsl #1           \n"  /* src move forward to the end of row */

		"cmp         %2, #8                      \n"
		"blt         2f                          \n"

		"sub        %0, #16                      \n"  /* src move backward to the start of last 16bytes */
		"mov        r3, #-16                     \n"

		"subs       %2, #8                      \n"
		/* 16bytes(8pixels) micro row */
		"1:                                      \n"
		"subs       %2, #8                       \n"
		"vld1.8     {q0}, [%0], r3               \n"  /* src -= 16 */
		"vrev64.16  q0, q0                       \n"
		"vst1.8     {d1}, [%1]!                  \n"  /* dst += 16 */
		"vst1.8     {d0}, [%1]!                  \n"
		"bge        1b                           \n"

		"add         %0, #16                     \n"
		"adds        %2, #8                      \n"
		"beq         5f                          \n"  /* if width eq 0, indicate all the micro row have been processed,
							       * just exit
							       */
		"2:                                      \n"
		/* if width < 2, process 2byte(1pixel) micro row */
		"cmp         %2, #2                      \n"
		"blt         4f                          \n"

		/* if width < 4, process 4bytes(2pixel)  micro row */
		"cmp         %5, #4                      \n"
		"blt         3f                          \n"

		/* 8bytes(4pixel) micro row */
		"sub        %0, #8                       \n"  /* src move backward to the start of last 8bytes */
		"subs       %2, #4                       \n"
		"vld1.8     {d0}, [%0]                   \n"
		"vrev64.16  d0, d0                       \n"
		"vst1.8     {d0}, [%1]!                  \n"  /* dst += 8 */
		"beq        5f                           \n"

		/* if width < 2, process 2byte(1pxiel) micro row */
		"cmp         %2, #2                      \n"
		"blt         4f                          \n"

		/* 4bytes(2pixel) micro row */
		"3:                                      \n"
		"sub        %0, #4                       \n"  /* src move backward to the start of last 4bytes */
		"subs       %2, #2                       \n"
		"vld1.32    {d0[0]}, [%0]                \n"
		"vrev32.16  d0, d0                       \n"
		"vst1.32    {d0[0]}, [%1]!               \n"  /* dst += 4 */
		"beq        5f                           \n"

		/* 2bytes(1pixel) micro row */
		"4:                                      \n"
		"sub        %0, #2                       \n"  /* src move backward to the start of last 2bytes */
		"subs       %2, #1                       \n"
		"vld1.16    {d0[0]}, [%0]                \n"
		"vst1.16    {d0[0]}, [%1]!               \n"  /* dst += 2 */


		"5:                                      \n"
		: "+r"(src),   /* %0 */
		  "+r"(dst),   /* %1 */
		  "+r"(width)  /* %2 */
		:
		: "memory", "cc", "r3", "q0");
}

void mirror_32bit_1row_neon(const unsigned char *src, unsigned char *dst, int width)
{
	asm volatile(
		"add        %0, %0, %2, lsl #2           \n"  /* src move forward to the end of row */

		"cmp         %2, #4                      \n"
		"blt         2f                          \n"

		"sub        %0, #16                      \n"  /* src move backward to the start of last 16bytes */
		"mov        r3, #-16                     \n"

		"subs       %2, #4                       \n"
		/* 16bytes(4pixels) micro row */
		"1:                                      \n"
		"subs       %2, #4                       \n"
		"vld1.8     {q0}, [%0], r3               \n"  /* src -= 16 */
		"vrev64.32  q0, q0                       \n"
		"vst1.8     {d1}, [%1]!                  \n"  /* dst += 16 */
		"vst1.8     {d0}, [%1]!                  \n"
		"bge        1b                           \n"

		"add         %0, #16                     \n"
		"adds        %2, #4                      \n"
		"beq         4f                          \n"  /* if width eq 0, indicate all the micro row have been processed,
							       * just exit
							       */
		"2:                                      \n"
		/* if width < 2, process 2byte(1pixel) micro row */
		"cmp         %2, #2                      \n"
		"blt         3f                          \n"

		/* 8bytes(2pixel) micro row */
		"sub        %0, #8                       \n"  /* src move backward to the start of last 8bytes */
		"subs       %2, #2                       \n"
		"vld1.8     {d0}, [%0]                   \n"
		"vrev64.32  d0, d0                       \n"
		"vst1.8     {d0}, [%1]!                  \n"  /* dst += 8 */
		"beq        4f                           \n"

		/* 4bytes(1pixel) micro row */
		"3:                                      \n"
		"sub        %0, #4                       \n"  /* src move backward to the start of last 4bytes */
		"subs       %2, #1                       \n"
		"vld1.32    {d0[0]}, [%0]                \n"
		"vst1.32    {d0[0]}, [%1]!               \n"  /* dst += 4 */


		"4:                                      \n"
		: "+r"(src),   /* %0 */
		  "+r"(dst),   /* %1 */
		  "+r"(width)  /* %2 */
		:
		: "memory", "cc", "r3", "q0");
}

/**
 * copy_neon - copy the image from src to the dst
 * @src: the buffer address of source image
 * @dst: the buffer address of destination image
 * @bytes: the bytes of data to be copied
 *
 * No return.
 */
void copy_neon(const unsigned char *src, unsigned char *dst, int bytes)
{
	asm volatile(
		"cmp         %2, #128                     \n"
		"blt         3f                           \n"

		"subs       %2, #128                     \n"
		"pld        [%0, #0]                     \n"
		"pld        [%0, #64]                    \n"

		/* 128bytes micro row */
		"1:                                      \n"
		"pld        [%0, #128]                   \n"
		"pld        [%0, #192]                   \n"
		"2:                                      \n"

		"vld1.32    {q0}, [%0]!                  \n"  /* src += 16 */
		"vld1.32    {q1}, [%0]!                  \n"
		"vst1.32    {q0}, [%1]!                  \n"  /* dst += 16 */
		"vld1.32    {q2}, [%0]!                  \n"
		"subs       %2, #128                     \n"
		"vst1.32    {q1}, [%1]!                  \n"
		"vld1.32    {q3}, [%0]!                  \n"
		"vst1.32    {q2}, [%1]!                  \n"
		"vld1.32    {q4}, [%0]!                  \n"
		"vst1.32    {q3}, [%1]!                  \n"
		"vld1.32    {q5}, [%0]!                  \n"
		"vst1.32    {q4}, [%1]!                  \n"
		"vld1.32    {q6}, [%0]!                  \n"
		"vst1.32    {q5}, [%1]!                  \n"
		"vld1.32    {q7}, [%0]!                  \n"
		"vst1.32    {q6}, [%1]!                  \n"
		"vst1.32    {q7}, [%1]!                  \n"

		"bgt        1b                           \n"
		"bge        2b                           \n"

		"adds        %2, #128                    \n"
		"beq         8f                          \n"  /* if width eq 0, indicate all the micro row have been processed,
							       * just exit
							       */
		"3:                                      \n"
		/* if width < 8, process 1byte micro row */
		"cmp         %2, #8                      \n"
		"blt         6f                          \n"

		/* if width < 16, process 8byte micro row */
		"cmp         %2, #16                      \n"
		"blt         5f                          \n"

		/* 16bytes micro row */
		"subs       %2, #16                       \n"
		"4:                                      \n"
		"subs       %2, #16                      \n"
		"vld1.32    {q0}, [%0]!                  \n"  /* src += 16 */
		"vst1.32    {q0}, [%1]!                  \n"  /* dst += 16 */
		"bge        4b                           \n"

		"adds       %2, #16                      \n"
		"beq        8f                           \n"

		/* if width < 8, process 1byte micro row */
		"cmp         %2, #8                      \n"
		"blt         6f                          \n"

		/* 8bytes micro row */
		"5:                                      \n"
		"subs       %2, #8                       \n"
		"vld1.32    {d0}, [%0]!                  \n"  /* src += 8 */
		"vst1.32    {d0}, [%1]!                  \n"  /* dst += 8 */

		"beq         8f                          \n"

		"6:                                      \n"
		/* 1bytes micro row */
		"subs       %2, #1                       \n"
		"sub        %0, #1                       \n"
		"sub        %1, #1                       \n"
		"7:                                      \n"
		"subs       %2, #1                       \n"
		"ldrb	    r3, [%0, #1]!                \n"
		"strb       r3, [%1, #1]!                \n"
		"bge        7b                           \n"


		"8:                                      \n"
		: "+r"(src),   /* %0 */
		  "+r"(dst),   /* %1 */
		  "+r"(bytes)  /* %2 */
		:
		: "memory", "cc", "r3", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7");
}

#endif /* __ROTATION_NEON_C__ */
