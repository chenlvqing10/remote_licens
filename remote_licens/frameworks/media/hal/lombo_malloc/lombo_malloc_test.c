#include "oscl_debug.h"
#include "oscl_time.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "lombo_malloc.h"
#include "lombo_cycle_buf.h"
#include "lombo_cycle_buf_priv.h"

#define ALLOC_UNIT 1024

int main(int argc, char *argv[])
{
	int i;
	void *buf[10];

	for (i = 0; i < 10; i++)
		buf[i] = lombo_malloc_cached(i*ALLOC_UNIT);

	lombo_malloc_dump();

	for (i = 0; i < 10; i++)
		lombo_free_cached(buf[i]);

	lombo_malloc_dump();

	for (i = 0; i < 10; i++)
		buf[i] = lombo_malloc_unca(i*ALLOC_UNIT);

	lombo_malloc_dump();

	for (i = 0; i < 10; i++)
		lombo_free_unca(buf[i]);

	lombo_malloc_dump();
	return 0;
}

void *buf[1024];

#define IS_CBUF_VALID(hdl) ((hdl != NULL) && (hdl->buf != NULL) && (hdl->total_len))
#define IS_CBUF_OFFSET_VALID(hdl) \
	((((hdl)->beg_ofs + (hdl)->date_len)%(hdl->total_len)) == hdl->end_ofs)

static int check_cycle_buffer(void *handle)
{
	_cbuf_node_t *tmp_node = NULL;
	_cbuf_node_t *next_node = NULL;
	_cbuf_node_t *tail_node = NULL;
	_cbuf_handle_t *cbuf = handle;
	char *end_addr;

	int index = 0;
	int n_node_heap = 0;
	int n_node = 0;
	int ret = 0;
	int n_node_padding = 0;
	oscl_param_check_exit(cbuf, -1, NULL);

	pthread_mutex_lock(&cbuf->mutex);
	oscl_param_check_exit(IS_CBUF_VALID(cbuf), -1, NULL);
	oscl_param_check_exit(IS_CBUF_OFFSET_VALID(cbuf), -1, NULL);

	end_addr = cbuf->buf + cbuf->total_len;
	tmp_node = cbuf->node_manager.node_head;
	tail_node = tmp_node;
	while (tmp_node) {
		tail_node = tmp_node;
		next_node = tmp_node->next_node;
		if (next_node && (tmp_node->addr + tmp_node->len != next_node->addr) &&
			(tmp_node->addr + tmp_node->len != end_addr)) {
			oscl_param_check_exit(1, -1, NULL);
		}
		if (tmp_node->node_stat == NODE_PADDING)
			n_node_padding++;
		if (tmp_node->addr + tmp_node->len != end_addr) {
			if (next_node && tmp_node->node_stat == NODE_PADDING
				 && next_node->node_stat != NODE_PADDING) {
				int padding_size = tmp_node->len;
				oscl_param_check_exit(
					next_node->len > padding_size, -1, NULL);
			}
		}
		index++;
		if (tmp_node->from_heap)
			n_node_heap++;
		tmp_node = tmp_node->next_node;
	}
	oscl_param_check_exit(index == cbuf->node_manager.ntotal, -1, NULL);
	oscl_param_check_exit(n_node_heap == cbuf->node_manager.nfrom_heap, -1, NULL);
	oscl_param_check_exit(tail_node == cbuf->node_manager.node_tail, -1, NULL);
	for (index = 0; index < MAX_CBUF_NODE_BUF_NUM; index++) {
		if (cbuf->node_manager.buf_node[index].node_stat != NODE_FREE)
			n_node++;
	}
	oscl_param_check_exit(
		n_node == cbuf->node_manager.ntotal - n_node_heap, -1, NULL);
	oscl_param_check_exit(n_node_padding <= 2, -1, NULL);

EXIT:
	if (ret < 0) {
		OSCL_LOGE("cycle buffer check err!");
		print_cbuf_info(handle, __LINE__);
	}
	pthread_mutex_unlock(&cbuf->mutex);
	return ret;
}

static void *free_mem_thread(void *param)
{
	int i;
	void *cbuf_hdle = param;
	int ret;

	oscl_mdelay(3000);
	OSCL_LOGE("========free_mem_thread start:%p========", cbuf_hdle);
	for (i = 0; i < 1024; i++) {
		OSCL_LOGE("cycle_buffer_free buf[%d]:%p!", i, buf[i]);
		cycle_buffer_free(cbuf_hdle, buf[i]);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
		oscl_mdelay(1000);
		if (i > 1 && buf[i] == NULL && buf[i - 1])
			break;
	}
	OSCL_LOGE("free_mem_thread quit!");
	return NULL;
}
void print_all_buf(void)
{
	int i;
	i = 0;
	while (i < 1024 && (buf[i++] == NULL))
		continue;
	for (; i < 1024; i += 8) {
		if (buf[i] == NULL)
			break;
		OSCL_LOGE("index %d: %8p %8p %8p %8p %8p %8p %8p %8p",
			i, buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4],
			buf[i+5], buf[i+6], buf[i+7]);
	}
}

#define __RANDOM(min, max) (min + rand()%(max - min))

static void *free_mem_rand(void *param)
{
	void *cbuf_hdle = param;
	int ret;
	int delay_ms;
	int index = 0;
	int count = 0;

	OSCL_LOGE("========malloc_mem_rand start:%p========", cbuf_hdle);
	while (1) {
		delay_ms = __RANDOM(1, 7)*10;
		count = 100;
		while (buf[index] == NULL && count--)
			oscl_mdelay(100);
		if (buf[index] == NULL) {
			OSCL_LOGE("buf[%d] is %p, quit!\n", index, buf[index]);
			break;
		}
		cycle_buffer_free(cbuf_hdle, buf[index]);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****cycle buffer check err!\n\n");
		OSCL_LOGE("free %d addr:%p!\n", index, buf[index]);
		buf[index] = NULL;
		index++;
		index = index % 1024;
		oscl_mdelay(delay_ms);
	}
	OSCL_LOGE("free_mem_thread quit!");
	return NULL;
}


static void *malloc_mem_rand(void *param)
{
	int i;
	void *cbuf_hdle = param;
	int ret;
	int size, delay_ms = 0;
	int index = 0;
	int count = 0;
	void *tmp_buffer;

	OSCL_LOGE("========malloc_mem_rand start:%p========", cbuf_hdle);
	for (i = 0; i < 50; i++) {
		size = __RANDOM(1, 10)*1024;

		buf[index] = cycle_buffer_malloc(cbuf_hdle, size);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****cycle buffer check err!\n\n");
		if (buf[index] != NULL) {
			index++;
			index = index % 1024;
		} else
			break;
	}

	for (i = 0; i < 5*1024; i++) {
		delay_ms = __RANDOM(1, 7)*10;
		size = __RANDOM(1, 600)*1024;
		count = 100;
		while (buf[index] && count--)
			oscl_mdelay(100);
		if (buf[index] != NULL) {
			OSCL_LOGE("buf[%d]:%p is not null, quit!\n", index, buf[index]);
			break;
		}

		tmp_buffer = cycle_buffer_malloc(cbuf_hdle, size);
		OSCL_LOGE("alloc %i, size:%d buf[%d]:%p!\n", i, size, index, tmp_buffer);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****cycle buffer check err!\n\n");
		if (delay_ms > 40) {
			size = __RANDOM(2, 800)*1024;
			tmp_buffer = cycle_buffer_realloc(cbuf_hdle, tmp_buffer, size);
			OSCL_LOGE("realloc %i, %d buf[%d]:%p!\n",
				i, size, index, tmp_buffer);
		}
		if (tmp_buffer != NULL) {
			buf[index] = tmp_buffer;
			index++;
			index = index % 1024;
		}
		oscl_mdelay(delay_ms);
	}
	OSCL_LOGE("free_mem_thread quit!");
	return NULL;
}



int main_cycle(int argc, char *argv[])
{
	int i;
	int ret;
	int start_index = 0;
	int end_index = 0;
	int num = 0;
	void *cbuf_hdle;
	void *buf_realloc;

	cbuf_hdle = cycle_buffer_creat("test", 4*1024*1024, 4096);
	if (cbuf_hdle == NULL)
		OSCL_LOGE("creat cbuffer failed!");
	cycle_buffer_set_block(cbuf_hdle, 0);

	/* malloc and reset */
	for (i = 0; i < 10; i++) {
		buf[i] = cycle_buffer_malloc(cbuf_hdle, 1);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
	}
	cycle_buffer_reset(cbuf_hdle);
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");

	/* malloc and free */
	cycle_buffer_set_block(cbuf_hdle, 0);
	memset(buf, 0, sizeof(buf));
	start_index = end_index = 0;
	for (i = 0; i < 1024; i++) {
		buf[end_index] = cycle_buffer_malloc(cbuf_hdle, 256*1024);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
		if (buf[end_index] == NULL)
			break;
		end_index++;
		num++;
	}
	OSCL_LOGE("===malloc %d bufffer ,size:%d=====", i, 256*1024);
	print_all_buf();

	for (i = 0; i < num / 2; i++) {
		cycle_buffer_free(cbuf_hdle, buf[start_index]);
		buf[start_index] = NULL;
		start_index++;
		num--;
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
	}
	OSCL_LOGE("===free bufffer =====");
	print_all_buf();

	for (i = end_index; i < 1024; i++) {
		buf[end_index] = cycle_buffer_malloc(cbuf_hdle, 256*1024);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
		if (buf[end_index] == NULL)
			break;
		end_index++;
		num++;
	}
	OSCL_LOGE("===malloc %d bufffer ,size:%d=====", num, 256*1024);
	print_all_buf();

	for (i = 0; i < num - 1; i++) {
		cycle_buffer_free(cbuf_hdle, buf[start_index]);
		buf[start_index] = NULL;
		start_index++;
		num--;
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
	}

	OSCL_LOGE("===malloc %d bufffer=====", num);
	print_all_buf();

	for (i = end_index; i < 1024; i++) {
		buf[end_index] = cycle_buffer_malloc(cbuf_hdle, 512*1024);
		ret = check_cycle_buffer(cbuf_hdle);
		if (ret != 0)
			OSCL_LOGE("****index:%d, cycle buffer check err!\n\n", i);
		if (buf[end_index] == NULL)
			break;
		end_index++;
		num++;
	}
	OSCL_LOGE("===malloc %d bufffer ,size:%d=====", num, 256*1024);
	print_all_buf();

	/* test for remalloc */
	cycle_buffer_reset(cbuf_hdle);
	memset(buf, 0, sizeof(buf));
	/* malloc and remalloc */
	buf[0] = cycle_buffer_malloc(cbuf_hdle, 2*1024*1024);
	buf[1] = cycle_buffer_malloc(cbuf_hdle, 128*1024);
	buf[2] = cycle_buffer_malloc(cbuf_hdle, 256*1024);
	cycle_buffer_free(cbuf_hdle, buf[0]);
	buf[0] = NULL;
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");

	buf_realloc = cycle_buffer_realloc(cbuf_hdle, buf[2], 512*1024);
	if (buf_realloc != buf[2])
		OSCL_LOGE("********cycle_buffer_realloc err! check it*******\n\n");
	buf[2] = buf_realloc;
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");
	OSCL_LOGE("buf[0]:%8p buf[1]:%8p buf[2]:%8p", buf[0], buf[1], buf[2]);

	buf_realloc = cycle_buffer_realloc(cbuf_hdle, buf[2], 1024*1024);
	if (buf_realloc != buf[2])
		OSCL_LOGE("********cycle_buffer_realloc err! check it*******\n\n");
	buf[2] = buf_realloc;
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");
	OSCL_LOGE("buf[0]:%8p buf[1]:%8p buf[2]:%8p", buf[0], buf[1], buf[2]);

	buf_realloc = cycle_buffer_realloc(cbuf_hdle, buf[2], 128*1024);
	if (buf_realloc != buf[2])
		OSCL_LOGE("********cycle_buffer_realloc err! check it*******\n\n");
	buf[2] = buf_realloc;
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");
	OSCL_LOGE("buf[0]:%8p buf[1]:%8p buf[2]:%8p", buf[0], buf[1], buf[2]);

	buf_realloc = cycle_buffer_realloc(cbuf_hdle, buf[2], 2*1024*1024);
	if (buf_realloc == buf[2])
		OSCL_LOGE("********cycle_buffer_realloc err! check it*******\n\n");
	buf[2] = buf_realloc;
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");
	OSCL_LOGE("buf[0]:%8p buf[1]:%8p buf[2]:%8p", buf[0], buf[1], buf[2]);


	buf_realloc = cycle_buffer_realloc(cbuf_hdle, buf[2], 1*1024*1024);
	if (buf_realloc != buf[2])
		OSCL_LOGE("********cycle_buffer_realloc err! check it*******\n\n");
	buf[2] = buf_realloc;
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");
	OSCL_LOGE("buf[0]:%8p buf[1]:%8p buf[2]:%8p", buf[0], buf[1], buf[2]);


	OSCL_LOGE("test async start...");
	pthread_t thread_id = 0;
	pthread_create(&thread_id,
			NULL,
			free_mem_thread,
			(void *)cbuf_hdle);

	buf[2] = cycle_buffer_realloc(cbuf_hdle, buf[2], 3*1024*1024);
	ret = check_cycle_buffer(cbuf_hdle);
	if (ret != 0)
		OSCL_LOGE("****cycle buffer check err!\n\n");
	OSCL_LOGE("buf[0]:%8p buf[1]:%8p buf[2]:%8p", buf[0], buf[1], buf[2]);
	pthread_join(thread_id, NULL);

	cycle_buffer_reset(cbuf_hdle);
	memset(buf, 0, sizeof(buf));
	srand((unsigned int)time(NULL));
	OSCL_LOGE("test async start1...");
	pthread_t thread_id_rand0 = 0;
	pthread_t thread_id_rand1 = 0;
	pthread_create(&thread_id_rand0,
			NULL,
			free_mem_rand,
			(void *)cbuf_hdle);
	pthread_create(&thread_id_rand1,
			NULL,
			malloc_mem_rand,
			(void *)cbuf_hdle);
	pthread_join(thread_id_rand0, NULL);
	pthread_join(thread_id_rand1, NULL);

	cycle_buffer_destory(cbuf_hdle);
	return 0;
}

