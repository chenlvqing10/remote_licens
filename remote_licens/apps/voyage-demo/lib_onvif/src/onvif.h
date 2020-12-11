#ifndef __ONVIF_H__
#define __ONVIF_H__

#include <stdio.h>

#define ONVIF_OK 0

#define ETH_NAME "eth0"
#define ETH_NAME1 "wlan0"
#define LINK_LOCAL_ADDR "192.168.39.249"

#define _DBUG()

#define DEFAULT_ONVIF_WSD_PORT 3702

#ifdef DEBUG_ONVIF
#define __D(fmt, args...) \
	fprintf(stderr, "ONVIF DEBUG(%s:%d): " \
	fmt, __func__, __LINE__, ## args)
#define __FUN_BEGIN(fmt, args...) \
	fprintf(stderr, "ONVIF FUNCTION BEGIN(%s, %d): " \
	fmt, __func__, __LINE__, ##args);
#define __FUN_END(fmt, args...) \
	fprintf(stderr, "ONVIF FUNCTION END(%s, %d): " \
	fmt, __func__, __LINE__, ##args);
#else
#define __D(fmt, args...)
#define __FUN_BEGIN(fmt, args...)
#define __FUN_END(fmt, args...)
#endif

#define __E(fmt, args...) \
	fprintf(stderr, "ONVIF FUNCTION END(%s, %d): " \
	fmt, __func__, __LINE__, ##args);

#define ALLOC_TOKEN(val, token) { \
	val = (char *) soap_malloc(soap, sizeof(char)*ONVIF_TOKEN_LEN); \
	if(val == NULL) { \
		printf("malloc err\r\n"); \
		return SOAP_FAULT; \
	} \
	memset(val, 0, sizeof(char)*ONVIF_TOKEN_LEN); \
	strcpy(val, token); \
	}

int _dn_Bye_send(struct soap* soap);


#endif /* __ONVIF_H__ */

