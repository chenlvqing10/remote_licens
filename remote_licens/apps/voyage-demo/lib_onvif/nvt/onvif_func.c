#include "stdsoap2.h"
#include "soapH.h"
#include "soapStub.h"

#include "onvif_param.h"
#include "onvif.h"
#include "onvif_server.h"
#include "onvif_proc.h"
#include "onvifface.h"
#include "onvif_subscribe.h"

#include "base64.h"
#include "packbits.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/reboot.h>

#define TRACE printf
#define ALOGD printf

extern int g_tev_TopicNum;

static ptz_config_t s_ptz_config;
static preset_t s_preset[16];

int init_ptz()
{
	int i;

	memset(&s_ptz_config, 0, sizeof(ptz_config_t));
	s_ptz_config.pan_tilt_x = 0;
	s_ptz_config.pan_tilt_y = 0;
	s_ptz_config.default_ptz_time_out = 10;

	s_ptz_config.pan_tilt_limits_x_min = 0;
	s_ptz_config.pan_tilt_limits_x_max = 100;
	s_ptz_config.pan_tilt_limits_y_min = 0;
	s_ptz_config.pan_tilt_limits_y_max = 100;

	s_ptz_config.zoom_limits_min = 0;
	s_ptz_config.zoom_limits_max = 100;
	s_ptz_config.move_ramp = 10;

	s_ptz_config.preset_ramp = 10;
	s_ptz_config.preset_tour_ramp = 10;

	s_ptz_config.zoom_x = 0;

	memset(s_preset, 0, sizeof(preset_t) * 16);
	strcpy(s_preset[0].name, "PreSet0");
	strcpy(s_preset[0].token, "PreSet0");
	s_preset[0].pan_tilt_x = 0;
	s_preset[0].pan_tilt_y = 0;
	s_preset[0].zoom_x = 0;

	for (i=1; i<16; i++) {
		s_preset[i].pan_tilt_x = 0;
		s_preset[i].pan_tilt_y = 0,
		s_preset[i].zoom_x = 0;
	}

	return 0;
}


#define ALLOC_STRUCT(val, type) {\
	val = (type *) soap_malloc(soap, sizeof(type));\
	if (val == NULL) {\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	memset(val, 0, sizeof(type)); \
}

#define ALLOC_STRUCT_INT(val, type, ret) {\
	val = (type *) soap_malloc(soap, sizeof(type));\
	if(val == NULL) {\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	*val = ret;\
}

#define ALLOC_STRUCT_NUM(val, type, num) {\
	val = (type *) soap_malloc(soap, sizeof(type)*num);\
	if (val == NULL) {\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	memset(val, 0, sizeof(type)*num);\
}

#define ALLOC_TOKEN(val, token) {\
	val = (char *) soap_malloc(soap, sizeof(char)*ONVIF_TOKEN_LEN);\
	if(val == NULL) {\
		printf("malloc err\r\n");\
		return SOAP_FAULT;\
	}\
	memset(val, 0, sizeof(char)*ONVIF_TOKEN_LEN);\
	strcpy(val, token); \
}

#define CHECK_USER {\
printf("[%s:%d] begin check\n", __func__, __LINE__);\
int ret = onvif_check_security(soap);\
if(ret != SOAP_OK)\
	return ret;\
}

#define CLEAR_WSSE_HEAD {\
int ret = onvif_clear_wsse_head(soap);\
if(ret != SOAP_OK)\
	return ret;\
}

char* topic_set[] =
{
#if 1
	//md alarm
	"<tns1:RuleEngine wstop:topic=\"true\">"
	"<tns1:CellMotionDetector wstop:topic=\"true\">"
	"<tns1:Motion wstop:topic=\"true\">"
	"<tt:MessageDescription IsProperty=\"true\">"
	"<tt:Source>"
	"<tt:SimpleItemDescription Name=\"VideoSourceConfigurationToken\" "
	"Type=\"tt:ReferenceToken\"/>"
	"<tt:SimpleItemDescription Name=\"VideoAnalyticsConfigurationToken\" "
	"Type=\"tt:ReferenceToken\"/>"
	"<tt:SimpleItemDescription Name=\"Rule\" Type=\"xs:string\"/>"
	"</tt:Source>"
	"<tt:Data>"
	"<tt:SimpleItemDescription Name=\"IsMotion\" Type=\"xs:boolean\"/>"
	"</tt:Data>"
	"</tt:MessageDescription>"
	"</tns1:Motion>"
	"</tns1:CellMotionDetector>"
	"</tns1:RuleEngine>",
#endif

#if 1
	//io alarm
	"<tns1:Device wstop:topic=\"true\">"
	"<tns1:Trigger wstop:topic=\"true\">"
	"<tns1:DigitalInput wstop:topic=\"true\">"
	"<tt:MessageDescription IsProperty=\"true\">"
	"<tt:Source>"
	"<tt:SimpleItemDescription Name=\"IOInputToken\" "
	"Type=\"tt:ReferenceToken\"/>"
	"</tt:Source>"
	"<tt:Data>"
	"<tt:SimpleItemDescription Name=\"LogicalState\" "
	"Type=\"xs:boolean\"/>"
	"</tt:Data>"
	"</tt:MessageDescription>"
	"</tns1:DigitalInput>"
	"</tns1:Trigger>"
	"</tns1:Device>"
#endif
};

void *soap_calloc(struct soap * soap, size_t size)
{
	void * p = soap_malloc(soap, size);
	if (p)
		memset(p, 0, size);

	return p;
}

int put_soap_header_wsa(struct soap *soap)
{
	char msgid[LARGE_INFO_LENGTH];
	char guid[256];
	/*
	char macaddr[MACH_ADDR_LENGTH];
	*/

	memset(guid, 0, sizeof(guid));
	onvif_get_dev_guid(guid, sizeof(guid));
	
	ALLOC_TOKEN(soap->header->wsa__To,
	"http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous");
	ALLOC_TOKEN(soap->header->wsa__Action,
	"http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches");
	ALLOC_STRUCT(soap->header->wsa__RelatesTo, struct wsa__Relationship);
	ALLOC_TOKEN(soap->header->wsa__RelatesTo->__item,
		soap->header->wsa__MessageID);
	soap->header->wsa__RelatesTo->RelationshipType = NULL;
	soap->header->wsa__RelatesTo->__anyAttribute = NULL;
	//onvif_get_mac_addr(macaddr, 6);
	sprintf(msgid, "%s", guid);
#if 0
		"urn:uuid:20200108-15-16-30-%02X%02X%02X%02X%02X%02X",
		macaddr[0], macaddr[1], macaddr[2],
		macaddr[3], macaddr[4], macaddr[5]);
#endif
	ALLOC_TOKEN(soap->header->wsa__MessageID, msgid)

	return 0;
}

int SOAP_ENV__Fault(struct soap *soap, char *faultcode, char *faultstring,
		char *faultactor, struct SOAP_ENV__Detail *detail,
		struct SOAP_ENV__Code *SOAP_ENV__Code,
		struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node,
		char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail)
{
	return SOAP_FAULT;
}

int __wsdd__Hello(struct soap* soap, struct wsdd__HelloType* wsdd__Hello)
{
	return SOAP_FAULT;
}

int __wsdd__Bye(struct soap* soap, struct wsdd__ByeType* wsdd__Bye)
{
	return SOAP_FAULT;
}

static int set_routeinfo(struct sockaddr* pNetInfo, char* pAddr, int flag)
{
	if (pNetInfo == NULL)
		return -1;

	struct sockaddr_in *pRTAddr = (struct sockaddr_in *)pNetInfo;

	memset(pRTAddr, 0, sizeof(struct sockaddr_in));

	pRTAddr->sin_family = AF_INET;

	if (pAddr == NULL)
		return -1;

	if (flag)
		pRTAddr->sin_addr.s_addr = inet_addr(pAddr);

	return 0;
}

static int netAddDelRoute(struct soap *soap, char *gateway, char *subnetmask,
		char *dev, int method)
{
	int fd, posi = 0;
	struct rtentry rt;
	char pdst[16] = {0}, *pstr = NULL, local_ip[16] = {0};
	char client_ip[16] = {0};
	const char *ipaddr;
	static int addrouteflag = 0;

	ipaddr = get_local_ip("eth0");
	if (ipaddr)
		sprintf(local_ip, "%s", ipaddr);
	sprintf(client_ip, "%s", "0.0.0.0");

	if ((strcmp(client_ip, "0.0.0") != 0)
			&& (strcmp(local_ip, client_ip) != 0)) {
		sprintf(client_ip, "%d.%d.%d.%d",
				(soap->ip >> 24) & 0xFF,
				(soap->ip >> 16) & 0xFF,
				(soap->ip >> 8) & 0xFF,
				(soap->ip) & 0xFF);
		pstr = client_ip;
getsegment:

		if ((pstr = strchr(pstr, '.')) != NULL) {
			posi++;

			if (posi != 3) {
				pstr += 1;
				goto getsegment;
			}
		} else if (posi != 3) {
			printf("%s:%d -- get setment err\r\n",
				__func__, __LINE__);
			return -1;
		}

		memcpy(pdst, client_ip, pstr - client_ip + 1);
		strcat(pdst, "0");

		memset(&rt, 0, sizeof(struct rtentry));
		set_routeinfo(&rt.rt_dst, pdst, 1);
		set_routeinfo(&rt.rt_gateway, gateway, 0);
		set_routeinfo(&rt.rt_genmask, subnetmask, 1);
		rt.rt_flags = RTF_UP;
		rt.rt_dev = dev;
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd < 0) {
			perror("socket");
			return -1;
		}

		if (method) {
			addrouteflag = 1;

			if (ioctl(fd, SIOCADDRT, (char*)&rt) < 0) {
				perror("ioctl add");
				close(fd);
				return -1;
			}
		} else {
			if (!addrouteflag)
				return 0;

			addrouteflag = 0;

			if (ioctl(fd, SIOCDELRT, (char*)&rt) < 0) {
				perror("ioctl del");
				close(fd);
				return -1;
			}
		}

		close(fd);
	}

	return 0;
}

int __wsdd__Probe(struct soap *soap, struct wsdd__ProbeType *wsdd__Probe)
{
	char _HwId[1024];
	char guid[256];
	wsdd__ProbeMatchesType ProbeMatches;

	CLEAR_WSSE_HEAD;

	memset(guid, 0, sizeof(guid));
	onvif_get_dev_guid(guid, sizeof(guid));

	soap->master = stOnvifSocket.eth0Socket;
	ProbeMatches.ProbeMatch =
			(struct wsdd__ProbeMatchType*)soap_calloc(soap,
			sizeof(struct wsdd__ProbeMatchType));
	ProbeMatches.ProbeMatch->XAddrs =
			(char *)soap_calloc(soap,
			sizeof(char) * INFO_LENGTH);
	ProbeMatches.ProbeMatch->Types =
			(char *)soap_calloc(soap,
			sizeof(char) * INFO_LENGTH);
	ProbeMatches.ProbeMatch->Scopes =
			(struct wsdd__ScopesType*)soap_calloc(soap,
			sizeof(struct wsdd__ScopesType));
	ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties =
			(struct wsa__ReferencePropertiesType*)soap_calloc(soap,
			sizeof(struct wsa__ReferencePropertiesType));
	ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters =
			(struct wsa__ReferenceParametersType*)soap_calloc(soap,
			sizeof(struct wsa__ReferenceParametersType));
	ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName =
			(struct wsa__ServiceNameType*)soap_calloc(soap,
			sizeof(struct wsa__ServiceNameType));
	ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType =
			(char **)soap_calloc(
			soap, sizeof(char *) * SMALL_INFO_LENGTH);
	ProbeMatches.ProbeMatch->wsa__EndpointReference.__any =
			(char **)soap_calloc(soap,
			sizeof(char *) * SMALL_INFO_LENGTH);
	ProbeMatches.ProbeMatch->wsa__EndpointReference.__anyAttribute =
			(char *)soap_calloc(soap,
			sizeof(char) * SMALL_INFO_LENGTH);
	ProbeMatches.ProbeMatch->wsa__EndpointReference.Address =
			(char *)soap_calloc(soap,
			sizeof(char) * INFO_LENGTH);
	sprintf(_HwId, "%s", guid);
	ProbeMatches.__sizeProbeMatch = 1;
	ALLOC_STRUCT_NUM(ProbeMatches.ProbeMatch->Scopes->__item, char,
		LARGE_INFO_LENGTH);
	onvif_get_scopes(ProbeMatches.ProbeMatch->Scopes->__item,
		LARGE_INFO_LENGTH);
	ProbeMatches.ProbeMatch->Scopes->MatchBy = NULL;
	sprintf(ProbeMatches.ProbeMatch->XAddrs,
		"http://%s:%d/onvif/device_service",
		onvif_get_ip_addr(),
		onvif_get_server_port());
	strcpy(ProbeMatches.ProbeMatch->Types, wsdd__Probe->Types);
	ProbeMatches.ProbeMatch->MetadataVersion = 1;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ReferenceProperties->__size = 0;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ReferenceProperties->__any = NULL;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ReferenceParameters->__size = 0;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ReferenceParameters->__any = NULL;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		PortType[0] = (char*)soap_calloc(soap,
		sizeof(char) * SMALL_INFO_LENGTH);
	strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.
		PortType[0], "ttl");
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ServiceName->__item = NULL;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ServiceName->PortName = NULL;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.
		ServiceName->__anyAttribute = NULL;
	ProbeMatches.ProbeMatch->wsa__EndpointReference.__any[0] =
		(char*)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
	strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.
		__any[0], "Any");
	strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.
		__anyAttribute, "Attribute");
	ProbeMatches.ProbeMatch->wsa__EndpointReference.__size = 0;
	strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.
		Address, _HwId);

	if (soap->header) {
		soap->header->wsa__To =
		"http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
		soap->header->wsa__Action =
		"http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
		soap->header->wsa__RelatesTo =
			(struct wsa__Relationship*)soap_malloc(soap,
			sizeof(struct wsa__Relationship));
		soap->header->wsa__RelatesTo->__item =
			soap->header->wsa__MessageID;
		soap->header->wsa__RelatesTo->RelationshipType = NULL;
		soap->header->wsa__RelatesTo->__anyAttribute = NULL;
		soap->header->wsa__MessageID =
			(char*)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
		strcpy(soap->header->wsa__MessageID, _HwId + 4);
	}

	netAddDelRoute(soap, NULL, "255.255.255.0", "eth0", 1);
	soap_send___wsdd__ProbeMatches(soap, "http://", NULL, &ProbeMatches);
	netAddDelRoute(soap, NULL, "255.255.255.0", "eth0", 0);
	return SOAP_OK;
}

int __wsdd__ProbeMatches(struct soap *soap,
		struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{
	__FUN_BEGIN("\n");
	CLEAR_WSSE_HEAD;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __wsdd__Resolve(struct soap *soap,
		struct wsdd__ResolveType *wsdd__Resolve)
{
	__FUN_BEGIN("\n");
	CLEAR_WSSE_HEAD;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __wsdd__ResolveMatches(struct soap *soap,
		struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{
	__FUN_BEGIN("\n");
	CLEAR_WSSE_HEAD;
	__FUN_END("\n");
	return SOAP_FAULT;
}

#ifndef SUPPORT_ONVIF_2_6
int __ns1__CreatePrivacyMask(struct soap *soap,
		struct _ns1__CreatePrivacyMask *ns1__CreatePrivacyMask,
		struct _ns1__CreatePrivacyMaskResponse 
		*ns1__CreatePrivacyMaskResponse)
{
	__FUN_BEGIN("\n");
	int pt = 0;
	int nSizePt = 0;
	float left = 1.0, top = -1.0, right = -1.0, button = 1.0;
	int w = 704, h = 576;
	float ptx[4] = {0}, pty[4] = {0};
	HI_DEV_VMASK_CFG_S  stMaskCfg;
	struct ns2__PrivacyMask* PrivacyMask = NULL;

	if (ns1__CreatePrivacyMask == NULL ||
		ns1__CreatePrivacyMask->PrivacyMask == NULL)
		return SOAP_FAULT;

	PrivacyMask =  ns1__CreatePrivacyMask->PrivacyMask;

	if (strcmp(PrivacyMask->VideoSourceToken,
		ONVIF_MEDIA_VIDEO_SOURCE_TOKEN) != 0)
		return SOAP_FAULT;

	if (PrivacyMask->MaskArea == NULL)
		return SOAP_OK;

	gOnvifDevCb.pMsgProc(NULL, HI_GET_PARAM_MSG, HI_VMASK_PARAM, 0,
		(char*)&stMaskCfg, sizeof(HI_DEV_VMASK_CFG_S));
	nSizePt = PrivacyMask->MaskArea->__sizePoint;

	for (pt = 0; pt < nSizePt; pt++) {
		if (pt >= 4)
			continue;

		ptx[pt] = *PrivacyMask->MaskArea->Point[pt].x;
		pty[pt] = *PrivacyMask->MaskArea->Point[pt].y;
		if (ptx[pt] < left )
			left = ptx[pt];

		if (pty[pt] > top )
			top = pty[pt];

		if (ptx[pt] > right)
			right = ptx[pt];

		if (pty[pt] < button)
			button = pty[pt];
	}

	printf("left = %f, top = %f, right = %f, button = %f\r\n",
		left, top, right, button);
	memset(&stMaskCfg.struArea[0], 0,
		HI_VMASK_REG_NUM * sizeof(HI_RECT_S));
	stMaskCfg.struArea[0].s32X = (float)(w / 2 * (1 + left));
	stMaskCfg.struArea[0].s32Y = (float)(h / 2 * (1 - top));
	stMaskCfg.struArea[0].u32Width = (float)(w / 2 * (right - left));
	stMaskCfg.struArea[0].u32Height = (float)(h / 2) * (top - button);
	printf("x = %d, y = %d, w = %d, h = %d\r\n",
		stMaskCfg.struArea[0].s32X,
		stMaskCfg.struArea[0].s32Y,
		stMaskCfg.struArea[0].u32Width,
		stMaskCfg.struArea[0].u32Height);

	if (stMaskCfg.u8Enable == 0)
		stMaskCfg.u8Enable = 1;

	__FUN_END("\n");
	return SOAP_OK;
}

int __ns1__DeletePrivacyMask(struct soap *soap,
	struct _ns1__DeletePrivacyMask *ns1__DeletePrivacyMask,
	struct _ns1__DeletePrivacyMaskResponse
	*ns1__DeletePrivacyMaskResponse)
{
	__FUN_BEGIN("\n");
	char *p = NULL;
	int nIndex = 0;
	printf("Delete token %s\r\n",
		ns1__DeletePrivacyMask->PrivacyMaskToken_x0020);

	if (ns1__DeletePrivacyMask == NULL ||
		ns1__DeletePrivacyMask->PrivacyMaskToken_x0020 == NULL)
		return SOAP_OK;

	p = strstr(ns1__DeletePrivacyMask->PrivacyMaskToken_x0020,
		"MaskToken_");
	if (p == NULL)
		return SOAP_FAULT;

	p += strlen("MaskToken_");
	nIndex = atoi(p);
	if (nIndex >= HI_VMASK_REG_NUM)
		return SOAP_FAULT;
	__FUN_END("\n");
	return SOAP_OK;
}

int  __ns1__GetPrivacyMasks(struct soap *soap,
	struct _ns1__GetPrivacyMasks *ns1__GetPrivacyMasks,
	struct _ns1__GetPrivacyMasksResponse
	*ns1__GetPrivacyMasksResponse)
{
	__FUN_BEGIN("\n");
	ns1__GetPrivacyMasksResponse->__sizePrivacyMask = 0;
	ns1__GetPrivacyMasksResponse->PrivacyMask = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

int __ns1__GetPrivacyMask(struct soap* soap,
	struct _ns1__GetPrivacyMask *ns1__GetPrivacyMask,
	struct _ns1__GetPrivacyMaskResponse
	*ns1__GetPrivacyMaskResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __ns1__SetPrivacyMask(struct soap *soap,
	struct _ns1__SetPrivacyMask *ns1__SetPrivacyMask,
	struct _ns1__SetPrivacyMaskResponse
	*ns1__SetPrivacyMaskResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__GetPrivacyMaskOptions(struct soap *soap,
	struct _ns1__GetPrivacyMaskOptions *ns1__GetPrivacyMaskOptions,
	struct _ns1__GetPrivacyMaskOptionsResponse
	*ns1__GetPrivacyMaskOptionsResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_OK;
}

int __ns1__Set3DZoom(struct soap *soap,
	struct _ns1__Set3DZoom *ns1__Set3DZoom,
	struct _ns1__Set3DZoomResponse *ns1__Set3DZoomResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__GetIOInputs(struct soap *soap,
	struct _ns1__GetIOInputs *ns1__GetIOInputs,
	struct _ns1__GetIOInputsResponse *ns1__GetIOInputsResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_OK;
}

int __ns1__GetIOInputOptions(struct soap *soap,
	struct _ns1__GetIOInputOptions *ns1__GetIOInputOptions,
	struct _ns1__GetIOInputOptionsResponse *ns1__GetIOInputOptionsResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __ns1__SetIOInput(struct soap *soap,
	struct _ns1__SetIOInput *ns1__SetIOInput,
	struct _ns1__SetIOInputResponse *ns1__SetIOInputResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__GetPatterns(struct soap *soap,
	struct _ns1__GetPatterns *ns1__GetPatterns,
	struct _ns1__GetPatternsResponse *ns1__GetPatternsResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__StartRecordPattern(struct soap *soap,
	struct _ns1__StartRecordPattern *ns1__StartRecordPattern,
	struct _ns1__StartRecordPatternResponse
	*ns1__StartRecordPatternResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__StopRecordPattern(struct soap *soap,
	struct _ns1__StopRecordPattern *ns1__StopRecordPattern,
	struct _ns1__StopRecordPatternResponse
	*ns1__StopRecordPatternResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__RunPattern(struct soap *soap,
	struct _ns1__RunPattern *ns1__RunPattern,
	struct _ns1__RunPatternResponse *ns1__RunPatternResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __ns1__StopPattern(struct soap *soap,
	struct _ns1__StopPattern *ns1__StopPattern,
	struct _ns1__StopPatternResponse
	*ns1__StopPatternResponse)
{
	__FUN_BEGIN("\n");
	__FUN_END("\n");
	return SOAP_FAULT;
}
#endif

int __tan__GetSupportedRules(struct soap *soap,
	struct _tan__GetSupportedRules *tan__GetSupportedRules,
	struct _tan__GetSupportedRulesResponse
	*tan__GetSupportedRulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tan__CreateRules(struct soap *soap,
	struct _tan__CreateRules *tan__CreateRules,
	struct _tan__CreateRulesResponse *tan__CreateRulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tan__DeleteRules(struct soap *soap,
	struct _tan__DeleteRules *tan__DeleteRules,
	struct _tan__DeleteRulesResponse *tan__DeleteRulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tan__GetRules(struct soap *soap,
	struct _tan__GetRules *tan__GetRules,
	struct _tan__GetRulesResponse *tan__GetRulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	struct tt__Config* Rule = NULL;
	
	tan__GetRulesResponse->__sizeRule = 2;
	Rule = soap_malloc(soap,
		tan__GetRulesResponse->__sizeRule * sizeof(struct tt__Config));
	tan__GetRulesResponse->Rule = Rule;
	ALLOC_TOKEN(Rule[0].Name, "MyMotionDetectorRule");
	ALLOC_TOKEN(Rule[0].Type, "tt:CellMotionDetector");
	ALLOC_STRUCT(Rule[0].Parameters, struct tt__ItemList);
	Rule[0].Parameters->__sizeElementItem = 0;
	Rule[0].Parameters->Extension = NULL;
	Rule[0].Parameters->ElementItem = NULL;
	Rule[0].Parameters->__sizeSimpleItem = 4;
	Rule[0].Parameters->SimpleItem = soap_malloc(soap,
		Rule[0].Parameters->__sizeSimpleItem * sizeof(struct
		_tt__ItemList_SimpleItem ));
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[0].Name, "MinCount");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[0].Value, "1");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[1].Name, "AlarmOnDelay");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[1].Value, "100");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[2].Name, "AlarmOffDelay");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[2].Value, "1000");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[3].Name, "ActiveCells");
	ALLOC_TOKEN(Rule[0].Parameters->SimpleItem[3].Value, "0P8A8A==");
	ALLOC_TOKEN(Rule[1].Name, "MyTamperDetectorRule");
	ALLOC_TOKEN(Rule[1].Type, "ns2:TamperDetector");
	ALLOC_STRUCT(Rule[1].Parameters, struct tt__ItemList);
	Rule[1].Parameters->__sizeElementItem = 0;
	Rule[1].Parameters->Extension = NULL;
	Rule[1].Parameters->ElementItem = NULL;
	Rule[1].Parameters->__sizeSimpleItem = 0;
	Rule[1].Parameters->SimpleItem = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tan__ModifyRules(struct soap *soap,
	struct _tan__ModifyRules *tan__ModifyRules,
	struct _tan__ModifyRulesResponse
	*tan__ModifyRulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	__FUN_END("\n");
	return SOAP_OK;
}

int __tan__GetServiceCapabilities(struct soap *soap,
	struct _tan__GetServiceCapabilities
	*tan__GetServiceCapabilities,
	struct _tan__GetServiceCapabilitiesResponse 
	*tan__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tan__GetSupportedAnalyticsModules(struct soap *soap,
	struct _tan__GetSupportedAnalyticsModules
	*tan__GetSupportedAnalyticsModules,
	struct _tan__GetSupportedAnalyticsModulesResponse
	*tan__GetSupportedAnalyticsModulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tan__CreateAnalyticsModules(struct soap *soap,
	struct _tan__CreateAnalyticsModules *tan__CreateAnalyticsModules,
	struct _tan__CreateAnalyticsModulesResponse
	*tan__CreateAnalyticsModulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tan__DeleteAnalyticsModules(struct soap *soap,
	struct _tan__DeleteAnalyticsModules *tan__DeleteAnalyticsModules,
	struct _tan__DeleteAnalyticsModulesResponse
	*tan__DeleteAnalyticsModulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tan__GetAnalyticsModules(struct soap *soap,
	struct _tan__GetAnalyticsModules *tan__GetAnalyticsModules,
	struct _tan__GetAnalyticsModulesResponse
	*tan__GetAnalyticsModulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tan__ModifyAnalyticsModules(struct soap *soap,
	struct _tan__ModifyAnalyticsModules *tan__ModifyAnalyticsModules,
	struct _tan__ModifyAnalyticsModulesResponse
	*tan__ModifyAnalyticsModulesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tdn__Hello(struct soap *soap,
	struct wsdd__HelloType tdn__Hello,
	struct wsdd__ResolveType *tdn__HelloResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tdn__Bye(struct soap *soap,
	struct wsdd__ByeType tdn__Bye,
	struct wsdd__ResolveType *tdn__ByeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tdn__Probe(struct soap *soap,
	struct wsdd__ProbeType tdn__Probe,
	struct wsdd__ProbeMatchesType *tdn__ProbeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetServices(struct soap *soap,
	struct _tds__GetServices *tds__GetServices,
	struct _tds__GetServicesResponse *tds__GetServicesResponse)
{
	char dev_uri[80];
	__FUN_BEGIN("\n");
	CLEAR_WSSE_HEAD;
	tds__GetServicesResponse->__sizeService = 6;
	ALLOC_STRUCT_NUM(tds__GetServicesResponse->Service, struct tds__Service, 6);
	ALLOC_TOKEN(tds__GetServicesResponse->Service[0].Namespace,
		"http://www.onvif.org/ver10/analytics/wsdl");
	snprintf(dev_uri, sizeof(dev_uri), "http://%s:%d/onvif/services",
		onvif_get_ip_addr(), onvif_get_server_port());
	ALLOC_TOKEN(tds__GetServicesResponse->Service[0].XAddr, dev_uri);
	ALLOC_STRUCT(tds__GetServicesResponse->Service[0].Version,
		struct tt__OnvifVersion);
	tds__GetServicesResponse->Service[0].Version->Major =
		onvif_get_major_version();
	tds__GetServicesResponse->Service[0].Version->Minor =
		onvif_get_minor_version();
	ALLOC_TOKEN(tds__GetServicesResponse->Service[1].Namespace,
		"http://www.onvif.org/ver10/device/wsdl");
	ALLOC_TOKEN(tds__GetServicesResponse->Service[1].XAddr, dev_uri);
	ALLOC_STRUCT(tds__GetServicesResponse->Service[1].Version,
		struct tt__OnvifVersion);
	tds__GetServicesResponse->Service[1].Version->Major =
		onvif_get_major_version();
	tds__GetServicesResponse->Service[1].Version->Minor =
		onvif_get_minor_version();
	ALLOC_TOKEN(tds__GetServicesResponse->Service[2].Namespace,
		"http://www.onvif.org/ver10/events/wsdl");
	ALLOC_TOKEN(tds__GetServicesResponse->Service[2].XAddr, dev_uri);
	ALLOC_STRUCT(tds__GetServicesResponse->Service[2].Version,
		struct tt__OnvifVersion);
	tds__GetServicesResponse->Service[2].Version->Major =
		onvif_get_major_version();
	tds__GetServicesResponse->Service[2].Version->Minor =
		onvif_get_minor_version();
	ALLOC_TOKEN(tds__GetServicesResponse->Service[3].Namespace,
		"http://www.onvif.org/ver10/imaging/wsdl");
	ALLOC_TOKEN(tds__GetServicesResponse->Service[3].XAddr, dev_uri);
	ALLOC_STRUCT(tds__GetServicesResponse->Service[3].Version,
		struct tt__OnvifVersion);
	tds__GetServicesResponse->Service[3].Version->Major =
		onvif_get_major_version();
	tds__GetServicesResponse->Service[3].Version->Minor =
		onvif_get_minor_version();
	ALLOC_TOKEN(tds__GetServicesResponse->Service[4].Namespace,
		"http://www.onvif.org/ver10/media/wsdl");
	ALLOC_TOKEN(tds__GetServicesResponse->Service[4].XAddr, dev_uri);
	ALLOC_STRUCT(tds__GetServicesResponse->Service[4].Version,
		struct tt__OnvifVersion);
	tds__GetServicesResponse->Service[4].Version->Major =
		onvif_get_major_version();
	tds__GetServicesResponse->Service[4].Version->Minor =
		onvif_get_minor_version();
	ALLOC_TOKEN(tds__GetServicesResponse->Service[5].Namespace,
		"http://www.onvif.org/ver10/ptz/wsdl");
	ALLOC_TOKEN(tds__GetServicesResponse->Service[5].XAddr, dev_uri);
	ALLOC_STRUCT(tds__GetServicesResponse->Service[5].Version,
		struct tt__OnvifVersion);
	tds__GetServicesResponse->Service[5].Version->Major =
		onvif_get_major_version();
	tds__GetServicesResponse->Service[5].Version->Minor =
		onvif_get_minor_version();
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetServiceCapabilities(struct soap* soap,
	struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities,
	struct _tds__GetServiceCapabilitiesResponse
	*tds__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CLEAR_WSSE_HEAD;
	ALLOC_STRUCT(tds__GetServiceCapabilitiesResponse->Capabilities,
		struct tds__DeviceServiceCapabilities);
	/* required element of type tds:NetworkCapabilities */
	struct tds__NetworkCapabilities *Network;
	/* required element of type tds:SecurityCapabilities */
	struct tds__SecurityCapabilities *Security;
	/* required element of type tds:SystemCapabilities */
	struct tds__SystemCapabilities *System;
	/* optional element of type tds:MiscCapabilities */
	/* struct tds__MiscCapabilities *Misc; */
	ALLOC_STRUCT(Network, struct tds__NetworkCapabilities);
	ALLOC_STRUCT(Network->NTP, int);
	*Network->NTP = 3;
	ALLOC_STRUCT(Network->HostnameFromDHCP, enum xsd__boolean);
	*Network->HostnameFromDHCP = xsd__boolean__false_;
	ALLOC_STRUCT(Network->Dot11Configuration, enum xsd__boolean);
	*Network->Dot11Configuration = xsd__boolean__false_;
	ALLOC_STRUCT(Network->DynDNS, enum xsd__boolean);
	*Network->DynDNS = xsd__boolean__false_;
	ALLOC_STRUCT(Network->IPVersion6, enum xsd__boolean);
	*Network->IPVersion6 = xsd__boolean__false_;
	ALLOC_STRUCT(Network->ZeroConfiguration, enum xsd__boolean);
	*Network->ZeroConfiguration = xsd__boolean__false_;
	ALLOC_STRUCT(Network->IPFilter, enum xsd__boolean);
	*Network->IPFilter = xsd__boolean__false_;
	ALLOC_STRUCT(Security, struct tds__SecurityCapabilities);
	ALLOC_STRUCT(Security->RELToken, enum xsd__boolean);
	*Security->RELToken = xsd__boolean__false_;
	ALLOC_STRUCT(Security->HttpDigest, enum xsd__boolean);
	*Security->HttpDigest = xsd__boolean__false_;
	ALLOC_STRUCT(Security->UsernameToken, enum xsd__boolean);
	*Security->UsernameToken = xsd__boolean__false_;
	ALLOC_STRUCT(Security->KerberosToken, enum xsd__boolean);
	*Security->KerberosToken = xsd__boolean__false_;
	ALLOC_STRUCT(Security->SAMLToken, enum xsd__boolean);
	*Security->SAMLToken = xsd__boolean__false_;
	ALLOC_STRUCT(Security->X_x002e509Token, enum xsd__boolean);
	*Security->X_x002e509Token = xsd__boolean__false_;
	ALLOC_STRUCT(Security->RemoteUserHandling, enum xsd__boolean);
	*Security->RemoteUserHandling = xsd__boolean__false_;
	ALLOC_STRUCT(Security->Dot1X, enum xsd__boolean);
	*Security->Dot1X = xsd__boolean__false_;
	ALLOC_STRUCT(Security->AccessPolicyConfig, enum xsd__boolean);
	*Security->AccessPolicyConfig = xsd__boolean__false_;
	ALLOC_STRUCT(Security->OnboardKeyGeneration, enum xsd__boolean);
	*Security->OnboardKeyGeneration = xsd__boolean__false_;
	ALLOC_STRUCT(Security->TLS1_x002e2, enum xsd__boolean);
	*Security->TLS1_x002e2 = xsd__boolean__false_;
	ALLOC_STRUCT(Security->TLS1_x002e1, enum xsd__boolean);
	*Security->TLS1_x002e1 = xsd__boolean__false_;
	ALLOC_STRUCT(Security->TLS1_x002e0, enum xsd__boolean);
	*Security->TLS1_x002e0 = xsd__boolean__false_;
	ALLOC_STRUCT(System, struct tds__SystemCapabilities);
	ALLOC_STRUCT(System->HttpSupportInformation, enum xsd__boolean);
	*System->HttpSupportInformation = xsd__boolean__true_;
	ALLOC_STRUCT(System->HttpSystemLogging, enum xsd__boolean);
	*System->HttpSystemLogging = xsd__boolean__true_;
	ALLOC_STRUCT(System->HttpSystemBackup, enum xsd__boolean);
	*System->HttpSystemBackup = xsd__boolean__false_;
	ALLOC_STRUCT(System->HttpFirmwareUpgrade, enum xsd__boolean);
	*System->HttpFirmwareUpgrade = xsd__boolean__true_;
	ALLOC_STRUCT(System->FirmwareUpgrade, enum xsd__boolean);
	*System->FirmwareUpgrade = xsd__boolean__false_;
	ALLOC_STRUCT(System->SystemLogging, enum xsd__boolean);
	*System->SystemLogging = xsd__boolean__true_;
	ALLOC_STRUCT(System->SystemBackup, enum xsd__boolean);
	*System->SystemBackup = xsd__boolean__false_;
	ALLOC_STRUCT(System->RemoteDiscovery, enum xsd__boolean);
	*System->RemoteDiscovery = xsd__boolean__false_;
	ALLOC_STRUCT(System->DiscoveryBye, enum xsd__boolean);
	*System->DiscoveryBye = xsd__boolean__false_;
	ALLOC_STRUCT(System->DiscoveryResolve, enum xsd__boolean);
	*System->DiscoveryResolve = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network = Network;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security = Security;
	tds__GetServiceCapabilitiesResponse->Capabilities->System = System;
	tds__GetServiceCapabilitiesResponse->Capabilities->Misc = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetDeviceInformation(struct soap* soap,
	struct _tds__GetDeviceInformation *tds__GetDeviceInformation,
	struct _tds__GetDeviceInformationResponse
	*tds__GetDeviceInformationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	char mfacture[64], serial_no[64];
	char model[64], version[64], hardware_id[64];

	tds__GetDeviceInformationResponse->Manufacturer =
			(char *)soap_calloc(soap, 256);
	tds__GetDeviceInformationResponse->Model =
			(char *)soap_calloc(soap, 256);
	tds__GetDeviceInformationResponse->FirmwareVersion =
			(char *)soap_calloc(soap, 256);
	tds__GetDeviceInformationResponse->SerialNumber =
			(char *)soap_calloc(soap, 256);
	tds__GetDeviceInformationResponse->HardwareId =
			(char *)soap_calloc(soap, 256);

	memset(mfacture, 0, sizeof(mfacture));
	onvif_get_manufacturer(mfacture, sizeof(mfacture));
	memset(model, 0, sizeof(model));
	onvif_get_device_model(model, sizeof(model));
	memset(version, 0, sizeof(version));
	onvif_get_device_version(version, sizeof(version));
	memset(serial_no, 0, sizeof(serial_no));
	onvif_get_serial_no(serial_no, sizeof(serial_no));
	memset(hardware_id, 0, sizeof(hardware_id));
	onvif_get_hardware_id(hardware_id, sizeof(hardware_id));
	strcpy(tds__GetDeviceInformationResponse->Manufacturer,
			mfacture);
	strcpy(tds__GetDeviceInformationResponse->Model,
			model);
	strcpy(tds__GetDeviceInformationResponse->FirmwareVersion,
			version);
	strcpy(tds__GetDeviceInformationResponse->SerialNumber,
			serial_no);
	strcpy(tds__GetDeviceInformationResponse->HardwareId,
			hardware_id);

	__FUN_END("\n");

	return SOAP_OK;
}

int  __tds__SetSystemDateAndTime(struct soap *soap,
	struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime,
	struct _tds__SetSystemDateAndTimeResponse
	*tds__SetSystemDateAndTimeResponse)
{
	time_t t = 0;
	struct tm tmp_time;
	struct timeval tv;
	int time_value = 0;
	unsigned char year = 0, mon = 0, day = 0;
	unsigned char hour = 0, min = 0, sec = 0;

	__FUN_BEGIN("\n");
	CHECK_USER;
	/* Time Zone */
	if (tds__SetSystemDateAndTime->TimeZone) {
		printf("===========zone============:%s\r\n",
			tds__SetSystemDateAndTime->TimeZone->TZ);
		onvif_set_timezone(tds__SetSystemDateAndTime->TimeZone->TZ);
		time_value = onvif_get_time_value(
			tds__SetSystemDateAndTime->TimeZone->TZ);
	}
	if (tds__SetSystemDateAndTime->UTCDateTime) {
		if (tds__SetSystemDateAndTime->UTCDateTime->Date) {
			year = (unsigned char )(tds__SetSystemDateAndTime->
				UTCDateTime->Date->Year - 1900);
			mon = (unsigned char )tds__SetSystemDateAndTime->
				UTCDateTime->Date->Month;
			day = (unsigned char )tds__SetSystemDateAndTime->
				UTCDateTime->Date->Day;
			printf("Y:%d, m:%d, d:%d\r\n", year, mon, day);

			if ((mon > 12) || (day > 31)) {
				onvif_fault(soap, "ter:InvalidArgVal",
						"ter:InvalidDateTime");
				return SOAP_FAULT;
			}
		}

		if (tds__SetSystemDateAndTime->UTCDateTime->Time) {
			hour = (unsigned char )tds__SetSystemDateAndTime->
				UTCDateTime->Time->Hour;
			min = (unsigned char )tds__SetSystemDateAndTime->
				UTCDateTime->Time->Minute;
			sec = (unsigned char )tds__SetSystemDateAndTime->
				UTCDateTime->Time->Second;

			if ((hour> 24) || (min > 60) || (sec > 60)) {
				onvif_fault(soap, "ter:InvalidArgVal",
						"ter:InvalidDateTime");
				return SOAP_FAULT;
			}
		}

		tmp_time.tm_year = year;
		tmp_time.tm_mon = mon - 1;
		tmp_time.tm_mday = day;
		tmp_time.tm_hour = hour;
		tmp_time.tm_min = min;
		tmp_time.tm_sec = sec;
		t = mktime(&tmp_time);
		t += time_value;
		tv.tv_sec = t;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);

	}

	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetSystemDateAndTime(struct soap *soap,
	struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime,
	struct _tds__GetSystemDateAndTimeResponse
	*tds__GetSystemDateAndTimeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	int index;
	struct tm *tm;
	time_t curTime = time(NULL);
	tm = localtime(&curTime);

	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->SystemDateAndTime,
		struct tt__SystemDateTime);

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DateTimeType =
			tt__SetDateTimeType__Manual; /* tt__SetDateTimeType__NTP */
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DaylightSavings =
			xsd__boolean__false_; /* xsd__boolean__true_ */

#if 1
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->
		SystemDateAndTime->TimeZone,
		struct tt__TimeZone);
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ =
		(char *)soap_calloc(soap, 256);
	onvif_get_timezone_index(&index);
	sprintf(tds__GetSystemDateAndTimeResponse->
		SystemDateAndTime->TimeZone->TZ, "%s",
		onvif_get_timezone(index));
#else
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone = NULL;
#endif

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime =
			NULL;
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->
		SystemDateAndTime->LocalDateTime,
		struct tt__DateTime);
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->
		SystemDateAndTime->LocalDateTime->Time,
		struct tt__Time);
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->
			LocalDateTime->Time->Hour =
			tm->tm_hour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->
			LocalDateTime->Time->Minute =
			tm->tm_min;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->
			LocalDateTime->Time->Second =
			tm->tm_sec;
	ALLOC_STRUCT(tds__GetSystemDateAndTimeResponse->
		SystemDateAndTime->LocalDateTime->Date,
		struct tt__Date);
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->
			LocalDateTime->Date->Year =
			tm->tm_year + 1900;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->
			LocalDateTime->Date->Month =
			tm->tm_mon + 1;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->
			LocalDateTime->Date->Day =
			tm->tm_mday;

	tds__GetSystemDateAndTimeResponse->
			SystemDateAndTime->Extension = NULL;
	tds__GetSystemDateAndTimeResponse->
			SystemDateAndTime->__anyAttribute = NULL;

	__FUN_END("\n");
	return SOAP_OK;
}

int  __tds__SetSystemFactoryDefault(struct soap *soap,
	struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault,
	struct _tds__SetSystemFactoryDefaultResponse
	*tds__SetSystemFactoryDefaultResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (tt__FactoryDefaultType__Hard ==
			tds__SetSystemFactoryDefault->FactoryDefault)
		printf("===system factorydefault hard===\n");
	else
		printf("===system factorydefault soft===\n");

	reboot(RB_AUTOBOOT);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__UpgradeSystemFirmware(struct soap *soap,
	struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware,
	struct _tds__UpgradeSystemFirmwareResponse
	*tds__UpgradeSystemFirmwareResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	/*
	tds__UpgradeSystemFirmware->Firmware->xop__Include.__ptr
	*/
	printf("upgrade: size %d id %s type %s options %s\n",
		tds__UpgradeSystemFirmware->Firmware->xop__Include.__size,
		tds__UpgradeSystemFirmware->Firmware->xop__Include.id,
		tds__UpgradeSystemFirmware->Firmware->xop__Include.type,
		tds__UpgradeSystemFirmware->Firmware->xop__Include.options);

	printf("conentType %s\n",
		tds__UpgradeSystemFirmware->Firmware->xmime__contentType);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SystemReboot(struct soap *soap,
	struct _tds__SystemReboot *tds__SystemReboot,
	struct _tds__SystemRebootResponse
	*tds__SystemRebootResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	reboot(RB_AUTOBOOT);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__RestoreSystem(struct soap *soap,
	struct _tds__RestoreSystem *tds__RestoreSystem,
	struct _tds__RestoreSystemResponse
	*tds__RestoreSystemResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
	__FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}

int __tds__GetSystemBackup(struct soap *soap,
	struct _tds__GetSystemBackup *tds__GetSystemBackup,
	struct _tds__GetSystemBackupResponse
	*tds__GetSystemBackupResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
	__FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}

int __tds__GetSystemLog(struct soap *soap,
	struct _tds__GetSystemLog *tds__GetSystemLog,
	struct _tds__GetSystemLogResponse
	*tds__GetSystemLogResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	struct tt__SystemLog *pSystemLog;
	ALLOC_STRUCT(pSystemLog, struct tt__SystemLog);

	pSystemLog->Binary = NULL;
	ALLOC_STRUCT_NUM(pSystemLog->String, char, 256);
	if (tds__GetSystemLog->LogType)
		sprintf(pSystemLog->String, "%s",
			"This is one AccessLog!\r\nThis is two AccessLog!\r\n");
	else
		sprintf(pSystemLog->String, "%s",
			"This is one SystemLog!\r\nThis is two SystemLog!\r\n");

	tds__GetSystemLogResponse->SystemLog = pSystemLog;

	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetSystemSupportInformation(struct soap *soap,
	struct _tds__GetSystemSupportInformation
	*tds__GetSystemSupportInformation,
	struct _tds__GetSystemSupportInformationResponse
	*tds__GetSystemSupportInformationResponse)
{
	__FUN_BEGIN("Temperarily no process!\r\n");
	__FUN_END("Temperarily no process!\r\n");
	return SOAP_FAULT;
}

int __tds__GetScopes(struct soap *soap,
	struct _tds__GetScopes *tds__GetScopes,
	struct _tds__GetScopesResponse *tds__GetScopesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	char dev_name[64], location[64];
	int nScopesSize = 7;
	struct tt__Scope *pScopes;
	tds__GetScopesResponse->__sizeScopes = nScopesSize;
	ALLOC_STRUCT_NUM(pScopes, struct tt__Scope, nScopesSize);
	tds__GetScopesResponse->Scopes = pScopes;

	memset(dev_name, 0, sizeof(dev_name));
	onvif_get_dev_name(dev_name, sizeof(dev_name));
	memset(location, 0, sizeof(location));
	onvif_get_location(location, sizeof(location));

	int i = 0;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	strcpy(pScopes[i].ScopeItem, "onvif://www.onvif.org/Profile/Streaming");

	i++;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	strcpy(pScopes[i].ScopeItem, "onvif://www.onvif.org/Profile/S");

	i++;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	strcpy(pScopes[i].ScopeItem, "onvif://www.onvif.org/type/audio_encoder");

	i++;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	strcpy(pScopes[i].ScopeItem, "onvif://www.onvif.org/type/video_encoder");

	i++;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	strcpy(pScopes[i].ScopeItem, "onvif://www.onvif.org/hardware/onvif_nvt");

	i++;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Configurable;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	sprintf(pScopes[i].ScopeItem, "%s%s",
			"onvif://www.onvif.org/name/",
			dev_name);

	i++;
	pScopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
	ALLOC_STRUCT_NUM(pScopes[i].ScopeItem, char, 256);
	sprintf(pScopes[i].ScopeItem, "%s%s",
			"onvif://www.onvif.org/location/",
			location);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetScopes(struct soap *soap,
	struct _tds__SetScopes *tds__SetScopes,
	struct _tds__SetScopesResponse *tds__SetScopesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	char *pos;
	char dev_name[64], location[64];
	memset(dev_name, 0, sizeof(dev_name));
	memset(location, 0, sizeof(location));
	if (tds__SetScopes->__sizeScopes == 2) {
		pos = strrchr(tds__SetScopes->Scopes[0], '/');
		if (pos) {
			pos++;
			snprintf(dev_name, sizeof(dev_name), "%s", pos);
			onvif_set_dev_name(dev_name);
		}

		pos = strrchr(tds__SetScopes->Scopes[1], ':');
		if (pos) {
			pos++;
			snprintf(location, sizeof(location), "%s", pos);
			onvif_set_location(location);
		}
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__AddScopes(struct soap *soap,
	struct _tds__AddScopes *tds__AddScopes,
	struct _tds__AddScopesResponse
	*tds__AddScopesResponse)
{
	onvif_fault(soap, "ter:Action", "ter:TooManyScopes");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

int __tds__RemoveScopes(struct soap *soap,
	struct _tds__RemoveScopes *tds__RemoveScopes,
	struct _tds__RemoveScopesResponse
	*tds__RemoveScopesResponse)
{
	onvif_fault(soap, "ter:OperationProhibited", "ter:FixedScope");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

int __tds__GetDiscoveryMode(struct soap* soap,
	struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode,
	struct _tds__GetDiscoveryModeResponse
	*tds__GetDiscoveryModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	tds__GetDiscoveryModeResponse->DiscoveryMode =
		tt__DiscoveryMode__Discoverable;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetDiscoveryMode(struct soap *soap,
	struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode,
	struct _tds__SetDiscoveryModeResponse
	*tds__SetDiscoveryModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetRemoteDiscoveryMode(struct soap *soap,
	struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode,
	struct _tds__GetRemoteDiscoveryModeResponse
	*tds__GetRemoteDiscoveryModeResponse)
{
	onvif_fault(soap, "ter:InvalidArgVal",
		"ter:GetRemoteDiscoveryModeNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

int __tds__SetRemoteDiscoveryMode(struct soap *soap,
	struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode,
	struct _tds__SetRemoteDiscoveryModeResponse
	*tds__SetRemoteDiscoveryModeResponse)
{
	onvif_fault(soap, "ter:InvalidArgVal",
		"ter:SetRemoteDiscoveryModeNotSupported");
	__FUN_BEGIN("Onvif fault process!\r\n");
	__FUN_END("Onvif fault process!\r\n");
	return SOAP_FAULT;
}

int __tds__GetDPAddresses(struct soap *soap,
	struct _tds__GetDPAddresses *tds__GetDPAddresses,
	struct _tds__GetDPAddressesResponse
	*tds__GetDPAddressesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetEndpointReference(struct soap *soap,
	struct _tds__GetEndpointReference *tds__GetEndpointReference,
	struct _tds__GetEndpointReferenceResponse
	*tds__GetEndpointReferenceResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetRemoteUser(struct soap *soap,
	struct _tds__GetRemoteUser *tds__GetRemoteUser,
	struct _tds__GetRemoteUserResponse
	*tds__GetRemoteUserResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetRemoteUser(struct soap *soap,
	struct _tds__SetRemoteUser *tds__SetRemoteUser,
	struct _tds__SetRemoteUserResponse
	*tds__SetRemoteUserResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetUsers(struct soap *soap,
	struct _tds__GetUsers *tds__GetUsers,
	struct _tds__GetUsersResponse *tds__GetUsersResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i, count, index;
	user_info_t user;

	count = 0;
	for (i = 0; i  < MAX_USER_NUM; i++) {
		memset(&user, 0, sizeof(user_info_t));
		onvif_get_user_info(i, &user, sizeof(user_info_t));
		if (0 < strlen(user.username))
			count++;
	}

	tds__GetUsersResponse->__sizeUser = count;
	ALLOC_STRUCT_NUM(tds__GetUsersResponse->User,
		struct tt__User, count);

	index = 0;
	for (i = 0; i < MAX_USER_NUM; i++) {
		memset(&user, 0, sizeof(user_info_t));
		onvif_get_user_info(i, &user, sizeof(user_info_t));
		if (0 < strlen(user.username)) {
			ALLOC_STRUCT_NUM(
				tds__GetUsersResponse->User[index].Username,
				char, 64);
			snprintf(tds__GetUsersResponse->User[index].Username,
				64, "%s", user.username);
			ALLOC_STRUCT_NUM(
				tds__GetUsersResponse->User[index].Password,
				char, 64);
			snprintf(tds__GetUsersResponse->User[index].Password,
				64, "%s", user.password);
			tds__GetUsersResponse->User[index].UserLevel =
				user.level;
			tds__GetUsersResponse->User[index].Extension = NULL;
			tds__GetUsersResponse->User[index].__anyAttribute = NULL;
			index++;
		}
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__CreateUsers(struct soap *soap,
	struct _tds__CreateUsers *tds__CreateUsers,
	struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i;
	user_info_t user;

	printf("__sizeUser %d\n", tds__CreateUsers->__sizeUser);
	for (i = 0; i < tds__CreateUsers->__sizeUser; i++) {
		snprintf(user.username, sizeof(user.username),
			"%s",
			tds__CreateUsers->User[i].Username);
		snprintf(user.password, sizeof(user.password),
			"%s",
			tds__CreateUsers->User[i].Password);
		user.level = tds__CreateUsers->User[i].UserLevel;
		onvif_add_user_info(&user);
		printf("[%d] username %s Password %s level %d\n",
			i,
			tds__CreateUsers->User[i].Username,
			tds__CreateUsers->User[i].Password,
			tds__CreateUsers->User[i].UserLevel);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__DeleteUsers(struct soap* soap,
	struct _tds__DeleteUsers* tds__DeleteUsers,
	struct _tds__DeleteUsersResponse* tds__DeleteUsersResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i;

	printf("__sizeUsername %d\n", tds__DeleteUsers->__sizeUsername);
	for (i = 0; i < tds__DeleteUsers->__sizeUsername; i++) {
		onvif_delete_user_info(tds__DeleteUsers->Username[i]);
		printf("[%d] Username %s\n",
			i,
			tds__DeleteUsers->Username[i]);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetUser(struct soap* soap,
	struct _tds__SetUser* tds__SetUser,
	struct _tds__SetUserResponse* tds__SetUserResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i, ret = 0;
	user_info_t user;

	printf("__sizeUser %d\n", tds__SetUser->__sizeUser);
	for (i = 0; i < tds__SetUser->__sizeUser; i++) {
		snprintf(user.username, sizeof(user.username),
			"%s",
			tds__SetUser->User[i].Username);
		snprintf(user.password, sizeof(user.password),
			"%s",
			tds__SetUser->User[i].Password);
		user.level = tds__SetUser->User[i].UserLevel;
		ret += onvif_set_user_info(user.username, &user);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetWsdlUrl(struct soap* soap,
	struct _tds__GetWsdlUrl* tds__GetWsdlUrl,
	struct _tds__GetWsdlUrlResponse* tds__GetWsdlUrlResponse)
{
	tds__GetWsdlUrlResponse->WsdlUrl =
		"http://www.onvif.org/Documents/Specifications.aspx";
	return SOAP_OK;
}

int __tds__GetCapabilities(struct soap* soap,
	struct _tds__GetCapabilities *tds__GetCapabilities,
	struct _tds__GetCapabilitiesResponse
	*tds__GetCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	enum tt__CapabilityCategory eCategory = tt__CapabilityCategory__All;
	if (tds__GetCapabilities->Category)
		eCategory = *tds__GetCapabilities->Category;

	struct tt__Capabilities *pCapabilities =
		(struct tt__Capabilities *)soap_calloc(soap,
		sizeof(struct tt__Capabilities));

	struct tt__AnalyticsCapabilities *pAnalytics = NULL;
	struct tt__DeviceCapabilities *pDevice = NULL;
	struct tt__EventCapabilities *pEvents = NULL;
	struct tt__ImagingCapabilities *pImaging = NULL;
	struct tt__MediaCapabilities *pMedia = NULL;
	struct tt__PTZCapabilities *pPtz = NULL;
	struct tt__CapabilitiesExtension *pExtension = NULL;

	if (eCategory == tt__CapabilityCategory__All)
		pAnalytics =
			(struct tt__AnalyticsCapabilities *)soap_calloc(soap,
			sizeof(struct tt__AnalyticsCapabilities));

	if (eCategory == tt__CapabilityCategory__All ||
			eCategory == tt__CapabilityCategory__Device)
		pDevice = (struct tt__DeviceCapabilities *)soap_calloc(soap,
			sizeof(struct tt__DeviceCapabilities));

	if (eCategory == tt__CapabilityCategory__All ||
			eCategory == tt__CapabilityCategory__Events)
		pEvents = (struct tt__EventCapabilities *)soap_calloc(soap,
			sizeof(struct tt__EventCapabilities));
#if 0
	if (eCategory == tt__CapabilityCategory__All ||
			eCategory == tt__CapabilityCategory__Imaging)
		pImaging = (struct tt__ImagingCapabilities *)soap_calloc(soap,
			sizeof(struct tt__ImagingCapabilities));
#else
	if (eCategory == tt__CapabilityCategory__Events)
		return -1;

	if (eCategory == tt__CapabilityCategory__Imaging)
		return -1;
#endif
	if (eCategory == tt__CapabilityCategory__All ||
			eCategory == tt__CapabilityCategory__Media)
		pMedia = (struct tt__MediaCapabilities *)soap_calloc(soap,
			sizeof(struct tt__MediaCapabilities));

	if (eCategory == tt__CapabilityCategory__All ||
			eCategory == tt__CapabilityCategory__PTZ)
		pPtz = (struct tt__PTZCapabilities *)soap_calloc(soap,
			sizeof(struct tt__PTZCapabilities));

	if (eCategory == tt__CapabilityCategory__All)
		pExtension = ( struct tt__CapabilitiesExtension *)soap_calloc(soap,
			sizeof( struct tt__CapabilitiesExtension ));

	pCapabilities->Analytics = pAnalytics;
	pCapabilities->Device = pDevice;
	pCapabilities->Events = pEvents;
	pCapabilities->Imaging = pImaging;
	pCapabilities->Media = pMedia;
	pCapabilities->PTZ = pPtz;
	pCapabilities->Extension = pExtension;

	if (pAnalytics) {
		pAnalytics->XAddr = (char *)soap_calloc(soap, 256);
		sprintf(pAnalytics->XAddr,
			"http://%s:%u/onvif/analytics_service",
			onvif_get_ip_addr(),
			onvif_get_server_port());
		pAnalytics->RuleSupport = xsd__boolean__true_;
		pAnalytics->AnalyticsModuleSupport = xsd__boolean__false_;
	}

	if (pDevice) {
		struct tt__NetworkCapabilities *pNetwork =
			(struct tt__NetworkCapabilities *)soap_calloc(soap,
			sizeof(struct tt__NetworkCapabilities));
		struct tt__SystemCapabilities *pSystem =
			(struct tt__SystemCapabilities *)soap_calloc(soap,
			sizeof(struct tt__SystemCapabilities));
		struct tt__IOCapabilities *pIO = 
			(struct tt__IOCapabilities *)soap_calloc(soap,
			sizeof(struct tt__IOCapabilities));
		struct tt__SecurityCapabilities *pSecurity =
			(struct tt__SecurityCapabilities *)soap_calloc(soap,
			sizeof(struct tt__SecurityCapabilities));

		pDevice->Network = pNetwork;
		pDevice->System = pSystem;
		pDevice->IO = pIO;
		pDevice->Security = pSecurity;

		pDevice->XAddr 	= (char *)soap_calloc(soap, 256);
		sprintf(pDevice->XAddr, "http://%s:%u/onvif/device_service",
			onvif_get_ip_addr(),
			onvif_get_server_port());

		if (pNetwork) {
			pNetwork->IPFilter =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pNetwork->ZeroConfiguration =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pNetwork->IPVersion6 =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pNetwork->DynDNS =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pNetwork->Extension =
				(struct tt__NetworkCapabilitiesExtension *)
				soap_calloc(soap,
				sizeof(struct tt__NetworkCapabilitiesExtension));
			pNetwork->Extension->Dot11Configuration =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));

			*(pNetwork->IPFilter) = xsd__boolean__false_;
			*(pNetwork->ZeroConfiguration)= xsd__boolean__false_;
			*(pNetwork->IPVersion6) = xsd__boolean__false_;
			*(pNetwork->DynDNS) = xsd__boolean__false_;

			*pNetwork->Extension->Dot11Configuration =
				xsd__boolean__false_;
		}

		if (pSystem) {
			pSystem->DiscoveryResolve = xsd__boolean__true_;
			pSystem->DiscoveryBye = xsd__boolean__true_;
			pSystem->RemoteDiscovery = xsd__boolean__false_;
			pSystem->SystemBackup = xsd__boolean__false_;
			pSystem->SystemLogging = xsd__boolean__true_;
			pSystem->FirmwareUpgrade = xsd__boolean__false_;

			pSystem->__sizeSupportedVersions = 1;
			pSystem->SupportedVersions =
				(struct tt__OnvifVersion *)soap_calloc(soap,
				sizeof(struct tt__OnvifVersion));
			pSystem->SupportedVersions->Major =
				onvif_get_major_version();
			pSystem->SupportedVersions->Minor=
				onvif_get_minor_version();

			pSystem->Extension =
				(struct tt__SystemCapabilitiesExtension *)
				soap_calloc(soap,
				sizeof(struct tt__SystemCapabilitiesExtension));
			pSystem->Extension->HttpFirmwareUpgrade =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pSystem->Extension->HttpSystemBackup =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pSystem->Extension->HttpSystemLogging =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));
			pSystem->Extension->HttpSupportInformation =
				(enum xsd__boolean *)soap_calloc(soap,
				sizeof(enum xsd__boolean));

			*pSystem->Extension->HttpFirmwareUpgrade =
				xsd__boolean__false_;
			*pSystem->Extension->HttpSystemBackup =
				xsd__boolean__false_;
			*pSystem->Extension->HttpSystemLogging =
				xsd__boolean__false_;
			*pSystem->Extension->HttpSupportInformation =
				xsd__boolean__false_;
		}

		if (pIO) {
			pIO->InputConnectors = (int *)soap_calloc(soap,
					sizeof(int));
			pIO->RelayOutputs = (int *)soap_calloc(soap,
					sizeof(int));
			*(pIO->InputConnectors) = 0;
			*(pIO->RelayOutputs) = 0;
		}

		if (pSecurity) {
			pSecurity->TLS1_x002e1 = xsd__boolean__false_;
			pSecurity->TLS1_x002e2 = xsd__boolean__false_;
			pSecurity->OnboardKeyGeneration = xsd__boolean__false_;
			pSecurity->AccessPolicyConfig = xsd__boolean__false_;
			pSecurity->X_x002e509Token = xsd__boolean__false_;
			pSecurity->SAMLToken = xsd__boolean__false_;
			pSecurity->KerberosToken = xsd__boolean__false_;
			pSecurity->RELToken = xsd__boolean__false_;

			pSecurity->Extension =
				(struct tt__SecurityCapabilitiesExtension *)
				soap_calloc(soap,
				sizeof(struct tt__SecurityCapabilitiesExtension));
			pSecurity->Extension->TLS1_x002e0 = xsd__boolean__false_;
		}
	}

	if (pEvents) {
		pEvents->XAddr = (char *)soap_calloc(soap, 256);
		sprintf(pEvents->XAddr, "http://%s:%u/onvif/Events",
				onvif_get_ip_addr(),
				onvif_get_server_port());
		pEvents->WSPausableSubscriptionManagerInterfaceSupport =
				xsd__boolean__false_;
		pEvents->WSPullPointSupport = xsd__boolean__false_;
		pEvents->WSSubscriptionPolicySupport = xsd__boolean__false_;
	}

	if (pImaging) {
		pImaging->XAddr = (char *)soap_calloc(soap, 256);
		sprintf(pImaging->XAddr, "http://%s:%u/onvif/Imaging",
				onvif_get_ip_addr(),
				onvif_get_server_port());
	}

	if (pMedia) {
		pMedia->XAddr = (char *)soap_calloc(soap, 256);
		sprintf(pMedia->XAddr, "http://%s:%u/onvif/Media",
			onvif_get_ip_addr(),
			onvif_get_server_port());
		pMedia->StreamingCapabilities =
			(struct tt__RealTimeStreamingCapabilities *)
			soap_calloc(soap,
			sizeof(struct tt__RealTimeStreamingCapabilities));
		pMedia->StreamingCapabilities->RTPMulticast =
			(enum xsd__boolean *)soap_calloc(soap,
			sizeof(enum xsd__boolean));
		pMedia->StreamingCapabilities->RTP_USCORETCP =
			(enum xsd__boolean *)soap_calloc(soap,
			sizeof(enum xsd__boolean));
		pMedia->StreamingCapabilities->RTP_USCORERTSP_USCORETCP =
			(enum xsd__boolean *)soap_calloc(soap,
			sizeof(enum xsd__boolean));

		*(pMedia->StreamingCapabilities->RTPMulticast) =
			xsd__boolean__false_;
		*(pMedia->StreamingCapabilities->RTP_USCORETCP) =
			xsd__boolean__true_;
		*(pMedia->StreamingCapabilities->RTP_USCORERTSP_USCORETCP) =
			xsd__boolean__true_;

		pMedia->Extension =
			(struct tt__MediaCapabilitiesExtension *)soap_calloc(soap,
			sizeof(struct tt__MediaCapabilitiesExtension));
		pMedia->Extension->ProfileCapabilities =
			(struct tt__ProfileCapabilities *)soap_calloc(soap,
			sizeof(struct tt__ProfileCapabilities));
		pMedia->Extension->ProfileCapabilities->MaximumNumberOfProfiles = 4;
	}

	if (pPtz) {
		pPtz->XAddr = (char *)soap_calloc(soap, 256);
		sprintf(pPtz->XAddr, "http://%s:%u/onvif/PTZ",
			onvif_get_ip_addr(),
			onvif_get_server_port());
	}

	if (pExtension) {
		struct tt__DeviceIOCapabilities *pDeviceIO =
			(struct tt__DeviceIOCapabilities *)soap_calloc(soap,
			sizeof(struct tt__DeviceIOCapabilities));
		pExtension->DeviceIO = pDeviceIO;

		pDeviceIO->XAddr = (char *)soap_calloc(soap, 256);
		sprintf(pDeviceIO->XAddr, "http://%s:%u/onvif/DeviceIO",
			onvif_get_ip_addr(),
			onvif_get_server_port());

		pDeviceIO->VideoSources = 1;
		pDeviceIO->VideoOutputs = 0;
		pDeviceIO->AudioOutputs = 1;
		pDeviceIO->AudioOutputs = 1;
		pDeviceIO->RelayOutputs = 0;
	}

	tds__GetCapabilitiesResponse->Capabilities = pCapabilities;

	__FUN_END("\n");
	return 0;
}

int __tds__SetDPAddresses(struct soap *soap,
	struct _tds__SetDPAddresses *tds__SetDPAddresses,
	struct _tds__SetDPAddressesResponse
	*tds__SetDPAddressesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetHostname(struct soap *soap,
	struct _tds__GetHostname *tds__GetHostname,
	struct _tds__GetHostnameResponse
	*tds__GetHostnameResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	struct tt__HostnameInformation *HostnameInformation;

	ALLOC_STRUCT(HostnameInformation,
			struct tt__HostnameInformation);

	HostnameInformation->FromDHCP = xsd__boolean__false_;
	ALLOC_STRUCT_NUM(HostnameInformation->Name, char, 64);
	onvif_get_hostname(HostnameInformation->Name, 64);
	HostnameInformation->Extension = NULL;
	HostnameInformation->__anyAttribute = NULL;

	tds__GetHostnameResponse->HostnameInformation = HostnameInformation;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetHostname(struct soap *soap,
	struct _tds__SetHostname *tds__SetHostname,
	struct _tds__SetHostnameResponse *tds__SetHostnameResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	onvif_set_hostname(tds__SetHostname->Name);
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetHostnameFromDHCP(struct soap *soap,
	struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP,
	struct _tds__SetHostnameFromDHCPResponse
	*tds__SetHostnameFromDHCPResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetDNS(struct soap *soap,
	struct _tds__GetDNS *tds__GetDNS,
	struct _tds__GetDNSResponse *tds__GetDNSResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	net_param_t net_param;

	onvif_get_net_param(0, &net_param, sizeof(net_param_t));

	struct tt__DNSInformation * pDNSInformation;
	ALLOC_STRUCT(pDNSInformation,
			struct tt__DNSInformation);
	pDNSInformation->FromDHCP = xsd__boolean__true_;
	pDNSInformation->__sizeSearchDomain = 0;
	pDNSInformation->SearchDomain = NULL;
	pDNSInformation->__sizeDNSFromDHCP = 2;
	ALLOC_STRUCT_NUM(pDNSInformation->DNSFromDHCP,
			struct tt__IPAddress,
			pDNSInformation->__sizeDNSFromDHCP);
	pDNSInformation->DNSFromDHCP[0].Type= tt__IPType__IPv4;
#if 0
	pDNSInformation->DNSFromDHCP[0].IPv4Address = "8.8.8.8";
#else
	ALLOC_STRUCT_NUM(pDNSInformation->DNSFromDHCP[0].IPv4Address,
			char, 64);
	strcpy(pDNSInformation->DNSFromDHCP[0].IPv4Address,
			net_param.dns1);
#endif
	pDNSInformation->DNSFromDHCP[0].IPv6Address = NULL;

	pDNSInformation->DNSFromDHCP[1].Type= tt__IPType__IPv4;
#if 0
	pDNSInformation->DNSFromDHCP[1].IPv4Address = "114.114.114.114";
#else
	ALLOC_STRUCT_NUM(pDNSInformation->DNSFromDHCP[1].IPv4Address,
			char, 64);
	strcpy(pDNSInformation->DNSFromDHCP[1].IPv4Address,
			net_param.dns2);
#endif
	pDNSInformation->DNSFromDHCP[1].IPv6Address = NULL;

	pDNSInformation->__sizeDNSManual = 0;
	pDNSInformation->DNSManual = NULL;
	pDNSInformation->Extension = NULL;
	pDNSInformation->__anyAttribute = NULL;

	tds__GetDNSResponse->DNSInformation = pDNSInformation;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetDNS(struct soap *soap,
	struct _tds__SetDNS *tds__SetDNS,
	struct _tds__SetDNSResponse *tds__SetDNSResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	net_param_t net_param;

	onvif_get_net_param(0, &net_param, sizeof(net_param_t));
	if (0 == tds__SetDNS->FromDHCP) {
		if (2 == tds__SetDNS->__sizeDNSManual) {
			if (tt__IPType__IPv4 ==
				tds__SetDNS->DNSManual[0].Type) {
				printf("IPv4Address[0] %s\n", 
					tds__SetDNS->DNSManual[0].IPv4Address);
				snprintf(net_param.dns1,
					sizeof(net_param.dns1),
					"%s",
					tds__SetDNS->DNSManual[0].IPv4Address);
			}
			if (tt__IPType__IPv4 ==
				tds__SetDNS->DNSManual[1].Type) {
				printf("IPv4Address[1] %s\n", 
					tds__SetDNS->DNSManual[1].IPv4Address);
				snprintf(net_param.dns2,
					sizeof(net_param.dns2),
					"%s",
					tds__SetDNS->DNSManual[1].IPv4Address);
			}
		}
	}
	onvif_set_net_param(0, &net_param);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetNTP(struct soap *soap,
	struct _tds__GetNTP *tds__GetNTP,
	struct _tds__GetNTPResponse *tds__GetNTPResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	struct tt__NTPInformation *NTPInformation;

	ALLOC_STRUCT(NTPInformation,
			struct tt__NTPInformation);

	NTPInformation->FromDHCP = xsd__boolean__false_;
	NTPInformation->__sizeNTPFromDHCP = 0;
	NTPInformation->NTPFromDHCP = NULL;
	NTPInformation->__sizeNTPManual = 1;
	ALLOC_STRUCT_NUM(NTPInformation->NTPManual,
				struct tt__NetworkHost, 1);

	NTPInformation->NTPManual[0].Type = tt__NetworkHostType__IPv4;
	ALLOC_STRUCT_NUM(NTPInformation->NTPManual[0].IPv4Address,
				char, 64);
	onvif_get_ntp_server(NTPInformation->NTPManual[0].IPv4Address, 64);
	NTPInformation->NTPManual[0].DNSname = NULL;

	NTPInformation->NTPManual[0].IPv6Address = NULL;
	NTPInformation->NTPManual[0].Extension = NULL;
	NTPInformation->NTPManual[0].__anyAttribute = NULL;

	NTPInformation->Extension = NULL;
	NTPInformation->__anyAttribute = NULL;

	tds__GetNTPResponse->NTPInformation = NTPInformation;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetNTP(struct soap *soap,
	struct _tds__SetNTP *tds__SetNTP,
	struct _tds__SetNTPResponse *tds__SetNTPResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i;
	printf("FromDHCP %d\n", tds__SetNTP->FromDHCP);

	printf("__sizeNTPManual %d\n", tds__SetNTP->__sizeNTPManual);
	for (i=0; i<tds__SetNTP->__sizeNTPManual; i++) {
		printf(" type[%d] %d\n", i, tds__SetNTP->NTPManual[i].Type);
		switch (tds__SetNTP->NTPManual[i].Type) {
		case tt__NetworkHostType__IPv4:
			printf(" IPv4Address[%d] %s\n",
				i, tds__SetNTP->NTPManual[i].IPv4Address);
			onvif_set_ntp_server(
				tds__SetNTP->NTPManual[i].IPv4Address);
			break;

		case tt__NetworkHostType__IPv6:
			printf(" IPv6Address[%d] %s\n",
				i, tds__SetNTP->NTPManual[i].IPv6Address);
			break;

		case tt__NetworkHostType__DNS:
			printf(" DNSname[%d] %s\n",
				i, tds__SetNTP->NTPManual[i].DNSname);
			onvif_set_ntp_server(
				tds__SetNTP->NTPManual[i].DNSname);
			break;
		}
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetDynamicDNS(struct soap *soap,
	struct _tds__GetDynamicDNS *tds__GetDynamicDNS,
	struct _tds__GetDynamicDNSResponse
	*tds__GetDynamicDNSResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetDynamicDNS(struct soap *soap,
	struct _tds__SetDynamicDNS *tds__SetDynamicDNS,
	struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetNetworkInterfaces(struct soap *soap,
	struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces,
	struct _tds__GetNetworkInterfacesResponse
	*tds__GetNetworkInterfacesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	
	int ret;
	net_param_t net_param;
	char mac_addr[8];

	struct tt__NetworkInterface *pNetworkInterfaces;
	ALLOC_STRUCT_NUM(pNetworkInterfaces,
			struct tt__NetworkInterface, 1 /*2*/);

	onvif_get_net_param(0, &net_param, sizeof(net_param_t));
	ret = onvif_get_mac_addr(mac_addr, 6);
	pNetworkInterfaces[0].Enabled = 1;
	ALLOC_STRUCT_NUM(pNetworkInterfaces[0].token, char, 64);
	strcpy(pNetworkInterfaces[0].token, "eth0");
	ALLOC_STRUCT(pNetworkInterfaces[0].Info,
		struct tt__NetworkInterfaceInfo);
	ALLOC_STRUCT_NUM(pNetworkInterfaces[0].Info->Name, char, 64);
	strcpy(pNetworkInterfaces[0].Info->Name, "eth0");
	ALLOC_STRUCT_NUM(pNetworkInterfaces[0].Info->HwAddress, char, 64);
	if (0 == ret)
		sprintf(pNetworkInterfaces[0].Info->HwAddress,
			"%02X:%02X:%02X:%02X:%02X:%02X",
			mac_addr[0], mac_addr[1], mac_addr[2],
			mac_addr[3], mac_addr[4], mac_addr[5]);
	else
		strcpy(pNetworkInterfaces[0].Info->HwAddress,
			"ab:ef:12:22:ff:12");
	ALLOC_STRUCT_NUM(pNetworkInterfaces[0].Info->MTU, int, 1);
	*(pNetworkInterfaces[0].Info->MTU) = 1440;

	ALLOC_STRUCT(pNetworkInterfaces[0].IPv4,
		struct tt__IPv4NetworkInterface);
	pNetworkInterfaces[0].IPv4->Enabled = 1;
	ALLOC_STRUCT(pNetworkInterfaces[0].IPv4->Config,
		struct tt__IPv4Configuration);
	pNetworkInterfaces[0].IPv4->Config->DHCP = 1;

	ALLOC_STRUCT(pNetworkInterfaces[0].IPv4->Config->FromDHCP,
		struct tt__PrefixedIPv4Address);

	ALLOC_STRUCT_NUM(pNetworkInterfaces[0].IPv4->Config->
		FromDHCP->Address, char, 64);
	strcpy(pNetworkInterfaces[0].IPv4->Config->FromDHCP->Address,
			net_param.ip_addr);
	pNetworkInterfaces[0].IPv4->Config->FromDHCP->PrefixLength = 24;

#if 1
	tds__GetNetworkInterfacesResponse->__sizeNetworkInterfaces = 1;
#else
	tds__GetNetworkInterfacesResponse->__sizeNetworkInterfaces = 2;
#endif
	tds__GetNetworkInterfacesResponse->NetworkInterfaces =
		pNetworkInterfaces;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetNetworkInterfaces(struct soap *soap,
	struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces,
	struct _tds__SetNetworkInterfacesResponse
	*tds__SetNetworkInterfacesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int is_wifi;
	net_param_t net_param;

	printf("	token %s\n", tds__SetNetworkInterfaces->InterfaceToken);

	if (0 == strcmp(tds__SetNetworkInterfaces->InterfaceToken, "eth0"))
		is_wifi = 0;
		
	else
		is_wifi = 1;
	onvif_get_net_param(is_wifi, &net_param, sizeof(net_param_t));
	if (tds__SetNetworkInterfaces->NetworkInterface) {
		if (tds__SetNetworkInterfaces->NetworkInterface->Enabled)
			printf("		enabled %d\n",
				*tds__SetNetworkInterfaces->
				NetworkInterface->Enabled);

		if (tds__SetNetworkInterfaces->NetworkInterface->Link)
			printf("		Link is not null.\n");

		if (tds__SetNetworkInterfaces->NetworkInterface->IPv4) {
			printf("		IPV4 info:\n");
			if (tds__SetNetworkInterfaces->
					NetworkInterface->IPv4->Enabled)
				printf("			enabled %d\n",
					*tds__SetNetworkInterfaces->
					NetworkInterface->IPv4->Enabled);

			if (tds__SetNetworkInterfaces->
					NetworkInterface->IPv4->Manual) {
				printf("			Manual ");
				if (tds__SetNetworkInterfaces->
						NetworkInterface->
						IPv4->Manual->Address) {
					net_param.enable_dhcp = 0;
					snprintf(net_param.ip_addr, 
						sizeof(net_param.ip_addr),
						"%s",
						tds__SetNetworkInterfaces->
						NetworkInterface->IPv4->
						Manual->Address);
					set_local_ip("eth0",
						tds__SetNetworkInterfaces->
						NetworkInterface->IPv4->
						Manual->Address);
					printf("ip %s ",
						tds__SetNetworkInterfaces->
						NetworkInterface->IPv4->
						Manual->Address);
				}
				printf("PrefixLength %d\n",
					tds__SetNetworkInterfaces->
					NetworkInterface->IPv4->
					Manual->PrefixLength);
			}
					
			if (tds__SetNetworkInterfaces->
					NetworkInterface->IPv4->DHCP) {
				net_param.enable_dhcp = 1;
				printf("			DHCP %d\n",
					*tds__SetNetworkInterfaces->
					NetworkInterface->IPv4->DHCP);
			}
		}

		if (tds__SetNetworkInterfaces->NetworkInterface->IPv6)
			printf("		IPv6 is not null.\n");
		if (tds__SetNetworkInterfaces->NetworkInterface->Extension)
			printf("		Extension is not null.\n");
		if (tds__SetNetworkInterfaces->
				NetworkInterface->__anyAttribute)
			printf("		__anyAttribute %s.\n",
				tds__SetNetworkInterfaces->
				NetworkInterface->__anyAttribute);
	}

	onvif_set_net_param(is_wifi, &net_param);

	tds__SetNetworkInterfacesResponse->RebootNeeded = xsd__boolean__false_;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetNetworkProtocols(struct soap *soap,
	struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols,
	struct _tds__GetNetworkProtocolsResponse
	*tds__GetNetworkProtocolsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	struct tt__NetworkProtocol *NetworkProtocols;
	ALLOC_STRUCT_NUM(NetworkProtocols,
			struct tt__NetworkProtocol, 2);

	NetworkProtocols[0].Name = tt__NetworkProtocolType__HTTP;
	NetworkProtocols[0].Enabled = xsd__boolean__true_;
	NetworkProtocols[0].__sizePort = 1;
	ALLOC_STRUCT_NUM(NetworkProtocols[0].Port, int, 2);
#if 1
	onvif_get_http_port(&NetworkProtocols[0].Port[0]);
#else
	NetworkProtocols[0].Port[0] = 3002;
#endif
	NetworkProtocols[0].__anyAttribute = NULL;

	NetworkProtocols[1].Name = tt__NetworkProtocolType__RTSP;
	NetworkProtocols[1].Enabled = xsd__boolean__true_;
	NetworkProtocols[1].__sizePort = 1;
	ALLOC_STRUCT_NUM(NetworkProtocols[1].Port, int, 2);
#if 1
	onvif_get_rtsp_port(&NetworkProtocols[1].Port[0]);
#else
	NetworkProtocols[1].Port[0] = 554;
#endif
	NetworkProtocols[1].__anyAttribute = NULL;

	tds__GetNetworkProtocolsResponse->__sizeNetworkProtocols = 2;
	tds__GetNetworkProtocolsResponse->NetworkProtocols = NetworkProtocols;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetNetworkProtocols(struct soap *soap,
	struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols,
	struct _tds__SetNetworkProtocolsResponse
	*tds__SetNetworkProtocolsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i;
	struct tt__NetworkProtocol *netProtocol;
	
	printf("__sizeNetworkProtocols %d\n",
		tds__SetNetworkProtocols->__sizeNetworkProtocols);
	for (i = 0; i < tds__SetNetworkProtocols->__sizeNetworkProtocols; i++) {
		netProtocol = &tds__SetNetworkProtocols->NetworkProtocols[i];
		if (netProtocol->Enabled) {
			switch (netProtocol->Name) {
			case tt__NetworkProtocolType__HTTP:
				if (netProtocol->__sizePort >= 1)
					onvif_set_http_port(netProtocol->Port[0]);
				break;

			case tt__NetworkProtocolType__RTSP:
				if (netProtocol->__sizePort >= 1)
					onvif_set_rtsp_port(netProtocol->Port[0]);
				break;

			default:
				break;
			}
		}
	}
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetNetworkDefaultGateway(struct soap *soap,
	struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway,
	struct _tds__GetNetworkDefaultGatewayResponse
	*tds__GetNetworkDefaultGatewayResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	net_param_t net_param;
	struct tt__NetworkGateway *net_gateway = NULL;

	onvif_get_net_param(0, &net_param, sizeof(net_param_t));

	ALLOC_STRUCT(net_gateway, struct tt__NetworkGateway);
	net_gateway->__sizeIPv4Address = 1;
	ALLOC_STRUCT_NUM(net_gateway->IPv4Address,
		char*, net_gateway->__sizeIPv4Address);
	ALLOC_STRUCT_NUM(net_gateway->IPv4Address[0], char, 64);
	snprintf(net_gateway->IPv4Address[0], 64, "%s", net_param.gateway);
	net_gateway->IPv6Address = NULL;
	net_gateway->__sizeIPv6Address = 0;

	tds__GetNetworkDefaultGatewayResponse->NetworkGateway = net_gateway;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetNetworkDefaultGateway(struct soap *soap,
	struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway,
	struct _tds__SetNetworkDefaultGatewayResponse
	*tds__SetNetworkDefaultGatewayResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i;
	net_param_t net_param;

	onvif_get_net_param(0, &net_param, sizeof(net_param_t));

	if (1 == tds__SetNetworkDefaultGateway->__sizeIPv4Address) {
		snprintf(net_param.gateway, sizeof(net_param.gateway),
			"%s", tds__SetNetworkDefaultGateway->IPv4Address[0]);
		onvif_set_net_param(0, &net_param);
	}

	printf("__sizeIPv4Address %d\n",
		tds__SetNetworkDefaultGateway->__sizeIPv4Address);
	for (i=0; i<tds__SetNetworkDefaultGateway->__sizeIPv4Address; i++) {
		printf("IPv4Address[%d] %s\n", i,
			tds__SetNetworkDefaultGateway->IPv4Address[i]);
	}
	for (i=0; i<tds__SetNetworkDefaultGateway->__sizeIPv6Address; i++) {
		printf("IPv6Address[%d] %s\n", i,
			tds__SetNetworkDefaultGateway->IPv6Address[i]);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__GetZeroConfiguration(struct soap *soap,
	struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration,
	struct _tds__GetZeroConfigurationResponse
	*tds__GetZeroConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetZeroConfiguration(struct soap *soap,
	struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration,
	struct _tds__SetZeroConfigurationResponse
	*tds__SetZeroConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	printf("the interface Token is %s\r\n",
		tds__SetZeroConfiguration->InterfaceToken);
	printf("the Enabled Token is %d\r\n",
		tds__SetZeroConfiguration->Enabled);
	__FUN_END("\n");
	return SOAP_OK;
}

int  __tds__GetIPAddressFilter(struct soap *soap,
	struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter,
	struct _tds__GetIPAddressFilterResponse
	*tds__GetIPAddressFilterResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetIPAddressFilter(struct soap *soap,
	struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter,
	struct _tds__SetIPAddressFilterResponse
	*tds__SetIPAddressFilterResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__AddIPAddressFilter(struct soap *soap,
	struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter,
	struct _tds__AddIPAddressFilterResponse
	*tds__AddIPAddressFilterResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__RemoveIPAddressFilter(struct soap *soap,
	struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter,
	struct _tds__RemoveIPAddressFilterResponse
	*tds__RemoveIPAddressFilterResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetAccessPolicy(struct soap *soap,
	struct _tds__GetAccessPolicy *tds__GetAccessPolicy,
	struct _tds__GetAccessPolicyResponse
	*tds__GetAccessPolicyResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetAccessPolicy(struct soap *soap,
	struct _tds__SetAccessPolicy *tds__SetAccessPolicy,
	struct _tds__SetAccessPolicyResponse
	*tds__SetAccessPolicyResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__CreateCertificate(struct soap *soap,
	struct _tds__CreateCertificate *tds__CreateCertificate,
	struct _tds__CreateCertificateResponse
	*tds__CreateCertificateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetCertificates(struct soap *soap,
	struct _tds__GetCertificates *tds__GetCertificates,
	struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	tds__GetCertificatesResponse->__sizeNvtCertificate = 0;
	tds__GetCertificatesResponse->NvtCertificate = NULL;
	return SOAP_OK;
}

int __tds__GetCertificatesStatus(struct soap *soap,
	struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus,
	struct _tds__GetCertificatesStatusResponse
	*tds__GetCertificatesStatusResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetCertificatesStatus(struct soap *soap,
	struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus,
	struct _tds__SetCertificatesStatusResponse
	*tds__SetCertificatesStatusResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__DeleteCertificates(struct soap *soap,
	struct _tds__DeleteCertificates *tds__DeleteCertificates,
	struct _tds__DeleteCertificatesResponse
	*tds__DeleteCertificatesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetPkcs10Request(struct soap *soap,
	struct _tds__GetPkcs10Request *tds__GetPkcs10Request,
	struct _tds__GetPkcs10RequestResponse
	*tds__GetPkcs10RequestResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__LoadCertificates(struct soap *soap,
	struct _tds__LoadCertificates *tds__LoadCertificates,
	struct _tds__LoadCertificatesResponse
	*tds__LoadCertificatesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetClientCertificateMode(struct soap *soap,
	struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode,
	struct _tds__GetClientCertificateModeResponse
	*tds__GetClientCertificateModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetClientCertificateMode(struct soap *soap,
	struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode,
	struct _tds__SetClientCertificateModeResponse
	*tds__SetClientCertificateModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetRelayOutputs(struct soap *soap,
	struct _tds__GetRelayOutputs *tds__GetRelayOutputs,
	struct _tds__GetRelayOutputsResponse
	*tds__GetRelayOutputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetRelayOutputSettings(struct soap *soap,
	struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings,
	struct _tds__SetRelayOutputSettingsResponse
	*tds__SetRelayOutputSettingsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SetRelayOutputState(struct soap *soap,
	struct _tds__SetRelayOutputState *tds__SetRelayOutputState,
	struct _tds__SetRelayOutputStateResponse
	*tds__SetRelayOutputStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tds__SendAuxiliaryCommand(struct soap *soap,
	struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand,
	struct _tds__SendAuxiliaryCommandResponse
	*tds__SendAuxiliaryCommandResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetCACertificates(struct soap *soap,
	struct _tds__GetCACertificates *tds__GetCACertificates,
	struct _tds__GetCACertificatesResponse
	*tds__GetCACertificatesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__LoadCertificateWithPrivateKey(struct soap *soap,
	struct _tds__LoadCertificateWithPrivateKey
	*tds__LoadCertificateWithPrivateKey,
	struct _tds__LoadCertificateWithPrivateKeyResponse
	*tds__LoadCertificateWithPrivateKeyResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetCertificateInformation(struct soap *soap,
	struct _tds__GetCertificateInformation
	*tds__GetCertificateInformation,
	struct _tds__GetCertificateInformationResponse
	*tds__GetCertificateInformationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__LoadCACertificates(struct soap *soap,
	struct _tds__LoadCACertificates *tds__LoadCACertificates,
	struct _tds__LoadCACertificatesResponse
	*tds__LoadCACertificatesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__CreateDot1XConfiguration(struct soap *soap,
	struct _tds__CreateDot1XConfiguration
	*tds__CreateDot1XConfiguration,
	struct _tds__CreateDot1XConfigurationResponse
	*tds__CreateDot1XConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__SetDot1XConfiguration(struct soap *soap,
	struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration,
	struct _tds__SetDot1XConfigurationResponse
	*tds__SetDot1XConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetDot1XConfiguration(struct soap *soap,
	struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration,
	struct _tds__GetDot1XConfigurationResponse
	*tds__GetDot1XConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetDot1XConfigurations(struct soap *soap,
	struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations,
	struct _tds__GetDot1XConfigurationsResponse
	*tds__GetDot1XConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__DeleteDot1XConfiguration(struct soap *soap,
	struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration,
	struct _tds__DeleteDot1XConfigurationResponse
	*tds__DeleteDot1XConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetDot11Capabilities(struct soap *soap,
	struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities,
	struct _tds__GetDot11CapabilitiesResponse
	*tds__GetDot11CapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetDot11Status(struct soap *soap,
	struct _tds__GetDot11Status *tds__GetDot11Status,
	struct _tds__GetDot11StatusResponse
	*tds__GetDot11StatusResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__ScanAvailableDot11Networks(struct soap *soap,
	struct _tds__ScanAvailableDot11Networks
	*tds__ScanAvailableDot11Networks,
	struct _tds__ScanAvailableDot11NetworksResponse
	*tds__ScanAvailableDot11NetworksResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__GetSystemUris(struct soap *soap,
	struct _tds__GetSystemUris *tds__GetSystemUris,
	struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__StartFirmwareUpgrade(struct soap *soap,
	struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade,
	struct _tds__StartFirmwareUpgradeResponse
	*tds__StartFirmwareUpgradeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tds__StartSystemRestore(struct soap *soap,
	struct _tds__StartSystemRestore *tds__StartSystemRestore,
	struct _tds__StartSystemRestoreResponse
	*tds__StartSystemRestoreResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

static int __waitforEvent(int nTimeout, HI_ALARM_INFO_S *pstAlarm)
{
	int ret = 0;
	time_t t1;
	t1 = time(NULL);

	while(1) {
		if (time(NULL) - t1 >= nTimeout) {
			ret = -1;
			break;
		}

		ret = _onvif_getAlarm(pstAlarm);
		if (ret != 0) {
			sleep(1);
			continue;
		}

		return 0;
	}

	return ret;
}

int __tev__PullMessages(struct soap *soap,
	struct _tev__PullMessages *tev__PullMessages,
	struct _tev__PullMessagesResponse *tev__PullMessagesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int nRet = 0;
	HI_ALARM_INFO_S stAlarm;
	tev_topic_t *pstTopic = NULL;
	tev_Subscription_t *pSubscription = NULL;

	if (tev__PullMessages == NULL)
		return SOAP_FAULT;

	int nTimeout = tev__PullMessages->Timeout;
	char* tmp = NULL;

	if (soap->header != NULL) {
		if (soap->header->wsa5__To != NULL)
			tmp = strstr(soap->header->wsa5__To, "?subscribe=");
	}

	if (tmp == NULL)
		return SOAP_FAULT;

	nTimeout = 20;
	tmp += strlen("?subscribe=");
	int id = atoi(tmp);
	printf("nTimeout = %d\r\n", nTimeout);
	nRet = __waitforEvent(nTimeout, &stAlarm);
	printf("__waitforEvent ret= %d, type = %d\r\n",
		nRet, stAlarm.u32AlarmType);
	pSubscription =
		tev_GetSubscriptionFromManager(&g_tev_SubscriptionManager, id);
	if (pSubscription == NULL)
		return SOAP_FAULT;

	tev__PullMessagesResponse->CurrentTime = time(NULL);
	tev__PullMessagesResponse->TerminationTime =
		pSubscription->tick + tev__PullMessagesResponse->CurrentTime;

	if (nRet == 0) {
		time_t b_time;
		struct tm* tim;
		char* topicAny = NULL;

		if (stAlarm.u32AlarmType == HI_MD_ALARM_HAPPEN) {
			topicAny = "MotionAlarm";
			pstTopic = &g_tev_TopicSet[0];
		} else if (stAlarm.u32AlarmType == HI_DI_ALARM_HAPPEN) {
			topicAny = "IoAlarm";
			pstTopic = &g_tev_TopicSet[1];
		} else
			return SOAP_FAULT;
		
		b_time = time(NULL);
		tim = localtime(&b_time);
		tev__PullMessagesResponse->__sizeNotificationMessage = 1;
		tev__PullMessagesResponse->wsnt__NotificationMessage =
			soap_malloc(soap,
			1 * sizeof(struct wsnt__NotificationMessageHolderType));
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			SubscriptionReference = NULL;
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			ProducerReference = NULL;
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].Topic =
			soap_malloc(soap,
			sizeof(struct wsnt__TopicExpressionType));
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Topic->Dialect = soap_malloc(soap, 128);
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Topic->__any = soap_malloc(soap, 128);
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Topic->__anyAttribute = NULL;
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Topic->__mixed = NULL;
		strcpy(tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Topic->Dialect,
			"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Topic->__any, topicAny);
		tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Message.__any = soap_malloc(soap, 1024);
		sprintf(tev__PullMessagesResponse->wsnt__NotificationMessage[0].
			Message.__any,
			"<tt:Message UtcTime=\"%d-%02d-%02dT%02d:%02d:%02d.%d\" "
			"PropertyOperation=\"Initialized\">"
			"<tt:Source>"
			"<tt:SimpleItem Name=\"%s\" Value=\"%s\"/>"
			"</tt:Source>"
			"<tt:Data>"
			"<tt:SimpleItem Name=\"%s\" Value=\"%s\" />"
			"</tt:Data>"
			"</tt:Message>",
			(tim->tm_year + 1900), (tim->tm_mon + 1),
			tim->tm_mday, tim->tm_hour,
			tim->tm_min, tim->tm_sec, 100,
			pstTopic->source.name, pstTopic->source.type,
			pstTopic->data.name,
			pstTopic->data.type);
	}

	if (soap->header) {
		soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action = (char*)
		"http://www.onvif.org/ver10/events/wsdl/PullPointSubscription"
		"/PullMessagesResponse";
	}

	__FUN_END("\n");
	return SOAP_OK;
}

int __tev__Seek(struct soap *soap,
	struct _tev__Seek *tev__Seek,
	struct _tev__SeekResponse *tev__SeekResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tev__SetSynchronizationPoint(struct soap *soap,
	struct _tev__SetSynchronizationPoint *tev__SetSynchronizationPoint,
	struct _tev__SetSynchronizationPointResponse
	*tev__SetSynchronizationPointResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tev__GetServiceCapabilities(struct soap *soap,
	struct _tev__GetServiceCapabilities *tev__GetServiceCapabilities,
	struct _tev__GetServiceCapabilitiesResponse
	*tev__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__CreatePullPointSubscription(struct soap *soap,
	struct _tev__CreatePullPointSubscription
	*tev__CreatePullPointSubscription,
	struct _tev__CreatePullPointSubscriptionResponse
	*tev__CreatePullPointSubscriptionResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__GetEventProperties(struct soap *soap,
	struct _tev__GetEventProperties *tev__GetEventProperties,
	struct _tev__GetEventPropertiesResponse
	*tev__GetEventPropertiesResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	tev__GetEventPropertiesResponse->__sizeTopicNamespaceLocation =
		g_tev_TopicNamespaceLocationNum;
	tev__GetEventPropertiesResponse->TopicNamespaceLocation =
		g_tev_TopicNamespaceLocation;
	tev__GetEventPropertiesResponse->wsnt__FixedTopicSet =
		xsd__boolean__true_;
	tev__GetEventPropertiesResponse->wstop__TopicSet =
		(struct wstop__TopicSetType*)
		soap_malloc(soap, sizeof(struct wstop__TopicSetType));
	tev__GetEventPropertiesResponse->wstop__TopicSet->documentation =
		NULL;
	tev__GetEventPropertiesResponse->wstop__TopicSet->__size =
		g_tev_TopicNum;
	tev__GetEventPropertiesResponse->wstop__TopicSet->__any =
		(char**)soap_malloc(soap, sizeof(char*)*g_tev_TopicNum);
	tev__GetEventPropertiesResponse->wstop__TopicSet->__anyAttribute =
		NULL;
	int i = 0;

	for (i = 0; i < g_tev_TopicNum; i++) {
		tev__GetEventPropertiesResponse->wstop__TopicSet->__any[i] =
			(char *)soap_malloc(soap, 1024);
		memset(tev__GetEventPropertiesResponse->wstop__TopicSet->
			__any[i], 0, DEV_LINE_LEN);
		strcpy(tev__GetEventPropertiesResponse->wstop__TopicSet->
			__any[i], topic_set[i]);
	}

	tev__GetEventPropertiesResponse->__sizeTopicExpressionDialect =
		g_tev_TopicExpressionDialectNum;
	tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect =
		g_tev_TopicExpressionDialect;
	tev__GetEventPropertiesResponse->__sizeMessageContentFilterDialect =
		g_tev_MessageContentFilterDialectNum;
	tev__GetEventPropertiesResponse->MessageContentFilterDialect =
		g_tev_MessageContentFilterDialect;
	tev__GetEventPropertiesResponse->__sizeProducerPropertiesFilterDialect = 0;
	tev__GetEventPropertiesResponse->ProducerPropertiesFilterDialect = NULL;
	tev__GetEventPropertiesResponse->__sizeMessageContentSchemaLocation =
		g_tev_MessageContentSchemaLocationNum;
	tev__GetEventPropertiesResponse->MessageContentSchemaLocation =
		g_tev_MessageContentSchemaLocation;
	tev__GetEventPropertiesResponse->__size = 0;
	tev__GetEventPropertiesResponse->__any = NULL;

	if (soap->header) {
		soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action = (char*)
			"http://www.onvif.org/ver10/events/wsdl/EventPortType/"
			"GetEventPropertiesResponse";
	}

	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__Renew(struct soap *soap,
	struct _wsnt__Renew *wsnt__Renew,
	struct _wsnt__RenewResponse *wsnt__RenewResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	tev_Subscription_t *pstSubscription = NULL;
	int nPostpone = 0;

	if (soap->header == NULL || soap->header->wsa5__To == NULL)
		return SOAP_FAULT;

	printf("%s\r\n", soap->header->wsa5__To);
	char* tmp = strstr(soap->header->wsa5__To, "device_service/");
	if (tmp == NULL)
		return SOAP_FAULT;

	if (wsnt__Renew->TerminationTime)
		nPostpone = __ns11__Parse_Duration_Type(
			wsnt__Renew->TerminationTime);

	tmp += strlen("device_service/");
	int id = atoi(tmp);
	nPostpone = 60;

	if (tev_RenewSubscriptionFromManager(
		&g_tev_SubscriptionManager, id, nPostpone) < 0)
		return SOAP_FAULT;

	pstSubscription = tev_GetSubscriptionFromManager(
		&g_tev_SubscriptionManager, id);

	if (pstSubscription == NULL)
		return SOAP_FAULT;

	printf("renew postpone %d, tick = %d\r\n",
		nPostpone, pstSubscription->tick);
	wsnt__RenewResponse->CurrentTime =
		(time_t*)soap_malloc(soap, sizeof(time_t));
	*wsnt__RenewResponse->CurrentTime = time(NULL);
	wsnt__RenewResponse->TerminationTime = 
		*wsnt__RenewResponse->CurrentTime +
		pstSubscription->tick;

	if (soap->header) {
		soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action = (char*)
			"http://docs.oasis-open.org/wsn/bw-2/"
			"SubscriptionManager/RenewResponse";
	}

	__FUN_END("\n");
	return SOAP_OK;
}

int __tev__Unsubscribe(struct soap *soap,
	struct _wsnt__Unsubscribe *wsnt__Unsubscribe,
	struct _wsnt__UnsubscribeResponse
	*wsnt__UnsubscribeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (soap->header == NULL || soap->header->wsa5__To == NULL)
		return SOAP_FAULT;

	char* tmp = strstr(soap->header->wsa5__To, "device_service/");
	if (tmp == NULL)
		return SOAP_FAULT;

	tmp += strlen("device_service/");
	int id = atoi(tmp);

	if (tev_DeleteSubscriptionFromManager(
		&g_tev_SubscriptionManager, id) < 0)
		return SOAP_FAULT;
	wsnt__UnsubscribeResponse->__size = 0;
	wsnt__UnsubscribeResponse->__any = NULL;

	if (soap->header) {
		soap_default_SOAP_ENV__Header(soap, soap->header);
		soap->header->wsa5__Action = (char*)
			"http://docs.oasis-open.org/wsn/bw-2/"
			"SubscriptionManager/UnsubscribeResponse";
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tev__Subscribe(struct soap *soap,
	struct _wsnt__Subscribe *wsnt__Subscribe,
	struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tev__GetCurrentMessage(struct soap *soap,
	struct _wsnt__GetCurrentMessage *wsnt__GetCurrentMessage,
	struct _wsnt__GetCurrentMessageResponse
	*wsnt__GetCurrentMessageResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__Notify(struct soap *soap, struct _wsnt__Notify *wsnt__Notify)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__GetMessages(struct soap *soap,
	struct _wsnt__GetMessages *wsnt__GetMessages,
	struct _wsnt__GetMessagesResponse *wsnt__GetMessagesResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__DestroyPullPoint(struct soap *soap,
	struct _wsnt__DestroyPullPoint *wsnt__DestroyPullPoint,
	struct _wsnt__DestroyPullPointResponse
	*wsnt__DestroyPullPointResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__Notify_(struct soap *soap, struct _wsnt__Notify *wsnt__Notify)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__CreatePullPoint(struct soap *soap,
	struct _wsnt__CreatePullPoint *wsnt__CreatePullPoint,
	struct _wsnt__CreatePullPointResponse
	*wsnt__CreatePullPointResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__Renew_(struct soap *soap,
	struct _wsnt__Renew *wsnt__Renew,
	struct _wsnt__RenewResponse *wsnt__RenewResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__Unsubscribe_(struct soap *soap,
	struct _wsnt__Unsubscribe *wsnt__Unsubscribe,
	struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__PauseSubscription(struct soap *soap,
	struct _wsnt__PauseSubscription *wsnt__PauseSubscription,
	struct _wsnt__PauseSubscriptionResponse
	*wsnt__PauseSubscriptionResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __tev__ResumeSubscription(struct soap *soap,
	struct _wsnt__ResumeSubscription *wsnt__ResumeSubscription,
	struct _wsnt__ResumeSubscriptionResponse
	*wsnt__ResumeSubscriptionResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __timg__GetServiceCapabilities(struct soap *soap,
	struct _timg__GetServiceCapabilities *timg__GetServiceCapabilities,
	struct _timg__GetServiceCapabilitiesResponse
	*timg__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __timg__GetImagingSettings(struct soap *soap,
	struct _timg__GetImagingSettings *timg__GetImagingSettings,
	struct _timg__GetImagingSettingsResponse
	*timg__GetImagingSettingsResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __timg__SetImagingSettings(struct soap *soap,
	struct _timg__SetImagingSettings *timg__SetImagingSettings,
	struct _timg__SetImagingSettingsResponse
	*timg__SetImagingSettingsResponse)
{
	__FUN_BEGIN("\r\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __timg__GetOptions(struct soap *soap,
	struct _timg__GetOptions *timg__GetOptions,
	struct _timg__GetOptionsResponse
	*timg__GetOptionsResponse)
{
	__FUN_BEGIN("\r\n");
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __timg__Move(struct soap *soap,
	struct _timg__Move *timg__Move,
	struct _timg__MoveResponse *timg__MoveResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\r\n");
	return SOAP_OK;
}

int __timg__Stop(struct soap *soap,
	struct _timg__Stop *timg__Stop,
	struct _timg__StopResponse *timg__StopResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	return SOAP_OK;
	__FUN_END("\n");
}

int __timg__GetStatus(struct soap *soap,
	struct _timg__GetStatus *timg__GetStatus,
	struct _timg__GetStatusResponse *timg__GetStatusResponse)
{
	return SOAP_FAULT;
}

int __timg__GetMoveOptions(struct soap *soap,
	struct _timg__GetMoveOptions *timg__GetMoveOptions,
	struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tmd__GetServiceCapabilities(struct soap *soap,
	struct _tmd__GetServiceCapabilities *tmd__GetServiceCapabilities,
	struct _tmd__GetServiceCapabilitiesResponse
	*tmd__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

#ifndef SUPPORT_ONVIF_2_6
int __tmd__GetRelayOutputOptions(struct soap *soap,
	struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions,
	struct _tmd__GetRelayOutputOptionsResponse
	*tmd__GetRelayOutputOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetAudioSources(struct soap *soap,
	struct _trt__GetAudioSources *trt__GetAudioSources,
	struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetAudioOutputs(struct soap *soap,
	struct _trt__GetAudioOutputs *trt__GetAudioOutputs,
	struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetVideoSources(struct soap *soap,
	struct _trt__GetVideoSources *trt__GetVideoSources,
	struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}
#else
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSources(struct soap *soap,
	struct tmd__Get *trt__GetAudioSources,
	struct tmd__GetResponse *trt__GetAudioSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputs(struct soap *soap,
	struct tmd__Get *trt__GetAudioOutputs,
	struct tmd__GetResponse *trt__GetAudioOutputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSources(struct soap *soap,
	struct tmd__Get *trt__GetVideoSources,
	struct tmd__GetResponse *trt__GetVideoSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}
#endif /* SUPPORT_ONVIF_2_6 */

int __tmd__GetVideoOutputs(struct soap *soap,
	struct _tmd__GetVideoOutputs *tmd__GetVideoOutputs,
	struct _tmd__GetVideoOutputsResponse *tmd__GetVideoOutputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetVideoSourceConfiguration(struct soap *soap,
	struct _tmd__GetVideoSourceConfiguration
	*tmd__GetVideoSourceConfiguration,
	struct _tmd__GetVideoSourceConfigurationResponse
	*tmd__GetVideoSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetVideoOutputConfiguration(struct soap *soap,
	struct _tmd__GetVideoOutputConfiguration
	*tmd__GetVideoOutputConfiguration,
	struct _tmd__GetVideoOutputConfigurationResponse
	*tmd__GetVideoOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetAudioSourceConfiguration(struct soap *soap,
	struct _tmd__GetAudioSourceConfiguration
	*tmd__GetAudioSourceConfiguration,
	struct _tmd__GetAudioSourceConfigurationResponse
	*tmd__GetAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetAudioOutputConfiguration(struct soap *soap,
	struct _tmd__GetAudioOutputConfiguration
	*tmd__GetAudioOutputConfiguration,
	struct _tmd__GetAudioOutputConfigurationResponse
	*tmd__GetAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetVideoSourceConfiguration(struct soap *soap,
	struct _tmd__SetVideoSourceConfiguration
	*tmd__SetVideoSourceConfiguration,
	struct _tmd__SetVideoSourceConfigurationResponse
	*tmd__SetVideoSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetVideoOutputConfiguration(struct soap *soap,
	struct _tmd__SetVideoOutputConfiguration
	*tmd__SetVideoOutputConfiguration,
	struct _tmd__SetVideoOutputConfigurationResponse
	*tmd__SetVideoOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetAudioSourceConfiguration(struct soap *soap,
	struct _tmd__SetAudioSourceConfiguration
	*tmd__SetAudioSourceConfiguration,
	struct _tmd__SetAudioSourceConfigurationResponse
	*tmd__SetAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetAudioOutputConfiguration(struct soap *soap,
	struct _tmd__SetAudioOutputConfiguration
	*tmd__SetAudioOutputConfiguration,
	struct _tmd__SetAudioOutputConfigurationResponse
	*tmd__SetAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetVideoSourceConfigurationOptions(struct soap *soap,
	struct _tmd__GetVideoSourceConfigurationOptions
	*tmd__GetVideoSourceConfigurationOptions,
	struct _tmd__GetVideoSourceConfigurationOptionsResponse
	*tmd__GetVideoSourceConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetVideoOutputConfigurationOptions(struct soap *soap,
	struct _tmd__GetVideoOutputConfigurationOptions
	*tmd__GetVideoOutputConfigurationOptions,
	struct _tmd__GetVideoOutputConfigurationOptionsResponse
	*tmd__GetVideoOutputConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetAudioSourceConfigurationOptions(struct soap *soap,
	struct _tmd__GetAudioSourceConfigurationOptions
	*tmd__GetAudioSourceConfigurationOptions,
	struct _tmd__GetAudioSourceConfigurationOptionsResponse
	*tmd__GetAudioSourceConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetAudioOutputConfigurationOptions(struct soap *soap,
	struct _tmd__GetAudioOutputConfigurationOptions
	*tmd__GetAudioOutputConfigurationOptions,
	struct _tmd__GetAudioOutputConfigurationOptionsResponse
	*tmd__GetAudioOutputConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetRelayOutputs(struct soap *soap,
	struct _tds__GetRelayOutputs *tds__GetRelayOutputs,
	struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetRelayOutputSettings(struct soap *soap,
	struct _tmd__SetRelayOutputSettings *tmd__SetRelayOutputSettings,
	struct _tmd__SetRelayOutputSettingsResponse
	*tmd__SetRelayOutputSettingsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetRelayOutputState(struct soap *soap,
	struct _tds__SetRelayOutputState *tds__SetRelayOutputState,
	struct _tds__SetRelayOutputStateResponse
	*tds__SetRelayOutputStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetDigitalInputs(struct soap *soap,
	struct _tmd__GetDigitalInputs *tmd__GetDigitalInputs,
	struct _tmd__GetDigitalInputsResponse *tmd__GetDigitalInputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetSerialPorts(struct soap *soap,
	struct _tmd__GetSerialPorts *tmd__GetSerialPorts,
	struct _tmd__GetSerialPortsResponse *tmd__GetSerialPortsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __tmd__GetSerialPortConfiguration(struct soap *soap,
	struct _tmd__GetSerialPortConfiguration
	*tmd__GetSerialPortConfiguration,
	struct _tmd__GetSerialPortConfigurationResponse
	*tmd__GetSerialPortConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SetSerialPortConfiguration(struct soap *soap,
	struct _tmd__SetSerialPortConfiguration
	*tmd__SetSerialPortConfiguration,
	struct _tmd__SetSerialPortConfigurationResponse
	*tmd__SetSerialPortConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__GetSerialPortConfigurationOptions(struct soap *soap,
	struct _tmd__GetSerialPortConfigurationOptions
	*tmd__GetSerialPortConfigurationOptions,
	struct _tmd__GetSerialPortConfigurationOptionsResponse
	*tmd__GetSerialPortConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tmd__SendReceiveSerialCommand(struct soap *soap,
	struct _tmd__SendReceiveSerialCommand *tmd__SendReceiveSerialCommand,
	struct _tmd__SendReceiveSerialCommandResponse
	*tmd__SendReceiveSerialCommandResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tptz__GetServiceCapabilities(struct soap *soap,
	struct _tptz__GetServiceCapabilities *tptz__GetServiceCapabilities,
	struct _tptz__GetServiceCapabilitiesResponse
	*tptz__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	tptz__GetServiceCapabilitiesResponse->Capabilities =
		(struct tptz__Capabilities *)soap_malloc(soap,
		sizeof(struct tptz__Capabilities));
	tptz__GetServiceCapabilitiesResponse->Capabilities->__size = 0;
	tptz__GetServiceCapabilitiesResponse->Capabilities->__any = NULL;
	tptz__GetServiceCapabilitiesResponse->Capabilities->EFlip =
		(enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->Capabilities->EFlip =
		xsd__boolean__false_;

	tptz__GetServiceCapabilitiesResponse->Capabilities->Reverse =
		(enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->Capabilities->Reverse =
		xsd__boolean__false_;

	tptz__GetServiceCapabilitiesResponse->
		Capabilities->GetCompatibleConfigurations =
		(enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->
		Capabilities->GetCompatibleConfigurations = xsd__boolean__false_;

	tptz__GetServiceCapabilitiesResponse->
		Capabilities->MoveStatus =
		(enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->
		Capabilities->MoveStatus = xsd__boolean__false_;

	tptz__GetServiceCapabilitiesResponse->
		Capabilities->StatusPosition =
		(enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->
		Capabilities->StatusPosition = xsd__boolean__false_;

	tptz__GetServiceCapabilitiesResponse->
		Capabilities->__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tptz__GetConfigurations(struct soap* soap,
	struct _tptz__GetConfigurations* tptz__GetConfigurations,
	struct _tptz__GetConfigurationsResponse *tptz__GetConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	tptz__GetConfigurationsResponse->__sizePTZConfiguration = 1;
	ALLOC_STRUCT(tptz__GetConfigurationsResponse->
		PTZConfiguration, struct tt__PTZConfiguration);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->Name = ONVIF_PTZ_NAME;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->UseCount = 1;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->token = ONVIF_PTZ_TOKEN;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->NodeToken = ONVIF_PTZ_TOKEN;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultAbsolutePantTiltPositionSpace =
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultAbsoluteZoomPositionSpace =
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultRelativePanTiltTranslationSpace =
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultRelativeZoomTranslationSpace =
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace ";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultContinuousPanTiltVelocitySpace =
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace ";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultContinuousZoomVelocitySpace =
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace ";
	ALLOC_STRUCT(tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed, struct tt__PTZSpeed);
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->PanTilt, struct tt__Vector2D);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->PanTilt->space =
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace ";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 1.0;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->PanTilt->y = 0.5;
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->Zoom, struct tt__Vector1D);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->Zoom->space =
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace ";
	tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;
	ALLOC_STRUCT(
		 tptz__GetConfigurationsResponse->
		 PTZConfiguration->DefaultPTZTimeout, LONG64);
	*tptz__GetConfigurationsResponse->
		PTZConfiguration->DefaultPTZTimeout = 5 * 1000;

	ALLOC_STRUCT(tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits,
		struct tt__PanTiltLimits);
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range,
		struct tt__Space2DDescription);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range->URI =
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace";
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range->XRange,
		struct tt__FloatRange);
	tptz__GetConfigurationsResponse->PTZConfiguration->
		PanTiltLimits->Range->XRange->Max = 1.0;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range->XRange->Min = - 1.0;
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range->YRange,
		struct tt__FloatRange);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range->YRange->Max = 1.0;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->PanTiltLimits->Range->YRange->Min = - 1.0;

	ALLOC_STRUCT(tptz__GetConfigurationsResponse->
		PTZConfiguration->ZoomLimits,
		struct tt__ZoomLimits);
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->ZoomLimits->Range,
		struct tt__Space1DDescription);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->ZoomLimits->Range->URI =
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace";
	ALLOC_STRUCT(
		tptz__GetConfigurationsResponse->
		PTZConfiguration->ZoomLimits->Range->XRange,
		struct tt__FloatRange);
	tptz__GetConfigurationsResponse->
		PTZConfiguration->ZoomLimits->Range->XRange->Max = 1.0;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->ZoomLimits->Range->XRange->Min = 0;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->Extension = NULL;
	tptz__GetConfigurationsResponse->
		PTZConfiguration->__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

int GetPreSet(struct soap* soap, int index, struct tt__PTZPreset *pstPreSet)
{
	if (!pstPreSet || index < 0  || index >= 16)
		return -1;

	ALLOC_STRUCT_NUM(pstPreSet->Name, char, 64);
	ALLOC_STRUCT_NUM(pstPreSet->token, char, 64);
	ALLOC_STRUCT(pstPreSet->PTZPosition, struct tt__PTZVector);
	if (strlen(s_preset[index].name))
		sprintf(pstPreSet->Name, "%s", s_preset[index].name);
	else
		sprintf(pstPreSet->Name, "PreSet%d", index);

	if (strlen(s_preset[index].token))
		sprintf(pstPreSet->token, "%s", s_preset[index].token);
	else
		sprintf(pstPreSet->token, "PreSet%d", index);

	ALLOC_STRUCT(pstPreSet->PTZPosition->PanTilt, struct tt__Vector2D);
	pstPreSet->PTZPosition->PanTilt->x =
		(float)s_preset[index].pan_tilt_x;
	pstPreSet->PTZPosition->PanTilt->y =
		(float)s_preset[index].pan_tilt_y;
	ALLOC_STRUCT_NUM(pstPreSet->PTZPosition->PanTilt->space, char, 256);
	sprintf(pstPreSet->PTZPosition->PanTilt->space, "%s",
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	ALLOC_STRUCT(pstPreSet->PTZPosition->Zoom, struct tt__Vector1D);
	pstPreSet->PTZPosition->Zoom->x =
		(float)s_preset[index].zoom_x;
	ALLOC_STRUCT_NUM(pstPreSet->PTZPosition->Zoom->space, char, 256);
	sprintf(pstPreSet->PTZPosition->Zoom->space, "%s",
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
	pstPreSet->__anyAttribute = NULL;

	return 0;
}

int __tptz__GetPresets(struct soap *soap,
	struct _tptz__GetPresets *tptz__GetPresets,
	struct _tptz__GetPresetsResponse *tptz__GetPresetsResponse)
{
	CHECK_USER;
	int i, iPTZPresets = 16;
	int index, count;

	count = 0;
	for (i=0; i<iPTZPresets; i++) {
		if (strlen(s_preset[i].name) > 0)
			count++;
	}

	tptz__GetPresetsResponse->__sizePreset = count;

	ALLOC_STRUCT(tptz__GetPresetsResponse->Preset, struct tt__PTZPreset);

	index = 0;
	for (i = 0; i < iPTZPresets; i++) {
		if (strlen(s_preset[i].name) > 0) {
			GetPreSet(soap, i,
				&tptz__GetPresetsResponse->Preset[index]);
			index++;
		}
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__SetPreset(struct soap *soap,
	struct _tptz__SetPreset *tptz__SetPreset,
	struct _tptz__SetPresetResponse *tptz__SetPresetResponse)
{
	CHECK_USER;
	int i, index = 0;
	printf("__tptz__SetPreset() ProfileToken %s.\n",
		tptz__SetPreset->ProfileToken);

	for (i=0; i<16; i++) {
		if (strlen(s_preset[i].name) == 0) {
			index = i;
			break;
		}
	}

	ALLOC_STRUCT_NUM(tptz__SetPresetResponse->PresetToken, char, 256);
	if (tptz__SetPreset->PresetName) {
		ALOGD("	PresetName %s\n", tptz__SetPreset->PresetName);
		sprintf(tptz__SetPresetResponse->PresetToken,
			"%s", tptz__SetPreset->PresetName);
		sprintf(s_preset[index].name, "%s",
			tptz__SetPreset->PresetName);
	}

	if (tptz__SetPreset->PresetToken) {
		ALOGD("	PresetToken %s\n", tptz__SetPreset->PresetToken);
		sprintf(tptz__SetPresetResponse->PresetToken,
			"%s", tptz__SetPreset->PresetToken);
		sprintf(s_preset[index].token, "%s",
			tptz__SetPreset->PresetToken);
	} else
		sprintf(s_preset[index].token,
			"%s", s_preset[index].name);

	s_preset[index].pan_tilt_x = (float)(rand() % 100);
	s_preset[index].pan_tilt_y = (float)(rand() % 100);
	s_preset[index].zoom_x = (float)(rand() % 100);

	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__RemovePreset(struct soap *soap,
	struct _tptz__RemovePreset *tptz__RemovePreset,
	struct _tptz__RemovePresetResponse *tptz__RemovePresetResponse)
{
	CHECK_USER;
	printf("__tptz__RemovePreset() ProfileToken %s.\n",
		tptz__RemovePreset->ProfileToken);

	int i = 0, iPTZPresets = 16;

	if (tptz__RemovePreset->PresetToken) {
		for (i=0; i<iPTZPresets; i++) {
			if (!strcmp(s_preset[i].name,
					tptz__RemovePreset->PresetToken)) {
				memset(&s_preset[i], 0, sizeof(preset_t));
				break;
			}
		}
	}

	printf("	PresetToken %s find index %d\n",
		tptz__RemovePreset->PresetToken, i);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GotoPreset(struct soap *soap,
	struct _tptz__GotoPreset *tptz__GotoPreset,
	struct _tptz__GotoPresetResponse *tptz__GotoPresetResponse)
{
	CHECK_USER;
	printf("__tptz__GotoPreset() ProfileToken %s.\n",
		tptz__GotoPreset->ProfileToken);

	int i = 0, iPTZPresets = 16;

	if (tptz__GotoPreset->PresetToken) {
		for (i=0; i<iPTZPresets; i++) {
			if (!strcmp(s_preset[i].name,
					tptz__GotoPreset->PresetToken))
				break;
		}
	}

	printf("	PresetToken %s find index %d\n",
		tptz__GotoPreset->PresetToken, i);

	if (tptz__GotoPreset->Speed && tptz__GotoPreset->Speed->PanTilt)
		printf("Speed PanTilt: %.2f %.2f space %s\n",
			tptz__GotoPreset->Speed->PanTilt->x,
			tptz__GotoPreset->Speed->PanTilt->y,
			tptz__GotoPreset->Speed->PanTilt->space);

	if (tptz__GotoPreset->Speed && tptz__GotoPreset->Speed->Zoom)
		printf("Speed Zoom: %.2f space %s\n",
			tptz__GotoPreset->Speed->Zoom->x,
			tptz__GotoPreset->Speed->Zoom->space);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetStatus(struct soap *soap,
	struct _tptz__GetStatus *tptz__GetStatus,
	struct _tptz__GetStatusResponse *tptz__GetStatusResponse)
{
	CHECK_USER;
	printf("__tptz__GetStatus() ProfileToken %s.\n",
		tptz__GetStatus->ProfileToken);

	ALLOC_STRUCT(tptz__GetStatusResponse->PTZStatus,
		struct tt__PTZStatus);
	ALLOC_STRUCT(tptz__GetStatusResponse->PTZStatus->Position,
		struct tt__PTZVector);
	ALLOC_STRUCT(tptz__GetStatusResponse->PTZStatus->Position->PanTilt,
		struct tt__Vector2D);
	tptz__GetStatusResponse->PTZStatus->Position->PanTilt->x =
		(float)s_ptz_config.pan_tilt_x;
	tptz__GetStatusResponse->PTZStatus->Position->PanTilt->y =
		(float)s_ptz_config.pan_tilt_y;
	ALLOC_STRUCT_NUM(tptz__GetStatusResponse->PTZStatus->
		Position->PanTilt->space, char, 256);
	sprintf(tptz__GetStatusResponse->PTZStatus->
		Position->PanTilt->space, "%s",
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	ALLOC_STRUCT(tptz__GetStatusResponse->PTZStatus->Position->Zoom,
		struct tt__Vector1D);
	tptz__GetStatusResponse->PTZStatus->Position->Zoom->x =
		(float)s_ptz_config.zoom_x;
	ALLOC_STRUCT_NUM(tptz__GetStatusResponse->
		PTZStatus->Position->Zoom->space,
		char, 256);
	sprintf(tptz__GetStatusResponse->
		PTZStatus->Position->Zoom->space, "%s",
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");

	ALLOC_STRUCT(tptz__GetStatusResponse->PTZStatus->MoveStatus,
		struct tt__PTZMoveStatus);

	tptz__GetStatusResponse->PTZStatus->Error = NULL;
	tptz__GetStatusResponse->PTZStatus->UtcTime= time(NULL);
	tptz__GetStatusResponse->PTZStatus->__size = 0;
	tptz__GetStatusResponse->PTZStatus->__any = NULL;
	tptz__GetStatusResponse->PTZStatus->__anyAttribute = NULL;
	return SOAP_OK;
}

int GetPTZConfiguration(struct soap* soap, int index,
	struct tt__PTZConfiguration *pstConfiguration)
{
	if (!pstConfiguration)
		return -1;

	ALLOC_STRUCT_NUM(pstConfiguration->Name, char, 256);
	ALLOC_STRUCT_NUM(pstConfiguration->token, char, 256);
	ALLOC_STRUCT_NUM(pstConfiguration->NodeToken, char, 256);
	ALLOC_STRUCT_NUM(pstConfiguration->
		DefaultContinuousPanTiltVelocitySpace, char, 256);
	ALLOC_STRUCT_NUM(pstConfiguration->
		DefaultContinuousZoomVelocitySpace, char, 256);

	ALLOC_STRUCT(pstConfiguration->DefaultPTZSpeed,
		struct tt__PTZSpeed);
	ALLOC_STRUCT(pstConfiguration->DefaultPTZTimeout,
		LONG64);
	ALLOC_STRUCT(pstConfiguration->PanTiltLimits,
		struct tt__PanTiltLimits);
	ALLOC_STRUCT(pstConfiguration->ZoomLimits,
		struct tt__ZoomLimits);

	sprintf(pstConfiguration->Name, "PTZConfiguration%d", index);
	sprintf(pstConfiguration->token, "PTZConfiguration%d", index);
	pstConfiguration->UseCount = 2; //1;
	sprintf(pstConfiguration->NodeToken, "PTZ%d", index);

	sprintf(pstConfiguration->DefaultContinuousPanTiltVelocitySpace, "%s",
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	sprintf(pstConfiguration->DefaultContinuousZoomVelocitySpace, "%s",
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");

	ALLOC_STRUCT(pstConfiguration->DefaultPTZSpeed->PanTilt,
		struct tt__Vector2D);
	pstConfiguration->DefaultPTZSpeed->PanTilt->x =
		(float)s_ptz_config.pan_tilt_x;
	pstConfiguration->DefaultPTZSpeed->PanTilt->y =
		(float)s_ptz_config.pan_tilt_y;
	ALLOC_STRUCT_NUM(pstConfiguration->DefaultPTZSpeed->PanTilt->space,
		char, 256);
	sprintf(pstConfiguration->DefaultPTZSpeed->PanTilt->space, "%s",
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
	ALLOC_STRUCT(pstConfiguration->DefaultPTZSpeed->Zoom,
		struct tt__Vector1D);
	pstConfiguration->DefaultPTZSpeed->Zoom->x =
		(float)s_ptz_config.zoom_x;
	ALLOC_STRUCT_NUM(pstConfiguration->DefaultPTZSpeed->Zoom->space,
		char, 256);
	sprintf(pstConfiguration->DefaultPTZSpeed->Zoom->space, "%s",
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");

	*pstConfiguration->DefaultPTZTimeout =
		(float)s_ptz_config.default_ptz_time_out;

	ALLOC_STRUCT(pstConfiguration->PanTiltLimits->Range,
		struct tt__Space2DDescription);
	ALLOC_STRUCT_NUM(pstConfiguration->PanTiltLimits->Range->URI,
		char, 256);
	sprintf(pstConfiguration->PanTiltLimits->Range->URI, "%s",
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	ALLOC_STRUCT(pstConfiguration->PanTiltLimits->Range->XRange,
		struct tt__FloatRange);
	pstConfiguration->PanTiltLimits->Range->XRange->Min =
		(float)s_ptz_config.pan_tilt_limits_x_min;
	pstConfiguration->PanTiltLimits->Range->XRange->Max =
		(float)s_ptz_config.pan_tilt_limits_x_max;
	ALLOC_STRUCT(pstConfiguration->PanTiltLimits->Range->YRange,
		struct tt__FloatRange);
	pstConfiguration->PanTiltLimits->Range->YRange->Min =
		(float)s_ptz_config.pan_tilt_limits_y_min;
	pstConfiguration->PanTiltLimits->Range->YRange->Max =
		(float)s_ptz_config.pan_tilt_limits_y_max;

	ALLOC_STRUCT(pstConfiguration->ZoomLimits->Range,
		struct tt__Space1DDescription);
	ALLOC_STRUCT_NUM(pstConfiguration->ZoomLimits->Range->URI,
		char, 256);
	sprintf(pstConfiguration->ZoomLimits->Range->URI, "%s",
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	ALLOC_STRUCT(pstConfiguration->ZoomLimits->Range->XRange,
		struct tt__FloatRange);
	pstConfiguration->ZoomLimits->Range->XRange->Min =
		(float)s_ptz_config.zoom_limits_min;
	pstConfiguration->ZoomLimits->Range->XRange->Max =
		(float)s_ptz_config.zoom_limits_max;

	return 0;
}

int __tptz__GetConfiguration(struct soap *soap,
	struct _tptz__GetConfiguration *tptz__GetConfiguration,
	struct _tptz__GetConfigurationResponse
	*tptz__GetConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	ALOGD("__tptz__GetConfiguration() PTZConfigurationToken %s.\n",
			tptz__GetConfiguration->PTZConfigurationToken);

	if (strcmp(tptz__GetConfiguration->PTZConfigurationToken,
			"PTZConfiguration0")) {
		soap->version = 2;
		return -1;
	}

	tptz__GetConfigurationResponse->PTZConfiguration =
		(struct tt__PTZConfiguration *)soap_malloc(soap,
		sizeof(struct tt__PTZConfiguration));
	return GetPTZConfiguration(soap, 0,
		tptz__GetConfigurationResponse->PTZConfiguration);
}

int GetPTZSpaces(struct soap* soap, struct tt__PTZSpaces *pstSpaces)
{
	if (!soap || !pstSpaces)
		return -1;

	pstSpaces->__sizeAbsolutePanTiltPositionSpace = 1;
	ALLOC_STRUCT(pstSpaces->AbsolutePanTiltPositionSpace,
		struct tt__Space2DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->AbsolutePanTiltPositionSpace->URI,
		char, 256);
	sprintf(pstSpaces->AbsolutePanTiltPositionSpace->URI, "%s",
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	ALLOC_STRUCT(pstSpaces->AbsolutePanTiltPositionSpace->XRange,
		struct tt__FloatRange);
	pstSpaces->AbsolutePanTiltPositionSpace->XRange->Min = 0.0;
	pstSpaces->AbsolutePanTiltPositionSpace->XRange->Max = 100.0;
	ALLOC_STRUCT(pstSpaces->AbsolutePanTiltPositionSpace->YRange,
		struct tt__FloatRange);
	pstSpaces->AbsolutePanTiltPositionSpace->YRange->Min = 0.0;
	pstSpaces->AbsolutePanTiltPositionSpace->YRange->Max = 100.0;

	pstSpaces->__sizeAbsoluteZoomPositionSpace = 1;
	ALLOC_STRUCT(pstSpaces->AbsoluteZoomPositionSpace,
		struct tt__Space1DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->AbsoluteZoomPositionSpace->URI,
		char, 256);
	sprintf(pstSpaces->AbsoluteZoomPositionSpace->URI, "%s",
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
	ALLOC_STRUCT(pstSpaces->AbsoluteZoomPositionSpace->XRange,
		struct tt__FloatRange);
	pstSpaces->AbsoluteZoomPositionSpace->XRange->Min = 0.0;
	pstSpaces->AbsoluteZoomPositionSpace->XRange->Max = 100.0;

	pstSpaces->__sizeRelativePanTiltTranslationSpace = 1;
	ALLOC_STRUCT(pstSpaces->RelativePanTiltTranslationSpace,
		struct tt__Space2DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->RelativePanTiltTranslationSpace->URI,
		char, 256);
	sprintf(pstSpaces->RelativePanTiltTranslationSpace->URI, "%s",
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
	ALLOC_STRUCT(pstSpaces->RelativePanTiltTranslationSpace->XRange,
		struct tt__FloatRange);
	pstSpaces->RelativePanTiltTranslationSpace->XRange->Min = 0.0;
	pstSpaces->RelativePanTiltTranslationSpace->XRange->Max = 100.0;
	ALLOC_STRUCT(pstSpaces->RelativePanTiltTranslationSpace->YRange,
		struct tt__FloatRange);
	pstSpaces->RelativePanTiltTranslationSpace->YRange->Min = 0.0;
	pstSpaces->RelativePanTiltTranslationSpace->YRange->Max = 100.0;

	pstSpaces->__sizeRelativeZoomTranslationSpace = 1;
	ALLOC_STRUCT(pstSpaces->RelativeZoomTranslationSpace,
		struct tt__Space1DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->RelativeZoomTranslationSpace->URI,
		char, 256);
	sprintf(pstSpaces->RelativeZoomTranslationSpace->URI, "%s",
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace");
	ALLOC_STRUCT(pstSpaces->RelativeZoomTranslationSpace->XRange,
		struct tt__FloatRange);
	pstSpaces->RelativeZoomTranslationSpace->XRange->Min = 0.0;
	pstSpaces->RelativeZoomTranslationSpace->XRange->Max = 100.0;

	pstSpaces->__sizeContinuousPanTiltVelocitySpace = 1;
	ALLOC_STRUCT(pstSpaces->ContinuousPanTiltVelocitySpace,
		struct tt__Space2DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->ContinuousPanTiltVelocitySpace->URI,
		char, 256);
	sprintf(pstSpaces->ContinuousPanTiltVelocitySpace->URI, "%s",
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	ALLOC_STRUCT(pstSpaces->ContinuousPanTiltVelocitySpace->XRange,
		struct tt__FloatRange);
	pstSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = 0.0;
	pstSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 100.0;
	ALLOC_STRUCT(pstSpaces->ContinuousPanTiltVelocitySpace->YRange,
		struct tt__FloatRange);
	pstSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = 0.0;
	pstSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 100.0;

	pstSpaces->__sizeContinuousZoomVelocitySpace = 1;
	ALLOC_STRUCT(pstSpaces->ContinuousZoomVelocitySpace,
		struct tt__Space1DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->ContinuousZoomVelocitySpace->URI,
		char, 256);
	sprintf(pstSpaces->ContinuousZoomVelocitySpace->URI, "%s",
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	ALLOC_STRUCT(pstSpaces->ContinuousZoomVelocitySpace->XRange,
		struct tt__FloatRange);
	pstSpaces->ContinuousZoomVelocitySpace->XRange->Min = 0.0;
	pstSpaces->ContinuousZoomVelocitySpace->XRange->Max = 100.0;

	pstSpaces->__sizePanTiltSpeedSpace = 1;
	ALLOC_STRUCT(pstSpaces->PanTiltSpeedSpace,
		struct tt__Space1DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->PanTiltSpeedSpace->URI,
		char, 256);
	sprintf(pstSpaces->PanTiltSpeedSpace->URI, "%s",
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
	ALLOC_STRUCT(pstSpaces->PanTiltSpeedSpace->XRange,
		struct tt__FloatRange);
	pstSpaces->PanTiltSpeedSpace->XRange->Min = 0.0;
	pstSpaces->PanTiltSpeedSpace->XRange->Max = 100.0;

	pstSpaces->__sizeZoomSpeedSpace = 1;
	ALLOC_STRUCT(pstSpaces->ZoomSpeedSpace,
		struct tt__Space1DDescription);
	ALLOC_STRUCT_NUM(pstSpaces->ZoomSpeedSpace->URI,
		char, 256);
	sprintf(pstSpaces->ZoomSpeedSpace->URI, "%s",
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
	ALLOC_STRUCT(pstSpaces->ZoomSpeedSpace->XRange,
		struct tt__FloatRange);
	pstSpaces->ZoomSpeedSpace->XRange->Min = 0.0;
	pstSpaces->ZoomSpeedSpace->XRange->Max = 100.0;

	pstSpaces->Extension = NULL;
	pstSpaces->__anyAttribute = NULL;

	return 0;
}

int GetPTZNode(struct soap* soap, int index, struct tt__PTZNode *pstNode)
{
	if (!pstNode)
		return -1;

	ALLOC_STRUCT_NUM(pstNode->token, char, 256);
	ALLOC_STRUCT_NUM(pstNode->Name, char, 256);
	sprintf(pstNode->token, "PTZ%d", index);
	sprintf(pstNode->Name, "PTZ%d", index);

	ALLOC_STRUCT(pstNode->SupportedPTZSpaces, struct tt__PTZSpaces);

	GetPTZSpaces(soap, pstNode->SupportedPTZSpaces);

	pstNode->MaximumNumberOfPresets = 16;

	pstNode->HomeSupported = xsd__boolean__true_;

	pstNode->__sizeAuxiliaryCommands = 0;
	pstNode->AuxiliaryCommands = NULL;
	pstNode->Extension = NULL;
	pstNode->FixedHomePosition = NULL;
	pstNode->GeoMove = NULL;
	pstNode->__anyAttribute = NULL;

	return 0;
}

int __tptz__GetNodes(struct soap *soap,
	struct _tptz__GetNodes *tptz__GetNodes,
	struct _tptz__GetNodesResponse *tptz__GetNodesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int i, iPTZNode = 1;

	tptz__GetNodesResponse->__sizePTZNode = iPTZNode;
	ALLOC_STRUCT(tptz__GetNodesResponse->PTZNode, struct tt__PTZNode);

	i = 0;
	GetPTZNode(soap, i, &tptz__GetNodesResponse->PTZNode[0]);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetNode(struct soap *soap,
	struct _tptz__GetNode *tptz__GetNode,
	struct _tptz__GetNodeResponse *tptz__GetNodeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	if (strcmp(tptz__GetNode->NodeToken, "PTZ0")) {
		soap->version = 2;
		if (!soap->fault) {
			soap->fault = (struct SOAP_ENV__Fault *)soap_malloc(soap,
				sizeof(struct SOAP_ENV__Fault));
			soap->fault->SOAP_ENV__Reason =
				(struct SOAP_ENV__Reason *)soap_malloc(soap,
				sizeof(struct SOAP_ENV__Reason));
			ALLOC_STRUCT_NUM(soap->fault->
				SOAP_ENV__Reason->SOAP_ENV__Text, char, 256);

			soap->fault->SOAP_ENV__Detail =
				(struct SOAP_ENV__Detail *)soap_malloc(soap,
				sizeof(struct SOAP_ENV__Detail));
		}
		sprintf(soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text,
			"ter:%s ter:%s", "InvalidArgVal", "NoEntity");
		return -1;
	}

	tptz__GetNodeResponse->PTZNode =
		(struct tt__PTZNode *)soap_malloc(soap,
		sizeof(struct tt__PTZNode));

	GetPTZNode(soap, 0, tptz__GetNodeResponse->PTZNode);

	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__SetConfiguration(struct soap *soap,
	struct _tptz__SetConfiguration *tptz__SetConfiguration,
	struct _tptz__SetConfigurationResponse
	*tptz__SetConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	if (!tptz__SetConfiguration->PTZConfiguration)
		return -1;

	if (tptz__SetConfiguration->PTZConfiguration->
			DefaultAbsolutePantTiltPositionSpace) {
		if (strcmp(tptz__SetConfiguration->PTZConfiguration->
			DefaultAbsolutePantTiltPositionSpace,
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace")) {
			soap->version = 2;
			return -1;
		}
	}

	if (tptz__SetConfiguration->PTZConfiguration->
			DefaultAbsoluteZoomPositionSpace) {
		if (strcmp(tptz__SetConfiguration->PTZConfiguration->
			DefaultAbsoluteZoomPositionSpace,
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace")) {
			soap->version = 2;
			return -1;
		}
	}

	if (tptz__SetConfiguration->PTZConfiguration->
			DefaultRelativePanTiltTranslationSpace) {
		if (strcmp(tptz__SetConfiguration->PTZConfiguration->
				DefaultRelativePanTiltTranslationSpace,
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace")) {
			soap->version = 2;
			return -1;
		}
	}

	if (tptz__SetConfiguration->PTZConfiguration->
			DefaultRelativeZoomTranslationSpace) {
		if (strcmp(tptz__SetConfiguration->PTZConfiguration->
				DefaultRelativeZoomTranslationSpace,
	"http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace")) {
			soap->version = 2;
			return -1;
		}
	}

	if (tptz__SetConfiguration->PTZConfiguration->
			DefaultContinuousPanTiltVelocitySpace) {
		if (strcmp(tptz__SetConfiguration->PTZConfiguration->
				DefaultContinuousPanTiltVelocitySpace,
	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace")) {
			soap->version = 2;
			return -1;
		}
	}

	if (tptz__SetConfiguration->PTZConfiguration->
			DefaultContinuousZoomVelocitySpace) {
		if (strcmp(tptz__SetConfiguration->
				PTZConfiguration->
				DefaultContinuousZoomVelocitySpace,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace")) {
			soap->version = 2;
			return -1;
		}
	}

	if (tptz__SetConfiguration->PTZConfiguration->DefaultPTZSpeed) {
		s_ptz_config.pan_tilt_x = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->DefaultPTZSpeed->PanTilt->x;
		s_ptz_config.pan_tilt_y = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->DefaultPTZSpeed->PanTilt->y;
		s_ptz_config.zoom_x = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->DefaultPTZSpeed->Zoom->x;
		s_ptz_config.default_ptz_time_out = (unsigned short)
			*tptz__SetConfiguration->
			PTZConfiguration->DefaultPTZTimeout;
	}

	if (tptz__SetConfiguration->PTZConfiguration->PanTiltLimits) {
		s_ptz_config.pan_tilt_limits_x_min = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->PanTiltLimits->Range->XRange->Min;
		s_ptz_config.pan_tilt_limits_x_max = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->PanTiltLimits->Range->XRange->Max;
		s_ptz_config.pan_tilt_limits_y_min = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->PanTiltLimits->Range->YRange->Min;
		s_ptz_config.pan_tilt_limits_y_max = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->PanTiltLimits->Range->YRange->Max;
	}

	if (tptz__SetConfiguration->PTZConfiguration->ZoomLimits) {
		s_ptz_config.zoom_limits_min = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->ZoomLimits->Range->XRange->Min;
		s_ptz_config.zoom_limits_max = (unsigned char)
			tptz__SetConfiguration->
			PTZConfiguration->ZoomLimits->Range->XRange->Max;
	}

	if (tptz__SetConfiguration->PTZConfiguration->MoveRamp)
		s_ptz_config.move_ramp =
			(unsigned short)*tptz__SetConfiguration->
			PTZConfiguration->MoveRamp;

	if (tptz__SetConfiguration->PTZConfiguration->PresetRamp)
		s_ptz_config.preset_ramp =
			(unsigned short)*tptz__SetConfiguration->
			PTZConfiguration->PresetRamp;

	if (tptz__SetConfiguration->PTZConfiguration->PresetTourRamp)
		s_ptz_config.preset_tour_ramp =
			(unsigned short)*tptz__SetConfiguration->
			PTZConfiguration->PresetTourRamp;

	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetConfigurationOptions(struct soap *soap,
	struct _tptz__GetConfigurationOptions
	*tptz__GetConfigurationOptions,
	struct _tptz__GetConfigurationOptionsResponse
	*tptz__GetConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions =
		(struct tt__PTZConfigurationOptions *)soap_malloc(soap,
		sizeof(struct tt__PTZConfigurationOptions));

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces =
		(struct tt__PTZSpaces *)soap_malloc(soap,
		sizeof(struct tt__PTZSpaces));
	GetPTZSpaces(soap, tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->Spaces);

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTZTimeout = (struct tt__DurationRange *)soap_malloc(soap,
		sizeof(struct tt__DurationRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTZTimeout->Min = 0.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTZTimeout->Max = 255.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->__size
		= 0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->__any 
		= NULL;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection = (struct tt__PTControlDirectionOptions *)
		soap_malloc(soap, sizeof(struct tt__PTControlDirectionOptions));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection->EFlip = (struct tt__EFlipOptions *)
		soap_malloc(soap, sizeof(struct tt__EFlipOptions));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection->EFlip->__sizeMode = 1;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection->EFlip->Mode = (enum tt__EFlipMode *)
		soap_malloc(soap, sizeof(enum tt__EFlipMode));
	*tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection->EFlip->Mode = tt__EFlipMode__OFF;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection->EFlip->Extension = NULL;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->
		PTControlDirection->EFlip->__anyAttribute = NULL;

	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Reverse =
		(struct tt__ReverseOptions *)soap_malloc(soap,
		sizeof(struct tt__ReverseOptions));
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Reverse->
		__sizeMode = 1;
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Reverse->
		Mode = (enum tt__ReverseMode *)soap_malloc(soap,
		sizeof(enum tt__ReverseMode));
	*tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Reverse->
		Mode = tt__ReverseMode__OFF;
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Reverse->
		Extension = NULL;
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Reverse->
		__anyAttribute = NULL;

	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->Extension = NULL;
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTControlDirection->__anyAttribute
		= NULL;

	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->Extension = NULL;
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->PTZRamps = NULL;
	tptz__GetConfigurationOptionsResponse->
		PTZConfigurationOptions->__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GotoHomePosition(struct soap *soap,
	struct _tptz__GotoHomePosition *tptz__GotoHomePosition,
	struct _tptz__GotoHomePositionResponse
	*tptz__GotoHomePositionResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (tptz__GotoHomePosition->Speed
			&& tptz__GotoHomePosition->Speed->PanTilt) {
		/* control ptz */
		/* ... */
		ALOGD("Speed PanTilt(%.2f %.2f, space %s)\n",
			tptz__GotoHomePosition->Speed->PanTilt->x,
			tptz__GotoHomePosition->Speed->PanTilt->y,
			tptz__GotoHomePosition->Speed->PanTilt->space);
	}

	if (tptz__GotoHomePosition->Speed
			&& tptz__GotoHomePosition->Speed->Zoom) {
		/* control ptz */
		/* ... */
		ALOGD("speed Zoom(%.2f, space %s)\n",
			tptz__GotoHomePosition->Speed->Zoom->x,
			tptz__GotoHomePosition->Speed->Zoom->space);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__SetHomePosition(struct soap *soap,
	struct _tptz__SetHomePosition *tptz__SetHomePosition,
	struct _tptz__SetHomePositionResponse
	*tptz__SetHomePositionResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	return SOAP_FAULT;
}

int __tptz__ContinuousMove(struct soap *soap,
	struct _tptz__ContinuousMove *tptz__ContinuousMove,
	struct _tptz__ContinuousMoveResponse
	*tptz__ContinuousMoveResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (tptz__ContinuousMove->Velocity
			&& tptz__ContinuousMove->Velocity->PanTilt) {
		/* control ptz */
		/* ... */
		ALOGD("Velocity PanTilt(%.2f %.2f, space %s)\n",
			tptz__ContinuousMove->Velocity->PanTilt->x,
			tptz__ContinuousMove->Velocity->PanTilt->y,
			tptz__ContinuousMove->Velocity->PanTilt->space);
	}

	if (tptz__ContinuousMove->Velocity
			&& tptz__ContinuousMove->Velocity->Zoom) {
		/* control ptz */
		/* ... */
		ALOGD("Velocity Zoom(%.2f, space %s)\n",
			tptz__ContinuousMove->Velocity->Zoom->x,
			tptz__ContinuousMove->Velocity->Zoom->space);
	}

	if (tptz__ContinuousMove->Timeout)
		ALOGD("Timeout %lld)\n", *tptz__ContinuousMove->Timeout);
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__RelativeMove(struct soap *soap,
	struct _tptz__RelativeMove *tptz__RelativeMove,
	struct _tptz__RelativeMoveResponse *tptz__RelativeMoveResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (tptz__RelativeMove->Translation
			&& tptz__RelativeMove->Translation->PanTilt) {
		/* control ptz */
		/* ... */
		ALOGD("Translation PanTilt(%.2f %.2f, space %s)\n",
			tptz__RelativeMove->Translation->PanTilt->x,
			tptz__RelativeMove->Translation->PanTilt->y,
			tptz__RelativeMove->Translation->PanTilt->space);
	}

	if (tptz__RelativeMove->Translation
			&& tptz__RelativeMove->Translation->Zoom) {
		ALOGD("Translation Zoom(%.2f space %s)\n",
			tptz__RelativeMove->Translation->Zoom->x,
			tptz__RelativeMove->Translation->Zoom->space);
	}

	if (tptz__RelativeMove->Speed && tptz__RelativeMove->Speed->PanTilt) {
		/* control ptz */
		/* ... */
		ALOGD("Speed PanTilt(%.2f %.2f, space %s)\n",
			tptz__RelativeMove->Speed->PanTilt->x,
			tptz__RelativeMove->Speed->PanTilt->y,
			tptz__RelativeMove->Speed->PanTilt->space);
	}

	if (tptz__RelativeMove->Speed && tptz__RelativeMove->Speed->Zoom) {
		/* control ptz */
		/* ... */
		ALOGD("Speed Zoom(%.2f space %s)\n",
			tptz__RelativeMove->Speed->Zoom->x,
			tptz__RelativeMove->Speed->Zoom->space);
	}
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tptz__SendAuxiliaryCommand(struct soap *soap,
	struct _tptz__SendAuxiliaryCommand *tptz__SendAuxiliaryCommand,
	struct _tptz__SendAuxiliaryCommandResponse
	*tptz__SendAuxiliaryCommandResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __tptz__AbsoluteMove(struct soap *soap,
	struct _tptz__AbsoluteMove *tptz__AbsoluteMove,
	struct _tptz__AbsoluteMoveResponse *tptz__AbsoluteMoveResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (tptz__AbsoluteMove->Position
			&& tptz__AbsoluteMove->Position->PanTilt) {
		/* control ptz */
		/* ... */
		ALOGD("Position PanTilt(%.2f %.2f, space %s)\n",
			tptz__AbsoluteMove->Position->PanTilt->x,
			tptz__AbsoluteMove->Position->PanTilt->y,
			tptz__AbsoluteMove->Position->PanTilt->space);
	}

	if (tptz__AbsoluteMove->Position && tptz__AbsoluteMove->Position->Zoom) {
		/* control ptz */
		/* ... */
		ALOGD("Position Zoom(%.2f space %s)\n",
			tptz__AbsoluteMove->Position->Zoom->x,
			tptz__AbsoluteMove->Position->Zoom->space);
	}

	if (tptz__AbsoluteMove->Speed && tptz__AbsoluteMove->Speed->PanTilt) {
		/* control ptz */
		/* ... */
		printf("Speed PanTilt(%.2f %.2f, space %s)\n",
			tptz__AbsoluteMove->Speed->PanTilt->x,
			tptz__AbsoluteMove->Speed->PanTilt->y,
			tptz__AbsoluteMove->Speed->PanTilt->space);
	}

	if (tptz__AbsoluteMove->Speed && tptz__AbsoluteMove->Speed->Zoom) {
		/* control ptz */
		/* ... */
		ALOGD("Speed Zoom(%.2f space %s)\n",
			tptz__AbsoluteMove->Speed->Zoom->x,
			tptz__AbsoluteMove->Speed->Zoom->space);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__Stop(struct soap *soap,
	struct _tptz__Stop *tptz__Stop,
	struct _tptz__StopResponse *tptz__StopResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (tptz__Stop->PanTilt) {
		/* control ptz */
		/* ... */
		ALOGD("PanTilt %d\n", *tptz__Stop->PanTilt);
	}

	if (tptz__Stop->Zoom) {
		/* control ptz */
		/* ... */
		ALOGD("PanTilt %d\n", *tptz__Stop->Zoom);
	}
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetPresetTours(struct soap *soap,
 	struct _tptz__GetPresetTours *tptz__GetPresetTours,
	struct _tptz__GetPresetToursResponse
	*tptz__GetPresetToursResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetPresetTour(struct soap *soap,
	struct _tptz__GetPresetTour *tptz__GetPresetTour,
	struct _tptz__GetPresetTourResponse
	*tptz__GetPresetTourResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetPresetTourOptions(struct soap *soap,
	struct _tptz__GetPresetTourOptions *tptz__GetPresetTourOptions,
	struct _tptz__GetPresetTourOptionsResponse
	*tptz__GetPresetTourOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__CreatePresetTour(struct soap *soap,
	struct _tptz__CreatePresetTour *tptz__CreatePresetTour,
	struct _tptz__CreatePresetTourResponse *tptz__CreatePresetTourResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__ModifyPresetTour(struct soap *soap,
	struct _tptz__ModifyPresetTour *tptz__ModifyPresetTour,
	struct _tptz__ModifyPresetTourResponse *tptz__ModifyPresetTourResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__OperatePresetTour(struct soap *soap,
	struct _tptz__OperatePresetTour *tptz__OperatePresetTour,
	struct _tptz__OperatePresetTourResponse
	*tptz__OperatePresetTourResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__RemovePresetTour(struct soap *soap,
	struct _tptz__RemovePresetTour *tptz__RemovePresetTour,
	struct _tptz__RemovePresetTourResponse *tptz__RemovePresetTourResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetCompatibleConfigurations(struct soap *soap,
	struct _tptz__GetCompatibleConfigurations
	*tptz__GetCompatibleConfigurations,
	struct _tptz__GetCompatibleConfigurationsResponse
	*tptz__GetCompatibleConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

#ifndef SUPPORT_ONVIF_2_6
int __tptz__Pix3DPosMove(struct soap *soap,
	struct _tptz__tptz_Pix3DPosMove *tptz__tptz_Pix3DPosMove,
	struct _tptz__tptz_Pix3DPosMoveResponse
	*tptz__tptz_Pix3DPosMoveResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__SetBallParam(struct soap *soap,
	struct _tptz__tptz_SetBallParam *tptz__tptz_SetBallParam,
	struct _tptz__SetBallParamResponse *tptz__SetBallParamResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__GetBallParam(struct soap *soap,
	struct _tptz__tptz_GetBallParam *tptz__tptz_GetBallParam,
	struct _tptz__GetBallParamResponse *tptz__GetBallParamResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __tptz__SetTrackingMovePosition(struct soap *soap,
	struct _tptz__SetTrackingMovePosition
	*tptz__SetTrackingMovePosition,
	struct _tptz__SetTrackingMovePositionResponse
	*tptz__SetTrackingMovePositionResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoSources(struct soap *soap,
	struct _trt__GetVideoSources *trt__GetVideoSources,
	struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	video_source_t vsource;

	memset(&vsource, 0, sizeof(video_source_t));
	onvif_get_video_source_info(&vsource, sizeof(video_source_t));

	trt__GetVideoSourcesResponse->__sizeVideoSources = 1;
	trt__GetVideoSourcesResponse->VideoSources =
		(struct tt__VideoSource *)soap_calloc(soap,
		trt__GetVideoSourcesResponse->__sizeVideoSources
		* sizeof(struct tt__VideoSource));

	trt__GetVideoSourcesResponse->VideoSources[0].token = 
		(char *) soap_calloc(soap, 64);
	strcpy(trt__GetVideoSourcesResponse->VideoSources[0].token,
		vsource.token);
	trt__GetVideoSourcesResponse->VideoSources[0].Framerate =
		vsource.frame_rate;
	trt__GetVideoSourcesResponse->VideoSources[0].Resolution =
		(struct tt__VideoResolution *)soap_calloc(soap,
		sizeof(struct tt__VideoResolution));
	trt__GetVideoSourcesResponse->VideoSources[0].Resolution->Width =
		vsource.width;
	trt__GetVideoSourcesResponse->VideoSources[0].Resolution->Height =
		vsource.height;
	__FUN_END("\n");
	return SOAP_OK;
}
#endif /* SUPPORT_ONVIF_2_6 */

int __trt__GetServiceCapabilities(struct soap *soap,
	struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities,
	struct _trt__GetServiceCapabilitiesResponse
	*trt__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	struct trt__Capabilities *pCapabilities =
		(struct trt__Capabilities *)soap_malloc(soap,
		sizeof(struct trt__Capabilities));
	struct trt__StreamingCapabilities *pStreamingCapabilities =
		(struct trt__StreamingCapabilities *)soap_malloc(soap,
		sizeof(struct trt__StreamingCapabilities));
	pStreamingCapabilities->NonAggregateControl =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pStreamingCapabilities->RTPMulticast =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pStreamingCapabilities->RTP_USCORERTSP_USCORETCP =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pStreamingCapabilities->RTP_USCORETCP =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pStreamingCapabilities->NoRTSPStreaming =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));

	*(pStreamingCapabilities->NonAggregateControl) = xsd__boolean__false_;
	*(pStreamingCapabilities->RTPMulticast) = xsd__boolean__false_;
	*(pStreamingCapabilities->RTP_USCORERTSP_USCORETCP) = xsd__boolean__true_;
	*(pStreamingCapabilities->RTP_USCORETCP) = xsd__boolean__true_;
	*(pStreamingCapabilities->NoRTSPStreaming) = xsd__boolean__false_;

	struct trt__ProfileCapabilities *pProfileCapabilities =
		(struct trt__ProfileCapabilities *)soap_malloc(soap,
		sizeof(struct trt__ProfileCapabilities));
	pProfileCapabilities->MaximumNumberOfProfiles =
		(int *)soap_malloc(soap, sizeof(int));
	*(pProfileCapabilities->MaximumNumberOfProfiles) = 2;

	pCapabilities->SnapshotUri =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pCapabilities->Rotation =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pCapabilities->VideoSourceMode =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pCapabilities->OSD =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pCapabilities->TemporaryOSDText =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	pCapabilities->EXICompression =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));

	*(pCapabilities->SnapshotUri) = xsd__boolean__false_;
	*(pCapabilities->Rotation) = xsd__boolean__false_;
	*(pCapabilities->VideoSourceMode) = xsd__boolean__true_;
	*(pCapabilities->OSD) = xsd__boolean__false_;
	*(pCapabilities->TemporaryOSDText) = xsd__boolean__false_;
	*(pCapabilities->EXICompression) = xsd__boolean__false_;

	pCapabilities->StreamingCapabilities = pStreamingCapabilities;
	pCapabilities->ProfileCapabilities = pProfileCapabilities;
	trt__GetServiceCapabilitiesResponse->Capabilities = pCapabilities;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioSources(struct soap *soap,
	struct _trt__GetAudioSources *trt__GetAudioSources,
	struct _trt__GetAudioSourcesResponse
	*trt__GetAudioSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	trt__GetAudioSourcesResponse->__sizeAudioSources = 1;
	
	trt__GetAudioSourcesResponse->AudioSources =
		(struct tt__AudioSource *)soap_malloc(soap,
		trt__GetAudioSourcesResponse->__sizeAudioSources
		* sizeof(struct tt__AudioSource));
	trt__GetAudioSourcesResponse->AudioSources[0].token = "AudioSource0";
	trt__GetAudioSourcesResponse->AudioSources[0].Channels = 1;
	trt__GetAudioSourcesResponse->AudioSources[0].__size = 0;
	trt__GetAudioSourcesResponse->AudioSources[0].__any = NULL;
	trt__GetAudioSourcesResponse->AudioSources[0].__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioOutputs(struct soap *soap,
	struct _trt__GetAudioOutputs *trt__GetAudioOutputs,
	struct _trt__GetAudioOutputsResponse
	*trt__GetAudioOutputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	trt__GetAudioOutputsResponse->__sizeAudioOutputs = 1;
	trt__GetAudioOutputsResponse->AudioOutputs =
		(struct tt__AudioOutput *)soap_malloc(soap,
		trt__GetAudioOutputsResponse->__sizeAudioOutputs
		* sizeof(struct tt__AudioOutput));
	trt__GetAudioOutputsResponse->AudioOutputs[0].token = "AudioOutput0";
	trt__GetAudioOutputsResponse->AudioOutputs[0].__size = 0;
	trt__GetAudioOutputsResponse->AudioOutputs[0].__any = NULL;
	trt__GetAudioOutputsResponse->AudioOutputs[0].__anyAttribute = NULL;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__CreateProfile(struct soap *soap,
	struct _trt__CreateProfile *trt__CreateProfile,
	struct _trt__CreateProfileResponse
	*trt__CreateProfileResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int GetProfile(struct soap *soap, int index, struct tt__Profile *pProfile)
{
	video_source_t vsource;
	video_encoder_t vencoder;
	if (!pProfile || index < 0 || index > 1)
		return -1;

	memset(&vsource, 0, sizeof(video_source_t));
	onvif_get_video_source_info(&vsource, sizeof(video_source_t));
#if 1
	struct tt__PTZConfiguration *pPTZConfiguration =
		(struct tt__PTZConfiguration *)soap_calloc(soap,
		sizeof(struct tt__PTZConfiguration));
	GetPTZConfiguration(soap, 0, pPTZConfiguration);
#endif
	struct tt__VideoSourceConfiguration *pVideoSourceConfiguration =
		(struct tt__VideoSourceConfiguration *)soap_calloc(soap,
		sizeof(struct tt__VideoSourceConfiguration));
	pVideoSourceConfiguration->Name = (char *)soap_calloc(soap, 64);
	pVideoSourceConfiguration->token = (char *)soap_calloc(soap, 64);
	pVideoSourceConfiguration->SourceToken = (char *)soap_calloc(soap, 64);
	pVideoSourceConfiguration->Bounds =
		(struct tt__IntRectangle *)soap_calloc(soap,
		sizeof(struct tt__IntRectangle));
	pVideoSourceConfiguration->UseCount = 2;
	sprintf(pVideoSourceConfiguration->Name, "%s", vsource.token);
	sprintf(pVideoSourceConfiguration->token, "%s", vsource.token);
	sprintf(pVideoSourceConfiguration->SourceToken, "%s", vsource.token);
	pVideoSourceConfiguration->Bounds->x = 0;
	pVideoSourceConfiguration->Bounds->y = 0;
	pVideoSourceConfiguration->Bounds->width = vsource.width;
	pVideoSourceConfiguration->Bounds->height = vsource.height;

	struct tt__AudioSourceConfiguration *pAudioSourceConfiguration =
		(struct tt__AudioSourceConfiguration *)soap_calloc(soap,
		sizeof(struct tt__AudioSourceConfiguration));
	pAudioSourceConfiguration->Name = (char *)soap_calloc(soap, 64);
	pAudioSourceConfiguration->token = (char *)soap_calloc(soap, 64);
	pAudioSourceConfiguration->SourceToken = (char *)soap_calloc(soap, 64);
	pAudioSourceConfiguration->UseCount = 2;
	sprintf(pAudioSourceConfiguration->Name, "AudioSource%d", 0);
	sprintf(pAudioSourceConfiguration->token, "AudioSource%d", 0);
	sprintf(pAudioSourceConfiguration->SourceToken, "AudioSource%d", 0);

	memset(&vencoder, 0, sizeof(video_encoder_t));
	onvif_get_video_encoder_info(index, &vencoder, sizeof(video_encoder_t));
	struct tt__VideoEncoderConfiguration *pVideoEncoderConfiguration =
		(struct tt__VideoEncoderConfiguration *)soap_calloc(soap,
		sizeof(struct tt__VideoEncoderConfiguration));
	pVideoEncoderConfiguration->Name = (char *)soap_calloc(soap, 64);
	pVideoEncoderConfiguration->token = (char *)soap_calloc(soap, 64);
	sprintf(pVideoEncoderConfiguration->Name, "%s", vencoder.token);
	sprintf(pVideoEncoderConfiguration->token, "%s", vencoder.token);
	pVideoEncoderConfiguration->UseCount = 1;
	pVideoEncoderConfiguration->Encoding = vencoder.encode_type;
	pVideoEncoderConfiguration->Resolution =
		(struct tt__VideoResolution *)soap_calloc(soap,
		sizeof(struct tt__VideoResolution));
	pVideoEncoderConfiguration->Resolution->Height = vencoder.height;
	pVideoEncoderConfiguration->Resolution->Width = vencoder.width;
	pVideoEncoderConfiguration->RateControl =
		(struct tt__VideoRateControl *)soap_calloc(soap,
		sizeof(struct tt__VideoRateControl));
	pVideoEncoderConfiguration->RateControl->FrameRateLimit =
		vencoder.frame_rate;
	pVideoEncoderConfiguration->RateControl->BitrateLimit =
		vencoder.bit_rate;
	pVideoEncoderConfiguration->RateControl->EncodingInterval =
		vencoder.encode_interval;
	pVideoEncoderConfiguration->Quality = vencoder.quality;
	pVideoEncoderConfiguration->H264 =
		(struct tt__H264Configuration *)soap_calloc(soap,
		sizeof(struct tt__H264Configuration));
	pVideoEncoderConfiguration->H264->GovLength = vencoder.gov_length;
	pVideoEncoderConfiguration->H264->H264Profile = vencoder.profile;
	pVideoEncoderConfiguration->Multicast = NULL;
	pVideoEncoderConfiguration->SessionTimeout = 0;

	struct tt__ProfileExtension *pExtension =
		(struct tt__ProfileExtension *)soap_calloc(soap,
		sizeof(struct tt__ProfileExtension));
	pExtension->AudioOutputConfiguration =
		(struct tt__AudioOutputConfiguration *)soap_calloc(soap,
		sizeof(struct tt__AudioOutputConfiguration));

	pExtension->AudioOutputConfiguration->Name =
		(char *)soap_calloc(soap, 64);
	pExtension->AudioOutputConfiguration->token =
		(char *)soap_calloc(soap, 64);
	pExtension->AudioOutputConfiguration->OutputToken =
		(char *)soap_calloc(soap, 64);
	pExtension->AudioOutputConfiguration->SendPrimacy =
		(char *)soap_calloc(soap, 64);
	strcpy(pExtension->AudioOutputConfiguration->Name, "AudioOutput0");
	strcpy(pExtension->AudioOutputConfiguration->token, "AudioOutput0");
	strcpy(pExtension->AudioOutputConfiguration->OutputToken,
		"AudioOutput");
	strcpy(pExtension->AudioOutputConfiguration->SendPrimacy,
		"www.onvif.org/ver20/HalfDuplex/Server");
	pExtension->AudioOutputConfiguration->UseCount = 2;
	pExtension->AudioOutputConfiguration->OutputLevel = 31;

	pExtension->AudioDecoderConfiguration =
		(struct tt__AudioDecoderConfiguration *)soap_calloc(soap,
		sizeof(struct tt__AudioDecoderConfiguration));
	pExtension->AudioDecoderConfiguration->Name =
		(char *)soap_calloc(soap, 64);
	pExtension->AudioDecoderConfiguration->token =
		(char *)soap_calloc(soap, 64);
	strcpy(pExtension->AudioDecoderConfiguration->Name, "AudioDecoder0");
	strcpy(pExtension->AudioDecoderConfiguration->token, "AudioDecoder0");
	pExtension->AudioDecoderConfiguration->UseCount = 2;

	pProfile->PTZConfiguration = pPTZConfiguration;
	pProfile->VideoSourceConfiguration = pVideoSourceConfiguration;
	pProfile->AudioSourceConfiguration = pAudioSourceConfiguration;
	pProfile->VideoEncoderConfiguration = pVideoEncoderConfiguration;
	pProfile->Extension = pExtension;

	pProfile->Name = (char *)soap_malloc(soap, 64);
	pProfile->token = (char *)soap_malloc(soap, 64);
	memset(pProfile->Name, 0, 64);
	memset(pProfile->token, 0, 64);
	onvif_get_media_profile(index, pProfile->Name, 64);
	onvif_get_media_profile(index, pProfile->token, 64);

	pProfile->fixed =
		(enum xsd__boolean *)soap_malloc(soap,
		sizeof(enum xsd__boolean));
	*(pProfile->fixed) = 1;
	pProfile->__anyAttribute = NULL;

	return 0;
}

int __trt__GetProfile(struct soap *soap,
	struct _trt__GetProfile *trt__GetProfile,
	struct _trt__GetProfileResponse *trt__GetProfileResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	int index = 0;
	if (trt__GetProfile->ProfileToken) {
		printf("__trt__GetProfile:%s\n", trt__GetProfile->ProfileToken);
		if (strcmp(trt__GetProfile->ProfileToken, "MajorStream"))
			index = 0;
		else
			index = 1;
	}

	struct tt__Profile *pProfile;
	ALLOC_STRUCT(pProfile, struct tt__Profile);

	GetProfile(soap, index, pProfile);

	trt__GetProfileResponse->Profile = pProfile;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetProfiles(struct soap *soap,
	struct _trt__GetProfiles *trt__GetProfiles,
	struct _trt__GetProfilesResponse *trt__GetProfilesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	trt__GetProfilesResponse->__sizeProfiles = 2;
	struct tt__Profile *pProfiles;
	ALLOC_STRUCT_NUM(pProfiles,
		struct tt__Profile,
		trt__GetProfilesResponse->__sizeProfiles);
	GetProfile(soap, 0, &pProfiles[0]);
	GetProfile(soap, 1, &pProfiles[1]);

	trt__GetProfilesResponse->Profiles = pProfiles;

	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__AddVideoEncoderConfiguration(struct soap *soap,
	struct _trt__AddVideoEncoderConfiguration
	*trt__AddVideoEncoderConfiguration,
	struct _trt__AddVideoEncoderConfigurationResponse
	*trt__AddVideoEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddVideoSourceConfiguration(struct soap *soap,
	struct _trt__AddVideoSourceConfiguration
	*trt__AddVideoSourceConfiguration,
	struct _trt__AddVideoSourceConfigurationResponse
	*trt__AddVideoSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddAudioEncoderConfiguration(struct soap *soap,
	struct _trt__AddAudioEncoderConfiguration
	*trt__AddAudioEncoderConfiguration,
	struct _trt__AddAudioEncoderConfigurationResponse
	*trt__AddAudioEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddAudioSourceConfiguration(struct soap *soap,
	struct _trt__AddAudioSourceConfiguration
	*trt__AddAudioSourceConfiguration,
	struct _trt__AddAudioSourceConfigurationResponse
	*trt__AddAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddPTZConfiguration(struct soap *soap,
	struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration,
	struct _trt__AddPTZConfigurationResponse
	*trt__AddPTZConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddVideoAnalyticsConfiguration(struct soap *soap,
	struct _trt__AddVideoAnalyticsConfiguration
	*trt__AddVideoAnalyticsConfiguration,
	struct _trt__AddVideoAnalyticsConfigurationResponse
	*trt__AddVideoAnalyticsConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__AddMetadataConfiguration(struct soap *soap,
	struct _trt__AddMetadataConfiguration
	*trt__AddMetadataConfiguration,
	struct _trt__AddMetadataConfigurationResponse
	*trt__AddMetadataConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddAudioOutputConfiguration(struct soap *soap,
	struct _trt__AddAudioOutputConfiguration
	*trt__AddAudioOutputConfiguration,
	struct _trt__AddAudioOutputConfigurationResponse
	*trt__AddAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__AddAudioDecoderConfiguration(struct soap *soap,
	struct _trt__AddAudioDecoderConfiguration
	*trt__AddAudioDecoderConfiguration,
	struct _trt__AddAudioDecoderConfigurationResponse
	*trt__AddAudioDecoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveVideoEncoderConfiguration(struct soap *soap,
	struct _trt__RemoveVideoEncoderConfiguration
	*trt__RemoveVideoEncoderConfiguration,
	struct _trt__RemoveVideoEncoderConfigurationResponse
	*trt__RemoveVideoEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveVideoSourceConfiguration(struct soap *soap,
	struct _trt__RemoveVideoSourceConfiguration
	*trt__RemoveVideoSourceConfiguration,
	struct _trt__RemoveVideoSourceConfigurationResponse
	*trt__RemoveVideoSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveAudioEncoderConfiguration(struct soap *soap,
	struct _trt__RemoveAudioEncoderConfiguration
	*trt__RemoveAudioEncoderConfiguration,
	struct _trt__RemoveAudioEncoderConfigurationResponse
	*trt__RemoveAudioEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveAudioSourceConfiguration(struct soap *soap,
	struct _trt__RemoveAudioSourceConfiguration
	*trt__RemoveAudioSourceConfiguration,
	struct _trt__RemoveAudioSourceConfigurationResponse
	*trt__RemoveAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemovePTZConfiguration(struct soap *soap,
	struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration,
	struct _trt__RemovePTZConfigurationResponse
	*trt__RemovePTZConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __trt__RemoveVideoAnalyticsConfiguration(struct soap *soap,
	struct _trt__RemoveVideoAnalyticsConfiguration
	*trt__RemoveVideoAnalyticsConfiguration,
	struct _trt__RemoveVideoAnalyticsConfigurationResponse
	*trt__RemoveVideoAnalyticsConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveMetadataConfiguration(struct soap *soap,
	struct _trt__RemoveMetadataConfiguration
	*trt__RemoveMetadataConfiguration,
	struct _trt__RemoveMetadataConfigurationResponse
	*trt__RemoveMetadataConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveAudioOutputConfiguration(struct soap *soap,
	struct _trt__RemoveAudioOutputConfiguration
	*trt__RemoveAudioOutputConfiguration,
	struct _trt__RemoveAudioOutputConfigurationResponse
	*trt__RemoveAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__RemoveAudioDecoderConfiguration(struct soap *soap,
	struct _trt__RemoveAudioDecoderConfiguration
	*trt__RemoveAudioDecoderConfiguration,
	struct _trt__RemoveAudioDecoderConfigurationResponse
	*trt__RemoveAudioDecoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__DeleteProfile(struct soap *soap,
	struct _trt__DeleteProfile *trt__DeleteProfile,
	struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetVideoSourceConfigurations(struct soap *soap,
	struct _trt__GetVideoSourceConfigurations
	*trt__GetVideoSourceConfigurations,
	struct _trt__GetVideoSourceConfigurationsResponse
	*trt__GetVideoSourceConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoEncoderConfigurations(struct soap *soap,
	struct _trt__GetVideoEncoderConfigurations
	*trt__GetVideoEncoderConfigurations,
	struct _trt__GetVideoEncoderConfigurationsResponse
	*trt__GetVideoEncoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioSourceConfigurations(struct soap *soap,
	struct _trt__GetAudioSourceConfigurations
	*trt__GetAudioSourceConfigurations,
	struct _trt__GetAudioSourceConfigurationsResponse
	*trt__GetAudioSourceConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioEncoderConfigurations(struct soap *soap,
	struct _trt__GetAudioEncoderConfigurations
	*trt__GetAudioEncoderConfigurations,
	struct _trt__GetAudioEncoderConfigurationsResponse
	*trt__GetAudioEncoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoAnalyticsConfigurations(struct soap *soap,
	struct _trt__GetVideoAnalyticsConfigurations
	*trt__GetVideoAnalyticsConfigurations,
	struct _trt__GetVideoAnalyticsConfigurationsResponse
	*trt__GetVideoAnalyticsConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetMetadataConfigurations(struct soap *soap,
	struct _trt__GetMetadataConfigurations
	*trt__GetMetadataConfigurations,
	struct _trt__GetMetadataConfigurationsResponse
	*trt__GetMetadataConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetAudioOutputConfigurations(struct soap *soap,
	struct _trt__GetAudioOutputConfigurations
	*trt__GetAudioOutputConfigurations,
	struct _trt__GetAudioOutputConfigurationsResponse
	*trt__GetAudioOutputConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetAudioDecoderConfigurations(struct soap *soap,
	struct _trt__GetAudioDecoderConfigurations
	*trt__GetAudioDecoderConfigurations,
	struct _trt__GetAudioDecoderConfigurationsResponse
	*trt__GetAudioDecoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __trt__GetVideoSourceConfiguration(struct soap *soap,
	struct _trt__GetVideoSourceConfiguration
	*trt__GetVideoSourceConfiguration,
	struct _trt__GetVideoSourceConfigurationResponse
	*trt__GetVideoSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoEncoderConfiguration(struct soap *soap,
	struct _trt__GetVideoEncoderConfiguration
	*trt__GetVideoEncoderConfiguration,
	struct _trt__GetVideoEncoderConfigurationResponse
	*trt__GetVideoEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioSourceConfiguration(struct soap *soap,
	struct _trt__GetAudioSourceConfiguration
	*trt__GetAudioSourceConfiguration,
	struct _trt__GetAudioSourceConfigurationResponse
	*trt__GetAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioEncoderConfiguration(struct soap *soap,
	struct _trt__GetAudioEncoderConfiguration
	*trt__GetAudioEncoderConfiguration,
	struct _trt__GetAudioEncoderConfigurationResponse
	*trt__GetAudioEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoAnalyticsConfiguration(struct soap *soap,
	struct _trt__GetVideoAnalyticsConfiguration
	*trt__GetVideoAnalyticsConfiguration,
	struct _trt__GetVideoAnalyticsConfigurationResponse
	*trt__GetVideoAnalyticsConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetMetadataConfiguration(struct soap *soap,
	struct _trt__GetMetadataConfiguration
	*trt__GetMetadataConfiguration,
	struct _trt__GetMetadataConfigurationResponse
	*trt__GetMetadataConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetAudioOutputConfiguration(struct soap *soap,
	struct _trt__GetAudioOutputConfiguration
	*trt__GetAudioOutputConfiguration,
	struct _trt__GetAudioOutputConfigurationResponse
	*trt__GetAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetAudioDecoderConfiguration(struct soap *soap,
	struct _trt__GetAudioDecoderConfiguration
	*trt__GetAudioDecoderConfiguration,
	struct _trt__GetAudioDecoderConfigurationResponse
	*trt__GetAudioDecoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetCompatibleVideoEncoderConfigurations(struct soap *soap,
	struct _trt__GetCompatibleVideoEncoderConfigurations
	*trt__GetCompatibleVideoEncoderConfigurations,
	struct _trt__GetCompatibleVideoEncoderConfigurationsResponse
	*trt__GetCompatibleVideoEncoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetCompatibleVideoSourceConfigurations(struct soap *soap,
	struct _trt__GetCompatibleVideoSourceConfigurations
	*trt__GetCompatibleVideoSourceConfigurations,
	struct _trt__GetCompatibleVideoSourceConfigurationsResponse
	*trt__GetCompatibleVideoSourceConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetCompatibleAudioEncoderConfigurations(struct soap *soap,
	struct _trt__GetCompatibleAudioEncoderConfigurations
	*trt__GetCompatibleAudioEncoderConfigurations,
	struct _trt__GetCompatibleAudioEncoderConfigurationsResponse
	*trt__GetCompatibleAudioEncoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetCompatibleAudioSourceConfigurations(struct soap *soap,
	struct _trt__GetCompatibleAudioSourceConfigurations
	*trt__GetCompatibleAudioSourceConfigurations,
	struct _trt__GetCompatibleAudioSourceConfigurationsResponse
	*trt__GetCompatibleAudioSourceConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetCompatibleVideoAnalyticsConfigurations(struct soap *soap,
	struct _trt__GetCompatibleVideoAnalyticsConfigurations
	*trt__GetCompatibleVideoAnalyticsConfigurations,
	struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse
	*trt__GetCompatibleVideoAnalyticsConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetCompatibleMetadataConfigurations(struct soap *soap,
	struct _trt__GetCompatibleMetadataConfigurations
	*trt__GetCompatibleMetadataConfigurations,
	struct _trt__GetCompatibleMetadataConfigurationsResponse
	*trt__GetCompatibleMetadataConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetCompatibleAudioOutputConfigurations(struct soap *soap,
	struct _trt__GetCompatibleAudioOutputConfigurations
	*trt__GetCompatibleAudioOutputConfigurations,
	struct _trt__GetCompatibleAudioOutputConfigurationsResponse
	*trt__GetCompatibleAudioOutputConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetCompatibleAudioDecoderConfigurations(struct soap *soap,
	struct _trt__GetCompatibleAudioDecoderConfigurations
	*trt__GetCompatibleAudioDecoderConfigurations,
	struct _trt__GetCompatibleAudioDecoderConfigurationsResponse
	*trt__GetCompatibleAudioDecoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__SetVideoSourceConfiguration(struct soap *soap,
	struct _trt__SetVideoSourceConfiguration
	*trt__SetVideoSourceConfiguration,
	struct _trt__SetVideoSourceConfigurationResponse
	*trt__SetVideoSourceConfigurationResponse)
{

	if (trt__SetVideoSourceConfiguration->Configuration != NULL)
		return SOAP_FAULT;

	onvif_fault(soap, "ter:InvalidArgVal", "ter:ConfigModify");
	return SOAP_FAULT;
}

int __trt__SetVideoEncoderConfiguration(struct soap *soap,
	struct _trt__SetVideoEncoderConfiguration
	*trt__SetVideoEncoderConfiguration,
	struct _trt__SetVideoEncoderConfigurationResponse
	*trt__SetVideoEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__SetAudioSourceConfiguration(struct soap *soap,
	struct _trt__SetAudioSourceConfiguration
	*trt__SetAudioSourceConfiguration,
	struct _trt__SetAudioSourceConfigurationResponse
	*trt__SetAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int  __trt__SetAudioEncoderConfiguration(struct soap *soap,
	struct _trt__SetAudioEncoderConfiguration
	*trt__SetAudioEncoderConfiguration,
	struct _trt__SetAudioEncoderConfigurationResponse
	*trt__SetAudioEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__SetVideoAnalyticsConfiguration(struct soap *soap,
	struct _trt__SetVideoAnalyticsConfiguration
	*trt__SetVideoAnalyticsConfiguration,
	struct _trt__SetVideoAnalyticsConfigurationResponse
	*trt__SetVideoAnalyticsConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__SetMetadataConfiguration(struct soap *soap,
	struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration,
	struct _trt__SetMetadataConfigurationResponse
	*trt__SetMetadataConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__SetAudioOutputConfiguration(struct soap *soap,
	struct _trt__SetAudioOutputConfiguration
	*trt__SetAudioOutputConfiguration,
	struct _trt__SetAudioOutputConfigurationResponse
	*trt__SetAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__SetAudioDecoderConfiguration(struct soap *soap,
	struct _trt__SetAudioDecoderConfiguration
	*trt__SetAudioDecoderConfiguration,
	struct _trt__SetAudioDecoderConfigurationResponse
	*trt__SetAudioDecoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetVideoSourceConfigurationOptions(struct soap *soap,
	struct _trt__GetVideoSourceConfigurationOptions
	*trt__GetVideoSourceConfigurationOptions,
	struct _trt__GetVideoSourceConfigurationOptionsResponse
	*trt__GetVideoSourceConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoEncoderConfigurationOptions(struct soap *soap,
	struct _trt__GetVideoEncoderConfigurationOptions
	*trt__GetVideoEncoderConfigurationOptions,
	struct _trt__GetVideoEncoderConfigurationOptionsResponse
	*trt__GetVideoEncoderConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	video_encoder_t vencoder;

	struct tt__VideoEncoderConfigurationOptions *pOptions =
		(struct tt__VideoEncoderConfigurationOptions *)soap_calloc(soap,
		sizeof(struct tt__VideoEncoderConfigurationOptions));
	pOptions->QualityRange =
		(struct tt__IntRange *)soap_calloc(soap,
		sizeof(struct tt__IntRange));
	pOptions->QualityRange->Max = 10;
	pOptions->QualityRange->Min = 0;

	pOptions->JPEG = NULL;
	pOptions->MPEG4 = NULL;

	pOptions->H264 =
		(struct tt__H264Options *)soap_calloc(soap,
		sizeof(struct tt__H264Options));

	pOptions->H264->__sizeResolutionsAvailable = 2;
	pOptions->H264->ResolutionsAvailable =
		(struct tt__VideoResolution *)soap_calloc(soap,
		pOptions->H264->__sizeResolutionsAvailable
		* sizeof(struct tt__VideoResolution));
	memset(&vencoder, 0, sizeof(video_encoder_t));
	onvif_get_video_encoder_info(0, &vencoder, sizeof(video_encoder_t));
	pOptions->H264->ResolutionsAvailable[0].Width = vencoder.width;
	pOptions->H264->ResolutionsAvailable[0].Height= vencoder.height;
	memset(&vencoder, 0, sizeof(video_encoder_t));
	onvif_get_video_encoder_info(1, &vencoder, sizeof(video_encoder_t));
	pOptions->H264->ResolutionsAvailable[1].Width = vencoder.width;
	pOptions->H264->ResolutionsAvailable[1].Height= vencoder.height;

	pOptions->H264->GovLengthRange =
		(struct tt__IntRange *)soap_calloc(soap,
		sizeof(struct tt__IntRange));
	pOptions->H264->GovLengthRange->Max = 100;
	pOptions->H264->GovLengthRange->Min = 0;

	pOptions->H264->FrameRateRange =
		(struct tt__IntRange *)soap_calloc(soap,
		sizeof(struct tt__IntRange));
	pOptions->H264->FrameRateRange->Max = 30;
	pOptions->H264->FrameRateRange->Min = 0;

	pOptions->H264->EncodingIntervalRange =
		(struct tt__IntRange *)soap_calloc(soap,
		sizeof(struct tt__IntRange));
	pOptions->H264->EncodingIntervalRange->Max = 250;
	pOptions->H264->EncodingIntervalRange->Min = 10;

	pOptions->H264->__sizeH264ProfilesSupported = 3;
	pOptions->H264->H264ProfilesSupported =
		(enum tt__H264Profile *)soap_calloc(soap,
		sizeof(enum tt__H264Profile)
		* pOptions->H264->__sizeH264ProfilesSupported);
	pOptions->H264->H264ProfilesSupported[0] = tt__H264Profile__Baseline;
	pOptions->H264->H264ProfilesSupported[1] = tt__H264Profile__Main;
	pOptions->H264->H264ProfilesSupported[2] = tt__H264Profile__Extended;

	pOptions->Extension = NULL;
	pOptions->__anyAttribute = NULL;

	trt__GetVideoEncoderConfigurationOptionsResponse->Options = pOptions;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioSourceConfigurationOptions(struct soap *soap,
	struct _trt__GetAudioSourceConfigurationOptions
	*trt__GetAudioSourceConfigurationOptions,
	struct _trt__GetAudioSourceConfigurationOptionsResponse
	*trt__GetAudioSourceConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetAudioEncoderConfigurationOptions(struct soap *soap,
	struct _trt__GetAudioEncoderConfigurationOptions
	*trt__GetAudioEncoderConfigurationOptions,
	struct _trt__GetAudioEncoderConfigurationOptionsResponse
	*trt__GetAudioEncoderConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetMetadataConfigurationOptions(struct soap *soap,
	struct _trt__GetMetadataConfigurationOptions
	*trt__GetMetadataConfigurationOptions,
	struct _trt__GetMetadataConfigurationOptionsResponse
	*trt__GetMetadataConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetAudioOutputConfigurationOptions(struct soap *soap,
	struct _trt__GetAudioOutputConfigurationOptions
	*trt__GetAudioOutputConfigurationOptions,
	struct _trt__GetAudioOutputConfigurationOptionsResponse
	*trt__GetAudioOutputConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetAudioDecoderConfigurationOptions(struct soap *soap,
	struct _trt__GetAudioDecoderConfigurationOptions
	*trt__GetAudioDecoderConfigurationOptions,
	struct _trt__GetAudioDecoderConfigurationOptionsResponse
	*trt__GetAudioDecoderConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetGuaranteedNumberOfVideoEncoderInstances(struct soap *soap,
	struct _trt__GetGuaranteedNumberOfVideoEncoderInstances
	*trt__GetGuaranteedNumberOfVideoEncoderInstances,
	struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse
	*trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;

	if (strcmp(trt__GetGuaranteedNumberOfVideoEncoderInstances->
			ConfigurationToken,
			ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN) != 0) {
		onvif_fault(soap, "ter:InvalidArgVal", "ter:NoConfig");
		return SOAP_FAULT;
	}

	trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse->
		TotalNumber = 1;
	return SOAP_OK;
}

int __trt__GetStreamUri(struct soap *soap,
	struct _trt__GetStreamUri *trt__GetStreamUri,
	struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (trt__GetStreamUri->ProfileToken)
		printf("Get Stream:%s uri\n", trt__GetStreamUri->ProfileToken);
	else
		printf("Get Stream: uri is null\n");

	struct tt__MediaUri *pMediaUri;
	ALLOC_STRUCT(pMediaUri, struct tt__MediaUri);
	trt__GetStreamUriResponse->MediaUri = pMediaUri;

	ALLOC_STRUCT_NUM(pMediaUri->Uri, char, 256);
	onvif_get_media_uri(trt__GetStreamUri->ProfileToken,
		pMediaUri->Uri, 256);
	printf("uri:%s\n", pMediaUri->Uri);

	pMediaUri->Timeout = (LONG64)(10*1000);
	pMediaUri->InvalidAfterConnect = xsd__boolean__true_;
	pMediaUri->InvalidAfterReboot  = xsd__boolean__true_;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__StartMulticastStreaming(struct soap *soap,
	struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming,
	struct _trt__StartMulticastStreamingResponse
	*trt__StartMulticastStreamingResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __trt__StopMulticastStreaming(struct soap *soap,
	struct _trt__StopMulticastStreaming
	*trt__StopMulticastStreaming,
	struct _trt__StopMulticastStreamingResponse
	*trt__StopMulticastStreamingResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int  __trt__SetSynchronizationPoint(struct soap* soap,
	struct _trt__SetSynchronizationPoint* trt__SetSynchronizationPoint,
	struct _trt__SetSynchronizationPointResponse*
	trt__SetSynchronizationPointResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetSnapshotUri(struct soap *soap,
	struct _trt__GetSnapshotUri *trt__GetSnapshotUri,
	struct _trt__GetSnapshotUriResponse
	*trt__GetSnapshotUriResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	if (trt__GetSnapshotUri->ProfileToken)
		printf("ProfileToken %s\n",
			trt__GetSnapshotUri->ProfileToken);

	ALLOC_STRUCT(trt__GetSnapshotUriResponse->MediaUri,
		struct tt__MediaUri);

	ALLOC_STRUCT_NUM(trt__GetSnapshotUriResponse->MediaUri->Uri,
		char, 256);
	onvif_get_snapshot_uri(trt__GetSnapshotUri->ProfileToken,
		trt__GetSnapshotUriResponse->MediaUri->Uri, 256);

	trt__GetSnapshotUriResponse->MediaUri->Timeout = (LONG64)(10 * 1000);
	trt__GetSnapshotUriResponse->MediaUri->InvalidAfterConnect =
			xsd__boolean__true_;
	trt__GetSnapshotUriResponse->MediaUri->InvalidAfterReboot  =
			xsd__boolean__true_;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetVideoSourceModes(struct soap *soap,
	struct _trt__GetVideoSourceModes *trt__GetVideoSourceModes,
	struct _trt__GetVideoSourceModesResponse
	*trt__GetVideoSourceModesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__SetVideoSourceMode(struct soap *soap,
	struct _trt__SetVideoSourceMode *trt__SetVideoSourceMode,
	struct _trt__SetVideoSourceModeResponse
	*trt__SetVideoSourceModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetOSDs(struct soap *soap,
	struct _trt__GetOSDs *trt__GetOSDs,
	struct _trt__GetOSDsResponse *trt__GetOSDsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__GetOSD(struct soap *soap,
	struct _trt__GetOSD *trt__GetOSD,
	struct _trt__GetOSDResponse *trt__GetOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}

int __trt__GetOSDOptions(struct soap *soap,
	struct _trt__GetOSDOptions *trt__GetOSDOptions,
	struct _trt__GetOSDOptionsResponse *trt__GetOSDOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__SetOSD(struct soap *soap,
	struct _trt__SetOSD *trt__SetOSD,
	struct _trt__SetOSDResponse *trt__SetOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__CreateOSD(struct soap *soap,
	struct _trt__CreateOSD *trt__CreateOSD,
	struct _trt__CreateOSDResponse *trt__CreateOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_OK;
}

int __trt__DeleteOSD(struct soap *soap,
	struct _trt__DeleteOSD *trt__DeleteOSD,
	struct _trt__DeleteOSDResponse *trt__DeleteOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return SOAP_FAULT;
}


/**
 * onvif 2.6 add function
 */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSources(struct soap* soap,
	struct _trt__GetVideoSources* trt__GetVideoSources,
	struct _trt__GetVideoSourcesResponse* trt__GetVideoSourcesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	video_source_t vsource;

	memset(&vsource, 0, sizeof(video_source_t));
	onvif_get_video_source_info(&vsource, sizeof(video_source_t));

	trt__GetVideoSourcesResponse->__sizeVideoSources = 1;
	trt__GetVideoSourcesResponse->VideoSources =
		(struct tt__VideoSource *)soap_calloc(soap,
		trt__GetVideoSourcesResponse->__sizeVideoSources
		* sizeof(struct tt__VideoSource));

	trt__GetVideoSourcesResponse->VideoSources[0].token = 
		(char *) soap_calloc(soap, 64);
	strcpy(trt__GetVideoSourcesResponse->VideoSources[0].token,
		vsource.token);
	trt__GetVideoSourcesResponse->VideoSources[0].Framerate =
		vsource.frame_rate;
	trt__GetVideoSourcesResponse->VideoSources[0].Resolution =
		(struct tt__VideoResolution *)soap_calloc(soap,
		sizeof(struct tt__VideoResolution));
	trt__GetVideoSourcesResponse->VideoSources[0].Resolution->Width =
		vsource.width;
	trt__GetVideoSourcesResponse->VideoSources[0].Resolution->Height =
		vsource.height;

	__FUN_END("\n");
	return SOAP_OK;
}


/** newly added onvif 2.6 function list */
SOAP_FMAC5 int SOAP_FMAC6 __tls__GetServiceCapabilities(struct soap *soap,
	struct _tls__GetServiceCapabilities *tls__GetServiceCapabilities,
	struct _tls__GetServiceCapabilitiesResponse
	*tls__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetLayout(struct soap *soap,
	struct _tls__GetLayout *tls__GetLayout,
	struct _tls__GetLayoutResponse *tls__GetLayoutResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__SetLayout(struct soap *soap,
	struct _tls__SetLayout *tls__SetLayout,
	struct _tls__SetLayoutResponse *tls__SetLayoutResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetDisplayOptions(struct soap *soap,
	struct _tls__GetDisplayOptions *tls__GetDisplayOptions,
	struct _tls__GetDisplayOptionsResponse
	*tls__GetDisplayOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetPaneConfigurations(struct soap *soap,
	struct _tls__GetPaneConfigurations *tls__GetPaneConfigurations,
	struct _tls__GetPaneConfigurationsResponse
	*tls__GetPaneConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetPaneConfiguration(struct soap *soap,
	struct _tls__GetPaneConfiguration *tls__GetPaneConfiguration,
	struct _tls__GetPaneConfigurationResponse
	*tls__GetPaneConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__SetPaneConfigurations(struct soap *soap,
	struct _tls__SetPaneConfigurations *tls__SetPaneConfigurations,
	struct _tls__SetPaneConfigurationsResponse
	*tls__SetPaneConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__SetPaneConfiguration(struct soap *soap,
	struct _tls__SetPaneConfiguration *tls__SetPaneConfiguration,
	struct _tls__SetPaneConfigurationResponse
	*tls__SetPaneConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__CreatePaneConfiguration(struct soap *soap,
	struct _tls__CreatePaneConfiguration *tls__CreatePaneConfiguration,
	struct _tls__CreatePaneConfigurationResponse
	*tls__CreatePaneConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tls__DeletePaneConfiguration(struct soap *soap,
	struct _tls__DeletePaneConfiguration *tls__DeletePaneConfiguration,
	struct _tls__DeletePaneConfigurationResponse
	*tls__DeletePaneConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}


SOAP_FMAC5 int SOAP_FMAC6 __trc__GetServiceCapabilities(struct soap *soap,
	struct _trc__GetServiceCapabilities *trc__GetServiceCapabilities,
	struct _trc__GetServiceCapabilitiesResponse
	*trc__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateRecording(struct soap *soap,
	struct _trc__CreateRecording *trc__CreateRecording,
	struct _trc__CreateRecordingResponse
	*trc__CreateRecordingResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteRecording(struct soap *soap,
	struct _trc__DeleteRecording *trc__DeleteRecording,
	struct _trc__DeleteRecordingResponse *trc__DeleteRecordingResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordings(struct soap *soap,
	struct _trc__GetRecordings *trc__GetRecordings,
	struct _trc__GetRecordingsResponse *trc__GetRecordingsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingConfiguration(struct soap *soap,
	struct _trc__SetRecordingConfiguration *trc__SetRecordingConfiguration,
	struct _trc__SetRecordingConfigurationResponse
	*trc__SetRecordingConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingConfiguration(struct soap *soap,
	struct _trc__GetRecordingConfiguration *trc__GetRecordingConfiguration,
	struct _trc__GetRecordingConfigurationResponse
	*trc__GetRecordingConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateTrack(struct soap *soap,
	struct _trc__CreateTrack *trc__CreateTrack,
	struct _trc__CreateTrackResponse *trc__CreateTrackResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteTrack(struct soap *soap,
	struct _trc__DeleteTrack *trc__DeleteTrack,
	struct _trc__DeleteTrackResponse *trc__DeleteTrackResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetTrackConfiguration(struct soap *soap,
	struct _trc__GetTrackConfiguration *trc__GetTrackConfiguration,
	struct _trc__GetTrackConfigurationResponse
	*trc__GetTrackConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetTrackConfiguration(struct soap *soap,
	struct _trc__SetTrackConfiguration *trc__SetTrackConfiguration,
	struct _trc__SetTrackConfigurationResponse
	*trc__SetTrackConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateRecordingJob(struct soap *soap,
	struct _trc__CreateRecordingJob *trc__CreateRecordingJob,
	struct _trc__CreateRecordingJobResponse
	*trc__CreateRecordingJobResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteRecordingJob(struct soap *soap,
	struct _trc__DeleteRecordingJob *trc__DeleteRecordingJob,
	struct _trc__DeleteRecordingJobResponse
	*trc__DeleteRecordingJobResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobs(struct soap *soap,
	struct _trc__GetRecordingJobs *trc__GetRecordingJobs,
	struct _trc__GetRecordingJobsResponse *trc__GetRecordingJobsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingJobConfiguration(struct soap *soap,
	struct _trc__SetRecordingJobConfiguration *trc__SetRecordingJobConfiguration,
	struct _trc__SetRecordingJobConfigurationResponse
	*trc__SetRecordingJobConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobConfiguration(struct soap *soap,
	struct _trc__GetRecordingJobConfiguration *trc__GetRecordingJobConfiguration,
	struct _trc__GetRecordingJobConfigurationResponse
	*trc__GetRecordingJobConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingJobMode(struct soap *soap,
	struct _trc__SetRecordingJobMode *trc__SetRecordingJobMode,
	struct _trc__SetRecordingJobModeResponse
	*trc__SetRecordingJobModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobState(struct soap *soap,
	struct _trc__GetRecordingJobState *trc__GetRecordingJobState,
	struct _trc__GetRecordingJobStateResponse
	*trc__GetRecordingJobStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

/**
 * Web service operation '__tr2__GetServiceCapabilities'
 * (returns SOAP_OK or error code)
 */
SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetServiceCapabilities(struct soap *soap,
	struct _tr2__GetServiceCapabilities *tr2__GetServiceCapabilities,
	struct _tr2__GetServiceCapabilitiesResponse
	*tr2__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}
/**
 * Web service operation '__tr2__CreateProfile'
 * (returns SOAP_OK or error code) 
 */
SOAP_FMAC5 int SOAP_FMAC6 __tr2__CreateProfile(struct soap *soap,
	struct _tr2__CreateProfile *tr2__CreateProfile,
	struct _tr2__CreateProfileResponse
	*tr2__CreateProfileResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

/**
 * Web service operation '__tr2__GetProfiles'
 * (returns SOAP_OK or error code)
 */
SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetProfiles(struct soap *soap,
	struct _tr2__GetProfiles *tr2__GetProfiles,
	struct _tr2__GetProfilesResponse *tr2__GetProfilesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__AddConfiguration(struct soap *soap,
	struct _tr2__AddConfiguration *tr2__AddConfiguration,
	struct _tr2__AddConfigurationResponse
	*tr2__AddConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__RemoveConfiguration(struct soap *soap,
	struct _tr2__RemoveConfiguration *tr2__RemoveConfiguration,
	struct _tr2__RemoveConfigurationResponse
	*tr2__RemoveConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__DeleteProfile(struct soap *soap,
	struct _tr2__DeleteProfile *tr2__DeleteProfile,
	struct _tr2__DeleteProfileResponse *tr2__DeleteProfileResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetVideoSourceConfigurations(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetVideoSourceConfigurations,
	struct _tr2__GetVideoSourceConfigurationsResponse
	*tr2__GetVideoSourceConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetVideoEncoderConfigurations(struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetVideoEncoderConfigurations,
	struct _tr2__GetVideoEncoderConfigurationsResponse
	*tr2__GetVideoEncoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioSourceConfigurations(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioSourceConfigurations,
	struct _tr2__GetAudioSourceConfigurationsResponse
	*tr2__GetAudioSourceConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioEncoderConfigurations(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioEncoderConfigurations,
	struct _tr2__GetAudioEncoderConfigurationsResponse
	*tr2__GetAudioEncoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAnalyticsConfigurations(struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAnalyticsConfigurations,
	struct _tr2__GetAnalyticsConfigurationsResponse
	*tr2__GetAnalyticsConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetMetadataConfigurations(struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetMetadataConfigurations,
	struct _tr2__GetMetadataConfigurationsResponse
	*tr2__GetMetadataConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioOutputConfigurations(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioOutputConfigurations,
	struct _tr2__GetAudioOutputConfigurationsResponse
	*tr2__GetAudioOutputConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioDecoderConfigurations(
	struct soap *soap,
	struct tr2__GetConfiguration
	*tr2__GetAudioDecoderConfigurations,
	struct _tr2__GetAudioDecoderConfigurationsResponse
	*tr2__GetAudioDecoderConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetVideoSourceConfiguration(struct soap *soap,
	struct _tr2__SetVideoSourceConfiguration
	*tr2__SetVideoSourceConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetVideoSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetVideoEncoderConfiguration(struct soap *soap,
	struct _tr2__SetVideoEncoderConfiguration
	*tr2__SetVideoEncoderConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetVideoEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetAudioSourceConfiguration(struct soap *soap,
	struct _tr2__SetAudioSourceConfiguration
	*tr2__SetAudioSourceConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetAudioSourceConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetAudioEncoderConfiguration(struct soap *soap,
	struct _tr2__SetAudioEncoderConfiguration
	*tr2__SetAudioEncoderConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetAudioEncoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetMetadataConfiguration(struct soap *soap,
	struct _tr2__SetMetadataConfiguration *tr2__SetMetadataConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetMetadataConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetAudioOutputConfiguration(struct soap *soap,
	struct _tr2__SetAudioOutputConfiguration
	*tr2__SetAudioOutputConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetAudioOutputConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetAudioDecoderConfiguration(struct soap *soap,
	struct _tr2__SetAudioDecoderConfiguration
	*tr2__SetAudioDecoderConfiguration,
	struct tr2__SetConfigurationResponse
	*tr2__SetAudioDecoderConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetVideoSourceConfigurationOptions(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetVideoSourceConfigurationOptions,
	struct _tr2__GetVideoSourceConfigurationOptionsResponse
	*tr2__GetVideoSourceConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetVideoEncoderConfigurationOptions(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetVideoEncoderConfigurationOptions,
	struct _tr2__GetVideoEncoderConfigurationOptionsResponse
	*tr2__GetVideoEncoderConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioSourceConfigurationOptions(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioSourceConfigurationOptions,
	struct _tr2__GetAudioSourceConfigurationOptionsResponse
	*tr2__GetAudioSourceConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioEncoderConfigurationOptions(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioEncoderConfigurationOptions,
	struct _tr2__GetAudioEncoderConfigurationOptionsResponse
	*tr2__GetAudioEncoderConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetMetadataConfigurationOptions(struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetMetadataConfigurationOptions,
	struct _tr2__GetMetadataConfigurationOptionsResponse
	*tr2__GetMetadataConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioOutputConfigurationOptions(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioOutputConfigurationOptions,
	struct _tr2__GetAudioOutputConfigurationOptionsResponse
	*tr2__GetAudioOutputConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetAudioDecoderConfigurationOptions(
	struct soap *soap,
	struct tr2__GetConfiguration *tr2__GetAudioDecoderConfigurationOptions,
	struct _tr2__GetAudioDecoderConfigurationOptionsResponse
	*tr2__GetAudioDecoderConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetVideoEncoderInstances(struct soap *soap,
	struct _tr2__GetVideoEncoderInstances *tr2__GetVideoEncoderInstances,
	struct _tr2__GetVideoEncoderInstancesResponse
	*tr2__GetVideoEncoderInstancesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetStreamUri(struct soap *soap,
	struct _tr2__GetStreamUri *tr2__GetStreamUri,
	struct _tr2__GetStreamUriResponse *tr2__GetStreamUriResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__StartMulticastStreaming(struct soap *soap,
	struct tr2__StartStopMulticastStreaming *tr2__StartMulticastStreaming,
	struct tr2__SetConfigurationResponse
	*tr2__StartMulticastStreamingResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__StopMulticastStreaming(struct soap *soap,
	struct tr2__StartStopMulticastStreaming *tr2__StopMulticastStreaming,
	struct tr2__SetConfigurationResponse
	*tr2__StopMulticastStreamingResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetSynchronizationPoint(struct soap *soap,
	struct _tr2__SetSynchronizationPoint *tr2__SetSynchronizationPoint,
	struct _tr2__SetSynchronizationPointResponse
	*tr2__SetSynchronizationPointResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetSnapshotUri(struct soap *soap,
	struct _tr2__GetSnapshotUri *tr2__GetSnapshotUri,
	struct _tr2__GetSnapshotUriResponse *tr2__GetSnapshotUriResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetVideoSourceModes(struct soap *soap,
	struct _tr2__GetVideoSourceModes *tr2__GetVideoSourceModes,
	struct _tr2__GetVideoSourceModesResponse
	*tr2__GetVideoSourceModesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetVideoSourceMode(struct soap *soap,
	struct _tr2__SetVideoSourceMode *tr2__SetVideoSourceMode,
	struct _tr2__SetVideoSourceModeResponse
	*tr2__SetVideoSourceModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetOSDs(struct soap *soap,
	struct _tr2__GetOSDs *tr2__GetOSDs,
	struct _tr2__GetOSDsResponse *tr2__GetOSDsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetOSDOptions(struct soap *soap,
	struct _tr2__GetOSDOptions *tr2__GetOSDOptions,
	struct _tr2__GetOSDOptionsResponse *tr2__GetOSDOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetOSD(struct soap *soap,
	struct _tr2__SetOSD *tr2__SetOSD,
	struct tr2__SetConfigurationResponse *tr2__SetOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__CreateOSD(struct soap *soap,
	struct _tr2__CreateOSD *tr2__CreateOSD,
	struct _tr2__CreateOSDResponse *tr2__CreateOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__DeleteOSD(struct soap *soap,
	struct _tr2__DeleteOSD *tr2__DeleteOSD,
	struct tr2__SetConfigurationResponse *tr2__DeleteOSDResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetMasks(struct soap *soap,
	struct _tr2__GetMasks *tr2__GetMasks,
	struct _tr2__GetMasksResponse *tr2__GetMasksResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__GetMaskOptions(struct soap *soap,
	struct _tr2__GetMaskOptions *tr2__GetMaskOptions,
	struct _tr2__GetMaskOptionsResponse *tr2__GetMaskOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__SetMask(struct soap *soap,
	struct _tr2__SetMask *tr2__SetMask,
	struct tr2__SetConfigurationResponse *tr2__SetMaskResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__CreateMask(struct soap *soap,
	struct _tr2__CreateMask *tr2__CreateMask,
	struct _tr2__CreateMaskResponse *tr2__CreateMaskResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tr2__DeleteMask(struct soap *soap,
	struct _tr2__DeleteMask *tr2__DeleteMask,
	struct tr2__SetConfigurationResponse *tr2__DeleteMaskResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tev__Unsubscribe__(struct soap *soap,
	struct _wsnt__Unsubscribe *wsnt__Unsubscribe,
	struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetStorageConfigurations(struct soap *soap,
	struct _tds__GetStorageConfigurations *tds__GetStorageConfigurations,
	struct _tds__GetStorageConfigurationsResponse
	*tds__GetStorageConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateStorageConfiguration(struct soap *soap,
	struct _tds__CreateStorageConfiguration
	*tds__CreateStorageConfiguration,
	struct _tds__CreateStorageConfigurationResponse
	*tds__CreateStorageConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetStorageConfiguration(struct soap *soap,
	struct _tds__GetStorageConfiguration *tds__GetStorageConfiguration,
	struct _tds__GetStorageConfigurationResponse
	*tds__GetStorageConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetStorageConfiguration(struct soap *soap,
	struct _tds__SetStorageConfiguration *tds__SetStorageConfiguration,
	struct _tds__SetStorageConfigurationResponse
	*tds__SetStorageConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteStorageConfiguration(struct soap *soap,
	struct _tds__DeleteStorageConfiguration
	*tds__DeleteStorageConfiguration,
	struct _tds__DeleteStorageConfigurationResponse
	*tds__DeleteStorageConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetGeoLocation(struct soap *soap,
	struct _tds__GetGeoLocation *tds__GetGeoLocation,
	struct _tds__GetGeoLocationResponse *tds__GetGeoLocationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetGeoLocation(struct soap *soap,
	struct _tds__SetGeoLocation *tds__SetGeoLocation,
	struct _tds__SetGeoLocationResponse *tds__SetGeoLocationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteGeoLocation(struct soap *soap,
	struct _tds__DeleteGeoLocation * tds__DeleteGeoLocation,
	struct _tds__DeleteGeoLocationResponse *tds__DeleteGeoLocationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingOptions(struct soap *soap,
	struct _trc__GetRecordingOptions *trc__GetRecordingOptions,
	struct _trc__GetRecordingOptionsResponse
	*trc__GetRecordingOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__ExportRecordedData(struct soap *soap,
	struct _trc__ExportRecordedData *trc__ExportRecordedData,
	struct _trc__ExportRecordedDataResponse
	*trc__ExportRecordedDataResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__StopExportRecordedData(struct soap *soap,
	struct _trc__StopExportRecordedData *trc__StopExportRecordedData,
	struct _trc__StopExportRecordedDataResponse
	*trc__StopExportRecordedDataResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetExportRecordedDataState(struct soap *soap,
	struct _trc__GetExportRecordedDataState
	*trc__GetExportRecordedDataState,
	struct _trc__GetExportRecordedDataStateResponse
	*trc__GetExportRecordedDataStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tanae__GetAnalyticsModuleOptions(struct soap *soap,
	struct _tan__GetAnalyticsModuleOptions *tan__GetAnalyticsModuleOptions,
	struct _tan__GetAnalyticsModuleOptionsResponse
	*tan__GetAnalyticsModuleOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tanre__GetRuleOptions(struct soap *soap,
	struct _tan__GetRuleOptions *tan__GetRuleOptions,
	struct _tan__GetRuleOptionsResponse *tan__GetRuleOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tetpps__Seek(struct soap *soap,
	struct _tev__Seek *tev__Seek,
	struct _tev__SeekResponse *tev__SeekResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tetpps__Unsubscribe(struct soap *soap,
	struct _wsnt__Unsubscribe *wsnt__Unsubscribe,
	struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetServiceCapabilities(struct soap *soap,
	struct _tad__GetServiceCapabilities *tad__GetServiceCapabilities,
	struct _tad__GetServiceCapabilitiesResponse
	*tad__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__DeleteAnalyticsEngineControl(
	struct soap *soap,
	struct _tad__DeleteAnalyticsEngineControl
	*tad__DeleteAnalyticsEngineControl,
	struct _tad__DeleteAnalyticsEngineControlResponse
	*tad__DeleteAnalyticsEngineControlResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__CreateAnalyticsEngineControl(struct soap *soap,
	struct _tad__CreateAnalyticsEngineControl
	*tad__CreateAnalyticsEngineControl,
	struct _tad__CreateAnalyticsEngineControlResponse
	*tad__CreateAnalyticsEngineControlResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__SetAnalyticsEngineControl(struct soap *soap,
	struct _tad__SetAnalyticsEngineControl *tad__SetAnalyticsEngineControl,
	struct _tad__SetAnalyticsEngineControlResponse
	*tad__SetAnalyticsEngineControlResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineControl(struct soap *soap,
	struct _tad__GetAnalyticsEngineControl *tad__GetAnalyticsEngineControl,
	struct _tad__GetAnalyticsEngineControlResponse
	*tad__GetAnalyticsEngineControlResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineControls(struct soap *soap,
	struct _tad__GetAnalyticsEngineControls
	*tad__GetAnalyticsEngineControls,
	struct _tad__GetAnalyticsEngineControlsResponse
	*tad__GetAnalyticsEngineControlsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngine(struct soap *soap,
	struct _tad__GetAnalyticsEngine *tad__GetAnalyticsEngine,
	struct _tad__GetAnalyticsEngineResponse
	*tad__GetAnalyticsEngineResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngines(struct soap *soap,
	struct _tad__GetAnalyticsEngines *tad__GetAnalyticsEngines,
	struct _tad__GetAnalyticsEnginesResponse
	*tad__GetAnalyticsEnginesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__SetVideoAnalyticsConfiguration(
	struct soap *soap,
	struct _tad__SetVideoAnalyticsConfiguration
	*tad__SetVideoAnalyticsConfiguration,
	struct _tad__SetVideoAnalyticsConfigurationResponse
	*tad__SetVideoAnalyticsConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__SetAnalyticsEngineInput(struct soap *soap,
	struct _tad__SetAnalyticsEngineInput *tad__SetAnalyticsEngineInput,
	struct _tad__SetAnalyticsEngineInputResponse
	*tad__SetAnalyticsEngineInputResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineInput(struct soap *soap,
	struct _tad__GetAnalyticsEngineInput *tad__GetAnalyticsEngineInput,
	struct _tad__GetAnalyticsEngineInputResponse
	*tad__GetAnalyticsEngineInputResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineInputs(struct soap *soap,
	struct _tad__GetAnalyticsEngineInputs *tad__GetAnalyticsEngineInputs,
	struct _tad__GetAnalyticsEngineInputsResponse
	*tad__GetAnalyticsEngineInputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsDeviceStreamUri(struct soap *soap,
	struct _tad__GetAnalyticsDeviceStreamUri
	*tad__GetAnalyticsDeviceStreamUri,
	struct _tad__GetAnalyticsDeviceStreamUriResponse
	*tad__GetAnalyticsDeviceStreamUriResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetVideoAnalyticsConfiguration(
	struct soap *soap,
	struct _tad__GetVideoAnalyticsConfiguration
	*tad__GetVideoAnalyticsConfiguration,
	struct _tad__GetVideoAnalyticsConfigurationResponse
	*tad__GetVideoAnalyticsConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__CreateAnalyticsEngineInputs(struct soap *soap,
	struct _tad__CreateAnalyticsEngineInputs
	*tad__CreateAnalyticsEngineInputs,
	struct _tad__CreateAnalyticsEngineInputsResponse
	*tad__CreateAnalyticsEngineInputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__DeleteAnalyticsEngineInputs(struct soap *soap,
	struct _tad__DeleteAnalyticsEngineInputs
	*tad__DeleteAnalyticsEngineInputs,
	struct _tad__DeleteAnalyticsEngineInputsResponse
	*tad__DeleteAnalyticsEngineInputsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsState(struct soap *soap,
	struct _tad__GetAnalyticsState *tad__GetAnalyticsState,
	struct _tad__GetAnalyticsStateResponse *tad__GetAnalyticsStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetPresets(struct soap *soap,
	struct _timg__GetPresets *timg__GetPresets,
	struct _timg__GetPresetsResponse *timg__GetPresetsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetCurrentPreset(struct soap *soap,
	struct _timg__GetCurrentPreset *timg__GetCurrentPreset,
	struct _timg__GetCurrentPresetResponse *timg__GetCurrentPresetResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __timg__SetCurrentPreset(struct soap *soap,
	struct _timg__SetCurrentPreset *timg__SetCurrentPreset,
	struct _timg__SetCurrentPresetResponse *timg__SetCurrentPresetResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetServiceCapabilities(struct soap *soap,
	struct _tse__GetServiceCapabilities *tse__GetServiceCapabilities,
	struct _tse__GetServiceCapabilitiesResponse
	*tse__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingSummary(struct soap *soap,
	struct _tse__GetRecordingSummary *tse__GetRecordingSummary,
	struct _tse__GetRecordingSummaryResponse
	*tse__GetRecordingSummaryResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingInformation(struct soap *soap,
	struct _tse__GetRecordingInformation *tse__GetRecordingInformation,
	struct _tse__GetRecordingInformationResponse
	*tse__GetRecordingInformationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetMediaAttributes(struct soap *soap,
	struct _tse__GetMediaAttributes *tse__GetMediaAttributes,
	struct _tse__GetMediaAttributesResponse *tse__GetMediaAttributesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindRecordings(struct soap *soap,
	struct _tse__FindRecordings *tse__FindRecordings,
	struct _tse__FindRecordingsResponse *tse__FindRecordingsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingSearchResults(struct soap *soap,
	struct _tse__GetRecordingSearchResults *tse__GetRecordingSearchResults,
	struct _tse__GetRecordingSearchResultsResponse
	*tse__GetRecordingSearchResultsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindEvents(struct soap *soap,
	struct _tse__FindEvents *tse__FindEvents,
	struct _tse__FindEventsResponse *tse__FindEventsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetEventSearchResults(struct soap *soap,
	struct _tse__GetEventSearchResults *tse__GetEventSearchResults,
	struct _tse__GetEventSearchResultsResponse
	*tse__GetEventSearchResultsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindPTZPosition(struct soap *soap,
	struct _tse__FindPTZPosition *tse__FindPTZPosition,
	struct _tse__FindPTZPositionResponse *tse__FindPTZPositionResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetPTZPositionSearchResults(struct soap *soap,
	struct _tse__GetPTZPositionSearchResults *tse__GetPTZPositionSearchResults,
	struct _tse__GetPTZPositionSearchResultsResponse
	*tse__GetPTZPositionSearchResultsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetSearchState(struct soap *soap,
	struct _tse__GetSearchState *tse__GetSearchState,
	struct _tse__GetSearchStateResponse *tse__GetSearchStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__EndSearch(struct soap *soap,
	struct _tse__EndSearch *tse__EndSearch,
	struct _tse__EndSearchResponse *tse__EndSearchResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindMetadata(struct soap *soap,
	struct _tse__FindMetadata *tse__FindMetadata,
	struct _tse__FindMetadataResponse *tse__FindMetadataResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetMetadataSearchResults(struct soap *soap,
	struct _tse__GetMetadataSearchResults *tse__GetMetadataSearchResults,
	struct _tse__GetMetadataSearchResultsResponse
	*tse__GetMetadataSearchResultsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetServiceCapabilities(struct soap *soap,
	struct _trv__GetServiceCapabilities *trv__GetServiceCapabilities,
	struct _trv__GetServiceCapabilitiesResponse
	*trv__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceivers(struct soap *soap,
	struct _trv__GetReceivers *trv__GetReceivers,
	struct _trv__GetReceiversResponse *trv__GetReceiversResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceiver(struct soap *soap,
	struct _trv__GetReceiver *trv__GetReceiver,
	struct _trv__GetReceiverResponse *trv__GetReceiverResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__CreateReceiver(struct soap *soap,
	struct _trv__CreateReceiver *trv__CreateReceiver,
	struct _trv__CreateReceiverResponse *trv__CreateReceiverResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__DeleteReceiver(struct soap *soap,
	struct _trv__DeleteReceiver *trv__DeleteReceiver,
	struct _trv__DeleteReceiverResponse *trv__DeleteReceiverResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__ConfigureReceiver(struct soap *soap,
	struct _trv__ConfigureReceiver *trv__ConfigureReceiver,
	struct _trv__ConfigureReceiverResponse
	*trv__ConfigureReceiverResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__SetReceiverMode(struct soap *soap,
	struct _trv__SetReceiverMode *trv__SetReceiverMode,
	struct _trv__SetReceiverModeResponse *trv__SetReceiverModeResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceiverState(struct soap *soap,
	struct _trv__GetReceiverState *trv__GetReceiverState,
	struct _trv__GetReceiverStateResponse
	*trv__GetReceiverStateResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetServiceCapabilities(struct soap *soap,
	struct _trp__GetServiceCapabilities *trp__GetServiceCapabilities,
	struct _trp__GetServiceCapabilitiesResponse
	*trp__GetServiceCapabilitiesResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetReplayUri(struct soap *soap,
	struct _trp__GetReplayUri *trp__GetReplayUri,
	struct _trp__GetReplayUriResponse *trp__GetReplayUriResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetReplayConfiguration(struct soap *soap,
	struct _trp__GetReplayConfiguration *trp__GetReplayConfiguration,
	struct _trp__GetReplayConfigurationResponse
	*trp__GetReplayConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__SetReplayConfiguration(struct soap *soap,
	struct _trp__SetReplayConfiguration *trp__SetReplayConfiguration,
	struct _trp__SetReplayConfigurationResponse
	*trp__SetReplayConfigurationResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetRuleOptions(struct soap *soap,
	struct _tan__GetRuleOptions *tan__GetRuleOptions,
	struct _tan__GetRuleOptionsResponse *tan__GetRuleOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetAnalyticsModuleOptions(struct soap *soap,
	struct _tan__GetAnalyticsModuleOptions *tan__GetAnalyticsModuleOptions,
	struct _tan__GetAnalyticsModuleOptionsResponse
	*tan__GetAnalyticsModuleOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GeoMove(struct soap *soap,
	struct _tptz__GeoMove *tptz__GeoMove,
	struct _tptz__GeoMoveResponse *tptz__GeoMoveResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetRelayOutputOptions(struct soap *soap,
	struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions,
	struct _tmd__GetRelayOutputOptionsResponse
	*tmd__GetRelayOutputOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetDigitalInputConfigurationOptions(
	struct soap *soap,
	struct _tmd__GetDigitalInputConfigurationOptions
	*tmd__GetDigitalInputConfigurationOptions,
	struct _tmd__GetDigitalInputConfigurationOptionsResponse
	*tmd__GetDigitalInputConfigurationOptionsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetDigitalInputConfigurations(struct soap *soap,
	struct _tmd__SetDigitalInputConfigurations *tmd__SetDigitalInputConfigurations,
	struct _tmd__SetDigitalInputConfigurationsResponse
	*tmd__SetDigitalInputConfigurationsResponse)
{
	__FUN_BEGIN("\n");
	CHECK_USER;
	__FUN_END("\n");
	return 0;
}

