/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef ___CSP_TVE__H___
#define ___CSP_TVE__H___

#include "csp_doss_tve_reg.h"
#include "../csp_tcon/csp_doss_tcon.h"

#define BASE_TVE 0x01620000
#define TVE_MAX_INST (2)
#define TVE_TCON_CLK_RATIO	(2)

typedef enum tve_enc_mode {
	TVE_CVBS	= 0x0,
	TVE_S_VIDEO	= 0x1,
	TVE_RGB		= 0x2,
	TVE_YPBPR	= 0x3,
} tve_enc_mode_t;

typedef enum tve_color_std {
	TVE_NTSC_M	= 0x00,
	TVE_NTSC_J	= 0x01,
	TVE_PAL_M	= 0x10,
	TVE_PAL_I	= 0x20,
	TVE_PAL_B	= 0x21,
	TVE_PAL_G	= 0x22,
	TVE_PAL_D	= 0x24,
	TVE_PAL_N	= 0x25,
	TVE_PAL_COMB_N	= 0x30,
} tve_color_std_t;

typedef struct tcon_tve_if {
	u32			index;
	tve_enc_mode_t		enc_mode;
	tve_color_std_t		color_std;
	tcon_host_t		*tcon_host;
} tcon_tve_if_t;

extern s32 csp_tve_set_register_base(u32 index, unsigned long addr, u32 size);
extern s32 csp_tve_get_register_base(u32 index, unsigned long *addr);
extern s32 csp_tve_init(u32 index);
extern s32 csp_tve_exit(u32 index);
extern s32 csp_tve_set_mode(u32 index, tcon_tve_if_t *tve_if);

#endif /* ___CSP_TVE__H___ */
