/*
 * csp_ac.h - Generic definitions for LomboTech SoCs
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

#ifndef __CSP_AC_H__
#define __CSP_AC_H__

#include "../i2s/csp_i2s.h"

/* set I2S data fmt */
void csp_ac_set_i2sfmt(void *regs, enum i2s_format fmt);

/* set master/slave mode */
void csp_ac_set_ms_mode(void *regs, enum i2s_ms_mode mode);

/* init some default config for ac */
void csp_ac_init(void *regs);

/* power on the ac */
void csp_ac_poweron(void *regs);

/* power off the ac */
void csp_ac_poweroff(void *regs);

/* mute of unmute the playback */
void csp_ac_playback_mute(void *regs, int mute);

/* set playback fs */
void csp_ac_set_playback_fs(void *regs, int fs);

/* set record fs */
void csp_ac_set_record_fs(void *regs, int fs);

#ifdef CONFIG_ARCH_LOMBO_N9
/* set channel cnt */
void csp_ac_set_dac_channel(void *regs, int channel);

void csp_ac_set_adc_channel(void *regs, int channel);

/* tx control on: 1 start tx, 0 stop tx */
void csp_ac_tx_ctl(void *regs, int on);

/* rx control on: 1 start tx, 0 stop tx */
void csp_ac_rx_ctl(void *regs, int on);
#endif

#endif /* __CSP_AC_H__ */
