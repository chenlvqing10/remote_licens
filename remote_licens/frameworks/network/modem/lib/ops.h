#ifndef _MODEM_OPS_CONFIG_H
#define _MODEM_OPS_CONFIG_H

#if __cplusplus
extern "C" {
#endif

int modem_open(void);
int modem_close(void);
void modem_set_default_gateway(void);

#if __cplusplus
};  /* extern "C" */
#endif

#endif /* _MODEM_OPS_CONFIG_H */
