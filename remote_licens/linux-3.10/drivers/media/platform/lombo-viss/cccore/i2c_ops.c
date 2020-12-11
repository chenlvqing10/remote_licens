/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * Camera Control Core i2c operation code for LomboTech
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

#include <linux/i2c.h>

#include <mach/csp.h>
#include <mach/debug.h>
#include <linux/delay.h>

#include "ccc.h"

#ifdef msg
#undef msg
#endif

#define CC_REG_R8_V8_DESC		((8UL << 8) | 8UL)
#define CC_REG_R16_V8_DESC		((16UL << 8) | 8UL)
#define CC_REG_R8_V16_DESC		((8UL << 8) | 16UL)
#define CC_REG_R16_V16_DESC		((16UL << 8) | 16UL)

/*
 *
 */
static int cc_write_r8_v8(struct cc_sensor_info *cc_si, u32 reg, u32 val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg = {0};
	u8 buf[2] = {(u8)reg, (u8)val};
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg.addr  = client->addr;
	msg.flags = 0;
	msg.len   = 2;
	msg.buf   = buf;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret >= 0)
		ret = 0;
	else
		PRT_ERR("%s ret=%d,reg=0x%04x,val=0x%04x\n",
			cc_si->name, ret, reg, val);

	return ret;
}

static int cc_read_r8_v8(struct cc_sensor_info *cc_si, u32 reg, void *val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg[2] = {0};
	u8 buf[1] = {(u8)reg};
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 1;
	msg[0].buf   = buf;

	msg[1].addr  = client->addr;
	msg[1].flags |= I2C_M_RD;
	msg[1].len   = 1;
	msg[1].buf   = (u8 *)val;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret >= 0)
		ret = 0;
	else
		PRT_ERR("%s ret=%d,reg=0x%04x\n", cc_si->name, ret, reg);

	return ret;
}

static int cc_write_r16_v8(struct cc_sensor_info *cc_si, u32 reg, u32 val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg = {0};
	u8 buf[3] = { (u8)(reg >> 8), (u8)(reg & 0xff), (u8)val };
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg.addr  = client->addr;
	msg.flags = 0;
	msg.len   = 3;
	msg.buf   = buf;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret >= 0)
		ret = 0;
	else
		PRT_ERR("%s ret=%d,reg=0x%04x,val=0x%04x\n",
			cc_si->name, ret, reg, val);

	return ret;
}

static int cc_read_r16_v8(struct cc_sensor_info *cc_si, u32 reg, void *val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg[2] = {0};
	u8 buf[2] = { (u8)(reg >> 8), (u8)(reg & 0xff) };
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 2;
	msg[0].buf   = buf;

	msg[1].addr  = client->addr;
	msg[1].flags |= I2C_M_RD;
	msg[1].len   = 1;
	msg[1].buf   = (u8 *)val;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret >= 0)
		ret = 0;
	else
		PRT_ERR("%s ret=%d,reg=0x%04x\n", cc_si->name, ret, reg);

	return ret;
}

static int cc_write_r8_v16(struct cc_sensor_info *cc_si, u32 reg, u32 val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg;
	u8 buf[3] = { (u8)reg, (u8)(val >> 8), (u8)(val & 0xff) };
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg.addr  = client->addr;
	msg.flags = 0;
	msg.len   = 3;
	msg.buf   = buf;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret >= 0)
		ret = 0;
	else
		PRT_ERR("%s ret=%d,reg=0x%04x,val=0x%04x\n",
			cc_si->name, ret, reg, val);

	return ret;
}

static int cc_read_r8_v16(struct cc_sensor_info *cc_si, u32 reg, void *val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg[2] = {0};
	u8 tmp[2] = {0};
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 1;
	msg[0].buf   = (u8 *)(&reg);

	msg[1].addr  = client->addr;
	msg[1].flags |= I2C_M_RD;
	msg[1].len   = 2;
	msg[1].buf   = tmp;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret >= 0) {
		*((u16 *)val) = (tmp[0] << 8) | tmp[1];
		ret = 0;
	} else
		PRT_ERR("%s ret=%d,reg=0x%04x\n", cc_si->name, ret, reg);

	return ret;
}

static int cc_write_r16_v16(struct cc_sensor_info *cc_si, u32 reg, u32 val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg[2] = {0};
	u8 buf[4] = { (u8)(reg >> 8), (u8)(reg & 0xff),
			(u8)(val >> 8), (u8)(val & 0xff) };
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 4;
	msg[0].buf   = buf;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, msg, 1);
	if (ret >= 0)
		ret = 0;
	else
		PRT_ERR("%s ret=%d,reg=0x%04x,val=0x%04x\n",
			cc_si->name, ret, reg, val);

	return ret;
}

static int cc_read_r16_v16(struct cc_sensor_info *cc_si, u32 reg, void *val)
{
	struct i2c_client *client = cc_si->client;
	struct i2c_msg msg[2] = {0};
	u8 tmp[2] = {0};
	u8 buf[2] = { (u8)(reg >> 8), (u8)(reg & 0xff) };
	int ret;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	msg[0].addr  = client->addr;
	msg[0].flags = 0;
	msg[0].len   = 2;
	msg[0].buf   = buf;

	msg[1].addr  = client->addr;
	msg[1].flags |= I2C_M_RD;
	msg[1].len   = 2;
	msg[1].buf   = tmp;

	/*
	 * i2c_transfer return message length,
	 * but this function should return 0 if correct case
	 */
	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret >= 0) {
		*((u16 *)val) = (tmp[0] << 8) | tmp[1];
		ret = 0;
	} else
		PRT_ERR("%s ret=%d,reg=0x%04x\n", cc_si->name, ret, reg);

	return ret;
}

static int cc_write_reg_list(struct cc_sensor_info *cc_si,
			const cc_reg_t *regs_list, u32 regs_cnt)
{
	int i;
	int ret = 0;

	if (cc_si->power == 0) {
		PRT_ERR("%s sensor not power on!\n", cc_si->name);
		return -ENODEV;
	}

	if ((regs_list == NULL) || (regs_cnt == 0))
		return 0;

	for (i = 0; i < regs_cnt; i++) {
		if (regs_list[i].reg == CC_REG_DELAY) {
			sensor_msleep(regs_list[i].val);
			continue;
		}

		ret = cc_si->write_reg(cc_si, regs_list[i].reg,
					regs_list[i].val);
		if (ret != 0) {
			PRT_ERR("%s write i %d reg 0x%04x val 0x%04x error\n",
				cc_si->name, i, regs_list[i].reg,
				regs_list[i].val);
			break;
		}
	}

	return ret;
}

int cc_set_reg_ops(struct cc_sensor_info *cc_si)
{
	u32 reg_desc;
	int ret = 0;

	reg_desc = (cc_si->reg_bits << 8) | cc_si->val_bits;

	switch (reg_desc) {
	case CC_REG_R8_V8_DESC:
		cc_si->write_reg = cc_write_r8_v8;
		cc_si->read_reg = cc_read_r8_v8;
		break;
	case CC_REG_R16_V8_DESC:
		cc_si->write_reg = cc_write_r16_v8;
		cc_si->read_reg = cc_read_r16_v8;
		break;
	case CC_REG_R8_V16_DESC:
		cc_si->write_reg = cc_write_r8_v16;
		cc_si->read_reg = cc_read_r8_v16;
		break;
	case CC_REG_R16_V16_DESC:
		cc_si->write_reg = cc_write_r16_v16;
		cc_si->read_reg = cc_read_r16_v16;
		break;
	default:
		PRT_ERR("Not supported comb of reg and val bits (%d, %d)\n",
			cc_si->reg_bits, cc_si->val_bits);
		ret = -EINVAL;
		break;
	}
	cc_si->write_reg_list = cc_write_reg_list;

	return ret;
}
