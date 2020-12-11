/*
 * blockdev.c - nand block driver code for LomboTech
 * nand block driver driver code implement
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

#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/kdev_t.h>
#include <linux/blkdev.h>
#include <linux/mutex.h>
#include <linux/scatterlist.h>
#include <linux/string_helpers.h>
#include <linux/delay.h>
#include <linux/capability.h>
#include <linux/compat.h>
#include <linux/uaccess.h>

#include "include/nftl.h"

/* #define DBG */
#include <mach/debug.h>
#undef MOD_NAME
#define MOD_NAME	"NANDBLK"


/* #define USE_ABUF */
#define USE_RBUF
/* #define TURBO_READ */
#ifdef TURBO_READ
/* #define SEPARATE_READ */
#endif
/* #define DEBUG_ADDR */

#define NAND_BLOCK_DEBUG
#ifdef NAND_BLOCK_DEBUG
#define DBG_INFO(fmt, ...)	PRT_INFO(fmt "\n", ##__VA_ARGS__)
#endif

#define DBG_ERR(fmt, ...)	PRT_ERR(fmt "\n", ##__VA_ARGS__)

#ifndef	assert
#define assert(x)	BUG_ON(!(x))
#endif

#define	SECTOR_SHIFT			9
#define MAX_NAND_BLOCK_REQ_SECTOR_CNT	128
#define MAX_NAND_BLOCK_REQ_SIZE		\
	((1UL << SECTOR_SHIFT) * MAX_NAND_BLOCK_REQ_SECTOR_CNT)

#define NAND_BLOCK_MAJOR		253
#define NAND_BLOCK_MINORS		32
#define NAND_NAME			"nand"
#define NAND_BLOCK_NAME			"nandblk"
#define NAND_BLOCK_THREAD_NAME		"nandblkqd"
#ifdef TURBO_READ
#define NAND_BLOCK_READ_THREAD_NAME	"nandblkqdr"
#endif
#define NFTL_THREAD_NAME		"nftlbg"

#define NFTL_SCHEDULE_TIMEOUT		(HZ)


struct nandblock_geometry {
	int sector_size;
	int sector_count;
};

struct nandblock_operations {
	int (*read_sector)(int start_sector, int sector_cnt, void *buf);
	int (*write_sector)(int start_sector, int sector_cnt, void *buf);
	int (*flush)(void);
	int (*trim)(int start_sector, int sector_cnt);
};

struct nandblock {
	char *name;
	int major;
	int minors;
	spinlock_t queue_lock;
	struct semaphore queue_thread_sem;
	struct task_struct *queue_thread;
#ifdef TURBO_READ
	struct semaphore queue_read_req_thread_sem;
	struct task_struct *queue_read_req_thread;
	struct mutex readreq_mutex;
#ifdef SEPARATE_READ
	spinlock_t readreq_spinlock;
	struct list_head readreq_head;
#endif
#endif
	struct task_struct *nftl_thread;
	struct gendisk *gd;
	struct nandblock_geometry geom;
	struct nandblock_operations *ops;
#ifdef	USE_ABUF
	char *arbuf;	/* align rbuf */
	char *awbuf;	/* align wbuf */
#endif
#ifdef	USE_RBUF
	char *rbuf;	/* read buf */
#endif
};

struct nandblock_operations nandblk_ops = {
	_nftl_read_sector,
	_nftl_write_sector,
	_nftl_flush,
	_nftl_trim,
};

static struct nandblock *g_nandblk;

static inline void init_nandblock_geometry(void)
{
	assert(g_nandblk);
	g_nandblk->geom.sector_size = _nftl_get_sector_size();
	g_nandblk->geom.sector_count = _nftl_get_sector_count();
}

static void free_nandblk(void)
{
	if (g_nandblk) {
#ifdef	USE_RBUF
		kfree(g_nandblk->rbuf);
#endif

#ifdef	USE_ABUF
		kfree(g_nandblk->arbuf);
		kfree(g_nandblk->awbuf);
#endif

		kfree(g_nandblk);
		g_nandblk = NULL;
	}
}

static int alloc_nandblk(void)
{
	int res = 0;

	if (g_nandblk) {
		DBG_INFO("realloc_nandblk");
		return 0;
	}

	g_nandblk = kzalloc(sizeof(*g_nandblk), GFP_KERNEL);
	if (!g_nandblk) {
		res = -ENOMEM;
		goto err;
	}

	init_nandblock_geometry();

#ifdef	USE_ABUF
	g_nandblk->arbuf = kmalloc(g_nandblk->geom.sector_size, GFP_KERNEL);
	if (!g_nandblk->arbuf) {
		res = -ENOMEM;
		goto err;
	}

	g_nandblk->awbuf = kmalloc(g_nandblk->geom.sector_size, GFP_KERNEL);
	if (!g_nandblk->awbuf) {
		res = -ENOMEM;
		goto err;
	}
#endif

#ifdef	USE_RBUF
	g_nandblk->rbuf = kmalloc(MAX_NAND_BLOCK_REQ_SIZE, GFP_KERNEL);
	if (!g_nandblk->rbuf) {
		res = -ENOMEM;
		goto err;
	}
#endif

	g_nandblk->name = NAND_BLOCK_NAME;
	g_nandblk->major = NAND_BLOCK_MAJOR;
	g_nandblk->minors = NAND_BLOCK_MINORS;
	spin_lock_init(&g_nandblk->queue_lock);
	sema_init(&g_nandblk->queue_thread_sem, 1);
#ifdef TURBO_READ
	sema_init(&g_nandblk->queue_read_req_thread_sem, 1);
	mutex_init(&g_nandblk->readreq_mutex);
#ifdef SEPARATE_READ
	spin_lock_init(&g_nandblk->readreq_spinlock);
	INIT_LIST_HEAD(&g_nandblk->readreq_head);
#endif
#endif

	g_nandblk->ops = &nandblk_ops;

	return 0;

err:
	DBG_ERR("kzalloc fail");
	free_nandblk();
	return res;
}

static inline int get_nandblk_ss(void)
{
	return g_nandblk->geom.sector_size >> SECTOR_SHIFT;
}

static inline int get_nandblk_cap(void)
{
	return g_nandblk->geom.sector_count * get_nandblk_ss();
}

#ifdef	TURBO_READ
#ifdef SEPARATE_READ
static inline void add_readreq(struct request *req)
{
	spin_lock(&g_nandblk->readreq_spinlock);
	list_add_tail(&req->queuelist, &g_nandblk->readreq_head);
	spin_unlock(&g_nandblk->readreq_spinlock);
}

#if 0
static inline void del_readreq(struct request *req)
{
	BUG_ON(list_empty(&req->queuelist));
	spin_lock(&g_nandblk->readreq_spinlock);
	list_del_init(&req->queuelist);
	spin_unlock(&g_nandblk->readreq_spinlock);
}
#endif

static inline struct request *get_readreq(void)
{
	struct request *req = NULL;

	spin_lock(&g_nandblk->readreq_spinlock);

	if (!list_empty(&g_nandblk->readreq_head)) {
		req = list_entry_rq(g_nandblk->readreq_head.next);
		list_del_init(&req->queuelist);
	}

	spin_unlock(&g_nandblk->readreq_spinlock);

	return req;
}
#endif
#endif

/*
 * @start_sector and @sector_cnt: in units of 512
 * @rw: 0 -> read; not 0 -> write
 */
static int nandblk_rw(int start_sector, int sector_cnt, void *buf, int rw)
{
	int res;
	int cur_sector_cnt;
	int nandblk_ss = get_nandblk_ss();
	int sector_offs = start_sector % nandblk_ss;

	while (sector_cnt > 0) {
		/* is not aligned */
		if (sector_offs || (sector_cnt < nandblk_ss)) {
#ifdef	USE_ABUF
			DBG_INFO("not aligned[0x%x, 0x%x], sector_offs = 0x%x",
				start_sector, sector_cnt, sector_offs);

#ifdef TURBO_READ
			/* lock arbuf */
			if (!rw)
				mutex_lock(&g_nandblk->readreq_mutex);
#endif

			res = g_nandblk->ops->read_sector(
				start_sector / nandblk_ss,
				1,
				rw ? g_nandblk->awbuf : g_nandblk->arbuf);
			if (res) {
				DBG_ERR("nandblk read_sector[0x%x, 1] fail(%i)",
					start_sector / nandblk_ss, res);
#ifdef TURBO_READ
				/* unlock arbuf */
				if (!rw)
					mutex_unlock(&g_nandblk->readreq_mutex);
#endif
				return -EIO;
			}

			cur_sector_cnt = min(
				nandblk_ss - sector_offs, sector_cnt);

			if (rw) {
				memcpy(g_nandblk->awbuf
					+ (sector_offs << SECTOR_SHIFT),
					buf,
					cur_sector_cnt << SECTOR_SHIFT);

				res = g_nandblk->ops->write_sector(
					start_sector / nandblk_ss,
					1,
					g_nandblk->awbuf);
				if (res) {
					DBG_ERR("w_sector[0x%x, 1] fail(%i)",
						start_sector / nandblk_ss, res);
					return -EIO;
				}
			} else {
				memcpy(buf,
					g_nandblk->arbuf
					+ (sector_offs << SECTOR_SHIFT),
					cur_sector_cnt << SECTOR_SHIFT);
#ifdef TURBO_READ
				/* unlock arbuf */
				mutex_unlock(&g_nandblk->readreq_mutex);
#endif
			}

			buf += cur_sector_cnt << SECTOR_SHIFT;
			start_sector += cur_sector_cnt;
			sector_cnt -= cur_sector_cnt;

			sector_offs = 0;
#else
			DBG_ERR("not aligned[0x%x, 0x%x], sector_offs = 0x%x",
				start_sector, sector_cnt, sector_offs);
			return -EINVAL;
#endif
		} else {
			if (rw) {
				res = g_nandblk->ops->write_sector(
					start_sector / nandblk_ss,
					sector_cnt / nandblk_ss,
					buf);
				if (res) {
					DBG_ERR("nandblk write_sector");
					DBG_ERR("[0x%x, 0x%x] fail(%i)",
						start_sector / nandblk_ss,
						sector_cnt / nandblk_ss,
						res);
					return -EIO;
				}
			} else {
				res = g_nandblk->ops->read_sector(
					start_sector / nandblk_ss,
					sector_cnt / nandblk_ss,
					buf);
				if (res) {
					DBG_ERR("nandblk read_sector");
					DBG_ERR("[0x%x, 0x%x] fail(%i)",
						start_sector / nandblk_ss,
						sector_cnt / nandblk_ss,
						res);
					return -EIO;
				}
			}

			cur_sector_cnt = sector_cnt / nandblk_ss * nandblk_ss;

			buf += cur_sector_cnt << SECTOR_SHIFT;
			start_sector += cur_sector_cnt;
			sector_cnt -= cur_sector_cnt;
		}
	}

	return 0;
}


static inline int nandblk_read_sector(int start_sector, int sector_cnt,
				void *buf)
{
	return nandblk_rw(start_sector, sector_cnt, buf, 0);
}

static inline int nandblk_write_sector(int start_sector, int sector_cnt,
				void *buf)
{
	return nandblk_rw(start_sector, sector_cnt, buf, 1);
}

static void nandblk_setup_discard(struct request_queue *q,
				unsigned int max_discard_sectors,
				unsigned int min_discard_bytes)
{
	if (!max_discard_sectors)
		return;

	queue_flag_set_unlocked(QUEUE_FLAG_DISCARD, q);
	q->limits.max_discard_sectors = max_discard_sectors;
	q->limits.discard_granularity = min_discard_bytes;
	/* granularity must not be greater than max. discard */
	if ((min_discard_bytes >> 9) > max_discard_sectors)
		q->limits.discard_granularity = 0;
}

static void nandblk_discard_req(struct request *req)
{
	int res = 0;
	int nandblk_ss = get_nandblk_ss();
	int start_sector = blk_rq_pos(req);
	int sector_cnt = blk_rq_sectors(req);

#ifdef DEBUG_ADDR
	DBG_INFO("discard[0x%x, 0x%x]", start_sector, sector_cnt);
#endif

	if ((start_sector % nandblk_ss) || (sector_cnt % nandblk_ss)) {
		DBG_INFO("start_sector(0x%x) or sector_cnt(0x%x)",
			start_sector, sector_cnt);
		DBG_INFO("is not aligned with nandblk_ss(0x%x)",
			nandblk_ss);
	} else
		res = g_nandblk->ops->trim(start_sector / nandblk_ss,
			sector_cnt / nandblk_ss);

	blk_end_request(req, res ? -EIO : 0, blk_rq_bytes(req));
}

static inline int __nandblk_flush_req(void)
{
	return g_nandblk->ops->flush();
}

static void nandblk_flush_req(struct request *req)
{
	int res = __nandblk_flush_req();

	blk_end_request_all(req, res ? -EIO : 0);
	/* blk_end_request(req, res ? -EIO : 0, blk_rq_bytes(req)); */

#ifdef DEBUG_ADDR
	DBG_INFO("flush");
#endif
}

/* async mode, need to care about q->queue_lock */
static void nandblk_write_req(struct request *req)
{
	int res;
	int start_sector;
	int sector_cnt;
	char *buf;

#ifdef DEBUG_ADDR
	DBG_INFO("[%i:%s]write[0x%llx, 0x%x]",
	current->pid, current->comm, blk_rq_pos(req), blk_rq_sectors(req));
#endif

	do {
		if (req->bio && req->bio->bi_vcnt)
			flush_dcache_page(bio_page(req->bio));

		start_sector = blk_rq_pos(req);
		sector_cnt = blk_rq_cur_sectors(req);
		buf = req->buffer;

		res = nandblk_write_sector(start_sector, sector_cnt, buf);
		if (res) {
			DBG_ERR("write req[0x%x, 0x%x] fail(%i)",
				start_sector, sector_cnt, res);
			blk_end_request(req, -EIO, blk_rq_bytes(req));
			return;
		}
	} while (blk_end_request_cur(req, res));
}

#ifndef	USE_RBUF
/* async mode, need to care about q->queue_lock */
static void nandblk_read_req(struct request *req)
{
	int res;
	int start_sector;
	int sector_cnt;
	char *buf;

#ifdef DEBUG_ADDR
	DBG_INFO("[%i:%s]read[0x%llx, 0x%x]",
	current->pid, current->comm, blk_rq_pos(req), blk_rq_sectors(req));
#endif

	do {
		start_sector = blk_rq_pos(req);
		sector_cnt = blk_rq_cur_sectors(req);
		buf = req->buffer;

		res = nandblk_read_sector(start_sector, sector_cnt, buf);
		if (res) {
			DBG_ERR("read req[0x%x, 0x%x] fail(%i)",
				start_sector, sector_cnt, res);
			blk_end_request(req, -EIO, blk_rq_bytes(req));
			return;
		}

		if (req->bio && req->bio->bi_vcnt)
			flush_dcache_page(bio_page(req->bio));
	} while (blk_end_request(req, res, blk_rq_cur_bytes(req)));
}
#else
/* async mode, need to care about q->queue_lock */
static void nandblk_read_req(struct request *req)
{
	int res;
	int start_sector;
	int sector_cnt;

	start_sector = blk_rq_pos(req);
	sector_cnt = blk_rq_sectors(req);

#ifdef DEBUG_ADDR
	DBG_INFO("[%i:%s]read[0x%x, 0x%x]",
	current->pid, current->comm, start_sector, sector_cnt);
#endif

	if (blk_rq_bytes(req) <= blk_rq_cur_bytes(req)) {
#ifdef DEBUG_ADDR
		DBG_INFO("no copied read[0x%x, 0x%x]",
			start_sector, sector_cnt);
#endif

		res = nandblk_read_sector(start_sector, sector_cnt,
			req->buffer);

		blk_end_request(req, res, blk_rq_bytes(req));

		if (res)
			DBG_ERR("no copied read req[0x%x, 0x%x] fail(%i)",
				start_sector, sector_cnt, res);
	} else {
		int cur_len;
		char *rbuf = g_nandblk->rbuf;

		assert(blk_rq_bytes(req) <= MAX_NAND_BLOCK_REQ_SIZE);

#ifdef TURBO_READ
		/* lock rbuf */
		mutex_lock(&g_nandblk->readreq_mutex);
#endif

		res = nandblk_read_sector(start_sector, sector_cnt, rbuf);
		if (res) {
			DBG_ERR("copied read req[0x%x, 0x%x] fail(%i)",
				start_sector, sector_cnt, res);
			blk_end_request(req, -EIO, blk_rq_bytes(req));

#ifdef TURBO_READ
			mutex_unlock(&g_nandblk->readreq_mutex);
#endif
			return;
		}

		do {
			char *buf = req->buffer;
			cur_len = blk_rq_cur_bytes(req);
			memcpy(buf, rbuf, cur_len);
			rbuf += cur_len;
		} while (blk_end_request(req, res, cur_len));

#ifdef TURBO_READ
		mutex_unlock(&g_nandblk->readreq_mutex);
#endif
	}

	rq_flush_dcache_pages(req);
}
#endif

/* async mode, need to care about q->queue_lock */
static void nandblk_do_requset(struct request *req)
{
	unsigned int cmd_flags = req ? req->cmd_flags : 0;

	if (cmd_flags & REQ_FLUSH)
		nandblk_flush_req(req);
	else if (cmd_flags & REQ_DISCARD)
		nandblk_discard_req(req);
	else {
		if (READ == rq_data_dir(req))
			nandblk_read_req(req);
		else {
			nandblk_write_req(req);

			if (cmd_flags & REQ_FUA) {
#ifdef DEBUG_ADDR
				DBG_INFO("fua");
#endif
				__nandblk_flush_req();
			}
		}
	}
}

#ifdef	TURBO_READ
/* async mode, need to care about q->queue_lock */
static inline int is_read_req(struct request *req)
{
	unsigned int cmd_flags = req ? req->cmd_flags : 0;

	return !(cmd_flags & REQ_FLUSH) && !(cmd_flags & REQ_DISCARD)
		&& (READ == rq_data_dir(req));
}

/* async mode, need to care about q->queue_lock */
static int nandblk_read_req_thread(void *d)
{
	struct nandblock *nandblk = d;
#ifndef SEPARATE_READ
	struct request_queue *q = nandblk->gd->queue;
#endif
	struct request *req = NULL;

	current->flags |= PF_MEMALLOC;

	down(&nandblk->queue_read_req_thread_sem);
	do {
#ifdef SEPARATE_READ
		set_current_state(TASK_INTERRUPTIBLE);
		req = get_readreq();

		if (req) {
			assert(is_read_req(req));
			nandblk_read_req(req);
		} else {
			if (kthread_should_stop()) {
				set_current_state(TASK_RUNNING);
				break;
			}
			up(&nandblk->queue_read_req_thread_sem);
			schedule();
			down(&nandblk->queue_read_req_thread_sem);
		}
#else
		spin_lock_irq(q->queue_lock);
		set_current_state(TASK_INTERRUPTIBLE);
		req = blk_fetch_request(q);

		if (req) {
			if (is_read_req(req)) {
				spin_unlock_irq(q->queue_lock);
				nandblk_read_req(req);
				continue;
			} else {
				blk_requeue_request(q, req);
				wake_up_process(nandblk->queue_thread);
				spin_unlock_irq(q->queue_lock);
			}
		} else
			spin_unlock_irq(q->queue_lock);

		if (kthread_should_stop()) {
			set_current_state(TASK_RUNNING);
			break;
		}
		up(&nandblk->queue_read_req_thread_sem);
		schedule();
		down(&nandblk->queue_read_req_thread_sem);
#endif
	} while (1);
	up(&nandblk->queue_read_req_thread_sem);

	return 0;
}
#endif

/* async mode, need to care about q->queue_lock */
static int nandblk_thread(void *d)
{
	struct nandblock *nandblk = d;
	struct request_queue *q = nandblk->gd->queue;
	struct request *req = NULL;

	current->flags |= PF_MEMALLOC;

	down(&nandblk->queue_thread_sem);
	do {
		spin_lock_irq(q->queue_lock);
		set_current_state(TASK_INTERRUPTIBLE);
		req = blk_fetch_request(q);
		spin_unlock_irq(q->queue_lock);

		if (req)
			nandblk_do_requset(req);
		else {
			__nandblk_flush_req();
			if (kthread_should_stop()) {
				set_current_state(TASK_RUNNING);
				break;
			}
			up(&nandblk->queue_thread_sem);
			schedule();
			down(&nandblk->queue_thread_sem);
		}
	} while (1);
	up(&nandblk->queue_thread_sem);

	return 0;
}

/* called in blk_fetch_request() */
static int nandblk_prep_request(struct request_queue *q, struct request *req)
{
	/*
	 * We only like normal block requests and discards.
	 */
	if (req->cmd_type != REQ_TYPE_FS && !(req->cmd_flags & REQ_DISCARD)) {
		DBG_ERR("invalid cmd_type(%i)", req->cmd_type);
		DBG_ERR("invalid cmd_type(0x%x)", req->cmd_flags);
		return BLKPREP_KILL;
	}

	if (!(req->cmd_flags & REQ_FLUSH)) {
		if ((blk_rq_bytes(req) <= 0) ||
			(blk_rq_pos(req) + blk_rq_sectors(req)
			> get_capacity(req->rq_disk))) {
			DBG_ERR("(%s)req over capacity. (0x%llx + 0x%x > %llx)",
				(req->cmd_flags & REQ_DISCARD) ? "discard" :
				rq_data_dir(req) ? "write" : "read",
				blk_rq_pos(req),
				blk_rq_sectors(req),
				get_capacity(req->rq_disk));
				return BLKPREP_KILL;
		}
	}

	req->cmd_flags |= REQ_DONTPREP;

	return BLKPREP_OK;
}

/* sync mode, don't care about q->queue_lock */
static void nandblk_request_func(struct request_queue *q)
{
	struct request *req;
	struct nandblock *nandblk = q->queuedata;

	if (!nandblk) {
		DBG_INFO("nandblk offline");
		while ((req = blk_fetch_request(q)) != NULL) {
			req->cmd_flags |= REQ_QUIET;
			__blk_end_request_all(req, -EIO);
		}
		return;
	}

#ifdef	TURBO_READ
#ifdef SEPARATE_READ
	req = blk_fetch_request(q);

	if (!req)
		return;

	if (is_read_req(req)) {
		add_readreq(req);
		wake_up_process(nandblk->queue_read_req_thread);
		return;
	} else
		blk_requeue_request(q, req);
#else
	wake_up_process(nandblk->queue_read_req_thread);
#endif
#endif
	wake_up_process(nandblk->queue_thread);
}

static int nftl_thread(void *d)
{
	struct nandblock *nandblk = d;

	while (!kthread_should_stop()) {
		/* check if do_swl per 4s */
		_nftl_bg(4);

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(NFTL_SCHEDULE_TIMEOUT);
	}

	nandblk->nftl_thread = NULL;
	return 0;
}


/* strcut block_device_operations */
static int nandblk_ioctl(struct block_device *dev, fmode_t no,
			unsigned cmd, unsigned long arg)
{
#if 0
	DBG_INFO("nandblk_ioctl");
#endif
	return -ENOTTY;
}

static int nandblk_open(struct block_device *dev, fmode_t no)
{
#if 0
	DBG_INFO("nandblk_open");
#endif
	return 0;
}

static void nandblk_release(struct gendisk *gd, fmode_t no)
{
#if 0
	DBG_INFO("nandblk_release");
#endif
}

static const struct block_device_operations nand_blkdev_ops = {
	.owner = THIS_MODULE,
	.open = nandblk_open,
	.release = nandblk_release,
	.ioctl = nandblk_ioctl,
};

void nandblk_exit(void)
{
	if (!g_nandblk)
		return;

	if (g_nandblk->nftl_thread && !IS_ERR(g_nandblk->nftl_thread)) {
		kthread_stop(g_nandblk->nftl_thread);
		g_nandblk->nftl_thread = NULL;
	}

#ifdef	TURBO_READ
	if (g_nandblk->queue_read_req_thread
		&& !IS_ERR(g_nandblk->queue_read_req_thread)) {
		kthread_stop(g_nandblk->queue_read_req_thread);
		g_nandblk->queue_read_req_thread = NULL;
	}
#endif

	if (g_nandblk->queue_thread && !IS_ERR(g_nandblk->queue_thread)) {
		kthread_stop(g_nandblk->queue_thread);
		g_nandblk->queue_thread = NULL;
	}

	if (g_nandblk->gd) {
		if (g_nandblk->gd->queue)
			blk_cleanup_queue(g_nandblk->gd->queue);

		del_gendisk(g_nandblk->gd);
		g_nandblk->gd = NULL;
	}

	unregister_blkdev(g_nandblk->major, NAND_NAME);

	free_nandblk();
}

int nandblk_init(void)
{
	int res;
	struct gendisk *gd;
	struct request_queue *queue;
	int cap;

	res = alloc_nandblk();
	if (res) {
		DBG_ERR("alloc_nandblk fail(%i)", res);
		goto out;
	}

	res = register_blkdev(g_nandblk->major, NAND_NAME);
	if (res) {
		res = register_blkdev(0, NAND_NAME);
		if ((res < 1) || (res > 255)) {
			DBG_ERR("register_blkdev fail(%i)", res);
			goto out;
		}

		g_nandblk->major = res;
		res = 0;
	}

	gd = alloc_disk(g_nandblk->minors);
	if (!gd) {
		DBG_ERR("alloc_disk fail");
		res = -ENOMEM;
		goto out_unregister;
	}

	g_nandblk->gd = gd;

	queue = blk_init_queue(nandblk_request_func, &g_nandblk->queue_lock);
	gd->queue = queue;
	if (!queue) {
		DBG_ERR("blk_init_queue fail");
		res = -ENOMEM;
		goto out_putdisk;
	}

	blk_queue_prep_rq(queue, nandblk_prep_request);
	queue_flag_set_unlocked(QUEUE_FLAG_NONROT, queue);

	queue->queuedata = g_nandblk;

	g_nandblk->queue_thread = kthread_run(nandblk_thread, g_nandblk,
				NAND_BLOCK_THREAD_NAME);
	if (IS_ERR(g_nandblk->queue_thread)) {
		DBG_ERR("kthread_run fail");
		res = PTR_ERR(g_nandblk->queue_thread);
		goto out_cleanup_queue;
	}

#ifdef	TURBO_READ
	g_nandblk->queue_read_req_thread = kthread_run(nandblk_read_req_thread,
				g_nandblk, NAND_BLOCK_READ_THREAD_NAME);
	if (IS_ERR(g_nandblk->queue_read_req_thread)) {
		DBG_ERR("kthread_run fail");
		res = PTR_ERR(g_nandblk->queue_read_req_thread);
		goto out_stop_queue_thread;
	}
#endif

	gd->major = g_nandblk->major;
	gd->first_minor = 0;
	gd->fops = &nand_blkdev_ops;
	/* gd->private_data = &nandblk; */

	snprintf(gd->disk_name, sizeof(gd->disk_name), g_nandblk->name);

	blk_queue_logical_block_size(queue, g_nandblk->geom.sector_size);
	blk_queue_max_hw_sectors(queue, MAX_NAND_BLOCK_REQ_SECTOR_CNT);

	/* sector_size is 512 */
	cap = get_nandblk_cap();
	set_capacity(gd, cap);

	blk_queue_flush(queue, REQ_FLUSH | REQ_FUA);
	nandblk_setup_discard(queue, cap, g_nandblk->geom.sector_size);

	add_disk(gd);

	g_nandblk->nftl_thread = kthread_run(nftl_thread, g_nandblk,
				NFTL_THREAD_NAME);
	if (IS_ERR(g_nandblk->nftl_thread)) {
		DBG_ERR("kthread_run fail");
		res = PTR_ERR(g_nandblk->nftl_thread);
		goto out_del_disk;
	}

	return 0;

out_del_disk:
	del_gendisk(gd);
#ifdef	TURBO_READ
	kthread_stop(g_nandblk->queue_read_req_thread);
out_stop_queue_thread:
#endif
	kthread_stop(g_nandblk->queue_thread);
out_cleanup_queue:
	blk_cleanup_queue(queue);
out_putdisk:
	put_disk(gd);
out_unregister:
	unregister_blkdev(g_nandblk->major, NAND_NAME);
out:
	free_nandblk();
	return res;
}

