/*
 * sha1.c - sha1
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "sha1.h"

#define SHA1_ROTL(a,b) (sha1_tmp=(a),((sha1_tmp>>(32-b))&(0x7fffffff>>(31-b)))|(sha1_tmp<<b))
#define SHA1_F(B,C,D,t) ((t<40)?((t<20)?((B&C)|((~B)&D)):(B^C^D)):((t<60)?((B&C)|(B&D)|(C&D)):(B^C^D)))

long sha1_tmp;

/* get_sha1_str: calcuating string sha1
 * @str: source string
 * @length: length of str
 * @sha1: destination string
 *
 * Return: 0 if success, -1 if failure.
*/
int get_sha1_str(const char* str, long long length, char* sha1)
{
	char *pp, *ppend;
	long l, i, k[80], w[80], temp;
	long a, b, c, d, e, h0, h1, h2, h3, h4;

	h0 = 0x67452301;
	h1 = 0xEFCDAB89;
	h2 = 0x98BADCFE;
	h3 = 0x10325476;
	h4 = 0xC3D2E1F0;

	for (i = 0; i < 20; k[i++] = 0x5A827999);
	for (i = 20; i < 40; k[i++] = 0x6ED9EBA1);
	for (i = 40; i < 60; k[i++] = 0x8F1BBCDC);
	for (i = 60; i < 80; k[i++] = 0xCA62C1D6);

	l = length + ((length % 64 > 56) ? 
		(128 - length % 64) : (64 - length % 64)) ;
	if (!(pp = (char*)malloc((unsigned long)l)))
		return -1;

	for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++) ;

	for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l;
		pp[i + 3 - 2 * (i % 4)] = 0, i++) ;

	*((long*)(pp + l - 4)) = length << 3;
	*((long*)(pp + l - 8)) = length >> 29;
	for (ppend = pp + l; pp < ppend; pp += 64) {
		for (i = 0; i < 16; w[i] = ((long*)pp)[i], i++);

		for (i = 16; i < 80; w[i] =
			SHA1_ROTL((w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]),
				1), i++);

		a = h0, b = h1, c = h2, d = h3, e = h4;
		for (i = 0; i < 80; i++) {
			temp = SHA1_ROTL(a, 5) +
				SHA1_F(b, c, d, i) + e + w[i] + k[i];
			e = d, d = c, c = SHA1_ROTL(b, 30), b = a, a = temp;
		}
		h0 += a, h1 += b, h2 += c, h3 += d, h4 += e;
	}

	free(pp - l);
	sprintf(sha1, "%08x%08x%08x%08x%08x", h0, h1, h2, h3, h4);

	return 0;
}
