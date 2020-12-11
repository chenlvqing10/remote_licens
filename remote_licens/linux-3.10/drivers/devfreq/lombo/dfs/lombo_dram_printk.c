/*
 * drivers/devfreq/lombo_dram_entry.c
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Driver of LomboTech DRAM frequency
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "lombo_dram_printk.h"

#ifdef __SRAM_DRAMFREQ_DEBUG

/* delete some inessential function to optimize code size */
#define CODE_SIZE_OPT

void int_to_hex_str(s32 val, u8 *str, u32 is_caps)
{
	u8 buf[8] = "";
	s32 i = 0;
	s32 len = 0;

	for (i = 0; i < 8; ++i) {
		if (is_caps)
			buf[i] = "0123456789ABCDEF"[val & 0xf];
		else
			buf[i] = "0123456789abcdef"[val & 0xf];

		val >>= 4;
	}

	str[0] = '0';
	if (is_caps)
		str[1] = 'X';
	else
		str[1] = 'x';

	for (i = 7, len = 2; i >= 0; --i, ++len)
		str[len] = buf[i];

	str[len] = '\0';
}

void uint_to_str(u32 val, u8 *str)
{
	u8 buf[10] = "";
	s32 i = 0;
	s32 len = 0;

	if (val == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	for (i = 0; val > 0; ++i) {
		buf[i] = (val % 10) + '0';
		val /= 10;
	}

	for (--i, len = 0; i >= 0; --i, ++len)
		str[len] = buf[i];

	str[len] = '\0';
}

void int_to_str(s32 val, u8 *str)
{
	u8 buf[10] = "";
	s32 i;
	s32 len = 0;
	s32 tmp = val < 0 ? -val : val;

	if (val == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	for (i = 0; tmp > 0; ++i) {
		buf[i] = (tmp % 10) + '0';
		tmp /= 10;
	}

	if (val < 0)
		str[len++] = '-';

	for (i = i - 1; i >= 0; --i, ++len)
		str[len] = buf[i];

	str[len] = '\0';
}

#ifndef CODE_SIZE_OPT
/**
 * double_to_str - convert double data to string
 * @val: the input double value
 * @str: the ouput string
 * @decimal_bits: the bit count of the fraction part
 *
 */
void double_to_str(double val, u8 *str, u8 decimal_bits)
{
	unsigned int transbit = 0; /* bits that have transfered to str */
	unsigned int bits = 8; /* bits to transfered */
	/* judge a double value whether is 0 when it's less than threshhold */
	double threshold[8] = {1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001,
				0.000001, 0.0000001};
	unsigned int integer;
	int i;

	/* support 8bits decimal max */
	if (decimal_bits < bits)
		bits = decimal_bits;

	/* handle 0.0 */
	if (val == 0.0) {
		str[0] = '0';
		str[1] = '.';
		str[2] = '0';
		str[3] = '\0';
		return;
	}

	/* handle negative sign */
	if (val < 0.0) {
		*str++ = '-';
		val = -val;
	}

	/* handle integer */
	integer = (unsigned int)val;
	val = val - integer;

	if (integer == 0)
		*str++ = '0';
	else {
		u8 buf[16] = "";
		u32 len;
		for (i = 0; integer > 0; ++i) {
			buf[i] = integer % 10 + '0';
			integer /= 10;
		}

		for (--i, len = 0; i >= 0; --i, ++len)
			*str++ = buf[i];
	}
	*str++ = '.';

	/* handle decimal fraction */
	if (val < threshold[bits])
		*str++ = '0';
	else {
		while (transbit < bits) {
			int digit = val * 10.0;
			*str++ = digit + '0';
			val = val * 10.0 - digit;
			transbit++;
		}
	}

	*str++ = '\0';
}
#endif

void uart_putc(u8 c)
{
	while (!(read_reg(UART_FSR) & (0x1 << 16)))
		;

	write_reg(c, UART_THR);
}

void uart_puts(u8 *str)
{
	while (*str != '\0') {
		if (*str == '\n')
			uart_putc('\r');

		uart_putc(*str++);
	}
}

int dramfreq_printk(u8 *str, ...)
{
	u8 str_fmt[16] = "";
	u8 *p_str;

	va_list para_list;
	va_start(para_list, str);

	while (*str) {
		if (*str == '%') { /* parse the value */
			++str;
			p_str = str_fmt;
			switch (*str) {
			case 'x':
			case 'X':
				int_to_hex_str(va_arg(para_list, s32),
					str_fmt, 1);
				uart_puts(p_str);
				++str;
				break;
			case 'u':
#ifndef CODE_SIZE_OPT
				uint_to_str(va_arg(para_list, s32), str_fmt);
				uart_puts(p_str);
				++str;
				break;
#endif
			case 'd':
				int_to_str(va_arg(para_list, s32), str_fmt);
				uart_puts(p_str);
				++str;
				break;
			case 's':
				uart_puts(va_arg(para_list, u8 *));
				++str;
				break;
			case 'c':
				uart_putc(va_arg(para_list, s32));
				++str;
				break;
			case 'f':
#ifndef CODE_SIZE_OPT
				double_to_str(va_arg(para_list, double),
					str_fmt, 4);
				uart_puts(p_str);
				++str;
				break;
#endif
			default:
				uart_putc('%');
				uart_putc(*str);
				++str;
			}
		}	else {
			if ('\n' == *str)
				uart_putc('\r');
			uart_putc(*str++);
		}
	}

	va_end(para_list);
}
#endif /* __SRAM_DRAMFREQ_DEBUG */

