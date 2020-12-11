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

#ifndef _EE_QUEUE_H_
#define _EE_QUEUE_H_

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

typedef struct ee_queue_s ee_queue_t;
struct ee_queue_s {
	ee_queue_t *prev;
	ee_queue_t *next;
};

/* Initialization queue */
#define ee_queue_init(q)		\
	do {				\
		(q)->prev = (q);	\
		(q)->next = (q);	\
	} while (0)

/* Determine if the queue is empty */
#define ee_queue_empty(h) ((h) == (h)->prev)

/* Insert node from the front */
#define ee_queue_insert_head(h, x)	\
	do {				\
		(x)->next = (h)->next;	\
		(x)->next->prev = (x);	\
		(x)->prev = (h);	\
		(h)->next = (x);	\
	} while (0)

#define ee_queue_insert_after ee_queue_insert_head

/* Insert node from the end */
#define ee_queue_insert_tail(h, x)	\
	do {				\
		(x)->prev = (h)->prev;	\
		(x)->prev->next = x;	\
		(x)->next = h;		\
		(h)->prev = x;		\
	} while (0)

/* Get the head node pointed to by the head pointer */
#define ee_queue_head(h) ((h)->next)

/* Get the last node */
#define ee_queue_last(h) ((h)->prev)

/* Sentinel node */
#define ee_queue_sentinel(h) (h)

/* Next node */
#define ee_queue_next(q) ((q)->next)

/* Previous node */
#define ee_queue_prev(q) ((q)->prev)

/* Remove a node */
#define ee_queue_remove(x)			\
	do {					\
		(x)->next->prev = (x)->prev;	\
		(x)->prev->next = (x)->next;	\
		(x)->prev = NULL;		\
		(x)->next = NULL;		\
	} while (0)

/* Split a queue
 *	h: Head pointer
 *	q: Head pointer of the queue that needs to be split
 *	n: Header of another queue after the split is complete
 */
#define ee_queue_split(h, q, n)			\
	do {					\
		(n)->prev = (h)->prev;		\
		(n)->prev->next = n;		\
		(n)->next = q;			\
		(h)->prev = (q)->prev;		\
		(h)->prev->next = h;		\
		(q)->prev = n;			\
	} while (0)

/* Merge two queues into one queue */
#define ee_queue_add(h, n)			\
	do {					\
		(h)->prev->next = (n)->next;	\
		(n)->next->prev = (h)->prev;	\
		(h)->prev = (n)->prev;		\
		(h)->prev->next = (h);		\
	} while (0)

/* According to the queue pointer, get the structure containing this queue
 * pointer
 *	q:Queue pointer
 *	type: Returned data type
 *	link: The corresponding queue item name in the data item
 */
#define ee_queue_data(q, type, link) ((type *) ((char *)q - offsetof(type, link)))

/* Find intermediate nodes */
ee_queue_t *ee_queue_middle(ee_queue_t *queue);

/* Sort the queue */
void ee_queue_sort(ee_queue_t *queue, int (*cmp)(const ee_queue_t *,
		const ee_queue_t *));

#define ee_queue_foreach(q, s, type, link) \
	ee_queue_t *_head_ = NULL; \
	for (_head_ = ee_queue_head(s), q = ee_queue_data(_head_, type, link);\
		_head_ != s; _head_ = ee_queue_next(_head_), \
		q = ee_queue_data(_head_, type, link))

#define NA_QUEUE_INIT(name) {&(name), &(name)}
#define ee_queue_is_last(head, node) ((node)->next == (head))

#define ee_queue_for_each(pos, head) \
	for (pos = (head)->next; pos != head; pos = pos->next)

#define ee_queue_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#endif /* _EE_QUEUE_H_ */

