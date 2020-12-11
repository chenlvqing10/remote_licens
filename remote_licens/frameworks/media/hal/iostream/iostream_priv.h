#ifndef __IOSTREAM_PRIV_H__
#define __IOSTREAM_PRIV_H__

#include <stdlib.h>
#include <stdint.h>

#include <av_media_type.h>
#include <demuxer_plugin.h>

#define IOSTREAM_VERSION        "1.1"

typedef struct IOStreamData {
	uint8_t *data;
	size_t size;
} IOStreamData_t;

typedef struct iostream_packet {
	void *vir_addr;
	unsigned long phy_addr;
	size_t data_size; /* data size in buffer */
	int64_t timestamp; /* timestamp in us */
	av_media_type_e media_type;
} iostream_packet_t;

typedef dmx_packet_t stream_packet_t;

#endif /* __IOSTREAM_PRIV_H__ */

