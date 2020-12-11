#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  LOG_INFO		1
#define  LOG_TAG		"cmd_test"
#include <log/log.h>

#include "at_parser.h"
#include "ops.h"

void at_command_test(void)
{
	struct csq_resp csq_r;
	struct common_resp com_r;
	struct ccid_resp ccid_r;
	struct cpin_resp cpin_r;
	struct creg_resp creg_r;
	struct cgatt_resp cgatt_r;
	struct sysinfo_resp sysinfo_r;
	struct netmsg_resp netmsg_r;
	int ret;

	/* AT command */
	memset(&com_r, 0, sizeof(struct common_resp));
	ret = at_cmd_check_at(&com_r);
	if (ret != 0)
		ALOGE("cmd AT fail, hardware no work\n");
	else
		ALOGI("cmd AT ok.\n");

	/* GMR command */
	memset(&com_r, 0, sizeof(struct common_resp));
	ret = at_cmd_get_gmr(&com_r);
	if (ret != 0)
		ALOGE("cmd GMR fail, hardware no work\n");
	else
		ALOGI("cmd GMR ok, gmr: %s\n", com_r.data);

	/* CGSN command */
	memset(&com_r, 0, sizeof(struct common_resp));
	ret = at_cmd_get_cgsn(&com_r);
	if (ret != 0)
		ALOGE("cmd CGSN fail, hardware no work\n");
	else
		ALOGI("cmd CGSN ok, imei: %s\n", com_r.data);

	/* CSQ command */
	memset(&csq_r, 0, sizeof(struct csq_resp));
	ret = at_cmd_get_csq(&csq_r);
	if (ret != 0)
		ALOGE("cmd CSQ fail\n");
	else {
		ALOGI("cmd CSQ ok: status: %d, signal: %d, ber: %d\n",
		      csq_r.status, csq_r.signal, csq_r.ber);
	}

	/* CCID command */
	memset(&ccid_r, 0, sizeof(struct ccid_resp));
	ret = at_cmd_get_ccid(&ccid_r);
	if (ret != 0)
		ALOGE("cmd CCID fail, hardware no work\n");
	else {
		ALOGI("cmd CCID ok, status: %d, id: %s, oper: %d\n",
		      ccid_r.status, ccid_r.id, ccid_r.oper);
	}

	/* CPIN command */
	memset(&cpin_r, 0, sizeof(struct cpin_resp));
	ret = at_cmd_get_cpin(&cpin_r);
	if (ret != 0)
		ALOGE("cmd CPIN fail, hardware no work\n");
	else {
		ALOGI("cmd CPIN ok, status: %d, pin: %s, is_ready: %d\n",
		      cpin_r.status, cpin_r.pin, cpin_r.is_ready);
	}

	/* CREG command */
	memset(&creg_r, 0, sizeof(struct creg_resp));
	ret = at_cmd_get_creg(&creg_r);
	if (ret != 0)
		ALOGE("cmd CREG fail, hardware no work\n");
	else {
		ALOGI("cmd CREG ok, status: %d, net_reg: %d, stat: %d\n",
		      creg_r.status, creg_r.net_reg, creg_r.stat);
	}

	/* CGATT command */
	memset(&cgatt_r, 0, sizeof(struct cgatt_resp));
	ret = at_cmd_get_cgatt(&cgatt_r);
	if (ret != 0)
		ALOGE("cmd CGATT fail, hardware no work\n");
	else {
		ALOGI("cmd CGATT ok, status: %d, result: %d\n",
		      cgatt_r.status, cgatt_r.result);
	}

	/* SYSINFO command */
	memset(&sysinfo_r, 0, sizeof(struct sysinfo_resp));
	ret = at_cmd_get_sysinfo(&sysinfo_r);
	if (ret != 0)
		ALOGE("cmd SYSINFO fail, hardware no work\n");
	else {
		ALOGI("cmd SYSINFO ok, status: %d, mode: %d, mnc: %d\n",
		      sysinfo_r.status, sysinfo_r.mode, sysinfo_r.mnc);
	}

	/* NETMSG command */
	memset(&netmsg_r, 0, sizeof(struct netmsg_resp));
	ret = at_cmd_get_netmsg(&netmsg_r);
	if (ret != 0)
		ALOGE("cmd NETMSG fail, hardware no work\n");
	else {
		ALOGI("cmd NETMSG ok, status: %d, mcc: %d, mnc: %d, mode: %s\n",
		netmsg_r.status, netmsg_r.mcc, netmsg_r.mnc, netmsg_r.net_mode);
	}
}

void at_command_csq_test(void)
{
	struct csq_resp csq_r;
	int ret;

	while (1) {
		/* CSQ command */
		memset(&csq_r, 0, sizeof(struct csq_resp));
		ret = at_cmd_get_csq(&csq_r);
		if (ret != 0)
			ALOGE("cmd CSQ fail\n");
		else {
			ALOGI("cmd CSQ ok: status: %d, signal: %d, ber: %d\n",
			      csq_r.status, csq_r.signal, csq_r.ber);
		}
		usleep(10000);
	}
}

/* use help */
static void use_help(void)
{
	printf("modem-cmd-test use help:\n");
	printf("modem-cmd-test [at | start | close | gateway]\n");
	printf("at - test AT command\n");
	printf("start - start modem module\n");
	printf("close - close modem module\n");
	printf("gateway - set modem interface as default gateway\n");
}

/* test main function */
int main(int argc, char *argv[])
{
	int ret = 0;

	if (argc != 2) {
		use_help();
		return 0;
	}

	if (!strncmp(argv[1], "at", 2))
		at_command_test();
	else if (!strncmp(argv[1], "start", 5))
		ret = modem_open();
	else if (!strncmp(argv[1], "close", 3))
		ret = modem_close();
	else if (!strncmp(argv[1], "gateway", 7))
		modem_set_default_gateway();
	else if (!strncmp(argv[1], "csq", 3))
		at_command_csq_test();
	else
		use_help();

	return ret;
}
