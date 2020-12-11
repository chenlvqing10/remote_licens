#include "stdsoap2.h"
#include "soapH.h"

#include "onvif_param.h"

#include "onvif.h"
#include "onvif_server.h"
#include "onvif_proc.h"
#include "soapStub.h"

#include "net_utils.h"

#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include  <linux/sockios.h>
#include  <linux/ethtool.h>
#include "DeviceBinding.nsmap"

#ifndef __MY_TEST__
#define LOG_TAG "ONVIF_NVT"
#include <log/log.h>
#endif /* __MY_TEST__ */


#ifdef USE_WSSE_AUTH
#include "wsseapi.h"
#include "wsaapi.h"
#endif

#ifdef USE_DIGEST_AUTH
#include "httpda.h"
#endif

#ifdef __MY_TEST__
#define ALOGD printf
#define ALOGE printf
#endif /* __MY_TEST__ */

struct soap gIntSoap;
int onvif_intr_thr_flag;
pthread_mutex_t g_inter_mutex;

#define NUM_BROADCAST (4) //number of times hello is broadcasted
#define NUM_TIMEOUT (5)
typedef struct _HI_ONVIF_MNG_S_
{
	int quit;
	int init;

	pthread_t helloThread;
	pthread_t probeThread;
	pthread_t serverThread;
	pthread_t helloWifiThread;
	pthread_t probeWifiThread;
	pthread_t onviftourThread;
	pthread_t interacteThread[MAX_SUPP_INTER_THRD];
	pthread_mutex_t mutex;
} HI_ONVIF_MNG_S;

static HI_ONVIF_MNG_S gOnvifMng;

int g_nPort;
char *g_pstringIP;

int onvif_major_version;
int onvif_minor_version;

static void hi_onvif_hello();

/**
 * onvif_get_ip_addr: get ip addr information
 *
 * return ip address.
 */
const char *onvif_get_ip_addr()
{
	return g_pstringIP;
}

/**
 * onvif_get_server_port: get server port information
 *
 * return server port.
 */
int onvif_get_server_port()
{
	return g_nPort;
}

/**
 * onvif_get_major_version: get onvif major version information
 *
 * return major version information.
 */
int onvif_get_major_version()
{
	return onvif_major_version;
}

/**
 * onvif_get_minor_version: get minor version information
 *
 * return minor version.
 */
int onvif_get_minor_version()
{
	return onvif_minor_version;
}

/**
 * get_local_ip: get local ip address
 * @iface_name: device name
 *
 * Returns ip address string.
 */
const char *get_local_ip(const char *iface_name)
{
	int iSocket, ret;
	struct ifreq ifr;
	struct sockaddr_in addr;
	static char szIpAddr[20];

	if (!iface_name)
		return NULL;

	iSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (iSocket < 0) {
		ALOGE("[%s:%d] socket() error, errno %d\n",
			__func__, __LINE__, errno);
		return NULL;
	}

	strcpy(ifr.ifr_name, iface_name);
	ret = ioctl(iSocket, SIOCGIFADDR, &ifr);
	if (ret < 0) {
		ALOGE("[%s:%d] ioctl(SIOCGIFADDR) error, errno %d\n",
			__func__, __LINE__, errno);
		close(iSocket);
		return NULL;
	}

	memcpy(&addr, &ifr.ifr_dstaddr, sizeof(struct sockaddr_in));
	strcpy(szIpAddr, inet_ntoa(addr.sin_addr));

	ALOGD("%s ip is %s\n", iface_name, szIpAddr);

	close(iSocket);

	g_pstringIP = szIpAddr;

	return szIpAddr;
}

/**
 * set_local_ip: set local ip address
 * @iface_name: device name
 * @ip: ip address
 *
 * Returns ip address string.
 */
int set_local_ip(const char* iface_name, const char *ip)
{
	int iSocket, ret;
	struct ifreq ifr;
	struct sockaddr_in addr;

	if (!iface_name || !ip)
		return -1;

	iSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (iSocket < 0) {
		ALOGE("[%s:%d] socket() error, errno %d\n",
			__func__, __LINE__, errno);
		return -1;
	}

	strcpy(ifr.ifr_name, iface_name);
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	memcpy(&ifr.ifr_addr, &addr, sizeof(struct sockaddr_in));
	ret = ioctl(iSocket, SIOCSIFADDR, &ifr);
	if (ret < 0) {
		ALOGE("[%s:%d] ioctl(SIOCSIFADDR) error, errno %d\n",
			__func__, __LINE__, errno);
		close(iSocket);
		return -1;
	}

	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	ret = ioctl(iSocket, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		ALOGE("[%s:%d] ioctl(SIOCSIFFLAGS) error, errno %d\n",
			__func__, __LINE__, errno);
		close(iSocket);
		return -1;
	}

	close(iSocket);

	return 0;
}

/**
 * onvif_get_mac_addr: get mac address
 * @macaddr: a pointer to mac address 
 * @len: length of mac address buffer.
 *
 * return minor version.
 */
int onvif_get_mac_addr(char *macaddr, int len)
{
	if (!macaddr || len < 6)
		return -1;
#if 1
	net_dev_get_mac_addr("eth0", (unsigned char *)macaddr);
#else
	macaddr[0] = 0x00;
	macaddr[1] = 0x11;
	macaddr[2] = 0x22;
	macaddr[3] = 0x33;
	macaddr[4] = 0x44;
	macaddr[5] = 0x55;
#endif
	return 0;
}

static int hi_onvif_get_netstatus()
{
	struct ethtool_value {
		unsigned int cmd;
		unsigned int data;
	};

	int skfd;
	struct ifreq ifr;
	struct ethtool_value edata;

	edata.cmd = ETHTOOL_GLINK;
	edata.data = 0;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name) - 1);
	ifr.ifr_data = (char*)&edata;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
		return -1;

	if (ioctl(skfd, SIOCETHTOOL, &ifr) == -1) {
		close(skfd);
		return -1;
	}

	close(skfd);
	return edata.data;
}

static void *hi_onvif_hello_check(void *data)
{
	int StatusFlag = 0;
	int ChangFlag = 0;

	pthread_detach(pthread_self());
	ChangFlag = hi_onvif_get_netstatus();

	while (!gOnvifMng.quit) {
		StatusFlag = hi_onvif_get_netstatus();

		if (ChangFlag != StatusFlag) {
			ChangFlag = StatusFlag;
			if (StatusFlag)
				hi_onvif_hello(0);
		}

		usleep(500000);
	}

	return NULL;
}

unsigned long hi_get_ip_by_interface(char *iface_name)
{
	int sockfd;
	struct sockaddr_in stAddr;
	struct ifreq ifr;

	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	strncpy(ifr.ifr_name, iface_name, IF_NAMESIZE);
	ifr.ifr_name[IF_NAMESIZE - 1] = '\0';

	if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) {
		close(sockfd);
		return -1;
	}

	close(sockfd);
	memcpy(&stAddr, &ifr.ifr_addr, sizeof(stAddr));
	return stAddr.sin_addr.s_addr;
}

static void *hi_onvif_probe(void *data)
{
	int nSocket = 0;
	unsigned long ipaddr;
	unsigned int accept_count = 0;
	struct soap* soap_udp;
	struct sockaddr_in servaddr;
	struct ip_mreq mip;

	pthread_detach(pthread_self());
	soap_udp = soap_new();
	soap_init1(soap_udp, SOAP_IO_UDP);

	do {
		ipaddr = hi_get_ip_by_interface("eth0");
		if (ipaddr != -1)
			break;
		usleep(100000);
	} while (1);

	if (!soap_valid_socket((nSocket = soap_bind(soap_udp, NULL,
			DEFAULT_ONVIF_WSD_PORT, 100)))) {
		soap_print_fault(soap_udp, stderr);
		__E("soap_bind err\r\n");
		soap_destroy(soap_udp);
		soap_end(soap_udp);
		soap_free(soap_udp);
		return NULL;
	}

	/* Enable Multicast Reception */
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ONVIF_WSD_MC_ADDR, &servaddr.sin_addr);
	servaddr.sin_port = htons(DEFAULT_ONVIF_WSD_PORT);
	mip.imr_multiaddr = servaddr.sin_addr;
	mip.imr_interface.s_addr = hi_get_ip_by_interface("eth0");
	setsockopt(nSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mip, sizeof(mip));
	
	while (!gOnvifMng.quit) {
		if (accept_count % 30 == 0 )
			printf("soap_accept %d\n", accept_count);

		accept_count++;

		if (soap_accept(soap_udp) < 0) {
			soap_print_fault(soap_udp, stderr);
			__E("soap_accept err\r\n");
			soap_destroy (soap_udp);
			soap_end(soap_udp);
			break;
		}
		stOnvifSocket.eth0Socket = soap_udp->master;
		soap_serve(soap_udp);
		soap_destroy(soap_udp);
		soap_end(soap_udp);
		usleep(500000);
	}

	soap_done(soap_udp);
	soap_free(soap_udp);

	return NULL;
}

void hi_onvif_hello(int nType)
{
	struct soap *soap = NULL;
	char guid[256];
	/*
	char macaddr[MACH_ADDR_LENGTH];
	*/
	soap = soap_new();
	if (soap == NULL) {
		printf("[%d]soap = NULL\n", __LINE__);
		return;
	}

	soap_set_namespaces(soap, namespaces);

	memset(guid, 0, sizeof(guid));
	onvif_get_dev_guid(guid, sizeof(guid));
	/*
	onvif_get_mac_addr(macaddr, 6);
	*/
	struct SOAP_ENV__Header *header;
	header = soap_malloc(soap, sizeof(struct SOAP_ENV__Header));
	/*setting default values for header*/
	soap_default_SOAP_ENV__Header(soap, header);
	header->wsa__MessageID = soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
	header->wsa__To = soap_malloc(soap, sizeof(char) * 128);
	header->wsa__Action = soap_malloc(soap, sizeof(char) * 128);

	sprintf(header->wsa__MessageID, "%s", guid);
#if 0
		"urn:uuid:20200108-15-16-30-%02X%02X%02X%02X%02X%02X",
		macaddr[0], macaddr[1], macaddr[2],
		macaddr[3], macaddr[4], macaddr[5]);
#endif
	strcpy(header->wsa__To,
		"urn:schemas-xmlsoap-org:ws:2005:04:discovery");
	strcpy(header->wsa__Action,
		"http://schemas.xmlsoap.org/ws/2005/04/discovery/Hello");
	soap->header = header;
	struct wsdd__HelloType* req_hello;
	req_hello = soap_malloc(soap, sizeof(struct wsdd__HelloType));
	soap_default_wsdd__HelloType(soap, req_hello);
	req_hello->Types  = "tdn:NetworkVideoTransmitter";
	req_hello->XAddrs = (char*)soap_malloc(soap, sizeof(char) * 128);
	sprintf(req_hello->XAddrs, "http://%s:%d/onvif/device_service",
		onvif_get_ip_addr(), onvif_get_server_port());
	req_hello->wsa__EndpointReference.Address = (char*)soap_malloc(soap,
		sizeof(char) * 128);

	sprintf(header->wsa__MessageID, "%s", guid);
#if 0
		"urn:uuid:20200108-15-16-30-%02X%02X%02X%02X%02X%02X",
		macaddr[0], macaddr[1], macaddr[2],
		macaddr[3], macaddr[4], macaddr[5]);
#endif
	req_hello->MetadataVersion = 1;
	soap_send___wsdd__Hello(soap, "soap.udp://239.255.255.250:3702/",
		NULL, req_hello);
	soap_destroy(soap);
	soap_end(soap);
	soap_done(soap);
	soap_free(soap);
}

int _dn_Bye_send(struct soap *soap)
{
	/*
	char macaddr[6];
	*/
	char guid[256];
	char msgid[LARGE_INFO_LENGTH];
	struct SOAP_ENV__Header header;
	struct wsdd__ByeType *Bye;
	memset(&header, 0, sizeof(struct SOAP_ENV__Header));
	/* setting default values for header */
	soap_default_SOAP_ENV__Header(soap, &header);
	soap->header = &header;
	
	memset(guid, 0, sizeof(guid));
	onvif_get_dev_guid(guid, sizeof(guid));
	/*
	onvif_get_mac_addr(macaddr, 6);
	*/
	memset(msgid, 0, sizeof(msgid));

	sprintf(msgid, "%s", guid);
#if 0
		"urn:uuid:20200108-15-16-30-%02X%02X%02X%02X%02X%02X",
		macaddr[0], macaddr[1], macaddr[2],
		macaddr[3], macaddr[4], macaddr[5]);
#endif
	ALLOC_TOKEN(header.wsa__MessageID, msgid);
	ALLOC_TOKEN(header.wsa__To,
		"urn:schemas-xmlsoap-org:ws:2005:04:discovery");
	ALLOC_TOKEN(header.wsa__Action,
		"http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye");
	Bye = soap_malloc(soap, sizeof(struct wsdd__ByeType));
	soap_default_wsdd__ByeType(soap, Bye);
	soap_send___wsdd__Bye(soap,
		"soap.udp://239.255.255.250:3702/", NULL, Bye);
	return SOAP_OK;
}

#if 0
void hi_onvif_bye(int nType)
{
	struct soap* pSoap;

	/* creating  soap object */
	pSoap = soap_new();

	/* initializing soap packet */
	soap_init1(pSoap, SOAP_IO_UDP | SO_BROADCAST);
	_dn_Bye_send(pSoap);
	soap_destroy(pSoap);
	soap_end(pSoap);
	soap_done(pSoap);
	soap_free(pSoap);
}
#endif

int onvif_interactive_work_proc(struct soap *soap)
{
	int ret = -1;
	fd_set r_set;
	struct timeval tv = {.tv_sec = 3, .tv_usec = 0 };
	FD_ZERO(&r_set);
	FD_SET(soap->socket, &r_set);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	ret = select(soap->socket + 1, &r_set, NULL, NULL, &tv);

	if (ret > 0) {
		soap->recv_timeout = 2;
		if ((ret = soap_serve(soap)) != SOAP_OK) {
			soap_print_fault(soap, stderr);
			printf("soap_serve err ret %d\n", ret);
		}
	} else if (ret < 0)
		printf("select sock %d err %m\n", soap->socket);

	return ret;
}

void* onvif_interactive_service_thrd(void* arg)
{
	struct soap *pSoap = NULL;
	int i = 0;
	int index = (int)arg;

	if (index < 0 || index >= MAX_SUPP_INTER_THRD)
		return NULL;

	while (!onvif_intr_thr_flag) {
		pthread_mutex_lock(&g_inter_mutex);
		if (!soap_valid_socket(soap_accept(&gIntSoap))) {
			printf("soap_accept err count %d => %m\n", i);
			if (++i > 20)
			{
				pthread_mutex_unlock(&g_inter_mutex);
				break;
			}
		}

		i = 0;
		pSoap = soap_copy(&gIntSoap);
		if (pSoap == NULL) {
			pthread_mutex_unlock(&g_inter_mutex);
			continue;
		}

		pthread_mutex_unlock(&g_inter_mutex);
		onvif_interactive_work_proc(pSoap);
		soap_destroy(pSoap);
		soap_end(pSoap);
		soap_free(pSoap);
	}

	return NULL;
}

int hi_onvif_interactive_thr()
{
	int i;
	SOAP_SOCKET udp_sock = -1;
	pthread_mutex_init(&g_inter_mutex, NULL);
	onvif_intr_thr_flag = 0;
	soap_init(&gIntSoap);

#ifdef USE_WSSE_AUTH
	printf("Need soap_register_plugin wsse\n");
	soap_register_plugin(&gIntSoap, soap_wsa);
#endif
#ifdef USE_DIGEST_AUTH
	printf("Need soap_register_plugin http_da\n");
	soap_register_plugin(&gIntSoap, http_da);
#endif
	soap_set_mode(&gIntSoap, SOAP_C_UTFSTRING);
	gIntSoap.bind_flags = SO_REUSEADDR;

	udp_sock = soap_bind(&gIntSoap, NULL,
			onvif_get_server_port(), 128);
	if (!soap_valid_socket(udp_sock)) {
		printf("onvif_interactive_service exiting[tid:%ld]...\n",
			getpid());
		soap_print_fault(&gIntSoap, stderr);
		return -1;
	}

	for (i = 0; i < MAX_SUPP_INTER_THRD; i++) {
		if (pthread_create(&gOnvifMng.interacteThread[i],
				NULL, onvif_interactive_service_thrd,
				(void*)i) < 0)
			return -2;
		else
			printf("interactive service thrd %d starting...\n", i);
	}

	return 0;
}

int onvif_init()
{
	__D("onvif initialize...\n");

	onvif_major_version = 3;
	onvif_minor_version = 1;

	/* g_nPort = 2085; */
	onvif_get_onvif_port(&g_nPort);
	g_pstringIP = (char *)get_local_ip("eth0");

	init_ptz();

	memset(&gOnvifMng, 0, sizeof(gOnvifMng));
	pthread_mutex_init(&gOnvifMng.mutex, NULL);
	onvif_server_init(g_nPort);
	hi_onvif_hello(0); /* Send Hello */
	pthread_create(&gOnvifMng.helloThread, NULL,
		hi_onvif_hello_check, NULL);
	pthread_create(&gOnvifMng.probeThread, NULL,
		hi_onvif_probe, NULL);
	gOnvifMng.init = 1;

	return 0;
}

int onvif_uninit()
{
	return 0;
}

int onvif_run()
{
	hi_onvif_interactive_thr();

	return 0;
}

int onvif_stop()
{
	return 0;
}

/**
 * onvif_nvt_start_service: start onvif nvt service
 * @NULL
 * Returns 0 means success, other means failure.
 */
int onvif_nvt_start_service()
{
	user_info_t user;
	sprintf(user.username, "%s", "admin");
	sprintf(user.password, "%s", "123456");
	user.level = 0;
	onvif_add_user_info(&user);

	onvif_init();
	onvif_run();

	return 0;
}

/**
 * onvif_nvt_stop_service: stop onvif nvt service
 * @NULL
 * Returns 0 means success, other means failure.
 */
int onvif_nvt_stop_service()
{
	onvif_stop();
	onvif_uninit();

	return 0;
}

#ifdef __MY_TEST__
int main()
{
	onvif_nvt_start_service();

	while (1) {

		sleep(1);
	}

	onvif_nvt_stop_service();

	return 0;
}
#endif /* __MY_TEST__ */

