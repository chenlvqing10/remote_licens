#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "iostream_file"

#define _FILE_OFFSET_BITS 64

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>

#include "oscl.h"
#include "list.h"
#include "iostream_file.h"

#define ENABLE_FILE_CACHE

#define FILE_CACHE_LEN		(32 * 1024)
#define FILE_CACHE_NUM		(8)

typedef enum {
	CACHE_STATE_INVALID,
	CACHE_STATE_CLEAN,
	CACHE_STATE_DIRT,
} cache_state_e;

typedef struct cache_buffer {
	struct list_head node;
	uint8_t *buf;
	size_t filled_len;
	size_t offset;
	int64_t block;
	cache_state_e state;
} cache_buffer_t;

typedef struct file_stream_info {
	int fd;
	int64_t file_size;
#ifdef ENABLE_FILE_CACHE
	struct list_head cache_list;
	int64_t vir_offset;
	int64_t file_offset;
	pthread_mutex_t lock;
#endif
	io_interface_t io_intf;
} file_stream_info_t;

static int64_t get_file_size(file_stream_info_t *stream)
{
	int64_t cur_ofs, ofs;
	cur_ofs = stream->io_intf.lseek(stream->fd, 0, SEEK_CUR);
	if (cur_ofs < 0) {
		OSCL_LOGE("lseek error(%s)", strerror(errno));
		return -1;
	}
	ofs = stream->io_intf.lseek(stream->fd, 0, SEEK_END);
	if (ofs < 0) {
		OSCL_LOGE("lseek error(%s)", strerror(errno));
		return -1;
	}
	cur_ofs = stream->io_intf.lseek(stream->fd, cur_ofs, SEEK_SET);
	if (cur_ofs < 0) {
		OSCL_LOGE("lseek error(%s)", strerror(errno));
		return -1;
	}
	return ofs;
}

static ssize_t _read(file_stream_info_t *stream, uint8_t *buf, size_t size)
{
	ssize_t rlen, remain;

	remain = size;
	while (remain > 0) {
		rlen = stream->io_intf.read(stream->fd, buf, remain);
		if (rlen < 0) {
			OSCL_LOGE("read %ld byte error(%s)!",
				(long)remain, strerror(errno));
			return -1;
		} else if (rlen == 0) {
			OSCL_LOGD("read file reach eof(%s)", strerror(errno));
			break;
		}
		remain -= rlen;
		buf += rlen;
	}
	return size - remain;
}

static ssize_t _write(file_stream_info_t *stream, uint8_t *buf, size_t size)
{
	ssize_t wlen, remain;

	if (stream->io_intf.write == NULL) {
		OSCL_LOGE("write func is NULL!");
		return -1;
	}

	remain = size;
	while (remain > 0) {
		wlen = stream->io_intf.write(stream->fd, buf, remain);
		if (wlen < 0) {
			OSCL_LOGE("write %ld bytes error(%s)!",
				(long)remain, strerror(errno));
			return -1;
		}
		remain -= wlen;
		buf += wlen;
	}
	return size;
}

#ifdef ENABLE_FILE_CACHE
static ssize_t read_one_block(file_stream_info_t *stream,
		cache_buffer_t *cache, int64_t block)
{
	int64_t offset = block * FILE_CACHE_LEN;
	ssize_t ret;

	if (stream->file_offset != offset) {
		OSCL_LOGD("seek to %lld", offset);
		offset = stream->io_intf.lseek(stream->fd, offset, SEEK_SET);
		if (offset == -1) {
			OSCL_LOGE("lseek error(%s)!", strerror(errno));
			return -1;
		}
		stream->file_offset = offset;
	}

	ret = _read(stream, cache->buf, FILE_CACHE_LEN);
	if (ret <= 0)
		return ret;

	cache->filled_len = ret;
	cache->block = block;
	cache->state = CACHE_STATE_CLEAN;
	stream->file_offset += ret;
	return ret;
}

static int flush_one_cache(file_stream_info_t *stream, cache_buffer_t *cache)
{
	int64_t offset;
	ssize_t ret;

	if (cache->state != CACHE_STATE_DIRT)
		return 0;

	offset = cache->block * FILE_CACHE_LEN;
	if (stream->file_offset != offset) {
		OSCL_LOGD("seek to %lld", offset);
		offset = stream->io_intf.lseek(stream->fd, offset, SEEK_SET);
		if (offset == -1) {
			OSCL_LOGE("lseek error(%s)!", strerror(errno));
			return -1;
		}
		stream->file_offset = offset;
	}

	ret = _write(stream, cache->buf, cache->filled_len);
	if (ret < 0)
		return ret;

	cache->state = CACHE_STATE_CLEAN;
	stream->file_offset += cache->filled_len;

	return 0;
}
#endif

ssize_t iostream_file_write(void *s, uint8_t *buf, size_t size)
{
	if (s == NULL || buf == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	file_stream_info_t *stream = (file_stream_info_t *)s;

#ifdef ENABLE_FILE_CACHE
	int ret;
	int64_t offset;

	pthread_mutex_lock(&stream->lock);
	if (stream->file_offset != stream->vir_offset) {
		OSCL_LOGD("seek to %lld", stream->vir_offset);
		offset = stream->io_intf.lseek(stream->fd, stream->vir_offset, SEEK_SET);
		if (offset == -1) {
			OSCL_LOGE("lseek error(%s)!", strerror(errno));
			ret = -1;
			goto EXIT;
		}
		stream->file_offset = offset;
		stream->vir_offset = offset;
	}
	ret = _write(stream, buf, size);
	if (ret < 0)
		goto EXIT;
	stream->vir_offset += ret;
	stream->file_size += ret;
	stream->file_offset += ret;

EXIT:
	pthread_mutex_unlock(&stream->lock);
	return ret;
#else
	return _write(stream, buf, size);
#endif
}

ssize_t iostream_file_read(void *s, uint8_t *buf, size_t size)
{
	if (s == NULL || buf == NULL) {
		OSCL_LOGE("para error! handle=%p, buf=%p, size=%d",
			s, buf, size);
		return -1;
	}
	file_stream_info_t *stream = (file_stream_info_t *)s;
#ifdef ENABLE_FILE_CACHE
	ssize_t read_len = 0;
	cache_buffer_t *cache;
	ssize_t read_sz;
	ssize_t remain;
	int64_t block, blk_offset;
	uint8_t *read_pos;

	OSCL_LOGD("read %d", size);
	pthread_mutex_lock(&stream->lock);
	if (stream->vir_offset >= stream->file_size)
		goto EXIT;
	read_pos = buf;
	read_sz = size;
	if (read_sz == 0)
		goto EXIT;

	remain = read_sz;
	block = stream->vir_offset / FILE_CACHE_LEN;
	blk_offset = stream->vir_offset % FILE_CACHE_LEN;

	while (remain > 0) {
		int hit_cache = 0;
		list_for_each_entry(cache, &stream->cache_list, node) {
			if (cache->state == CACHE_STATE_INVALID)
				continue;
			if (block == cache->block) {
				hit_cache = 1;
				break;
			}
		}
		if (!hit_cache) {
			int ret;
			cache = list_last_entry(&stream->cache_list,
				cache_buffer_t, node);
			if (flush_one_cache(stream, cache) < 0) {
				OSCL_LOGE("flush_one_cache error!");
				read_len = -1;
				goto EXIT;
			}
			ret = read_one_block(stream, cache, block);
			if (ret < 0) {
				OSCL_LOGE("read_one_block error(block=%lld)!", block);
				read_len = -1;
				goto EXIT;
			} else if (ret == 0)
				break;
		}
		int left_sz = stream->vir_offset - cache->block * FILE_CACHE_LEN;
		int right_sz = cache->filled_len - left_sz;
		if (remain < right_sz) {
			memcpy(read_pos, cache->buf+blk_offset, remain);
			stream->vir_offset += remain;
			read_pos += remain;
			remain = 0;
			break;
		}
		memcpy(read_pos, cache->buf + blk_offset, right_sz);
		stream->vir_offset += right_sz;
		read_pos += right_sz;
		remain -= right_sz;
		list_move(&cache->node, &stream->cache_list);
		block++;
		blk_offset = 0;
		if (cache->filled_len != FILE_CACHE_LEN) {
			OSCL_LOGD("reach eof");
			break;
		}
	}
	read_len = read_sz - remain;

EXIT:
	pthread_mutex_unlock(&stream->lock);

	return read_len;
#else
	return _read(stream, buf, size);
#endif
}

int64_t iostream_file_seek(void *s, int64_t offset, int whence)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	file_stream_info_t *stream = (file_stream_info_t *)s;
#ifdef ENABLE_FILE_CACHE
	cache_buffer_t *cache;
	int64_t target, start;
	int hit_cache = 0;

	OSCL_LOGD("cur offset %lld, seek offset %lld, whence %d",
		stream->vir_offset, offset, whence);

	pthread_mutex_lock(&stream->lock);
	switch (whence) {
	case SEEK_CUR:
		target = stream->vir_offset + offset;
		break;
	case SEEK_SET:
		target = offset;
		break;
	case SEEK_END:
		target = offset + stream->file_size;
		break;
	default:
		OSCL_LOGE("unknown seek command %d!!", whence);
		goto ERR_EXIT;
	}

	if (target < 0) {
		OSCL_LOGW("invalid offset %lld!!", target);
		target = 0;
	}

	list_for_each_entry(cache, &stream->cache_list, node) {
		if (cache->state == CACHE_STATE_INVALID)
			continue;
		start = cache->block * FILE_CACHE_LEN;
		if (target >= start && target < start + cache->filled_len) {
			hit_cache = 1;
			break;
		}
	}
	if (!hit_cache) {
		target = stream->io_intf.lseek(stream->fd, target, SEEK_SET);
		if (target == -1) {
			OSCL_LOGE("lseek error(%s)!", strerror(errno));
			goto ERR_EXIT;
		}
		stream->file_offset = target;
	} else
		list_move(&cache->node, &stream->cache_list);
	stream->vir_offset = target;
	pthread_mutex_unlock(&stream->lock);

	return target;

ERR_EXIT:
	pthread_mutex_unlock(&stream->lock);
	return -1;
#else
	return stream->io_intf.lseek(stream->fd, offset, whence);
#endif
}

int64_t iostream_file_tell(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	file_stream_info_t *stream = (file_stream_info_t *)s;

#ifdef ENABLE_FILE_CACHE
	int64_t offset;
	pthread_mutex_lock(&stream->lock);
	offset = stream->vir_offset;
	pthread_mutex_unlock(&stream->lock);
	return offset;
#else
	return stream->io_intf.lseek(stream->fd, 0, SEEK_CUR);
#endif
}

int64_t iostream_file_fsize(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("ctx==NULL!");
		return -1;
	}
	file_stream_info_t *stream = (file_stream_info_t *)s;

	return stream->file_size;
}

long iostream_file_ctrl(void *s, int cmd, void *arg)
{
	OSCL_LOGW("ctrl is not support yet!");
	return -1;
}

void *iostream_file_create(void *param)
{
	file_stream_info_t *stream;
	file_stream_para_t *para = param;

	stream = (file_stream_info_t *)oscl_malloc(sizeof(file_stream_info_t));
	if (stream == NULL) {
		OSCL_LOGE("alloc file stream info error!");
		return NULL;
	}
	stream->io_intf = para->io_intf;
	stream->fd = stream->io_intf.open(para->filename, O_RDONLY);
	if (stream->fd < 0) {
		OSCL_LOGE("open %s error(%s)!", para->filename, strerror(errno));
		goto ERR_OPEN_FILE;
	}
	stream->file_size = get_file_size(stream);
	if (stream->file_size < 0) {
		OSCL_LOGE("get_file_size error!");
		goto ERR_GET_SIZE;
	}

#ifdef ENABLE_FILE_CACHE
	cache_buffer_t *cache, *tmp;
	int i;

	stream->file_offset = 0;
	stream->vir_offset = 0;
	INIT_LIST_HEAD(&stream->cache_list);

	for (i = 0; i < FILE_CACHE_NUM; i++) {
		cache = (cache_buffer_t *)oscl_malloc(sizeof(cache_buffer_t));
		if (cache == NULL) {
			OSCL_LOGE("alloc cache error!");
			goto ERR_ALLOC_CTX;
		}
		cache->buf = (uint8_t *)oscl_malloc(FILE_CACHE_LEN);
		if (cache->buf == NULL) {
			OSCL_LOGE("alloc cache buffer error!");
			oscl_free(cache);
			goto ERR_ALLOC_CTX;
		}
		cache->filled_len = 0;
		cache->block = -1;
		cache->state = CACHE_STATE_INVALID;
		list_add(&cache->node, &stream->cache_list);
	}

	pthread_mutex_init(&stream->lock, NULL);
#endif

	return stream;

#ifdef ENABLE_FILE_CACHE
ERR_ALLOC_CTX:
	list_for_each_entry_safe(cache, tmp, &stream->cache_list, node) {
		list_del(&cache->node);
		oscl_free(cache->buf);
		oscl_free(cache);
	}
#endif
ERR_GET_SIZE:
	stream->io_intf.close(stream->fd);
ERR_OPEN_FILE:
	oscl_free(stream);
	return NULL;
}

void iostream_file_destroy(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return;
	}
	file_stream_info_t *stream = (file_stream_info_t *)s;

#ifdef ENABLE_FILE_CACHE
	pthread_mutex_lock(&stream->lock);
	cache_buffer_t *cache, *tmp;
	list_for_each_entry_safe(cache, tmp, &stream->cache_list, node) {
		list_del(&cache->node);
		if (cache->state == CACHE_STATE_DIRT) {
			flush_one_cache(stream, cache);
			cache->state = CACHE_STATE_CLEAN;
		}
		oscl_free(cache->buf);
		oscl_free(cache);
	}
	pthread_mutex_unlock(&stream->lock);
	pthread_mutex_destroy(&stream->lock);
#endif
	stream->io_intf.close(stream->fd);
	oscl_free(stream);
}

