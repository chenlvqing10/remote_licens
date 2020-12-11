#ifndef _MODEM_AT_PARSER_H
#define _MODEM_AT_PARSER_H

#if __cplusplus
extern "C" {
#endif

/* command result define */
#define AT_OK		(0)
#define AT_ERR		(-1)

/* operator define */
#define CHINA_MOBILE_1		"898600"
#define CHINA_MOBILE_2		"898602"
#define CHINA_MOBILE_3		"898604"
#define CHINA_MOBILE_4		"898607"
#define CHINE_UNICOM_1		"898601"
#define CHINE_UNICOM_2		"898606"
#define CHINE_UNICOM_3		"898609"
#define CHINE_TELECOM_1		"898603"
#define CHINE_TELECOM_2		"898611"

/**
 * OPER_MOBILE: china mobile operator.
 * @OPER_UNICOM: china unicom operator.
 * @OPER_TELECOM: china telecom operator.
 */
typedef enum {
	OPER_UNDEFINE = 0,
	OPER_MOBILE,
	OPER_UNICOM,
	OPER_TELECOM,
} OPER_CODE;

/**
 * common_resp - command response common struct
 * @data: at command response data
 * @len: response data length
 * @status: AT_OK or AT_ERR
 */
struct common_resp {
	char data[512];
	int len;
	int status;
};

/**
 * ccid_resp - CCID command response struct
 * @id: ccid string
 * @oper: operator
 * @status: AT_OK or AT_ERR
 */
struct ccid_resp {
	char id[20];
	OPER_CODE oper;
	int status;
};

/**
 * cpin_resp - CPIN command response struct
 * @pin: pin code
 * @is_ready: is sim card ready
 * @status: AT_OK or AT_ERR
 */
struct cpin_resp {
	char pin[20];
	int is_ready;
	int status;
};

/**
 * creg_resp - CREG command response struct
 * @net_reg: 0 - disable network register report
 *           1 - allow network register report result
 *           2 - allow network register report local info
 * @stat: register status
 *           0 - no register, no find operator
 *           1 - already register, local network
 *           2 - no register, finding station
 *           3 - register refuse
 *           4 - unknow code
 *           5 - already register, roaming
 * @status: AT_OK or AT_ERR
 */
struct creg_resp {
	int net_reg;
	int stat;
	int status;
};

/**
 * csq_resp - CSQ command response struct
 * @signal: signal strength
 * @ber: error rate
 * @status: AT_OK or AT_ERR
 */
struct csq_resp {
	int signal;
	int ber;
	int status;
};

/**
 * cgatt_resp - CGATT command response struct
 * @result: connect state, 0 - disconnect, 1 - connect
 * @status: AT_OK or AT_ERR
 */
struct cgatt_resp {
	int result;
	int status;
};

/**
 * sysinfo_resp - MYSYSINFO command response struct
 * @mode: network mode, 1 - auto, 2 - 2G, 3 - 3G, 4 - 4G, 5 - 2G+3G, 6 - 2G+4G
 *        7 - 3G+4G
 * @mnc: operator number
 *        00 - register fail
 *        01 - china mobile
 *        02 - china unicom
 *        03 - china telecom
 *        04 - unknow
 * @status: AT_OK or AT_ERR
 */
struct sysinfo_resp {
	int mode;
	int mnc;
	int status;
};

/**
 * netmsg_resp - NETMSG command response struct
 * @mcc: country code
 * @mnc: operator number
 * @net_mode: register network mode
 * @status: AT_OK or AT_ERR
 */
struct netmsg_resp {
	int mcc;
	int mnc;
	char net_mode[30];
	int status;
};

/* function */
int at_cmd_send_recv(char *cmd, int cmd_len, char *data, int *data_len);
int at_cmd_check_at(struct common_resp *resp);
int at_cmd_get_gmr(struct common_resp *resp);
int at_cmd_get_ccid(struct ccid_resp *resp);
int at_cmd_get_cpin(struct cpin_resp *resp);
int at_cmd_get_creg(struct creg_resp *resp);
int at_cmd_get_csq(struct csq_resp *resp);
int at_cmd_get_cgatt(struct cgatt_resp *resp);
int at_cmd_set_cgatt(struct cgatt_resp *resp, int connect);
int at_cmd_get_sysinfo(struct sysinfo_resp *resp);
int at_cmd_get_netmsg(struct netmsg_resp *resp);
int at_cmd_get_cgsn(struct common_resp *resp);

#if __cplusplus
};  /* extern "C" */
#endif

#endif /* _MODEM_AT_PARSER_H */
