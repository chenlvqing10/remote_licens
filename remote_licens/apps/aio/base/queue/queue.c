#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "queue.h"

/**
 * @brief  join the queue
 * @param  *head: pointer the queue
 * @param  *buf: data address
 * @retval 0 if success
 */
int en_queue(head_t *head, void *buf)
{
	if (!head)
		return -1;

	node_t *e = malloc(sizeof(node_t));
	if (!e) {
		printf("malloc failed, errno: %d (%s)", errno, strerror(errno));
		return errno;
	}
	e->buf = buf;
	TAILQ_INSERT_TAIL(head, e, nodes);

	return 0;
}

/**
 * @brief  join the queue
 * @param  *head: pointer the queue
 * @param  *buf: data address
 * @retval 0 if success
 */
int en_queue_head(head_t *head, void *buf)
{
	if (!head)
		return -1;

	node_t *e = malloc(sizeof(node_t));
	if (!e) {
		printf("malloc failed, errno: %d (%s)", errno, strerror(errno));
		return errno;
	}
	e->buf = buf;
	TAILQ_INSERT_HEAD(head, e, nodes);

	return 0;
}

/**
 * @brief  out of th queue
 * @param  *head: pointer the queue
 * @retval None
 */
void *de_queue(head_t *head)
{
	void *retval = NULL;

	if (!head)
		return NULL;

	if (TAILQ_EMPTY(head))
		return NULL;

	node_t *e = TAILQ_FIRST(head);
	TAILQ_REMOVE(head, e, nodes);
	retval = e->buf;
	free(e);
	e = NULL;

	return retval;
}

/**
 * @brief  the release of the queue
 * @param  *head: pointer the queue
 * @retval None
 */
void free_queue(head_t *head)
{
	node_t *e = NULL;
	while (!TAILQ_EMPTY(head)) {
		e = TAILQ_FIRST(head);
		TAILQ_REMOVE(head, e, nodes);
		free(e);
		e = NULL;
	}
}

/**
 * @brief  determine that the queue is empty
 * @param  *head: pointer the queue
 * @retval
 */
int empty_queue(head_t *head)
{
	return TAILQ_EMPTY(head);
}

/**
 * @brief  queue size
 * @param  *head: pointer the queue
 * @retval size
 */
int queue_size(head_t *head)
{
	node_t *e = NULL;
	int size = 0;
	TAILQ_FOREACH(e, head, nodes) {
		size++;
	}
	return size;
}

/**
 * @brief  remove queue
 * @param  *head: pointer the queue
 * @param  *e:
 * @retval None
 */
void queue_remove(head_t *head, node_t *e)
{
	TAILQ_REMOVE(head, e, nodes);
	free(e);
	e = NULL;
}
