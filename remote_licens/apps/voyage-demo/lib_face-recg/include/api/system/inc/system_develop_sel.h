/*
 * system_develop_sel.h - Interface to Set system developer selection
 * information from Excel.
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#ifndef _SYSTEM_DEVELOP_SEL_H
#define _SYSTEM_DEVELOP_SEL_H

typedef struct develop_sel_para {
	/* debug switch
	 * Options:
	 *	0: close
	 *	1: open
	 */
	char debug_sw;
} develop_sel_para_t;

typedef void (*develop_sel_para_change_cb)(develop_sel_para_t *p);

/**
* developer selection parameter change notify callback function
* @param p a pointer of callback
* @return NULL
*/
void develop_sel_register_change_cb(develop_sel_para_change_cb cb);

/**
 * get debug switch
 * @param p_develop_sel a pointer of parameter
 * @return 0-successful, ohter-failed
 */
int system_develop_sel_get_para(develop_sel_para_t *p_develop_sel);

/**
 * get debug switch
 * @param p_develop_sel a pointer of parameter
 * @return 0-successful, ohter-failed
 */
int system_develop_sel_set_para(develop_sel_para_t *p_develop_sel);


#endif /* _SYSTEM_TIME_H */
