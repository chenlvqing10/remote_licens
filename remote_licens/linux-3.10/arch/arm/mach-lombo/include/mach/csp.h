/*
 * csp.h - register operation head file for LomboTech Socs
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

#ifndef __CSP_H
#define __CSP_H

#if defined(CONFIG_ARCH_LOMBO_N7V0)
#include "csp/csp-n7v0.h"
#elif defined(CONFIG_ARCH_LOMBO_N7V1)
#include "csp/csp-n7v1.h"
#else
#error "please select a correct platform"
#endif

#endif /* __CSP_H */

