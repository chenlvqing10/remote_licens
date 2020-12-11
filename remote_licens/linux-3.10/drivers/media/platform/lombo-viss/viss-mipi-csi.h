/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS-MIPI-CSI driver code for LomboTech
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

#ifndef ___VISS___MIPI__CSI__H___
#define ___VISS___MIPI__CSI__H___

#define MIPI_CSI_DRV_NAME		"viss-mipi-csi"

static inline bool mcsi_active(struct viss_dev *mcsi)
{
	unsigned long flags;
	bool ret;

	spin_lock_irqsave(&mcsi->slock, flags);
	ret = !!(mcsi->state & (1 << ST_PREV_RUN) ||
		mcsi->state & (1 << ST_PREV_STREAM));
	spin_unlock_irqrestore(&mcsi->slock, flags);
	return ret;
}

static inline void mcsi_active_queue_add(struct viss_dev *mcsi,
					 struct viss_buffer *buf)
{
	list_add_tail(&buf->list, &mcsi->active_buf_q);
	mcsi->active_cnt++;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
}

static inline struct viss_buffer *mcsi_active_queue_pop(struct viss_dev *mcsi)
{
	struct viss_buffer *buf = list_entry(mcsi->active_buf_q.next,
					      struct viss_buffer, list);
	list_del(&buf->list);
	mcsi->active_cnt--;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
	return buf;
}

static inline void mcsi_pending_queue_add(struct viss_dev *mcsi,
					 struct viss_buffer *buf)
{
	list_add_tail(&buf->list, &mcsi->pending_buf_q);
	mcsi->pending_cnt++;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
}

static inline struct viss_buffer *mcsi_pending_queue_pop(struct viss_dev *mcsi)
{
	struct viss_buffer *buf = list_entry(mcsi->pending_buf_q.next,
					     struct viss_buffer, list);
	list_del(&buf->list);
	mcsi->pending_cnt--;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
	return buf;
}

static inline struct viss_buffer *mcsi_active_queue_pop_tail(
						struct viss_dev *mcsi)
{
	struct viss_buffer *buf = list_entry(mcsi->active_buf_q.prev,
					      struct viss_buffer, list);
	list_del(&buf->list);
	mcsi->active_cnt--;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
	return buf;
}

static inline void mcsi_pending_queue_add_front(struct viss_dev *mcsi,
					 struct viss_buffer *buf)
{
	list_add(&buf->list, &mcsi->pending_buf_q);
	mcsi->pending_cnt++;
	PRT_DBG("y=0x%08x\n", buf->paddr.y);
}

#endif /* ___VISS___MIPI__CSI__H___ */
