/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS driver code for LomboTech
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

#ifndef ___VISS___VIC__H___
#define ___VISS___VIC__H___

#define VIC_DRV_NAME		"viss-vic"

static inline bool vic_active(struct viss_dev *vic)
{
	unsigned long flags;
	bool ret;

	spin_lock_irqsave(&vic->slock, flags);
	ret = !!(vic->state & (1 << ST_PREV_RUN) ||
		vic->state & (1 << ST_PREV_STREAM));
	spin_unlock_irqrestore(&vic->slock, flags);
	return ret;
}

static inline void vic_active_queue_add(struct viss_dev *vic,
					 struct viss_buffer *buf)
{
	list_add_tail(&buf->list, &vic->active_buf_q);
	vic->active_cnt++;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
}

static inline struct viss_buffer *vic_active_queue_pop(struct viss_dev *vic)
{
	struct viss_buffer *buf = list_entry(vic->active_buf_q.next,
					      struct viss_buffer, list);
	list_del(&buf->list);
	vic->active_cnt--;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
	return buf;
}

static inline void vic_pending_queue_add(struct viss_dev *vic,
					 struct viss_buffer *buf)
{
	list_add_tail(&buf->list, &vic->pending_buf_q);
	vic->pending_cnt++;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
}

static inline struct viss_buffer *vic_pending_queue_pop(struct viss_dev *vic)
{
	struct viss_buffer *buf = list_entry(vic->pending_buf_q.next,
					     struct viss_buffer, list);
	list_del(&buf->list);
	vic->pending_cnt--;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
	return buf;
}

static inline struct viss_buffer *vic_active_queue_pop_tail(
						struct viss_dev *vic)
{
	struct viss_buffer *buf = list_entry(vic->active_buf_q.prev,
					      struct viss_buffer, list);
	list_del(&buf->list);
	vic->active_cnt--;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
	return buf;
}

static inline void vic_pending_queue_add_front(struct viss_dev *vic,
					 struct viss_buffer *buf)
{
	list_add(&buf->list, &vic->pending_buf_q);
	vic->pending_cnt++;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
}

#endif /* ___VISS___VIC__H___ */
