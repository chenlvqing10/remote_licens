#include "connect.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lb_os_http.h"

#define LOG_TAG "connect"
#include <log/log.h>

/**
 * init_connection: get ip information by host name and port
 * hostname: cloud server host name
 * port: cloud server port string
 * res: ip information
 *
 * Returns 0 means success, other means failure.
 */
int init_connection(char *hostname, char *port, struct addrinfo **res)
{
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	return getaddrinfo(hostname, port, &hints, res);
}

/**
 * make_connection: connect by ip information
 * res: ip information
 *
 * Returns -1 means failure, other means socket handle.
 */
int make_connection(struct addrinfo *res)
{
	int sockfd, status;
	struct timeval timeout = {5, 0};

	sockfd = socket(res->ai_family,
		res->ai_socktype, res->ai_protocol);
	jump_unless(sockfd > 0);

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
		&timeout, sizeof(struct timeval));

	status = connect(sockfd, res->ai_addr, res->ai_addrlen);
	jump_unless(status == 0);

	return sockfd;

error:
	if (sockfd >= 0)
		close(sockfd);

	return -1;
}

/**
 * build_request: create a http POST requst
 * host: host name or ip address
 * request_path: request path
 * snd_body: message string
 * body_len: message length
 *
 * Returns cmplete http POST request string.
 */
char *build_request(char *host, char *request_path, char *snd_body, long body_len)
{
	char *request = NULL;
	buffer_t *request_buffer = buffer_alloc(BUF_SIZE);

	buffer_appendf(request_buffer, "POST %s HTTP/1.1\r\n", request_path);
	buffer_appendf(request_buffer, "Host: %s\r\n", host);
	buffer_appendf(request_buffer, "Content-Length: %ld\r\n", body_len);
#if 0
	buffer_appendf(request_buffer,
		"Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n");
#else
	buffer_appendf(request_buffer,
		"Content-Type: application/json\r\n");
#endif
	buffer_appendf(request_buffer, "\r\n");
	buffer_appendf(request_buffer, snd_body);

	request = buffer_to_s(request_buffer);
	buffer_free(request_buffer);

	return request;
}

/**
 * make_request: send a http POST requst to cloud server
 * sockfd: socket handle
 * host: host name or ip address
 * request_path: request path
 * snd_body: message string
 * body_len: message length
 *
 * Returns length of having been sent data.
 */
int make_request(int sockfd, char *host, char *request_path,
				char *snd_body, long body_len)
{
	size_t bytes_sent = 0;
	size_t total_bytes_sent = 0;
	char *request = build_request(host, request_path, snd_body, body_len);
	size_t total_bytes_to_send = strlen(request);
	size_t bytes_to_send = strlen(request);

	ALOGD("all Bytes to send: %d\n", bytes_to_send);

	while (1) {
		bytes_sent = send(sockfd, request+total_bytes_sent, bytes_to_send, 0);
		total_bytes_sent += bytes_sent;
		bytes_to_send -= bytes_sent;

		ALOGD("Bytes sent: %d", bytes_sent);

		if ((total_bytes_sent >= total_bytes_to_send)
			|| (bytes_to_send <= 0))
			break;
	}
	lb_mem_free(request);

	return total_bytes_sent;
}

/**
 * fetch_response: recv response of http POST requst
 * sockfd: socket handle
 * response: a pointer of store that will receive data buffer
 * recv_size: data buffer length
 *
 * Returns 0 means success, other means failure.
 */
int fetch_response(int sockfd, buffer_t **response, int recv_size)
{
	size_t bytes_received;
	int status = 0;
	char data[recv_size];

	ALOGD("Receiving data...\n");

	while (1) {
		memset(data, 0, sizeof(data));
		bytes_received = recv(sockfd, data, RECV_SIZE, 0);

		if (bytes_received == -1) {
			ALOGD("timeout bytes_received = -1\n");
			return -1;
		} else if (bytes_received == 0) {
			ALOGD("timeout bytes_received = 0\n");
			return 0;
		}

		if (bytes_received > 0) {
			status = buffer_append(*response, data, bytes_received);
			if (status != 0) {
				ALOGD("Failed to append to buffer.\n");
				return -1;
			}
		}
	}
	ALOGD("Finished receiving data.\n");
	return status;
}

