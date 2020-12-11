/*
 * record_auto_delete.h - Delete punch record auto at intervals.
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

#ifndef _RECORD_AUTO_DELETE_H
#define _RECORD_AUTO_DELETE_H

/**
 * is_deleting_record - return flag of deleting record.
 * @
 *
 * Returns 1 if success, otherwise 0.
 */
int is_deleting_record();

/**
 * punch_record_auto_del_proc - Delete punch record auto at intervals.
 * @
 *
 * Returns 0 if success, otherwise not 0.
 */
int punch_record_auto_del_proc();

#endif /* _RECORD_AUTO_DELETE_H */
