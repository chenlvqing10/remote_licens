
#ifndef __ONVIF_FACE_H__
#define __ONVIF_FACE_H__

int onvif_set_timezone(char *TZ);

const char *onvif_get_timezone(int index);

int onvif_get_time_value(char *TZ);

int onvif_fault(struct soap *soap, char *value1, char *value2);
int isValidHostname(char *str);

#endif /* __ONVIF_FACE_H__ */

