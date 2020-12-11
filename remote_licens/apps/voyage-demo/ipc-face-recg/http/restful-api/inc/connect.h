#ifndef CONNECT_H
#define CONNECT_H

#include <netdb.h>
#include <unistd.h>

#include "buffer.h"
#include "dbg.h"

#define RECV_SIZE (1024)
#define BUF_SIZE  (RECV_SIZE + 1)

int init_connection(char *hostname, char *port, struct addrinfo **res);
int make_connection(struct addrinfo *res);
int make_request(int sockfd, char *host, char *request_path,
		char *snd_body, long body_len);
int fetch_response(int sockfd, buffer_t **response, int recv_size);

#endif
