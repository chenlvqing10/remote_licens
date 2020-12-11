/*
 * common_record_info.h - Interface for management punch record
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

#ifndef __COMMON_RECORD_INFO_H
#define __COMMON_RECORD_INFO_H

#include "ee_queue.h"

typedef struct tag_punch_record {
	/* id */
	int id;

	/* The person ID of the punch record */
	int fk_per_id;

	/* Face recognition score */
	double match_score;

	/* The address where punched */
	char address[32];

	/* Punch time(ms) */
	double punch_time;

	/* The name of the large picture of the punch record */
	char punch_live_large_pic_name[128];

	/* The name of the small picture of the punch record */
	char punch_live_small_pic_name[128];

	/* Punch pass. Face recognition is below a preset threshold, or a nonliving
	 * record.
	 * Options:
	 *	1: recognition passed
	 *	0: recognition reject
	 */
	char punch_pass;

	/* Bidirectional queue */
	ee_queue_t queue;
} punch_record_t;

#endif /* __COMMON_RECORD_INFO_H */

