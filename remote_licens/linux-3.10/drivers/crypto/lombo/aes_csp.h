/*
 * aes_csp.h - the chip operations file for AES
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

#ifndef __LOMBO_AES_CSP_H__
#define __LOMBO_AES_CSP_H__

#include <mach/csp.h>

#define AES_MODE_ECB	0x00
#define AES_MODE_CBC	0x01
#define AES_MODE_CTR	0x02
#define AES_MODE_CBC_MAC 0x03

#define AES_KEY_128BIT	0x00
#define AES_KEY_192BIT	0x01
#define AES_KEY_256BIT	0x02

#define AES_CRYPTO_ENCRYPT	0
#define AES_CRYPTO_DECRYPT	1

#define AES_KEY_SRC_EFUSE	0
#define AES_KEY_SRC_REG		1

#define ENABLE	1
#define DISABLE	0

/*
 * csp_aes_key0 - set key0~3
 * @key: key introduced by host
 *
 */
void csp_aes_setkey0(void *base, void *key);

/*
 * csp_aes_key1 - set key4~7
 * @key: key introduced by host
 *
 */
void csp_aes_setkey1(void *base, void *key);

/*
 * csp_aes_key2 - set key8~11
 * @key: key introduced by host
 *
 */
void csp_aes_setkey2(void *base, void *key);

/*
 * csp_aes_key3 - set key12~15
 * @key: key introduced by host
 *
 */
void csp_aes_setkey3(void *base, void *key);

/*
 * csp_aes_key4 - set key16~19
 * @key: key introduced by host
 *
 */
void csp_aes_setkey4(void *base, void *key);

/*
 * csp_aes_key5 - set key20~23
 * @key: key introduced by host
 *
 */
void csp_aes_setkey5(void *base, void *key);

/*
 * csp_aes_key6 - set key24~27
 * @key: key introduced by host
 *
 */
void csp_aes_setkey6(void *base, void *key);

/*
 * csp_aes_key7 - set key28~31
 * @key: key introduced by host
 *
 */
void csp_aes_setkey7(void *base, void *key);

/*
 * csp_aes_iv0 - set iv0~3
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv0(void *base, void *iv);

/*
 * csp_aes_getiv0 - get iv0~3
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv0(void *base, void *iv);

/*
 * csp_aes_iv1 - set iv4~7
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv1(void *base, void *iv);

/*
 * csp_aes_getiv1 - get iv4~7
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv1(void *base, void *iv);

/*
 * csp_aes_iv2 - set iv8~11
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv2(void *base, void *iv);

/*
 * csp_aes_getiv2 - get iv8~11
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv2(void *base, void *iv);

/*
 * csp_aes_iv3 - set iv12~15
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv3(void *base, void *iv);

/*
 * csp_aes_getiv3 - get iv12~15
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv3(void *base, void *iv);

/*
 * csp_aes_ctr0 - set counter0~3
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr0(void *base, void *ctr);

/*
 * csp_aes_getctr0 - get counter0~3
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr0(void *base, void *ctr);

/*
 * csp_aes_ctr1 - set counter4~7
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr1(void *base, void *ctr);

/*
 * csp_aes_getctr1 - get counter4~7
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr1(void *base, void *ctr);

/*
 * csp_aes_ctr2 - set counter8~11
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr2(void *base, void *ctr);

/*
 * csp_aes_getctr2 - get counter8~11
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr2(void *base, void *ctr);

/*
 * csp_aes_ctr3 - set counter12~15
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr3(void *base, void *ctr);

/*
 * csp_aes_getctr3 - get counter12~15
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr3(void *base, void *ctr);

/*
 * csp_aes_sw_rst - reset AES module by software
 * @en: 1, enable; 0, disable
 *
 */
void csp_aes_sw_rst(void *base, u32 en);

/*
 * csp_aes_set_key_src - select AES key source
 * @key_src: 1, reg; 0, efuse
 *
 */
void csp_aes_set_key_src(void *base, u32 src);

/*
 * csp_aes_key_src - start or stop AES module
 * @status: 1, start; 0, end
 *
 */
void csp_aes_start(void *base, u32 status);

/*
 * csp_aes_key_src - start or stop AES module
 * @status: 1, start; 0, end
 *
 */
int csp_aes_get_start(void *base);

/*
 * csp_aes_set_key_length - set the length of AES key
 * @key_len: 0x0, 128bit;
 *	 0x1, 192bit;
 *	 0x2, 256bit;
 *
 */
void csp_aes_set_key_length(void *base, u32 key_len);

/*
 * csp_aes_set_mode - set the length of AES key
 * @mode: 0x0, ECB;
 *	  0x1, CBC;
 *	  0x2, CTR;
 *	  0x3, CBC_MAC;
 *
 */
void csp_aes_set_mode(void *base, u32 mode);

/*
 * csp_aes_set_crypto - set decrypt or encrypt
 * @crypto: 1, decrypt; 0, encrypt
 *
 */
void csp_aes_set_crypto_ed(void *base, u32 crypto);

/*
 * csp_aes_data_output_en - enable aes data output
 * @en: 1, enable; 0, didable
 *
 */
void csp_aes_data_output_en(void *base, u32 en);

/*
 * csp_aes_get_status - get the status of aes, whether it is busy
 * @no para
 *
 * return: 1, busy; 0, idle
 */
u32 csp_aes_get_status(void *base);

/*
 * csp_aes_data_out_ready - whether the data output is ready
 * @no para
 *
 * return: 1, ready; 0, idle
 */
u32 csp_aes_data_out_ready(void *base);

/*
 * csp_aes_input0 - aes input0~3
 * @input: aes data input
 *
 */
void csp_aes_input0(void *base, void *input);

/*
 * csp_aes_input1 - aes input4~7
 * @input: aes data input
 *
 */
void csp_aes_input1(void *base, void *input);

/*
 * csp_aes_input2 - aes input8~11
 * @input: aes data input
 *
 */
void csp_aes_input2(void *base, void *input);

/*
 * csp_aes_input3 - aes input12~15
 * @input: aes data input
 *
 */
void csp_aes_input3(void *base, void *input);

/*
 * csp_aes_output0 - aes output0~3
 * @output: aes data output
 *
 */
void csp_aes_output0(void *base, void *output);

/*
 * csp_aes_output1 - aes output4~7
 * @output: aes data output
 *
 */
void csp_aes_output1(void *base, void *output);

/*
 * csp_aes_output2 - aes output8~11
 * @output: aes data output
 *
 */
void csp_aes_output2(void *base, void *output);

/*
 * csp_aes_output3 - aes output12~15
 * @output: aes data output
 *
 */
void csp_aes_output3(void *base, void *output);
#endif
