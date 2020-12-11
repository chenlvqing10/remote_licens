/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _CSP_DOSS_TCON_H_
#define _CSP_DOSS_TCON_H_

#if IS_ENABLED(CONFIG_ARCH_LOMBO_N7V0)

#include "csp_doss_tcon_v1_0.h"

#else

#include "csp_doss_tcon_v1_1.h"

#endif /* #if IS_ENABLED(CONFIG_ARCH_LOMBO_N7V0) */

#endif /* #ifndef _CSP_DOSS_TCON_H_ */
