#include "stdsoap2.h"
#include "soapStub.h"
#include "soapH.h"

#include "onvif_param.h"
#include "onvif.h"
#include "onvif_server.h"
#include "onvif_subscribe.h"
#include "onvif_proc.h"

#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>

static HI_ALARM_INFO_S pAlarmInfo_onvif;
static int ALAM_FLAG;
pthread_mutex_t alarm_mutex;

tev_SubscriptionManager_t g_tev_SubscriptionManager;
int PullPoint_TimeOut;
int g_tev_TopicNamespaceLocationNum;
char** g_tev_TopicNamespaceLocation;

int g_tev_TopicNum;
tev_topic_t* g_tev_TopicSet;

int g_tev_TopicExpressionDialectNum;
char** g_tev_TopicExpressionDialect;

int g_tev_MessageContentFilterDialectNum;
char** g_tev_MessageContentFilterDialect;

int g_tev_MessageContentSchemaLocationNum;
char** g_tev_MessageContentSchemaLocation;

int __ns11__Parse_Duration_Type(char *InitialTerminationTime)
{
	char* buf = InitialTerminationTime;
	char time[128] = {0};
	char buffer[128] = {0};
	char* p1, *p2, *p3, *p4;
	int e = 0;
	long Time = 0;
	
	p3 = NULL;
	p4 = NULL;
	p3 = strchr(buf, 'P');
	p4 = strchr(buf, 'T');

	if (p3 != NULL && p4 != NULL) {
		if (sscanf(p4, "%s", time) == 0)
			return 0;
	}
	else
		return 0;

	p1 = NULL;
	p2 = NULL;
	memset(buffer, 0, 128);
	p1 = strchr(time, 'T');
	if (p1 != NULL) {
		p2 = strchr(time, 'S');
		if (p2 != NULL) {
			if (sscanf(p1, "%*[T]%[0-9]", buffer) != 0) {
				if (buffer != NULL)
					Time  = (e = atoi(buffer));
				else
					return 0;
			}
		}
	}
	else
		return 0;

	printf("All seconds = %d\r\n", Time);
	return Time;
}

int __ns11__Parse_DateTime_Type(char *InitialTerminationTime)
{
	char* buf = InitialTerminationTime;
	char date[128] = {0};
	char time[128] = {0};
	char buffer[128] = {0};
	char* p1, *p2;
	int e = 0;
	int Time;
	struct tm t;

	p2 = NULL;
	p2 = strchr(buf, 'T');

	if (p2 != NULL) {
		if (sscanf(buf, "%[^T]", date) == 0)
			return 0;

		if (sscanf(p2, "%*[T]%s", time) == 0)
			return 0;
	}

	if (date != NULL) {
		if (sscanf(date, "%[0-9]", buffer) != 0) {
			if (buffer != NULL)
				t.tm_year  = ((e = atoi(buffer)) - 1900);
			else
				return 0;

			if (e < 1902 || e > 2037)
				return 0;
		}
		else
			return 0;

		p1 = NULL;
		p1 = strchr(date, '-');

		if (sscanf(p1, "%*[-]%[0-9]", buffer) != 0) {
			if (buffer != NULL)
				t.tm_mon = (e = atoi(buffer));
			else
				return 0;

			if (e < 0 || e > 11)
				return 0;
		}
		else
			return 0;

		p1 = strchr(p1 + 1, '-');
		if (sscanf(p1, "%*[-]%[0-9]", buffer) != 0)
		{
			if (buffer != NULL)
				t.tm_mday = (e = atoi(buffer));
			else
				return 0;

			if (e < 1 || e > 31)
				return 0;
		}
		else
			return 0;
	}
	else
		return 0;

	if (time != NULL) {
		if (sscanf(time, "%[0-9]", buffer) != 0) {
			if (buffer != NULL)
				t.tm_hour  = (e = atoi(buffer));
			else
				return 0;

			if (e < 0 || e > 23)
				return 0;
		}
		else
			return 0;

		p1 = NULL;
		p1 = strchr(time, ':');
		if (sscanf(p1, "%*[:]%[0-9]", buffer) != 0) {
			if (buffer != NULL)
				t.tm_min = (e = atoi(buffer));
			else
				return 0;

			if (e < 0 || e > 59)
				return 0;
		}
		else
			return 0;

		p1 = strchr(p1 + 1, ':');
		if (sscanf(p1, "%*[:]%[0-9]", buffer) != 0) {
			if (buffer != NULL)
				t.tm_sec = (e = atoi(buffer));
			else
				return 0;

			if (e < 0 || e > 59)
				return 0;
		}
		else
			return 0;
	} else
		return 0;

	if ((Time = mktime(&t)) == -1)
		return 0;

	return Time;
}

int __ns11__Check_TopicFilter(struct soap *soap,
		struct wsnt__FilterType *Filter, char *Topic)
{
	if (Filter == NULL)
		return -1;

	printf("Filter->__size = %d\r\n", Filter->__size);
	sprintf(Topic, "RuleEngine/CellMotionDetector/Motion");

	return SOAP_OK;
}

int tev_RenewSubscriptionFromManager(
		tev_SubscriptionManager_t *SubscriptionManager,
		int id, int nTerminateTime)
{
	if (SubscriptionManager == NULL || id <= 0)
		return -1;

	if (SubscriptionManager->state == 0)
		return -1;

	pthread_mutex_lock(&SubscriptionManager->mutex);
	tev_Subscription_t *Subscription = SubscriptionManager->head;

	while (Subscription) {
		if (Subscription->id == id)
			break;

		Subscription = Subscription->next;
	}

	if (Subscription == NULL) {
		pthread_mutex_unlock(&SubscriptionManager->mutex);
		return -1;
	}

	Subscription->tick = nTerminateTime;
	pthread_mutex_unlock(&SubscriptionManager->mutex);

	return 0;
}

void tev_DelSubsription(tev_Subscription_t *Subscription)
{
	if (Subscription != NULL)
		free(Subscription);
}

int tev_DeleteSubscriptionFromManager(
		tev_SubscriptionManager_t *SubscriptionManager, int id)
{
	if (SubscriptionManager == NULL || id <= 0)
		return -1;

	if (SubscriptionManager->state == 0)
		return -1;

	pthread_mutex_lock(&SubscriptionManager->mutex);

	if (SubscriptionManager->head == NULL) {
		pthread_mutex_unlock(&SubscriptionManager->mutex);
		return -1;
	}

	printf("DeleteSubscription id %d\r\n", id);
	tev_Subscription_t *Subscription = SubscriptionManager->head;
	tev_Subscription_t *prec = NULL;

	if (SubscriptionManager->head->id == id)
		SubscriptionManager->head = SubscriptionManager->head->next;
	else {
		while (1) {
			prec = Subscription;
			Subscription = Subscription->next;

			if (Subscription == NULL || Subscription->id == id)
				break;
		}

		if (Subscription == NULL) {
			pthread_mutex_unlock(&SubscriptionManager->mutex);
			return -1;
		}

		prec->next = Subscription->next;
	}

	tev_DelSubsription(Subscription);
	SubscriptionManager->num--;
	pthread_mutex_unlock(&SubscriptionManager->mutex);

	return 0;
}

tev_Subscription_t *tev_GetSubscriptionFromManager(
		tev_SubscriptionManager_t *SubscriptionManager, int id)
{
	tev_Subscription_t* pSubscription = NULL;

	if (SubscriptionManager == NULL || id <= 0)
		return NULL;

	if (SubscriptionManager->state == 0)
		return NULL;

	if (SubscriptionManager->head == NULL)
		return NULL;

	pthread_mutex_lock(&SubscriptionManager->mutex);
	pSubscription = SubscriptionManager->head;

	while (1) {
		if (pSubscription == NULL || pSubscription->id == id)
			break;

		pSubscription = pSubscription->next;
	}

	pthread_mutex_unlock(&SubscriptionManager->mutex);
	return pSubscription;
}

int tev_NotifySubscription(tev_Subscription_t *Subscription)
{
	if (Subscription == NULL)
		return -1;

	struct soap soap;
	struct _wsnt__Notify  Notify;
	time_t	 b_time;
	struct	 tm*   tim;
	char acIsMotion[32];
	soap_init(&soap);
	soap.recv_timeout = 1;
	soap_default__wsnt__Notify(&soap, &Notify);

	if (pAlarmInfo_onvif.u32AlarmType == HI_MD_ALARM_HAPPEN ||
		pAlarmInfo_onvif.u32AlarmType == HI_DI_ALARM_HAPPEN)
	{
		strcpy(acIsMotion, "true");
	}
	else if (pAlarmInfo_onvif.u32AlarmType == HI_MD_ALARM_DISAPP ||
		pAlarmInfo_onvif.u32AlarmType == HI_DI_ALARM_DISAPP)
	{
		strcpy(acIsMotion, "false");
	}

	Notify.__sizeNotificationMessage = 1;
	Notify.NotificationMessage =
		(struct wsnt__NotificationMessageHolderType *)soap_malloc(&soap,
		1 * sizeof(struct wsnt__NotificationMessageHolderType));
	int i = 0, j = 123;;

	for (i = 0; i < Notify.__sizeNotificationMessage; i++) {
		soap_default_wsnt__NotificationMessageHolderType( &soap,
			Notify.NotificationMessage + i);
		Notify.NotificationMessage[i].Topic =
			(struct wsnt__TopicExpressionType*)soap_malloc(&soap,
			sizeof(struct wsnt__TopicExpressionType));
		soap_default_wsnt__TopicExpressionType(&soap,
			Notify.NotificationMessage[i].Topic);
		Notify.NotificationMessage[i].SubscriptionReference = (struct
			wsa5__EndpointReferenceType*)soap_malloc(&soap,
			sizeof(struct wsa5__EndpointReferenceType));
		Notify.NotificationMessage[i].SubscriptionReference->Address =
			soap_malloc(&soap, 128);
		strcpy(Notify.NotificationMessage[i].
			SubscriptionReference->Address, Subscription->addr);
		Notify.NotificationMessage[i].
			SubscriptionReference->Metadata = NULL;
		Notify.NotificationMessage[i].
			SubscriptionReference->ReferenceParameters = NULL;
		Notify.NotificationMessage[i].
			SubscriptionReference->__size = 0;
		Notify.NotificationMessage[i].
			SubscriptionReference->__any = NULL;
		Notify.NotificationMessage[i].
			SubscriptionReference->__anyAttribute = NULL;
		Notify.NotificationMessage[i].ProducerReference = (struct
			wsa5__EndpointReferenceType*)soap_malloc(&soap,
			sizeof(struct wsa5__EndpointReferenceType));
		Notify.NotificationMessage[i].ProducerReference->Address
			= soap_malloc(&soap, 128);
		sprintf(Notify.NotificationMessage[i].
			ProducerReference->Address,
			"http://%s:%d/onvif/device_service/%d",
			onvif_get_ip_addr(),
			onvif_get_server_port(),
			Subscription->id);
		Notify.NotificationMessage[i].
			ProducerReference->Metadata = NULL;
		Notify.NotificationMessage[i].
			ProducerReference->ReferenceParameters = NULL;
		Notify.NotificationMessage[i].
			ProducerReference->__any = NULL;
		Notify.NotificationMessage[i].
			ProducerReference->__anyAttribute = NULL;
		Notify.NotificationMessage[i].
			ProducerReference->__size = 0;
		Notify.NotificationMessage[i].
			Topic->Dialect = (char*)soap_malloc(&soap,
			sizeof(char) * ENDPOINT_ANY_LEN);
		memset(Notify.NotificationMessage[i].Topic->Dialect,
			0, ENDPOINT_ANY_LEN);
		Notify.NotificationMessage[i].Topic->Dialect =
		"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";
		Notify.NotificationMessage[i].Topic->__any =
			(char*)soap_malloc(&soap,
			sizeof(char) * ENDPOINT_ANY_LEN);
		memset(Notify.NotificationMessage[i].Topic->__any,
			0, ENDPOINT_ANY_LEN);
#if 0
		Notify.NotificationMessage[i].Topic->__any = pTopic->topic;
#else
		Notify.NotificationMessage[i].Topic->__any =
			soap_malloc(&soap, 64);
		strcpy(Notify.NotificationMessage[i].Topic->__any,
			"tns1:RuleEngine/CellMotionDetector/Motion");
#endif
		Notify.NotificationMessage[i].Message.__any =
			(char*)soap_malloc(&soap,
			sizeof(char) * ENDPOINT_ANY_LEN);
		memset(Notify.NotificationMessage[i].Message.__any,
			0, ENDPOINT_ANY_LEN);
		b_time = time(NULL);
		tim = localtime(&b_time);

		if (pAlarmInfo_onvif.u32AlarmType == HI_MD_ALARM_HAPPEN ||
			pAlarmInfo_onvif.u32AlarmType == HI_MD_ALARM_DISAPP) {
			snprintf(Notify.NotificationMessage[i].Message.__any,
				ENDPOINT_ANY_LEN - 1,
				"<tt:Message UtcTime="
				"\"%d-%02d-%02dT%02d:%02d:%02d.%d\" "
				"PropertyOperation=\"Changed\">"
				"<tt:Source>"
				"<tt:SimpleItem Name="
				"\"VideoSourceConfigurationToken\" "
				"Value=\"VideoSourceConfigToken\"/>"
				"<tt:SimpleItem Name="
				"\"VideoAnalyticsConfigurationToken\" "
				"Value=\"VideoAnalyticsConfigToken\"/>"
				"<tt:SimpleItem Name=\"Rule\" "
				"Value=\"MyMotionDetectorRule\"/>"
				"</tt:Source>"
				"<tt:Data>"
				"<tt:SimpleItem Name=\"IsMotion\" "
				"Value=\"%s\"/>"
				"</tt:Data>"
				"</tt:Message>",
				(tim->tm_year + 1900), (tim->tm_mon + 1),
				tim->tm_mday, tim->tm_hour,
				tim->tm_min, tim->tm_sec, j, acIsMotion);
		}

		if (pAlarmInfo_onvif.u32AlarmType == HI_DI_ALARM_HAPPEN ||
			pAlarmInfo_onvif.u32AlarmType == HI_DI_ALARM_DISAPP) {
			snprintf(Notify.NotificationMessage[i].Message.__any,
				ENDPOINT_ANY_LEN - 1,
				"<tt:Message UtcTime="
				"\"%d-%02d-%02dT%02d:%02d:%02d.%d\" "
				"PropertyOperation=\"Changed\">"
				"<tt:Source>"
				"<tt:SimpleItem Name=\"InputToken\" "
				"Value=\"IOInputToken_0\"/>"
				"<tt:SimpleItemDescription Name="
				"\"IOInputToken\" Type=\"tt:ReferenceToken\"/>"
				"</tt:Source>"
				"<tt:Data>"
				"<tt:SimpleItem Name=\"LogicalState\" "
				"Value=\"%s\"/>"
				"</tt:Data>"
				"</tt:Message>",
				(tim->tm_year + 1900), (tim->tm_mon + 1),
				tim->tm_mday, tim->tm_hour,
				tim->tm_min, tim->tm_sec, j, acIsMotion);
		}
		j = j + 100;
	}

	struct SOAP_ENV__Header header;
	struct tm* tm_t;
	time_t time_n;
	/*setting default values for header*/
	soap_default_SOAP_ENV__Header(&soap, &header);
	soap.header = &header;
	header.wsa5__To = Subscription->addr;
	header.wsa5__Action =
		"http://docs.oasis-open.org/wsn/bw-2/NotificationConsumer/Notify";
	header.wsa5__MessageID = (char*)soap_malloc(&soap, sizeof(char) * 1024);

	if (NULL == header.wsa5__MessageID)
		return -1;

	time_n = time(NULL);
	tm_t = localtime(&time_n);
	sprintf(header.wsa5__MessageID,
		"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
		tm_t->tm_wday, tm_t->tm_mday, tm_t->tm_mon,
		tm_t->tm_year, tm_t->tm_hour,
		tm_t->tm_min, tm_t->tm_sec,
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55);

	if (soap_send___tev__Notify(&soap, Subscription->addr, NULL, &Notify))
		soap_print_fault(&soap, stderr);

	usleep(500000);
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	return 0;
}

static int tev_RunSubscriptionManager(void *param)
{
	if (param == NULL)
		return -1;

	int j = 0;
	tev_SubscriptionManager_t *SubscriptionManager =
		(tev_SubscriptionManager_t *)param;
	tev_Subscription_t *Subscription = NULL;
	tev_Subscription_t *pNext = NULL;
	pthread_detach(pthread_self());

	while (SubscriptionManager->state) {
		pthread_mutex_lock(&SubscriptionManager->mutex);

		if (ALAM_FLAG == 0) {
			//No Alarm information
			Subscription = SubscriptionManager->head;

			while (Subscription) {
				pNext = Subscription->next;
				Subscription->tick --;

				if (Subscription->tick <= 0) {
					j = Subscription->id;
					Subscription = Subscription->next;
					pthread_mutex_unlock(
						&SubscriptionManager->mutex);
					tev_DeleteSubscriptionFromManager(
						SubscriptionManager, j);
					pthread_mutex_lock(
						&SubscriptionManager->mutex);
				}

				Subscription = pNext;
			}

			pthread_mutex_unlock(&SubscriptionManager->mutex);
			sleep(1);
			continue;
		}

		Subscription = SubscriptionManager->head;

		while (Subscription) {
			pNext = Subscription->next;

			if (Subscription->id > 0 && Subscription->flag)
				tev_NotifySubscription(Subscription);

			Subscription->tick --;

			if (Subscription->tick <= 0) {
				j = Subscription->id;
				Subscription = Subscription->next;
				pthread_mutex_unlock(
					&SubscriptionManager->mutex);
				tev_DeleteSubscriptionFromManager(
					SubscriptionManager, j);
				pthread_mutex_lock(
					&SubscriptionManager->mutex);
			}

			Subscription = pNext;
		}

		ALAM_FLAG = 0;
		pthread_mutex_unlock(&SubscriptionManager->mutex);
		sleep(1);
	}

	while (SubscriptionManager->head) {
		Subscription = SubscriptionManager->head;
		SubscriptionManager->head = SubscriptionManager->head->next;
		tev_DelSubsription(Subscription);
	}

	return 0;
}

static int tev_StartSubscriptionManager(
		tev_SubscriptionManager_t *SubscriptionManager)
{
	if (SubscriptionManager != NULL) {
		SubscriptionManager->state = 1;

		if (pthread_create(&SubscriptionManager->pid, NULL,
				(void *)tev_RunSubscriptionManager,
				(void *)SubscriptionManager) < 0) {
			perror("pthread_create tev_RunSubscriptionManager");
			return -1;
		}
	}

	return 0;
}

tev_Subscription_t *tev_NewSubscription(const char *addr, int nTimeout)
{
	static int id = 1;
	tev_Subscription_t *Subscription =
		(tev_Subscription_t *)malloc(sizeof(tev_Subscription_t));
	Subscription->id = id++;
	Subscription->tick = nTimeout;

	if (addr) {
		strncpy(Subscription->addr, addr, DEV_URI_LEN - 1);
		Subscription->flag = 1;
	} else {
		memset(Subscription->addr, 0, DEV_URI_LEN);
		Subscription->flag = 0;
	}

	return Subscription;
}

tev_Subscription_t *tev_FindSubscription(
		tev_SubscriptionManager_t *SubscriptionManager,
		const char *addr)
{
	tev_Subscription_t *pSubscription = NULL;

	if (SubscriptionManager == NULL || addr == 0)
		return NULL;

	if (SubscriptionManager->state == 0)
		return NULL;

	if (SubscriptionManager->head == NULL)
		return NULL;

	pthread_mutex_lock(&SubscriptionManager->mutex);
	pSubscription = SubscriptionManager->head;

	while (1) {
		if (pSubscription == NULL 
			|| (strcmp(pSubscription->addr, addr) == 0))
			break;
		
		pSubscription = pSubscription->next;
	}

	pthread_mutex_unlock(&SubscriptionManager->mutex);
	return pSubscription;
}

int tev_InsertSubscriptionToManager(
		tev_SubscriptionManager_t *SubscriptionManager,
		const char *addr, int nTimeout)
{
	int ret = 0;
	tev_Subscription_t* Subscription = NULL;

	if (SubscriptionManager == NULL)
		return -1;

	if (SubscriptionManager->state == 0)
		return -1;

	Subscription = tev_FindSubscription(
		&g_tev_SubscriptionManager, addr);

	if (Subscription != NULL)
		return Subscription->id;

	pthread_mutex_lock(&SubscriptionManager->mutex);
	Subscription = tev_NewSubscription(addr, nTimeout);
	ret = Subscription->id;
	Subscription->next = SubscriptionManager->head;
	SubscriptionManager->head = Subscription;
	SubscriptionManager->num ++;
	printf("InsertSubscription id %d addr %s, timeout %d\r\n",
		Subscription->id, addr, nTimeout);
	pthread_mutex_unlock(&SubscriptionManager->mutex);

	return ret;
}

int tev_Init()
{
	g_tev_TopicNamespaceLocation =
		(char **)malloc(TEV_TNL_SIZE * sizeof(char*));
	if (g_tev_TopicNamespaceLocation == NULL)
		printf("Failed to malloc for g_tev_TopicNamespaceLocation.\n");

	g_tev_TopicNamespaceLocation[0] = (char*)malloc(TEV_URI_LEN);
	if (g_tev_TopicNamespaceLocation[0] == NULL)
		printf("Failed to malloc for g_tev_TopicNamespaceLocation[0].\n");

	g_tev_TopicNamespaceLocationNum = 1;
	strncpy(g_tev_TopicNamespaceLocation[0],
		"http://www.onvif.org/onvif/ver10/topics/topicns.xml",
		DEV_URI_LEN - 1);
	g_tev_TopicNum = 2;
	g_tev_TopicSet =
		(tev_topic_t*)malloc(sizeof(tev_topic_t) * g_tev_TopicNum);
	if (g_tev_TopicSet == NULL)
		printf("Failed to malloc for g_tev_TopicSet.\n");

	/* tns1 */
	memset(g_tev_TopicSet, 0, sizeof(tev_topic_t)*g_tev_TopicNum);
	strncpy(g_tev_TopicSet[0].topic,
		"tt:MotionAlarm", DEV_STR_LEN - 1);
	strncpy(g_tev_TopicSet[0].source.name,
		"VideoSourceToken", DEV_STR_LEN - 1);
	strncpy(g_tev_TopicSet[0].source.type,
		"tt:ReferenceToken", DEV_STR_LEN - 1);
	strncpy(g_tev_TopicSet[0].data.name,
		"State", DEV_STR_LEN - 1);
	strncpy(g_tev_TopicSet[0].data.type,
		"xsd:boolean", DEV_STR_LEN - 1);
	strncpy(g_tev_TopicSet[0].key.name,
		"any", DEV_STR_LEN - 1);
	strncpy(g_tev_TopicSet[0].key.type,
		"any", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].topic,
		"tt:IoAlarm", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].source.name,
		"Source", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].source.type,
		"tt:ReferenceToken", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].data.name,
		"State", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].data.type,
		"xsd:boolean", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].key.name,
		"any", DEV_STR_LEN - 1);
	memcpy(g_tev_TopicSet[1].key.type,
		"any", DEV_STR_LEN - 1);
	g_tev_TopicExpressionDialect =
		(char**)malloc(TEV_TED_SIZE * sizeof(char*));
	if (g_tev_TopicExpressionDialect == NULL)
		printf("Failed to malloc for g_tev_TopicExpressionDialect.\n");
	
	g_tev_TopicExpressionDialect[0] = (char*)malloc(DEV_URI_LEN);
	if (g_tev_TopicExpressionDialect[0] == NULL)
		printf("Failed to malloc for g_tev_TopicExpressionDialect[0].\n");

	g_tev_TopicExpressionDialect[1] = (char*)malloc(DEV_URI_LEN);
	if (g_tev_TopicExpressionDialect[1] == NULL)
		printf("Failed to malloc for g_tev_TopicExpressionDialect[1].\n");

	g_tev_TopicExpressionDialectNum = 2;
	strncpy(g_tev_TopicExpressionDialect[0],
		"http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete",
		DEV_URI_LEN - 1);
	strncpy(g_tev_TopicExpressionDialect[1],
		"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet",
		DEV_URI_LEN - 1);
	g_tev_MessageContentFilterDialect =
		(char **)malloc(TEV_MCFD_SIZE * sizeof(char *));
	if (g_tev_MessageContentFilterDialect == NULL)
		printf("Failed to malloc for g_tev_MessageContentFilterDialect.\n");

	g_tev_MessageContentFilterDialect[0] = (char *)malloc(DEV_URI_LEN);
	if (g_tev_MessageContentFilterDialect[0] == NULL)
		printf("Failed to malloc for g_tev_MessageContentFilterDialect[0].\n");

	g_tev_MessageContentFilterDialectNum = 1;
	strncpy(g_tev_MessageContentFilterDialect[0],
		"http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter",
		DEV_URI_LEN - 1);
	g_tev_MessageContentSchemaLocation =
		(char **)malloc(TEV_MCSL_SIZE * sizeof(char *));
	if (g_tev_MessageContentSchemaLocation == NULL)
		printf("Failed to malloc for g_tev_MessageContentSchemaLocation.\n");

	g_tev_MessageContentSchemaLocation[0] = (char *)malloc(DEV_URI_LEN);
	if (g_tev_MessageContentSchemaLocation[0] == NULL)
		printf("Failed to malloc for g_tev_MessageContentSchemaLocation[0].\n");
	
	g_tev_MessageContentSchemaLocationNum = 1;
	strncpy(g_tev_MessageContentSchemaLocation[0],
		"http://www.onvif.org/onvif/ver10/schema/onvif.xsd",
		DEV_URI_LEN - 1);
	memset(&g_tev_SubscriptionManager, 0,
		sizeof(tev_SubscriptionManager_t));
	memset(&PullPoint_TimeOut, 0, sizeof(int));
	memset(&ALAM_FLAG, 0, sizeof(int));
	memset(&pAlarmInfo_onvif, 0, sizeof(HI_ALARM_INFO_S));
	pthread_mutex_init(&g_tev_SubscriptionManager.mutex, NULL);
	tev_StartSubscriptionManager(&g_tev_SubscriptionManager);

	return 0;
}

int hi_onvif_send_alarm(LPHI_ALARM_INFO_S pAlarmInfo)
{
	if (pAlarmInfo->u32AlarmType != HI_MD_ALARM_HAPPEN
		&& pAlarmInfo->u32AlarmType != HI_MD_ALARM_DISAPP
		&& pAlarmInfo->u32AlarmType != HI_DI_ALARM_DISAPP
		&& pAlarmInfo->u32AlarmType != HI_DI_ALARM_HAPPEN)
		return -1;

	pthread_mutex_lock(&g_tev_SubscriptionManager.mutex);
	ALAM_FLAG = 1;
	memcpy(&pAlarmInfo_onvif, pAlarmInfo, sizeof(HI_ALARM_INFO_S));
	pthread_mutex_unlock(&g_tev_SubscriptionManager.mutex);

	return 0;
}

int _onvif_getAlarm(HI_ALARM_INFO_S *Alarm)
{
	pthread_mutex_lock(&g_tev_SubscriptionManager.mutex);

	if (ALAM_FLAG == 0) {
		pthread_mutex_unlock(&g_tev_SubscriptionManager.mutex);
		return -1;
	}

	memcpy(Alarm, &pAlarmInfo_onvif, sizeof(HI_ALARM_INFO_S));
	ALAM_FLAG = 0;
	pthread_mutex_unlock(&g_tev_SubscriptionManager.mutex);

	return 0;
}

