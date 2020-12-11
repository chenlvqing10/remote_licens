/*
 * system_multi_lang.h - Multi-language setting business layer interface
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#ifndef _SYSTEM_MULTI_LANG_H
#define _SYSTEM_MULTI_LANG_H

typedef enum {
	MULTI_LANG_ENGLISH = 0,
	MULTI_LANG_CHINESE,
} multi_lang_type_e;

/**
 * system_multi_lang_set Set system language type
 * @lang_type System language type
 *
 * @return 0-successful, ohter-failed
 */
int system_multi_lang_set(multi_lang_type_e lang_type);

/**
 * system_multi_lang_get Get system language type
 * @type Pointer to multi_lang_type_e
 *
 * @return 0-successful, ohter-failed
 */
int system_multi_lang_get(multi_lang_type_e *type);

#endif /* _SYSTEM_MULTI_LANG_H */
