#include <stdlib.h>
#include <stdio.h>

#include "connect.h"

#include "dbg.h"
#include "buffer.h"

#define LOG_TAG "http"
#include <log/log.h>

typedef struct _url {
	char host[64]; /* cloud server host name or ip address*/
	char port[64]; /* cloud server port */
	char path[128]; /* cloud server path */
} url_t;

/**
 * url_parse: parse url string
 * str_url: url string
 * url:  pointer of store url parse data
 *
 * Returns 0 means success, other means failure.
 */
int url_parse(char *str_url, url_t *url)
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;

	p1 = strstr(str_url, "://");
	if (!p1) {
		ALOGD("NO input url host.\n");
		return -1;
	}
	p2 = strstr(p1+3, ":");
	p3 = strstr(p1+3, "/");

	if (!p3) {
		ALOGD("NO input url path.\n");
		return -1;
	}

	ALOGD("sizeof url.path=%d\n", sizeof(url->path));
	strncpy(url->path, p3, sizeof(url->path));

	if (p2) {
		strncpy(url->port, (p2+1), (p3-(p2+1)));
		strncpy(url->host, (p1+3), (p2-(p1+3)));
	} else {
		strncpy(url->port, "80", sizeof(url->port));
		strncpy(url->host, (p1+3), (p3-(p1+3)));
	}

	return 0;
}

/**
 * send_request: send request data to the cloud server
 * str_url: url of cloud server
 * snd_body: message data of requesting
 * body_len: message data len
 * recv_deal: callback pointer of processing received data
 *
 * Returns 0 means success, other means failure.
 */
int send_request(char *str_url, char *snd_body, long body_len,
			int (*recv_deal)(int sockfd, char *msg, long len))
{
	buffer_t *response = buffer_alloc(BUF_SIZE);
	url_t url;

	int ret = 0;
	int status = 0;
	int sockfd = 0;
	struct addrinfo *res = NULL;

	if (NULL == str_url || NULL == snd_body)
		return -1;

	if (strlen(str_url) <= 0)
		return -2;

	memset(&url, 0, sizeof(url));
	ret = url_parse(str_url, &url);
	if (ret != 0) {
		printf("Url parse fail.\n");
		return -3;
	}
	ALOGD("host=%s, port=%s, path=%s\n", url.host, url.port, url.path);

	status = init_connection(url.host, url.port, &res);
	ALOGD("status = %d\n", status);
	if (status)
		return -4;
	sockfd = make_connection(res);
	if (sockfd <= 0) {
		ALOGD("Could not make connection to %s on port %s\n",
			url.host, url.port);
		return -1;
	}
	ALOGD("sockfd = %d\n", sockfd);
	status = make_request(sockfd, url.host, url.path, snd_body, body_len);
	if (status <= 0)
		ALOGD("[send fail]send request len = %d\n", status);

	status = fetch_response(sockfd, &response, RECV_SIZE);
	ALOGD("fetch_response ret = %d\n", status);
	ALOGD("fetch_response strlen(response contents) = %d\n",
		strlen(response->contents));

	ALOGD("<<<start---\n");
	ALOGD("---end>>>\n");

	if (recv_deal)
		(*recv_deal)(sockfd, response->contents,
				strlen(response->contents));

	close(sockfd);

	freeaddrinfo(res);
	buffer_free(response);
	return 0;

	if (sockfd > 0)
		close(sockfd);
	if (res != NULL)
		freeaddrinfo(res);

	buffer_free(response);

	return 1;
}

static unsigned char char_to_hex(unsigned char x)
{
	return (unsigned char)(x > 9 ? x + 55: x + 48);
}

static int is_alpha_number_char(unsigned char c)
{
	if ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z')
			|| (c >= '0' && c <= '9'))
		return 1;

	return 0;
}

/** url encode */
void urlencode(unsigned char * src, int  src_len,
		unsigned char * dest, int  dest_len)
{
	unsigned char ch;
	int  len = 0;

	while (len < (dest_len - 4) && *src)
	{
		ch = (unsigned char)*src;
		if (*src == ' ')
			*dest++ = '+';
		else if (is_alpha_number_char(ch) || strchr("-_.!~*'()", ch))
			*dest++ = *src;
		else {
			*dest++ = '%';
			*dest++ = char_to_hex((unsigned char)(ch >> 4));
			*dest++ = char_to_hex((unsigned char)(ch % 16));
		} 
		++src;
		++len;
	}
	*dest = 0;
	return ;
}

/** decode url encode */
unsigned char* urldecode(unsigned char* encd,unsigned char* decd)
{
	int j,i;
	char *cd = (char *)encd;
	char p[2];

	j = 0;

	for (i = 0; i < strlen(cd); i++) {
		memset(p, 0, 2);
		if (cd[i] != '%') {
			decd[j++] = cd[i];
			continue;
		}

		p[0] = cd[++i];
		p[1] = cd[++i];

		p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0)
			- ((p[0] >= 'a') ? 32 : 0);
		p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0)
			- ((p[1] >= 'a') ? 32 : 0);
		decd[j++] = (unsigned char)(p[0] * 16 + p[1]);
	}
	decd[j] = 0;

	return decd;
}

