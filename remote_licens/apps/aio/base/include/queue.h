#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <sys/queue.h>

typedef struct node {
	void *buf;
	TAILQ_ENTRY(node) nodes;
} node_t;

typedef TAILQ_HEAD(head_s, node) head_t;

typedef struct list_head list_head_t;

/**
 * @brief  join the queue
 * @param  *head: pointer the queue
 * @param  *buf: data address
 * @retval 0 if success
 */
int en_queue(head_t *head, void *buf);

/**
 * @brief  join the queue
 * @param  *head: pointer the queue
 * @param  *buf: data address
 * @retval 0 if success
 */
int en_queue_head(head_t *head, void *buf);

/**
 * @brief  out of th queue
 * @param  *head: pointer the queue
 * @retval None
 */
void *de_queue(head_t *head);

/**
 * @brief  the release of the queue
 * @param  *head: pointer the queue
 * @retval None
 */
void free_queue(head_t *head);

/**
 * @brief  determine that the queue is empty
 * @param  *head: pointer the queue
 * @retval None
 */
int empty_queue(head_t *head);

/**
 * @brief  queue size
 * @param  *head: pointer the queue
 * @retval size
 */
int queue_size(head_t *head);

/**
 * @brief  remove queue
 * @param  *head: pointer the queue
 * @retval None
 */
void queue_remove(head_t *head, node_t *e);

#endif /* _QUEUE_H_ */
