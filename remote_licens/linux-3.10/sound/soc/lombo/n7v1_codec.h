/*
 * n7v1_codec.h - Generic definitions for LomboTech SoCs
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

#ifndef __N7V1_CODEC_H
#define __N7V1_CODEC_H

#define N7V1_AC_VER              (0x00000000)
#define N7V1_AC_I2SCFG           (0x00000004)
#define N7V1_AC_ADFS             (0x00000008)
#define N7V1_AC_DAFS             (0x0000000C)
#define N7V1_AC_ADCLK            (0x00000010)
#define N7V1_AC_DACLK            (0x00000014)
#define N7V1_AC_ACRST            (0x00000020)
#define N7V1_AC_PWRCM            (0x00000030)
#define N7V1_AC_PWRM             (0x00000034)
#define N7V1_AC_PWRIN            (0x00000038)
#define N7V1_AC_PWROUT           (0x0000003C)
#define N7V1_AC_SCTL             (0x00000040)
#define N7V1_AC_IRECLS           (0x00000050)
#define N7V1_AC_IRECRS           (0x00000054)
#define N7V1_AC_MUTEIN           (0x00000060)
#define N7V1_AC_VPGAL            (0x00000070)
#define N7V1_AC_VPGAR            (0x00000074)
#define N7V1_AC_VRECL            (0x00000080)
#define N7V1_AC_VRECR            (0x00000084)
#define N7V1_AC_MPLAYL           (0x00000090)
#define N7V1_AC_DMUTEOUT         (0x000000A0)
#define N7V1_AC_AMUTEOUT         (0x000000A8)
#define N7V1_AC_VPLAYL           (0x000000B0)
#define N7V1_AC_RECCTL           (0x000000D4)
#define N7V1_AC_PLAYCTL          (0x000000D8)
#define N7V1_AC_VOLSP            (0x000000E0)
#define N7V1_AC_CHARGE           (0x000000E4)

#define N7AC_CACHEREGNUM	27

#define RESET_DEFAULT		0x3
#define AC_MUTE			0x1
#define AC_UNMUTE		0x0
#define AC_POWER_ON		0x1
#define AC_POWER_OFF		0x0

#endif /* __N7V1_CODEC_H */
