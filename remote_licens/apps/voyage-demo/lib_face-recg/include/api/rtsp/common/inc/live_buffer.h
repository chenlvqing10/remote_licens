#ifndef _LIVE_BUFFER_H_
#define _LIVE_BUFFER_H_
#include "media_common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct sfifo_list_des_s {
	int sfifo_num;

	struct sfifo_s *head;
	struct sfifo_s *tail;

	pthread_mutex_t lock_mutex;
	pthread_cond_t cond;
};

struct sfifo_des_s {
	int sfifo_init;

	unsigned int sfifos_num;
	unsigned int sfifos_active_max_num;

	struct sfifo_list_des_s free_list;
	struct sfifo_list_des_s active_list;
};

#if 0
typedef enum live_stream_type {
	STREAM_TYPE_UNKNOWN = 0,
	STREAM_TYPE_AUDIO,
	STREAM_TYPE_VIDEO,
} live_stream_type_e;
#endif

struct sfifo_s {
	stream_type_e type;
	unsigned char *buffer;
	unsigned int size;
	int64_t timestamp;
	struct sfifo_s *next;
};

int save_data_to_buf(unsigned char *buffer, unsigned int size, stream_type_e type);

int get_data_from_buf(unsigned char *buffer, unsigned int *size, stream_type_e type);

struct sfifo_des_s *video_buffer_init();

int video_buffer_deinit();

extern struct sfifo_des_s *sfifo_init(int sfifo_num, int sfifo_buffer_size, int sfifo_active_max_num);

/* productor */
extern struct sfifo_s* sfifo_get_free_buf(struct sfifo_des_s *sfifo_des_p);
extern int sfifo_put_free_buf(struct sfifo_s *sfifo, struct sfifo_des_s *sfifo_des_p);

/* consumer */
extern struct sfifo_s* sfifo_get_active_buf(struct sfifo_des_s *sfifo_des_p);
extern int sfifo_put_active_buf(struct sfifo_s *sfifo, struct sfifo_des_s *sfifo_des_p);

#ifdef __cplusplus
}
#endif

#endif /* _LIVE_BUFFER_H_ */
