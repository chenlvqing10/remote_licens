#ifndef _WIFI_SOFTAP_H
#define _WIFI_SOFTAP_H

#if __cplusplus
extern "C" {
#endif

#define SSID_MAX_LENGTH 32
#define PASSWD_MAX_LENGTH 32

typedef enum {
	WIFI_OPEN = 0,
	WIFI_WPA_PSK,
	WIFI_WPA2_PSK,
} AP_SECURITY;

typedef struct {
	char *ssid;
	char *passwd;
	AP_SECURITY security;
} AP_CONFIG;

typedef struct {
	char ssid[SSID_MAX_LENGTH+1];
	char passwd[PASSWD_MAX_LENGTH+1];
	AP_SECURITY security;
} AP_INFO;

int softap_set_config(AP_CONFIG *config);
int softap_get_info(AP_INFO *info);
int softap_enable(void);
int softap_disable(void);

#if __cplusplus
};
/* extern "C" */
#endif

#endif
