/*
 * MD5 hash implementation and interface functions
 * Copyright (c) 2003-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef DD_MD5_H
#define DD_MD5_H

#define MD5_MAC_LEN 16

int hmac_md5_vector(const unsigned char *key, unsigned int key_len, unsigned int num_elem,
		const unsigned char *addr[], const unsigned int *len, unsigned char *mac);
/**
 * hmac_md5_vector - HMAC-MD5 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (16 bytes)
 * Returns: 0 on success, -1 on failure
 */

int hmac_md5(const unsigned char *key, unsigned int key_len, const unsigned char *data, 
			unsigned int data_len, unsigned char *mac);
/**
 * hmac_md5 - HMAC-MD5 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (16 bytes)
 * Returns: 0 on success, -1 on failure
 */
#endif /* DD_MD5_H */

