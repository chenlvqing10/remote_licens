#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "ring_buffer_mgr"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "oscl.h"

/* #define RING_BUF_DEBUG */

typedef struct RingBufferMgr {
	uint8_t *pBuf;
	uint8_t *pRPos;
	uint8_t *pWPos;
	long bufSize;
	long dataSize;
	long dirtySize;
	long freeSize;
	long minDirtySize;
	int64_t curOffset;
	pthread_mutex_t lock;
} RingBufferMgr_t;

#ifdef RING_BUF_DEBUG
static int check_error(RingBufferMgr_t *pRbm, const char *func, const int line)
{
	uint8_t *pe = pRbm->pBuf + pRbm->bufSize;
	long expectDritySize, expectDataSize;
	int ret = 0;
	if (pRbm->pWPos > pRbm->pRPos) {
		long rFreeSz = pe - pRbm->pWPos;
		long lFreeSz = pRbm->pRPos - pRbm->pBuf;
		expectDritySize = rFreeSz + lFreeSz - pRbm->freeSize;
		expectDataSize = pRbm->pWPos - pRbm->pRPos;
	} else if (pRbm->pWPos < pRbm->pRPos) {
		expectDritySize = pRbm->pRPos - pRbm->pWPos;
		expectDataSize = (pe - pRbm->pRPos) + (pRbm->pWPos - pRbm->pBuf);
	} else {
		if (pRbm->dataSize > 0) {
			expectDritySize = 0;
			expectDataSize = pRbm->bufSize;
		} else if (pRbm->freeSize > 0) {
			expectDritySize = pRbm->bufSize - pRbm->freeSize;
			expectDataSize = 0;
		} else {
			expectDritySize = pRbm->bufSize;
			expectDataSize = 0;
		}
	}
	if (expectDritySize != pRbm->dirtySize) {
		OSCL_LOGE("<%s,%d>fatal error! dirtySize %ld != %ld", func, line,
			expectDritySize, pRbm->dirtySize);
		ret = -1;
	}
	if (expectDataSize != pRbm->dataSize) {
		OSCL_LOGE("<%s,%d>fatal error! dataSize %ld != %ld", func, line,
			expectDataSize, pRbm->dataSize);
		ret = -1;
	}
	if (pRbm->dataSize + pRbm->dirtySize + pRbm->freeSize != pRbm->bufSize) {
		OSCL_LOGE("<%s,%d>fatal error!data(%ld)+dirty(%ld)+free(%ld)!=buf(%ld)",
			func, line,
			pRbm->dataSize, pRbm->dirtySize, pRbm->freeSize, pRbm->bufSize);
		ret = -1;
	}
	return ret;
}
#endif

static void _rbm_reset(RingBufferMgr_t *pRbm)
{
	pRbm->pRPos = pRbm->pBuf;
	pRbm->pWPos = pRbm->pBuf;
	pRbm->dataSize = 0;
	pRbm->dirtySize = 0;
	pRbm->freeSize = pRbm->bufSize;
	pRbm->curOffset = 0;
}

void rbm_reset(void *handle)
{
	RingBufferMgr_t *pRbm = (RingBufferMgr_t *)handle;

	pthread_mutex_lock(&pRbm->lock);
	_rbm_reset(pRbm);
	pthread_mutex_unlock(&pRbm->lock);
}

void *rbm_create(size_t bufsize, int align)
{
	RingBufferMgr_t *pRbm;

	pRbm = oscl_malloc(sizeof(RingBufferMgr_t));
	if (pRbm == NULL) {
		OSCL_LOGE("alloc RingBufferMgr error!");
		return NULL;
	}

	pRbm->pBuf = oscl_malloc(bufsize);
	if (pRbm->pBuf == NULL) {
		OSCL_LOGE("alloc pBuf error!");
		free(pRbm);
		return NULL;
	}

	pRbm->pRPos = pRbm->pBuf;
	pRbm->pWPos = pRbm->pBuf;
	pRbm->bufSize = bufsize;
	pRbm->dataSize = 0;
	pRbm->dirtySize = 0;
	pRbm->freeSize = bufsize;
	pRbm->minDirtySize = bufsize / 16;
	pRbm->curOffset = 0;
	pthread_mutex_init(&pRbm->lock, NULL);

	return pRbm;
}

void rbm_destroy(void *handle)
{
	if (handle == NULL) {
		OSCL_LOGE("handle==NULL!");
		return;
	}
	RingBufferMgr_t *pRbm = (RingBufferMgr_t *)handle;
	pthread_mutex_destroy(&pRbm->lock);
	oscl_free(pRbm->pBuf);
	oscl_free(pRbm);
}

long rbm_write(void *handle, uint8_t *data, unsigned long size)
{
	if (handle == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	RingBufferMgr_t *pRbm = (RingBufferMgr_t *)handle;
	uint8_t *p = data, *pe = pRbm->pBuf + pRbm->bufSize;
	long remain = size;
	long wlen, cpSize;

	pthread_mutex_lock(&pRbm->lock);
	OSCL_LOGD("writing %ld bytes, rpos=%ld, wpos=%ld, data=%ld, dirty=%ld, free=%ld",
		size,
		(long)(pRbm->pRPos-pRbm->pBuf),
		(long)(pRbm->pWPos-pRbm->pBuf),
		pRbm->dataSize,
		pRbm->dirtySize,
		pRbm->freeSize);
	while (remain > 0) {
		if (pRbm->pWPos > pRbm->pRPos) {
			long rFreeSz = pe - pRbm->pWPos;
			long lFreeSz = pRbm->pRPos - pRbm->pBuf;
			long freeSz = rFreeSz + lFreeSz;
			if (freeSz <= pRbm->minDirtySize)
				freeSz = 0;
			else if (lFreeSz < pRbm->minDirtySize)
				freeSz -= pRbm->minDirtySize;
			else
				freeSz = rFreeSz;
			if (remain > freeSz)
				cpSize = freeSz;
			else
				cpSize = remain;
		} else if (pRbm->pWPos < pRbm->pRPos) {
			long freeSz = pRbm->pRPos - pRbm->pWPos;
			if (freeSz <= pRbm->minDirtySize)
				freeSz = 0;
			else
				freeSz -= pRbm->minDirtySize;
			if (remain > freeSz)
				cpSize = freeSz;
			else
				cpSize = remain;
		} else {
			if (pRbm->dataSize > 0) {
				OSCL_LOGE("fatal error! buf=%ld, rpos=%ld, wpos=%ld",
					pRbm->bufSize,
					(long)(pRbm->pRPos-pRbm->pBuf),
					(long)(pRbm->pWPos-pRbm->pBuf));
				OSCL_LOGE("data=%ld, dirty=%ld, free=%ld",
					pRbm->dataSize, pRbm->dirtySize, pRbm->freeSize);
				OSCL_LOGE("fatal error! writeSize=%ld, remain=%ld",
					size, remain);
				goto EXIT_ERR;
			}
			OSCL_LOGD("buffer empty!");
			cpSize = 1;
		}
		if (cpSize == 0) {
			OSCL_LOGD("buf full, dataSize=%ld, dirtySize=%ld, freeSize=%ld",
				pRbm->dataSize, pRbm->dirtySize, pRbm->freeSize);
			break;
		} else if (cpSize < 0) {
			OSCL_LOGE("fatal error! buf=%ld, rpos=%ld, wpos=%ld",
				pRbm->bufSize,
				(long)(pRbm->pRPos-pRbm->pBuf),
				(long)(pRbm->pWPos-pRbm->pBuf));
			OSCL_LOGE("data=%ld, dirty=%ld, free=%ld",
				pRbm->dataSize, pRbm->dirtySize, pRbm->freeSize);
			OSCL_LOGE("fatal error! writeSize=%ld, remain=%ld, cpSize=%ld",
				size, remain, cpSize);
			goto EXIT_ERR;
		}
		memcpy(pRbm->pWPos, p, cpSize);
		pRbm->pWPos += cpSize;
		p += cpSize;
		pRbm->dataSize += cpSize;
		remain -= cpSize;
		if (pRbm->freeSize == 0) {
			pRbm->dirtySize -= cpSize;
			OSCL_LOGD("dirtySize=%ld, cpSize=%ld", pRbm->dirtySize, cpSize);
		} else {
			if (pRbm->freeSize < cpSize) {
				pRbm->dirtySize -= cpSize - pRbm->freeSize;
				OSCL_LOGI("dirtySize=%ld, freeSize=%ld, cpSize=%ld",
					pRbm->dirtySize, pRbm->freeSize, cpSize);
				pRbm->freeSize = 0;
			} else {
				pRbm->freeSize -= cpSize;
			}
		}
		if (pRbm->pWPos >= pe)
			pRbm->pWPos = pRbm->pBuf;
	}

	wlen = size - remain;
	OSCL_LOGD("write %ld bytes, dataSize=%ld, dirtySize=%ld, buf=%ld, wpos=%ld",
		wlen, pRbm->dataSize, pRbm->dirtySize,
		pRbm->bufSize, (long)(pRbm->pWPos-pRbm->pBuf));
#ifdef RING_BUF_DEBUG
	if (check_error(pRbm, __func__, __LINE__) != 0)
		goto EXIT_ERR;
#endif
	pthread_mutex_unlock(&pRbm->lock);

	return wlen;

EXIT_ERR:
	pthread_mutex_unlock(&pRbm->lock);
	return -1;
}

long rbm_read(void *handle, uint8_t *data, unsigned long size)
{
	if (handle == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	RingBufferMgr_t *pRbm = (RingBufferMgr_t *)handle;
	uint8_t *p = data;
	uint8_t *pe = pRbm->pBuf + pRbm->bufSize;
	long remain = size;
	long cpSize;
	long readRes = 0;

	pthread_mutex_lock(&pRbm->lock);
	OSCL_LOGD("reading %ld bytes, rpos=%ld, wpos=%ld, data=%ld, dirty=%ld, free=%ld",
		size,
		(long)(pRbm->pRPos-pRbm->pBuf),
		(long)(pRbm->pWPos-pRbm->pBuf),
		pRbm->dataSize,
		pRbm->dirtySize,
		pRbm->freeSize);
	while (remain > 0) {
		if (pRbm->pWPos > pRbm->pRPos) {
			if (remain > pRbm->dataSize)
				cpSize = pRbm->dataSize;
			else
				cpSize = remain;
		} else if (pRbm->pWPos < pRbm->pRPos) {
			int dataSz = pe - pRbm->pRPos;
			if (remain > dataSz)
				cpSize = dataSz;
			else
				cpSize = remain;
		} else {
			if (pRbm->dataSize > 0) {
				OSCL_LOGD("buffer full!");
				cpSize = 1;
			} else {
				OSCL_LOGD("buffer empty!");
				break;
			}
		}
		memcpy(p, pRbm->pRPos, cpSize);
		pRbm->pRPos += cpSize;
		pRbm->curOffset += cpSize;
		pRbm->dataSize -= cpSize;
		remain -= cpSize;
		p += cpSize;
		if (pRbm->pRPos >= pe)
			pRbm->pRPos = pRbm->pBuf;
		OSCL_LOGD("cpSize=%ld, remain=%ld", cpSize, remain);
	}

	readRes = size - remain;
	pRbm->dirtySize += readRes;
	OSCL_LOGD("read %ld bytes, dataSize=%ld, dirtySize=%ld, buf=%ld, rpos=%ld",
		readRes, pRbm->dataSize, pRbm->dirtySize,
		pRbm->bufSize, (long)(pRbm->pRPos-pRbm->pBuf));
#ifdef RING_BUF_DEBUG
	if (check_error(pRbm, __func__, __LINE__) != 0)
		readRes = -1;
#endif

	pthread_mutex_unlock(&pRbm->lock);
	return readRes;
}

int64_t _rbm_seekCur(RingBufferMgr_t *pRbm, int64_t offset)
{
	uint8_t *pTarget;
	uint8_t *pr, *pw, *pe;
	long dataSz, dirtySz;
	int res = 0;

	pr = pRbm->pRPos;
	pw = pRbm->pWPos;
	pe = pRbm->pBuf + pRbm->bufSize;
	dataSz = pRbm->dataSize;
	dirtySz = pRbm->dirtySize;

	int64_t tmp = offset;
	while (tmp != 0) {
		long seek_len;
		pTarget = pr + tmp;
		if (pw > pr) {
			if (pTarget < pRbm->pBuf) {
				seek_len = pRbm->pBuf - pr;
			} else if (pTarget <= pw) {
				seek_len = tmp;
			} else {
				OSCL_LOGD("");
				res = -1;
				break;
			}
		} else if (pw < pr) {
			if (pTarget < pw) {
				OSCL_LOGD("");
				res = -1;
				break;
			} else if (pTarget <= pe) {
				seek_len = tmp;
			} else {
				seek_len = pe - pr;
			}
		} else {
			if (dataSz == 0 && dirtySz == 0) {
				OSCL_LOGD("buffer has no data yet!");
				res = -1;
				break;
			}
			if (dataSz > 0) {
				OSCL_LOGD("buffer full!");
				if (pTarget < pw) {
					res = -1;
					break;
				} else {
					seek_len = 1;
				}
			} else {
				OSCL_LOGD("buffer empty!");
				if (pTarget > pw) {
					res = -1;
					break;
				} else {
					seek_len = -1;
				}
			}
		}
		OSCL_LOGD("seek %ld, epos=%ld, rpos=%ld, wpos=%ld", seek_len,
			pe-pRbm->pBuf, pr-pRbm->pBuf, pw-pRbm->pBuf);
		if (tmp > 0) {
			if (pr >= pe)
				pr = pRbm->pBuf;
		} else {
			if (pr <= pRbm->pBuf) {
				if (dirtySz <= 0) {
					OSCL_LOGD("");
					res = -1;
					break;
				}
				pr = pe;
			}
		}
		tmp -= seek_len;
		dirtySz += seek_len;
		dataSz -= seek_len;
		pr += seek_len;
	}
	if (res != 0)
		return -1;

	pRbm->dataSize = dataSz;
	pRbm->dirtySize = dirtySz;
	pRbm->curOffset += offset;
	pRbm->pRPos = pr;
	if (pRbm->pRPos >= pe)
		pRbm->pRPos = pRbm->pBuf;
	if (pRbm->curOffset < 0)
		OSCL_LOGE("fatal error! curpos=%lld", pRbm->curOffset);

	return pRbm->curOffset;
}

int64_t rbm_seek(void *s, int64_t offset, int whence)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	RingBufferMgr_t *pRbm = (RingBufferMgr_t *)s;
	int64_t ret = -1;

	pthread_mutex_lock(&pRbm->lock);
	OSCL_LOGD("seeking %lld offset, dataSize=%ld, dirtySize=%ld",
		offset, pRbm->dataSize, pRbm->dirtySize);
	switch (whence) {
	case SEEK_CUR: {
		ret = _rbm_seekCur(pRbm, offset);
		break;
	}
	case SEEK_SET: {
		ret = _rbm_seekCur(pRbm, offset - pRbm->curOffset);
		break;
	}
	case SEEK_END:
		OSCL_LOGE("SEEK_END is not support!");
		break;
	default:
		OSCL_LOGE("unknown seek command %d!!", whence);
		break;
	}
	if (ret < 0) {
		OSCL_LOGD("seek %lld err, whence=%d, curpos=%lld!",
			offset, whence, pRbm->curOffset);
		OSCL_LOGD("buf=(%p,%ld), rpos=%p, wpos=%p, pe=%p, data=%ld, dirty=%ld",
			pRbm->pBuf, pRbm->bufSize,
			pRbm->pRPos, pRbm->pWPos, pRbm->pBuf + pRbm->bufSize,
			pRbm->dataSize, pRbm->dirtySize);
	} else {
		OSCL_LOGD("seek %lld success, whence=%d.", offset, whence);
	}
#ifdef RING_BUF_DEBUG
	if (check_error(pRbm, __func__, __LINE__) != 0)
		ret = -1;
#endif

	pthread_mutex_unlock(&pRbm->lock);
	return ret;
}

int64_t rbm_tell(void *s)
{
	if (s == NULL) {
		OSCL_LOGE("handle==NULL!");
		return -1;
	}
	RingBufferMgr_t *pRbm = (RingBufferMgr_t *)s;
	int64_t ret = -1;

	pthread_mutex_lock(&pRbm->lock);
	ret = pRbm->curOffset;
	pthread_mutex_unlock(&pRbm->lock);
	OSCL_LOGD("current offset %lld", ret);

	return ret;
}
