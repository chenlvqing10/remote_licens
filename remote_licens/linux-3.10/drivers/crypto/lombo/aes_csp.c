/*
 * aes_csp.c - the chip operations for aes
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

#include "aes_csp.h"

/* struct for converting big-little endian */
typedef union {
	u32 val;
	struct {
	u32 val0:8;    /*0~7bit*/
	u32 val1:8;    /*8~15bit*/
	u32 val2:8;    /*15~23bit*/
	u32 val3:8;    /*23~31bit*/
	} bits;
} reg_val_t;

u32 csp_aes_get_ver(void *base)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ver_t reg;

	reg.val = READREG32(&aes->ver);
	return reg.val;
}

/*
 * csp_aes_setkey0 - set key0~3
 * @key: key introduced by host
 *
 */
void csp_aes_setkey0(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key0_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key3 = p_key.bits.val0;
	reg.bits.key2 = p_key.bits.val1;
	reg.bits.key1 = p_key.bits.val2;
	reg.bits.key0 = p_key.bits.val3;

	WRITEREG32(&aes->key0, reg.val);
}

/*
 * csp_aes_setkey1 - set key4~7
 * @key: key introduced by host
 *
 */
void csp_aes_setkey1(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key1_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key7 = p_key.bits.val0;
	reg.bits.key6 = p_key.bits.val1;
	reg.bits.key5 = p_key.bits.val2;
	reg.bits.key4 = p_key.bits.val3;

	WRITEREG32(&aes->key1, reg.val);
}

/*
 * csp_aes_setkey2 - set key8~11
 * @key: key introduced by host
 *
 */
void csp_aes_setkey2(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key2_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key11 = p_key.bits.val0;
	reg.bits.key10 = p_key.bits.val1;
	reg.bits.key9 = p_key.bits.val2;
	reg.bits.key8 = p_key.bits.val3;

	WRITEREG32(&aes->key2, reg.val);
}

/*
 * csp_aes_setkey3 - set key12~15
 * @key: key introduced by host
 *
 */
void csp_aes_setkey3(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key3_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key15 = p_key.bits.val0;
	reg.bits.key14 = p_key.bits.val1;
	reg.bits.key13 = p_key.bits.val2;
	reg.bits.key12 = p_key.bits.val3;

	WRITEREG32(&aes->key3, reg.val);
}

/*
 * csp_aes_setkey4 - set key16~19
 * @key: key introduced by host
 *
 */
void csp_aes_setkey4(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key4_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key19 = p_key.bits.val0;
	reg.bits.key18 = p_key.bits.val1;
	reg.bits.key17 = p_key.bits.val2;
	reg.bits.key16 = p_key.bits.val3;

	WRITEREG32(&aes->key4, reg.val);
}

/*
 * csp_aes_setkey5 - set key20~23
 * @key: key introduced by host
 *
 */
void csp_aes_setkey5(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key5_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key23 = p_key.bits.val0;
	reg.bits.key22 = p_key.bits.val1;
	reg.bits.key21 = p_key.bits.val2;
	reg.bits.key20 = p_key.bits.val3;

	WRITEREG32(&aes->key5, reg.val);
}

/*
 * csp_aes_setkey6 - set key24~27
 * @key: key introduced by host
 *
 */
void csp_aes_setkey6(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key6_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key27 = p_key.bits.val0;
	reg.bits.key26 = p_key.bits.val1;
	reg.bits.key25 = p_key.bits.val2;
	reg.bits.key24 = p_key.bits.val3;

	WRITEREG32(&aes->key6, reg.val);
}

/*
 * csp_aes_setkey7 - set key28~31
 * @key: key introduced by host
 *
 */
void csp_aes_setkey7(void *base, void *key)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_key;
	reg_aes_key7_t reg;

	reg.val = 0;
	p_key.val = *(u32 *)key;

	reg.bits.key31 = p_key.bits.val0;
	reg.bits.key30 = p_key.bits.val1;
	reg.bits.key29 = p_key.bits.val2;
	reg.bits.key28 = p_key.bits.val3;

	WRITEREG32(&aes->key7, reg.val);
}

/*
 * csp_aes_setiv0 - set iv0~3
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv0(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv0_t reg;

	reg.val = 0;
	p_iv.val = *(u32 *)iv;

	reg.bits.iv3 = p_iv.bits.val0;
	reg.bits.iv2 = p_iv.bits.val1;
	reg.bits.iv1 = p_iv.bits.val2;
	reg.bits.iv0 = p_iv.bits.val3;

	WRITEREG32(&aes->iv0, reg.val);
}

/*
 * csp_aes_getiv0 - get iv0~3
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv0(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv0_t reg;

	reg.val = READREG32(&aes->iv0);
	p_iv.bits.val0 = reg.bits.iv3;
	p_iv.bits.val1 = reg.bits.iv2;
	p_iv.bits.val2 = reg.bits.iv1;
	p_iv.bits.val3 = reg.bits.iv0;

	*(u32 *)iv = p_iv.val;
}

/*
 * csp_aes_setiv1 - set iv4~7
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv1(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv1_t reg;

	reg.val = 0;
	p_iv.val = *(u32 *)iv;

	reg.bits.iv7 = p_iv.bits.val0;
	reg.bits.iv6 = p_iv.bits.val1;
	reg.bits.iv5 = p_iv.bits.val2;
	reg.bits.iv4 = p_iv.bits.val3;

	WRITEREG32(&aes->iv1, reg.val);
}

/*
 * csp_aes_getiv1 - get iv4~7
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv1(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv1_t reg;

	reg.val = READREG32(&aes->iv1);
	p_iv.bits.val0 = reg.bits.iv7;
	p_iv.bits.val1 = reg.bits.iv6;
	p_iv.bits.val2 = reg.bits.iv5;
	p_iv.bits.val3 = reg.bits.iv4;

	*(u32 *)iv = p_iv.val;
}

/*
 * csp_aes_setiv2 - set iv8~11
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv2(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv2_t reg;

	reg.val = 0;
	p_iv.val = *(u32 *)iv;

	reg.bits.iv11 = p_iv.bits.val0;
	reg.bits.iv10 = p_iv.bits.val1;
	reg.bits.iv9 = p_iv.bits.val2;
	reg.bits.iv8 = p_iv.bits.val3;

	WRITEREG32(&aes->iv2, reg.val);
}

/*
 * csp_aes_getiv2 - get iv8~11
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv2(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv2_t reg;

	reg.val = READREG32(&aes->iv2);
	p_iv.bits.val0 = reg.bits.iv11;
	p_iv.bits.val1 = reg.bits.iv10;
	p_iv.bits.val2 = reg.bits.iv9;
	p_iv.bits.val3 = reg.bits.iv8;

	*(u32 *)iv = p_iv.val;
}

/*
 * csp_aes_setiv3 - set iv12~15
 * @iv: iv introduced by host
 *
 */
void csp_aes_setiv3(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv3_t reg;

	reg.val = 0;
	p_iv.val = *(u32 *)iv;

	reg.bits.iv15 = p_iv.bits.val0;
	reg.bits.iv14 = p_iv.bits.val1;
	reg.bits.iv13 = p_iv.bits.val2;
	reg.bits.iv12 = p_iv.bits.val3;

	WRITEREG32(&aes->iv3, reg.val);
}

/*
 * csp_aes_getiv3 - get iv12~15
 * @iv: iv introduced by host
 *
 */
void csp_aes_getiv3(void *base, void *iv)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_iv;
	reg_aes_iv3_t reg;

	reg.val = READREG32(&aes->iv3);
	p_iv.bits.val0 = reg.bits.iv15;
	p_iv.bits.val1 = reg.bits.iv14;
	p_iv.bits.val2 = reg.bits.iv13;
	p_iv.bits.val3 = reg.bits.iv12;

	*(u32 *)iv = p_iv.val;
}

/*
 * csp_aes_setctr0 - set counter0~3
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr0(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr0_t reg;

	reg.val = 0;
	p_ctr.val = *(u32 *)ctr;

	reg.bits.ctr3 = p_ctr.bits.val0;
	reg.bits.ctr2 = p_ctr.bits.val1;
	reg.bits.ctr1 = p_ctr.bits.val2;
	reg.bits.ctr0 = p_ctr.bits.val3;

	WRITEREG32(&aes->ctr0, reg.val);
}

/*
 * csp_aes_getctr0 - get counter0~3
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr0(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr0_t reg;

	reg.val = READREG32(&aes->ctr0);
	p_ctr.bits.val0 = reg.bits.ctr3;
	p_ctr.bits.val1 = reg.bits.ctr2;
	p_ctr.bits.val2 = reg.bits.ctr1;
	p_ctr.bits.val3 = reg.bits.ctr0;

	*(u32 *)ctr = p_ctr.val;
}

/*
 * csp_aes_setctr1 - set counter4~7
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr1(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr1_t reg;

	reg.val = 0;
	p_ctr.val = *(u32 *)ctr;

	reg.bits.ctr7 = p_ctr.bits.val0;
	reg.bits.ctr6 = p_ctr.bits.val1;
	reg.bits.ctr5 = p_ctr.bits.val2;
	reg.bits.ctr4 = p_ctr.bits.val3;

	WRITEREG32(&aes->ctr1, reg.val);
}

/*
 * csp_aes_getctr1 - get counter4~7
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr1(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr1_t reg;

	reg.val = READREG32(&aes->ctr1);
	p_ctr.bits.val0 = reg.bits.ctr7;
	p_ctr.bits.val1 = reg.bits.ctr6;
	p_ctr.bits.val2 = reg.bits.ctr5;
	p_ctr.bits.val3 = reg.bits.ctr4;

	*(u32 *)ctr = p_ctr.val;
}

/*
 * csp_aes_setctr2 - set counter8~11
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr2(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr2_t reg;

	reg.val = 0;
	p_ctr.val = *(u32 *)ctr;

	reg.bits.ctr11 = p_ctr.bits.val0;
	reg.bits.ctr10 = p_ctr.bits.val1;
	reg.bits.ctr9 = p_ctr.bits.val2;
	reg.bits.ctr8 = p_ctr.bits.val3;

	WRITEREG32(&aes->ctr2, reg.val);
}

/*
 * csp_aes_getctr2 - get counter8~11
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr2(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr2_t reg;

	reg.val = READREG32(&aes->ctr2);
	p_ctr.bits.val0 = reg.bits.ctr11;
	p_ctr.bits.val1 = reg.bits.ctr10;
	p_ctr.bits.val2 = reg.bits.ctr9;
	p_ctr.bits.val3 = reg.bits.ctr8;

	*(u32 *)ctr = p_ctr.val;
}

/*
 * csp_aes_setctr3 - set counter12~15
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_setctr3(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr3_t reg;

	reg.val = 0;
	p_ctr.val = *(u32 *)ctr;

	reg.bits.ctr15 = p_ctr.bits.val0;
	reg.bits.ctr14 = p_ctr.bits.val1;
	reg.bits.ctr13 = p_ctr.bits.val2;
	reg.bits.ctr12 = p_ctr.bits.val3;

	WRITEREG32(&aes->ctr3, reg.val);
}

/*
 * csp_aes_getctr3 - get counter12~15
 * @ctr: counter value introduced by host
 *
 */
void csp_aes_getctr3(void *base, void *ctr)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_ctr;
	reg_aes_ctr3_t reg;

	reg.val = READREG32(&aes->ctr3);
	p_ctr.bits.val0 = reg.bits.ctr15;
	p_ctr.bits.val1 = reg.bits.ctr14;
	p_ctr.bits.val2 = reg.bits.ctr13;
	p_ctr.bits.val3 = reg.bits.ctr12;

	*(u32 *)ctr = p_ctr.val;
}

/*
 * csp_aes_sw_rst - reset AES module by software
 * @en: 1, enable; 0, disable
 *
 */
void csp_aes_sw_rst(void *base, u32 en)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	reg.bits.sw_rst = en;

	WRITEREG32(&aes->ctrl, reg.val);
}

/*
 * csp_aes_set_key_src - select AES key source
 * @key_src: 1, reg; 0, efuse
 *
 */
void csp_aes_set_key_src(void *base, u32 src)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	reg.bits.key_src = src;

	WRITEREG32(&aes->ctrl, reg.val);
}

/*
 * csp_aes_key_src - start or stop AES module
 * @status: 1, start; 0, end
 *
 */
void csp_aes_start(void *base, u32 status)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	reg.bits.start = status;

	WRITEREG32(&aes->ctrl, reg.val);
}

/*
 * csp_aes_key_src - start or stop AES module
 * @status: 1, start; 0, end
 *
 */
int csp_aes_get_start(void *base)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	return reg.bits.start;
}

/*
 * csp_aes_set_key_length - set the length of AES key
 * @key_len: 0x0, 128bit;
 *	 0x1, 192bit;
 *	 0x2, 256bit;
 *
 */
void csp_aes_set_key_length(void *base, u32 key_len)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	reg.bits.length = key_len;

	WRITEREG32(&aes->ctrl, reg.val);
}

/*
 * csp_aes_set_mode - set the length of AES key
 * @mode: 0x0, ECB;
 *	  0x1, CBC;
 *	  0x2, CTR;
 *	  0x3, CBC_MAC;
 *
 */
void csp_aes_set_mode(void *base, u32 mode)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	reg.bits.mode = mode;

	WRITEREG32(&aes->ctrl, reg.val);
}

/*
 * csp_aes_set_crypto - set decrypt or encrypt
 * @crypto: 1, decrypt; 0, encrypt
 *
 */
void csp_aes_set_crypto_ed(void *base, u32 crypto)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_ctrl_t reg;

	reg.val = READREG32(&aes->ctrl);
	reg.bits.ed = crypto;

	WRITEREG32(&aes->ctrl, reg.val);
}

/*
 * csp_aes_data_output_en - enable aes data output
 * @en: 1, enable; 0, didable
 *
 */
void csp_aes_data_output_en(void *base, u32 en)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_mask_t reg;

	reg.val = READREG32(&aes->mask);
	reg.bits.ordy = en;

	WRITEREG32(&aes->mask, reg.val);
}

/*
 * csp_aes_get_status - get the status of aes, whether it is busy
 * @no para
 *
 * return: 1, busy; 0, idle
 */
u32 csp_aes_get_status(void *base)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_sts_t reg;

	reg.val = READREG32(&aes->sts);

	return reg.bits.busy;
}

/*
 * csp_aes_data_out_ready - whether the data output is ready
 * @no para
 *
 * return: 1, ready; 0, idle
 */
u32 csp_aes_data_out_ready(void *base)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_aes_sts_t reg;

	reg.val = READREG32(&aes->sts);

	return reg.bits.ordy;
}


/*
 * csp_aes_input0 - aes input0~3
 * @input: aes data input
 *
 */
void csp_aes_input0(void *base, void *input)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_in;
	reg_aes_input0_t reg;

	reg.val = 0;
	p_in.val = *(u32 *)input;

	reg.bits.in3 = p_in.bits.val0;
	reg.bits.in2 = p_in.bits.val1;
	reg.bits.in1 = p_in.bits.val2;
	reg.bits.in0 = p_in.bits.val3;

	WRITEREG32(&aes->input0, reg.val);
}

/*
 * csp_aes_input1 - aes input4~7
 * @input: aes data input
 *
 */
void csp_aes_input1(void *base, void *input)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_in;
	reg_aes_input1_t reg;

	reg.val = 0;
	p_in.val = *(u32 *)input;

	reg.bits.in7 = p_in.bits.val0;
	reg.bits.in6 = p_in.bits.val1;
	reg.bits.in5 = p_in.bits.val2;
	reg.bits.in4 = p_in.bits.val3;

	WRITEREG32(&aes->input1, reg.val);
}

/*
 * csp_aes_input2 - aes input8~11
 * @input: aes data input
 *
 */
void csp_aes_input2(void *base, void *input)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_in;
	reg_aes_input2_t reg;

	reg.val = 0;
	p_in.val = *(u32 *)input;

	reg.bits.in11 = p_in.bits.val0;
	reg.bits.in10 = p_in.bits.val1;
	reg.bits.in9 = p_in.bits.val2;
	reg.bits.in8 = p_in.bits.val3;

	WRITEREG32(&aes->input2, reg.val);
}

/*
 * csp_aes_input3 - aes input12~15
 * @input: aes data input
 *
 */
void csp_aes_input3(void *base, void *input)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_in;
	reg_aes_input3_t reg;

	reg.val = 0;
	p_in.val = *(u32 *)input;

	reg.bits.in15 = p_in.bits.val0;
	reg.bits.in14 = p_in.bits.val1;
	reg.bits.in13 = p_in.bits.val2;
	reg.bits.in12 = p_in.bits.val3;

	WRITEREG32(&aes->input3, reg.val);
}

/*
 * csp_aes_output0 - aes output0~3
 * @output: aes data output
 *
 */
void csp_aes_output0(void *base, void *output)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_out;
	reg_aes_output0_t reg;

	reg.val = READREG32(&aes->output0);

	p_out.bits.val0 = reg.bits.out3;
	p_out.bits.val1 = reg.bits.out2;
	p_out.bits.val2 = reg.bits.out1;
	p_out.bits.val3 = reg.bits.out0;

	*(u32 *)output = p_out.val;
}

/*
 * csp_aes_output1 - aes output4~7
 * @output: aes data output
 *
 */
void csp_aes_output1(void *base, void *output)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_out;
	reg_aes_output1_t reg;

	reg.val = READREG32(&aes->output1);

	p_out.bits.val0 = reg.bits.out7;
	p_out.bits.val1 = reg.bits.out6;
	p_out.bits.val2 = reg.bits.out5;
	p_out.bits.val3 = reg.bits.out4;

	*(u32 *)output = p_out.val;
}

/*
 * csp_aes_output2 - aes output8~11
 * @output: aes data output
 *
 */
void csp_aes_output2(void *base, void *output)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_out;
	reg_aes_output2_t reg;

	reg.val = READREG32(&aes->output2);

	p_out.bits.val0 = reg.bits.out11;
	p_out.bits.val1 = reg.bits.out10;
	p_out.bits.val2 = reg.bits.out9;
	p_out.bits.val3 = reg.bits.out8;

	*(u32 *)output = p_out.val;
}

/*
 * csp_aes_output3 - aes output12~15
 * @output: aes data output
 *
 */
void csp_aes_output3(void *base, void *output)
{
	reg_aes_t *aes = (reg_aes_t *)base;
	reg_val_t p_out;
	reg_aes_output3_t reg;

	reg.val = READREG32(&aes->output3);

	p_out.bits.val0 = reg.bits.out15;
	p_out.bits.val1 = reg.bits.out14;
	p_out.bits.val2 = reg.bits.out13;
	p_out.bits.val3 = reg.bits.out12;

	*(u32 *)output = p_out.val;
}
