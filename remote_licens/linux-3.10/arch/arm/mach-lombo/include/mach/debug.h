/*
 * debug.h - debug macros for each driver
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

#ifndef __MACH_DEBUG_H___
#define __MACH_DEBUG_H___

#ifdef __KERNEL__ /* ignored by deep sleep module */

#include <linux/printk.h>

#ifndef DEBUG	/* kill the possible warning "DEBUG redefned..." */
#define DEBUG	/* validate pr_debug */
#endif

/*
 * common defination for all modules
 */
#define MOD_ERR(mod, fmt, ...)		\
	pr_err("[%s] %s:%d err: "fmt, mod, __func__, __LINE__, ##__VA_ARGS__)

#define MOD_WARN(mod, fmt, ...)		\
	pr_err("[%s] %s:%d warn: "fmt, mod, __func__, __LINE__, ##__VA_ARGS__)

#define MOD_MODIFY(mod, fmt, ...)	\
	pr_err("[%s] %s:%d modify: "fmt, mod, __func__, __LINE__, ##__VA_ARGS__)

#define MOD_INFO(mod, fmt, ...)		\
	pr_info("[%s] %s:%d "fmt, mod, __func__, __LINE__, ##__VA_ARGS__)

#define MOD_DBG(mod, fmt, ...)		\
	pr_debug("[%s] %s:%d "fmt, mod, __func__, __LINE__, ##__VA_ARGS__)

#ifdef ENABLE_TRACE
#ifdef TRACE_FUNC_NAME_ONLY
#define MOD_TRACE_BEGIN(mod, fmt, ...)		\
	 pr_debug("[%s]%s|B\n", mod, __func__)
#define MOD_TRACE_END(mod, fmt, ...)		\
	 pr_debug("[%s]%s|E|\n", mod, __func__)
#else
#define MOD_TRACE_BEGIN(mod, fmt, ...)		\
	 pr_debug("[%s]%s|B|"fmt, mod, __func__, ##__VA_ARGS__)
#define MOD_TRACE_END(mod, fmt, ...)		\
	 pr_debug("[%s]%s|E|"fmt, mod, __func__, ##__VA_ARGS__)
#endif
#else
#define MOD_TRACE_BEGIN(mod, fmt, ...)	do {} while (0)
#define MOD_TRACE_END(mod, fmt, ...)	do {} while (0)
#endif

/*
 * the default module name
 * MOD_NAME should be redefined in each module's source code
 */
#ifndef MOD_NAME
#define MOD_NAME	KBUILD_MODNAME
#endif

/*
 * print functions for all n7 modules
 * usage:
 * 1. redefine MOD_NAME in specific module source file
 * 2. use the macros below to print various messages
 */
#define PRT_ERR(fmt, ...)	MOD_ERR(MOD_NAME, fmt, ##__VA_ARGS__)
#define PRT_WARN(fmt, ...)	MOD_WARN(MOD_NAME, fmt, ##__VA_ARGS__)
#define PRT_MODIFY(fmt, ...)	MOD_MODIFY(MOD_NAME, fmt, ##__VA_ARGS__)
#define PRT_INFO(fmt, ...)	MOD_INFO(MOD_NAME, fmt, ##__VA_ARGS__)
#define PRT_DBG(fmt, ...)	MOD_DBG(MOD_NAME, fmt, ##__VA_ARGS__)

#define PRT_TRACE_BEGIN(fmt, ...)		\
	 MOD_TRACE_BEGIN(MOD_NAME, fmt, ##__VA_ARGS__)
#define PRT_TRACE_END(fmt, ...)		\
	 MOD_TRACE_END(MOD_NAME, fmt, ##__VA_ARGS__)

#endif /* __KERNEL__ */

#endif /* __MACH_DEBUG_H___ */

