/*
 * aes_test.c - Driver test of lomboTech AES
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
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <mach/csp.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <mach/debug.h>
#include "aes.h"

/* #define DEBUG_AES */
#ifndef DEBUG_AES
#undef PRT_INFO
#define PRT_INFO(fmt, ...)
#endif

#define AES_TEXT_BLOCK_SIZE	16

struct lombo_aes_op {
	u8 *src;
	u8 *dst;

	u8 iv[16];
	u8 ctr[16];
	u8 key[32];
	u32 mode;
	u32 crypto_ed;
	u32 key_len;
	u32 key_src;
	int ret;

	void __iomem *base;
};
struct lombo_aes_op lombo_aes;

u8 *code = "Hello,I'm Lombo1";
/*u8 code[16] ={0x34,0x07,0x37,0xe0,0xa2,0x98,0x31,
	0x31,0x8d,0x30,0x5a,0x88,0xa8,0xf6,0x43,0x32};*/


u8 *key_128bit = "yvhnk53zu59a384ef";
u8 *key_192bit = "84jd33fr5gjs5saeffhebbcc";
u8 *key_256bit = "24fg8cj49jhssj8ffgfgh5ghdrty403z";
/*u8 key_128bit[16] = {0x3c,0x4f,0xcf,0x09,0x88,0x15,0xf7,0xab,
		0xa6,0xd2,0xae,0x28,0x16,0x15,0x7e,0x2b};*/

u8 *iv = "tyjg84jf89abxa6k";
u8 *ctr = "1234567899876543";

static int __aes_set_iv(void *base, void *iv)
{
	u32 *p_iv = (u32 *)iv;
	int ret = 0;

	if ((base == 0) || (iv == NULL)) {
		PRT_ERR("base = %x, iv = %x\n", (u32)base, (u32)iv);
		ret = __LINE__;
	}
	csp_aes_setiv3(base, p_iv);
	csp_aes_setiv2(base, p_iv + 1);
	csp_aes_setiv1(base, p_iv + 2);
	csp_aes_setiv0(base, p_iv + 3);

	return ret;
}

static int __aes_get_iv(void *base, void *iv)
{
	u32 *p_iv = (u32 *)iv;
	int ret = 0;

	if ((base == 0) || (iv == NULL)) {
		PRT_ERR("base = %x, iv = %x\n", (u32)base, (u32)iv);
		ret = __LINE__;
	}

	csp_aes_getiv3(base, p_iv);
	csp_aes_getiv2(base, p_iv + 1);
	csp_aes_getiv1(base, p_iv + 2);
	csp_aes_getiv0(base, p_iv + 3);

	return ret;
}

static int __aes_set_ctr(void *base, void *ctr)
{
	u32 *p_ctr = (u32 *)ctr;
	int ret = 0;

	if ((base == 0) || (ctr == NULL)) {
		PRT_ERR("base = %x, ctr = %x\n", (u32)base, (u32)ctr);
		ret = __LINE__;
	}

	csp_aes_setctr3(base, p_ctr);
	csp_aes_setctr2(base, p_ctr + 1);
	csp_aes_setctr1(base, p_ctr + 2);
	csp_aes_setctr0(base, p_ctr + 3);

	return ret;
}

static int __aes_get_ctr(void *base, void *ctr)
{
	u32 *p_ctr = (u32 *)ctr;
	int ret = 0;

	if ((base == 0) || (ctr == NULL)) {
		PRT_ERR("base = %x, ctr = %x\n", (u32)base, (u32)ctr);
		ret = __LINE__;
	}

	csp_aes_getctr3(base, p_ctr);
	csp_aes_getctr2(base, p_ctr + 1);
	csp_aes_getctr1(base, p_ctr + 2);
	csp_aes_getctr0(base, p_ctr + 3);

	return ret;
}

static int __aes_input(void *base, void *input)
{
	u32 *p_in = (u32 *)input;
	int ret = 0;

	if ((base == 0) || (input == NULL)) {
		PRT_ERR("base = %x, ctr = %x\n", (u32)base, (u32)input);
		ret = __LINE__;
	}
	PRT_INFO("p_in = %x %x %x %x\n",
			*(p_in+3), *(p_in+2), *(p_in+1), *p_in);

	csp_aes_input3(base, p_in);
	csp_aes_input2(base, p_in + 1);
	csp_aes_input1(base, p_in + 2);
	csp_aes_input0(base, p_in + 3);

	return ret;
}

static int __aes_output(void *base, void *output)
{
	u32 *p_out = (u32 *)output;
	int ret = 0;

	if ((base == 0) || (output == NULL)) {
		PRT_ERR("base = %x, ctr = %x\n", (u32)base, (u32)output);
		ret = __LINE__;
	}

	csp_aes_output3(base, p_out);
	csp_aes_output2(base, p_out + 1);
	csp_aes_output1(base, p_out + 2);
	csp_aes_output0(base, p_out + 3);

	PRT_INFO("p_out = %x %x %x %x\n",
			*(p_out+3), *(p_out+2), *(p_out+1), *p_out);

	return ret;
}

static int __aes_set_key(void *base, void *key, u32 key_len)
{
	int ret = 0;
	u32 *p_key = (u32 *)key;

	if ((base == 0) || (key == NULL)) {
		PRT_ERR("base = %x, key = %x\n", (u32)base, (u32)key);
		ret = __LINE__;
	}

	switch (key_len) {
	case AES_KEY_128BIT:
		csp_aes_setkey3(base, p_key);
		csp_aes_setkey2(base, p_key + 1);
		csp_aes_setkey1(base, p_key + 2);
		csp_aes_setkey0(base, p_key + 3);
		csp_aes_set_key_length(base, AES_KEY_128BIT);
		break;
	case AES_KEY_192BIT:
		csp_aes_setkey5(base, p_key);
		csp_aes_setkey4(base, p_key + 1);
		csp_aes_setkey3(base, p_key + 2);
		csp_aes_setkey2(base, p_key + 3);
		csp_aes_setkey1(base, p_key + 4);
		csp_aes_setkey0(base, p_key + 5);
		csp_aes_set_key_length(base, AES_KEY_192BIT);
		break;
	case AES_KEY_256BIT:
		csp_aes_setkey7(base, p_key);
		csp_aes_setkey6(base, p_key + 1);
		csp_aes_setkey5(base, p_key + 2);
		csp_aes_setkey4(base, p_key + 3);
		csp_aes_setkey3(base, p_key + 4);
		csp_aes_setkey2(base, p_key + 5);
		csp_aes_setkey1(base, p_key + 6);
		csp_aes_setkey0(base, p_key + 7);
		csp_aes_set_key_length(base, AES_KEY_256BIT);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static void aes_ahb_init(void)
{
	reg_prcm_ahb_gat0_t reg;

	reg.val = READREG32(VA_PRCM_AHB_GAT0);
	reg.bits.aes = 1;

	WRITEREG32(VA_PRCM_AHB_GAT0, reg.val);
}

static void aes_ahb_rst(u32 en)
{
	reg_prcm_ahb_rst0_t reg;

	reg.val = READREG32(VA_PRCM_AHB_RST0);
	reg.bits.aes = en;

	WRITEREG32(VA_PRCM_AHB_RST0, reg.val);
}

static void aes_clk_init(void)
{
	reg_prcm_aes_clk_ctrl_t reg;

	reg.val = READREG32(VA_PRCM_AES_CLK_CTRL);
	reg.bits.en = 1;

	WRITEREG32(VA_PRCM_AES_CLK_CTRL, reg.val);
}

static void aes_init(void)
{
	aes_ahb_init();
	aes_clk_init();
	aes_ahb_rst(0);
	aes_ahb_rst(1);
}

static int do_crypt(struct lombo_aes_op *lombo_aes)
{
	u32 status = 0;

	__aes_input(lombo_aes->base, lombo_aes->src);
	csp_aes_data_output_en(lombo_aes->base, 0);

	/* Start the operation */
	do {
		status = csp_aes_get_status(lombo_aes->base);
		PRT_INFO("waiting until the aes moudle was ready!\n");
	} while (status != 0);
	csp_aes_start(lombo_aes->base, 1);

	/* wait for output ready */
	/* do {
		status = csp_aes_data_out_ready(lombo_aes->base);
		PRT_INFO("waiting until the output was ready!\n");
	} while (status != 1);
	*/
	mdelay(15);
	/* output enable */
	csp_aes_data_output_en(lombo_aes->base, 1);
	while ((lombo_aes->dst[0] == 0) && (lombo_aes->dst[2] == 0) &&
		(lombo_aes->dst[4] == 0) && (lombo_aes->dst[7] == 0)) {
		__aes_output(lombo_aes->base, lombo_aes->dst);
	}

	return 0;
}

static unsigned int lombo_aes_crypt(struct lombo_aes_op *lombo_aes)
{
	int ret = 0;

	if ((lombo_aes->crypto_ed == AES_CRYPTO_ENCRYPT) ||
			(lombo_aes->crypto_ed == AES_CRYPTO_DECRYPT))
		csp_aes_set_crypto_ed(lombo_aes->base, lombo_aes->crypto_ed);
	else {
		PRT_ERR("error aes crypto mode!\n");
		ret = __LINE__;
		goto out;
	}

	if (lombo_aes->mode == AES_MODE_CBC) {
		ret = __aes_set_iv(lombo_aes->base, lombo_aes->iv);
		if (ret != 0) {
			PRT_ERR("Failed to set iv!\n");
			ret = __LINE__;
			goto out;
		}
	} else if (lombo_aes->mode == AES_MODE_CTR) {
		ret = __aes_set_ctr(lombo_aes->base, lombo_aes->ctr);
		if (ret != 0) {
			PRT_ERR("Failed to set ctr!\n");
			ret = __LINE__;
			goto out;
		}
	}
	csp_aes_set_mode(lombo_aes->base, lombo_aes->mode);

	if (lombo_aes->key_src == AES_KEY_SRC_REG) {
		csp_aes_set_key_src(lombo_aes->base, AES_KEY_SRC_REG);
		ret = __aes_set_key(lombo_aes->base, lombo_aes->key,
							lombo_aes->key_len);
		if (ret != 0) {
			PRT_ERR("Failed to set key!\n");
			ret = __LINE__;
			goto out;
		}
	} else if (lombo_aes->key_src == AES_KEY_SRC_EFUSE) {
		csp_aes_set_key_src(lombo_aes->base, AES_KEY_SRC_EFUSE);
		csp_aes_set_key_length(lombo_aes->base, AES_KEY_128BIT);
		/* don't need to set key but best to set key length */
	}

	ret = do_crypt(lombo_aes);

	if (lombo_aes->mode == AES_MODE_CBC)
		__aes_get_iv(lombo_aes->base, lombo_aes->iv);
	else if (lombo_aes->mode == AES_MODE_CTR)
		__aes_get_ctr(lombo_aes->base, lombo_aes->ctr);

	return ret;
out:
	PRT_ERR("error at %d.\n", ret);
	return ret;
}

static int get_param(const char *buf, struct lombo_aes_op *lombo_aes)
{
	int ret = 0;
	/* u8 *mode = NULL, *key_src = NULL; */
	u8 *p1 = NULL, *p2 = NULL;
	u8 *data = NULL;
	u32 key_len = 0;

	data = kzalloc(6, GFP_KERNEL | __GFP_ZERO);
	if (data == NULL) {
		PRT_ERR("unable to alloc data.\n");
		ret = -ENOMEM;
		return ret;
	}
#if 0
	key_src = kzalloc(5, GFP_KERNEL | __GFP_ZERO);
	if (key_src == NULL) {
		PRT_ERR("unable to alloc data.\n");
		kfree(mode);
		ret = -ENOMEM;
		return ret;
	}

	/* get AES mode */;
	sscanf(buf, "%s,%s,%d", mode, key_src, &key_len);
	PRT_INFO("mode = %s\n", mode);
	PRT_INFO("key_src = %s\n", key_src);
	PRT_INFO("key_len = %d\n", key_len);
#endif
	sscanf(buf, "%s", data);
	PRT_INFO("mode = %s\n", data);
	if (strcmp(data, "ecb") == 0)
		lombo_aes->mode = AES_MODE_ECB;
	else if (strcmp(data, "cbc") == 0) {
		lombo_aes->mode = AES_MODE_CBC;
		memcpy(lombo_aes->iv, iv, 16);
	} else if (strcmp(data, "ctr") == 0) {
		lombo_aes->mode = AES_MODE_CTR;
		memcpy(lombo_aes->ctr, ctr, 16);
	} else {
		PRT_ERR("Error AES mode.\n");
		ret = __LINE__;
		goto out;
	}

	/* get key source */
	memset(data, 0, 6);
	p1 = strchr(buf, ' ');
	sscanf(p1 + 1, "%s", data);
	PRT_INFO("key_src = %s\n", data);
	if (strcmp(data, "reg") == 0)
		lombo_aes->key_src = AES_KEY_SRC_REG;
	else if (strcmp(data, "efuse") == 0) {
		lombo_aes->key_src = AES_KEY_SRC_EFUSE;
		lombo_aes->key_len = AES_KEY_128BIT;
		/* if the key source is efuse, don't need to set key */
		goto out;
	} else {
		PRT_ERR("Error AES key source.\n");
		ret = __LINE__;
		goto out;
	}

	/* get key length(only 128bit if key source is efuse) */
	p2 = strchr(p1 + 1, ' ');
	sscanf(p2, "%d", &key_len);
	PRT_INFO("key_len = %dbit\n", key_len);
	if (key_len == 128) {
		lombo_aes->key_len = AES_KEY_128BIT;
		memcpy(lombo_aes->key, key_128bit, 16);
	} else if (key_len == 192) {
		lombo_aes->key_len = AES_KEY_192BIT;
		memcpy(lombo_aes->key, key_192bit, 24);
	} else if (key_len == 256) {
		lombo_aes->key_len = AES_KEY_256BIT;
		memcpy(lombo_aes->key, key_256bit, 32);
	} else {
		PRT_ERR("Error AES key length.\n");
		ret = __LINE__;
		goto out;
	}
out:
	if (data != NULL) {
		kfree(data);
		data = NULL;
	}

	return ret;
}

static ssize_t aes_show(struct class *class, struct class_attribute *attr,
								char *buf)
{
	if (lombo_aes.ret == 0)
		PRT_INFO("test success!\n");
	else {
		PRT_ERR("test failed!\n");
		PRT_ERR("Error at %d\n", lombo_aes.ret);
	}

	sprintf(buf, "%d", lombo_aes.ret);
	return strlen(buf);
}

static ssize_t aes_store(struct class *class, struct class_attribute *attr,
						const char *buf, size_t size)
{
	u32 base = 0xf1205000;
	int ret = 0;

	PRT_INFO("----aes test start---");
	/* init aes clk source */
	aes_init();

	/* alloc buf for data */
	lombo_aes.src = kzalloc(AES_BLK_SIZE, GFP_KERNEL | __GFP_ZERO);
	if (lombo_aes.src == NULL) {
		PRT_ERR("unable to alloc src.\n");
		ret = -ENOMEM;
		goto out;
	}
	lombo_aes.dst = kzalloc(AES_BLK_SIZE, GFP_KERNEL | __GFP_ZERO);
	if (lombo_aes.dst == NULL) {
		PRT_ERR("unable to alloc dst.\n");
		ret = -ENOMEM;
		goto out;
	}

	ret = get_param(buf, &lombo_aes);
	if (ret != 0) {
		PRT_ERR("Failed to get param!\n");
		ret = __LINE__;
		goto out;
	}

	lombo_aes.crypto_ed = AES_CRYPTO_ENCRYPT;
	lombo_aes.base = (void *)base;
	memcpy(lombo_aes.src, code, AES_BLK_SIZE);

	/* start encrypt */
	ret = lombo_aes_crypt(&lombo_aes);
	if (ret == 0) {
		PRT_INFO("encrypt success!\n");
		/* start decrypt */
		lombo_aes.crypto_ed = AES_CRYPTO_DECRYPT;
		memcpy(lombo_aes.src, lombo_aes.dst, AES_BLK_SIZE);

		if (lombo_aes.mode == AES_MODE_CBC)
			memcpy(lombo_aes.iv, iv, 16);
		else if (lombo_aes.mode == AES_MODE_CTR)
			memcpy(lombo_aes.ctr, ctr, 16);
		memset(lombo_aes.dst, 0, AES_BLK_SIZE);

		ret = lombo_aes_crypt(&lombo_aes);
		if (ret == 0) {
			PRT_INFO("decrypt success!\n");
			/* compare with plaintext */
			ret = strcmp(lombo_aes.dst, code);
			if (ret == 0) {
				PRT_INFO("encrypt and decrypt test success!\n");
			} else {
				PRT_ERR("encrypt and decrypt test failed!\n");
				PRT_ERR("the input is: %s\n", code);
				PRT_ERR("the output is: %s\n", lombo_aes.dst);
				ret = __LINE__;
				goto out;
			}
		} else {
			PRT_ERR("decrypt failed!\n");
			ret = __LINE__;
			goto out;
		}
	} else {
		PRT_ERR("encrypt failed!\n");
		ret = __LINE__;
		goto out;
	}
out:
	if (lombo_aes.src != NULL) {
		kfree(lombo_aes.src);
		lombo_aes.src = NULL;
	}
	if (lombo_aes.dst != NULL) {
		kfree(lombo_aes.dst);
		lombo_aes.dst = NULL;
	}
	lombo_aes.ret = ret;
	PRT_INFO("----aes test end---");
	return size;
}

static struct class_attribute aes_attrs[] = {
	__ATTR(aes, S_IWUSR | S_IRUGO, aes_show, aes_store),
	__ATTR_NULL
};

static struct class aes_class = {
	.name		= "lombo_aes",
	.owner		= THIS_MODULE,
	.class_attrs	= aes_attrs,
};

int __init aes_class_init(void)
{
	int ret = 0;

	ret = class_register(&aes_class);
	if (ret)
		PRT_ERR("aes_class register failed!\n");

	return ret;
}

void __exit aes_class_exit(void)
{
	class_unregister(&aes_class);
}

module_init(aes_class_init);
module_exit(aes_class_exit);

MODULE_AUTHOR("lomboswer@lombotech.com");
MODULE_DESCRIPTION("Driver for LomboTech aes test module");
MODULE_LICENSE("GPL");
