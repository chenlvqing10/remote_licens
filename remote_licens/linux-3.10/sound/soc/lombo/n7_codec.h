/*
 * n7_codec.h - Generic definitions for LomboTech SoCs
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

#ifndef __N7_CODEC_H
#define __N7_CODEC_H

#define N7_AC_ACR                (0x00000000)
#define N7_AC_ACL                (0x00000004)
#define N7_AC_ADCLK              (0x00000008)
#define N7_AC_DACLK              (0x0000000C)
#define N7_AC_I2SADFS            (0x00000030)
#define N7_AC_I2SDAFS            (0x00000034)
#define N7_AC_I2SCFG             (0x00000038)
#define N7_AC_SCTL               (0x00000044)
#define N7_AC_PWRCM              (0x00000048)
#define N7_AC_PWRM               (0x00000054)
#define N7_AC_PWRIN              (0x00000058)
#define N7_AC_PWROUT             (0x00000060)
#define N7_AC_MCTL0              (0x00000074)
#define N7_AC_MCTL2              (0x0000007C)
#define N7_AC_MCTL4              (0x00000084)
#define N7_AC_RECVL              (0x00000090)
#define N7_AC_RECVR              (0x00000094)
#define N7_AC_IPGAVL             (0x00000098)
#define N7_AC_IPGAVR             (0x0000009C)
#define N7_AC_OPMVC              (0x000000D0)
#define N7_AC_PGAISL             (0x0000011C)
#define N7_AC_PGAISM             (0x00000120)
#define N7_AC_OPMC               (0x00000164)
#define N7_AC_PSC                (0x00000288)
#define N7_AC_DT                 (0x00000348)
#define N7_AC_HPM                (0x0000034C)
#define N7_AC_DMIX               (0x00000350)
#define N7_AC_SRMP               (0x00000354)
#define N7_AC_DAAT1              (0x000003C0)
#define N7_AC_DAAT2              (0x000003C4)
#define N7_AC_DAAT3              (0x000003C8)
#define N7_AC_DAAT4              (0x000003CC)
#define N7_AC_DAAT5              (0x000003D0)
#define N7_AC_PAT0               (0x000003E0)
#define N7_AC_PAT1               (0x000003E4)
#define N7_AC_PAT2               (0x000003E8)
#define N7_AC_PAT3               (0x000003EC)
#define N7_AC_VER                (0x000003F0)

#define N7AC_CACHEREGNUM	27

#define RESET_DEFAULT		0x3
#define AC_MUTE			0x1
#define AC_UNMUTE		0x0
#define AC_POWER_ON		0x1
#define AC_POWER_OFF		0x0

#endif /* __N7_CODEC_H */
