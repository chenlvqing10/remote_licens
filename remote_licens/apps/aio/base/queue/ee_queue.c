/*
 * ee_queue.h - Bidirectional Queue: Refer to nginx and linux kernel queue
 * definitions.
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

#include <stdlib.h>
#include <stdio.h>

#include "ee_queue.h"

/*
 * find the middle queue element if the queue has odd number of elements
 * or the first element of the queue's second part otherwise
 */
ee_queue_t *ee_queue_middle(ee_queue_t *queue)
{
	ee_queue_t  *middle, *next;

	middle = ee_queue_head(queue);

	if (middle == ee_queue_last(queue))
		return middle;

	next = ee_queue_head(queue);

	for (;;) {
		middle = ee_queue_next(middle);

		next = ee_queue_next(next);

		if (next == ee_queue_last(queue))
			return middle;

		next = ee_queue_next(next);

		if (next == ee_queue_last(queue))
			return middle;
	}
}

/* the stable insertion sort */
void ee_queue_sort(ee_queue_t *queue,
	int (*cmp)(const ee_queue_t *, const ee_queue_t *))
{
	ee_queue_t  *q, *prev, *next;

	q = ee_queue_head(queue);

	if (q == ee_queue_last(queue))
		return;

	for (q = ee_queue_next(q); q != ee_queue_sentinel(queue); q = next) {
		prev = ee_queue_prev(q);
		next = ee_queue_next(q);

		ee_queue_remove(q);

		do {
			if (cmp(prev, q) <= 0)
				break;

			prev = ee_queue_prev(prev);

		} while (prev != ee_queue_sentinel(queue));

		ee_queue_insert_after(prev, q);
	}
}

