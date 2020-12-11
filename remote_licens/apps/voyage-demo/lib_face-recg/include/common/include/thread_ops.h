/*
 * thread_ops.h Thread operation interface.
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

#ifndef _THREAD_OPS_H
#define _THREAD_OPS_H

typedef enum tag_thread_done_e {
	EE_THREAD_NOT_RUN = 0,	/* Thread is not running yet */
	EE_THREAD_RUNNING,	/* Thread is running */
	EE_THREAD_DONE,		/* Thread is done */
} thread_done_e;

#endif /* _THREAD_OPS_H */

