/*
 * char_conv.h - Interface for character or string operation and conversion.
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

#define _XOPEN_SOURCE	    /* See feature_test_macros(7) */
#include <time.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LOG_TAG "XLS"
#include "log/log.h"
#include "char_conv.h"

#define OUTLEN 255

/**
 * code_convert - Convert characters from one encoding to another.
 * @from_charset: String encoding.
 * @to_charset: String encoding that needs to be converted.
 * @inbuf: IN, Pointer to the input string.
 * @inbuf: IN, Input string length.
 * @outbuf: OUT, Output string pointer after conversion.
 * @outlen OUT, Output string length after conversion.
 *
 * Returns 0 if success, otherwise not 0.
 */
static int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen,
		char *outbuf, int outlen)
{
	size_t n;
	iconv_t cd;

	cd = iconv_open(to_charset, from_charset);
	if ((iconv_t)(-1) == cd) {
		ALOGE("iconv_open fail: %s\n", strerror(errno));
		return -1;
	}

	n = iconv(cd, &inbuf, (unsigned int *)&inlen, &outbuf, (unsigned int *)&outlen);
	if ((size_t)(-1) == n) {
		ALOGE("iconve fail: %s\n", strerror(errno));
		iconv_close(cd);
		return -1;
	}

	iconv_close(cd);

	return 0;
}

/**
 * utf8_to_gb2312 - Convert characters from UTF-8 encoding to GB2312.
 * @inbuf: IN, Pointer to the input string.
 * @inbuf: IN, Input string length.
 * @outbuf: OUT, Output string pointer after conversion.
 * @outlen OUT, Output string length after conversion.
 *
 * Returns 0 if success, otherwise not 0.
 */
int utf8_to_gb2312(char *inbuf, int inlen, char *outbuf, int outlen)
{
#if 0
	return code_convert("UTF-8", "UCS-2", inbuf, inlen,
			outbuf, outlen);
#else
	return code_convert("UTF-8//IGNORE", "GB2312//IGNORE", inbuf, inlen,
			outbuf, outlen);
#endif
}

/**
 * gb2312_to_utf8 - Convert characters from GB2312 encoding to UTF-8.
 * @inbuf: IN, Pointer to the input string.
 * @inbuf: IN, Input string length.
 * @outbuf: OUT, Output string pointer after conversion.
 * @outlen OUT, Output string length after conversion.
 *
 * Returns 0 if success, otherwise not 0.
 */
int gb2312_to_utf8(char *inbuf, int inlen, char *outbuf, int outlen)
{
	return code_convert("GB2312//IGNORE", "UTF-8//IGNORE", inbuf, inlen,
			outbuf, outlen);
}

int convert_test()
{
	char *in_utf8 = "utf8";
	char *in_gb2312 = "\xbe\xb2\xcc\xac\xc4\xa3\xca\xbd";
	char out[OUTLEN];
	int ret;

	/* Unicode to GB2312 */
	ret = utf8_to_gb2312(in_utf8, strlen(in_utf8), out, OUTLEN);
	if (ret == 0)
		ALOGD("unicode-->gb2312 out=%s\n", out);

	/* GB2312 to Unicode */
	ret = gb2312_to_utf8(in_gb2312, strlen(in_gb2312), out, OUTLEN);
	if (ret == 0)
		ALOGD("gb2312-->unicode out=%s\n", out);

	return 0;
}

/**
 * string_replace - Replace a specified string with a new string in a string.
 * @orig Original string
 * @rep String to be replaced
 * @with New string replaced
 *
 * NOTE: You must free the result if result is non-NULL.
 *
 * Returns the replaced string pointer, otherwise return to NULL.
 */
char *string_replace(char *orig, char *rep, char *with)
{
	char *result; /* the return string */
	char *ins;    /* the next insert point */
	char *tmp;    /* varies */
	int len_rep;  /* length of rep (the string to remove) */
	int len_with; /* length of with (the string to replace rep with) */
	int len_front; /* distance between rep and end of last rep */
	int count;    /* number of replacements */

	/* sanity checks and initialization */
	if (!orig || !rep)
		return NULL;

	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; /* empty rep causes infinite loop during count */

	if (!with)
		with = "";

	len_with = strlen(with);

	/* count the number of replacements needed */
	ins = orig;
	for (count = 0; ((tmp = strstr(ins, rep)) != NULL); ++count)
		ins = tmp + len_rep;

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	/* first time through the loop, all the variable are set correctly
	 * from here on,
	 *	tmp points to the end of the result string
	 *	ins points to the next occurrence of rep in orig
	 *	orig points to the remainder of orig after "end of rep"
	 */
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; /* move to next "end of rep" */
	}
	strcpy(tmp, orig);

	return result;
}

