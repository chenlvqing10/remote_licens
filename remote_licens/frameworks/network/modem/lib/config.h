#ifndef _MODEM_AT_CONFIG_H
#define _MODEM_AT_CONFIG_H

#if __cplusplus
extern "C" {
#endif

int at_get_tty_path(char *path, int *len);

#if __cplusplus
};  /* extern "C" */
#endif

#endif /* _MODEM_AT_CONFIG_H */
